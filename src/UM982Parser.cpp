#include "UM982Parser.h"

#include <cmath>
#include <cstring>

namespace
{
const uint32_t kCrcTable[256] = {
    0x00000000UL, 0x77073096UL, 0xee0e612cUL, 0x990951baUL, 0x076dc419UL, 0x706af48fUL,
    0xe963a535UL, 0x9e6495a3UL, 0x0edb8832UL, 0x79dcb8a4UL, 0xe0d5e91eUL, 0x97d2d988UL,
    0x09b64c2bUL, 0x7eb17cbdUL, 0xe7b82d07UL, 0x90bf1d91UL, 0x1db71064UL, 0x6ab020f2UL,
    0xf3b97148UL, 0x84be41deUL, 0x1adad47dUL, 0x6ddde4ebUL, 0xf4d4b551UL, 0x83d385c7UL,
    0x136c9856UL, 0x646ba8c0UL, 0xfd62f97aUL, 0x8a65c9ecUL, 0x14015c4fUL, 0x63066cd9UL,
    0xfa0f3d63UL, 0x8d080df5UL, 0x3b6e20c8UL, 0x4c69105eUL, 0xd56041e4UL, 0xa2677172UL,
    0x3c03e4d1UL, 0x4b04d447UL, 0xd20d85fdUL, 0xa50ab56bUL, 0x35b5a8faUL, 0x42b2986cUL,
    0xdbbbc9d6UL, 0xacbcf940UL, 0x32d86ce3UL, 0x45df5c75UL, 0xdcd60dcfUL, 0xabd13d59UL,
    0x26d930acUL, 0x51de003aUL, 0xc8d75180UL, 0xbfd06116UL, 0x21b4f4b5UL, 0x56b3c423UL,
    0xcfba9599UL, 0xb8bda50fUL, 0x2802b89eUL, 0x5f058808UL, 0xc60cd9b2UL, 0xb10be924UL,
    0x2f6f7c87UL, 0x58684c11UL, 0xc1611dabUL, 0xb6662d3dUL, 0x76dc4190UL, 0x01db7106UL,
    0x98d220bcUL, 0xefd5102aUL, 0x71b18589UL, 0x06b6b51fUL, 0x9fbfe4a5UL, 0xe8b8d433UL,
    0x7807c9a2UL, 0x0f00f934UL, 0x9609a88eUL, 0xe10e9818UL, 0x7f6a0dbbUL, 0x086d3d2dUL,
    0x91646c97UL, 0xe6635c01UL, 0x6b6b51f4UL, 0x1c6c6162UL, 0x856530d8UL, 0xf262004eUL,
    0x6c0695edUL, 0x1b01a57bUL, 0x8208f4c1UL, 0xf50fc457UL, 0x65b0d9c6UL, 0x12b7e950UL,
    0x8bbeb8eaUL, 0xfcb9887cUL, 0x62dd1ddfUL, 0x15da2d49UL, 0x8cd37cf3UL, 0xfbd44c65UL,
    0x4db26158UL, 0x3ab551ceUL, 0xa3bc0074UL, 0xd4bb30e2UL, 0x4adfa541UL, 0x3dd895d7UL,
    0xa4d1c46dUL, 0xd3d6f4fbUL, 0x4369e96aUL, 0x346ed9fcUL, 0xad678846UL, 0xda60b8d0UL,
    0x44042d73UL, 0x33031de5UL, 0xaa0a4c5fUL, 0xdd0d7cc9UL, 0x5005713cUL, 0x270241aaUL,
    0xbe0b1010UL, 0xc90c2086UL, 0x5768b525UL, 0x206f85b3UL, 0xb966d409UL, 0xce61e49fUL,
    0x5edef90eUL, 0x29d9c998UL, 0xb0d09822UL, 0xc7d7a8b4UL, 0x59b33d17UL, 0x2eb40d81UL,
    0xb7bd5c3bUL, 0xc0ba6cadUL, 0xedb88320UL, 0x9abfb3b6UL, 0x03b6e20cUL, 0x74b1d29aUL,
    0xead54739UL, 0x9dd277afUL, 0x04db2615UL, 0x73dc1683UL, 0xe3630b12UL, 0x94643b84UL,
    0x0d6d6a3eUL, 0x7a6a5aa8UL, 0xe40ecf0bUL, 0x9309ff9dUL, 0x0a00ae27UL, 0x7d079eb1UL,
    0xf00f9344UL, 0x8708a3d2UL, 0x1e01f268UL, 0x6906c2feUL, 0xf762575dUL, 0x806567cbUL,
    0x196c3671UL, 0x6e6b06e7UL, 0xfed41b76UL, 0x89d32be0UL, 0x10da7a5aUL, 0x67dd4accUL,
    0xf9b9df6fUL, 0x8ebeeff9UL, 0x17b7be43UL, 0x60b08ed5UL, 0xd6d6a3e8UL, 0xa1d1937eUL,
    0x38d8c2c4UL, 0x4fdff252UL, 0xd1bb67f1UL, 0xa6bc5767UL, 0x3fb506ddUL, 0x48b2364bUL,
    0xd80d2bdaUL, 0xaf0a1b4cUL, 0x36034af6UL, 0x41047a60UL, 0xdf60efc3UL, 0xa867df55UL,
    0x316e8eefUL, 0x4669be79UL, 0xcb61b38cUL, 0xbc66831aUL, 0x256fd2a0UL, 0x5268e236UL,
    0xcc0c7795UL, 0xbb0b4703UL, 0x220216b9UL, 0x5505262fUL, 0xc5ba3bbeUL, 0xb2bd0b28UL,
    0x2bb45a92UL, 0x5cb36a04UL, 0xc2d7ffa7UL, 0xb5d0cf31UL, 0x2cd99e8bUL, 0x5bdeae1dUL,
    0x9b64c2b0UL, 0xec63f226UL, 0x756aa39cUL, 0x026d930aUL, 0x9c0906a9UL, 0xeb0e363fUL,
    0x72076785UL, 0x05005713UL, 0x95bf4a82UL, 0xe2b87a14UL, 0x7bb12baeUL, 0x0cb61b38UL,
    0x92d28e9bUL, 0xe5d5be0dUL, 0x7cdcefb7UL, 0x0bdbdf21UL, 0x86d3d2d4UL, 0xf1d4e242UL,
    0x68ddb3f8UL, 0x1fda836eUL, 0x81be16cdUL, 0xf6b9265bUL, 0x6fb077e1UL, 0x18b74777UL,
    0x88085ae6UL, 0xff0f6a70UL, 0x66063bcaUL, 0x11010b5cUL, 0x8f659effUL, 0xf862ae69UL,
    0x616bffd3UL, 0x166ccf45UL, 0xa00ae278UL, 0xd70dd2eeUL, 0x4e048354UL, 0x3903b3c2UL,
    0xa7672661UL, 0xd06016f7UL, 0x4969474dUL, 0x3e6e77dbUL, 0xaed16a4aUL, 0xd9d65adcUL,
    0x40df0b66UL, 0x37d83bf0UL, 0xa9bcae53UL, 0xdebb9ec5UL, 0x47b2cf7fUL, 0x30b5ffe9UL,
    0xbdbdf21cUL, 0xcabac28aUL, 0x53b39330UL, 0x24b4a3a6UL, 0xbad03605UL, 0xcdd70693UL,
    0x54de5729UL, 0x23d967bfUL, 0xb3667a2eUL, 0xc4614ab8UL, 0x5d681b02UL, 0x2a6f2b94UL,
    0xb40bbe37UL, 0xc30c8ea1UL, 0x5a05df1bUL, 0x2d02ef8dUL};

uint32_t computeCrc32Part(uint32_t crc, const uint8_t *data, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        crc = kCrcTable[(crc ^ data[i]) & 0xff] ^ (crc >> 8);
    }
    return crc;
}
} // namespace

