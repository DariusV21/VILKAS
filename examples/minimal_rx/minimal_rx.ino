// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Darius Vaitiekus
//
// VILKAS minimal compatible receiver (protocol v4.x)
// --------------------------------------------------
// A minimal RX that pairs with an official VILKAS-TX, receives control packets,
// sends telemetry and implements failsafe. It does NOT implement the parameter
// channel (caps = 0), so the remote shows robot settings as N/A.
//
// Board:  any ESP32-C3 (Arduino-ESP32 core 3.x). Pins below match VILKAS-RX Rev C.
// Use:    hold BIND (GPIO1 -> GND) while powering up = pairing mode, then pick
//         "XX-....." on the remote (hold BIND on the remote while powering it up).
//
// Replace applyOutputs() / stopDrive() / stopWeapon() with your own motor code.

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <Preferences.h>
#include "bb_protocol.h"

// ── Identity ──
static const char     PREFIX[2]  = { 'X', 'X' };   // Your manufacturer prefix (see manufacturer-prefixes.md)
static const char     HW_REV     = 'A';
static const uint16_t FW_VER100  = 100;           // Your firmware version x100 (display only)

// ── Pins (VILKAS-RX Rev C) ──
static const int PIN_BIND = 1;   // Active LOW
static const int PIN_LED  = 6;

// ── Timing ──
static const uint32_t FAILSAFE_DRIVE_MS  = 500;
static const uint32_t FAILSAFE_WEAPON_MS = 850;
static const uint32_t TELEMETRY_MS       = 100;
static const uint32_t OFFER_MS           = 150;
static const uint32_t PAIRING_WINDOW_MS  = 90000;

static const uint8_t BCAST[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

Preferences prefs;
uint8_t  pairedTx[6] = { 0 };
bool     paired = false;
bool     pairingMode = false;
uint32_t pairingStart = 0;
int      channel = 1;

// Shared with the ESP-NOW callback (runs in the Wi-Fi task)
portMUX_TYPE  mux = portMUX_INITIALIZER_UNLOCKED;
ControlPacket ctrl;                      // Last valid control packet
volatile uint32_t lastCtrlMs = 0;
volatile bool     newCtrl = false;
volatile uint8_t  rxCount = 0;
volatile bool     justPaired = false;

// ───────────── Outputs (replace with your hardware) ─────────────
void applyOutputs(int16_t left, int16_t right, uint16_t weapon, bool weaponLive) {
  static uint32_t lastPrint = 0;
  if (millis() - lastPrint > 200) {
    lastPrint = millis();
    Serial.printf("L=%5d R=%5d W=%4u live=%d\n", left, right, weapon, weaponLive);
  }
}
void stopDrive()  { /* set both drive motors to 0 */ }
void stopWeapon() { /* ESC: 1000 us pulse; DC: brake or coast */ }

// ───────────── Helpers ─────────────
bool isOddChannel(int ch) { return ch >= 1 && ch <= 13 && (ch & 1); }

void addPeer(const uint8_t* mac) {
  if (esp_now_is_peer_exist(mac)) return;
  esp_now_peer_info_t p = {};
  memcpy(p.peer_addr, mac, 6);
  p.channel = 0;            // 0 = current Wi-Fi channel
  p.encrypt = false;
  esp_now_add_peer(&p);
}

// ───────────── Radio receive ─────────────
void onRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  if (!info || !data) return;
  if (!bbAccept(data, len, PKT_CONTROL, CONTROL_MIN_LEN)) return;   // magic, major 4, type, min length

  bool fromPaired = paired && memcmp(info->src_addr, pairedTx, 6) == 0;
  if (!paired && pairingMode) {          // Pairing: accept the first remote that talks to us
    memcpy(pairedTx, info->src_addr, 6);
    paired = true;
    fromPaired = true;
    justPaired = true;
  }
  if (!fromPaired) return;

  portENTER_CRITICAL(&mux);
  bbCopy(&ctrl, sizeof(ctrl), data, len);  // Older/shorter packet -> missing fields = 0
  lastCtrlMs = millis();
  newCtrl = true;
  rxCount++;
  portEXIT_CRITICAL(&mux);
}

