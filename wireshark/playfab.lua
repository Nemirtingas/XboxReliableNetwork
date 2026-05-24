local xrnm_structs = require("xrnm_structs")

local bit = bit32 or bit

local band   = bit.band
local rshift = bit.rshift
local lshift = bit.lshift

local playfab = Proto("playfab", "PlayFab Protocol")

local packet_type_str = {
    [0x02] = "PlayFab 2",
    [0x05] = "No More Traffic To Local Endpoint Being Destroyed",
    [0x07] = "Finish Destroying Remote Device",
    [0x0d] = "Relay Autentication Response",
    [0x14] = "Remote Invitation Created",
    [0x1b] = "Remote Device Join With Direct Peer Connectivity Started",
    [0x1c] = "Ready To Accept Direct Peer Connection",
    [0x1d] = "Establishing Direct Peer Connection Completed",
    [0x1e] = "Remote Device Join With Direct Peer Connectivity Completed",
    [0x21] = "Device Statistics",
    [0x25] = "No New Remote Devices Will See Endpoint",
    [0x26] = "Local XRNM Address Update",
    [0x27] = "Remote Device Joined",
    [0x28] = "Network Configuration",
    [0x29] = "Authentication Request"
}

local init_packet_type_str = {
    [0x01] = "Direct Peer Init Request",
    [0x02] = "Init Response",
    [0x05] = "Relay Channel Create",
    [0x06] = "Relay Init Request"
}

local session_revocability_str = {
    [0] = "PARTY_INVITATION_REVOCABILITY_CREATOR",
    [1] = "PARTY_INVITATION_REVOCABILITY_ANYONE",
}

local direct_peer_connectivity_options_platform_str = {
    [0x0] = "None",
    [0x1] = "Same Platform",
    [0x2] = "Different Platform",
    [0x3] = "Any Platform"
}

local direct_peer_connectivity_options_login_provider_str = {
    [0x0] = "None",
    [0x1] = "Same Login Provider",
    [0x2] = "Different Login Provider",
    [0x3] = "Any Login Provider"
}

local platform_type_str = {
    [0] = "None",
    [1] = "Windows",
    [16] = "Linux"
}

