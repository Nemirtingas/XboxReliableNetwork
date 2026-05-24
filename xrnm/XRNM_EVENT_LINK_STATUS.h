#pragma once

#define XRNM_EVENT_TYPE_LINK_STATUS 7

struct XRNM_EVENT_LINK_STATUS
{
	int32_t mCbSize;
	int32_t mEventType;
	XRNM_LINK_HANDLE mLink;
	void* mLinkUserData;
	uint32_t mOldStatus;
	uint32_t mNewstatus;
	int32_t mNewStatusInfo;
	int32_t mPadding1;
	const uint8_t* mReplyData;
	int32_t mReplyDataSize;
	XRNM_DEFAULT_CHANNELS mDefaultChannels;
};

