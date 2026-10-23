// SPDX-License-Identifier: MIT
// VILKAS radio protocol v4 - reference header (identical in official TX and RX firmware).
// Specification: docs/protocol/VILKAS-protocol-v4.md. Comments below are in Lithuanian; the spec is in English.
// Copyright (c) 2026 Darius Vaitiekus

#ifndef BB_PROTOCOL_H
#define BB_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

// ============================================================
// BB RADIJO PROTOKOLAS v4 (ESP-NOW) - BENDRAS RX IR TX
//
// Sis failas IDENTISKAS abiejuose projektuose (TX_FW/bb_protocol.h ir RX_FW/bb_protocol.h).
// Pakeitus viena - nukopijuoti i kita.
//
// SUDERINAMUMO TAISYKLES (kad parduotu moduliu nereiketu atnaujinti kiekvienu leidimu):
//  1. Kiekvienas paketas prasideda PktHeader. Tipas nustatomas pagal header.type, NE pagal ilgi.
//  2. Paketai tik PAPILDOMI gale (append-only). Lauku negalima trinti, perkelti ar keisti ju prasmes.
//     Naujas laukas -> BB_PROTO_MINOR++ ir apibrezta numatytoji reiksme, kai paketas trumpesnis.
//  3. Gavejas priima len >= *_MIN_LEN (v4.0 dydis): skaito tai, ka zino, likusius baitus ignoruoja.
//     Trumpesniam (senesnio minor) paketui truksta lauku -> naudojamos numatytosios reiksmes
//     (paketas pries kopijuojant isvalomas nuliais - todel nauju lauku "0" turi reiksti "nera").
//  4. BB_PROTO_MAJOR keiciamas TIK jei suderinamumo islaikyti neimanoma. Skirtingas major = nesuderinama.
//  5. Roboto nustatymai - per parametru kanala (PARAM_*): naujas nustatymas = naujas ID, paketai
//     nesikeicia. Nezinomas ID -> PARAM_ST_UNKNOWN, pultas ta meniu punkta rodo kaip N/A.
//  6. Naujos galimybes skelbiamos CAP_* bitais (OfferPacket / TelemetryPacket), ne versijos numeriu.
//
// Visi skaiciai little-endian, strukturos supakuotos (be tarpu).
// ============================================================

#define BB_MAGIC        0xB0
#define BB_PROTO_MAJOR  4
#define BB_PROTO_MINOR  1   // 4.1: ControlPacket.wpn_live, PARAM_WPN_OUT, TLM_FLAG_WPN_SAFE

// Paketu tipai (PktHeader.type)
enum : uint8_t {
  PKT_OFFER       = 1,   // RX -> broadcast: nesusietas robotas siulosi (kas <=150ms)
  PKT_CONTROL     = 2,   // TX -> RX: valdymas (kas 20ms)
  PKT_TELEMETRY   = 3,   // RX -> TX: telemetrija (kas 100ms)
  PKT_PARAM_CMD   = 4,   // TX -> RX: parametro skaitymas / keitimas
  PKT_PARAM_REPLY = 5,   // RX -> TX: atsakymas i PKT_PARAM_CMD
};

struct PktHeader {
  uint8_t magic;         // BB_MAGIC
  uint8_t type;          // PKT_*
  uint8_t proto_major;   // BB_PROTO_MAJOR
  uint8_t proto_minor;   // BB_PROTO_MINOR (siuntejo)
} __attribute__((packed));

// RX galimybes (OfferPacket.caps / TelemetryPacket.caps)
#define CAP_PARAMS       0x00000001UL   // Parametru kanalas (PKT_PARAM_*)
#define CAP_WIFI_UPDATE  0x00000002UL   // Wi-Fi atnaujinimo rezimas (PARAM_ACT_UPDATE_MODE)
#define CAP_WIFI_DRIVE   0x00000004UL   // (RX 4.30) Wi-Fi rezime - valdymas telefonu (captive portal, virtualus joystick)

