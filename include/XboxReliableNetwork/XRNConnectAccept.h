#pragma once

class XRNConnectAccept
{
    static constexpr uint16_t MinimumPacketSize = 36;

    static constexpr uint16_t TypeOffset = 2;
    static constexpr uint16_t ProtocolOffset = 3;
    static constexpr uint16_t ReceivePoolSizeOffset = 5;
    static constexpr uint16_t NormalAcknowledgeOffset = 7;
    static constexpr uint16_t LazyAcknowledgeOffset = 9;
    static constexpr uint16_t EchoLinkIdOffset = 11;
    static constexpr uint16_t LinkIdOffset = 15;
    static constexpr uint16_t TimestampDeltaOffset = 19;
    static constexpr uint16_t CurrentTimestampOffset = 23;
    static constexpr uint16_t MaxNumberSendChannelsOffset = 27;
    static constexpr uint16_t FlagsOffset = 31;

    static constexpr uint8_t Channel1Flag = 0x01;
    static constexpr uint8_t Channel2Flag = 0x02;
    static constexpr uint8_t IPv6Flag = 0x40;
    static constexpr uint8_t IPv4Flag = 0x80;

    const uint8_t* buffer = nullptr;
    size_t bufferSize = 0;
    const uint8_t* payload = nullptr;
    size_t payloadSize = 0;
    XRNDefaultChannels defaultChannels{};
    XRNAddress reflectedAddress{};

public:
    static constexpr XRNMessageType MessageType = XRNMessageType::ConnectAccept;

    inline uint16_t ProtocolVersion() const { return XRNntohs(buffer + ProtocolOffset); }
    inline uint16_t ReceivePoolSize() const { return XRNntohs(buffer + ReceivePoolSizeOffset); }
    inline uint16_t NormalAcknowledgePeriod() const { return XRNntohs(buffer + NormalAcknowledgeOffset); }
    inline uint16_t LazyAcknowledgePeriod() const { return XRNntohs(buffer + LazyAcknowledgeOffset); }
    inline uint32_t EchoLinkId() const { return XRNntohl(buffer + EchoLinkIdOffset); }
    inline uint32_t LinkId() const { return XRNntohl(buffer + LinkIdOffset); }
    inline uint32_t TimestampDelta() const { return XRNntohl(buffer + TimestampDeltaOffset); }
    inline uint32_t CurrentTimestamp() const { return XRNntohl(buffer + CurrentTimestampOffset); }
    inline uint32_t MaxNumberSendChannels() const { return XRNntohl(buffer + MaxNumberSendChannelsOffset); }
    inline uint8_t Flags() const { return buffer[FlagsOffset]; }
    inline XRNDefaultChannels DefaultChannels() const { return defaultChannels; }
    inline const uint8_t* Payload() const { return payload; }
    inline const size_t PayloadSize() const { return payloadSize; }

    bool Initialize(uint8_t const* buffer, size_t bufferSize);

    static size_t WriteHeader(
        uint8_t* buffer,
        size_t bufferSize,
        uint16_t protocolVersion,
        uint16_t receivePoolSize,
        uint16_t normalAcknowledgePeriod,
        uint16_t lazyAcknowledgePrediod,
        uint32_t echoLinkId,
        uint32_t linkId,
        uint32_t maxNumberSendChannels,
        XRNDefaultChannels const* defaultChannels,
        XRNAddress const* reflectedAddress,
        uint16_t userPayloadSize);

    static bool RefreshHeader(uint8_t* buffer, size_t bufferSize, uint32_t timestampDelta, uint32_t currentTimestamp);
};