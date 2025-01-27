/*
 *  Copyright (C) 2017-2021, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may
 *    not use this file except in compliance with the License. You may obtain
 *    a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 *    THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR
 *    CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 *    LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
 *    FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 *    See the Apache Version 2.0 License for specific language governing
 *    permissions and limitations under the License.
 *
 */

#include "sai_windows.h"
#include "sai.h"
#include "mlnx_sai.h"
#include "assert.h"

#undef  __MODULE__
#define __MODULE__ SAI_FDB

static sx_verbosity_level_t LOG_VAR_NAME(__MODULE__) = SX_VERBOSITY_LEVEL_WARNING;
static sai_status_t mlnx_fdb_type_set(_In_ const sai_object_key_t      *key,
                                      _In_ const sai_attribute_value_t *value,
                                      void                             *arg);
static sai_status_t mlnx_fdb_port_set(_In_ const sai_object_key_t      *key,
                                      _In_ const sai_attribute_value_t *value,
                                      void                             *arg);
static sai_status_t mlnx_fdb_action_set(_In_ const sai_object_key_t      *key,
                                        _In_ const sai_attribute_value_t *value,
                                        void                             *arg);
static sai_status_t mlnx_fdb_endpoint_ip_set(_In_ const sai_object_key_t      *key,
                                             _In_ const sai_attribute_value_t *value,
                                             void                             *arg);
static sai_status_t mlnx_fdb_macmove_set(_In_ const sai_object_key_t      *key,
                                         _In_ const sai_attribute_value_t *value,
                                         void                             *arg);
static sai_status_t mlnx_fdb_type_get(_In_ const sai_object_key_t   *key,
                                      _Inout_ sai_attribute_value_t *value,
                                      _In_ uint32_t                  attr_index,
                                      _Inout_ vendor_cache_t        *cache,
                                      void                          *arg);
static sai_status_t mlnx_fdb_port_get(_In_ const sai_object_key_t   *key,
                                      _Inout_ sai_attribute_value_t *value,
                                      _In_ uint32_t                  attr_index,
                                      _Inout_ vendor_cache_t        *cache,
                                      void                          *arg);
static sai_status_t mlnx_fdb_action_get(_In_ const sai_object_key_t   *key,
                                        _Inout_ sai_attribute_value_t *value,
                                        _In_ uint32_t                  attr_index,
                                        _Inout_ vendor_cache_t        *cache,
                                        void                          *arg);
static sai_status_t mlnx_fdb_endpoint_ip_get(_In_ const sai_object_key_t   *key,
                                             _Inout_ sai_attribute_value_t *value,
                                             _In_ uint32_t                  attr_index,
                                             _Inout_ vendor_cache_t        *cache,
                                             void                          *arg);
static sai_status_t mlnx_fdb_macmove_get(_In_ const sai_object_key_t   *key,
                                         _Inout_ sai_attribute_value_t *value,
                                         _In_ uint32_t                  attr_index,
                                         _Inout_ vendor_cache_t        *cache,
                                         void                          *arg);
static sai_status_t mlnx_fdb_bv_id_to_sx_fid(_In_ sai_object_id_t bv_id, _Out_ sx_fid_t       *sx_fid);
static sai_status_t mlnx_bridge_port_translate_to_sdk(mlnx_bridge_port_t          *bport,
                                                      sx_fdb_uc_mac_addr_params_t *fdb_entry);