UM982Parser::UM982Parser() = default;

void UM982Parser::begin(Stream &input, float antennaHeightMeters)
{
    _input = &input;
    _antennaHeightMeters = antennaHeightMeters;
    reset();

    _input->print("UNLOG\r\n");
    _input->print("CONFIG ANTENNA POWERON\r\n");
    _input->print("CONFIG NMEAVERSION V410\r\n");
    _input->print("CONFIG RTK TIMEOUT 600\r\n");
    _input->print("CONFIG RTK RELIABILITY 3 1\r\n");
    _input->print("CONFIG PPP TIMEOUT 300\r\n");
    _input->print("CONFIG SMOOTH RTKHEIGHT 0\r\n");
    _input->print("CONFIG HEADING OFFSET 90.0 0.0\r\n");
    _input->print("CONFIG HEADING RELIABILITY 3\r\n");
    _input->print("CONFIG HEADING TRACTOR\r\n");
    _input->print("CONFIG HEADING VARIABLELENGTH\r\n");
    _input->print("CONFIG DGPS TIMEOUT 600\r\n");
    _input->print("CONFIG RTCMB1CB2A ENABLE\r\n");
    _input->print("CONFIG SMOOTH HEADING 0\r\n");
    _input->print("CONFIG ANTENNADELTAHEN 0.0000 0.0000 0.0000\r\n");
    _input->print("CONFIG SBAS DISABLE\r\n");
    _input->print("CONFIG PPS ENABLE GPS POSITIVE 500000 1000 0 0\r\n");
    _input->print("CONFIG MMP ENABLE\r\n");
    _input->print("CONFIG SIGNALGROUP 4 5\r\n");
    _input->print("CONFIG ANTIJAM AUTO\r\n");
    _input->print("CONFIG AGNSS DISABLE\r\n");
    _input->print("CONFIG COM1 460800\r\n");
    _input->print("CONFIG COM2 460800\r\n");
    _input->print("CONFIG COM3 460800\r\n");
    _input->print("MODE ROVER SURVEY\r\n");
    _input->print("AGRICB 0.1\r\n");
}

