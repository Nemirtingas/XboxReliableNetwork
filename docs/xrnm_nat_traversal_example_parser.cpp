struct XRNM_ADDRESS
{
    uint8_t reserved[600];
};

#pragma pack(push, 1)

struct XrnmIPv4NatTraversalAddress
{
    uint32_t ipBE;
    uint16_t portBE;
};

struct XrnmIPv6NatTraversalAddress
{
    uint8_t  ip[16];
    uint16_t portBE;
};

#pragma pack(pop)

struct XrnmAddress
{
    uint8_t version;

    uint16_t xboxLiveAddrLen;
    const uint8_t* xboxLiveAddr;

    uint16_t flags;

    uint8_t dtlsPortOffset;

    const char* hostnameString;
    uint8_t hostnameStringLength;

    uint8_t ipv6Count;
    uint8_t ipv4Count;

    const XrnmIPv6NatTraversalAddress* ipv6Endpoints;
    const XrnmIPv4NatTraversalAddress* ipv4Endpoints;

    uint16_t mainPort;

    uint32_t parsedSize;

    const uint8_t* dtlsFingerprint;
};

enum ParseResult : uint32_t
{
    XRNM_OK = 0,
    XRNM_INVALID_VERSION = 0x807A102B,
    XRNM_MALFORMED_PACKET = 0x807A102C,
    XRNM_INVALID_DATA = 0x807A102D,
};

class ByteReader
{
public:
    ByteReader(const uint8_t* data, size_t size)
        : m_start(data), m_ptr(data), m_remaining(size)
    {
    }

    bool ReadU8(uint8_t& out)
    {
        if (m_remaining < 1)
            return false;

        out = *m_ptr;

        ++m_ptr;
        --m_remaining;

        return true;
    }

    bool ReadBE16(uint16_t& out)
    {
        if (m_remaining < 2)
            return false;

        out =
            (uint16_t(m_ptr[0]) << 8) |
            uint16_t(m_ptr[1]);

        m_ptr += 2;
        m_remaining -= 2;

        return true;
    }

    bool ReadBytes(const uint8_t*& out, size_t size)
    {
        if (m_remaining < size)
            return false;

        out = m_ptr;

        m_ptr += size;
        m_remaining -= size;

        return true;
    }

    size_t Remaining() const
    {
        return m_remaining;
    }

    const uint8_t* Start() const
    {
        return m_start;
    }

    const uint8_t* Current() const
    {
        return m_ptr;
    }

private:
    const uint8_t* m_start;
    const uint8_t* m_ptr;
    size_t m_remaining;
};

uint32_t XrnmGetNatTraversalAddressTypeFlagForIpv4Address(const uint32_t* ipv4Addr) {
    uint32_t addr = ntohl(*ipv4Addr);

    // Unspecified 0.0.0.0
    if (addr == 0) return 0;

    // Loopback 127.0.0.0/8
    if ((addr & 0xFF000000) == 0x7F000000) return 0x01;

    // Multicast 224.0.0.0/4
    if ((addr & 0xF0000000) == 0xE0000000) return 0x00;

    // Link-local 169.254.0.0/16
    if ((addr & 0xFFFF0000) == 0xA9FE0000) return 0x02;

    // Private ranges:
    // 10.0.0.0/8
    if ((addr & 0xFF000000) == 0x0A000000) return 0x04;
    // 172.16.0.0/12
    if ((addr & 0xFFF00000) == 0xAC100000) return 0x04;
    // 192.168.0.0/16
    if ((addr & 0xFFFF0000) == 0xC0A80000) return 0x04;

    // Otherwise assume global
    return 0x08;
}

uint32_t XrnmGetNatTraversalAddressTypeFlagForIpv6Address(
    const uint8_t* ipv6Addr,
    bool allowIpv6MappedIpv4
) {
    const uint8_t* addr = ipv6Addr;

    // Link-local fe80::/10
    if ((addr[0] == 0xFE) && ((addr[1] & 0xC0) == 0x80)) {
        return 0x0400; // Link-local
    }

    // Teredo 2001::/32
    if ((addr[0] == 0x20) && (addr[1] == 0x01)) {
        return 0x2000; // Teredo
    }

    // Multicast ff00::/8
    if ((addr[0] & 0xFF) == 0xFF) {
        return 0x0000; // Multicast
    }

    // Loopback ::1
    bool isLoopback = true;
    for (int i = 0; i < 15; ++i) {
        if (addr[i] != 0) {
            isLoopback = false;
            break;
        }
    }
    if (isLoopback && addr[15] == 1) {
        return 0x0100; // Loopback
    }

    // IPv4-mapped ::ffff:a.b.c.d
    if ((addr[0] == 0) && (addr[1] == 0) && (addr[2] == 0) && (addr[3] == 0) &&
        (addr[4] == 0) && (addr[5] == 0) && (addr[6] == 0) && (addr[7] == 0) &&
        (addr[8] == 0) && (addr[9] == 0) && (addr[10] == 0xFF) && (addr[11] == 0xFF)) {
        if (allowIpv6MappedIpv4)
        {
            uint32_t addr4;
            std::memcpy(&addr4, addr + 12, sizeof(addr4));
            return XrnmGetNatTraversalAddressTypeFlagForIpv4Address(&addr4);
        }
        else
        {
            return 0; // IPv4-mapped disallowed
        }
    }

    // Otherwise global/unclassified
    return 0x1000;
}

int32_t XrnmValidateIpv4NatTraversalAddress(XrnmIPv4NatTraversalAddress const* a1)
{
    auto NatTraversalAddressTypeFlagForIpv4Address = XrnmGetNatTraversalAddressTypeFlagForIpv4Address(&a1->ipBE);
    if (NatTraversalAddressTypeFlagForIpv4Address == 0)
        return 0x807A1025;
    
    if (a1->portBE == 0)
        return 0x807A102D;

    return 0;
}

