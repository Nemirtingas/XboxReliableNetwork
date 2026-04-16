#include "XboxReliableNetwork.h"

bool XRNConnectComplete::Initialize(uint8_t const* buffer, size_t bufferSize, uint16_t protocolVersion)
{
    if (buffer == nullptr)
        return  false;

    auto size = XRNCommonHeader::HeaderSize(buffer, bufferSize);
    if (size + 2 != bufferSize)
        return false;

    if (XRNCommonHeader::MessageType(buffer, bufferSize) != XRNMessageType::ConnectComplete)
        return false;

    if (size < MinimumPacketSize)
        return false;

    this->buffer = buffer;
    this->bufferSize = bufferSize;

    if (protocolVersion <= 0x1FF && (protocolVersion & 0xFF00) != 0x0100 && (protocolVersion & 0x00FC) == 0)
    {
        memset(&reflectedAddress, 0, sizeof(reflectedAddress));
        return true;
    }

    if (Flags() == IPv4Flag)
    {
        if (size < (MinimumPacketSize + sizeof(reflectedAddress.ip4.s_addr) + sizeof(reflectedAddress.port)))
            return false;
    }
    else if (Flags() == IPv6Flag)
    {
        if (size < (MinimumPacketSize + sizeof(reflectedAddress.ip6.s6_addr) + sizeof(reflectedAddress.port)))
            return false;
    }
    else
    {
        return false;
    }

    return true;
}

size_t XRNConnectComplete::WriteHeader(
    uint8_t* buffer,
    size_t bufferSize,
    uint16_t protocolVersion,
    uint32_t linkId,
    uint32_t currentTimestamp,
    XRNAddress const* reflectedAddress)
{
    if (buffer == nullptr)
        return 0;

    if (bufferSize < MinimumPacketSizeOld)
        return 0;

    buffer[TypeOffset] = static_cast<uint8_t>(XRNMessageType::ConnectComplete);
    WriteLongNetworkOrder(buffer + LinkOffset, linkId);
    WriteLongNetworkOrder(buffer + CurrentTimestampOffset, currentTimestamp);

    if (protocolVersion <= 0x1FF && (protocolVersion & 0xFF00) != 0x0100 && (protocolVersion & 0x00FC) == 0)
    {
        WriteShortNetworkOrder(buffer, MinimumPacketSizeOld - 2);
        return MinimumPacketSizeOld;
    }

    if (reflectedAddress == nullptr)
        return 0;

    size_t reflectedAddressOffset = FlagsOffset + 1;

    if (reflectedAddress->family == AF_INET)
    {
        if (bufferSize - MinimumPacketSize < (sizeof(reflectedAddress->ip4.s_addr) + sizeof(reflectedAddress->port)))
            return 0;

        buffer[FlagsOffset] = IPv4Flag;
        memcpy(buffer + reflectedAddressOffset, &reflectedAddress->ip4.s_addr, sizeof(reflectedAddress->ip4.s_addr));
        reflectedAddressOffset += sizeof(reflectedAddress->ip4.s_addr);
    }
    else if (reflectedAddress->family == AF_INET6)
    {
        if (bufferSize - MinimumPacketSize < (sizeof(reflectedAddress->ip6.s6_addr) + sizeof(reflectedAddress->port)))
            return 0;

        buffer[FlagsOffset] = IPv6Flag;
        memcpy(buffer + reflectedAddressOffset, &reflectedAddress->ip6.s6_addr, sizeof(reflectedAddress->ip6.s6_addr));
        reflectedAddressOffset += sizeof(reflectedAddress->ip6.s6_addr);
    }
    else
    {
        return 0;
    }

    memcpy(buffer + reflectedAddressOffset, &reflectedAddress->port, sizeof(reflectedAddress->port));
    reflectedAddressOffset += sizeof(reflectedAddress->port);

    WriteShortNetworkOrder(buffer, reflectedAddressOffset - 2);

    return reflectedAddressOffset;
}