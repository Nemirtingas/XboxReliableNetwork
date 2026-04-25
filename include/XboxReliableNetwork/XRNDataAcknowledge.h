#pragma once

class XRNDataAcknowledge
{
    static constexpr uint16_t TypeOffset = 2;
    static constexpr size_t FlagsOffset = 3;
    static constexpr size_t WireIdOffset = 4;
    static constexpr size_t CompressedNCDOffset = 4;

    static constexpr uint16_t WireIdMask = 0x3FF;
    static constexpr uint16_t CompressedNCDMask = 0xFC00;

	static constexpr uint8_t SelectiveAcknowledgeFlag = 0x01;

    static constexpr uint16_t MinimumPacketSize = 6;

    const uint8_t* buffer = nullptr;
    size_t bufferSize = 0;

    const uint8_t* sackBuffer = nullptr;
    size_t sackSize = 0;

public:
    static constexpr XRNMessageType MessageType = XRNMessageType::DataAck;

    inline uint16_t WireId() const { return XRNntohs(buffer + WireIdOffset) & WireIdMask; }
    inline uint16_t CompressedNCD() const { return (XRNntohs(buffer + CompressedNCDOffset) & CompressedNCDMask) >> 2; }
    inline const uint8_t* SelectiveAcknowledgeBuffer() const { return sackBuffer; }
    inline size_t SelectiveAcknowledgeSize() const { return sackSize; }

    bool Initialize(uint8_t const* buffer, size_t bufferSize);

    static size_t WriteHeader(uint8_t* buffer, size_t bufferSize, uint16_t compressedNCD, const XRNAcknowledgeManager* acknowledgeManager);
};