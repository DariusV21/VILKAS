# Examples

Minimal, MIT-licensed examples of the VILKAS radio protocol for the ESP32-C3 (Arduino-ESP32 core 3.x).

| Example | What it does |
|---|---|
| [`minimal_rx`](minimal_rx/) | A compatible receiver: pairs with an official VILKAS-TX, receives control, sends telemetry and has failsafe. Motor output is left to you. |
| [`minimal_tx`](minimal_tx/) | A compatible remote: finds a robot in pairing mode, drives it from two pots and prints telemetry. Works with an official VILKAS-RX. |

Each example folder has its own copy of [`bb_protocol.h`](../docs/protocol/bb_protocol.h) (Arduino only compiles files in the sketch folder). Keep the copies identical.

These examples are deliberately small. They skip the parameter channel, saved robots and channel recovery that the official firmware has. **They're not tested event firmware.** Test failsafe before every use on a real robot.
