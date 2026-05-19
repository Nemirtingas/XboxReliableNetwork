local bit = bit32 or bit

local band   = bit.band
local rshift = bit.rshift
local lshift = bit.lshift

local xrnm = Proto("xrnm", "XRNM Protocol (DTLS Payload)")

local sessions = {}

local pkt_type_str = {
    [0x01] = "ConnectRequest",
    [0x02] = "ConnectAccept",
    [0x03] = "ConnectComplete",
    [0x04] = "ConnectDeny",
    [0x05] = "Disconnect",
    [0x06] = "Dack",
    [0x07] = "LinkProbe",
    [0x80] = "Data"
}

local data_pkt_type_str = {
    [1] = "Data"
}

local link_probe_type_str = {
    [0] = "Pong",
    [1] = "Ping"
}

local data_ack_type_str = {
    [0] = "None",
    [1] = "Lazy Ack",
    [2] = "Normal Ack",
    [3] = "Immediate Ack"
}

local subpkt_type_str = {
    [1] = "Channel Create",
    [2] = "Channel Terminate",
    [3] = "Fragment",
    [4] = "Sync dependency",
    [0x80] = "Data",
}

local channel_type_str = {
    [0] = "Type 0",
    [1] = "Type 1",
    [2] = "Type 2",
}

-- =========================
-- Fields
-- =========================
local f_length      = ProtoField.uint16("xrnm.length", "Packet Length", base.DEC)
local f_type        = ProtoField.uint8("xrnm.type", "Packet Type", base.HEX, pkt_type_str)

local f_ver_major   = ProtoField.uint8("xrnm.ver.major", "Protocol Version Major", base.DEC)
local f_ver_minor   = ProtoField.uint8("xrnm.ver.minor", "Protocol Version Minor", base.DEC)

local f_recv_pool   = ProtoField.uint16("xrnm.recv_pool", "Recv Pool Size", base.DEC)
local f_ack_period  = ProtoField.uint16("xrnm.ack", "Normal Ack Period", base.DEC)
local f_lazy_ack    = ProtoField.uint16("xrnm.lazy_ack", "Lazy Ack Period", base.DEC)

local f_local_link_id  = ProtoField.uint32("xrnm.link_id", "Local Link ID", base.HEX)
local f_remote_link_id = ProtoField.uint32("xrnm.remote_link_id", "Remote Link ID", base.HEX)
local f_link_id     = ProtoField.uint32("xrnm.link_id", "Link ID", base.HEX)
local f_timestamp   = ProtoField.uint32("xrnm.timestamp", "Current Timestamp", base.HEX)
local f_req_ts_delta= ProtoField.uint32("xrnm.req_ts_delta", "Request Timestamp Delta", base.HEX)
local f_max_channels= ProtoField.uint32("xrnm.max_channels", "Max Send Channels", base.DEC)

local f_flags    = ProtoField.uint8("xrnm.flags", "Flags", base.HEX)

local f_flags_c1 = ProtoField.uint8("xrnm.flags.c1", "C1", base.DEC, nil, 0x01)
local f_flags_c2 = ProtoField.uint8("xrnm.flags.c2", "C2", base.DEC, nil, 0x02)

-- ConnectAccept flags
local f_flags_connect_accept_ipv4 = ProtoField.uint8("xrnm.flags.ipv4", "Reflected IPv4", base.DEC, nil, 0x80)
local f_flags_connect_accept_ipv6 = ProtoField.uint8("xrnm.flags.ipv6", "Reflected IPv6", base.DEC, nil, 0x40)

-- ConnectComplete flags
local f_flags_connect_complete_ipv4 = ProtoField.uint8("xrnm.flags.ipv4", "Reflected IPv4", base.DEC, nil, 0x02)
local f_flags_connect_complete_ipv6 = ProtoField.uint8("xrnm.flags.ipv6", "Reflected IPv6", base.DEC, nil, 0x01)

local f_connect_deny_reason    = ProtoField.uint8("xrnm.connect_deny_reason", "Deny reason", base.HEX)

local f_ch1         = ProtoField.uint16("xrnm.ch1", "Channel 1 Config", base.HEX)
local f_ch2         = ProtoField.uint16("xrnm.ch2", "Channel 2 Config", base.HEX)

local f_reflected_ipv4           = ProtoField.ipv4("xrnm.reflected_ipv4", "Reflected IPv4 Address")
local f_reflected_ipv6           = ProtoField.ipv6("xrnm.reflected_ipv6", "Reflected IPv6 Address")
local f_reflected_port           = ProtoField.uint16("xrnm.reflected_port", "Reflected Port", base.DEC)

-- dack
local f_flags_dack_sack = ProtoField.uint8("xrnm.flags.sack", "Has Selective Acknowledge", base.DEC, nil, 0x01)
local f_dack_ncd_sequence = ProtoField.uint16("xrnm.ncd_sequence", "NCD and Sequence", base.HEX)
local f_dack_ncd = ProtoField.uint16("xrnm.ncd", "NCD", base.DEC, nil, 0xFC00)
local f_dack_sequence = ProtoField.uint16("xrnm.sequence", "Sequence", base.DEC, nil, 0x3FF)

-- link probe
local f_flags_link_probe_type = ProtoField.uint16("xrnm.flags.probe_type", "Probe type", base.DEC, link_probe_type_str, 0x8000)
local f_flags_link_probe_sack = ProtoField.uint16("xrnm.flags.sack", "Has Selective Acknowledge", base.DEC, nil, 0x4000)
local f_flags_link_probe_gap = ProtoField.uint16("xrnm.flags.gap", "Has Gap", base.DEC, nil, 0x2000)
local f_link_probe_wire_id = ProtoField.uint16("xrnm.wire_id_nel", "Wire Id", base.DEC, nil, 0x03FF)
local f_link_probe_payload = ProtoField.uint32("xrnm.probe.payload", "Payload", base.HEX)