void UM982Parser::reset()
{
    _state = State::Sync1;
    _headerIndex = 0;
    _payloadIndex = 0;
    _crcIndex = 0;
    _messageReady = false;
    _message = UM982Message{};
}

bool UM982Parser::update()
{
    if (_input == nullptr)
    {
        return false;
    }

    while (_input->available() > 0)
    {
        const uint8_t byte = static_cast<uint8_t>(_input->read());
        processByte(byte);
        if (_messageReady)
        {
            return true;
        }
    }

    return _messageReady;
}

bool UM982Parser::hasMessage() const
{
    return _messageReady;
}

const UM982Message &UM982Parser::message() const
{
    return _message;
}

void UM982Parser::clearMessage()
{
    _messageReady = false;
}

bool UM982Parser::decodeAgricToPAOGI(const UM982Message &message, UM982PAOGIData &outData) const
{
    if (message.payloadLength < 96)
    {
        return false;
    }

    const uint8_t *payload = message.payload;

    // These are your H+x binary offsets in the docs
    const uint8_t hour = payload[8];
    const uint8_t minute = payload[9];
    const uint8_t second = payload[10];
    outData.timeOfFixHms = static_cast<uint32_t>(hour) * 10000u +
                           static_cast<uint32_t>(minute) * 100u +
                           static_cast<uint32_t>(second);
    outData.fixQuality = payload[11];
    const uint8_t numGps = payload[13];
    const uint8_t numBds = payload[14];
    const uint8_t numGlo = payload[15];
    uint8_t numGal = 0;
    if (message.payloadLength > 224)
    {
        numGal = payload[224];
    }
    outData.satellites = static_cast<uint8_t>(numGps + numBds + numGlo + numGal);

    outData.headingDegrees = readLeFloat(payload + 40);
    outData.pitchDegrees = readLeFloat(payload + 44);
    outData.rollDegrees = readLeFloat(payload + 48);

    const float speedMetersPerSecond = readLeFloat(payload + 52);
    Serial.print("Speed m/s: ");
    Serial.print(speedMetersPerSecond, 3);
    outData.speedKnots = speedMetersPerSecond * 1.943844f;

    outData.latitudeDegrees = readLeDouble(payload + 80);
    outData.longitudeDegrees = readLeDouble(payload + 88);
    outData.altitudeMeters = static_cast<float>(readLeDouble(payload + 96));
    Serial.print(" Height: ");
    Serial.print(outData.altitudeMeters, 3);
    Serial.print(" Antenna height: ");
    Serial.print(_antennaHeightMeters, 3);

    const bool latLonZero = (std::fabs(outData.latitudeDegrees) < 1e-6) && (std::fabs(outData.longitudeDegrees) < 1e-6);
    const bool latLonOutOfRange = (std::fabs(outData.latitudeDegrees) > 90.0) || (std::fabs(outData.longitudeDegrees) > 180.0);
    if ((latLonZero || latLonOutOfRange) && outData.satellites > 0)
    {
        const double latBe = readBeDouble(payload + 80);
        const double lonBe = readBeDouble(payload + 88);
        const float altBe = static_cast<float>(readBeDouble(payload + 96));
        if (std::fabs(latBe) <= 90.0 && std::fabs(lonBe) <= 180.0)
        {
            outData.latitudeDegrees = latBe;
            outData.longitudeDegrees = lonBe;
            outData.altitudeMeters = altBe;
            outData.headingDegrees = readBeFloat(payload + 40);
            outData.pitchDegrees = readBeFloat(payload + 44);
            outData.rollDegrees = readBeFloat(payload + 48);
            const float speedBe = readBeFloat(payload + 52);
            outData.speedKnots = speedBe * 1.943844f;
        }
    }

    outData.hdop = NAN;
    outData.dgpsAgeSeconds = NAN;
    outData.yawRateDegPerSec = NAN;

    if (std::isfinite(_antennaHeightMeters) && _antennaHeightMeters > 0.0f &&
        std::isfinite(outData.rollDegrees) && std::isfinite(outData.headingDegrees) &&
        std::isfinite(outData.latitudeDegrees) && std::isfinite(outData.longitudeDegrees))
    {
        const double rollRad = static_cast<double>(outData.rollDegrees) * (M_PI / 180.0);
        const double headingRad = static_cast<double>(outData.headingDegrees) * (M_PI / 180.0);

        const double lateralOffset = static_cast<double>(_antennaHeightMeters) * std::sin(rollRad);
        const double verticalOffset = static_cast<double>(_antennaHeightMeters) * std::cos(rollRad);

        const double dNorth = -lateralOffset * std::sin(headingRad);
        const double dEast = lateralOffset * std::cos(headingRad);

        constexpr double kEarthRadiusMeters = 6378137.0;
        const double latRad = outData.latitudeDegrees * (M_PI / 180.0);
        const double dLat = dNorth / kEarthRadiusMeters;
        const double dLon = dEast / (kEarthRadiusMeters * std::cos(latRad));

        outData.latitudeDegrees += dLat * (180.0 / M_PI);
        outData.longitudeDegrees += dLon * (180.0 / M_PI);
        if (std::isfinite(outData.altitudeMeters))
        {
            outData.altitudeMeters = static_cast<float>(outData.altitudeMeters - verticalOffset);
        }
    }

    return true;
}

