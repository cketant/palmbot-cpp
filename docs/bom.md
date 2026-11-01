# PALM-BOT — Bill of Materials

| Ref | Item | Category | Price | Notes |
|---|---|---|---|---|
| U1 | Raspberry Pi 5, 8 GB | Compute | incl. | Brain: LLM, vision, agent loop |
| U2 | Raspberry Pi Active Cooler (fan + heatsink) | Compute | incl. | Pi 5 runs hot under sustained AI load |
| U3 | SanDisk 64 GB Extreme microSD (UHS-I, 170 MB/s) | Compute | incl. | OS + models |
| CAM1 | Raspberry Pi Camera Module 3 | Vision | incl. | Object detection + ArUco localization |
| CAM2 | Mekalero Pi 5 camera cable — 22-pin → 15-pin FPC, 500 mm | Vision | incl. | Pi 5's narrow connector needs this adapter ribbon |
| PT1 | Arducam Camera Pan-Tilt Platform (B0283) — 2× GH-S37D servos + its own PCA9685 I²C board | Vision | incl. | Aims the camera; has its own control board (not SRVDRV) |
| BT1 | Geekworm X1201 Ultra-Thin 5 V UPS board for Pi 5 (5.1 V / 5 A) | Power | incl. | Stable rail on the move + safe shutdown; battery % over I²C (MAX17040, 0x36) |
| BT2 | 2× Molicel P28A 18650, 2800 mAh, 35 A, flat-top (unprotected — HAT does protection) | Power | $13.40 | ~1.5–2.5 h runtime; charge via the X1201's USB-C, not the Pi's |
| M1, M2 | 2× Pololu #5141 150:1 Micro Metal Gearmotor MP, 6 V, 12 CPR encoder (side connector) | Motion | $59.90 | Drive + odometry; 6-wire JST SH (2 motor + 4 encoder) |
| DRV1 | HiLetgo DRV8833 dual H-bridge motor driver (3-pack) | Motion | incl. | Lets logic pins safely switch motor current (chip by TI) |
| CAB1 | 2× Pololu #4763 6-pin JST SH female cable, 30 cm (one per motor) — **STILL TO BUY** | Motion | $5.90 | Encoders ship with a JST SH connector, not leads. Amazon alt: B0BKSNMCV4 |
| W1 | Pololu Wheel 42 × 19 mm pair (fits the gearmotor D-shaft) | Motion | $9.95 | The two driven wheels (press-fit) |
| BRK1 | Pololu Micro Metal Gearmotor bracket pair (#989) — fallback clamp (#2-56 × 7/16″ screws incl.) | Motion | $2.95 | Fouls the 42×19 mm recessed hubs, so superseded by the printed mount |
| BRK2 | Motor mount — **printed into the CH1 chassis** (no bracket to buy) | Motion | $0 | Clamp/pocket sized to the 10×12×26 mm motor with recessed-hub clearance |
| CAS1 | CY-15A Rueda Loca Metálica — 5/8″ steel ball, ~20 mm, M4 holes @ 40 mm — **purchased** | Motion | 10,800 COP | Rear caster; ~20 mm sits just under the 21 mm ideal — add a ~1 mm shim if it noses down |
| CH1 | Custom 3D-printed chassis (≤ 100 × 180 mm) + 365-pc M2.5 nylon standoff/screw kit | Structure | incl. | The body everything mounts to |
| GRP1 | LewanSoul mechanical claw / gripper | Manipulation | incl. | Jaw assembly. NOTE: designed for a standard-size servo — an MG90S may not fit; consider an MG90S-native printed claw |
| SRV1, SRV2 | 2× Miuzei MG90S metal-gear micro servos | Manipulation | incl. | Arm lift + claw drive (pan/tilt uses PT1's own servos) |
| SRVDRV | HiLetgo PCA9685 16-channel 12-bit I²C PWM / servo driver (2-pack) | Manipulation | incl. | Drives the two gripper servos. Default 0x40 — clashes with PT1; re-address one |
| SW1 | HiLetgo KW12-3 micro limit switch, roller lever (10-pack) | Manipulation | incl. | In one jaw — hardware "touching object" signal, GPIO 27 |
| IMU1 | HiLetgo GY-521 MPU-6050 6-axis IMU (gyro + accel) | Sensing | incl. | Heading + tilt; I²C 0x68 |
| TOF1 | HiLetgo VL53L0X time-of-flight distance sensor | Sensing | incl. | Forward distance — obstacle avoidance + grab confirmation; I²C 0x29 |
| LED1 | DIYmall WS2812B RGB LED ring, 12 pixels | I/O | incl. | Status ring; SPI MOSI on GPIO 10 (Pi 5 method) |
| MIC1 | SunFounder USB 2.0 mini microphone | I/O | incl. | Whisper speech-to-text input; USB-A |
| SPK1 | MAX98357 I²S Class-D amplifier (2-pack) | I/O | incl. | Spoken replies via Piper TTS; I²S GPIO 18/19/21 |
| SPK2 | Gikfun 4 Ω 40 mm 3 W full-range speaker (2-pack) | I/O | incl. | The driver the MAX98357 powers |
| MISC | ELEGOO 120-pc Dupont jumper wires (M-M, M-F, F-F) + standoffs | Wiring | incl. | Prototyping wiring |
| HW1 | 1110-pc M2 stainless machine screw + nut + washer assortment (4–12 mm) — **STILL TO BUY** | Hardware | $7.99 | Lift-arm joints, servo horns & claw base (the 365-pc kit is M2.5) |
| HW2 | 513-pc heat-set insert kit (M2–M6) + soldering-iron tips — **STILL TO BUY** | Hardware | $26.98 | Durable threads in the printed arm/lid via the TL1 iron |
| HW3 | M4 screws + nuts for the caster (2× M4 × 10 mm) — **STILL TO BUY** | Hardware | ~$5 | CAS1 uses M4 @ 40 mm; your kits are M2/M2.5 only |
| TL1 | 60 W soldering iron kit, adjustable temp, 5 tips | Tools | incl. | Motor leads, encoder pins, speaker, heat-set inserts |

## Still to buy

- **CAB1** — 2× Pololu #4763 6-pin JST SH cable — $5.90
- **HW1** — M2 screw/nut/washer assortment — $7.99
- **HW2** — heat-set insert kit + iron tips — $26.98
- **HW3** — M4 screws + nuts for the caster — ~$5
