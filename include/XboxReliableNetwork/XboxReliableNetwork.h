#pragma once

#include <mutex>
#include <atomic>
#include <thread>
#include <list>
#include <deque>
#include <chrono>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>
#include <string_view>

#ifdef _WIN32

#include <WinSock2.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <Windows.h>

typedef SOCKET XRNSocketType_t;

#define XRN_INVALID_SOCKET INVALID_SOCKET

#else

#include <sys/socket.h>

typedef int XRNSocketType_t;

#define XRN_INVALID_SOCKET ((int)-1)

#endif

class XRNEndpoint;
class XRNLink;
class XRNOutgoingPacket;
class XRNIncomingPacket;

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

#include "XRNNet.h"
#include "XRNUtils.h"
#include "XRNLog.h"

#include "XRNEvent.h"
#include "XRNAcknowledgeManager.h"
#include "XRNSelectiveAcknowledgeFieldWalker.h"
#include "XRNGapManager.h"
#include "XRNGapFieldWalker.h"
#include "XRNTimer.h"

#include "XRNCommonMessage.h"
#include "XRNConnectRequest.h"
#include "XRNConnectAccept.h"
#include "XRNConnectComplete.h"
#include "XRNConnectDeny.h"
#include "XRNDisconnect.h"
#include "XRNDataAcknowledge.h"
#include "XRNLinkProbe.h"
#include "XRNData.h"

#include "XRNSubPacket.h"
#include "XRNSubPacketChannelCreate.h"
#include "XRNSubPacketChannelTerminate.h"
#include "XRNSubPacketFragmentStart.h"
#include "XRNSubPacketSyncDependency.h"

#include "XRNIncomingPacket.h"
#include "XRNOutgoingPacket.h"

#include "XRNDtlsState.h"
#include "XRNLink.h"
#include "XRNEndpoint.h"