#include "UM982Parser.h"

#include <cmath>

UM982Parser::UM982Parser() = default;

void UM982Parser::begin(Stream &input, float antennaHeightMeters)
{
    _input = &input;
    _antennaHeightMeters = antennaHeightMeters;
    reset();
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

bool UM982Parser::formatPandaSentence(const UM982PandaData &data, String &outSentence)
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
    sentence += "$PANDA,";
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
    _message.header.messageLength = readLe16(&_headerBuffer[6]);
    _message.header.timeRef = _headerBuffer[8];
    _message.header.timeStatus = _headerBuffer[9];
    _message.header.week = readLe16(&_headerBuffer[10]);
    _message.header.msOfWeek = readLe32(&_headerBuffer[12]);
    _message.header.version = readLe32(&_headerBuffer[16]);
    _message.header.reserved = _headerBuffer[20];
    _message.header.leapSeconds = _headerBuffer[21];
    _message.header.delayMs = readLe16(&_headerBuffer[22]);

    _message.payloadLength = _message.header.messageLength;
    if (_message.payloadLength > kMaxPayload)
    {
        return false;
    }

    return true;
}

void UM982Parser::finalizeMessage()
{
    _message.crc = readLe32(_crcBuffer);
    _message.validCrc = false;
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