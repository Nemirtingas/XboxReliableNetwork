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
	Data = 0x80,
};

class XRNCommonHeader
{


public:
	static inline uint16_t HeaderSize(const uint8_t* buffer, size_t bufferSize)
	{
		if (bufferSize < sizeof(uint16_t))
			return 0;

		return ReadShortHostOrder(buffer);
	}

	static inline XRNMessageType MessageType(const uint8_t* buffer, size_t bufferSize)
	{
		if (bufferSize < (sizeof(uint16_t) + sizeof(XRNMessageType)))
			return XRNMessageType::Invalid;

		return (XRNMessageType)buffer[2];
	}

};