# Agent Notes

## Project
- **Name**: _UM982Parse
- **Summary**: To take the AGRIC output from the UniCore UM982 dual-antenna GPS and parse it into fields
- **Primary goals**:
  - Provide a simple class-based implementation that users can easily integrate into their projects
  - Keep it simple - a single call in the setup() loop should initialise the class, with little more than the Serial* input expected to receive the data
  - setup should take an optional parameter of antenna heigh - if provided, the true-position of antenna base with reference to the ground should be provided, calculated from antenna position, roll angle, and antenna height
  - Output it in a useful struct, or standard-style NMEA sentence
  - Performance is key
  - It should not block or delay the call to read any more than is strictly necessary (eg, definitely no use of delay())

## Inputs / Outputs
- **Input data**:
  - AGRIC sentence from UM982
  - Sample (in ASCII, but we will be receiving in binary): #AGRICA,97,GPS,FINE,2190,363942000,0,0,18,12;GNSS,232,21,12,30,5,5,24,1,0,5,15,1,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.005,-0.003,0.001,0.004,0.042,0.050,0.044,40.07898274722,116.23663152683,60.0036,-2160488.6213,4383615.6655,4084732.9679,1.8493,1.8902,4.4654,0.0000,0.0000,0.0000,0.00000000000,0.00000000000,0.0000,-0.00000000000,0.00000000000,0.0000,363942000,0.000,15.213205,-8.492279,0.000000,0.000000,5,0,0,0*0b2e294a

# Field definitions

### Message Header

### Table 7-47 Binary Message Structure

| ID | Structure | Description |
|---|---|--|
| 1 | Header | All Unicore messages have a header. Binary format header has 3 syn bytes and 24 bytes in total. See Table 7-49 Binary Header Structure for more information. Please always check the header length before decoding binary messages. ASCII format header is described in Table 7-50 ASCII Header Structure. |
| 2 | Data | Data field, the length is variable according to different message types. Please refer to specific messages for more information. |
|3 | CRC | Unicore messages end with 32-bit CRC. Binary format messages contain a 32-bit CRC calculating all data including the header. ASCII format CRC calculates all data except “#”

### Table 7-48 - Sync Bytes of Binary Header

| Byte  | Hex  | Decimal |
|-------|------|---------|
| First | 0xAA | 170     |
| Second| 0x44 | 68      |
| Third | 0xB5 | 181     |


### Table 7-49 Binary Header Structure

| ID | Field         | Type   | Description                         | Binary Bytes | Binary Offset |
|----|---------------|--------|-------------------------------------|--------------|---------------|
| 1  | Sync          | UCHAR  | Hexadecimal `0xAA`                  | 1            | 0 |
| 2  | Sync          | UCHAR  | Hexadecimal `0x44`                  | 1            | 1 |
| 3  | Sync          | UCHAR  | Hexadecimal `0xB5`                  | 1            | 2 |
| 4  | CPUIDle       | UCHAR  | CPU idle (0–100)                    | 1            | 3 |
| 5  | Message ID    | USHORT | Message ID                          | 2            | 4 |
| 6  | MessageLength | USHORT | Message length                      | 2            | 6 |
| 7  | TimeRef       | UCHAR  | Reference time (GPST or BDST)       | 1            | 8 |
| 8  | TimeStatus    | UCHAR  | Time status                         | 1            | 9 |
| 9  | Wn            | USHORT | Week number                         | 2            | 10 |
| 10 | Ms            | ULONG  | Seconds of week (ms)                | 4            | 12 |
| 11 | Version       | ULONG  | Release version                     | 4            | 16 |
| 12 | Reserved      | UCHAR  | Reserved                            | 1            | 20 |
| 13 | Leap sec      | UCHAR  | Leap second                         | 1            | 21 |
| 14 | DelayMs       | USHORT | Output delay                        | 2            | 22 |


### Table 7-81 AGRIC Message Structure

