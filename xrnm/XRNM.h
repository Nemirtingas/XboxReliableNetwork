#pragma once

/* =========================
   Platform detection
   ========================= */
#if defined(_WIN32) || defined(__CYGWIN__)
  #define XRNM_PLATFORM_WINDOWS 1
#else
  #define XRNM_PLATFORM_WINDOWS 0
#endif

/* =========================
   Build configuration
   ========================= */
/*
Define:
- XRNM_STATIC        -> build or use static lib
- XRNM_BUILD_DLL     -> building the DLL (export symbols)
Otherwise            -> using the DLL (import symbols)
*/

/* =========================
   Symbol visibility
   ========================= */
#if XRNM_PLATFORM_WINDOWS

  #if defined(XRNM_STATIC)
    #define XRNM_API
  #elif defined(XRNM_BUILD_DLL)
    #define XRNM_API __declspec(dllexport)
  #else
    #define XRNM_API __declspec(dllimport)
  #endif

#else /* Linux / macOS */

  #if defined(XRNM_STATIC)
    #define XRNM_API
  #else
    #define XRNM_API __attribute__((visibility("default")))
  #endif

#endif

/* =========================
   C linkage
   ========================= */
#ifdef __cplusplus
  #define XRNM_EXTERN_C extern "C"
#else
  #define XRNM_EXTERN_C
#endif

/* =========================
   Final API macro
   ========================= */
#define XRNM_EXTERN_API XRNM_EXTERN_C XRNM_API

#include <cstdint>

typedef struct _XRNM_SECURESOCKETSCACHE_HANDLE* XRNM_SECURESOCKETSCACHE_HANDLE;
typedef struct _XRNM_ENDPOINT_HANDLE *XRNM_ENDPOINT_HANDLE;
typedef struct _XRNM_LINK_HANDLE *XRNM_LINK_HANDLE;
typedef void* XRNM_GENERIC_HANDLE;

// Compatible handles: 
//   - XRNM_ENDPOINT_HANDLE
//   - XRNM_LINK_HANDLE
typedef struct _XRNM_POLL_HANDLE *XRNM_POLL_HANDLE;

#include "XRNM_ERROR.h"
#include "XRNM_ADDRESS.h"
#include "XRNM_ALERT_SETTINGS.h"
#include "XRNM_SEND_BUFFER.h"
#include "XRNM_SEND_QUEUE_OPTIONS.h"
#include "XRNM_DEFAULT_CHANNELS.h"
#include "XRNM_RECEIPT_TYPE.h"
#include "XRNM_OPTION.h"

#include "XRNM_EVENT.h"

#define XRNM_SEND_FLAG_RELIABLE 0x01
#define XRNM_SEND_FLAG_SEQUENTIAL 0x02
#define XRNM_SEND_FLAG_0x04 0x04
#define XRNM_SEND_FLAG_LAZY_ACK 0x08
#define XRNM_SEND_FLAG_0x10 0x10
#define XRNM_SEND_FLAG_0x20 0x20
#define XRNM_SEND_FLAG_COALESCED 0x40

#define XRNM_CHANNEL_TYPE_1 0x7FFFFFFF
#define XRNM_CHANNEL_TYPE_2 0x7EFFFFFF

#define XRNM_DEFAULT_CHANNEL(channelType, channelId) ((channelType) - (channelId))
#define XRNM_USER_CHANNEL(channelType, channelId) (todo)

