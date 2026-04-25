#pragma once

#define XRN_ETHERNET_MAX_MTU ((uint16_t)1500)
#define XRN_IPV4_HEADER_SIZE ((uint16_t)20)
#define XRN_UDP_HEADER_SIZE ((uint16_t)8)
#define XRN_DTLS_OVERHEAD ((uint16_t)64)

#define XRN_UDP_MAX_PAYLOAD_SIZE (XRN_ETHERNET_MAX_MTU - XRN_IPV4_HEADER_SIZE - XRN_UDP_HEADER_SIZE)
#define XRN_DEFAULT_MTU ((uint16_t)1264)

static constexpr uint16_t XRNProtocolVersion = 0x0105;

struct XRNAddress;

void XRNMapIPv4ToIPv6(XRNAddress& ipv6, XRNAddress const& ipv4);
void XRNSockAddrToXRNAddress(XRNAddress& address, sockaddr_storage const& sockstorage);
void XRNSockAddrToIPv6(XRNAddress& ipv6, sockaddr_storage const& sockstorage);
void XRNAddressToSockAddr(sockaddr_storage& sockstorage, socklen_t& socklength, XRNAddress const& address);

struct XRNAddress
{
    uint8_t family;
    uint16_t port;
    in_addr ip4;
    in6_addr ip6;

    friend bool operator <(XRNAddress const& l, XRNAddress const& r)
    {
        XRNAddress tmpL, tmpR;
        XRNAddress const *pL = &l, *pR = &r;

        if (l.family == AF_INET)
        {
            XRNMapIPv4ToIPv6(tmpL, l);
            pL = &tmpL;
        }
        if (r.family == AF_INET)
        {
            XRNMapIPv4ToIPv6(tmpR, r);
            pR = &tmpR;
        }

        auto result = memcmp(&pL->ip6, &pR->ip6, sizeof(pL->ip6));
        if (result != 0)
            return result;

        return pL->port < pR->port;
    }

    friend bool operator <(XRNAddress const& l, sockaddr_storage const& r)
    {
        XRNAddress tmpL, tmpR;
        XRNAddress const* pL = &l, *pR = &tmpR;

        if (l.family == AF_INET)
        {
            XRNMapIPv4ToIPv6(tmpL, l);
            pL = &tmpL;
        }
        
        XRNSockAddrToIPv6(tmpR, r);

        auto result = memcmp(&pL->ip6, &pR->ip6, sizeof(pL->ip6));
        if (result != 0)
            return result;

        return pL->port < pR->port;
    }
};

struct SHA256Hash_t
{
    uint8_t mHash[32];
};

struct SHA256Fingerprint_t
{
    char mHash[96];
};

struct Certificate_t
{
    char mPrivateKey[4096];
    char mCertificate[4096];
};

struct XRNDefaultChannels
{
    uint16_t channel1;
    uint16_t channel2;
};

enum XRNAcknowledgeType
{
    XRNNoAck = 0,
    XRNLazyAck = 1,
    XRNNormalAck = 2,
    XRNImmediateAck = 3,
};

inline uint16_t XRNntohs(const void* buffer)
{
    uint16_t v;
    memcpy(&v, buffer, sizeof(v));
    return ntohs(v);
}

inline uint32_t XRNntohl(const void* buffer)
{
    uint32_t v;
    memcpy(&v, buffer, sizeof(v));
    return ntohl(v);
}

inline void XRNhtons(void* buffer, uint16_t v)
{
    v = htons(v);
    memcpy(buffer, &v, sizeof(v));
}

inline void XRNhtonl(void* buffer, uint32_t v)
{
    v = htonl(v);
    memcpy(buffer, &v, sizeof(v));
}

XRNSocketType_t XRNCreateSocket(int af, int type, int protocol);
void XRNCloseSocket(XRNSocketType_t s);
int XRNSetNonBlocking(XRNSocketType_t s);