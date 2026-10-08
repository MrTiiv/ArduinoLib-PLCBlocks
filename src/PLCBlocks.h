// SPDX-License-Identifier: Unlicense
// Created with assistance from OpenAI ChatGPT/Codex; see NOTICE.md.

#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace PLCBlocks {

using Time = uint32_t;

// Siemens-style time helpers. Results that exceed the millis() range saturate.
inline Time clampTime(uint64_t milliseconds) {
  return milliseconds > UINT32_MAX ? UINT32_MAX
                                    : static_cast<Time>(milliseconds);
}

inline Time T_MS(uint32_t value)   { return value; }
inline Time T_SEC(uint32_t value)  { return clampTime(uint64_t(value) * 1000ULL); }
inline Time T_MIN(uint32_t value)  { return clampTime(uint64_t(value) * 60000ULL); }
inline Time T_HOUR(uint32_t value) { return clampTime(uint64_t(value) * 3600000ULL); }

// Reset-dominant Siemens SR.
class SR {
 public:
  bool Q = false;

  void process(bool S, bool R1);
  void reset();
};

// Set-dominant Siemens RS.
class RS {
 public:
  bool Q = false;

  void process(bool S1, bool R);
  void reset();
};

class P_TRIG {
 public:
  bool Q = false;

  void process(bool CLK);
  void process(bool CLK, uint32_t activeCycles);
  void reset();
  void reset(bool initialState);

 private:
  bool previous_ = false;
  uint32_t remainingCycles_ = 0;
};

class N_TRIG {
 public:
  bool Q = false;

  void process(bool CLK);
  void process(bool CLK, uint32_t activeCycles);
  void reset();
  void reset(bool initialState);

 private:
  bool previous_ = false;
  uint32_t remainingCycles_ = 0;
};

class TON {
 public:
  explicit TON(Time preset = 0) : pt_(preset) {}

  bool Q = false;
  Time ET = 0;

  void process(bool IN);
  void process(bool IN, Time PT);
  void processAt(bool IN, Time PT, uint32_t now);
  void setPT(Time PT) { pt_ = PT; }
  Time preset() const { return pt_; }
  void reset();

 private:
  Time pt_ = 0;
  Time elapsed_ = 0;
  uint32_t lastUpdate_ = 0;
  bool timing_ = false;
};

class TOF {
 public:
  explicit TOF(Time preset = 0) : pt_(preset) {}

  bool Q = false;
  Time ET = 0;

  void process(bool IN);
  void process(bool IN, Time PT);
  void processAt(bool IN, Time PT, uint32_t now);
  void setPT(Time PT) { pt_ = PT; }
  Time preset() const { return pt_; }
  void reset();

 private:
  Time pt_ = 0;
  Time elapsed_ = 0;
  uint32_t lastUpdate_ = 0;
  bool timing_ = false;
  bool previousIn_ = false;
};

class TP {
 public:
  explicit TP(Time preset = 0) : pt_(preset) {}

  bool Q = false;
  Time ET = 0;

  void process(bool IN);
  void process(bool IN, Time PT);
  void processAt(bool IN, Time PT, uint32_t now);
  void setPT(Time PT) { pt_ = PT; }
  Time preset() const { return pt_; }
  void reset();

 private:
  Time pt_ = 0;
  Time elapsed_ = 0;
  uint32_t lastUpdate_ = 0;
  bool active_ = false;
  bool previousIn_ = false;
};

class CTU {
 public:
  bool Q = false;
  int32_t CV = 0;

  void process(bool CU, bool R, int32_t PV);
  void reset(int32_t value = 0);

 private:
  bool previousCU_ = false;
};

class CTD {
 public:
  bool Q = true;
  int32_t CV = 0;

  void process(bool CD, bool LD, int32_t PV);
  void reset(int32_t value = 0);

 private:
  bool previousCD_ = false;
};

class CTUD {
 public:
  bool QU = false;
  bool QD = true;
  int32_t CV = 0;

  void process(bool CU, bool CD, bool R, bool LD, int32_t PV);
  void reset(int32_t value = 0);

 private:
  bool previousCU_ = false;
  bool previousCD_ = false;
};

class FirstScan {
 public:
  bool Q = false;

  void process();
  void reset();

 private:
  bool first_ = true;
};

class CycleTime {
 public:
  Time MS = 0;
  float Seconds = 0.0f;

  void process();
  void processAt(uint32_t now);
  void reset();
  void resetAt(uint32_t now);

 private:
  uint32_t previous_ = 0;
  bool initialized_ = false;
};

// TIA-style clock byte. Q0..Q7 use periods of
// 100, 200, 400, 500, 800, 1000, 1600 and 2000 ms.
class ClockMemory {
 public:
  bool Q0 = false;
  bool Q1 = false;
  bool Q2 = false;
  bool Q3 = false;
  bool Q4 = false;
  bool Q5 = false;
  bool Q6 = false;
  bool Q7 = false;
  uint8_t Value = 0;

  void process();
  void processAt(uint32_t now);
  void reset();
  void resetAt(uint32_t now);
  bool getBit(uint8_t index) const;

 private:
  void publish();

  uint32_t lastToggle_[8] = {};
  bool states_[8] = {};
  bool initialized_ = false;
};

}  // namespace PLCBlocks
