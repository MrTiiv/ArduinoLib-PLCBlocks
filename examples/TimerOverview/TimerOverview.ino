// SPDX-License-Identifier: Unlicense
// Created with assistance from OpenAI ChatGPT/Codex; see NOTICE.md.

// Zeigt TON, TOF und TP nebeneinander im Serial Monitor.
// Es ist keine externe Verdrahtung erforderlich.
//
// Das simulierte Eingangssignal ist 2 Sekunden EIN und 2 Sekunden AUS.
// Alle Timer haben eine Vorgabezeit von 1 Sekunde.

#include <PLCBlocks.h>

using namespace PLCBlocks;

TON onDelay(T_SEC(1));
TOF offDelay(T_SEC(1));
TP pulse(T_SEC(1));

void setup() {
  Serial.begin(115200);
  Serial.println("IN | TON.Q TON.ET | TOF.Q TOF.ET | TP.Q TP.ET");
}

void loop() {
  const uint32_t now = millis();
  const bool input = ((now / T_SEC(2)) % 2U) == 0U;

  onDelay.process(input);
  offDelay.process(input);
  pulse.process(input);

  static uint32_t lastReport = 0;
  if (now - lastReport >= T_MS(250)) {
    lastReport = now;

    Serial.print(input);
    Serial.print("  |   ");
    Serial.print(onDelay.Q);
    Serial.print("   ");
    Serial.print(static_cast<unsigned long>(onDelay.ET));
    Serial.print("   |   ");
    Serial.print(offDelay.Q);
    Serial.print("   ");
    Serial.print(static_cast<unsigned long>(offDelay.ET));
    Serial.print("   |  ");
    Serial.print(pulse.Q);
    Serial.print("  ");
    Serial.println(static_cast<unsigned long>(pulse.ET));
  }
}