local f_packet_type = ProtoField.uint8("playfab.packet_type", "Packet type", base.DEC, packet_type_str)
local f_init_packet_type = ProtoField.uint8("playfab.init_packet_type", "Packet type", base.DEC, init_packet_type_str)
local f_relay_authentication_result = ProtoField.uint8("playfab.relay_authentication.result", "Relay Authentication Result")
local f_relay_authentication_entity_id = ProtoField.string("playfab.relay_authentication.entity_id", "Relay Autentication Entity Id")
local f_entity = ProtoField.bytes("playfab.entity", "Entity")
local f_entity_count8 = ProtoField.uint8("playfab.entity.count8", "Entity count", base.DEC)
local f_entity_count = ProtoField.uint16("playfab.entity.count", "Entity count", base.DEC)
local f_entity_type = ProtoField.uint8("playfab.entity.type", "Entity type", base.DEC)
local f_session_id = ProtoField.string("playfab.session.id", "Session Id")
local f_session_revocability = ProtoField.uint8("playfab.session.revocability", "Session Revocability", base.DEC, session_revocability_str)
local f_entity_token = ProtoField.string("playfab.entity_token", "Entity token")
local f_client_instance_id = ProtoField.guid("playfab.instance_id", "Instance Id")
local f_party_id = ProtoField.guid("playfab.party_id", "Party Id")
local f_device_id = ProtoField.uint16("playfab.device_id", "Device Id")
local f_protocol_major = ProtoField.uint8("playfab.protocol.major", "Protocol Major")
local f_protocol_minor = ProtoField.uint8("playfab.protocol.minor", "Protocol Minor")
local f_protocol_prerelease_feature_version = ProtoField.uint16("playfab.protocol.prerelease_feature_version", "Prerelease Feature Version")
local f_platform_type = ProtoField.uint8("playfab.platform_type", "Platform Type", base.DEC, platform_type_str)
local f_round_trip_latency = ProtoField.uint32("playfab.round_trip_latency", "Round Trip Latency (ms)")
local f_options = ProtoField.uint8("playfab.options", "Options")
local f_dtls_fingerprint = ProtoField.bytes("playfab.dtls_fingerprint", "Dtls Fingerprint")
local f_peer_id = ProtoField.guid("playfab.peer_id", "Peer Id")
local f_payload_size = ProtoField.uint16("playfab.payload_size", "Payload Size")
local f_payload = ProtoField.bytes("playfab.payload", "Payload")
local f_max_users = ProtoField.uint32("playfab.max_users", "Max Users")
local f_max_devices = ProtoField.uint16("playfab.max_devices", "Max Devices")
local f_max_users_per_device = ProtoField.uint16("playfab.max_users_per_device", "Max User Per Device")
local f_max_devices_per_user = ProtoField.uint16("playfab.max_devices_per_user", "Max Devices Per User")
local f_max_public_endpoint_per_device = ProtoField.uint16("playfab.max_public_endpoint_per_device", "Max Public Endpoint Per Device")
local f_max_private_endpoint_per_device = ProtoField.uint16("playfab.max_private_endpoint_per_device", "Max Private Endpoint Per Device")
local f_direct_peer_connectivity_options_platform = ProtoField.uint8("playfab.direct_peer_connectivity_options_platform", "Direct Peer Connectivity Options Platform", base.HEX, direct_peer_connectivity_options_platform_str, 0x03)
local f_direct_peer_connectivity_options_login_provider = ProtoField.uint8("playfab.direct_peer_connectivity_options_login_provider", "Direct Peer Connectivity Options Login Provider", base.HEX, direct_peer_connectivity_options_login_provider_str, 0x0C)
local f_endpoint_domain = ProtoField.uint16("playfab.endpoint.domain", "Endpoint Domain")
local f_error = ProtoField.uint32("playfab.error", "Error")
local f_xrnm_address_length = ProtoField.uint16("playfab.xrnm.address.length", "XRNM Address Length")
local f_xrnm_address_value = ProtoField.string("playfab.xrnm.address.value", "XRNM Address")
-- invitation
local f_invitation_id = ProtoField.uint32("playfab.invitation.id", "Invitation Id")
-- statistics
local f_device_statistics_count = ProtoField.uint8("playfab.device_statistics.count", "Device Statistics Count")
local f_device_count = ProtoField.uint8("playfab.device_statistics.device_count", "Device count")
local f_device_statistic_id = ProtoField.uint8("playfab.device_statistics.statistic_id", "Statistic Id")
local f_device_statistic_device_id = ProtoField.uint16("playfab.device_statistics.device_id", "Statistic Device Id")
local f_device_statistic_value = ProtoField.uint64("playfab.device_statistics.statistic_value", "Statistic Value")
local f_client_device_statistics_refresh_period = ProtoField.uint32("playfab.device_statistics.client_device_statistics_refresh_period", "Client Device Statistics Refresh Period")
-- utils
local f_packed_string = ProtoField.bytes("playfab.packed_string", "Packed String")
local f_packed_string_length = ProtoField.uint8("playfab.packed_string.length", "Packed String Length")
local f_packed_string_value = ProtoField.string("playfab.packed_string.value", "Packed String")
-- unknown
local f_unknown_byte = ProtoField.uint8("playfab.unknown_byte", "Unknown byte", base.HEX)
local f_unknown_short = ProtoField.uint16("playfab.unknown_short", "Unknown short", base.HEX)
local f_unknown_long = ProtoField.uint32("playfab.unknown_long", "Unknown long", base.HEX)

playfab.fields = {
    f_packet_type,
    f_init_packet_type,
    f_relay_authentication_result,
    f_relay_authentication_entity_id,
    f_entity,
    f_entity_count8,
    f_entity_count,
    f_entity_type,
    f_session_id,
    f_session_revocability,
    f_entity_token,
    f_client_instance_id,
    f_party_id,
    f_device_id,
    f_protocol_major, f_protocol_minor, f_protocol_prerelease_feature_version,
    f_platform_type,
    f_round_trip_latency,
    f_options,
    f_dtls_fingerprint,
    f_peer_id,
    f_payload_size, f_payload,
    f_max_users, f_max_devices, f_max_users_per_device, f_max_devices_per_user, f_max_public_endpoint_per_device, f_max_private_endpoint_per_device,
    f_direct_peer_connectivity_options_platform, f_direct_peer_connectivity_options_login_provider,
    f_endpoint_domain, f_error,
    f_xrnm_endpoint_length, f_xrnm_address_length, f_xrnm_address_value,
    -- invitation
    f_invitation_id,
    -- statistics
    f_device_statistics_count, f_device_count, f_device_statistic_id, f_device_statistic_device_id, f_device_statistic_value, f_client_device_statistics_refresh_period,
    -- utils
    f_packed_string, f_packed_string_length, f_packed_string_value,
    -- unknown
    f_unknown_byte, f_unknown_short, f_unknown_long
}

xrnm_structs.add_fields(playfab)

local b='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'

