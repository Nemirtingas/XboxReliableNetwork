#pragma once

typedef struct XRNM_SEND_QUEUE_OPTIONS
{
	int32_t sendPriority;
	int32_t cancelValue;
	int32_t timeout;
} XRNM_SEND_QUEUE_OPTIONS;