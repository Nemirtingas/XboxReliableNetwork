#pragma once

struct XRNAddress
{
    uint8_t family;
    uint16_t port;
    in_addr ip4;
    in6_addr ip6;
};

struct XRNDefaultChannels
{
    uint16_t channel1;
    uint16_t channel2;
};

inline uint16_t ReadShortHostOrder(const uint8_t* buffer)
{
    uint16_t v;
    memcpy(&v, buffer, sizeof(v));
    return ntohs(v);
}

inline uint32_t ReadLongHostOrder(const uint8_t* buffer)
{
    uint32_t v;
    memcpy(&v, buffer, sizeof(v));
    return ntohl(v);
}

inline void WriteShortNetworkOrder(uint8_t* buffer, uint16_t v)
{
    v = htons(v);
    memcpy(buffer, &v, sizeof(v));
}

inline void WriteLongNetworkOrder(uint8_t* buffer, uint32_t v)
{
    v = htonl(v);
    memcpy(buffer, &v, sizeof(v));
}