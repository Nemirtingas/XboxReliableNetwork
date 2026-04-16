#include <XboxReliableNetwork/XboxReliableNetwork.h>

bool XRNConnectDeny::Initialize(uint8_t const* buffer, size_t bufferSize)
{
    if (buffer == nullptr)
        return  false;

    auto size = XRNCommonHeader::HeaderSize(buffer, bufferSize);
    if (size + 2 != bufferSize)
        return false;

    if (XRNCommonHeader::MessageType(buffer, bufferSize) != XRNMessageType::ConnectDeny)
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

    reason = ReadLongHostOrder(buffer + ReasonOffset);

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
    uint16_t protocolVersion,
    uint32_t linkId,
    uint32_t reason,
    size_t payloadSize)
{
    if (buffer == nullptr || bufferSize < MinimumPacketSize)
        return 0;

    buffer[TypeOffset] = static_cast<uint8_t>(XRNMessageType::ConnectDeny);
    WriteShortNetworkOrder(buffer + ProtocolOffset, protocolVersion);
    WriteLongNetworkOrder(buffer + LinkIdOffset, linkId);

    if (protocolVersion < 0x0102)
    {
        WriteShortNetworkOrder(buffer, payloadSize + MinimumPacketSizeOld - 2);
        return MinimumPacketSizeOld;
    }

    WriteLongNetworkOrder(buffer + ReasonOffset, reason);

    WriteShortNetworkOrder(buffer, payloadSize + MinimumPacketSize - 2);
    return MinimumPacketSize;
}
