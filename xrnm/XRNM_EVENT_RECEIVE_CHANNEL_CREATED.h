#pragma once

#define XRNM_EVENT_TYPE_RECEIVE_CHANNEL_CREATED 2

struct XRNM_EVENT_RECEIVE_CHANNEL_CREATED
{
	int32_t mCbSize;
	int32_t mEventType;
	XRNM_LINK_HANDLE mLink;
	void* mLinkUserData;
	uint32_t mExternalChannelId;
	uint32_t mChannelType;
	const uint8_t* mCreateData;
	uint32_t mCreateDataSize;
};

