#pragma once

enum XRNMessageType : uint8_t
{
	Invalid = 0x00,
	ConnectRequest = 0x01,
	ConnectAccept = 0x02,
	ConnectComplete = 0x03,
	ConnectDeny = 0x04,
	Disconnect = 0x05,
	DataAck = 0x06,
	LinkProbe = 0x07,
	LastControlType = LinkProbe,
	Data = 0x80,
};

class XRNCommonHeader
{
	static constexpr size_t MessageTypeOffset = 2;

	static constexpr uint8_t ControlTypeMask = 0x7F;
	static constexpr uint8_t DataTypeMask = 0x80;

public:
	static inline uint16_t HeaderSize(const uint8_t* buffer, size_t bufferSize)
	{
		if (bufferSize < sizeof(uint16_t))
			return 0;

		return XRNntohs(buffer);
	}

	static inline XRNMessageType MessageType(const uint8_t* buffer, size_t bufferSize)
	{
		if (bufferSize < (sizeof(uint16_t) + sizeof(XRNMessageType)))
			return XRNMessageType::Invalid;

		if ((buffer[MessageTypeOffset] & DataTypeMask) == static_cast<uint8_t>(XRNMessageType::Data))
			return XRNMessageType::Data;

		auto t = static_cast<uint8_t>(buffer[2] & ControlTypeMask);
		if (t > static_cast<uint8_t>(XRNMessageType::LastControlType))
			return XRNMessageType::Invalid;

		return static_cast<XRNMessageType>(t);
	}

};