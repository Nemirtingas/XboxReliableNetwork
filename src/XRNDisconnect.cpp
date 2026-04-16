#include <XboxReliableNetwork/XboxReliableNetwork.h>

bool XRNDisconnect::Initialize(uint8_t const* buffer, size_t bufferSize)
{
    if (buffer == nullptr)
        return  false;

    auto magic = XRNCommonHeader::HeaderSize(buffer, bufferSize);
    if (magic != DisconnectMagic)
        return false;

    if (XRNCommonHeader::MessageType(buffer, bufferSize) != XRNMessageType::Disconnect)
        return false;

    if (bufferSize < MinimumPacketSize)
        return false;

    return true;
}

size_t XRNDisconnect::WriteHeader(
    uint8_t* buffer,
    size_t bufferSize,
    uint32_t linkId)
{
    if (buffer == nullptr || bufferSize < MinimumPacketSize)
        return 0;

    WriteShortNetworkOrder(buffer, DisconnectMagic);
    buffer[TypeOffset] = static_cast<uint8_t>(XRNMessageType::Disconnect);
    WriteLongNetworkOrder(buffer + LinkIdOffset, linkId);
    return MinimumPacketSize;
}
