#pragma once

#define XRNM_EVENT_TYPE_ALERT 9

struct XRNM_EVENT_ALERT
{
	int32_t mCbSize;
	int32_t mEventType;
	const void* mEndpoint;
	const void* mEndpointUserData;
	const void* mLink;
	const void* mLinkUserData;
	uint32_t mIdChannel;
	uint32_t mPadding1;
	const void* mChannelUserData;
	uint32_t mAlertType;
	uint32_t mPadding2;
	uint64_t mAlertValue;
	XRNM_ALERT_SETTINGS mAlertSettings;
	bool mDefaultChannel;
	int32_t mPadding3;
};
