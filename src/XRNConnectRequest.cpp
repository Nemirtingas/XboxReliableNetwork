#include "XboxReliableNetwork.h"

bool XRNConnectRequest::Initialize(uint8_t const* buffer, size_t bufferSize)
{
    if (buffer == nullptr)
        return  false;

    auto size = XRNCommonHeader::HeaderSize(buffer, bufferSize);
    if (size + 2 != bufferSize)
        return false;

    if (XRNCommonHeader::MessageType(buffer, bufferSize) != XRNMessageType::ConnectRequest)
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

    payloadSize = bufferSize - channelOffset;
    if (payloadSize > 0)
        payload = buffer + channelOffset;
    else
        payload = nullptr;

    return true;
}

size_t XRNConnectRequest::WriteHeader(
    uint8_t* buffer,
    size_t bufferSize,
    uint16_t protocolVersion,
    uint16_t receivePoolSize,
    uint16_t normalAcknowledgePeriod,
    uint16_t lazyAcknowledgePrediod,
    uint32_t linkId,
    uint32_t maxNumberSendChannels,
    XRNDefaultChannels const* defaultChannels,
    uint16_t userPayloadSize)
{
    if (buffer == nullptr || bufferSize < MinimumPacketSize || defaultChannels == nullptr)
        return 0;

    buffer[TypeOffset] = static_cast<uint8_t>(XRNMessageType::ConnectRequest);
    WriteShortNetworkOrder(buffer + ProtocolOffset, protocolVersion);
    WriteShortNetworkOrder(buffer + receivePoolSizeOffset, receivePoolSize);
    WriteShortNetworkOrder(buffer + NormalAcknowledgeOffset, normalAcknowledgePeriod);
    WriteShortNetworkOrder(buffer + LazyAcknowledgeOffset, lazyAcknowledgePrediod);
    WriteLongNetworkOrder(buffer + LinkIdOffset, linkId);
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

    buffer[FlagsOffset] = flags;

    WriteShortNetworkOrder(buffer, channelOffset + userPayloadSize - 2);

    return MinimumPacketSize + channelOffset;
}

bool XRNConnectRequest::RefreshHeader(uint8_t* buffer, size_t bufferSize, uint32_t currentTimestamp)
{
    if (bufferSize < (CurrentTimestampOffset + sizeof(currentTimestamp)))
        return false;

    WriteLongNetworkOrder(buffer + CurrentTimestampOffset, currentTimestamp);

    return true;
}