local function raw_to_bytearray(raw)
    local hex = {}

    for i = 1, #raw do
        hex[#hex+1] = string.format("%02X", string.byte(raw, i))
    end

    return ByteArray.new(table.concat(hex))
end

local function base64_decode_to_tvb(base64, name)

    if not base64 or #base64 == 0 then
        return nil
    end

    if base64:find('[^'..b..'=%s]') then
        return nil
    end

    local ok, decoded = pcall(function()

        base64 = string.gsub(base64, '[^'..b..'=]', '')

        return (base64:gsub('.', function(x)

            if x == '=' then
                return ''
            end

            local r,f='',(b:find(x)-1)

            for i=6,1,-1 do
                r = r .. (
                    f % 2^i - f % 2^(i-1) > 0
                    and '1'
                    or '0'
                )
            end

            return r

        end):gsub('%d%d%d?%d?%d?%d?%d?%d?', function(x)

            if #x ~= 8 then
                return ''
            end

            local c = 0

            for i=1,8 do
                c = c + (
                    x:sub(i,i) == '1'
                    and 2^(8-i)
                    or 0
                )
            end

            return string.char(c)
        end))
    end)

    if not ok or not decoded or #decoded == 0 then
        return nil
    end

    local ba = raw_to_bytearray(decoded)

    return ba:tvb(name or "Base64 Decoded")
end

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

local function create_packed_string_item(buffer, offset, tree, title, extra_bytes_count)
    local packed_string_length = buffer(offset, 1):uint()
    tree:add(f_packed_string_length, buffer(offset, 1)):set_text(title .. " Length: " .. packed_string_length)
    offset = offset + 1
    
    tree:add(f_packed_string_value, buffer(offset, packed_string_length)):set_text(title .. ": " .. buffer(offset, packed_string_length):string())
    offset = offset + packed_string_length
    
    tree:set_len(1 + packed_string_length + extra_bytes_count)
    tree:set_text(title .. " (" .. 1 + packed_string_length + extra_bytes_count .. " bytes)")
    
    return offset
end

local function create_entity_item(buffer, offset, tree)
    local entity_type = buffer(offset, 1):uint()
    local entity_item = tree:add(f_entity, buffer(offset, 1))
    
    entity_item:add(f_entity_type, buffer(offset, 1))
    offset = offset + 1
    
    if entity_type == 1 then
        local old_offset = offset
        offset = create_packed_string_item(buffer, offset, entity_item, "Entity", 1)
    end
    
    return offset
end

local function playfab_2_dissector(buffer, pinfo, tree)
    local offset = 0
        
    if buffer:len() == 5 then
        local subtree = tree:add(playfab, buffer(), "PlayFab (" .. init_packet_type_str[buffer(offset, 1):uint()] .. ")")
        
        subtree:add(f_init_packet_type, buffer(offset, 1)):set_text(init_packet_type_str[buffer(offset, 1):uint()])
        offset = offset + 1
        
        subtree:add(f_protocol_major, buffer(offset, 1))
        offset = offset + 1
        
        subtree:add(f_protocol_minor, buffer(offset, 1))
        offset = offset + 1
        
        subtree:add_le(f_protocol_prerelease_feature_version, buffer(offset, 2))
        offset = offset + 2
        
    else
        local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
        subtree:add(f_packet_type, buffer(offset, 1)):set_text(init_packet_type_str[buffer(offset, 1):uint()])
        offset = offset + 1
    
        offset = create_entity_item(buffer, offset, subtree)
    end
end

local function playfab_5_dissector(buffer, pinfo, tree)
    local offset = 0
    
    if buffer:len() == 4 then    
        local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
        
        subtree:add(f_packet_type, buffer(offset, 1)):set_text(init_packet_type_str[buffer(offset, 1):uint()])
        offset = offset + 1
        
        subtree:add(f_endpoint_domain, buffer(offset, 1))
        offset = offset + 1
        
        subtree:add_le(f_device_id, buffer(offset, 2))
        offset = offset + 2
    else
        local subtree = tree:add(playfab, buffer(), "PlayFab (" .. init_packet_type_str[buffer(offset, 1):uint()] .. ")")
        
        subtree:add(f_packet_type, buffer(offset, 1)):set_text(init_packet_type_str[buffer(offset, 1):uint()])
        offset = offset + 1
        
        subtree:add(f_endpoint_domain, buffer(offset, 1))
        offset = offset + 1
        
        subtree:add_le(f_device_id, buffer(offset, 2))
        offset = offset + 2
        
        local entity_count = buffer(offset, 1):uint()
        subtree:add(f_entity_count8, buffer(offset, 1))
        offset = offset + 1
        
        for i = 1, entity_count do
            offset = create_entity_item(buffer, offset, subtree)
        end
    end
end

local function playfab_6_dissector(buffer, pinfo, tree)
    local offset = 0

    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. init_packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_init_packet_type, buffer(offset, 1)):set_text(init_packet_type_str[buffer(offset, 1):uint()])
    offset = offset + 1
    
    subtree:add_le(f_party_id, buffer(offset, 16))
    offset = offset + 16
    
    subtree:add_le(f_client_instance_id, buffer(offset, 16))
    offset = offset + 16
    
    subtree:add(f_protocol_major, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add(f_protocol_minor, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_protocol_prerelease_feature_version, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add_le(f_platform_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_unknown_byte, buffer(offset, 1))
    offset = offset + 1
    
    local payload_size = buffer(offset, 2):le_uint()
    subtree:add_le(f_payload_size, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add(f_payload, buffer(offset, payload_size)):set_text("Payload (" .. payload_size .. " bytes)")
    
    local decoded_tvb = base64_decode_to_tvb(buffer(offset, payload_size):string())
    
    if decoded_tvb then
        local decoded_buffer = decoded_tvb:range()
        local decoded_offset = 0
        local decoded_item = subtree:add(decoded_buffer, "Decoded Base64 Payload")
        xrnm_structs.create_xrnm_address_item(decoded_buffer, decoded_offset, decoded_item)
    end
end

local function finish_destroying_remote_device_dissector(buffer, pinfo, tree)
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_device_id, buffer(offset, 2))
    offset = offset + 2
end

local function relay_authentication_response_dissector(buffer, pinfo, tree)    
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")

    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add(f_relay_authentication_result, buffer(offset, 1))
    offset = offset + 1
    
    local str, string_length = read_c_string(buffer, offset)
    
    subtree:add(f_relay_authentication_entity_id, buffer(offset, string_length))
    offset = offset + string_length
    
end

local function remote_invitation_created_dissector(buffer, pinfo, tree)    
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_invitation_id, buffer(offset, 4))
    offset = offset + 4
    
    local session_id_item = subtree:add(f_packed_string, buffer(offset, 1))
    offset = create_packed_string_item(buffer, offset, session_id_item, "Session Id", 0)
    
    subtree:add(f_session_revocability, buffer(offset, 1))
    offset = offset + 1
    
    local entity_count = buffer(offset, 2):uint()
    subtree:add(f_entity_count, buffer(offset, 2))
    offset = offset + 2
    
    for i = 1, entity_count do
        offset = create_packed_string_item(buffer, offset, subtree, "Entity Id", 0)
    end
end

local function remote_device_join_with_direct_peer_connectivity_started_dissector(buffer, pinfo, tree)    
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_device_id, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add_le(f_client_instance_id, buffer(offset, 16))
    offset = offset + 16
    
    subtree:add(f_protocol_major, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add(f_protocol_minor, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_protocol_prerelease_feature_version, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add_le(f_round_trip_latency, buffer(offset, 4))
    offset = offset + 4
    
    subtree:add(f_options, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add(f_dtls_fingerprint, buffer(offset, 32))
    offset = offset + 32
    
    subtree:add_le(f_peer_id, buffer(offset, 16))
    offset = offset + 16
    
    local payload_size = buffer(offset, 2):le_uint()
    subtree:add_le(f_payload_size, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add(f_payload, buffer(offset, payload_size)):set_text("Payload (" .. payload_size .. " bytes)")
    
    local decoded_tvb = base64_decode_to_tvb(buffer(offset, payload_size):string())
    
    if decoded_tvb then
        local decoded_buffer = decoded_tvb:range()
        local decoded_offset = 0
        local decoded_item = subtree:add(decoded_buffer, "Decoded Base64 Payload")
        xrnm_structs.create_xrnm_address_item(decoded_buffer, decoded_offset, decoded_item)
    end
end

local function ready_to_accept_direct_peer_connection_dissector(buffer, pinfo, tree)
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_device_id, buffer(offset, 2))
    offset = offset + 2
end

local function establishing_direct_peer_connection_completed_dissector(buffer, pinfo, tree)
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_device_id, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add_le(f_error, buffer(offset, 4))
    offset = offset + 4
    
end

local function remote_device_join_with_direct_peer_connectivity_completed_dissector(buffer, pinfo, tree)
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_device_id, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add_le(f_options, buffer(offset, 1))
    offset = offset + 1
end

local function device_statistics_message_dissector(buffer, pinfo, tree)    
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    local statistics_count = buffer(offset, 1):uint()
    subtree:add(f_device_statistics_count, buffer(offset, 1))
    offset = offset + 1
    
    local device_count = buffer(offset, 1):uint()
    subtree:add(f_device_count, buffer(offset, 1))
    offset = offset + 1
    
    for i = 1, statistics_count do
        subtree:add(f_device_statistic_id, buffer(offset, 1))
        offset = offset + 1
    end
    
    for i = 1, device_count do
        subtree:add_le(f_device_statistic_device_id, buffer(offset, 2))
        offset = offset + 2
    end
    
    for i = 1, statistics_count do
        for j = 1, device_count do
            subtree:add_le(f_device_statistic_value, buffer(offset, 8)):append_text(string.format(" (stat %d, device %d)", i, j))
            offset = offset + 8
        end
    end
end

local function no_new_remote_devices_will_see_endpoint_message_dissector(buffer, pinfo, tree)    
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add(f_endpoint_domain, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_device_id, buffer(offset, 2))
    offset = offset + 2
end

local function local_endpoint_update_dissector(buffer, pinfo, tree)    
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    local address_length = buffer(offset, 2):le_uint()
    subtree:add_le(f_xrnm_address_length, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add(f_xrnm_address_value, buffer(offset, address_length))
    
    local decoded_tvb = base64_decode_to_tvb(buffer(offset, address_length):string())
    
    if decoded_tvb then
        local decoded_buffer = decoded_tvb:range()
        local decoded_offset = 0
        local decoded_item = subtree:add(decoded_buffer, "Decoded Base64 Payload")
        xrnm_structs.create_xrnm_address_item(decoded_buffer, decoded_offset, decoded_item)
    end
    
    offset = offset + address_length
end

local function network_configuration_dissector(buffer, pinfo, tree)    
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_max_users, buffer(offset, 4))
    offset = offset + 4
    
    subtree:add_le(f_max_devices, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add_le(f_max_users_per_device, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add_le(f_max_devices_per_user, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add_le(f_max_public_endpoint_per_device, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add_le(f_max_private_endpoint_per_device, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add(f_direct_peer_connectivity_options_platform, buffer(offset, 1))
    subtree:add(f_direct_peer_connectivity_options_login_provider, buffer(offset, 1))
    offset = offset + 1
    
    subtree:add_le(f_device_id, buffer(offset, 2))
    offset = offset + 2
    
    subtree:add_le(f_client_device_statistics_refresh_period, buffer(offset, 4))
    offset = offset + 4
    
end

local function authentication_request_dissector(buffer, pinfo, tree)    
    local offset = 0
    
    local subtree = tree:add(playfab, buffer(), "PlayFab (" .. packet_type_str[buffer(offset, 1):uint()] .. ")")
    
    subtree:add(f_packet_type, buffer(offset, 1))
    offset = offset + 1
    
    offset = create_entity_item(buffer, offset, subtree)
    
    local session_id_item = subtree:add(f_packed_string, buffer(offset, 1))
    offset = create_packed_string_item(buffer, offset, session_id_item, "Session Id", 0)
    
    subtree:add(f_entity_token, buffer(offset))
    
    local decoded_tvb = base64_decode_to_tvb(buffer(offset):string())
    
    if decoded_tvb then
        local decoded_buffer = decoded_tvb:range()
        local decoded_offset = 0
        local decoded_item = subtree:add(decoded_buffer, "Decoded Base64 Payload")
    end
    
    offset = offset + buffer(offset):len()
end

local handlers = {}
handlers[0x02] = playfab_2_dissector
handlers[0x05] = playfab_5_dissector
handlers[0x06] = playfab_6_dissector
handlers[0x07] = finish_destroying_remote_device_dissector
handlers[0x0d] = relay_authentication_response_dissector
handlers[0x14] = remote_invitation_created_dissector
handlers[0x1b] = remote_device_join_with_direct_peer_connectivity_started_dissector
handlers[0x1c] = ready_to_accept_direct_peer_connection_dissector
handlers[0x1d] = establishing_direct_peer_connection_completed_dissector
handlers[0x1e] = remote_device_join_with_direct_peer_connectivity_completed_dissector
handlers[0x21] = device_statistics_message_dissector
handlers[0x25] = no_new_remote_devices_will_see_endpoint_message_dissector
handlers[0x26] = local_endpoint_update_dissector
handlers[0x28] = network_configuration_dissector
handlers[0x29] = authentication_request_dissector

function playfab.dissector(buffer, pinfo, tree)
    -- Packet type (1 byte)
    local pkt_type = buffer(0, 1):uint()
    
    local handler = handlers[pkt_type]
    if not handler then return end
    
    handler(buffer, pinfo, tree)
end