bool UM982Parser::formatPAOGISentence(const UM982PAOGIData &data, String &outSentence)
{
    char latBuffer[16] = {0};
    char lonBuffer[16] = {0};
    char latHem = 'N';
    char lonHem = 'E';
    formatLatLon(data.latitudeDegrees, true, latBuffer, sizeof(latBuffer), latHem);
    formatLatLon(data.longitudeDegrees, false, lonBuffer, sizeof(lonBuffer), lonHem);

    char timeBuffer[8] = {0};
    snprintf(timeBuffer, sizeof(timeBuffer), "%06lu", static_cast<unsigned long>(data.timeOfFixHms));

    String sentence;
    sentence.reserve(120);
    sentence += "$PAOGI,";
    sentence += timeBuffer;
    sentence += ",";
    sentence += latBuffer;
    sentence += ",";
    sentence += latHem;
    sentence += ",";
    sentence += lonBuffer;
    sentence += ",";
    sentence += lonHem;
    sentence += ",";
    sentence += String(data.fixQuality);
    sentence += ",";
    sentence += String(data.satellites);
    sentence += ",";
    appendFloat(sentence, data.hdop, 1);
    sentence += ",";
    appendFloat(sentence, data.altitudeMeters, 1);
    sentence += ",";
    appendFloat(sentence, data.dgpsAgeSeconds, 1);
    sentence += ",";
    appendFloat(sentence, data.speedKnots, 1);
    sentence += ",";
    appendFloat(sentence, data.headingDegrees, 1);
    sentence += ",";
    // you could return this as 8888 to prevent AOG calculating the offset, or better yet antenna height in AOG to 0 (that way roll still shows in AOG)?
    // of course, have to compile antenna height in here temporarily
    // Position.designer.cs #709
    appendFloat(sentence, data.rollDegrees, 1);
    sentence += ",";
    appendFloat(sentence, data.pitchDegrees, 1);
    sentence += ",";
    appendFloat(sentence, data.yawRateDegPerSec, 1);

    const uint8_t checksum = computeNmeaChecksum(sentence);
    char checksumBuffer[6] = {0};
    snprintf(checksumBuffer, sizeof(checksumBuffer), "*%02X", checksum);
    sentence += checksumBuffer;

    outSentence = sentence;
    return true;
}

