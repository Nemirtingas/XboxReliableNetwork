#include "XRNPrivate.h"

bool XRNData::Initialize(uint8_t const* buffer, size_t bufferSize)
{
    if (bufferSize < 6)
        return 0;

    auto size = XRNCommonHeader::HeaderSize(buffer, bufferSize);
    if (size + 2 != bufferSize)
        return false;

    this->buffer = buffer;
    this->bufferSize = bufferSize;

    m_gapBegin = nullptr;
    m_sackBegin = nullptr;
    m_payloadBegin = nullptr;
    m_gapSize = 0;
    m_sackSize = 0;
    m_encryptedPayloadSize = 0;

    const uint8_t* current = buffer + 6;
    const uint8_t* buffer_end = buffer + size;

    if ((buffer[FlagsOffset] & GapFlag) != 0)
    {
        if (!XRNGapFieldWalker::Validate(current, buffer_end, &m_gapSize))
            return 0;

        m_gapBegin = current;
        current += m_gapSize;
    }

    if ((buffer[FlagsOffset] & SackFlag) != 0)
    {
        if (!XRNSelectiveAcknowledgeFieldWalker::Validate(current, buffer_end, &m_sackSize))
            return false;

        m_sackBegin = current;
        current += m_sackSize;
    }

    size_t offset = current - buffer;

    m_payloadSize = bufferSize - offset;
    
    if (m_payloadSize > 0)
    {
        m_payloadBegin = current;
        m_encryptedPayloadSize = bufferSize - offset;
    }
    else
    {
        if (size > offset)
            return false;

        m_payloadBegin = nullptr;
        m_encryptedPayloadSize = 0;
    }

    return true;
}