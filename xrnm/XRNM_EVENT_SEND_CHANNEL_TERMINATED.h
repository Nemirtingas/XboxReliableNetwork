#pragma once

#define XRNM_EVENT_TYPE_SEND_CHANNEL_TERMINATED 4

struct XRNM_EVENT_SEND_CHANNEL_TERMINATED
{
	int32_t mCbSize;
	int32_t mEventType;
	void* mLink;
	void* mLinkUserData;
	uint32_t mExternalChannelId;
	int32_t mPadding1;
	void* mChannelUserData;
	uint32_t mPadding2;
	int32_t mNewStatusInfo;
};

