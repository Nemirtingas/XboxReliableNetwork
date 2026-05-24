#pragma once

#define XRNM_EVENT_TYPE_ENDPOINT_ADDRESSES_CHANGED 10

struct XRNM_EVENT_ENDPOINT_ADDRESSES_CHANGED
{
	int32_t mCbSize;
	int32_t mEventType;
	XRNM_ENDPOINT_HANDLE mEndpoint;
	void* mEndpointUserData;
};

