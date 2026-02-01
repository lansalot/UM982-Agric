# UM982 AGRIC Parser

This repository contains a PlatformIO/Arduino C++ parser for UniCore UM982/UM98x AGRIC binary messages. It reads the UM982 binary stream, validates the CRC, decodes key fields, and can emit a lightweight NMEA-style $PANDA sentence suitable for downstream consumers.

## What it does
- Parses UM982 binary messages with sync bytes `0xAA 0x44 0xB5`.
- Validates the 32-bit CRC for each message.
- Decodes AGRIC payload fields into a structured `UM982PandaData` record.
- Formats output as a `$PANDA` sentence (GGA-like) with checksum.
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
- Seriously/practically, it's for UM982 dual only, hence the name of the library
- The `$PANDA` sentence is a project-specific, GGA-like format for agOpenGPS.


## Performance

Seems speedy enough, decoding in less than 30 microseconds (note values here are zero as I was testing with UM981, my UM982 is away for repair)

```
$PANDA,154732,0000.0000,N,00000.0000,E,0,0,,0.0,,0.0,0.0,0.0,0.0,*65 Satellites: 0  Parse duration: 27 us
$PANDA,154732,0000.0000,N,00000.0000,E,0,0,,0.0,,0.0,0.0,0.0,0.0,*65 Satellites: 0  Parse duration: 28 us
$PANDA,154732,0000.0000,N,00000.0000,E,0,0,,0.0,,0.0,0.0,0.0,0.0,*65 Satellites: 0  Parse duration: 28 us
$PANDA,154732,0000.0000,N,00000.0000,E,0,0,,0.0,,0.0,0.0,0.0,0.0,*65 Satellites: 0  Parse duration: 27 us
$PANDA,154732,0000.0000,N,00000.0000,E,0,0,,0.0,,0.0,0.0,0.0,0.0,*65 Satellites: 0  Parse duration: 28 us

```

