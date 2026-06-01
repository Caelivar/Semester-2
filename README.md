# Semester 2 Bottle Opener Firmware

Firmware for an automatic bottle opener prototype built with an Arduino Mega
2560, Arduino Motor Shield Rev3, Nextion display, HX711 load cell amplifier,
DC motors, and continuous-rotation servos.

## What It Does

- Runs the automatic bottle-opening sequence from a Nextion `START` button.
- Shows the measured load on the display as kilograms, for example `0.7 kg`.
- Stops the pressure motor when the load cell reaches `700 g`.
- Supports `CALIBRATE`, `STOP`, `TARE`, and `MANUAL` controls on the display.
- Uses smooth PWM ramping for DC motors instead of instant full-speed starts.
- Uses a physical stop button as a reference/limit input for Motor A.
- Sends debug logs over USB serial for HX711 testing and calibration.

## Main Hardware

- Arduino Mega 2560
- Arduino Motor Shield Rev3
- Nextion display
- HX711 load cell amplifier + load cell
- 2 continuous-rotation servos
- 2 DC motors on the Motor Shield
- 1 DC motor on an external H-bridge driver

## Project Structure

```text
src/
  main.c            Editable configuration and main loop
  app.c             Main state machine and opening sequence
  nextion.c         Nextion display communication
  hx711.c           Load cell reading, tare, and filtering
  motor_shield.c    Motor Shield Rev3 control
  external_motor.c  External H-bridge motor control
  servo.c           Servo pulse generation
  ramp.c            PWM ramp helper
  button.c          Physical stop button input
  debug.c           USB serial debug output
```

## Nextion Display

The firmware expects two Nextion pages:

- `page0`: main screen with `START`, `CALIBRATE`, `STOP`, `TARE`, `MANUAL`
- `page1`: manual test screen for motors and servos

Display fields used by the firmware:

- `tLoad`: load value in kg, for example `0.7 kg`
- `jLoad`: progress bar toward the load threshold
- `tState`: machine state
- `tStep`: current sequence step

## Key Wiring

| Function | Arduino Mega pins |
|---|---|
| Nextion Serial2 | D16 TX2, D17 RX2 |
| Servo signals | D44, D45 |
| External H-bridge | D10 ENA, D22 INA, D23 INB |
| HX711 | D24 DOUT, D25 SCK |
| Physical stop button | D26 to GND |
| Motor Shield A | D12 direction, D3 PWM, D9 brake |
| Motor Shield B | D13 direction, D11 PWM, D8 brake |

All motor drivers, the HX711, the display, and external power supplies must
share a common ground with the Arduino.

## Build

Install PlatformIO, then run:

```powershell
platformio run
```

Upload to the Arduino Mega:

```powershell
platformio run --target upload
```

Open USB serial debug logs:

```powershell
platformio device monitor -b 115200
```

## Calibration Notes

The HX711 is automatically tared on startup, so the load cell should be
unloaded when the Arduino is powered. The load conversion factor is configured
in `src/main.c` as `CFG_HX711_COUNTS_PER_GRAM` and should be adjusted with a
known weight.