// TelemetryPacket.flags
#define TLM_FLAG_DRV_DEFAULT  0x01      // Rate / S-Curve / Trim robote dar niekada neissaugoti (gamykliniai) -
                                        // pultas vienkartinai nukopijuoja savo reiksmes
#define TLM_FLAG_WPN_SAFE     0x02      // (4.1) Ginklas neapginkluotas: laukiama, kol ratukas bus ties 0%
                                        // (ESC) arba centre (DC). Po ijungimo ir po ginklo failsafe.

// ── PKT_OFFER (RX -> broadcast FF:FF:FF:FF:FF:FF) ──
struct OfferPacket {
  PktHeader h;
  char     prefix[2];      // Gamintojo zyma, 2 simboliai A-Z / 0-9 (be '\0'), pvz. 'D','V'
  char     hwRev;          // Aparatines irangos revizija 'A'..'Z'
  uint16_t fwVersion100;   // RX firmware versija x100 (400 = 4.00)
  uint32_t caps;           // CAP_*
} __attribute__((packed));
#define OFFER_MIN_LEN 13

// ── PKT_CONTROL (TX -> RX, kas 20ms) ──
struct ControlPacket {
  PktHeader h;
  int16_t  motor_left;     // -4095..4095, jau sumiksuota. + = pirmyn
  int16_t  motor_right;    // -4095..4095, jau sumiksuota. + = pirmyn
  uint16_t weapon;         // 0..4095 pilna ratuko eiga (ginklo limitus taiko RX)
  uint8_t  btn;            // Joystick mygtukas: 1 = paspaustas
  uint8_t  flip;           // FLIP: 1 = aktyvus (jau pritaikytas miksavime)
  uint8_t  cmd_channel;    // 0 = nieko, 1..13 (nelyginis) = pereiti i si kanala
  uint8_t  fs_mot1;        // 1 = dingus rysiui stabdyti kairi varikli
  uint8_t  fs_mot2;        // 1 = dingus rysiui stabdyti desini varikli
  uint8_t  fs_weapon;      // 1 = dingus rysiui stabdyti ginkla
  uint16_t tx_time;        // TX millis() & 0xFFFF - RTT matavimui
  // ── 4.1 ──
  uint8_t  wpn_live;       // 1 = 'weapon' yra tikra ratuko padetis. 0 = stabdyti ginkla (pulto meniu arba
                           // senas 4.0 pultas). Reikalinga DC ginklui: ten weapon = 0 reiskia pilna REV.
} __attribute__((packed));
#define CONTROL_MIN_LEN 18

// ── PKT_TELEMETRY (RX -> TX, kas 100ms) ──
struct TelemetryPacket {
  PktHeader h;
  uint16_t rxVolt100;      // Sukalibruota, filtruota roboto baterijos itampa x100 (840 = 8.40V)
  uint8_t  packetCount;    // Priimtu PKT_CONTROL skaitiklis (perpildomas ties 255)
  uint16_t fwVersion100;   // RX firmware versija x100
  uint16_t echo_time;      // Paskutinio gauto tx_time kopija
  char     prefix[2];      // Gamintojo zyma (be '\0')
  char     hwRev;          // Aparatines irangos revizija
  uint8_t  flags;          // TLM_FLAG_*
  uint16_t paramRev;       // Keiciasi pasikeitus bet kuriam parametrui -> pultas persiskaito visus
  uint32_t caps;           // CAP_*
} __attribute__((packed));
#define TELEMETRY_MIN_LEN 21

// ── Parametru kanalas ──
enum : uint8_t {
  PARAM_OP_GET = 1,        // Perskaityti (value ignoruojama)
  PARAM_OP_SET = 2,        // Nustatyti ir issaugoti NVS
};

