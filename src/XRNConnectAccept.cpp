#include <XboxReliableNetwork/XboxReliableNetwork.h>

bool XRNConnectAccept::Initialize(uint8_t const* buffer, size_t bufferSize)
{
    if (buffer == nullptr)
        return  false;

    auto size = XRNCommonHeader::HeaderSize(buffer, bufferSize);
    if (size + 2 != bufferSize)
        return false;

    if (XRNCommonHeader::MessageType(buffer, bufferSize) != XRNMessageType::ConnectAccept)
        return false;

    if (bufferSize < MinimumPacketSize)
        return false;

    this->buffer = buffer;
    this->bufferSize = bufferSize;

    auto flags = Flags();
    int channelOffset = FlagsOffset + 1;
    if (flags & Channel1Flag)
    {
        defaultChannels.channel1 = ReadShortHostOrder(buffer + channelOffset);
        channelOffset += sizeof(defaultChannels.channel1);
    }
    else
    {
        defaultChannels.channel1 = 0;
    }

    if (flags & Channel2Flag)
    {
        defaultChannels.channel2 = ReadShortHostOrder(buffer + channelOffset);
        channelOffset += sizeof(defaultChannels.channel2);
    }
    else
    {
        defaultChannels.channel2 = 0;
    }

    int reflectedAddressOffset = channelOffset;

    if (flags & IPv4Flag)
    {
        if (bufferSize - reflectedAddressOffset < (sizeof(reflectedAddress.ip4.s_addr) + sizeof(reflectedAddress.port)))
            return false;

        reflectedAddress.family = AF_INET;
        memcpy(&reflectedAddress.ip4.s_addr, buffer + reflectedAddressOffset, sizeof(reflectedAddress.ip4.s_addr));
        reflectedAddressOffset += sizeof(reflectedAddress.ip4.s_addr);
        memcpy(&reflectedAddress.port, buffer + reflectedAddressOffset, sizeof(reflectedAddress.port));
        reflectedAddressOffset += sizeof(reflectedAddress.port);
    }
    else if (flags & IPv6Flag)
    {
        if (bufferSize - reflectedAddressOffset < (sizeof(reflectedAddress.ip6.s6_addr) + sizeof(reflectedAddress.port)))
            return false;

        reflectedAddress.family = AF_INET6;
        memcpy(&reflectedAddress.ip6.s6_addr, buffer + reflectedAddressOffset, sizeof(reflectedAddress.ip6.s6_addr));
        reflectedAddressOffset += sizeof(reflectedAddress.ip6.s6_addr);
        memcpy(&reflectedAddress.port, buffer + reflectedAddressOffset, sizeof(reflectedAddress.port));
        reflectedAddressOffset += sizeof(reflectedAddress.port);
    }
    else
    {
        return false;
    }

    payloadSize = bufferSize - reflectedAddressOffset;
    if (payloadSize > 0)
        payload = buffer + reflectedAddressOffset;
    else
        payload = nullptr;

    return true;
}

