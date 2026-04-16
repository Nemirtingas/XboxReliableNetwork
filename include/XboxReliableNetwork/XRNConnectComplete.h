#pragma once


class XRNConnectComplete
{
    static constexpr uint16_t TypeOffset = 2;
    static constexpr uint16_t LinkOffset = 3;
    static constexpr uint16_t CurrentTimestampOffset = 7;
    static constexpr uint16_t FlagsOffset = 11;

    static constexpr uint16_t MinimumPacketSizeOld = 11;
    static constexpr uint16_t MinimumPacketSize = 12;

    static constexpr uint8_t IPv6Flag = 0x01;
    static constexpr uint8_t IPv4Flag = 0x02;

    const uint8_t* buffer = nullptr;
    size_t bufferSize = 0;
    XRNAddress reflectedAddress{};

public:

    inline uint32_t LinkId() const { return ReadLongHostOrder(buffer + LinkOffset); }
    inline uint32_t CurrentTimestamp() const { return ReadLongHostOrder(buffer + CurrentTimestampOffset); }
    inline uint8_t Flags() const { return buffer[FlagsOffset]; }

    bool Initialize(uint8_t const* buffer, size_t bufferSize, uint16_t protocolVersion);

    static size_t WriteHeader(
        uint8_t* buffer,
        size_t bufferSize,
        uint16_t protocolVersion,
        uint32_t linkId,
        uint32_t currentTimestamp,
        XRNAddress const* reflectedAddress);
};