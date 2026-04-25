#include "XRNPrivate.h"

void XRNMapIPv4ToIPv6(XRNAddress& ipv6, XRNAddress const& ipv4)
{
	memset(&ipv6, 0, sizeof(ipv6));
	ipv6.family = AF_INET6;
	ipv6.port = ipv4.port;

	ipv6.ip6.s6_addr[10] = 0xFF;
	ipv6.ip6.s6_addr[11] = 0xFF;
	memcpy(&ipv6.ip6.s6_addr[12], &ipv4.ip4.s_addr, sizeof(ipv4.ip4.s_addr));
}

void XRNSockAddrToXRNAddress(XRNAddress& address, sockaddr_storage const& sockstorage)
{
	if (sockstorage.ss_family == AF_INET)
	{
		address.family = AF_INET;

		auto& in4 = (sockaddr_in&)sockstorage;
		address.port = in4.sin_port;
		memcpy(&address.ip4.s_addr, &in4.sin_addr.s_addr, sizeof(address.ip4.s_addr));
	}
	else if (sockstorage.ss_family == AF_INET6)
	{
		address.family = AF_INET6;

		auto& in6 = (sockaddr_in6&)sockstorage;
		address.port = in6.sin6_port;
		memcpy(&address.ip6.s6_addr, &in6.sin6_addr.s6_addr, sizeof(address.ip6.s6_addr));
	}
}

void XRNSockAddrToIPv6(XRNAddress& ipv6, sockaddr_storage const& sockstorage)
{
	memset(&ipv6.ip6, 0, sizeof(ipv6.ip6));
	ipv6.family = AF_INET6;
	ipv6.ip6.s6_addr[10] = 0xFF;
	ipv6.ip6.s6_addr[11] = 0xFF;

	if (sockstorage.ss_family == AF_INET)
	{
		auto& in4 = (sockaddr_in&)sockstorage;
		ipv6.port = in4.sin_port;
		memcpy(&ipv6.ip6.s6_addr[12], &in4.sin_addr.s_addr, sizeof(in4.sin_addr.s_addr));
	}
	else if (sockstorage.ss_family == AF_INET6)
	{
		auto& in6 = (sockaddr_in6&)sockstorage;
		ipv6.port = in6.sin6_port;
		memcpy(&ipv6.ip6.s6_addr, &in6.sin6_addr.s6_addr, sizeof(ipv6.ip6.s6_addr));
	}
}

void XRNAddressToSockAddr(sockaddr_storage& sockstorage, socklen_t& socklength, XRNAddress const& address)
{
	sockstorage.ss_family = address.family;
	if (address.family == AF_INET)
	{
		auto& in4 = (sockaddr_in&)sockstorage;
		in4.sin_port = address.port;
		memcpy(&in4.sin_addr.s_addr, &address.ip4.s_addr, sizeof(address.ip4.s_addr));

		socklength = sizeof(in4);
	}
	else if (address.family == AF_INET6)
	{
		auto& in6 = (sockaddr_in6&)sockstorage;
		in6.sin6_port = address.port;
		memcpy(&in6.sin6_addr.s6_addr, &address.ip6.s6_addr, sizeof(address.ip6.s6_addr));

		socklength = sizeof(in6);
	}
}

XRNSocketType_t XRNCreateSocket(int af, int type, int protocol)
{
	XRNSocketType_t s = socket(af, type, protocol);

#ifdef _WIN32
	if (s == XRN_INVALID_SOCKET)
	{
		if (WSAGetLastError() == WSANOTINITIALISED)
		{
			WSADATA ws2Data{};
			if (WSAStartup(0x0202, &ws2Data) == 0)
				s = socket(af, type, protocol);
		}
	}
#endif

	return s;
}

void XRNCloseSocket(XRNSocketType_t s)
{
#ifdef _WIN32
	closesocket(s);
#else
	close(s);
#endif
}

int XRNSetNonBlocking(XRNSocketType_t s)
{
	u_long v = 1;
	return ioctlsocket(s, FIONBIO, &v);
}