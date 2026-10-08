// SPDX-License-Identifier: Unlicense
// Created with assistance from OpenAI ChatGPT/Codex; see NOTICE.md.

// Einfache Start-/Stopp-Speicherung wie in einer SPS.
//
// Verdrahtung:
//   Start-Taster: Pin 4 -- Taster -- GND
//   Stopp-Taster: Pin 5 -- Taster -- GND
//
// Beide Eingange verwenden INPUT_PULLUP. Ein gedrueckter Taster liefert LOW.

#include <PLCBlocks.h>

using namespace PLCBlocks;

constexpr uint8_t START_PIN = 4;
constexpr uint8_t STOP_PIN = 5;
#ifdef LED_BUILTIN
constexpr uint8_t LED_PIN = LED_BUILTIN;
#else
constexpr uint8_t LED_PIN = 13;  // Falls erforderlich an das Board anpassen.
#endif

SR runMemory;  // Reset-dominanter Speicher: Stopp hat Vorrang.

void setup() {
  pinMode(START_PIN, INPUT_PULLUP);
  pinMode(STOP_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  const bool startPressed = (digitalRead(START_PIN) == LOW);
  const bool stopPressed = (digitalRead(STOP_PIN) == LOW);

  runMemory.process(startPressed, stopPressed);
  digitalWrite(LED_PIN, runMemory.Q ? HIGH : LOW);
}
