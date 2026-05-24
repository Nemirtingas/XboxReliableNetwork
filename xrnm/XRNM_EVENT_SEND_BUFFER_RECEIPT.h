#pragma once

#define XRNM_EVENT_TYPE_SEND_BUFFER_RECEIPT 6

#define XRNM_EVENT_SEND_BUFFER_RECEIPT_DEFAULT_CHANNEL ((uint32_t)0x00000001)
#define XRNM_EVENT_SEND_BUFFER_RECEIPT_00000002 ((uint32_t)0x00000002)

struct XRNM_EVENT_SEND_BUFFER_RECEIPT
{
	int32_t mCbSize;
	int32_t mEventType;
	const void* mLink;
	const void* mLinkUserData;
	uint32_t mChannelId;
	uint32_t field_1C;
	const void* mSendChannelUserData;
	uint32_t mChannelType;
	XRNM_RECEIPT_TYPE mReceiptType;
	int32_t mHResult;
	XRNM_SEND_QUEUE_OPTIONS mQueueOptions;
	const void* mSendUserData;
	int mSendFlags;
	int mChannelFlags;
};

