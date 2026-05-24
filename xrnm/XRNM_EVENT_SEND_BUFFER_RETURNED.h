#pragma once

#define XRNM_EVENT_TYPE_SEND_BUFFER_RETURNED 5

#define XRNM_EVENT_SEND_BUFFER_RETURNED_DEFAULT_CHANNEL ((uint32_t)0x00000001)
#define XRNM_EVENT_SEND_BUFFER_RETURNED_02000000 ((uint32_t)0x02000000)

struct XRNM_EVENT_SEND_BUFFER_RETURNED
{
	int32_t mCbSize;
	int32_t mEventType;
	const void* mLink;
	const void* mLinkUserData;
	uint32_t mIdChannel;
	uint32_t mPadding1;
	const void* mSendChannelUserData;
	uint32_t mChannelType;
	uint32_t mPadding2;
	const void* mSendUserData;
	uint32_t mBufferType;
	uint32_t mPadding3;
	const void* mDataBuffer;
	uint32_t mDataBufferSize;
	uint32_t mChannelFlags;
};