void UM982Parser::processByte(uint8_t byte)
{
    switch (_state)
    {
    case State::Sync1:
        if (byte == kSync1)
        {
            _headerBuffer[0] = byte;
            _state = State::Sync2;
        }
        break;
    case State::Sync2:
        if (byte == kSync2)
        {
            _headerBuffer[1] = byte;
            _state = State::Sync3;
        }
        else
        {
            resetToSync();
        }
        break;
    case State::Sync3:
        if (byte == kSync3)
        {
            _headerBuffer[2] = byte;
            _headerIndex = 3;
            _state = State::Header;
        }
        else
        {
            resetToSync();
        }
        break;
    case State::Header:
        _headerBuffer[_headerIndex++] = byte;
        if (_headerIndex >= kHeaderLength)
        {
            if (!parseHeader())
            {
                resetToSync();
                return;
            }

            _payloadIndex = 0;
            if (_message.payloadLength == 0)
            {
                _crcIndex = 0;
                _state = State::Crc;
            }
            else
            {
                _state = State::Payload;
            }
        }
        break;
    case State::Payload:
        if (_payloadIndex < kMaxPayload)
        {
            _message.payload[_payloadIndex++] = byte;
            if (_payloadIndex >= _message.payloadLength)
            {
                _crcIndex = 0;
                _state = State::Crc;
            }
        }
        else
        {
            resetToSync();
        }
        break;
    case State::Crc:
        _crcBuffer[_crcIndex++] = byte;
        if (_crcIndex >= kCrcLength)
        {
            finalizeMessage();
            resetToSync();
        }
        break;
    }
}

bool UM982Parser::parseHeader()
{
    if (_headerBuffer[0] != kSync1 || _headerBuffer[1] != kSync2 || _headerBuffer[2] != kSync3)
    {
        return false;
    }

    _message.header.sync1 = _headerBuffer[0];
    _message.header.sync2 = _headerBuffer[1];
    _message.header.sync3 = _headerBuffer[2];
    _message.header.cpuIdle = _headerBuffer[3];
    _message.header.messageId = readLe16(&_headerBuffer[4]);
    const uint16_t lengthLe = readLe16(&_headerBuffer[6]);
    const uint16_t lengthBe = static_cast<uint16_t>(_headerBuffer[6]) << 8 | static_cast<uint16_t>(_headerBuffer[7]);
    _message.header.messageLength = lengthLe;
    _message.header.timeRef = _headerBuffer[8];
    _message.header.timeStatus = _headerBuffer[9];
    _message.header.week = readLe16(&_headerBuffer[10]);
    _message.header.msOfWeek = readLe32(&_headerBuffer[12]);
    _message.header.version = readLe32(&_headerBuffer[16]);
    _message.header.reserved = _headerBuffer[20];
    _message.header.leapSeconds = _headerBuffer[21];
    _message.header.delayMs = readLe16(&_headerBuffer[22]);

    uint16_t payloadLength = _message.header.messageLength;
    if (payloadLength > kMaxPayload && lengthBe <= kMaxPayload)
    {
        payloadLength = lengthBe;
        _message.header.messageLength = lengthBe;
    }

    if (payloadLength > kMaxPayload && payloadLength >= kCrcLength && (payloadLength - kCrcLength) <= kMaxPayload)
    {
        payloadLength = static_cast<uint16_t>(payloadLength - kCrcLength);
    }

    if (payloadLength > kMaxPayload)
    {
        return false;
    }

    _message.payloadLength = payloadLength;

    return true;
}

