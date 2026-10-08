// SPDX-License-Identifier: Unlicense
// Created with assistance from OpenAI ChatGPT/Codex; see NOTICE.md.

// Einschaltverzoegerung mit TON.
// Wird der Taster 2 Sekunden lang gehalten, schaltet die LED ein.
// Beim Loslassen werden Q und ET sofort zurueckgesetzt.
//
// Verdrahtung: Pin 4 -- Taster -- GND

#include <PLCBlocks.h>

using namespace PLCBlocks;

constexpr uint8_t BUTTON_PIN = 4;
#ifdef LED_BUILTIN
constexpr uint8_t LED_PIN = LED_BUILTIN;
#else
constexpr uint8_t LED_PIN = 13;  // Falls erforderlich an das Board anpassen.
#endif

TON onDelay(T_SEC(2));

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  const bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  onDelay.process(buttonPressed);
  digitalWrite(LED_PIN, onDelay.Q ? HIGH : LOW);

  // ET und Q viermal pro Sekunde im Serial Monitor anzeigen.
  static uint32_t lastReport = 0;
  const uint32_t now = millis();
  if (now - lastReport >= T_MS(250)) {
    lastReport = now;
    Serial.print("IN=");
    Serial.print(buttonPressed);
    Serial.print(" ET=");
    Serial.print(static_cast<unsigned long>(onDelay.ET));
    Serial.print(" ms Q=");
    Serial.println(onDelay.Q);
  }
}