-- data
local f_data_pkt_type = ProtoField.uint16("xrnm.type", "Packet Type", base.DEC, data_pkt_type_str, 0x8000)
local f_flags_data_ack_type = ProtoField.uint16("xrnm.flags.ack_type", "Acknowledge type", base.DEC, data_ack_type_str, 0x6000)
local f_flags_data_receipt_ack = ProtoField.uint16("xrnm.flags.receipt", "Wants Receipt Acknowledge", base.DEC, nil, 0x1000)
local f_flags_data_sack = ProtoField.uint16("xrnm.flags.selective_ack", "Has Selective Acknowledge", base.DEC, nil, 0x0800)
local f_flags_data_gap = ProtoField.uint16("xrnm.flags.gap", "Has Gap", base.DEC, nil, 0x0400)
local f_data_wire_id = ProtoField.uint16("xrnm.wire_id", "Wire Id", base.DEC, nil, 0x03FF)

-- subpkt
local f_subpkt_type = ProtoField.uint8("xrnm.subpkt.flags.control_type", "Subpacket Type", base.HEX, subpkt_type_str)
local f_subpkt_data_type_flag = ProtoField.uint8("xrnm.subpkt.flags.data_type", "Is Data Type", base.DEC, nil, 0x80)
local f_subpkt_reliable_flag = ProtoField.uint8("xrnm.subpkt.flags.reliable", "Reliable", base.DEC, nil, 0x40)
local f_subpkt_sequence_flag = ProtoField.uint8("xrnm.subpkt.flags.sequencial", "Has Sequence", base.DEC, nil, 0x20)
local f_subpkt_encrypted_flag = ProtoField.uint8("xrnm.subpkt.flags.encrypted", "Encrypted", base.DEC, nil, 0x10)
local f_subpkt_channel_tiny = ProtoField.uint8("xrnm.subpkt.channel_tiny", "Channel", base.DEC, nil, 0x0F)
local f_subpkt_channel_small = ProtoField.uint8("xrnm.subpkt.channel_small", "Channel", base.DEC)
local f_subpkt_channel_short = ProtoField.uint16("xrnm.subpkt.channel_short", "Channel", base.DEC, nil, 0x3FFF)
local f_subpkt_channel_long = ProtoField.uint32("xrnm.subpkt.channel_long", "Channel", base.DEC, nil, 0x3FFFFFFF)
local f_subpkt_sequence = ProtoField.uint16("xrnm.subpkt.sequence", "Sequence", base.DEC)
local f_subpkt_encrypted_payload_size_short = ProtoField.uint16("xrnm.subpkt.payload_size_short", "Encrypted payload size", base.DEC)
local f_subpkt_encrypted_payload_size_small = ProtoField.uint8("xrnm.subpkt.payload_size_small", "Encrypted payload size", base.DEC)

-- subpkt create channel
local f_subpkt_create_channel_header = ProtoField.uint8("xrnm.subpkt.create_channel.header", "Header", base.HEX)
local f_subpkt_create_channel_sync_dependency_flag = ProtoField.uint8("xrnm.subpkt.create_channel.flags.sync_dependency", "Has sync dependency", base.DEC, nil, 0x80)
local f_subpkt_create_channel_type = ProtoField.uint8("xrnm.subpkt.create_channel.type", "Channel type", base.DEC, channel_type_str, 0x03)
local f_subpkt_create_channel_create_order = ProtoField.uint16("xrnm.subpkt.create_channel.create_order", "Create order", base.DEC)
local f_subpkt_create_channel_payload_size = ProtoField.uint32("xrnm.subpkt.create_channel.payload_size", "Payload size", base.DEC)

-- subpkt terminate channel
local f_subpkt_terminate_channel_create_order = ProtoField.uint16("xrnm.subpkt.terminate_channel.create_order", "Create order", base.DEC)
local f_subpkt_terminate_channel_payload_size = ProtoField.uint32("xrnm.subpkt.terminate_channel.payload_size", "Payload size", base.DEC)

-- subpkt sync dependency
local f_subpkt_sync_dependency_wire_id = ProtoField.uint16("xrnm.subpkt.sync_dependency.wire_id", "Wire Id", base.DEC)

--

local f_ncd = ProtoField.uint16("xrnm.ncd", "NCD", base.DEC, nil, 0xFC00)
local f_nel = ProtoField.uint16("xrnm.nel", "Next Expected Loss", base.DEC, nil, 0x03FF)
local f_sack = ProtoField.string("xrnm.sack", "Selective Acknowledge")
local f_gap = ProtoField.string("xrnm.gap", "Gap")
local f_xrnm_payload = ProtoField.bytes("xrnm.payload", "Payload")


local f_response_in = ProtoField.framenum("xrnm.response_in", "Response In", base.NONE)
local f_response_to = ProtoField.framenum("xrnm.response_to", "Response To", base.NONE)
local f_rtt = ProtoField.double("xrnm.rtt", "RTT (ms)")

