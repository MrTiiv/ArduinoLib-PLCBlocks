// SPDX-License-Identifier: Unlicense
// Created with assistance from OpenAI ChatGPT/Codex; see NOTICE.md.

// LED-Blinker ohne delay().
// ClockMemory.Q5 hat eine Periodendauer von 1 Sekunde.

#include <PLCBlocks.h>

using namespace PLCBlocks;

#ifdef LED_BUILTIN
constexpr uint8_t LED_PIN = LED_BUILTIN;
#else
constexpr uint8_t LED_PIN = 13;  // Falls erforderlich an das Board anpassen.
#endif

ClockMemory clockMemory;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  clockMemory.process();

  // Q5 wechselt alle 500 ms seinen Zustand: 500 ms EIN, 500 ms AUS.
  digitalWrite(LED_PIN, clockMemory.Q5 ? HIGH : LOW);
}