| ID | Field    | Type  | Description | Format | Binary Bytes | Binary Offset |
|----|----------|-------|-------------|--------|--------------|---------------|
| 1  | AGRIC header | — | Log header, above | Binary Header | — | H + 0 |
| 2  | GNSS     | Char  | — | — | 4 | H |
| 3  | length   | uchar | Command length, from GNSS to CRC, 232 bytes in total, a fixed value of `0xE8` | — | 1 | H + 4 |
| 4  | Year     | uchar | UTC-year, for example: 2016 → 16; 2116 → 116 | — | 1 | H + 5 |
| 5  | Month    | uchar | UTC-month | — | 1 | H + 6 |
| 6  | Day      | uchar | UTC-day | — | 1 | H + 7 |
| 7  | Hour     | uchar | UTC-hour | — | 1 | H + 8 |
| 8  | Minute   | uchar | UTC-minute | — | 1 | H + 9 |
| 9  | Second   | uchar | UTC-second | — | 1 | H + 10 |
| 10 | Postype  | uchar | Rover position status:<br>• 0: Invalid solution<br>• 1: Single point solution<br>• 2: Pseudorange differential solution<br>• 4: Fixed solution<br>• 5: Float solution<br>• 7: Input a fixed position (only supported by specific versions) | — | 1 | H + 11 |
| 11 | Heading Status | uchar | Heading solution status of master and slave antennas:<br>• 0: Invalid solution<br>• 4: Fixed solution<br>• 5: Float solution | — | 1 | H + 12 |
| 12 | Num GPS Sta | uchar | Number of GPS satellites used in the solution | — | 1 | H + 13 |
| 13 | Num BDS Sta | uchar | Number of BDS satellites used in the solution | — | 1 | H + 14 |
| 14 | Num GLO Sta | uchar | Number of GLONASS satellites used in the solution | — | 1 | H + 15 |
| 15 | Baseline_N | float | Baseline vector from the base station to the rover station, northern component | — | 4 | H + 16 |
| 16 | Baseline_E | float | Baseline vector from the base station to the rover station, eastern component | — | 4 | H + 20 |
| 17 | Baseline_U | float | Baseline vector from the base station to the rover station, vertical component | — | 4 | H + 24 |
| 18 | Baseline_NStd | float | Baseline vector from the base station to the rover station, northern component standard deviation | — | 4 | H + 28 |
| 19 | Baseline_EStd | float | Baseline vector from the base station to the rover station, eastern component standard deviation | — | 4 | H + 32 |
| 20 | Baseline_UStd | float | Baseline vector from the base station to the rover station, vertical component standard deviation | — | 4 | H + 36 |
| 21 | Heading | float | Heading | — | 4 | H + 40 |
| 22 | Pitch | float | Pitch | — | 4 | H + 44 |
| 23 | Roll | float | Roll | — | 4 | H + 48 |
| 24 | Speed | float | Speed, scalar | — | 4 | H + 52 |
| 25 | Velocity of North | float | North velocity | — | 4 | H + 56 |
| 26 | Velocity of East | float | East velocity | — | 4 | H + 60 |
| 27 | Velocity of Up | float | Up velocity | — | 4 | H + 64 |
| 28 | Xigema_Vx | float | North velocity standard deviation | — | 4 | H + 68 |
| 29 | Xigema_Vy | float | East velocity standard deviation | — | 4 | H + 72 |
| 30 | Xigema_Vz | float | Up velocity standard deviation | — | 4 | H + 76 |
| 31 | lat | double | Latitude of the rover station: −90° to 90°, positive for North and negative for South | — | 8 | H + 80 |
| 32 | lon | double | Longitude of the rover station: −180° to 180°, positive for East and negative for West | — | 8 | H + 88 |
| 33 | alt | double | Height of the rover station | — | 8 | H + 96 |
| 34 | ECEF_X | double | X axis of the ECEF coordinate system | — | 8 | H + 104 |
| 35 | ECEF_Y | double | Y axis of the ECEF coordinate system | — | 8 | H + 112 |
| 36 | ECEF_Z | double | Z axis of the ECEF coordinate system | — | 8 | H + 120 |
| 37 | Xigema_lat | float | Latitude standard deviation | — | 4 | H + 128 |
| 38 | Xigema_lon | float | Longitude standard deviation | — | 4 | H + 132 |
| 39 | Xigema_alt | float | Height standard deviation | — | 4 | H + 136 |
| 40 | Xigema_ECEF_X | float | ECEF_X standard deviation | — | 4 | H + 140 |
| 41 | Xigema_ECEF_Y | float | ECEF_Y standard deviation | — | 4 | H + 144 |
| 42 | Xigema_ECEF_Z | float | ECEF_Z standard deviation | — | 4 | H + 148 |
| 43 | BASE_lat | double | Latitude of the base station: −90° to 90° | — | 8 | H + 152 |
| 44 | BASE_lon | double | Longitude of the base station: −180° to 180° | — | 8 | H + 160 |
| 45 | BASE_alt | double | Height of the base station | — | 8 | H + 168 |
| 46 | SEC_lat | double | Latitude of the slave antenna: −90° to 90° | — | 8 | H + 176 |
| 47 | SEC_lon | double | Longitude of the slave antenna: −180° to 180° | — | 8 | H + 184 |
| 48 | SEC_alt | double | Height of the slave antenna | — | 8 | H + 192 |
| 49 | GPS_WEEK_SECOND | int | Milliseconds of GPS week | — | 4 | H + 200 |
| 50 | Diffage | float | Differential age | — | 4 | H + 204 |
| 51 | Speed_Heading | float | Direction of velocity | — | 4 | H + 208 |
| 52 | Undulation | float | Undulation | — | 4 | H + 212 |
| 53 | Remain_float_3 | float | Reserved | — | 4 | H + 216 |
| 54 | Remain_float_4 | float | Reserved | — | 4 | H + 220 |
| 55 | Num GAL Sta | uchar | Number of Galileo satellites | — | 1 | H + 224 |
| 56 | Speed_Type | uchar | Speed solution status:<br>• 0: speed solution status valid<br>• 1: speed solution status invalid | — | 1 | H + 225 |
| 57 | Remain_char_3 | uchar | Reserved | — | 1 | H + 226 |
| 58 | Remain_char_4 | uchar | Reserved | — | 1 | H + 227 |
| 59 | xxxx | HEX | 32-bit CRC | — | 4 | H + 228 |
| 60 | [CR][LF] | — | Sentence terminator (ASCII only) | — | — | — |