int32_t XrnmValidateIpv6NatTraversalAddress(XrnmIPv6NatTraversalAddress const* a1)
{
    auto NatTraversalAddressTypeFlagForIpv6Address = XrnmGetNatTraversalAddressTypeFlagForIpv6Address(a1->ip, false);
    if (NatTraversalAddressTypeFlagForIpv6Address == 0)
        return 0x807A1025;

    if (a1->portBE == 0)
        return 0x807A102D;

    return 0;
}

int32_t ParseXrnmAddress(
    XrnmAddress& out,
    const uint8_t* data,
    size_t size)
{
    memset(&out, 0, sizeof(out));

    ByteReader br(data, size);

    //----------------------------------------
    // Version
    //----------------------------------------

    if (!br.ReadU8(out.version))
        return XRNM_MALFORMED_PACKET;

    if (out.version < 2)
        return XRNM_INVALID_VERSION;

    //----------------------------------------
    // Xbox Live device address
    //----------------------------------------

    if (!br.ReadBE16(out.xboxLiveAddrLen))
        return XRNM_MALFORMED_PACKET;

    if (out.xboxLiveAddrLen > 300)
        return XRNM_INVALID_DATA;

    if (out.xboxLiveAddrLen)
    {
        if (!br.ReadBytes(
            out.xboxLiveAddr,
            out.xboxLiveAddrLen))
        {
            return XRNM_MALFORMED_PACKET;
        }
    }

    //----------------------------------------
    // Flags
    //----------------------------------------

    if (!br.ReadBE16(out.flags))
        return XRNM_MALFORMED_PACKET;

    //----------------------------------------
    // DTLS port offset
    //----------------------------------------

    if (!br.ReadU8(out.dtlsPortOffset) || (out.dtlsPortOffset + 5) >= size)
        return XRNM_MALFORMED_PACKET;

    //----------------------------------------
    // Hostname string
    //----------------------------------------

    {
        const uint8_t* strData = br.Current();

		// if br.Current() 8 first bytes matches: extrnxss, then skip 8 bytes to read the hostname.
		// extrnxss is not handled in this example.

        size_t len = strnlen(
            reinterpret_cast<const char*>(strData),
            out.dtlsPortOffset);

        if (len == br.Remaining())
            return XRNM_MALFORMED_PACKET;

        out.hostnameString =
            reinterpret_cast<const char*>(strData);

        out.hostnameStringLength = static_cast<uint8_t>(len);

        // advance cursor INCLUDING null terminator
        if (!br.ReadBytes(strData, len + 1))
            return XRNM_MALFORMED_PACKET;
    }

    if ((br.Current() - br.Start()) < (out.dtlsPortOffset + 6))
    {
        //----------------------------------------
        // Packed endpoint counts
        //----------------------------------------

        uint8_t packedCounts;

        if (!br.ReadU8(packedCounts))
            return XRNM_MALFORMED_PACKET;

        out.ipv6Count = (packedCounts >> 4) & 0xF;
        out.ipv4Count = packedCounts & 0xF;

        //----------------------------------------
        // IPv6 endpoints
        //----------------------------------------

        if (out.ipv6Count > 5)
            return XRNM_MALFORMED_PACKET;

        if (out.ipv6Count)
        {
            const uint8_t* ptr;

            size_t bytes =
                out.ipv6Count * sizeof(XrnmIPv6NatTraversalAddress);

            if (!br.ReadBytes(ptr, bytes))
                return XRNM_MALFORMED_PACKET;

            out.ipv6Endpoints =
                reinterpret_cast<const XrnmIPv6NatTraversalAddress*>(ptr);

            //------------------------------------
            // Validation
            //------------------------------------

            for (uint32_t i = 0; i < out.ipv6Count; ++i)
            {
                int32_t err = XrnmValidateIpv6NatTraversalAddress(out.ipv6Endpoints + i);

                if (err)
                    return XRNM_MALFORMED_PACKET;
            }
        }

        //----------------------------------------
        // IPv4 endpoints
        //----------------------------------------

        if (out.ipv4Count > 5)
            return XRNM_MALFORMED_PACKET;

        if (out.ipv4Count)
        {
            const uint8_t* ptr;

            size_t bytes =
                out.ipv4Count * sizeof(XrnmIPv4NatTraversalAddress);

            if (!br.ReadBytes(ptr, bytes))
                return XRNM_MALFORMED_PACKET;

            out.ipv4Endpoints =
                reinterpret_cast<const XrnmIPv4NatTraversalAddress*>(ptr);

            //------------------------------------
            // Validation
            //------------------------------------

            for (uint32_t i = 0; i < out.ipv4Count; ++i)
            {
                int32_t err = XrnmValidateIpv4NatTraversalAddress(out.ipv4Endpoints + i);

                if (err)
                    return XRNM_MALFORMED_PACKET;
            }
        }
    }

    //----------------------------------------
    // Main port
    //----------------------------------------

    if (!br.ReadBE16(out.mainPort))
        return XRNM_MALFORMED_PACKET;

    //----------------------------------------
    // Crypto blob
    //----------------------------------------

    if (static_cast<uint16_t>(br.Remaining()) < 32)
        return XRNM_MALFORMED_PACKET;

    out.dtlsFingerprint = br.Current();

    //----------------------------------------
    // Final parsed size
    //----------------------------------------

    out.parsedSize =
        static_cast<uint32_t>(size);

    return XRNM_OK;
}