static const sai_vendor_attribute_entry_t fdb_vendor_attribs[] = {
    { SAI_FDB_ENTRY_ATTR_TYPE,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_fdb_type_get, NULL,
      mlnx_fdb_type_set, NULL },
    { SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_fdb_port_get, NULL,
      mlnx_fdb_port_set, NULL },
    { SAI_FDB_ENTRY_ATTR_PACKET_ACTION,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_fdb_action_get, NULL,
      mlnx_fdb_action_set, NULL },
    { SAI_FDB_ENTRY_ATTR_ENDPOINT_IP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_fdb_endpoint_ip_get, NULL,
      mlnx_fdb_endpoint_ip_set, NULL },
    { SAI_FDB_ENTRY_ATTR_ALLOW_MAC_MOVE,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_fdb_macmove_get, NULL,
      mlnx_fdb_macmove_set, NULL },
    { END_FUNCTIONALITY_ATTRIBS_ID,
      { false, false, false, false },
      { false, false, false, false },
      NULL, NULL,
      NULL, NULL }
};
static const mlnx_attr_enum_info_t        fdb_entry_enum_info[] = {
    [SAI_FDB_ENTRY_ATTR_TYPE] = ATTR_ENUM_VALUES_LIST(
        SAI_FDB_ENTRY_TYPE_STATIC,
        SAI_FDB_ENTRY_TYPE_DYNAMIC
        ),
    [SAI_FDB_ENTRY_ATTR_PACKET_ACTION] = ATTR_ENUM_VALUES_LIST(
        SAI_PACKET_ACTION_FORWARD,
        SAI_PACKET_ACTION_TRAP,
        SAI_PACKET_ACTION_LOG,
        SAI_PACKET_ACTION_DROP
        )
};
static size_t fdb_entry_info_print(_In_ const sai_object_key_t *key, _Out_ char *str, _In_ size_t max_len)
{
    return snprintf(str, max_len, "[%02x:%02x:%02x:%02x:%02x:%02x] [BV_ID:0x%lX]",
                    key->key.fdb_entry.mac_address[0],
                    key->key.fdb_entry.mac_address[1],
                    key->key.fdb_entry.mac_address[2],
                    key->key.fdb_entry.mac_address[3],
                    key->key.fdb_entry.mac_address[4],
                    key->key.fdb_entry.mac_address[5],
                    key->key.fdb_entry.bv_id);
}
const mlnx_obj_type_attrs_info_t mlnx_fdb_entry_obj_type_info =
{ fdb_vendor_attribs, OBJ_ATTRS_ENUMS_INFO(fdb_entry_enum_info), OBJ_STAT_CAP_INFO_EMPTY(), fdb_entry_info_print};
static const sai_vendor_attribute_entry_t fdb_flush_vendor_attribs[] = {
    { SAI_FDB_FLUSH_ATTR_BRIDGE_PORT_ID,
      { true, false, false, false },
      { true, false, false, false },
      NULL, NULL,
      NULL, NULL },
    { SAI_FDB_FLUSH_ATTR_BV_ID,
      { true, false, false, false },
      { true, false, false, false },
      NULL, NULL,
      NULL, NULL },
    { SAI_FDB_FLUSH_ATTR_ENTRY_TYPE,
      { true, false, false, false },
      { true, false, false, false },
      NULL, NULL,
      NULL, NULL },
    { END_FUNCTIONALITY_ATTRIBS_ID,
      { true, false, false, false },
      { true, false, false, false },
      NULL, NULL,
      NULL, NULL }
};
static const mlnx_attr_enum_info_t        fdb_flush_enum_info[] = {
    [SAI_FDB_FLUSH_ATTR_ENTRY_TYPE] = ATTR_ENUM_VALUES_LIST(
        SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC)
};
const mlnx_obj_type_attrs_info_t          mlnx_fdb_flush_obj_type_info =
{ fdb_flush_vendor_attribs, OBJ_ATTRS_ENUMS_INFO(fdb_flush_enum_info), OBJ_STAT_CAP_INFO_EMPTY(), NULL};
static sai_status_t mlnx_add_or_del_mac(sx_fdb_uc_mac_addr_params_t *mac_entry, sx_access_cmd_t cmd)
{
    uint32_t            entries_count = 1;
    const char         *cmd_name = cmd == SX_ACCESS_CMD_ADD ? "add" : "del";
    sx_status_t         status;
    mlnx_bridge_port_t *port;

    SX_LOG_ENTER();

    status = sx_api_fdb_uc_mac_addr_set(gh_sdk, cmd, DEFAULT_ETH_SWID, mac_entry, &entries_count);
    if (SX_ERR(status)) {
        SX_LOG_ERR("Failed to %s %d fdb entries %s.\n", cmd_name, entries_count, SX_STATUS_MSG(status));
        SX_LOG_ERR("[%02x:%02x:%02x:%02x:%02x:%02x], vlan %d, log port 0x%x, entry type %u, action %u, dest type %u\n",
                   mac_entry->mac_addr.ether_addr_octet[0],
                   mac_entry->mac_addr.ether_addr_octet[1],
                   mac_entry->mac_addr.ether_addr_octet[2],
                   mac_entry->mac_addr.ether_addr_octet[3],
                   mac_entry->mac_addr.ether_addr_octet[4],
                   mac_entry->mac_addr.ether_addr_octet[5],
                   mac_entry->fid_vid,
                   mac_entry->log_port,
                   mac_entry->entry_type,
                   mac_entry->action,
                   mac_entry->dest_type);
        return sdk_to_sai(status);
    }

    /* Check if this entry is CPU port related */
    if (SX_FDB_IS_PORT_REDUNDANT(mac_entry->entry_type, mac_entry->action)) {
        SX_LOG_EXIT();
        return SAI_STATUS_SUCCESS;
    }

    if ((mac_entry->entry_type == SX_FDB_UC_AGEABLE)
        || (mac_entry->entry_type == SX_FDB_UC_REMOTE)) {
        SX_LOG_EXIT();
        return SAI_STATUS_SUCCESS;
    }
    if (mlnx_log_port_is_cpu(mac_entry->log_port)) {
        SX_LOG_EXIT();
        return SAI_STATUS_SUCCESS;
    }

    sai_db_write_lock();

    if (mac_entry->log_port == g_sai_db_ptr->sx_nve_log_port) {
        sai_db_unlock();
        SX_LOG_EXIT();
        return SAI_STATUS_SUCCESS;
    }

    status = mlnx_bridge_port_by_log(mac_entry->log_port, &port);
    if (SAI_ERR(status)) {
        sai_db_unlock();
        SX_LOG_ERR("Failed to get port using log port 0x%x\n", mac_entry->log_port);
        SX_LOG_EXIT();
        return status;
    }
    if (cmd == SX_ACCESS_CMD_ADD) {
        port->fdbs++;
    } else if (port->fdbs) {
        port->fdbs--;
    }
    sai_db_unlock();

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_add_mac(sx_fdb_uc_mac_addr_params_t *mac_entry)
{
    return mlnx_add_or_del_mac(mac_entry, SX_ACCESS_CMD_ADD);
}

static sai_status_t mlnx_del_mac(sx_fdb_uc_mac_addr_params_t *mac_entry)
{
    return mlnx_add_or_del_mac(mac_entry, SX_ACCESS_CMD_DELETE);
}

static sai_status_t mlnx_fdb_bv_id_to_sx_fid(_In_ sai_object_id_t bv_id, _Out_ sx_fid_t       *sx_fid)
{
    sai_status_t      status;
    sai_object_type_t bv_id_type;
    sx_vlan_id_t      vlan_id;
    sx_bridge_id_t    bridge_id;

    assert(sx_fid);

    bv_id_type = sai_object_type_query(bv_id);
    if (bv_id_type == SAI_NULL_OBJECT_ID) {
        SX_LOG_ERR("Bad object id - %lx\n", bv_id);
        return SAI_STATUS_INVALID_OBJECT_ID;
    }

    if (bv_id_type == SAI_OBJECT_TYPE_VLAN) {
        status = sai_object_to_vlan(bv_id, &vlan_id);
        if (SAI_ERR(status)) {
            return status;
        }

        *sx_fid = vlan_id;
    } else if (bv_id_type == SAI_OBJECT_TYPE_BRIDGE) {
        status = mlnx_bridge_oid_to_id(bv_id, &bridge_id);
        if (SAI_ERR(status)) {
            return status;
        }

        *sx_fid = bridge_id;
    } else {
        SX_LOG_ERR("Invalid fdb entry bv_id object type - %s, should be VLAN or Bridge\n", SAI_TYPE_STR(bv_id_type));
        return SAI_STATUS_INVALID_PARAMETER;
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_fdb_entry_to_sdk(const sai_fdb_entry_t *fdb_entry, sx_fdb_uc_mac_addr_params_t *mac_entry)
{
    sai_status_t status;

    status = mlnx_fdb_bv_id_to_sx_fid(fdb_entry->bv_id, &mac_entry->fid_vid);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert bv_id %lx to sx_fid\n", fdb_entry->bv_id);
        return status;
    }

    memcpy(&mac_entry->mac_addr, fdb_entry->mac_address, sizeof(mac_entry->mac_addr));

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_get_mac(const sai_fdb_entry_t *fdb_entry, sx_fdb_uc_mac_addr_params_t *mac_entry)
{
    uint32_t                    entries_count = 1;
    sx_fdb_uc_mac_addr_params_t mac_key;
    sx_fdb_uc_key_filter_t      filter;
    sx_status_t                 status;

    SX_LOG_ENTER();

    status = mlnx_fdb_entry_to_sdk(fdb_entry, &mac_key);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert sai_fdb_entry_t to SDK params\n");
        return status;
    }

    memset(&filter, 0, sizeof(filter));

    if (SX_STATUS_SUCCESS !=
        (status =
             sx_api_fdb_uc_mac_addr_get(gh_sdk, DEFAULT_ETH_SWID, SX_ACCESS_CMD_GET, SX_FDB_UC_ALL, &mac_key, &filter,
                                        mac_entry, &entries_count))) {
        SX_LOG_ERR("Failed to get %d fdb entries %s.\n", entries_count, SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_get_n_delete_mac(const sai_fdb_entry_t *fdb_entry, sx_fdb_uc_mac_addr_params_t *mac_entry)
{
    sai_status_t status;

    SX_LOG_ENTER();

    status = mlnx_get_mac(fdb_entry, mac_entry);
    if (SAI_STATUS_SUCCESS != status) {
        SX_LOG_ERR("Failed to get mac\n");
        SX_LOG_EXIT();
        return status;
    }

    status = mlnx_del_mac(mac_entry);

    SX_LOG_EXIT();
    return status;
}

sai_status_t mlnx_translate_sai_action_to_sdk(sai_int32_t                  action,
                                              sx_fdb_uc_mac_addr_params_t *mac_entry,
                                              uint32_t                     param_index)
{
    switch (action) {
    case SAI_PACKET_ACTION_FORWARD:
        mac_entry->action = SX_FDB_ACTION_FORWARD;
        break;

    case SAI_PACKET_ACTION_TRAP:
        mac_entry->action = SX_FDB_ACTION_TRAP;
        break;

    case SAI_PACKET_ACTION_LOG:
        mac_entry->action = SX_FDB_ACTION_MIRROR_TO_CPU;
        break;

    case SAI_PACKET_ACTION_DROP:
        mac_entry->action = SX_FDB_ACTION_DISCARD;
        break;

    default:
        SX_LOG_ERR("Invalid fdb action %d\n", action);
        return SAI_STATUS_INVALID_ATTR_VALUE_0 + param_index;
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_translate_sai_type_to_sdk(sai_int32_t                  type,
                                                   bool                         allow_move,
                                                   sx_fdb_uc_mac_addr_params_t *mac_entry,
                                                   uint32_t                     param_index)
{
    switch (type) {
    case SAI_FDB_ENTRY_TYPE_DYNAMIC:
        mac_entry->entry_type = SX_FDB_UC_AGEABLE;
        break;

    case SAI_FDB_ENTRY_TYPE_STATIC:
        mac_entry->entry_type = allow_move ? SX_FDB_UC_REMOTE : SX_FDB_UC_STATIC;
        break;

    default:
        SX_LOG_ERR("Invalid fdb entry type %d\n", type);
        return SAI_STATUS_INVALID_ATTR_VALUE_0 + param_index;
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_fdb_attrs_to_sx(_In_ const sai_attribute_value_t  *type_attr,
                                         _In_ uint32_t                      type_attr_index,
                                         _In_ const sai_attribute_value_t  *bport_attr,
                                         _In_ uint32_t                      bport_attr_index,
                                         _In_ const sai_attribute_value_t  *action_attr,
                                         _In_ uint32_t                      action_index,
                                         _In_ const sai_attribute_value_t  *allow_move_attr,
                                         _In_ uint32_t                      allow_move_index,
                                         _In_ const sai_attribute_value_t  *ip_addr,
                                         _Out_ sx_fdb_uc_mac_addr_params_t *fdb_entry)
{
    sai_status_t        status = SAI_STATUS_SUCCESS;
    sai_packet_action_t packet_action;
    mlnx_bridge_port_t *bport;
    sai_ip_address_t    default_ip_addr = {
        .addr_family = SAI_IP_ADDR_FAMILY_IPV4,
        .addr = {.ip4 = ntohl(0)}
    };
    bool                allow_move = false;

    assert(type_attr);
    assert(fdb_entry);

    if (allow_move_attr) {
        allow_move = allow_move_attr->booldata;
    }

    status = mlnx_translate_sai_type_to_sdk(type_attr->s32, allow_move, fdb_entry, type_attr_index);
    if (SAI_ERR(status)) {
        return status;
    }

    if (!action_attr) {
        packet_action = SAI_PACKET_ACTION_FORWARD;
    } else {
        packet_action = action_attr->s32;
    }

    if (allow_move && (packet_action != SAI_PACKET_ACTION_FORWARD)) {
        SX_LOG_ERR("Remote FDB entry can only work with Forward action\n");
        return SAI_STATUS_INVALID_ATTR_VALUE_0 + action_index;
    }

    status = mlnx_translate_sai_action_to_sdk(packet_action, fdb_entry, action_index);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_translate_sai_ip_address_to_sdk(
        ip_addr ? &ip_addr->ipaddr : &default_ip_addr,
        &fdb_entry->dest.next_hop.next_hop_key.next_hop_key_entry.ip_tunnel.underlay_dip);
    if (SAI_ERR(status)) {
        return status;
    }

    if (!bport_attr || (SAI_NULL_OBJECT_ID == bport_attr->oid)) {
        if (false == SX_FDB_IS_PORT_REDUNDANT(fdb_entry->entry_type, fdb_entry->action)) {
            SX_LOG_NTC("Failed to create FDB Entry - action (%d) needs a port id attribute\n", packet_action);
            return SAI_STATUS_MANDATORY_ATTRIBUTE_MISSING;
        }

        fdb_entry->log_port = SX_INVALID_PORT;
        return SAI_STATUS_SUCCESS;
    }

    sai_db_read_lock();

    status = mlnx_bridge_port_by_oid(bport_attr->oid, &bport);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to lookup bridge port by oid %" PRIx64 "\n", bport_attr->oid);
        goto out;
    }

    status = mlnx_bridge_port_translate_to_sdk(bport, fdb_entry);
    if (status == SAI_STATUS_INVALID_ATTR_VALUE_0) {
        status += bport_attr_index;
    }

out:
    sai_db_unlock();
    return status;
}

static sai_status_t mlnx_bridge_port_translate_to_sdk(mlnx_bridge_port_t          *bport,
                                                      sx_fdb_uc_mac_addr_params_t *fdb_entry)
{
    sai_status_t     status = SAI_STATUS_SUCCESS;
    sx_tunnel_id_t   sx_tunnel_id;
    sai_ip_address_t default_ip_addr = {
        .addr_family = SAI_IP_ADDR_FAMILY_IPV4,
        .addr = {.ip4 = ntohl(0)}
    };

    switch (bport->port_type) {
    case SAI_BRIDGE_PORT_TYPE_PORT:
    case SAI_BRIDGE_PORT_TYPE_SUB_PORT:
        fdb_entry->log_port = bport->logical;
        break;

    case SAI_BRIDGE_PORT_TYPE_1D_ROUTER:
    case SAI_BRIDGE_PORT_TYPE_1Q_ROUTER:
        if ((fdb_entry->action != SX_FDB_ACTION_FORWARD)
            && (fdb_entry->action != SX_FDB_ACTION_FORWARD_TO_ROUTER)) {
            SX_LOG_ERR("Bridge port type 1D/1Q router is only valid for SAI_PACKET_ACTION_FORWARD\n");
            status = SAI_STATUS_INVALID_ATTR_VALUE_0;
            return status;
        }
        fdb_entry->action = SX_FDB_ACTION_FORWARD_TO_ROUTER;
        break;

    case SAI_BRIDGE_PORT_TYPE_TUNNEL:
        if (!sdk_is_valid_ip_address(&fdb_entry->dest.next_hop.next_hop_key.next_hop_key_entry.ip_tunnel.underlay_dip))
        {
            /** default endpoint ip is 0.0.0.0 */
            status = mlnx_translate_sai_ip_address_to_sdk(&default_ip_addr,
                                                          &fdb_entry->dest.next_hop.next_hop_key.next_hop_key_entry.ip_tunnel.underlay_dip);
            if (status != SAI_STATUS_SUCCESS) {
                return status;
            }
        }

        sx_tunnel_id =
            g_sai_tunnel_db_ptr->tunnel_entry_db[bport->tunnel_idx].sx_tunnel_id_ipv4;

        if (mlnx_is_vxlan_tunnel_bridge_port(bport)) {
            fdb_entry->log_port = g_sai_db_ptr->sx_nve_log_port;
        }

        fdb_entry->dest_type = SX_FDB_UC_MAC_ADDR_DEST_TYPE_NEXT_HOP;
        fdb_entry->dest.next_hop.next_hop_key.type = SX_NEXT_HOP_TYPE_TUNNEL_ENCAP;
        fdb_entry->dest.next_hop.next_hop_key.next_hop_key_entry.ip_tunnel.tunnel_id = sx_tunnel_id;
        break;

    default:
        SX_LOG_ERR("Unsupported type of bridge port - %d\n", bport->port_type);
        status = SAI_STATUS_FAILURE;
        return status;
    }
    return status;
}

static sai_status_t mlnx_fdb_check_static_entry_exists(_In_ const sx_fdb_uc_mac_addr_params_t *fdb_entry,
                                                       _Out_ bool                             *is_exists)
{
    sx_status_t                 sx_status;
    sx_fdb_uc_mac_addr_params_t existing_fdb_entry;
    sx_fdb_uc_key_filter_t      filter;
    uint32_t                    entries_count = 1;

    assert(fdb_entry);
    assert(is_exists);

    memset(&filter, 0, sizeof(filter));
    memset(&existing_fdb_entry, 0, sizeof(existing_fdb_entry));

    sx_status = sx_api_fdb_uc_mac_addr_get(gh_sdk, DEFAULT_ETH_SWID, SX_ACCESS_CMD_GET, SX_FDB_UC_STATIC,
                                           fdb_entry, &filter, &existing_fdb_entry, &entries_count);
    if (SX_ERR(sx_status)) {
        if (SX_STATUS_ENTRY_NOT_FOUND == sx_status) {
            *is_exists = false;
            return SAI_STATUS_SUCCESS;
        }

        SX_LOG_ERR("Failed to get FDB entry - %s.\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    *is_exists = true;

    return SAI_STATUS_SUCCESS;
}

/*
 * Routine Description:
 *    Create FDB entry
 *
 * Arguments:
 *    [in] fdb_entry - fdb entry
 *    [in] attr_count - number of attributes
 *    [in] attr_list - array of attributes
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_create_fdb_entry(_In_ const sai_fdb_entry_t* fdb_entry,
                                          _In_ uint32_t               attr_count,
                                          _In_ const sai_attribute_t *attr_list)
{
    sai_status_t                 status;
    const sai_attribute_value_t *type = NULL, *action = NULL, *port = NULL, *ip_addr = NULL;
    const sai_attribute_value_t *macmove = NULL;
    uint32_t                     type_index, action_index, port_index, ip_index, macmove_index;
    sx_fdb_uc_mac_addr_params_t  mac_entry;
    bool                         is_entry_exists = false;

    SX_LOG_ENTER();

    if (NULL == fdb_entry) {
        SX_LOG_ERR("NULL fdb entry param\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    status = check_attribs_on_create_without_oid(attr_count, attr_list, SAI_OBJECT_TYPE_FDB_ENTRY);
    if (SAI_ERR(status)) {
        return status;
    }

    MLNX_LOG_ATTRS(attr_count, attr_list, SAI_OBJECT_TYPE_FDB_ENTRY);

    find_attrib_in_list(attr_count, attr_list, SAI_FDB_ENTRY_ATTR_TYPE, &type, &type_index);
    find_attrib_in_list(attr_count, attr_list, SAI_FDB_ENTRY_ATTR_PACKET_ACTION, &action, &action_index);
    find_attrib_in_list(attr_count, attr_list, SAI_FDB_ENTRY_ATTR_ENDPOINT_IP, &ip_addr, &ip_index);
    find_attrib_in_list(attr_count, attr_list, SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID, &port, &port_index);
    find_attrib_in_list(attr_count, attr_list, SAI_FDB_ENTRY_ATTR_ALLOW_MAC_MOVE, &macmove, &macmove_index);

    status = mlnx_fdb_attrs_to_sx(type,
                                  type_index,
                                  port,
                                  port_index,
                                  action,
                                  action_index,
                                  macmove,
                                  macmove_index,
                                  ip_addr,
                                  &mac_entry);
    if (SAI_ERR(status)) {
        goto out;
    }

    status = mlnx_fdb_entry_to_sdk(fdb_entry, &mac_entry);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert sai_fdb_entry_t to SDK params\n");
        goto out;
    }

    status = mlnx_fdb_check_static_entry_exists(&mac_entry, &is_entry_exists);
    if (SAI_ERR(status)) {
        goto out;
    }

    if (is_entry_exists) {
        SX_LOG_ERR("Static FDB Entry is already created\n");
        status = SAI_STATUS_ITEM_ALREADY_EXISTS;
        goto out;
    }

    status = mlnx_add_mac(&mac_entry);
    if (SAI_ERR(status)) {
        goto out;
    }

    MLNX_LOG_KEY_CREATED(SX_LOG_NOTICE, fdb_entry, SAI_OBJECT_TYPE_FDB_ENTRY);

out:
    SX_LOG_EXIT();
    return status;
}

/*
 * Routine Description:
 *    Remove FDB entry
 *
 * Arguments:
 *    [in] fdb_entry - fdb entry
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_remove_fdb_entry(_In_ const sai_fdb_entry_t* fdb_entry)
{
    sx_fdb_uc_mac_addr_params_t mac_entry;
    sai_status_t                status;

    SX_LOG_ENTER();

    if (NULL == fdb_entry) {
        SX_LOG_ERR("NULL fdb entry param\n");
        SX_LOG_EXIT();
        return SAI_STATUS_INVALID_PARAMETER;
    }

    MLNX_LOG_KEY_REMOVE(SX_LOG_NOTICE, fdb_entry, SAI_OBJECT_TYPE_FDB_ENTRY);

    status = mlnx_get_n_delete_mac(fdb_entry, &mac_entry);
    if (SAI_ERR(status)) {
        goto out;
    }
out:
    SX_LOG_EXIT();
    return status;
}

/*
 * Routine Description:
 *    Set fdb entry attribute value
 *
 * Arguments:
 *    [in] fdb_entry - fdb entry
 *    [in] attr - attribute
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_set_fdb_entry_attribute(_In_ const sai_fdb_entry_t *fdb_entry,
                                                 _In_ const sai_attribute_t *attr)
{
    sai_object_key_t key;

    if (!fdb_entry) {
        SX_LOG_ERR("Entry is NULL.\n");
        return SAI_STATUS_FAILURE;
    }

    memcpy(&key.key.fdb_entry, fdb_entry, sizeof(*fdb_entry));
    return sai_set_attribute(&key, SAI_OBJECT_TYPE_FDB_ENTRY, attr);
}

/* Set FDB entry type [sai_fdb_entry_type_t] */
static sai_status_t mlnx_fdb_type_set(_In_ const sai_object_key_t      *key,
                                      _In_ const sai_attribute_value_t *value,
                                      void                             *arg)
{
    sai_status_t                status;
    sx_fdb_uc_mac_addr_params_t old_mac_entry, new_mac_entry;
    const sai_fdb_entry_t      *fdb_entry = &key->key.fdb_entry;
    bool                        roaming = false;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS != (status = mlnx_get_mac(fdb_entry, &old_mac_entry))) {
        return status;
    }

    if ((SAI_FDB_ENTRY_TYPE_DYNAMIC == value->s32) && (SX_FDB_ACTION_FORWARD != old_mac_entry.action)) {
        SX_LOG_ERR("Failed to update FDB Entry Type - Dynamic entries can only have Forward action\n");
        return SAI_STATUS_INVALID_ATTR_VALUE_0;
    }

    new_mac_entry = old_mac_entry;

    if (SAI_STATUS_SUCCESS != (status = mlnx_translate_sai_type_to_sdk(value->s32, roaming, &new_mac_entry, 0))) {
        return status;
    }

    if (SAI_STATUS_SUCCESS != (status = mlnx_del_mac(&old_mac_entry))) {
        return status;
    }

    if (SAI_STATUS_SUCCESS != (status = mlnx_add_mac(&new_mac_entry))) {
        return status;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* FDB entry port id [sai_object_id_t] (MANDATORY_ON_CREATE|CREATE_AND_SET)
 * The port id here can refer to a generic port object such as SAI port object id,
 * SAI LAG object id and etc. on. */
static sai_status_t mlnx_fdb_port_set(_In_ const sai_object_key_t      *key,
                                      _In_ const sai_attribute_value_t *value,
                                      void                             *arg)
{
    sai_status_t                status;
    sx_fdb_uc_mac_addr_params_t old_mac_entry, new_mac_entry;
    const sai_fdb_entry_t      *fdb_entry = &key->key.fdb_entry;
    mlnx_bridge_port_t         *bport;

    SX_LOG_ENTER();

    status = mlnx_get_mac(fdb_entry, &old_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    new_mac_entry = old_mac_entry;

    mlnx_fdb_action_fetch(fdb_entry, &new_mac_entry);

    if (SAI_NULL_OBJECT_ID == value->oid) {
        if (SX_FDB_ACTION_TRAP != old_mac_entry.action) {
            new_mac_entry.action = SX_FDB_ACTION_DISCARD;
        }
        new_mac_entry.log_port = 0;
    } else {
        sai_db_read_lock();
        status = mlnx_bridge_port_by_oid(value->oid, &bport);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to lookup bridge port by oid %" PRIx64 "\n", value->oid);
            sai_db_unlock();
            return status;
        }
        status = mlnx_bridge_port_translate_to_sdk(bport, &new_mac_entry);
        if (SAI_ERR(status)) {
            sai_db_unlock();
            return status;
        }
        sai_db_unlock();
    }

    status = mlnx_del_mac(&old_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_add_mac(&new_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Set FDB entry packet action [sai_packet_action_t] */
static sai_status_t mlnx_fdb_action_set(_In_ const sai_object_key_t      *key,
                                        _In_ const sai_attribute_value_t *value,
                                        void                             *arg)
{
    sai_status_t                status;
    sx_fdb_uc_mac_addr_params_t old_mac_entry, new_mac_entry;
    const sai_fdb_entry_t      *fdb_entry = &key->key.fdb_entry;

    SX_LOG_ENTER();

    status = mlnx_get_mac(fdb_entry, &old_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    new_mac_entry = old_mac_entry;

    if (((SX_FDB_UC_AGEABLE == old_mac_entry.entry_type) || (SX_FDB_UC_REMOTE == old_mac_entry.entry_type))
        && (SAI_PACKET_ACTION_FORWARD != value->s32)) {
        SX_LOG_ERR("Failed to change action - Dynamic entries can only have Forward action\n");
        return SAI_STATUS_INVALID_ATTR_VALUE_0;
    }

    if ((SX_FDB_IS_PORT_REDUNDANT(old_mac_entry.entry_type, old_mac_entry.action)) &&
        ((SAI_PACKET_ACTION_FORWARD == value->s32) || (SAI_PACKET_ACTION_LOG == value->s32))) {
        status = mlnx_fdb_action_save(fdb_entry, value->s32);
        if (SAI_ERR(status)) {
            return status;
        }
    } else {
        status = mlnx_translate_sai_action_to_sdk(value->s32, &new_mac_entry, 0);
        if (SAI_ERR(status)) {
            return status;
        }

        mlnx_fdb_action_clear(fdb_entry);
    }

    status = mlnx_del_mac(&old_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_add_mac(&new_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Set FDB entry endpoint ip [sai_ip_address_t] */
static sai_status_t mlnx_fdb_endpoint_ip_set(_In_ const sai_object_key_t      *key,
                                             _In_ const sai_attribute_value_t *value,
                                             void                             *arg)
{
    sai_status_t                status;
    sx_fdb_uc_mac_addr_params_t old_mac_entry, new_mac_entry;
    const sai_fdb_entry_t      *fdb_entry = &key->key.fdb_entry;

    SX_LOG_ENTER();

    status = mlnx_get_mac(fdb_entry, &old_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    new_mac_entry = old_mac_entry;

    status = mlnx_translate_sai_ip_address_to_sdk(&value->ipaddr,
                                                  &new_mac_entry.dest.next_hop.next_hop_key.next_hop_key_entry.ip_tunnel.underlay_dip);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_del_mac(&old_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_add_mac(&new_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Set FDB entry allow_mac_move [bool] */
static sai_status_t mlnx_fdb_macmove_set(_In_ const sai_object_key_t      *key,
                                         _In_ const sai_attribute_value_t *value,
                                         void                             *arg)
{
    sai_status_t                status;
    sx_fdb_uc_mac_addr_params_t old_mac_entry, new_mac_entry;
    const sai_fdb_entry_t      *fdb_entry = &key->key.fdb_entry;

    SX_LOG_ENTER();

    status = mlnx_get_mac(fdb_entry, &old_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    new_mac_entry = old_mac_entry;

    if ((SX_FDB_UC_STATIC != old_mac_entry.entry_type) && (SX_FDB_UC_REMOTE != old_mac_entry.entry_type)) {
        SX_LOG_ERR("allow mac-move cannot be set to Dynamic or Invalid entries \n");
        return SAI_STATUS_INVALID_ATTR_VALUE_0;
    }

    if ((SX_FDB_ACTION_FORWARD != old_mac_entry.action)
        && (value->booldata == true) && (SX_FDB_UC_STATIC == old_mac_entry.entry_type)) {
        SX_LOG_ERR("Failed to allow FDB Entry move - Dynamic entries can only have Forward action\n");
        return SAI_STATUS_INVALID_ATTR_VALUE_0;
    }

    if ((SX_FDB_UC_REMOTE == old_mac_entry.entry_type) && (value->booldata == false)) {
        new_mac_entry.entry_type = SX_FDB_UC_STATIC;
    } else if ((SX_FDB_UC_STATIC == old_mac_entry.entry_type) && (value->booldata == true)) {
        new_mac_entry.entry_type = SX_FDB_UC_REMOTE;
    } else {
        /*no need to change*/
        SX_LOG_EXIT();
        return SAI_STATUS_SUCCESS;
    }

    status = mlnx_del_mac(&old_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_add_mac(&new_mac_entry);
    if (SAI_ERR(status)) {
        return status;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}
/*
 * Routine Description:
 *    Get fdb entry attribute value
 *
 * Arguments:
 *    [in] fdb_entry - fdb entry
 *    [in] attr_count - number of attributes
 *    [inout] attr_list - array of attributes
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_get_fdb_entry_attribute(_In_ const sai_fdb_entry_t *fdb_entry,
                                                 _In_ uint32_t               attr_count,
                                                 _Inout_ sai_attribute_t    *attr_list)
{
    sai_object_key_t key;

    if (!fdb_entry) {
        SX_LOG_ERR("Entry is NULL.\n");
        return SAI_STATUS_FAILURE;
    }

    memcpy(&key.key.fdb_entry, fdb_entry, sizeof(*fdb_entry));
    return sai_get_attributes(&key, SAI_OBJECT_TYPE_FDB_ENTRY, attr_count, attr_list);
}

static sai_status_t fill_fdb_cache(mlnx_fdb_cache_t *fdb_cache, const sai_fdb_entry_t *fdb_entry)
{
    sai_status_t                status;
    sx_fdb_uc_mac_addr_params_t mac_entry;

    if (fdb_cache->fdb_cache_set) {
        return SAI_STATUS_SUCCESS;
    }

    if (SAI_STATUS_SUCCESS != (status = mlnx_get_mac(fdb_entry, &mac_entry))) {
        return status;
    }

    fdb_cache->fdb_cache_set = true;
    fdb_cache->action = mac_entry.action;
    fdb_cache->entry_type = mac_entry.entry_type;
    fdb_cache->log_port = mac_entry.log_port;
    memcpy(&fdb_cache->endpoint_ip,
           &mac_entry.dest.next_hop.next_hop_key.next_hop_key_entry.ip_tunnel.underlay_dip,
           sizeof(fdb_cache->endpoint_ip));

    return SAI_STATUS_SUCCESS;
}

/* Get FDB entry type [sai_fdb_entry_type_t] */
static sai_status_t mlnx_fdb_type_get(_In_ const sai_object_key_t   *key,
                                      _Inout_ sai_attribute_value_t *value,
                                      _In_ uint32_t                  attr_index,
                                      _Inout_ vendor_cache_t        *cache,
                                      void                          *arg)
{
    sai_status_t           status;
    const sai_fdb_entry_t *fdb_entry = &key->key.fdb_entry;
    mlnx_fdb_cache_t      *fdb_cache = &(cache->fdb_cache);

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS != (status = fill_fdb_cache(fdb_cache, fdb_entry))) {
        return status;
    }

    switch (fdb_cache->entry_type) {
    case SX_FDB_UC_STATIC:
    case SX_FDB_UC_REMOTE:
        value->s32 = SAI_FDB_ENTRY_TYPE_STATIC;
        break;

    case SX_FDB_UC_AGEABLE:
        value->s32 = SAI_FDB_ENTRY_TYPE_DYNAMIC;
        break;

    default:
        SX_LOG_ERR("Unexpected entry type %d\n", fdb_cache->entry_type);
        return SAI_STATUS_FAILURE;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* FDB entry port id [sai_object_id_t] (MANDATORY_ON_CREATE|CREATE_AND_SET)
 * The port id here can refer to a generic port object such as SAI port object id,
 * SAI LAG object id and etc. on.
 * Port 0 is returned for entries with action = drop or action = trap
 * Since port is irrelevant for these actions, even if actual port is set
 * In case the action is changed from drop/trap to forward/log, need to also set port
 */
static sai_status_t mlnx_fdb_port_get(_In_ const sai_object_key_t   *key,
                                      _Inout_ sai_attribute_value_t *value,
                                      _In_ uint32_t                  attr_index,
                                      _Inout_ vendor_cache_t        *cache,
                                      void                          *arg)
{
    sai_status_t           status;
    const sai_fdb_entry_t *fdb_entry = &key->key.fdb_entry;
    mlnx_fdb_cache_t      *fdb_cache = &(cache->fdb_cache);

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS != (status = fill_fdb_cache(fdb_cache, fdb_entry))) {
        return status;
    }

    if (SX_FDB_ACTION_DISCARD == fdb_cache->action) {
        value->oid = SAI_NULL_OBJECT_ID;
    } else if (SX_FDB_ACTION_FORWARD_TO_ROUTER == fdb_cache->action) {
        SX_LOG_ERR("Getting a bridge port while it's 1D/1Q router is not supported\n");
        SX_LOG_EXIT();
        return SAI_STATUS_NOT_SUPPORTED;
    } else {
        status = mlnx_log_port_to_sai_bridge_port(fdb_cache->log_port, &value->oid);
        if (SAI_ERR(status)) {
            return status;
        }
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Get FDB entry packet action [sai_packet_action_t] */
static sai_status_t mlnx_fdb_action_get(_In_ const sai_object_key_t   *key,
                                        _Inout_ sai_attribute_value_t *value,
                                        _In_ uint32_t                  attr_index,
                                        _Inout_ vendor_cache_t        *cache,
                                        void                          *arg)
{
    sai_status_t           status;
    const sai_fdb_entry_t *fdb_entry = &key->key.fdb_entry;
    mlnx_fdb_cache_t      *fdb_cache = &(cache->fdb_cache);

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS != (status = fill_fdb_cache(fdb_cache, fdb_entry))) {
        return status;
    }

    switch (fdb_cache->action) {
    case SX_FDB_ACTION_FORWARD:
    case SX_FDB_ACTION_FORWARD_TO_ROUTER:
        value->s32 = SAI_PACKET_ACTION_FORWARD;
        break;

    case SX_FDB_ACTION_TRAP:
        value->s32 = SAI_PACKET_ACTION_TRAP;
        break;

    case SX_FDB_ACTION_MIRROR_TO_CPU:
        value->s32 = SAI_PACKET_ACTION_LOG;
        break;

    case SX_FDB_ACTION_DISCARD:
        value->s32 = SAI_PACKET_ACTION_DROP;
        break;

    default:
        SX_LOG_ERR("Unexpected fdb action %d\n", fdb_cache->action);
        return SAI_STATUS_FAILURE;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Get FDB entry endpoint ip [sai_ip_address_t] */
static sai_status_t mlnx_fdb_endpoint_ip_get(_In_ const sai_object_key_t   *key,
                                             _Inout_ sai_attribute_value_t *value,
                                             _In_ uint32_t                  attr_index,
                                             _Inout_ vendor_cache_t        *cache,
                                             void                          *arg)
{
    sai_status_t           status;
    const sai_fdb_entry_t *fdb_entry = &key->key.fdb_entry;
    mlnx_fdb_cache_t      *fdb_cache = &(cache->fdb_cache);

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS != (status = fill_fdb_cache(fdb_cache, fdb_entry))) {
        return status;
    }

    status = mlnx_translate_sdk_ip_address_to_sai(&fdb_cache->endpoint_ip,
                                                  &value->ipaddr);

    if (SAI_ERR(status)) {
        return status;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Get FDB entry allow_mac_move [bool] */
static sai_status_t mlnx_fdb_macmove_get(_In_ const sai_object_key_t   *key,
                                         _Inout_ sai_attribute_value_t *value,
                                         _In_ uint32_t                  attr_index,
                                         _Inout_ vendor_cache_t        *cache,
                                         void                          *arg)
{
    sai_status_t           status;
    const sai_fdb_entry_t *fdb_entry = &key->key.fdb_entry;
    mlnx_fdb_cache_t      *fdb_cache = &(cache->fdb_cache);

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS != (status = fill_fdb_cache(fdb_cache, fdb_entry))) {
        return status;
    }

    switch (fdb_cache->entry_type) {
    case SX_FDB_UC_STATIC:
        value->booldata = false;
        break;

    case SX_FDB_UC_REMOTE:
        value->booldata = true;
        break;

    default:
        SX_LOG_ERR("entry type %d has no mac-move attribute\n", fdb_cache->entry_type);
        return SAI_STATUS_FAILURE;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}
/*
 * Routine Description:
 *    Remove all FDB entries by attribute set in sai_fdb_flush_attr
 *
 * Arguments:
 *    [in] attr_count - number of attributes
 *    [in] attr_list - array of attributes
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_flush_fdb_entries(_In_ sai_object_id_t        switch_id,
                                           _In_ uint32_t               attr_count,
                                           _In_ const sai_attribute_t *attr_list)
{
    sx_status_t                  status;
    const sai_attribute_value_t *port, *bv_id, *type;
    uint32_t                     port_index, bv_id_index, type_index;
    bool                         port_found = false, bv_id_found = false;
    sx_port_log_id_t             port_id;
    sx_fid_t                     sx_fid;

    SX_LOG_ENTER();

    status = check_attribs_on_create_without_oid(attr_count, attr_list, SAI_OBJECT_TYPE_FDB_FLUSH);
    if (SAI_ERR(status)) {
        return status;
    }

    MLNX_LOG_ATTRS(attr_count, attr_list, SAI_OBJECT_TYPE_FDB_FLUSH);

    if (SAI_STATUS_SUCCESS ==
        (status =
             find_attrib_in_list(attr_count, attr_list, SAI_FDB_FLUSH_ATTR_BRIDGE_PORT_ID,
                                 &port, &port_index))) {
        port_found = true;

        if (mlnx_is_vxlan_tunnel_bport_oid(port->oid)) {
            sai_db_read_lock();
            port_id = g_sai_db_ptr->sx_nve_log_port;
            sai_db_unlock();
        } else {
            status = mlnx_bridge_port_sai_to_log_port(port->oid, &port_id);
            if (SAI_ERR(status)) {
                return status;
            }
        }
    }

    if (SAI_STATUS_SUCCESS ==
        (status =
             find_attrib_in_list(attr_count, attr_list, SAI_FDB_FLUSH_ATTR_BV_ID,
                                 &bv_id, &bv_id_index))) {
        if (mlnx_bridge_default_1q_oid() == bv_id->oid) {
            SX_LOG_ERR("Flush by .1Q bridge is not implemented.\n");
            return SAI_STATUS_ATTR_NOT_IMPLEMENTED_0 + bv_id_index;
        }
        bv_id_found = true;
        status = mlnx_fdb_bv_id_to_sx_fid(bv_id->oid, &sx_fid);
        if (SAI_ERR(status)) {
            return status;
        }
    }

    if (SAI_STATUS_SUCCESS ==
        (status =
             find_attrib_in_list(attr_count, attr_list, SAI_FDB_FLUSH_ATTR_ENTRY_TYPE,
                                 &type, &type_index))) {
        if (SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC != type->s32) {
            SX_LOG_ERR("Flush of static FDB entries is not implemented, got %d.\n", type->s32);
            return SAI_STATUS_ATTR_NOT_IMPLEMENTED_0 + type_index;
        }
    }

    /* Mellanox implementation flushes only dynamic entries. Static entries should be deleted with entry remove */
    if ((!port_found) && (!bv_id_found)) {
        if (SX_STATUS_SUCCESS != (status = sx_api_fdb_uc_flush_all_set(gh_sdk, DEFAULT_ETH_SWID))) {
            SX_LOG_ERR("Failed to flush all fdb entries - %s.\n", SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    } else if ((port_found) && (bv_id_found)) {
        if (SX_STATUS_SUCCESS != (status = sx_api_fdb_uc_flush_port_fid_set(gh_sdk, port_id, sx_fid))) {
            SX_LOG_ERR("Failed to flush port vlan fdb entries - %s.\n", SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    } else if (port_found) {
        if (SX_STATUS_SUCCESS != (status = sx_api_fdb_uc_flush_port_set(gh_sdk, port_id))) {
            SX_LOG_ERR("Failed to flush port fdb entries - %s.\n", SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    } else if (bv_id_found) {
        if (SX_STATUS_SUCCESS != (status = sx_api_fdb_uc_flush_fid_set(gh_sdk, DEFAULT_ETH_SWID, sx_fid))) {
            SX_LOG_ERR("Failed to flush vlan fdb entries - %s.\n", SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_fdb_log_set(sx_verbosity_level_t level)
{
    LOG_VAR_NAME(__MODULE__) = level;

    if (gh_sdk) {
        return sdk_to_sai(sx_api_fdb_log_verbosity_level_set(gh_sdk, SX_LOG_VERBOSITY_BOTH, level, level));
    } else {
        return SAI_STATUS_SUCCESS;
    }
}

const sai_fdb_api_t mlnx_fdb_api = {
    mlnx_create_fdb_entry,
    mlnx_remove_fdb_entry,
    mlnx_set_fdb_entry_attribute,
    mlnx_get_fdb_entry_attribute,
    mlnx_flush_fdb_entries,
    NULL,
    NULL,
    NULL,
    NULL
};
