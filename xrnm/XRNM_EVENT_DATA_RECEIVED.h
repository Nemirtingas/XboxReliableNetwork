#pragma once

#define XRNM_EVENT_TYPE_DATA_RECEIVED 1

#define XRNM_EVENT_DATA_RECEIVED_FLAG_RELIABLE ((uint32_t)0x01)
#define XRNM_EVENT_DATA_RECEIVED_FLAG_UNSEQUENCED ((uint32_t)0x02)
#define XRNM_EVENT_DATA_RECEIVED_FLAG_DEFAULT_CHANNEL ((uint32_t)0x04)
#define XRNM_EVENT_DATA_RECEIVED_FLAG_FRAGMENT_RECOMPOSED ((uint32_t)0x08)

struct XRNM_EVENT_DATA_RECEIVED
{
	int32_t mCbSize;
	int32_t mEventType;
	XRNM_LINK_HANDLE mLink;
	void* mLinkUserData;
	uint32_t mChannelId;
	uint32_t mPadding1;
	const void* mChannelUserData;
	int mChannelType;
	int mPadding2;
	const uint8_t* mData;
	uint32_t mDataSize;
	uint32_t mChannelFlags;
};

