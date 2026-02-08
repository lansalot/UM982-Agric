# UM982 AGRIC Parser

This repository contains a PlatformIO/Arduino C++ parser for UniCore UM982/UM98x AGRIC binary messages. It reads the UM982 binary stream, validates the CRC, decodes key fields, and can emit a lightweight NMEA-style $PAOGI sentence suitable for downstream consumers.

## What it does
- Parses UM982 binary messages with sync bytes `0xAA 0x44 0xB5`.
- Validates the 32-bit CRC for each message.
- Decodes AGRIC payload fields into a structured `UM982PAOGIData` record.
- Formats output as a `$PAOGI` sentence (GGA-like) with checksum.
- Optionally applies antenna-height + roll correction to compute ground position at time of sample

## Repository layout
- [src/UM982Parser.cpp](src/UM982Parser.cpp): parser implementation.
- [include/UM982Parser.h](include/UM982Parser.h): data structures and public API.
- [src/main.cpp](src/main.cpp): example wiring for Serial3 input and Serial output.
- [agent.md](agent.md): working notes and field references.

## Usage (Arduino/PlatformIO)
1. Connect the GPS module UART to your board’s `Serial3` (typical for AOG, or change to your port).
2. In `setup()`, call `parser.begin(SerialGPS, antennaHeightMeters)`.
3. In `loop()`, call `parser.update()` and handle messages.

## Notes
- AGRIC content may vary by receiver model/firmware (e.g., single-antenna units may report zeros for some fields).
- No, seriously - it's for UM982 dual only, hence the name of the library


## Performance

Seems speedy enough, decoding in approx 35 microseconds

```
$PAOGI,211144,5729.7208,N,00407.1813,W,1,22,,43.0,,0.0,118.3,0.0,-1.8,*5D Satellites: 22  Parse duration: 35 us
$PAOGI,211144,5729.7208,N,00407.1813,W,1,22,,43.0,,0.0,117.7,0.0,-1.9,*57 Satellites: 22  Parse duration: 35 us
$PAOGI,211145,5729.7208,N,00407.1813,W,1,22,,43.0,,0.0,117.7,0.0,-1.6,*59 Satellites: 22  Parse duration: 35 us
$PAOGI,211145,5729.7208,N,00407.1813,W,1,22,,43.0,,0.0,117.6,0.0,-2.1,*5C Satellites: 22  Parse duration: 35 us


```

