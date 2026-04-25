#pragma once

class XRNDisconnect
{
    static constexpr uint16_t TypeOffset = 2;
    static constexpr uint16_t LinkIdOffset = 3;

    static constexpr uint16_t DisconnectMagic = 1280;
    static constexpr uint16_t MinimumPacketSize = 7;

    const uint8_t* buffer = nullptr;
    size_t bufferSize = 0;

public:
    static constexpr XRNMessageType MessageType = XRNMessageType::Disconnect;

    inline uint32_t LinkId() const { return XRNntohl(buffer + LinkIdOffset); }

    bool Initialize(uint8_t const* buffer, size_t bufferSize);

    static size_t WriteHeader(
        uint8_t* buffer,
        size_t bufferSize,
        uint32_t linkId);
};