xrnm.fields = {
    f_length, f_type,
    f_ver_major, f_ver_minor,
    f_recv_pool, f_ack_period, f_lazy_ack,
    f_local_link_id, f_remote_link_id, f_link_id, f_timestamp, f_req_ts_delta, f_max_channels,
    f_flags,
    f_flags_c1, f_flags_c2,
    f_flags_connect_accept_ipv4, f_flags_connect_accept_ipv6,
    f_flags_connect_complete_ipv4, f_flags_connect_complete_ipv6,
    f_connect_deny_reason,
    f_ch1, f_ch2,
    f_reflected_ipv4, f_reflected_ipv6, f_reflected_port,
    -- dack
    f_flags_dack_sack, f_dack_ncd_sequence, f_dack_ncd, f_dack_sequence,
    -- link probe
    f_flags_link_probe_type, f_flags_link_probe_sack, f_flags_link_probe_gap, f_link_probe_wire_id,
    f_link_probe_payload,
    -- data
    f_data_pkt_type, f_flags_data_ack_type, f_flags_data_receipt_ack, f_flags_data_sack, f_flags_data_gap, f_data_wire_id,
    -- subpkt
    f_subpkt_type, f_subpkt_data_type_flag, f_subpkt_reliable_flag, f_subpkt_sequence_flag, f_subpkt_encrypted_flag,
    f_subpkt_channel_tiny, f_subpkt_channel_small, f_subpkt_channel_short, f_subpkt_channel_long,
    f_subpkt_sequence, f_subpkt_encrypted_payload_size_short, f_subpkt_encrypted_payload_size_small,
    -- subpkt create channel
    f_subpkt_create_channel_header, f_subpkt_create_channel_sync_dependency_flag, f_subpkt_create_channel_type, f_subpkt_create_channel_create_order, f_subpkt_create_channel_payload_size,
    -- subpkt terminate channel
    f_subpkt_terminate_channel_create_order, f_subpkt_terminate_channel_payload_size,
    -- subpkt fragment
    f_subpkt_fragment_size,
    -- subpkt sync dependency
    f_subpkt_sync_dependency_wire_id,
    --
    f_ncd, f_nel,
    f_sack, f_gap,
    f_xrnm_payload,
	f_response_in, f_response_to, f_rtt
}

local function decode_pkt_type(v)
    if band(v, 0x80) ~= 0 then
        return 0x80
    end

    local base = band(v, 0x7F)
    if base >= 8 then
        return 0
    end

    return base
end

local function get_flow_key(pinfo)

    local a = tostring(pinfo.src) .. ":" .. tostring(pinfo.src_port)
    local b = tostring(pinfo.dst) .. ":" .. tostring(pinfo.dst_port)

    if a < b then
        return a .. "|" .. b
    else
        return b .. "|" .. a
    end
end

local function get_or_create_xrnm_session(key)
    if not sessions[key] then
        sessions[key] = { ping_map = {} }
    end
    
	return sessions[key]
end

local function get_xrnm_session(key)
    return sessions[key]
end

local function parse_sack(buffer, offset, ack_seq, subtree)

    local sack_tree = subtree:add(f_sack, "SACK Field")

    local base = ack_seq + 1
    local index = 0

    local max_len = math.min(buffer:len() - offset, 37)

    while index < max_len do

        local b = buffer(offset + index, 1):uint()

        local is_last = band(b, 0x80) ~= 0
        local bits = band(b, 0x7F)

        local byte_tree = sack_tree:add(
            string.format("SACK byte %d (base %d)", index, base + index * 7)
        )

        -- TODO
        -- for i = 0, 6 do
        --     local pkt_id = base + index * 7 + i
        --     local mask = lshift(1, 6 - i)
        -- 
        --     local received = band(bits, mask) ~= 0
        -- 
        --     byte_tree:add(string.format(
        --         "Packet %d: %s",
        --         pkt_id,
        --         received and "✓" or "✗"
        --     ))
        -- end

        index = index + 1

        if is_last then
            byte_tree:append_text(" (LAST)")
            break
        end
    end

    return index
end

local function parse_gap(buffer, offset, tree)

    local gap_tree = tree:add(f_gap, "GAP Field")

    local index = 0

    local max_len = math.min(buffer:len() - offset, 37)

    while index < max_len do

        local b = buffer(offset + index, 1):uint()

        local is_last = band(b, 0x80) ~= 0
        local bits = band(b, 0x7F)

        local byte_tree = gap_tree:add(
            string.format("GAP byte %d (0x%02X)", i, b)
        )

        -- TODO
        -- for b = 0, 6 do
        --     local mask = lshift(1, b)
        --     local received = band(b, mask) ~= 0
        -- 
        --     byte_tree:add(string.format(
        --         "Bit %d: %s",
        --         b,
        --         received and "1" or "0"
        --     ))
        -- end

        index = index + 1

        if is_last then
            byte_tree:append_text(" (LAST)")
            break
        end
    end

    return index
end

-- =========================
-- Parser
-- =========================
local function subpacket_common_header_handler(buffer, subtree, flags_offset, channel_size, channel_offset, sequence_offset, sequence_value, payload_offset, payload_size, payload_value, subpkt_type_offset, subpkt_type)
    local header_item = subtree:add("Type/Flags/Channel Id", buffer(flags_offset, 1))
    header_item:add(f_subpkt_data_type_flag, buffer(flags_offset, 1))
    header_item:add(f_subpkt_reliable_flag, buffer(flags_offset, 1))
    header_item:add(f_subpkt_sequence_flag, buffer(flags_offset, 1))
    header_item:add(f_subpkt_encrypted_flag, buffer(flags_offset, 1))
    header_item:add(f_subpkt_channel_tiny, buffer(flags_offset, 1))
    
    if channel_size == 1 then
        header_item:add(f_subpkt_channel_small, buffer(channel_offset, channel_size))
    elseif channel_size == 2 then
        header_item:add(f_subpkt_channel_short, buffer(channel_offset, channel_size))
    elseif channel_size == 4 then
        header_item:add(f_subpkt_channel_long, buffer(channel_offset, channel_size))
    end
    
    if sequence_offset ~= 0 then
        subtree:add(f_subpkt_sequence, buffer(sequence_offset, 2))
    elseif sequence_value ~= 0 then
        subtree:add(f_subpkt_sequence, sequence_value)
    end
    
    if payload_size == 2 then
        subtree:add(f_subpkt_encrypted_payload_size_short, buffer(payload_offset, payload_size))
    elseif payload_size == 1 then
        subtree:add(f_subpkt_encrypted_payload_size_small, buffer(payload_offset, payload_size))
            :append_text(string.format(" (%d)", payload_value))
    end
    
    if subpkt_type_offset ~= 0 then
        subtree:add(f_subpkt_type, buffer(subpkt_type_offset, 1))
    else
        subtree:add(f_subpkt_type, subpkt_type)
    end
