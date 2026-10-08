// SPDX-License-Identifier: Unlicense
// Created with assistance from OpenAI ChatGPT/Codex; see NOTICE.md.

// Der Taktmerker Q5 erzeugt jede Sekunde eine positive Flanke.
// CTU zaehlt diese Flanken. Nach 5 Flanken wird die LED eingeschaltet.
// Mit dem Taster an Pin 5 kann der Zaehler zurueckgesetzt werden.
//
// Verdrahtung: Pin 5 -- Reset-Taster -- GND

#include <PLCBlocks.h>

using namespace PLCBlocks;

constexpr uint8_t RESET_PIN = 5;
constexpr int32_t PRESET_COUNT = 5;
#ifdef LED_BUILTIN
constexpr uint8_t LED_PIN = LED_BUILTIN;
#else
constexpr uint8_t LED_PIN = 13;  // Falls erforderlich an das Board anpassen.
#endif

ClockMemory clockMemory;
CTU counter;

void setup() {
  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  clockMemory.process();

  const bool resetPressed = (digitalRead(RESET_PIN) == LOW);

  // CTU erkennt die positive Flanke an CU selbst.
  counter.process(clockMemory.Q5, resetPressed, PRESET_COUNT);
  digitalWrite(LED_PIN, counter.Q ? HIGH : LOW);

  // Nur dann ausgeben, wenn sich der Zaehlwert geaendert hat.
  static int32_t previousValue = -1;
  if (counter.CV != previousValue) {
    previousValue = counter.CV;
    Serial.print("CV=");
    Serial.print(static_cast<long>(counter.CV));
    Serial.print(" / PV=");
    Serial.print(static_cast<long>(PRESET_COUNT));
    Serial.print(" Q=");
    Serial.println(counter.Q);
  }
}
