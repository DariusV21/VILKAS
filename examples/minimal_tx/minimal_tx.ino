// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Darius Vaitiekus
//
// VILKAS minimal compatible remote (protocol v4.x)
// -------------------------------------------------
// Scans the odd Wi-Fi channels for a VILKAS-RX in pairing mode, pairs with the
// first one it hears, then sends PKT_CONTROL every 20 ms and prints telemetry.
//
// Board:  any ESP32-C3 (Arduino-ESP32 core 3.x).
// Inputs: two potentiometers / stick axes on THROTTLE_PIN and STEER_PIN,
//         weapon potentiometer on WEAPON_PIN (0..3.3 V).
// Use:    put the robot in pairing mode (hold its BIND button while powering up),
//         then power this board.
//
// Pairing is not saved in this example: every power-up pairs again.

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include "bb_protocol.h"

static const int THROTTLE_PIN = 3;
static const int STEER_PIN    = 4;
static const int WEAPON_PIN   = 0;

static const uint32_t CONTROL_MS      = 20;
static const uint32_t SCAN_DWELL_MS   = 300;   // Longer than the RX offer period (150 ms)
static const uint32_t LINK_TIMEOUT_MS = 1000;

uint8_t robot[6];
volatile bool found = false;
volatile int  foundChannel = 0;
int channel = 1;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
TelemetryPacket tlm;
volatile uint32_t lastTlmMs = 0;

void onRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  if (!info || !data) return;

  if (!found && bbAccept(data, len, PKT_OFFER, OFFER_MIN_LEN)) {
    OfferPacket o;
    bbCopy(&o, sizeof(o), data, len);
    memcpy(robot, info->src_addr, 6);
    foundChannel = channel;
    found = true;
    Serial.printf("Offer from %c%c rev %c fw %.2f\n", o.prefix[0], o.prefix[1], o.hwRev, o.fwVersion100 / 100.0);
    return;
  }

  if (found && memcmp(info->src_addr, robot, 6) == 0 &&
      bbAccept(data, len, PKT_TELEMETRY, TELEMETRY_MIN_LEN)) {
    portENTER_CRITICAL(&mux);
    bbCopy(&tlm, sizeof(tlm), data, len);
    lastTlmMs = millis();
    portEXIT_CRITICAL(&mux);
  }
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != ESP_OK) { Serial.println("ESP-NOW init failed"); while (true) delay(1000); }
  esp_now_register_recv_cb(onRecv);

  // Scan odd channels until a robot offers itself
  Serial.println("Scanning for robots...");
  while (!found) {
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    uint32_t t0 = millis();
    while (!found && millis() - t0 < SCAN_DWELL_MS) delay(5);
    if (!found) channel = (channel >= 13) ? 1 : channel + 2;
  }
  channel = foundChannel;
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  esp_now_peer_info_t p = {};
  memcpy(p.peer_addr, robot, 6);
  p.channel = 0;
  esp_now_add_peer(&p);
  Serial.printf("Pairing with %02X:%02X:%02X:%02X:%02X:%02X on ch %d\n",
                robot[0], robot[1], robot[2], robot[3], robot[4], robot[5], channel);
}

// Arcade mix: throttle/steer -1..1 -> left/right -4095..4095
static int16_t clampMot(float v) { return (int16_t)constrain(v * 4095.0f, -4095.0f, 4095.0f); }

void loop() {
  static uint32_t lastCtrl = 0, lastPrint = 0;
  uint32_t now = millis();

  if (now - lastCtrl >= CONTROL_MS) {
    lastCtrl = now;
    float thr = (analogRead(THROTTLE_PIN) - 2048) / 2048.0f;
    float str = (analogRead(STEER_PIN)    - 2048) / 2048.0f;
    if (fabsf(thr) < 0.03f) thr = 0;
    if (fabsf(str) < 0.03f) str = 0;

    ControlPacket c = {};
    bbInitHeader(c.h, PKT_CONTROL);
    c.motor_left  = clampMot(thr + str);
    c.motor_right = clampMot(thr - str);
    c.weapon      = (uint16_t)analogRead(WEAPON_PIN);   // Raw 0..4095; the RX applies its limits
    c.btn         = 0;
    c.flip        = 0;
    c.cmd_channel = 0;                                  // 0 = stay on this channel
    c.fs_mot1 = c.fs_mot2 = c.fs_weapon = 1;            // Stop everything on link loss
    c.tx_time     = (uint16_t)(now & 0xFFFF);
    c.wpn_live    = 1;                                  // 0 would force the weapon to stop
    esp_now_send(robot, (uint8_t*)&c, sizeof(c));
  }

  if (now - lastPrint >= 500) {
    lastPrint = now;
    TelemetryPacket t;
    uint32_t last;
    portENTER_CRITICAL(&mux);
    t = tlm; last = lastTlmMs;
    portEXIT_CRITICAL(&mux);
    if (last && now - last < LINK_TIMEOUT_MS) {
      uint16_t rtt = (uint16_t)(now & 0xFFFF) - t.echo_time;   // Approximate: includes up to 100 ms telemetry period
      Serial.printf("RX %c%c  %.2f V  fw %.2f  pkts %u  rtt<=%u ms%s\n",
                    t.prefix[0], t.prefix[1], t.rxVolt100 / 100.0, t.fwVersion100 / 100.0,
                    t.packetCount, rtt, (t.flags & TLM_FLAG_WPN_SAFE) ? "  [weapon disarmed]" : "");
    } else {
      Serial.println("No telemetry");
    }
  }
  delay(1);
}