// ───────────── Send ─────────────
void sendOffer() {
  OfferPacket o = {};
  bbInitHeader(o.h, PKT_OFFER);
  memcpy(o.prefix, PREFIX, 2);
  o.hwRev = HW_REV;
  o.fwVersion100 = FW_VER100;
  o.caps = 0;                              // No parameter channel in this example
  esp_now_send(BCAST, (uint8_t*)&o, sizeof(o));
}

void sendTelemetry(uint16_t echoTime, bool weaponSafe) {
  TelemetryPacket t = {};
  bbInitHeader(t.h, PKT_TELEMETRY);
  t.rxVolt100    = 0;                      // Put your battery voltage x100 here
  t.packetCount  = rxCount;
  t.fwVersion100 = FW_VER100;
  t.echo_time    = echoTime;               // Lets the remote measure round-trip latency
  memcpy(t.prefix, PREFIX, 2);
  t.hwRev        = HW_REV;
  t.flags        = weaponSafe ? TLM_FLAG_WPN_SAFE : 0;
  t.paramRev     = 0;
  t.caps         = 0;
  esp_now_send(pairedTx, (uint8_t*)&t, sizeof(t));
}

// ───────────── Setup ─────────────
void setup() {
  pinMode(PIN_BIND, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  stopDrive();
  stopWeapon();
  Serial.begin(115200);

  delay(20);
  bool bindHeld = true;                    // BIND must stay LOW for ~150 ms
  for (int i = 0; i < 30; i++) { if (digitalRead(PIN_BIND)) { bindHeld = false; break; } delay(5); }

  prefs.begin("vk_rx", false);
  channel = prefs.getInt("ch", 1);
  if (!isOddChannel(channel)) channel = 1;
  if (bindHeld) {
    prefs.remove("tx_mac");                // Forget the old remote
    pairingMode = true;
    pairingStart = millis();
  } else if (prefs.getBytes("tx_mac", pairedTx, 6) == 6) {
    paired = true;
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  if (esp_now_init() != ESP_OK) { Serial.println("ESP-NOW init failed"); while (true) delay(1000); }
  esp_now_register_recv_cb(onRecv);
  addPeer(BCAST);
  if (paired) addPeer(pairedTx);

  Serial.printf("VILKAS minimal RX  ch=%d  %s\n", channel, pairingMode ? "PAIRING" : (paired ? "paired" : "not paired"));
}

// ───────────── Loop ─────────────
void loop() {
  uint32_t now = millis();
  static uint32_t lastOffer = 0, lastTlm = 0;
  static bool weaponArmed = false;

  // Pairing: broadcast offers until a remote picks us
  if (pairingMode && !paired) {
    if (now - lastOffer >= OFFER_MS) { lastOffer = now; sendOffer(); }
    if (now - pairingStart > PAIRING_WINDOW_MS) pairingMode = false;
  }
  if (justPaired) {
    justPaired = false;
    pairingMode = false;
    prefs.putBytes("tx_mac", pairedTx, 6);
    addPeer(pairedTx);
    Serial.println("Paired");
  }

  // Copy the latest control packet
  ControlPacket c;
  bool fresh;
  uint32_t last;
  portENTER_CRITICAL(&mux);
  c = ctrl; fresh = newCtrl; newCtrl = false; last = lastCtrlMs;
  portEXIT_CRITICAL(&mux);

  uint32_t age = (last == 0) ? UINT32_MAX : now - last;

  if (fresh) {
    // Remote asked us to move to another channel
    if (isOddChannel(c.cmd_channel) && c.cmd_channel != channel) {
      channel = c.cmd_channel;
      prefs.putInt("ch", channel);
      esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    }
    // Weapon arming (ESC mode): only after the wheel has been at ~0 with wpn_live = 1
    if (!c.wpn_live) weaponArmed = false;
    else if (!weaponArmed && c.weapon < 4095 * 5 / 100) weaponArmed = true;

    applyOutputs(c.motor_left, c.motor_right, weaponArmed ? c.weapon : 0, weaponArmed);
  }

  // Failsafe
  if (age > FAILSAFE_DRIVE_MS)  stopDrive();
  if (age > FAILSAFE_WEAPON_MS) { stopWeapon(); weaponArmed = false; }

  // Telemetry
  if (paired && now - lastTlm >= TELEMETRY_MS) {
    lastTlm = now;
    sendTelemetry(c.tx_time, !weaponArmed);
  }

  digitalWrite(PIN_LED, (age < FAILSAFE_DRIVE_MS) ? HIGH : ((now / 250) & 1));
  delay(2);
}
