#pragma once

#define XRNM_EVENT_TYPE_ENDPOINT_INCOMING_LINK_REQUEST 8


struct XRNM_EVENT_ENDPOINT_INCOMING_LINK_REQUEST
{
	int32_t mCbSize;
	int32_t mEventType;
	XRNM_ENDPOINT_HANDLE mEndpoint;
	void* mEndpointUserData;
	XRNM_ADDRESS mInboundAddress;
	const uint8_t* mRequestData;
	uint32_t mRequestDataLength;
	XRNM_DEFAULT_CHANNELS mDefaultReceiveChannels;
};