XRNM_EXTERN_API int32_t XrnmCreateEndpoint(XRNM_SECURESOCKETSCACHE_HANDLE hSecureSocketsCache, void* ulpEndpointUserData, XRNM_ENDPOINT_HANDLE* phEndpoint);
XRNM_EXTERN_API int32_t XrnmAllowInboundLinkRequests(XRNM_ENDPOINT_HANDLE hEndpoint, uint32_t fAllow);
XRNM_EXTERN_API int32_t XrnmCreateOutboundLink(XRNM_ENDPOINT_HANDLE hEndpoint, const XRNM_ADDRESS* pRemoteAddress, const uint8_t* pbyLinkRequestData, uint32_t dwLinkRequestDataSize, XRNM_DEFAULT_CHANNELS* pDefaultSendChannels, void* ulpLinkUserData, XRNM_LINK_HANDLE* phLink);
XRNM_EXTERN_API int32_t XrnmCreateInboundLink(XRNM_EVENT_ENDPOINT_INCOMING_LINK_REQUEST const* pEventInboundLinkRequest, const uint8_t* pbyReplyData, uint32_t dwReplyDataSize, const XRNM_DEFAULT_CHANNELS* pDefaultSendChannels, void* ulpLinkUserData, XRNM_LINK_HANDLE* phLink);
XRNM_EXTERN_API int32_t XrnmTerminateLink(XRNM_LINK_HANDLE hLink);
XRNM_EXTERN_API int32_t XrnmTerminateSendChannel(XRNM_LINK_HANDLE hLink, uint32_t idSendChannel, const XRNM_SEND_BUFFER* pTerminateDataBuffers, uint32_t dwNumTerminateDataBuffers, void* ulpTerminateBuffersUserData, uint32_t dwFlags);
XRNM_EXTERN_API int32_t XrnmSend(XRNM_LINK_HANDLE hLink, uint32_t idSendChannel, const XRNM_SEND_BUFFER* pBuffers, uint32_t dwNumBuffers, const XRNM_SEND_QUEUE_OPTIONS* pSendQueueOptions, void* ulpSendUserData, uint32_t dwFlags);
XRNM_EXTERN_API int32_t XrnmGetEvent(XRNM_POLL_HANDLE hPoll, uint32_t dwTimeout, XRNM_EVENT** ppEvent);
XRNM_EXTERN_API int32_t XrnmReturnEvent(XRNM_EVENT* pEvent);
XRNM_EXTERN_API int32_t XrnmCloseHandle(XRNM_GENERIC_HANDLE hHandle);
XRNM_EXTERN_API int32_t XrnmGetLocalAddressForEndpoint(XRNM_ENDPOINT_HANDLE hEndpoint, XRNM_ADDRESS* pAddress);
XRNM_EXTERN_API int32_t XrnmSetOpt(XRNM_GENERIC_HANDLE h, uint32_t idChannel, XRNM_OPTION Option, const void* pvOptionData, uint32_t dwOptionDataSize);
XRNM_EXTERN_API int32_t XrnmGetOpt(XRNM_GENERIC_HANDLE h, uint32_t idChannel, XRNM_OPTION Option, void* pvOptionData, uint32_t* pdwOptionDataSize);
XRNM_EXTERN_API int32_t XrnmConvertStringToAddress(const wchar_t* szString, XRNM_ADDRESS* pAddress);
XRNM_EXTERN_API int32_t XrnmConvertAddressToString(const XRNM_ADDRESS* pAddress, uint32_t dwMaxBufferChars, wchar_t* szBuffer, uint32_t* pdwCharsNeededOrWritten);
XRNM_EXTERN_API int32_t XrnmCreateSendChannel(XRNM_LINK_HANDLE hLink, uint32_t ChannelType, const XRNM_SEND_BUFFER* pCreateDataBuffers, uint32_t dwNumCreateDataBuffers, void* ulpCreateBuffersUserData, void* ulpChannelUserData, uint32_t dwFlags, uint32_t* pidSendChannel);
XRNM_EXTERN_API int32_t XrnmCreateAddressFromDtlsComponents(const char* szHostname, uint16_t wPort, uint32_t dwCertificateFingerprintSize, const uint8_t* pbyCertificateFingerprint, XRNM_ADDRESS* pAddress);
XRNM_EXTERN_API int32_t XrnmSynchronizeSendChannels(XRNM_LINK_HANDLE hLink, uint32_t* pidSendChannels, uint32_t dwNumSendChannels, uint32_t dwUnblockMask, uint32_t dwUnblockCompareValue, uint32_t dwFlags);
XRNM_EXTERN_API int32_t XrnmSetRemoteAddressAndNatTraversalIdForOutboundLink(XRNM_LINK_HANDLE hLink, XRNM_ADDRESS* pRemoteAddress, uint32_t dwNatTraversalIdSize, const uint8_t* pbyNatTraversalId, uint32_t dwFlags);