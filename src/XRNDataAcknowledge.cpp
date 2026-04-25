#include "XRNPrivate.h"

bool XRNDataAcknowledge::Initialize(uint8_t const* buffer, size_t bufferSize)
{
    if (bufferSize < MinimumPacketSize)
        return false;

    if (buffer[TypeOffset] != MessageType)
        return false;

    auto sackPointer = buffer + MinimumPacketSize;
    auto bufferEnd = buffer + bufferSize;

    this->buffer = buffer;
    sackBuffer = nullptr;
    sackSize = 0;
    if ((buffer[FlagsOffset] & SelectiveAcknowledgeFlag) != 0)
    {
        if (!XRNSelectiveAcknowledgeFieldWalker::Validate(sackPointer, bufferEnd, &sackSize))
            return false;

        sackBuffer = sackPointer;
        sackPointer += sackSize;
    }

    if (bufferSize != (sackPointer - buffer))
        return false;

    if (XRNntohs(buffer) + 2 != bufferSize)
        return false;

    return true;
}

size_t XRNDataAcknowledge::WriteHeader(uint8_t* buffer, size_t bufferSize, uint16_t compressedNCD, const XRNAcknowledgeManager* acknowledgeManager)
{
    if (bufferSize < MinimumPacketSize)
        return 0;

    buffer[TypeOffset] = static_cast<uint8_t>(MessageType);

    uint16_t lastPacketId = acknowledgeManager->LastPacketId();
    uint16_t nel = (compressedNCD << 10) | (lastPacketId & WireIdMask);

    buffer[FlagsOffset] = 0;
    XRNhtons(buffer + WireIdOffset, nel);

    uint32_t offset = MinimumPacketSize;

    if (acknowledgeManager->NextExpectedPacketId() != lastPacketId)
    {
        buffer[FlagsOffset] |= SelectiveAcknowledgeFlag;

        offset += acknowledgeManager->BuildSackField(buffer + offset, 0x25);
    }

    XRNhtons(buffer, offset - 2);

    return static_cast<int64_t>(offset);
}