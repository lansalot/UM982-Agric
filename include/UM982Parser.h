#pragma once

#include <Arduino.h>

struct UM982BinaryHeader
{
    uint8_t sync1 = 0;
    uint8_t sync2 = 0;
    uint8_t sync3 = 0;
    uint8_t cpuIdle = 0;
    uint16_t messageId = 0;
    uint16_t messageLength = 0;
    uint8_t timeRef = 0;
    uint8_t timeStatus = 0;
    uint16_t week = 0;
    uint32_t msOfWeek = 0;
    uint32_t version = 0;
    uint8_t reserved = 0;
    uint8_t leapSeconds = 0;
    uint16_t delayMs = 0;
};

struct UM982Message
{
    UM982BinaryHeader header;
    uint16_t payloadLength = 0;
    uint8_t payload[512] = {0};
    uint32_t crc = 0;
    bool validCrc = false;
};

struct UM982PAOGIData
{
    uint32_t timeOfFixHms = 0;     // 1
    double latitudeDegrees = 0.0;  // 2,3 ("N" or "S" hemisphere is determined by the sign of the value, positive for N and negative for S)
    double longitudeDegrees = 0.0; // 4,5 ("E" or "W" hemisphere is determined by the sign of the value, positive for E and negative for W)
    uint8_t fixQuality = 0;        // 6
    uint8_t satellites = 0;        // 7
    float hdop = NAN;              // 8
    float altitudeMeters = NAN;    // 9
    float dgpsAgeSeconds = NAN;    // 10
    float speedKnots = NAN;        // 11
    float headingDegrees = NAN;    // 12
    float rollDegrees = NAN;       // 13
    float pitchDegrees = NAN;      // 14
    float yawRateDegPerSec = NAN;  // 15
};

class UM982Parser
{
public:
    UM982Parser();

    void begin(Stream &input, float antennaHeightMeters = NAN);
    void reset();

    bool update();
    bool hasMessage() const;
    const UM982Message &message() const;
    void clearMessage();

    bool decodeAgricToPAOGI(const UM982Message &message, UM982PAOGIData &outData) const;
    static bool formatPAOGISentence(const UM982PAOGIData &data, String &outSentence);

private:
    enum class State
    {
        Sync1,
        Sync2,
        Sync3,
        Header,
        Payload,
        Crc
    };

    static constexpr uint8_t kSync1 = 0xAA;
    static constexpr uint8_t kSync2 = 0x44;
    static constexpr uint8_t kSync3 = 0xB5;
    static constexpr size_t kHeaderLength = 24;
    static constexpr size_t kCrcLength = 4;
    static constexpr size_t kMaxPayload = sizeof(UM982Message::payload);

    void processByte(uint8_t byte);
    bool parseHeader();
    void finalizeMessage();
    void resetToSync();

    static uint16_t readLe16(const uint8_t *data);
    static uint32_t readLe32(const uint8_t *data);
    static float readLeFloat(const uint8_t *data);
    static double readLeDouble(const uint8_t *data);
    static void formatLatLon(double degrees, bool isLatitude, char *outBuffer, size_t outSize, char &hemisphere);
    static void appendFloat(String &target, float value, uint8_t decimals);
    static uint8_t computeNmeaChecksum(const String &sentence);

    Stream *_input = nullptr;
    float _antennaHeightMeters = NAN;

    State _state = State::Sync1;
    size_t _headerIndex = 0;
    size_t _payloadIndex = 0;
    size_t _crcIndex = 0;

    uint8_t _headerBuffer[kHeaderLength] = {0};
    uint8_t _crcBuffer[kCrcLength] = {0};

    UM982Message _message;
    bool _messageReady = false;
};