size_t XRNConnectAccept::WriteHeader(
    uint8_t* buffer,
    size_t bufferSize,
    uint16_t protocolVersion,
    uint16_t receivePoolSize,
    uint16_t normalAcknowledgePeriod,
    uint16_t lazyAcknowledgePrediod,
    uint32_t echoLinkId,
    uint32_t linkId,
    uint32_t maxNumberSendChannels,
    XRNDefaultChannels const* defaultChannels,
    XRNAddress const* reflectedAddress,
    uint16_t userPayloadSize)
{
    if (buffer == nullptr || bufferSize < MinimumPacketSize || defaultChannels == nullptr || reflectedAddress == nullptr)
        return 0;

    buffer[TypeOffset] = static_cast<uint8_t>(XRNMessageType::ConnectAccept);
    WriteShortNetworkOrder(buffer + ProtocolOffset, protocolVersion);
    WriteShortNetworkOrder(buffer + receivePoolSizeOffset, receivePoolSize);
    WriteShortNetworkOrder(buffer + NormalAcknowledgeOffset, normalAcknowledgePeriod);
    WriteShortNetworkOrder(buffer + LazyAcknowledgeOffset, lazyAcknowledgePrediod);
    WriteLongNetworkOrder(buffer + EchoLinkIdOffset, echoLinkId);
    WriteLongNetworkOrder(buffer + LinkIdOffset, linkId);
    memset(buffer + TimestampDeltaOffset, 0, sizeof(uint32_t));
    memset(buffer + CurrentTimestampOffset, 0, sizeof(uint32_t));
    WriteLongNetworkOrder(buffer + MaxNumberSendChannelsOffset, maxNumberSendChannels);
    uint8_t flags = 0;
    size_t channelOffset = FlagsOffset + 1;
    if (defaultChannels->channel1 != 0)
    {
        if (bufferSize - channelOffset < sizeof(defaultChannels->channel1))
            return 0;

        WriteShortNetworkOrder(buffer + channelOffset, defaultChannels->channel1);
        channelOffset += sizeof(defaultChannels->channel1);
        flags |= Channel1Flag;
    }
    if (defaultChannels->channel2 != 0)
    {
        if (bufferSize - channelOffset < sizeof(defaultChannels->channel2))
            return 0;

        WriteShortNetworkOrder(buffer + channelOffset, defaultChannels->channel2);
        channelOffset += sizeof(defaultChannels->channel2);
        flags |= Channel2Flag;
    }

    size_t reflectedAddressOffset = channelOffset;

    if (reflectedAddress->family == AF_INET)
    {
        if (bufferSize - reflectedAddressOffset < (sizeof(reflectedAddress->ip4.s_addr) + sizeof(reflectedAddress->port)))
            return 0;

        memcpy(buffer + reflectedAddressOffset, &reflectedAddress->ip4.s_addr, sizeof(reflectedAddress->ip4.s_addr));
        reflectedAddressOffset += sizeof(reflectedAddress->ip4.s_addr);

        flags |= IPv4Flag;
    }
    else if (reflectedAddress->family == AF_INET6)
    {
        const bool isIPv4Mapped =
            reflectedAddress->ip6.s6_addr[0] == 0x00 && reflectedAddress->ip6.s6_addr[1] == 0x00 &&
            reflectedAddress->ip6.s6_addr[2] == 0x00 && reflectedAddress->ip6.s6_addr[3] == 0x00 &&
            reflectedAddress->ip6.s6_addr[4] == 0x00 && reflectedAddress->ip6.s6_addr[5] == 0x00 &&
            reflectedAddress->ip6.s6_addr[6] == 0x00 && reflectedAddress->ip6.s6_addr[7] == 0x00 &&
            reflectedAddress->ip6.s6_addr[8] == 0x00 && reflectedAddress->ip6.s6_addr[9] == 0x00 &&
            reflectedAddress->ip6.s6_addr[10] == 0xFF && reflectedAddress->ip6.s6_addr[11] == 0xFF;

        if (isIPv4Mapped)
        {
            if (bufferSize - reflectedAddressOffset < (sizeof(reflectedAddress->ip4.s_addr) + sizeof(reflectedAddress->port)))
                return 0;

            memcpy(buffer + reflectedAddressOffset, &reflectedAddress->ip4.s_addr, sizeof(reflectedAddress->ip4.s_addr));
            reflectedAddressOffset += sizeof(reflectedAddress->ip4.s_addr);

            flags |= IPv4Flag;
        }
        else
        {
            if (bufferSize - reflectedAddressOffset < (sizeof(reflectedAddress->ip6.s6_addr) + sizeof(reflectedAddress->port)))
                return 0;

            memcpy(buffer + reflectedAddressOffset, &reflectedAddress->ip6.s6_addr, sizeof(reflectedAddress->ip6.s6_addr));
            reflectedAddressOffset += sizeof(reflectedAddress->ip6.s6_addr);

            flags |= IPv6Flag;
        }
    }
    else
    {
        return 0;
    }

    memcpy(buffer + reflectedAddressOffset, &reflectedAddress->port, sizeof(reflectedAddress->port));
    reflectedAddressOffset += sizeof(reflectedAddress->port);

    buffer[FlagsOffset] = flags;

    WriteShortNetworkOrder(buffer, reflectedAddressOffset + userPayloadSize - 2);

    return reflectedAddressOffset;
}

bool XRNConnectAccept::RefreshHeader(uint8_t* buffer, size_t bufferSize, uint32_t timestampDelta, uint32_t currentTimestamp)
{
    if (bufferSize < MinimumPacketSize)
        return false;

    WriteLongNetworkOrder(buffer + TimestampDeltaOffset, timestampDelta);
    WriteLongNetworkOrder(buffer + CurrentTimestampOffset, currentTimestamp);

    return true;
}