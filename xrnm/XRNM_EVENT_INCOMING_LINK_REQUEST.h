#pragma once

#define XRNM_EVENT_TYPE_INCOMING_LINK_REQUEST 8


struct XRNM_EVENT_INCOMING_LINK_REQUEST
{
	int32_t mCbSize;
	int32_t mEventType;
	const void* mEndpoint;
	const void* mEndpointUserData;
	XRNM_ADDRESS mInboundAddress;
	const uint8_t* mRequestData;
	uint32_t mRequestDataLength;
	XRNM_DEFAULT_CHANNELS mDefaultReceiveChannels;
};

