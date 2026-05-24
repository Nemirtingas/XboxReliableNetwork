#pragma once

#define XRNM_EVENT_TYPE_RECEIVE_CHANNEL_TERMINATED 3

struct XRNM_EVENT_RECEIVE_CHANNEL_TERMINATED
{
	int32_t mCbSize;
	int32_t mEventType;
	XRNM_LINK_HANDLE mLink;
	void* mLinkUserData;
	uint32_t mExternalChannelId;
	int32_t mPadding1;
	const void* mChannelUserData;
	uint32_t mChannelType;
	int32_t mPadding2;
	const uint8_t* mData;
	uint32_t mDataSize;
	int32_t mNewStatusInfo;
};