enum : uint8_t {
  PARAM_ST_OK       = 0,
  PARAM_ST_UNKNOWN  = 1,   // RX sio ID nezino (senesnis FW) - pultas rodo N/A
  PARAM_ST_RANGE    = 2,   // Reiksme uz ribu (ribos - atsakyme)
  PARAM_ST_BUSY     = 3,   // Siuo metu keisti negalima (pvz. robotas juda)
  PARAM_ST_READONLY = 4,
};

// Parametru ID. NIEKADA nekeisti esamu numeriu - tik prideti naujus.
enum : uint16_t {
  PARAM_MOT_DIR    = 1,    // bit0 = kairys REV, bit1 = desinys REV (keiciama tik robotui stovint)
  PARAM_BATT_TYPE  = 2,    // BATT_TYPE_LIPO / BATT_TYPE_LIHV
  PARAM_BATT_CELLS = 3,    // 2..4
  PARAM_VCAL       = 4,    // Itampos daugiklis x1000 (500..2000)
  PARAM_SPD_LMT    = 5,    // Vaziuokles greicio limitas % (0..100)
  PARAM_WPN_MIN    = 6,    // Ginklo MIN % (0..WPN_MAX)
  PARAM_WPN_MAX    = 7,    // Ginklo MAX % (WPN_MIN..100)
  PARAM_RATE       = 8,    // Max Rate % (10..100) - saugo RX, taiko TX
  PARAM_EXPO       = 9,    // S-Curve % (0..100)   - saugo RX, taiko TX
  PARAM_TRIM       = 10,   // Steer Trim (-100..100) - saugo RX, taiko TX
  PARAM_WPN_OUT    = 11,   // (4.1) Ginklo isejimas: WPN_OUT_ESC / WPN_OUT_DC. maxVal = 0, jei plokste DC neturi.
                           // Keiciama tik robotui stovint; po pakeitimo ginklas neapginkluotas.
  // DC ginklo nustatymai (4.2). Plokste be DC H-tilto atsako PARAM_ST_UNKNOWN (pultas rodo N/A).
  PARAM_DC_START   = 12,   // Pradine galia % (0..50): 1-as greicio laiptelis prasideda nuo jos (varikliai
                           // nesisuka esant mazam duty). Laipteliai isdestomi tarp START ir WPN_MAX.
  PARAM_DC_REV     = 13,   // DC ginklo kryptis: 0 = normali, 1 = apversta (tik robotui stovint)
  PARAM_DC_BRAKE   = 14,   // Sustojus: 0 = coast (abu IN LOW), 1 = brake (abu IN HIGH)
  PARAM_DC_RAMP    = 15,   // Isibegejimo laikas 0..100% ms (0..2000; 0 = be ramp). Letejimas visada staigus.
  // (RX 4.30)
  PARAM_PWM_SERVO  = 16,   // GPIO10 (ESC isejimo) signalas: 0 = ESC (stop = 1000us, ramp-up), 1 = SERVO (centras
                           // 1500us = stop, 1000..2000us be ramp; neapginkluotas / failsafe - be impulsu).
                           // Kanalo W signalas (nuo 4.30 nepriklauso nuo PARAM_WPN_OUT). Atskiras ID (ne WPN_OUT = 2), kad senas pultas,
                           // perjunginedamas Out: ESC/DC, niekada netycia neijungtu SERVO (1500us ESC'ui = ~50%).
                           // Keiciama tik robotui stovint; po pakeitimo ginklas neapginkluotas.
  PARAM_Q_MAX      = 17,   // (RX 4.30) Q kanalo (DC) MAX % (PARAM_DC_START..100). Nuo 4.30 PARAM_WPN_MIN/MAX taikomi
                           // tik W kanalui (ESC), Q turi savo: MIN = PARAM_DC_START, MAX = PARAM_Q_MAX.
  PARAM_ID_LAST    = 17,   // Paskutinis nustatymu ID (pultas sinchronizuoja 1..PARAM_ID_LAST)

