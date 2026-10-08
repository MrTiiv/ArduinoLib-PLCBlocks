// SPDX-License-Identifier: Unlicense
// Created with assistance from OpenAI ChatGPT/Codex; see NOTICE.md.

#include <PLCBlocks.h>

using namespace PLCBlocks;

// Verdrahtung:
//   Start-Taster: Pin 4 -- Taster -- GND
//   Reset-Taster: Pin 5 -- Taster -- GND
// INPUT_PULLUP bedeutet: Ein gedrueckter Taster liefert LOW.
constexpr uint8_t BUTTON_PIN = 4;
constexpr uint8_t RESET_PIN = 5;
#ifdef LED_BUILTIN
constexpr uint8_t LED_PIN = LED_BUILTIN;
#else
constexpr uint8_t LED_PIN = 13;  // Falls erforderlich an das Board anpassen.
#endif

FirstScan firstScan;
CycleTime cycleTime;
ClockMemory clockMemory;
P_TRIG startEdge;
SR runLatch;
TON startDelay(T_SEC(2));
CTU starts;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // Wie in einem SPS-Zyklus wird jede Instanz einmal pro loop() aufgerufen.
  firstScan.process();
  cycleTime.process();
  clockMemory.process();

  const bool start = (digitalRead(BUTTON_PIN) == LOW);
  const bool reset = (digitalRead(RESET_PIN) == LOW);

  startEdge.process(start);
  runLatch.process(startEdge.Q, reset);   // SR: Reset hat Vorrang.
  startDelay.process(runLatch.Q);         // Nicht blockierender 2-s-TON.
  starts.process(startEdge.Q, reset, 3);  // Positive Startflanken zaehlen.

  // Nach der Verzoegerung blinkt die LED mit 1 s Periodendauer.
  digitalWrite(LED_PIN, (startDelay.Q && clockMemory.Q5) ? HIGH : LOW);

  if (firstScan.Q) {
    Serial.println("First scan");
  }

  // Einmal pro Sekunde die wichtigsten Werte ausgeben.
  static uint32_t lastReport = 0;
  const uint32_t now = millis();
  if (now - lastReport >= T_SEC(1)) {
    lastReport = now;
    Serial.print("Run=");
    Serial.print(runLatch.Q);
    Serial.print(" TON.Q=");
    Serial.print(startDelay.Q);
    Serial.print(" ET=");
    Serial.print(static_cast<unsigned long>(startDelay.ET));
    Serial.print(" Starts=");
    Serial.print(static_cast<long>(starts.CV));
    Serial.print(" Cycle=");
    Serial.print(static_cast<unsigned long>(cycleTime.MS));
    Serial.println(" ms");
  }
}
