#pragma once


class XRNConnectDeny
{
    static constexpr uint16_t TypeOffset = 2;
    static constexpr uint16_t ProtocolOffset = 3;
    static constexpr uint16_t LinkIdOffset = 5;
    static constexpr uint16_t ReasonOffset = 9;

    static constexpr uint16_t MinimumPacketSizeOld = 9;
    static constexpr uint16_t MinimumPacketSize = 13;

    const uint8_t* buffer = nullptr;
    size_t bufferSize = 0;
    const uint8_t* payload = nullptr;
    size_t payloadSize = 0;
    uint32_t reason = 0;

public:
    static constexpr uint16_t MaximumPacketSize = 13;

    static constexpr XRNMessageType MessageType = XRNMessageType::ConnectDeny;

    inline uint16_t ProtocolVersion() const { return XRNntohs(buffer + ProtocolOffset); }
    inline uint32_t LinkId() const { return XRNntohl(buffer + LinkIdOffset); }
    inline const uint8_t* Payload() const { return payload; }
    inline size_t PayloadSize() const { return payloadSize; }
    inline uint32_t Reason() const { return reason; }

    bool Initialize(uint8_t const* buffer, size_t bufferSize);

    static size_t WriteHeader(
        uint8_t* buffer,
        size_t bufferSize,
        uint16_t requestProtocolVersion,
        uint16_t responseProtocolVersion,
        uint32_t linkId,
        uint32_t linkDataSize,
        uint32_t reason);
};