void UM982Parser::finalizeMessage()
{
    _message.crc = readLe32(_crcBuffer);
    uint32_t crc = 0;
    crc = computeCrc32Part(crc, _headerBuffer, kHeaderLength);
    if (_message.payloadLength > 0)
    {
        crc = computeCrc32Part(crc, _message.payload, _message.payloadLength);
    }
    _message.validCrc = (crc == _message.crc);
    _messageReady = true;
}

void UM982Parser::resetToSync()
{
    _state = State::Sync1;
    _headerIndex = 0;
    _payloadIndex = 0;
    _crcIndex = 0;
}

uint16_t UM982Parser::readLe16(const uint8_t *data)
{
    return static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
}

uint32_t UM982Parser::readLe32(const uint8_t *data)
{
    return static_cast<uint32_t>(data[0]) |
           (static_cast<uint32_t>(data[1]) << 8) |
           (static_cast<uint32_t>(data[2]) << 16) |
           (static_cast<uint32_t>(data[3]) << 24);
}

float UM982Parser::readLeFloat(const uint8_t *data)
{
    float value = 0.0f;
    uint8_t buffer[4] = {data[0], data[1], data[2], data[3]};
    memcpy(&value, buffer, sizeof(value));
    return value;
}

double UM982Parser::readLeDouble(const uint8_t *data)
{
    double value = 0.0;
    uint8_t buffer[8] = {data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]};
    memcpy(&value, buffer, sizeof(value));
    return value;
}

float UM982Parser::readBeFloat(const uint8_t *data)
{
    float value = 0.0f;
    uint8_t buffer[4] = {data[3], data[2], data[1], data[0]};
    memcpy(&value, buffer, sizeof(value));
    return value;
}

double UM982Parser::readBeDouble(const uint8_t *data)
{
    double value = 0.0;
    uint8_t buffer[8] = {data[7], data[6], data[5], data[4], data[3], data[2], data[1], data[0]};
    memcpy(&value, buffer, sizeof(value));
    return value;
}

void UM982Parser::formatLatLon(double degrees, bool isLatitude, char *outBuffer, size_t outSize, char &hemisphere)
{
    if (outBuffer == nullptr || outSize == 0)
    {
        return;
    }

    hemisphere = (isLatitude ? (degrees >= 0.0 ? 'N' : 'S') : (degrees >= 0.0 ? 'E' : 'W'));
    const double absDeg = std::fabs(degrees);
    const int wholeDegrees = static_cast<int>(absDeg);
    const double minutes = (absDeg - static_cast<double>(wholeDegrees)) * 60.0;

    if (isLatitude)
    {
        snprintf(outBuffer, outSize, "%02d%07.4f", wholeDegrees, minutes);
    }
    else
    {
        snprintf(outBuffer, outSize, "%03d%07.4f", wholeDegrees, minutes);
    }
}

void UM982Parser::appendFloat(String &target, float value, uint8_t decimals)
{
    if (std::isnan(value))
    {
        return;
    }

    target += String(value, decimals);
}

uint8_t UM982Parser::computeNmeaChecksum(const String &sentence)
{
    uint8_t checksum = 0;
    bool started = false;
    for (size_t i = 0; i < sentence.length(); ++i)
    {
        const char c = sentence[i];
        if (!started)
        {
            if (c == '$')
            {
                started = true;
            }
            continue;
        }

        if (c == '*')
        {
            break;
        }

        checksum ^= static_cast<uint8_t>(c);
    }

    return checksum;
}