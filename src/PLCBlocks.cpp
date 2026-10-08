// SPDX-License-Identifier: Unlicense
// Created with assistance from OpenAI ChatGPT/Codex; see NOTICE.md.

#include "PLCBlocks.h"

#include <limits.h>

namespace PLCBlocks {

namespace {

Time addElapsed(Time current, Time delta) {
  return delta > UINT32_MAX - current ? UINT32_MAX : current + delta;
}

}  // namespace

void SR::process(bool S, bool R1) {
  if (R1) {
    Q = false;
  } else if (S) {
    Q = true;
  }
}

void SR::reset() { Q = false; }

void RS::process(bool S1, bool R) {
  if (S1) {
    Q = true;
  } else if (R) {
    Q = false;
  }
}

void RS::reset() { Q = false; }

void P_TRIG::process(bool CLK) { process(CLK, 1); }

void P_TRIG::process(bool CLK, uint32_t activeCycles) {
  if (CLK && !previous_) {
    remainingCycles_ = activeCycles;
  }

  Q = (remainingCycles_ > 0);
  if (remainingCycles_ > 0) {
    --remainingCycles_;
  }
  previous_ = CLK;
}

void P_TRIG::reset() {
  remainingCycles_ = 0;
  Q = false;
}

void P_TRIG::reset(bool initialState) {
  previous_ = initialState;
  reset();
}

void N_TRIG::process(bool CLK) { process(CLK, 1); }

void N_TRIG::process(bool CLK, uint32_t activeCycles) {
  if (!CLK && previous_) {
    remainingCycles_ = activeCycles;
  }

  Q = (remainingCycles_ > 0);
  if (remainingCycles_ > 0) {
    --remainingCycles_;
  }
  previous_ = CLK;
}

void N_TRIG::reset() {
  remainingCycles_ = 0;
  Q = false;
}

void N_TRIG::reset(bool initialState) {
  previous_ = initialState;
  reset();
}

void TON::process(bool IN) { processAt(IN, pt_, millis()); }

void TON::process(bool IN, Time PT) { processAt(IN, PT, millis()); }

void TON::processAt(bool IN, Time PT, uint32_t now) {
  pt_ = PT;
  if (!IN) {
    reset();
    return;
  }

  if (!timing_) {
    elapsed_ = 0;
    lastUpdate_ = now;
    timing_ = true;
  } else {
    elapsed_ = addElapsed(elapsed_, static_cast<Time>(now - lastUpdate_));
    lastUpdate_ = now;
  }

  ET = elapsed_ >= pt_ ? pt_ : elapsed_;
  Q = (elapsed_ >= pt_);
}

void TON::reset() {
  Q = false;
  ET = 0;
  elapsed_ = 0;
  timing_ = false;
  lastUpdate_ = 0;
}

void TOF::process(bool IN) { processAt(IN, pt_, millis()); }

void TOF::process(bool IN, Time PT) { processAt(IN, PT, millis()); }

void TOF::processAt(bool IN, Time PT, uint32_t now) {
  pt_ = PT;

  if (IN) {
    Q = true;
    ET = 0;
    timing_ = false;
  } else {
    if (previousIn_) {
      elapsed_ = 0;
      lastUpdate_ = now;
      timing_ = true;
    }

    if (timing_) {
      elapsed_ = addElapsed(elapsed_, static_cast<Time>(now - lastUpdate_));
      lastUpdate_ = now;
      ET = elapsed_ >= pt_ ? pt_ : elapsed_;
      Q = (elapsed_ < pt_);
      if (!Q) {
        timing_ = false;
      }
    } else {
      Q = false;
    }
  }

  previousIn_ = IN;
}

void TOF::reset() {
  Q = false;
  ET = 0;
  elapsed_ = 0;
  timing_ = false;
  previousIn_ = false;
  lastUpdate_ = 0;
}

void TP::process(bool IN) { processAt(IN, pt_, millis()); }

void TP::process(bool IN, Time PT) { processAt(IN, PT, millis()); }

void TP::processAt(bool IN, Time PT, uint32_t now) {
  pt_ = PT;
  const bool risingEdge = IN && !previousIn_;

  if (!active_ && risingEdge) {
    elapsed_ = 0;
    lastUpdate_ = now;
    active_ = (pt_ > 0);
    Q = active_;
    ET = 0;
  }

  if (active_) {
    elapsed_ = addElapsed(elapsed_, static_cast<Time>(now - lastUpdate_));
    lastUpdate_ = now;
    ET = elapsed_ >= pt_ ? pt_ : elapsed_;
    Q = (elapsed_ < pt_);
    if (!Q) {
      active_ = false;
      if (!IN) {
        ET = 0;
      }
    }
  } else if (!IN) {
    ET = 0;
    Q = false;
  }

  previousIn_ = IN;
}

void TP::reset() {
  Q = false;
  ET = 0;
  elapsed_ = 0;
  active_ = false;
  previousIn_ = false;
  lastUpdate_ = 0;
}

void CTU::process(bool CU, bool R, int32_t PV) {
  const bool risingCU = CU && !previousCU_;
  previousCU_ = CU;

  if (R) {
    CV = 0;
  } else if (risingCU && CV < INT32_MAX) {
    ++CV;
  }
  Q = (CV >= PV);
}

void CTU::reset(int32_t value) {
  CV = value;
  Q = false;
  previousCU_ = false;
}

void CTD::process(bool CD, bool LD, int32_t PV) {
  const bool risingCD = CD && !previousCD_;
  previousCD_ = CD;

  if (LD) {
    CV = PV;
  } else if (risingCD && CV > INT32_MIN) {
    --CV;
  }
  Q = (CV <= 0);
}

void CTD::reset(int32_t value) {
  CV = value;
  Q = (CV <= 0);
  previousCD_ = false;
}

void CTUD::process(bool CU, bool CD, bool R, bool LD, int32_t PV) {
  const bool risingCU = CU && !previousCU_;
  const bool risingCD = CD && !previousCD_;
  previousCU_ = CU;
  previousCD_ = CD;

  if (R) {
    CV = 0;
  } else if (LD) {
    CV = PV;
  } else if (risingCU != risingCD) {
    if (risingCU && CV < INT32_MAX) {
      ++CV;
    } else if (risingCD && CV > INT32_MIN) {
      --CV;
    }
  }

  QU = (CV >= PV);
  QD = (CV <= 0);
}

void CTUD::reset(int32_t value) {
  CV = value;
  QU = false;
  QD = (CV <= 0);
  previousCU_ = false;
  previousCD_ = false;
}

void FirstScan::process() {
  Q = first_;
  first_ = false;
}

void FirstScan::reset() {
  Q = false;
  first_ = true;
}

void CycleTime::process() { processAt(millis()); }

void CycleTime::processAt(uint32_t now) {
  if (!initialized_) {
    MS = 0;
    Seconds = 0.0f;
    initialized_ = true;
  } else {
    MS = static_cast<Time>(now - previous_);
    Seconds = static_cast<float>(MS) / 1000.0f;
  }
  previous_ = now;
}

void CycleTime::reset() {
  MS = 0;
  Seconds = 0.0f;
  previous_ = 0;
  initialized_ = false;
}

void CycleTime::resetAt(uint32_t now) {
  MS = 0;
  Seconds = 0.0f;
  previous_ = now;
  initialized_ = true;
}

void ClockMemory::process() { processAt(millis()); }

void ClockMemory::processAt(uint32_t now) {
  static const uint16_t halfPeriods[8] = {50, 100, 200, 250,
                                         400, 500, 800, 1000};

  if (!initialized_) {
    resetAt(now);
    return;
  }

  for (uint8_t i = 0; i < 8; ++i) {
    const uint32_t elapsed = now - lastToggle_[i];
    const uint32_t steps = elapsed / halfPeriods[i];
    if (steps > 0) {
      if ((steps & 1U) != 0U) {
        states_[i] = !states_[i];
      }
      lastToggle_[i] += steps * halfPeriods[i];
    }
  }
  publish();
}

void ClockMemory::reset() { resetAt(millis()); }

void ClockMemory::resetAt(uint32_t now) {
  for (uint8_t i = 0; i < 8; ++i) {
    lastToggle_[i] = now;
    states_[i] = false;
  }
  initialized_ = true;
  publish();
}

bool ClockMemory::getBit(uint8_t index) const {
  return index < 8 ? states_[index] : false;
}

void ClockMemory::publish() {
  Q0 = states_[0];
  Q1 = states_[1];
  Q2 = states_[2];
  Q3 = states_[3];
  Q4 = states_[4];
  Q5 = states_[5];
  Q6 = states_[6];
  Q7 = states_[7];

  Value = 0;
  for (uint8_t i = 0; i < 8; ++i) {
    if (states_[i]) {
      Value |= static_cast<uint8_t>(1U << i);
    }
  }
}

}  // namespace PLCBlocks
