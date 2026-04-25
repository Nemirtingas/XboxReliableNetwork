#pragma once

class XRNLinkProbe
{
    static constexpr uint16_t TypeOffset = 2;
    static constexpr uint16_t FlagsOffset = 3;
    static constexpr uint16_t NSPacketIdOffset = 3;
    static constexpr uint16_t CompressedNCDOffset = 5;
    static constexpr uint16_t NELWireIdOffset = 5;
    static constexpr uint16_t ProbeIdOffset = 7;
    static constexpr uint16_t ExtraFieldsOffset = 11;

    static constexpr uint16_t GapFlag = 0x20;
    static constexpr uint16_t SelectiveAcknowledgeFlag = 0x40;
    static constexpr uint16_t EmitterFlag = 0x80;

    static constexpr uint8_t FlagsMask = 0xFC;
    static constexpr uint16_t NSPacketIdMask = 0x03FF;
    static constexpr uint8_t CompressedNCDMask = 0xFC;
    static constexpr uint16_t NextExpectedLossWireIdMask = 0x03FF;

    static constexpr uint16_t MinimumPacketSize = 11;

    const uint8_t* buffer = nullptr;
    size_t bufferSize = 0;
    const uint8_t* gapBuffer = nullptr;
    size_t gapBufferSize = 0;
    const uint8_t* selectiveAcknowledgeBuffer = nullptr;
    size_t selectiveAcknowledgeBufferSize = 0;

public:
    static constexpr XRNMessageType MessageType = XRNMessageType::LinkProbe;

    inline uint32_t ProbeId() const { return XRNntohl(buffer + ProbeIdOffset); }
    inline uint8_t Flags() const { return (XRNntohs(buffer + FlagsOffset) & FlagsMask); }
    inline uint8_t NSPacketId() const { return XRNntohs(buffer + NSPacketIdOffset) & NSPacketIdMask; }
    inline uint8_t CompressedNCD() const { return (XRNntohs(buffer + CompressedNCDOffset) & CompressedNCDMask) >> 2; }
    inline uint8_t NextExpectedLossWireId() const { return XRNntohs(buffer + NELWireIdOffset) & NextExpectedLossWireIdMask; }
    inline const uint8_t* SelectiveAckowledgeBuffer() const { return selectiveAcknowledgeBuffer; }
    inline size_t SelectiveAckowledgeBufferSize() const { return selectiveAcknowledgeBufferSize; }
    inline const uint8_t* GapBuffer() const { return gapBuffer; }
    inline size_t GapBufferSize() const { return gapBufferSize; }
    inline bool IsSender() const { return Flags() & EmitterFlag; }

    bool Initialize(uint8_t const* buffer, size_t bufferSize, uint16_t remoteProtocolVersion);

    static size_t WriteHeader(
        uint8_t* buffer,
        size_t bufferSize,
        bool bRequest,
        uint16_t wNSPktId,
        uint16_t wCompressedNCD,
        uint32_t dwProbeId,
        XRNAcknowledgeManager* pAcknowledgeManager,
        XRNGapManager* pGapManager);
};