# Order of information

- on receipt of sync characters, begin parsing
- no outputs required from tables 7-48 or 7-49

- **Output data**:
  - Output information should contain the following fields

|Field|Information|
|---|---|
| $PANDA | Sentence header |
| (1) Time of fix | Time of fix (hhmmss) |
| (2,3) | Latitude 4807.038,N → Latitude 48° 07.038' N |
| (4,5) | Longitude 01131.000,E → Longitude 11° 31.000' E |
| (6) Fix quality | 0 = invalid<br>1 = GPS fix (SPS)<br>2 = DGPS fix<br>3 = PPS fix<br>4 = Real Time Kinematic<br>5 = Float RTK<br>6 = estimated (dead reckoning) (2.3 feature)<br>7 = Manual input mode<br>8 = Simulation mode |
| (7) Satellites | Number of satellites being tracked |
| (8) HDOP | Horizontal dilution of position (e.g., 0.9) |
| (9) Altitude | Altitude in meters above mean sea level (e.g., 545.4) |
| (10) DGPS age | Time in seconds since last DGPS update (e.g., 1.2) |
| (11) Speed | Speed in knots |
| (12) Heading | Heading in degrees |
| (13) Roll angle | Roll angle in degrees (positive roll = right leaning: right down, left up) |
| (14) Pitch angle | Pitch angle in degrees (positive pitch = nose up) |
| (15) Yaw rate | Yaw rate in degrees per second |



## Notes
- How to configure the module:
- Message ID: 11276

| Syntax | Command | Command2 |
|---|--|--|
| ASCII Syntax: | AGRICA 1 | AGRICA COM2 1 |
| Binary Syntax: | AGRICB 1 | AGRICB COM2 1 |
