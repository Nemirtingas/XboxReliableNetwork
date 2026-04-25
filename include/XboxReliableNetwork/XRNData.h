#pragma once

class XRNData
{
    static constexpr uint16_t FlagsOffset = 2;
    static constexpr uint16_t AcknowledgeTypeOffset = 2;
    static constexpr uint16_t WireIdOffset = 2;
    static constexpr uint16_t CompressedNCDOffset = 4;
    static constexpr uint16_t NELWireIdOffset = 4;

    static constexpr uint8_t GapFlag = 0x04;
    static constexpr uint8_t SackFlag = 0x08;
    static constexpr uint8_t ReceiptRequiredFlag = 0x10;
    
    static constexpr uint16_t AcknowledgeTypeMask = 0x60;
    static constexpr uint16_t WireIdMask = 0x3FF;
    static constexpr uint16_t NextExpectedLossWireIdMask = 0x3FF;

    const uint8_t* buffer = nullptr;
    size_t bufferSize = 0;

    const uint8_t* m_gapBegin{ nullptr };
    size_t m_gapSize{ 0 };
    const uint8_t* m_sackBegin{ nullptr };
    size_t m_sackSize{ 0 };

    const uint8_t* m_payloadBegin{ nullptr };
    size_t m_payloadSize{ 0 };
    size_t m_encryptedPayloadSize{ 0 };
    

public:
    static constexpr XRNMessageType MessageType = XRNMessageType::Data;

    inline bool ReceiptRequired() const { return (buffer[FlagsOffset] & ReceiptRequiredFlag) == ReceiptRequiredFlag; }
    inline XRNAcknowledgeType AckType() const { return (XRNAcknowledgeType)((buffer[AcknowledgeTypeOffset] & AcknowledgeTypeMask) >> 5); }
    inline uint16_t WireId() const { return XRNntohs(buffer + WireIdOffset) & WireIdMask; }
    inline uint16_t NextExpectedLossWireId() const { return XRNntohs(buffer + NELWireIdOffset) & NextExpectedLossWireIdMask; }
    inline uint16_t CompressedNCD() const { return buffer[CompressedNCDOffset] >> 2; }
    inline const uint8_t* SelectiveAckowledgeBuffer() const { return m_sackBegin; }
    inline size_t SelectiveAckowledgeBufferSize() const { return m_sackSize; }
    inline const uint8_t* GapBuffer() const { return m_gapBegin; }
    inline size_t GapBufferSize() const { return m_gapSize; }
    inline const uint8_t* Payload() const { return m_payloadBegin; }
    inline size_t PayloadSize() const { return m_payloadSize; }
    inline size_t EncryptedPayloadSize() const { return m_encryptedPayloadSize; }

    bool Initialize(uint8_t const* buffer, size_t bufferSize);


};