end

local function subpacket_channel_create_dissector(buffer, subpkt_start_offset, offset, pinfo, tree, flags_offset, channel_size, channel_offset, sequence_offset, sequence_value, payload_offset, payload_size, payload_value, subpkt_type_offset, subpkt_type)
    local subtree = tree:add(xrnm, buffer(subpkt_start_offset), "XRNM SubPacket (Create Channel)")
    
    subpacket_common_header_handler(buffer, subtree, flags_offset, channel_size, channel_offset, sequence_offset, sequence_value, payload_offset, payload_size, payload_value, subpkt_type_offset, subpkt_type)
    
    local header_item = subtree:add(f_subpkt_create_channel_header, buffer(offset, 1))
    header_item:add(f_subpkt_create_channel_sync_dependency_flag, buffer(offset, 1))
    header_item:add(f_subpkt_create_channel_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add(f_subpkt_create_channel_create_order, buffer(offset, 2))
    offset = offset + 2
    
    local subpkt_payload_size = buffer(offset, 4):uint()
    subtree:add(f_subpkt_create_channel_payload_size, buffer(offset, 4))
    offset = offset + 4
    
    -- =========================
    -- Payload
    -- =========================

    if subpkt_payload_size ~= 0 then
        local payload = buffer(offset, subpkt_payload_size)
        subtree:add(f_xrnm_payload, payload):set_text("Payload (" .. payload:len() .. " bytes)")
        offset = offset + subpkt_payload_size
        
        local playfab = Dissector.get("playfab")
        if playfab then
            playfab:call(payload:tvb(), pinfo, tree)
        end
    end
    
    return offset
end

local function subpacket_channel_terminate_dissector(buffer, subpkt_start_offset, offset, pinfo, tree, flags_offset, channel_size, channel_offset, sequence_offset, sequence_value, payload_offset, payload_size, payload_value, subpkt_type_offset, subpkt_type)
    local subtree = tree:add(xrnm, buffer(subpkt_start_offset), "XRNM SubPacket (Terminate Channel)")
    
    subpacket_common_header_handler(buffer, subtree, flags_offset, channel_size, channel_offset, sequence_offset, sequence_value, payload_offset, payload_size, payload_value, subpkt_type_offset, subpkt_type)
    
    subtree:add(f_subpkt_terminate_channel_create_order, buffer(offset, 2))
    offset = offset + 2
    
    local subpkt_payload_size = buffer(offset, 4):uint()
    subtree:add(f_subpkt_terminate_channel_payload_size, buffer(offset, 4))
    offset = offset + 4
    
    -- =========================
    -- Payload
    -- =========================

    if subpkt_payload_size ~= 0 then
        local payload = buffer(offset, subpkt_payload_size)
        subtree:add(f_xrnm_payload, payload):set_text("Payload (" .. payload:len() .. " bytes)")
        offset = offset + subpkt_payload_size
        
        local playfab = Dissector.get("playfab")
        if playfab then
            playfab:call(payload:tvb(), pinfo, tree)
        end
    end
    
    return offset
end

local function subpacket_sync_dependency_dissector(buffer, subpkt_start_offset, offset, pinfo, tree, flags_offset, channel_size, channel_offset, sequence_offset, sequence_value, payload_offset, payload_size, payload_value, subpkt_type_offset, subpkt_type)
    local subtree = tree:add(xrnm, buffer(subpkt_start_offset), "XRNM SubPacket (Sync Dependency)")

    subpacket_common_header_handler(buffer, subtree, flags_offset, channel_size, channel_offset, sequence_offset, sequence_value, payload_offset, payload_size, payload_value, subpkt_type_offset, subpkt_type)

    subtree:add(f_subpkt_sync_dependency_wire_id, buffer(offset, 2))
    offset = offset + 2

    return offset
end

local function subpacket_data_dissector(buffer, subpkt_start_offset, offset, pinfo, tree, flags_offset, channel_size, channel_offset, sequence_offset, sequence_value, payload_offset, payload_size, payload_value, subpkt_type_offset, subpkt_type)
    local subtree = tree:add(xrnm, buffer(subpkt_start_offset), "XRNM SubPacket (Data)")

    subpacket_common_header_handler(buffer, subtree, flags_offset, channel_size, channel_offset, sequence_offset, sequence_value, payload_offset, payload_size, payload_value, subpkt_type_offset, subpkt_type)

    -- =========================
    -- Payload
    -- =========================

    local payload = buffer(offset, payload_value)
    subtree:add(f_xrnm_payload, payload):set_text("Payload (" .. payload:len() .. " bytes)")
    offset = offset + payload_value
    
    local playfab = Dissector.get("playfab")
    if playfab then
        playfab:call(payload:tvb(), pinfo, tree)
    end
    
    return offset
end

local subpkt_handlers = {}
subpkt_handlers[0x01] = subpacket_channel_create_dissector
subpkt_handlers[0x02] = subpacket_channel_terminate_dissector
--subpkt_handlers[0x03] = subpacket_fragment_dissector
subpkt_handlers[0x04] = subpacket_sync_dependency_dissector
subpkt_handlers[0x80] = subpacket_data_dissector

local function subpacket_dissector(buffer, offset, pinfo, tree)
    local parsed_subpkt = {}

    while offset < buffer:len() do
        local header = buffer(offset, 1):uint()
        
        local subpkt_start_offset = offset
        local flags_offset = offset
        local channel_offset = 0
        local channel_size = 0
        local sequence_offset = 0
        local sequence_value = 0
        local payload_offset = 0
        local payload_size = 0
        local payload_value = 0
        local subpkt_type_offset = 0
        local subpkt_type = 0x80
        
        local channel_id = 0
        
        if band(header, 0x0F) then
            channel_id = band(buffer(offset, 1):uint(), 0x0f)
            offset = offset + 1
        else
            offset = offset + 1
            channel_offset = offset
            local packedIntType = buffer(offset, 1):uint()
            if band(packedIntType, 0xC0) == 0xC0 then
                channel_id = band(buffer(offset, 4), 0x3FFFFFFF)
                channel_size = 4
                offset = offset + 4
            elseif band(packedIntType, 0x80) == 0x80 then
                channel_id = band(buffer(offset, 2), 0x3FFF)
                channel_size = 2
                offset = offset + 2
            else
                channel_id = buffer(offset, 1)
                channel_size = 1
                offset = offset + 1
            end
        end
        
        local is_data_subpkt = band(header, 0x80) ~= 0
        local has_sequence = band(header, 0x20) ~= 0
        local has_encrypted_payload = band(header, 0x10) ~= 0
        
        if has_sequence then
            local found = false
            for i = #parsed_subpkt, 1, -1 do
                local subpkt = parsed_subpkt[i]
            
                if subpkt.channel_id == channel_id then
                    sequence_value = subpkt.sequence_value + 1
                    found = true
                end
            end
            
            if not found then
                sequence_offset = offset
                sequence_value = buffer(offset, 2):uint()
                offset = offset + 2
            end
            
            table.insert(parsed_subpkt, {
                sequence_value = sequence_value,
                channel_id = channel_id
            })
        end
        
        payload_offset = offset
        if has_encrypted_payload then
            payload_value = buffer(offset, 2):uint()
            payload_size = 2
            offset = offset + 2
        else
            payload_value = buffer(offset, 1):uint() * 2
            payload_size = 1
            offset = offset + 1
        end
        
        if not is_data_subpkt then
            subpkt_type_offset = offset
            subpkt_type = buffer(offset, 1):uint()
            offset = offset + 1
        end
        
        local handler = subpkt_handlers[subpkt_type]
        if not handler then
            return false
        end
        
        if payload_value == 0 then
            payload_value = buffer:len() - offset
        end
        
        offset = handler(buffer, subpkt_start_offset, offset, pinfo, tree, flags_offset, channel_size, channel_offset, sequence_offset, sequence_value, payload_offset, payload_size, payload_value, subpkt_type_offset, subpkt_type)
    end
    
    return true
end


local function connect_request_dissector(buffer, pinfo, tree)
    if buffer:len() < 18 then return false end

    local xrnm_session = get_or_create_xrnm_session(get_flow_key(pinfo))

    local subtree = tree:add(xrnm, buffer(), "XRNM (ConnectRequest)")

    local offset = 0

    -- Length
    subtree:add(f_length, buffer(offset, 2))
    offset = offset + 2

    -- Type
    local raw_type = buffer(offset, 1):uint()
    local pkt_type = decode_pkt_type(raw_type)
    subtree:add(f_type, buffer(offset, 1))
    offset = offset + 1
    
    -- Version
    subtree:add(f_ver_major, buffer(offset, 1))
    subtree:add(f_ver_minor, buffer(offset + 1, 1))
    
    xrnm_session.xrnm_major = buffer(offset, 1):uint()
    xrnm_session.xrnm_minor = buffer(offset, 1):uint()
    
    offset = offset + 2

    -- Fixed fields
    subtree:add(f_recv_pool, buffer(offset, 2))
    offset = offset + 2

    subtree:add(f_ack_period, buffer(offset, 2))
    offset = offset + 2

    subtree:add(f_lazy_ack, buffer(offset, 2))
    offset = offset + 2

    subtree:add(f_local_link_id, buffer(offset, 4))
    
    xrnm_session.remote_link_id = buffer(offset, 4):uint()
    
    offset = offset + 4

    subtree:add(f_timestamp, buffer(offset, 4))
    offset = offset + 4

    subtree:add(f_max_channels, buffer(offset, 4))
    offset = offset + 4

    -- Channels
    local flags = buffer(offset, 1):uint()
    local c1 = band(flags, 0x01) ~= 0
    local c2 = band(flags, 0x02) ~= 0

    local flags_item = subtree:add(f_flags, buffer(offset,1))
    flags_item:add(f_flags_c1, buffer(offset, 1))
    flags_item:add(f_flags_c2, buffer(offset, 1))
    offset = offset + 1

    -- Optional channel fields
    if c1 then
        subtree:add(f_ch1, buffer(offset, 2))
        offset = offset + 2
    end

    if c2 then
        subtree:add(f_ch2, buffer(offset, 2))
        offset = offset + 2
    end

    -- Payload
    if offset < buffer:len() then
        local payload = buffer(offset)
        subtree:add(f_xrnm_payload, payload):set_text("Payload (" .. payload:len() .. " bytes)")
        
        local playfab = Dissector.get("playfab")
        if playfab then
            playfab:call(payload:tvb(), pinfo, tree)
        end
    end
    
    return true
end

local function connect_accept_dissector(buffer, pinfo, tree)

    local xrnm_session = get_xrnm_session(get_flow_key(pinfo))
    if not xrnm_session then return false end

    local subtree = tree:add(xrnm, buffer(), "XRNM (ConnectAccept)")

    -- =========================
    -- Fixed header fields
    -- =========================

    local offset = 0
    
    -- Length
    subtree:add(f_length, buffer(offset, 2))
    offset = offset + 2

    -- Type
    local raw_type = buffer(offset, 1):uint()
    local pkt_type = decode_pkt_type(raw_type)
    subtree:add(f_type, buffer(offset, 1))
    offset = offset + 1
    
    -- Version
    subtree:add(f_ver_major, buffer(offset,1))
    subtree:add(f_ver_minor, buffer(offset+1,1))
    
    xrnm_session.xrnm_major = buffer(offset, 1):uint()
    xrnm_session.xrnm_minor = buffer(offset, 1):uint()
    
    offset = offset + 2

    subtree:add(f_recv_pool, buffer(offset,2))
    offset = offset + 2

    subtree:add(f_ack_period, buffer(offset,2))
    offset = offset + 2

    subtree:add(f_lazy_ack, buffer(offset,2))
    offset = offset + 2

    subtree:add(f_remote_link_id, buffer(offset,4))
    offset = offset + 4

    subtree:add(f_local_link_id, buffer(offset,4))
    
    xrnm_session.local_link_id = buffer(offset, 4):uint()
    
    offset = offset + 4

    subtree:add(f_req_ts_delta, buffer(offset,4))
    offset = offset + 4

    subtree:add(f_timestamp, buffer(offset,4))
    offset = offset + 4

    subtree:add(f_max_channels, buffer(offset,4))
    offset = offset + 4

    -- =========================
    -- Channels field (bitmask)
    -- =========================

    local flags = buffer(offset,1):uint()
    local c1 = band(flags, 0x01) ~= 0
    local c2 = band(flags, 0x02) ~= 0
    local i6 = band(flags, 0x40) ~= 0
    local i4 = band(flags, 0x80) ~= 0
    
    local flags_item = subtree:add(f_flags, buffer(offset,1))
    flags_item:add(f_flags_c1, buffer(offset, 1))
    flags_item:add(f_flags_c2, buffer(offset, 1))
    flags_item:add(f_flags_connect_accept_ipv4, buffer(offset, 1))
    flags_item:add(f_flags_connect_accept_ipv6, buffer(offset, 1))
    offset = offset + 1

    -- =========================
    -- Optional Channel 1
    -- =========================
    if c1 then
        subtree:add(f_ch1, buffer(offset,2))
        offset = offset + 2
    end

    -- =========================
    -- Optional Channel 2
    -- =========================
    if c2 then
        subtree:add(f_ch2, buffer(offset,2))
        offset = offset + 2
    end

    -- =========================
    -- IPv4 / IPv6
    -- =========================

    if i4 then
        subtree:add(f_reflected_ipv4, buffer(offset,4))
        offset = offset + 4

    elseif i6 then
        subtree:add(f_reflected_ipv6, buffer(offset,16))
        offset = offset + 16
    end

    -- =========================
    -- Port (if IP present)
    -- =========================

    if i4 or i6 then
        subtree:add(f_reflected_port, buffer(offset,2))
        offset = offset + 2
    end

    -- =========================
    -- Payload
    -- =========================

    if offset < buffer:len() then
        local payload = buffer(offset)
        subtree:add(f_xrnm_payload, payload):set_text("Payload (" .. payload:len() .. " bytes)")
        
        local playfab = Dissector.get("playfab")
        if playfab then
            playfab:call(payload:tvb(), pinfo, tree)
        end
    end

    return true
end

local function connect_complete_dissector(buffer, pinfo, tree)
    local xrnm_session = get_xrnm_session(get_flow_key(pinfo))
    if not xrnm_session then return false end

    local subtree = tree:add(xrnm, buffer(), "XRNM (ConnectComplete)")

    -- =========================
    -- Fixed header fields
    -- =========================

    local offset = 0
    
    -- Length
    subtree:add(f_length, buffer(offset, 2))
    offset = offset + 2

    -- Type
    local raw_type = buffer(offset, 1):uint()
    local pkt_type = decode_pkt_type(raw_type)
    subtree:add(f_type, buffer(offset, 1))
    offset = offset + 1
    
    -- Fields
    local link_id_item = subtree:add(f_link_id, buffer(offset,4))
    
    if xrnm_session then
        link_id_item:append_text(string.format(" (%x ^ %x)", xrnm_session.remote_link_id, xrnm_session.local_link_id))
    end
    offset = offset + 4

    subtree:add(f_timestamp, buffer(offset,4))
    offset = offset + 4

    -- =========================
    -- Flags field (bitmask)
    -- =========================

    local flags = buffer(offset,1):uint()
    local i6 = band(flags, 0x01) ~= 0
    local i4 = band(flags, 0x02) ~= 0
    
    local flags_item = subtree:add(f_flags, buffer(offset,1))
    flags_item:add(f_flags_connect_complete_ipv4, buffer(offset, 1))
    flags_item:add(f_flags_connect_complete_ipv6, buffer(offset, 1))
    offset = offset + 1

    -- =========================
    -- IPv4 / IPv6
    -- =========================

    if i4 then
        subtree:add(f_reflected_ipv4, buffer(offset,4))
        offset = offset + 4

    elseif i6 then
        subtree:add(f_reflected_ipv6, buffer(offset,16))
        offset = offset + 16
    end

    -- =========================
    -- Port (if IP present)
    -- =========================

    if i4 or i6 then
        subtree:add(f_reflected_port, buffer(offset,2))
        offset = offset + 2
    end

    return true
end

local function connect_deny_dissector(buffer, pinfo, tree)
    local xrnm_session = get_xrnm_session(get_flow_key(pinfo))
    if not xrnm_session then return false end
    
    local subtree = tree:add(xrnm, buffer(), "XRNM (ConnectDeny)")

    -- =========================
    -- Fixed header fields
    -- =========================

    local offset = 0
    
    -- Length
    subtree:add(f_length, buffer(offset, 2))
    offset = offset + 2

    -- Type
    local raw_type = buffer(offset, 1):uint()
    local pkt_type = decode_pkt_type(raw_type)
    subtree:add(f_type, buffer(offset, 1))
    offset = offset + 1
    
    -- Version
    subtree:add(f_ver_major, buffer(offset,1))
    subtree:add(f_ver_minor, buffer(offset+1,1))
    offset = offset + 2
    
    -- Fields
    local link_id = buffer(offset,4)
    subtree:add(f_link_id, link_id)
    offset = offset + 4

    -- =========================
    -- Deny reason
    -- =========================

    if offset + 4 <= buffer:len() then
        subtree:add(f_connect_deny_reason, buffer(offset, 4))
        offset = offset + 4
    else
        local item = subtree:add(f_connect_deny_reason, buffer(0,0))
        item:append_text(" = 0x807A1008 (default)")
    end

    -- =========================
    -- Payload
    -- =========================

    if offset < buffer:len() then
        local payload = buffer(offset)
        subtree:add(payload, "Payload (" .. payload:len() .. " bytes)")
    end

    return true
end

local function disconnect_dissector(buffer, pinfo, tree)
    local xrnm_session = get_xrnm_session(get_flow_key(pinfo))
    if not xrnm_session then return false end
    
    local subtree = tree:add(xrnm, buffer(), "XRNM (Disconnect)")

    -- =========================
    -- Fixed header fields
    -- =========================

    local offset = 0
    
    -- Length
    subtree:add(f_length, buffer(offset, 2))
    offset = offset + 2

    -- Type
    local raw_type = buffer(offset, 1):uint()
    local pkt_type = decode_pkt_type(raw_type)
    subtree:add(f_type, buffer(offset, 1))
    offset = offset + 1
    
    -- Fields
    local link_id = buffer(offset,4)
    subtree:add(f_link_id, link_id)
    offset = offset + 4

    return true
end

local function dack_dissector(buffer, pinfo, tree)
    local xrnm_session = get_xrnm_session(get_flow_key(pinfo))
    if not xrnm_session then return false end
    
    local subtree = tree:add(xrnm, buffer(), "XRNM (DACK)")

    -- =========================
    -- Fixed header fields
    -- =========================

    local offset = 0
    
    -- Length
    subtree:add(f_length, buffer(offset, 2))
    offset = offset + 2

    -- Type
    local raw_type = buffer(offset, 1):uint()
    local pkt_type = decode_pkt_type(raw_type)
    subtree:add(f_type, buffer(offset, 1))
    offset = offset + 1
    
    -- Fields
    local flags = buffer(offset,1):uint()
    local has_sack = band(flags, 0x01) ~= 0
    
    local flags_item = subtree:add(f_flags, buffer(offset,1))
    flags_item:add(f_flags_dack_sack, buffer(offset, 1))
    offset = offset + 1
    
    -- NCD and Sequence id
    local ncd_sequence_item = subtree:add(f_dack_ncd_sequence, buffer(offset,2))
    ncd_sequence_item:add(f_dack_ncd, buffer(offset, 2))
    ncd_sequence_item:add(f_dack_sequence, buffer(offset, 2))
    offset = offset + 2
    
    if has_sack then
        local ack_seq = 0
        local consumed = parse_sack(buffer, offset, ack_seq, subtree)
        offset = offset + consumed
    end
    
    return true
end

local function link_probe_dissector(buffer, pinfo, tree)
    local xrnm_session = get_xrnm_session(get_flow_key(pinfo))
    if not xrnm_session then return false end
    
    local subtree = tree:add(xrnm, buffer(), "XRNM (Link Probe)")

    -- =========================
    -- Fixed header fields
    -- =========================

    local offset = 0
    
    -- Length
    subtree:add(f_length, buffer(offset, 2))
    offset = offset + 2

    -- Type
    local raw_type = buffer(offset, 1):uint()
    local pkt_type = decode_pkt_type(raw_type)
    subtree:add(f_type, buffer(offset, 1))
    offset = offset + 1
    
    xrnm_session = get_xrnm_session(pinfo)
    local reverse_order = false
    
    local xrnm_major = 1
    local xrnm_minor = 5
    if xrnm_session then
        xrnm_major = xrnm_session.xrnm_major
        xrnm_minor = xrnm_session.xrnm_minor
    end
    
    if xrnm_major ~= 1 or xrnm_minor < 3 then
        reverse_order = true
    end

    local flags = buffer(offset,1):uint()
    local is_request = band(flags, 0x80) ~= 0
    local has_sack = band(flags, 0x40) ~= 0
    local has_gap = band(flags, 0x20) ~= 0
     
    local flags_item = subtree:add("Flags/Wire Id", buffer(offset, 2))
    flags_item:add(f_flags_link_probe_type, buffer(offset, 2))
    flags_item:add(f_flags_link_probe_sack, buffer(offset, 2))
    flags_item:add(f_flags_link_probe_gap, buffer(offset, 2))
    flags_item:add(f_link_probe_wire_id, buffer(offset, 2))
    offset = offset + 2

    local ncd_nel_item = subtree:add("NCD/NEL", buffer(offset, 2))
    ncd_nel_item:add(f_ncd, buffer(offset, 2))
    ncd_nel_item:add(f_nel, buffer(offset, 2))
    offset = offset + 2

    local probe_id = buffer(offset, 4):uint()
    subtree:add(f_link_probe_payload, buffer(offset, 4))
    offset = offset + 4

    local xrnm_session = get_xrnm_session(get_flow_key(pinfo))
    if xrnm_session then
        if is_request then
    
            if not pinfo.visited then
                xrnm_session.ping_map[probe_id] = {
                    frame = pinfo.number,
                    ts = pinfo.abs_ts
                }
            end
            
            local probe = xrnm_session.ping_map[probe_id]
    
            if probe and probe.reply_frame then
                subtree:add(f_response_in, probe.reply_frame)
            end
    
        else
    
            local probe = xrnm_session.ping_map[probe_id]
			
            if probe then
                local rtt = pinfo.abs_ts - probe.ts
    
                subtree:add(f_response_to, probe.frame)
                subtree:add(f_rtt, rtt * 1000)
                subtree:append_text(string.format(
                    " [PONG to frame %d RTT=%.3f ms]",
                    probe.frame,
                    rtt * 1000.0
                ))
    
                if not pinfo.visited then
                    probe.reply_frame = pinfo.number
                end
    
            else
                subtree:append_text(" [Orphan Probe]")
            end
        end
    end

    if has_gap and reverse_order then
        local consumed = parse_gap(buffer, offset, subtree)
        offset = offset + consumed
    end

    if has_sack then
        local ack_seq = 0
        local consumed = parse_sack(buffer, offset, ack_seq, subtree)
        offset = offset + consumed
    end

    if has_gap and not reverse_order then
        local consumed = parse_gap(buffer, offset, subtree)
        offset = offset + consumed
    end
    
    return true
end

local function data_dissector(buffer, pinfo, tree)
    local xrnm_session = get_xrnm_session(get_flow_key(pinfo))
    if not xrnm_session then return false end
    
    local subtree = tree:add(xrnm, buffer(), "XRNM (Data)")

    -- =========================
    -- Fixed header fields
    -- =========================

    local offset = 0
    
    -- Length
    subtree:add(f_length, buffer(offset, 2))
    offset = offset + 2

    -- Type
    local raw_type = buffer(offset, 1):uint()
    local pkt_type = decode_pkt_type(raw_type)
    local flags_item = subtree:add("Type/Flags/Wire Id", buffer(offset, 2))
    flags_item:add(f_data_pkt_type, buffer(offset, 2))
    flags_item:add(f_flags_data_ack_type, buffer(offset, 2))
    flags_item:add(f_flags_data_receipt_ack, buffer(offset, 2))
    flags_item:add(f_flags_data_sack, buffer(offset, 2))
    flags_item:add(f_flags_data_gap, buffer(offset, 2))
    flags_item:add(f_data_wire_id, buffer(offset, 2))
    
    local ack_type = rshift(band(raw_type, 0x60), 5)
    local has_rack = band(raw_type, 0x10) ~= 0
    local has_sack = band(raw_type, 0x08) ~= 0
    local has_gap = band(raw_type, 0x04) ~= 0

    offset = offset + 2
    
    local ncd_nel_item = subtree:add("NCD/NEL", buffer(offset, 2))
    ncd_nel_item:add(f_ncd, buffer(offset, 2))
    ncd_nel_item:add(f_nel, buffer(offset, 2))
    offset = offset + 2
    
    if has_sack then
        local ack_seq = 0
        local consumed = parse_sack(buffer, offset, ack_seq, subtree)
        offset = offset + consumed
    end

    if has_gap then
        local consumed = parse_gap(buffer, offset, subtree)
        offset = offset + consumed
    end
    
    -- =========================
    -- Payload
    -- =========================

    if offset < buffer:len() then
        if not subpacket_dissector(buffer, offset, pinfo, tree) then
            local payload = buffer(offset, buffer:len() - offset)
            local subpkt_item = subtree:add(payload, "Payload (" .. payload:len() .. " bytes)")
        else
            subtree:set_len(offset)
        end
    end
    
    return true
end

local handlers = {}
handlers[0x01] = connect_request_dissector
handlers[0x02] = connect_accept_dissector
handlers[0x03] = connect_complete_dissector
handlers[0x04] = connect_deny_dissector
handlers[0x05] = disconnect_dissector
handlers[0x06] = dack_dissector
handlers[0x07] = link_probe_dissector
handlers[0x80] = data_dissector

function xrnm.dissector(buffer, pinfo, tree)
    -- Packet length (2 bytes)
    local pkt_len = buffer(0, 2):uint()

    -- Packet type (1 byte)
    local raw_type = buffer(2, 1):uint()
    local pkt_type = decode_pkt_type(raw_type)
    
    if handlers[pkt_type](buffer, pinfo, tree) then
        pinfo.cols.protocol = "XRNM"
    end
end


local function heuristic_xrnm(buffer, pinfo, tree)
    if buffer:len() < 5 then
        return false
    end
    
    local maybe_packet_size = buffer(0, 2):uint()
    if (maybe_packet_size + 2) ~= buffer:len() then
        return false
    end
    
    local maybe_raw_packet_type = buffer(2, 1):uint()
    local maybe_pkt_type = decode_pkt_type(maybe_raw_packet_type)

    local handler = handlers[maybe_pkt_type]
    if not handler then
        return false
    end
    
    xrnm.dissector(buffer, pinfo, tree)
    
    return true
end


xrnm:register_heuristic("dtls", heuristic_xrnm)