  // Veiksmai (0x100+): SET su raktu atlieka veiksma, GET grazina 0
  PARAM_ACT_UPDATE_MODE = 0x100,   // SET PARAM_UPDATE_KEY -> RX persikrauna i Wi-Fi atnaujinimo rezima
};
#define PARAM_UPDATE_KEY  0x54445055L   // "UPDT"

#define BATT_TYPE_LIPO 0
#define BATT_TYPE_LIHV 1

// PARAM_WPN_OUT: kuri ginklo kanala valdo pulto ratukas (ir telefono slankiklis). Nuo RX 4.30 abu kanalai
// veikia kartu (REV:D), nevaldomas kanalas laikomas sustabdytas (arba ji valdo telefono mygtukas).
#define WPN_OUT_ESC 0      // Kanalas W: GPIO10, ESC 50Hz 1000-2000us (arba SERVO - PARAM_PWM_SERVO). Ratukas 0% = stop
#define WPN_OUT_DC  1      // Kanalas Q: DC variklis per H-tilta (REV:D+). Ratuko centras = stop, galai = pilnas FWD / REV

struct ParamCmdPacket {
  PktHeader h;
  uint8_t  op;             // PARAM_OP_*
  uint8_t  seq;            // Uzklausos numeris - grazinamas atsakyme
  uint16_t id;             // PARAM_*
  int32_t  value;          // SET reiksme
} __attribute__((packed));
#define PARAM_CMD_MIN_LEN 12

struct ParamReplyPacket {
  PktHeader h;
  uint8_t  seq;            // Is uzklausos
  uint8_t  status;         // PARAM_ST_*
  uint16_t id;
  int32_t  value;          // Dabartine (po SET - nauja) reiksme
  int32_t  minVal;         // Leistinos ribos (pultas pagal jas riboja redagavima)
  int32_t  maxVal;
} __attribute__((packed));
#define PARAM_REPLY_MIN_LEN 20

static_assert(sizeof(PktHeader) == 4, "PktHeader");
static_assert(sizeof(OfferPacket)      >= OFFER_MIN_LEN,       "OfferPacket v4.0");
static_assert(sizeof(ControlPacket)    >= CONTROL_MIN_LEN,     "ControlPacket v4.0");
static_assert(sizeof(TelemetryPacket)  >= TELEMETRY_MIN_LEN,   "TelemetryPacket v4.0");
static_assert(sizeof(ParamCmdPacket)   >= PARAM_CMD_MIN_LEN,   "ParamCmdPacket v4.0");
static_assert(sizeof(ParamReplyPacket) >= PARAM_REPLY_MIN_LEN, "ParamReplyPacket v4.0");

inline void bbInitHeader(PktHeader &h, uint8_t type) {
  h.magic = BB_MAGIC; h.type = type; h.proto_major = BB_PROTO_MAJOR; h.proto_minor = BB_PROTO_MINOR;
}

// Musu protokolo paketas (bet kurio major)?
inline bool bbIsOurs(const uint8_t* data, int len) {
  return data && len >= (int)sizeof(PktHeader) && data[0] == BB_MAGIC;
}

// Musu, suderinamo major, nurodyto tipo ir ne trumpesnis nei v4.0?
inline bool bbAccept(const uint8_t* data, int len, uint8_t type, int minLen) {
  return bbIsOurs(data, len) && len >= minLen &&
         ((const PktHeader*)data)->type == type &&
         ((const PktHeader*)data)->proto_major == BB_PROTO_MAJOR;
}

// Saugus kopijavimas: trumpesnis (senesnis) paketas - truksta lauku = 0, ilgesnis - perteklius ignoruojamas
inline void bbCopy(void* dst, size_t dstSize, const uint8_t* src, int len) {
  uint8_t* d = (uint8_t*)dst;
  size_t n = (size_t)len < dstSize ? (size_t)len : dstSize;
  for (size_t i = 0; i < dstSize; i++) d[i] = (i < n) ? src[i] : 0;
}

#endif
