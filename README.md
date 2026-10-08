# PLCBlocks

PLCBlocks brings commonly used PLC function blocks inspired by Siemens TIA
Portal to Arduino-compatible microcontrollers. The blocks are instance-based,
non-blocking, and called once per `loop()` in a PLC-like scan cycle.

The library includes latches, edge detection, timers, counters, and utility
blocks for the first scan, cycle time, and clock memory.

## AI Transparency

As stated in my profile description, I want to be transparent about how I use
AI in my projects.

I used Codex for this project.

If you want to know how I used AI for this project, you can read the
[NOTICE.md](NOTICE.md).

## Supported Platforms

The library uses only the Arduino API and is declared compatible with all
architectures in `library.properties` (`architectures=*`). It can be used with
classic Arduino boards and ESP32 boards, for example. Actual compatibility also
depends on the board core in use.

No additional libraries are required.

## Installation

### Install as a ZIP File in the Arduino IDE

1. Download this repository from GitHub using **Code > Download ZIP**.
2. In the Arduino IDE, select **Sketch > Include Library > Add .ZIP Library...**.
3. Select the downloaded ZIP file.
4. Open an example from **File > Examples > PLCBlocks**.

These steps follow the official
[Arduino library installation guide](https://support.arduino.cc/hc/en-us/articles/5145457742236-Install-libraries-in-the-Arduino-IDE).

Alternatively, copy the `PLCBlocks` folder into the `libraries` folder of your
Arduino sketchbook, then restart the Arduino IDE.

### Install with the Arduino Library Manager

Once PLCBlocks is available through the official Arduino Library Manager, it can
be installed directly from the IDE:

1. Open **Tools > Manage Libraries...**.
2. Search for `PLCBlocks`.
3. Select **Install**.

ZIP installation works even if the library is not yet available through the
Library Manager.

## Quick Start: On-Delay Timer

Connect a push button between pin 4 and GND. When the button is held for two
seconds, the built-in LED turns on. Because the input uses `INPUT_PULLUP`, a
pressed button reads `LOW`.

```cpp
#include <PLCBlocks.h>

using namespace PLCBlocks;

constexpr uint8_t BUTTON_PIN = 4;
#ifdef LED_BUILTIN
constexpr uint8_t LED_PIN = LED_BUILTIN;
#else
constexpr uint8_t LED_PIN = 13;  // Adjust for your board if necessary.
#endif

TON onDelay(T_SEC(2));

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  const bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  onDelay.process(buttonPressed);
  digitalWrite(LED_PIN, onDelay.Q ? HIGH : LOW);
}
```

The call to `onDelay.process(...)` does not block program execution. Other tasks
can continue to run during the two-second delay.

## Basic Concept

Each object represents an independent function block instance with its own
internal state:

```cpp
TON motor1Delay(T_SEC(2));
TON motor2Delay(T_SEC(5));
```

For predictable PLC-like behavior, call every function block instance exactly
once per `loop()` iteration and in the same order whenever possible. The blocks
do not require `delay()`.

Read the outputs after calling `process()`:

```cpp
motor1Delay.process(motor1Request);
digitalWrite(MOTOR1_PIN, motor1Delay.Q);
```

## Time Values

`Time` is an unsigned 32-bit value in milliseconds. The following helpers make
time values easier to read:

```cpp
T_MS(250)    // 250 ms
T_SEC(2)     // 2 s
T_MIN(3)     // 3 min
T_HOUR(1)    // 1 h
```

Values above the supported `millis()` range are clamped to `UINT32_MAX`. The
function blocks handle `millis()` rollover correctly.

## Function Block Reference

### Latches and Edge Detection

| Block | Call | Description |
|---|---|---|
| `SR` | `process(S, R1)` | Set/reset latch with reset priority |
| `RS` | `process(S1, R)` | Set/reset latch with set priority |
| `P_TRIG` | `process(CLK)` | `Q` is active for one cycle on a rising edge |
| `N_TRIG` | `process(CLK)` | `Q` is active for one cycle on a falling edge |

Use `process(CLK, activeCycles)` to keep the edge output active for multiple
`loop()` iterations. `activeCycles` specifies scan cycles, not milliseconds.

`reset()` clears the output while preserving the last detected input state. Use
`reset(initialState)` to explicitly set the stored input state as well.

### Timers

| Block | Call | Outputs | Description |
|---|---|---|---|
| `TON` | `process(IN, PT)` | `Q`, `ET` | On-delay timer |
| `TOF` | `process(IN, PT)` | `Q`, `ET` | Off-delay timer |
| `TP` | `process(IN, PT)` | `Q`, `ET` | Fixed-duration pulse timer |

The preset time can be passed to the constructor:

```cpp
TOF fanRunOn(T_SEC(10));
fanRunOn.process(machineRunning);
```

Alternatively, pass it with every call:

```cpp
TOF fanRunOn;
fanRunOn.process(machineRunning, T_SEC(10));
```

`setPT()` changes the stored preset time, `preset()` returns it, and `reset()`
resets the block. `ET` contains the elapsed time in milliseconds.

The `processAt(..., now)` variants are intended for tests or applications that
provide an external time base. For normal sketches, `process()` with its internal
`millis()` time base is simpler.

### Counters

| Block | Call | Outputs | Description |
|---|---|---|---|
| `CTU` | `process(CU, R, PV)` | `Q`, `CV` | Up counter; counts rising edges on `CU` |
| `CTD` | `process(CD, LD, PV)` | `Q`, `CV` | Down counter; `LD` loads `PV` |
| `CTUD` | `process(CU, CD, R, LD, PV)` | `QU`, `QD`, `CV` | Up/down counter |

`PV` is the preset value and `CV` is the current count. For `CTU`, `Q` becomes
`true` when `CV >= PV`. For `CTD`, `Q` becomes `true` when `CV <= 0`. For
`CTUD`, `QU` and `QD` indicate the upper and lower limits respectively.

Priorities:

- `CTU`: reset before counting
- `CTD`: load before counting
- `CTUD`: reset before load before counting
- Simultaneous rising edges on `CU` and `CD` do not change `CV`

Mechanical push buttons can bounce and cause multiple counts. Use hardware or
software debouncing for real button inputs.

### System Blocks

| Block | Outputs | Description |
|---|---|---|
| `FirstScan` | `Q` | Active only on the first call after startup or `reset()` |
| `CycleTime` | `MS`, `Seconds` | Time elapsed since the previous call |
| `ClockMemory` | `Q0` to `Q7`, `Value` | Eight periodic clock bits |

The `ClockMemory` periods are:

| Output | Period |
|---|---:|
| `Q0` | 100 ms |
| `Q1` | 200 ms |
| `Q2` | 400 ms |
| `Q3` | 500 ms |
| `Q4` | 800 ms |
| `Q5` | 1 s |
| `Q6` | 1.6 s |
| `Q7` | 2 s |

`Value` contains all eight states as a byte. Alternatively, use `getBit(index)`
to read a single clock bit.

## Examples

After installation, the examples are available under
**File > Examples > PLCBlocks**:

- **BlinkWithClockMemory** – blink an LED without `delay()`
- **StartStopLatch** – start/stop control using a reset-dominant `SR` latch
- **OnDelayTimer** – on-delay behavior using `TON`
- **EdgeCounter** – count rising clock edges using `CTU`
- **TimerOverview** – compare `TON`, `TOF`, and `TP` in the Serial Monitor
- **BasicDemo** – combine multiple blocks in a cyclic program

All hardware examples use `LED_BUILTIN` where available. Pins and signal levels
can be adjusted at the beginning of each sketch to match the target board.


## License

PLCBlocks is released under the [Unlicense](LICENSE) and is provided without
warranty.
