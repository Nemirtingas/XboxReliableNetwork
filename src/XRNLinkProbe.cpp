#include "XRNPrivate.h"

bool XRNLinkProbe::Initialize(uint8_t const* buffer, size_t bufferSize, uint16_t remoteProtocolVersion)
{
    if (buffer == nullptr)
        return  false;

    auto size = XRNCommonHeader::HeaderSize(buffer, bufferSize);
    if (size + 2 != bufferSize)
        return false;

    if (XRNCommonHeader::MessageType(buffer, bufferSize) != MessageType)
        return false;

    if (bufferSize < MinimumPacketSize)
        return false;

    auto reversedFields = remoteProtocolVersion < 0x0103;

    uint16_t fieldsOffset = ExtraFieldsOffset;

    if (!reversedFields && buffer[FlagsOffset] & GapFlag)
    {
        if (!XRNGapFieldWalker::Validate(buffer + fieldsOffset, buffer + bufferSize, &gapBufferSize))
            return false;

        gapBuffer = buffer + fieldsOffset;
        fieldsOffset += gapBufferSize;
    }

    if (buffer[FlagsOffset] & SelectiveAcknowledgeFlag)
    {
        if (!XRNSelectiveAcknowledgeFieldWalker::Validate(buffer + fieldsOffset, buffer + bufferSize, &selectiveAcknowledgeBufferSize))
            return false;

        selectiveAcknowledgeBuffer = buffer + fieldsOffset;
        fieldsOffset += selectiveAcknowledgeBufferSize;
    }
    else
    {
        selectiveAcknowledgeBuffer = nullptr;
        selectiveAcknowledgeBufferSize = 0;
    }

    if (reversedFields && buffer[FlagsOffset] & GapFlag)
    {
        if (!XRNGapFieldWalker::Validate(buffer + fieldsOffset, buffer + bufferSize, &gapBufferSize))
            return false;

        gapBuffer = buffer + fieldsOffset;
        fieldsOffset += gapBufferSize;
    }
    else
    {
        gapBuffer = nullptr;
        gapBufferSize = 0;
    }

    this->buffer = buffer;
    this->bufferSize = bufferSize;

    return true;
}

size_t XRNLinkProbe::WriteHeader(
    uint8_t* buffer,
    size_t bufferSize,
    bool bRequest,
    uint16_t wNSPktId,
    uint16_t wCompressedNCD,
    uint32_t dwProbeId,
    XRNAcknowledgeManager* pAcknowledgeManager,
    XRNGapManager* pGapManager)
{
    if (buffer == nullptr || bufferSize < MinimumPacketSize)
        return 0;

    buffer[TypeOffset] = static_cast<uint8_t>(MessageType);
    
    XRNhtons(buffer + NSPacketIdOffset, wNSPktId & NSPacketIdMask);
    XRNhtons(buffer + CompressedNCDOffset, ((wCompressedNCD << 2) & CompressedNCDMask) | (pAcknowledgeManager->LastPacketId() & NextExpectedLossWireIdMask));
    XRNhtonl(buffer + ProbeIdOffset, dwProbeId);

    size_t fieldsOffset = ExtraFieldsOffset;

    if (pGapManager->HasGap())
    {
        buffer[FlagsOffset] |= GapFlag;
        fieldsOffset += pGapManager->BuildGapField(buffer + fieldsOffset, wNSPktId, false);
    }

    if (pAcknowledgeManager->NextExpectedPacketId() != static_cast<uint16_t>(pAcknowledgeManager->LastPacketId())) {
        buffer[FlagsOffset] |= SelectiveAcknowledgeFlag;
        fieldsOffset += pAcknowledgeManager->BuildSackField(buffer + fieldsOffset, 37);
    }

    return fieldsOffset;
}
