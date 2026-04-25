#include "XRNPrivate.h"

bool XRNConnectDeny::Initialize(uint8_t const* buffer, size_t bufferSize)
{
    if (buffer == nullptr)
        return  false;

    auto size = XRNCommonHeader::HeaderSize(buffer, bufferSize);
    if (size + 2 != bufferSize)
        return false;

    if (XRNCommonHeader::MessageType(buffer, bufferSize) != MessageType)
        return false;

    if (bufferSize < MinimumPacketSizeOld)
        return false;

    this->buffer = buffer;
    this->bufferSize = bufferSize;

    if (ProtocolVersion() < 0x0102)
    {
        payloadSize = bufferSize - MinimumPacketSize;
        if (payloadSize > 0)
            payload = buffer + MinimumPacketSizeOld;
        else
            payload = nullptr;

        reason = 0x807A1008;
        return true;
    }

    if (bufferSize < MinimumPacketSize)
        return false;

    reason = XRNntohl(buffer + ReasonOffset);

    payloadSize = bufferSize - MinimumPacketSize;
    if (payloadSize > 0)
        payload = buffer + MinimumPacketSize;
    else
        payload = nullptr;

    return true;
}

size_t XRNConnectDeny::WriteHeader(
    uint8_t* buffer,
    size_t bufferSize,
    uint16_t requestProtocolVersion,
    uint16_t responseProtocolVersion,
    uint32_t linkId,
    uint32_t linkDataSize,
    uint32_t reason)
{
    if (buffer == nullptr || bufferSize < MinimumPacketSize)
        return 0;

    buffer[TypeOffset] = static_cast<uint8_t>(MessageType);
    XRNhtonl(buffer + LinkIdOffset, linkId);

    if (requestProtocolVersion < 0x0102)
    {
        XRNhtons(buffer + ProtocolOffset, 1);
        XRNhtons(buffer, MinimumPacketSizeOld - 2);
        return MinimumPacketSizeOld;
    }

    XRNhtons(buffer + ProtocolOffset, responseProtocolVersion);
    XRNhtonl(buffer + ReasonOffset, reason);

    XRNhtons(buffer, linkDataSize + MinimumPacketSize - 2);
    return MinimumPacketSize;
}
