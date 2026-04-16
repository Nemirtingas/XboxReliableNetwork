#pragma once

#include <mutex>
#include <atomic>
#include <thread>
#include <list>
#include <chrono>
#include <algorithm>

#ifdef _WIN32

#include <WinSock2.h>
#include <iphlpapi.h>
#include <Windows.h>

#else

#include <sys/socket.h>

#endif

#include "XRNNet.h"
#include "XRNUtils.h"
#include "XRNCommonMessage.h"
#include "XRNConnectRequest.h"
#include "XRNConnectAccept.h"
#include "XRNConnectComplete.h"
