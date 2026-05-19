local bit = bit32 or bit

local band   = bit.band
local rshift = bit.rshift
local lshift = bit.lshift

-- xrnm address
local f_xrnm_address_version = ProtoField.uint8("xrnm.address.version", "Version")
local f_xrnm_address_xbox_live_address_length = ProtoField.uint8("xrnm.address.xbox_live_address_length", "Xbox Live Address Length")
local f_xrnm_address_xbox_live_address = ProtoField.bytes("xrnm.address.xbox_live_address", "Xbox Live Address")
local f_xrnm_address_xbox_thing = ProtoField.uint16("xrnm.address.xbox_thing", "Xbox Thing")
local f_xrnm_address_dtls_port_offset = ProtoField.uint8("xrnm.address.dtls_port_offset", "Dtls Port Offset")
local f_xrnm_address_dtls_hostname = ProtoField.string("xrnm.address.dlts_hostname", "Dtls Hostname")
local f_xrnm_address_ipv4 = ProtoField.bytes("xrnm.address.ipv4", "IPv4 Address")
local f_xrnm_address_ipv6 = ProtoField.bytes("xrnm.address.ipv6", "IPv6 Address")
local f_xrnm_address_ipv4_address_count = ProtoField.uint8("xrnm.address.ipv4.address_count", "IPv4 Address Count", base.DEC, nil, 0x0F)
local f_xrnm_address_ipv6_address_count = ProtoField.uint8("xrnm.address.ipv6.address_count", "IPv6 Address Count", base.DEC, nil, 0xF0)
local f_xrnm_address_ipv4_address = ProtoField.ipv4("xrnm.address.ipv4.address", "Address")
local f_xrnm_address_ipv6_address = ProtoField.ipv6("xrnm.address.ipv6.address", "Address")
local f_xrnm_address_ip_port = ProtoField.uint16("xrnm.address.port", "IP Port")
local f_xrnm_address_dtls_port = ProtoField.uint16("xrnm.address.dtls_port", "DTLS Port")
local f_xrnm_address_dtls_fingerprint = ProtoField.bytes("xrnm.address.dtls_fingerprint", "Dtls Fingerprint")

local M = {}

local function read_c_string(buffer, offset)
    local string_length = 0
    local offset_max = buffer:len() - offset

    while string_length < offset_max do
        if buffer(offset + string_length, 1):uint() == 0 then
            string_length = string_length + 1
            break
        end
        string_length = string_length + 1
    end

    return buffer(offset, string_length):string(), string_length
end

function M.add_fields(proto)

    proto.fields.f_xrnm_address_version = f_xrnm_address_version
    proto.fields.f_xrnm_address_xbox_live_address_length = f_xrnm_address_xbox_live_address_length
    proto.fields.f_xrnm_address_xbox_live_address = f_xrnm_address_xbox_live_address
    proto.fields.f_xrnm_address_xbox_thing = f_xrnm_address_xbox_thing
    proto.fields.f_xrnm_address_dtls_port_offset = f_xrnm_address_dtls_port_offset
    proto.fields.f_xrnm_address_dtls_hostname = f_xrnm_address_dtls_hostname
    proto.fields.f_xrnm_address_ipv4 = f_xrnm_address_ipv4
    proto.fields.f_xrnm_address_ipv6 = f_xrnm_address_ipv6
    proto.fields.f_xrnm_address_ipv4_address_count = f_xrnm_address_ipv4_address_count
    proto.fields.f_xrnm_address_ipv6_address_count = f_xrnm_address_ipv6_address_count
    proto.fields.f_xrnm_address_ipv4_address = f_xrnm_address_ipv4_address
    proto.fields.f_xrnm_address_ipv6_address = f_xrnm_address_ipv6_address
    proto.fields.f_xrnm_address_ip_port = f_xrnm_address_ip_port
    proto.fields.f_xrnm_address_dtls_port = f_xrnm_address_dtls_port
    proto.fields.f_xrnm_address_dtls_fingerprint = f_xrnm_address_dtls_fingerprint

end

function M.create_xrnm_address_item(buffer, offset, tree)
    tree:add(f_xrnm_address_version, buffer(offset, 1))
	offset = offset + 1
	
	local xbox_live_address_length = buffer(offset, 2):uint()
    tree:add(f_xrnm_address_xbox_live_address_length, buffer(offset, 2))
	offset = offset + 2
	
	if xbox_live_address_length > 0 then
        tree:add(f_xrnm_address_xbox_live_address, buffer(offset, xbox_live_address_length))
		offset = offset + xbox_live_address_length
	end
	
	tree:add(f_xrnm_address_xbox_thing, buffer(offset, 2))
	offset = offset + 2
		
    tree:add(f_xrnm_address_dtls_port_offset, buffer(offset, 1))
	offset = offset + 1
	
	local str, string_length = read_c_string(buffer, offset)
	tree:add(f_xrnm_address_dtls_hostname, buffer(offset, string_length))
	offset = offset + string_length
	
	local ip_count = buffer(offset, 1):uint()
	local ipv4_count = band(ip_count, 0x0F)
	local ipv6_count = rshift(band(ip_count, 0xF0), 4)
	
	tree:add(f_xrnm_address_ipv4_address_count, buffer(offset, 1))
	tree:add(f_xrnm_address_ipv6_address_count, buffer(offset, 1))
	offset = offset + 1
	
	for i = 1, ipv6_count do
	    local ipv6_item = tree:add(f_xrnm_address_ipv6, buffer(offset, 18))
	
	    local ipv6_addr = buffer(offset, 16)
		ipv6_item:add(f_xrnm_address_ipv6_address, buffer(offset, 16))
	    offset = offset + 16
		
		local port = buffer(offset, 2)
		ipv6_item:add(f_xrnm_address_ip_port, buffer(offset, 2))
	    offset = offset + 2
		
		ipv6_item:set_text(string.format(
            "IPv6: [%s]:%d",
            ipv6_addr:ipv6(),
            port:uint()
        ))
	end
	
	for i = 1, ipv4_count do
	    local ipv4_item = tree:add(f_xrnm_address_ipv4, buffer(offset, 6))
	
        local ipv4_addr = buffer(offset, 4)
        ipv4_item:add(f_xrnm_address_ipv4_address, ipv4_addr)
        offset = offset + 4
	    
        local port = buffer(offset, 2)
        ipv4_item:add(f_xrnm_address_ip_port, port)
        offset = offset + 2
	    
        ipv4_item:set_text(string.format(
            "IPv4: %s:%d",
            ipv4_addr:ipv4(),
            port:uint()
        ))
	end
	
	tree:add(f_xrnm_address_dtls_port, buffer(offset, 2))
	offset = offset + 2
	
	tree:add(f_xrnm_address_dtls_fingerprint, buffer(offset, 32))
	offset = offset + 32
	
	return offset
end

return M