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
#include <errno.h>

#undef  __MODULE__
#define __MODULE__ SAI_PORT

static sx_verbosity_level_t LOG_VAR_NAME(__MODULE__) = SX_VERBOSITY_LEVEL_WARNING;
sai_status_t mlnx_sai_port_hw_lanes_get(_In_ sx_port_log_id_t *port_id, _Inout_ sai_attribute_value_t *value);
extern uint32_t mlnx_cells_to_bytes(uint32_t cells);
static sai_status_t mlnx_port_tc_get(_In_ const sai_object_id_t port, _Out_ uint8_t *tc);
static sai_status_t mlnx_port_state_set(_In_ const sai_object_key_t      *key,
                                        _In_ const sai_attribute_value_t *value,
                                        void                             *arg);
static sai_status_t mlnx_port_internal_loopback_set_impl(_In_ sx_port_log_id_t                  port_id,
                                                         _In_ sai_port_internal_loopback_mode_t loop_mode);
static sai_status_t mlnx_port_internal_loopback_set(_In_ const sai_object_key_t      *key,
                                                    _In_ const sai_attribute_value_t *value,
                                                    void                             *arg);
static sai_status_t mlnx_port_mtu_set(_In_ const sai_object_key_t      *key,
                                      _In_ const sai_attribute_value_t *value,
                                      void                             *arg);
static sai_status_t mlnx_port_global_flow_ctrl_set_impl(_In_ sx_port_log_id_t             port_id,
                                                        _In_ sai_port_flow_control_mode_t pfc_mode);
static sai_status_t mlnx_port_global_flow_ctrl_set(_In_ const sai_object_key_t      *key,
                                                   _In_ const sai_attribute_value_t *value,
                                                   void                             *arg);
static sai_status_t mlnx_port_fec_set(_In_ const sai_object_key_t      *key,
                                      _In_ const sai_attribute_value_t *value,
                                      void                             *arg);
static sai_status_t mlnx_port_attr_set(_In_ const sai_object_key_t      *key,
                                       _In_ const sai_attribute_value_t *value,
                                       void                             *arg);
static sai_status_t mlnx_port_type_get(_In_ const sai_object_key_t   *key,
                                       _Inout_ sai_attribute_value_t *value,
                                       _In_ uint32_t                  attr_index,
                                       _Inout_ vendor_cache_t        *cache,
                                       void                          *arg);
static sai_status_t mlnx_port_state_get(_In_ const sai_object_key_t   *key,
                                        _Inout_ sai_attribute_value_t *value,
                                        _In_ uint32_t                  attr_index,
                                        _Inout_ vendor_cache_t        *cache,
                                        void                          *arg);
static sai_status_t mlnx_port_hw_lanes_get(_In_ const sai_object_key_t   *key,
                                           _Inout_ sai_attribute_value_t *value,
                                           _In_ uint32_t                  attr_index,
                                           _Inout_ vendor_cache_t        *cache,
                                           void                          *arg);
static sai_status_t mlnx_port_supported_breakout_get(_In_ const sai_object_key_t   *key,
                                                     _Inout_ sai_attribute_value_t *value,
                                                     _In_ uint32_t                  attr_index,
                                                     _Inout_ vendor_cache_t        *cache,
                                                     void                          *arg);
static sai_status_t mlnx_port_current_breakout_get(_In_ const sai_object_key_t   *key,
                                                   _Inout_ sai_attribute_value_t *value,
                                                   _In_ uint32_t                  attr_index,
                                                   _Inout_ vendor_cache_t        *cache,
                                                   void                          *arg);
static sai_status_t mlnx_port_supported_speed_get(_In_ const sai_object_key_t   *key,
                                                  _Inout_ sai_attribute_value_t *value,
                                                  _In_ uint32_t                  attr_index,
                                                  _Inout_ vendor_cache_t        *cache,
                                                  void                          *arg);
static sai_status_t mlnx_port_supported_fec_mode_get(_In_ const sai_object_key_t   *key,
                                                     _Inout_ sai_attribute_value_t *value,
                                                     _In_ uint32_t                  attr_index,
                                                     _Inout_ vendor_cache_t        *cache,
                                                     void                          *arg);
static sai_status_t mlnx_port_supported_auto_neg_mode_get(_In_ const sai_object_key_t   *key,
                                                          _Inout_ sai_attribute_value_t *value,
                                                          _In_ uint32_t                  attr_index,
                                                          _Inout_ vendor_cache_t        *cache,
                                                          void                          *arg);
static sai_status_t mlnx_port_supported_flow_control_mode_get(_In_ const sai_object_key_t   *key,
                                                              _Inout_ sai_attribute_value_t *value,
                                                              _In_ uint32_t                  attr_index,
                                                              _Inout_ vendor_cache_t        *cache,
                                                              void                          *arg);
static sai_status_t mlnx_port_number_of_priority_groups_get(_In_ const sai_object_key_t   *key,
                                                            _Inout_ sai_attribute_value_t *value,
                                                            _In_ uint32_t                  attr_index,
                                                            _Inout_ vendor_cache_t        *cache,
                                                            void                          *arg);
static sai_status_t mlnx_port_priority_group_list_get(_In_ const sai_object_key_t   *key,
                                                      _Inout_ sai_attribute_value_t *value,
                                                      _In_ uint32_t                  attr_index,
                                                      _Inout_ vendor_cache_t        *cache,
                                                      void                          *arg);
static sai_status_t mlnx_port_oper_speed_get(_In_ const sai_object_key_t   *key,
                                             _Inout_ sai_attribute_value_t *value,
                                             _In_ uint32_t                  attr_index,
                                             _Inout_ vendor_cache_t        *cache,
                                             void                          *arg);
static sai_status_t mlnx_port_lag_drop_tags_set_impl(_In_ mlnx_port_config_t *port_config,
                                                     _In_ sai_attr_id_t       attr_id,
                                                     _In_ bool                enable);
static sai_status_t mlnx_port_fec_get(_In_ const sai_object_key_t   *key,
                                      _Inout_ sai_attribute_value_t *value,
                                      _In_ uint32_t                  attr_index,
                                      _Inout_ vendor_cache_t        *cache,
                                      void                          *arg);
static sai_status_t mlnx_port_duplex_get(_In_ const sai_object_key_t   *key,
                                         _Inout_ sai_attribute_value_t *value,
                                         _In_ uint32_t                  attr_index,
                                         _Inout_ vendor_cache_t        *cache,
                                         void                          *arg);
static sai_status_t mlnx_port_attr_get(_In_ const sai_object_key_t   *key,
                                       _Inout_ sai_attribute_value_t *value,
                                       _In_ uint32_t                  attr_index,
                                       _Inout_ vendor_cache_t        *cache,
                                       void                          *arg);
static sai_status_t mlnx_port_internal_loopback_get(_In_ const sai_object_key_t   *key,
                                                    _Inout_ sai_attribute_value_t *value,
                                                    _In_ uint32_t                  attr_index,
                                                    _Inout_ vendor_cache_t        *cache,
                                                    void                          *arg);
static sai_status_t mlnx_port_mtu_get(_In_ const sai_object_key_t   *key,
                                      _Inout_ sai_attribute_value_t *value,
                                      _In_ uint32_t                  attr_index,
                                      _Inout_ vendor_cache_t        *cache,
                                      void                          *arg);
static sai_status_t mlnx_port_global_flow_ctrl_get(_In_ const sai_object_key_t   *key,
                                                   _Inout_ sai_attribute_value_t *value,
                                                   _In_ uint32_t                  attr_index,
                                                   _Inout_ vendor_cache_t        *cache,
                                                   void                          *arg);
static sai_status_t mlnx_port_update_dscp_get(_In_ const sai_object_key_t   *key,
                                              _Inout_ sai_attribute_value_t *value,
                                              _In_ uint32_t                  attr_index,
                                              _Inout_ vendor_cache_t        *cache,
                                              void                          *arg);
static sai_status_t mlnx_port_update_dscp_set_impl(_In_ sx_port_log_id_t port_id, _In_ bool enable);
static sai_status_t mlnx_port_update_dscp_set(_In_ const sai_object_key_t      *key,
                                              _In_ const sai_attribute_value_t *value,
                                              void                             *arg);
static sai_status_t mlnx_port_qos_default_tc_get(_In_ const sai_object_key_t   *key,
                                                 _Inout_ sai_attribute_value_t *value,
                                                 _In_ uint32_t                  attr_index,
                                                 _Inout_ vendor_cache_t        *cache,
                                                 void                          *arg);
static sai_status_t mlnx_port_qos_default_tc_set_impl(_In_ mlnx_port_config_t *port_config,
                                                      _In_ sx_port_log_id_t    port_id,
                                                      _In_ uint8_t             tc);
static sai_status_t mlnx_port_qos_default_tc_set(_In_ const sai_object_key_t      *key,
                                                 _In_ const sai_attribute_value_t *value,
                                                 void                             *arg);
static sai_status_t mlnx_port_qos_map_id_get(_In_ const sai_object_key_t   *key,
                                             _Inout_ sai_attribute_value_t *value,
                                             _In_ uint32_t                  attr_index,
                                             _Inout_ vendor_cache_t        *cache,
                                             void                          *arg);
static sai_status_t mlnx_port_qos_map_id_set_impl(_In_ sai_object_id_t    port,
                                                  _In_ sai_object_id_t    qos_map_id,
                                                  _In_ sai_qos_map_type_t qos_map_type);
static sai_status_t mlnx_port_qos_map_id_set(_In_ const sai_object_key_t      *key,
                                             _In_ const sai_attribute_value_t *value,
                                             void                             *arg);
static sai_status_t mlnx_port_mirror_session_get(_In_ const sai_object_key_t   *key,
                                                 _Inout_ sai_attribute_value_t *value,
                                                 _In_ uint32_t                  attr_index,
                                                 _Inout_ vendor_cache_t        *cache,
                                                 void                          *arg);
static sai_status_t mlnx_port_mirror_session_set_impl(_In_ sx_port_log_id_t             port_id,
                                                      _In_ sx_mirror_direction_t        sdk_mirror_direction,
                                                      _In_ const sai_attribute_value_t *value);
static sai_status_t mlnx_port_mirror_session_set(_In_ const sai_object_key_t      *key,
                                                 _In_ const sai_attribute_value_t *value,
                                                 void                             *arg);
static sai_status_t mlnx_port_samplepacket_session_get(_In_ const sai_object_key_t   *key,
                                                       _Inout_ sai_attribute_value_t *value,
                                                       _In_ uint32_t                  attr_index,
                                                       _Inout_ vendor_cache_t        *cache,
                                                       void                          *arg);
static sai_status_t mlnx_port_samplepacket_session_set_impl(_In_ mlnx_port_config_t *port_config,
                                                            _In_ sx_port_log_id_t    sx_port_log_id,
                                                            _In_ sai_object_id_t     sp_oid);
static sai_status_t mlnx_port_samplepacket_session_set(_In_ const sai_object_key_t      *key,
                                                       _In_ const sai_attribute_value_t *value,
                                                       void                             *arg);
static sai_status_t mlnx_port_pfc_control_get(_In_ const sai_object_key_t   *key,
                                              _Inout_ sai_attribute_value_t *value,
                                              _In_ uint32_t                  attr_index,
                                              _Inout_ vendor_cache_t        *cache,
                                              void                          *arg);
static sai_status_t mlnx_port_pfc_control_mode_set_impl(_In_ sx_port_log_id_t                      port_id,
                                                        _In_ sai_port_priority_flow_control_mode_t pfc_mode);
static sai_status_t mlnx_port_pfc_control_mode_set(_In_ const sai_object_key_t      *key,
                                                   _In_ const sai_attribute_value_t *value,
                                                   void                             *arg);
static sai_status_t mlnx_port_pfc_control_set_impl(_In_ sx_port_log_id_t             port_id,
                                                   _In_ const sai_attribute_value_t *value,
                                                   _In_ int32_t                      arg);
static sai_status_t mlnx_port_pfc_control_set(_In_ const sai_object_key_t      *key,
                                              _In_ const sai_attribute_value_t *value,
                                              void                             *arg);
static sai_status_t mlnx_port_queue_num_get(_In_ const sai_object_key_t   *key,
                                            _Inout_ sai_attribute_value_t *value,
                                            _In_ uint32_t                  attr_index,
                                            _Inout_ vendor_cache_t        *cache,
                                            void                          *arg);
static sai_status_t mlnx_port_queue_list_get(_In_ const sai_object_key_t   *key,
                                             _Inout_ sai_attribute_value_t *value,
                                             _In_ uint32_t                  attr_index,
                                             _Inout_ vendor_cache_t        *cache,
                                             void                          *arg);
static sai_status_t mlnx_port_pool_list_get(_In_ const sai_object_key_t   *key,
                                            _Inout_ sai_attribute_value_t *value,
                                            _In_ uint32_t                  attr_index,
                                            _Inout_ vendor_cache_t        *cache,
                                            void                          *arg);
static sai_status_t mlnx_port_max_headroom_size_get(_In_ const sai_object_key_t   *key,
                                                    _Inout_ sai_attribute_value_t *value,
                                                    _In_ uint32_t                  attr_index,
                                                    _Inout_ vendor_cache_t        *cache,
                                                    void                          *arg);
static sai_status_t mlnx_port_sched_groups_num_get(_In_ const sai_object_key_t   *key,
                                                   _Inout_ sai_attribute_value_t *value,
                                                   _In_ uint32_t                  attr_index,
                                                   _Inout_ vendor_cache_t        *cache,
                                                   void                          *arg);
static sai_status_t mlnx_port_sched_groups_list_get(_In_ const sai_object_key_t   *key,
                                                    _Inout_ sai_attribute_value_t *value,
                                                    _In_ uint32_t                  attr_index,
                                                    _Inout_ vendor_cache_t        *cache,
                                                    void                          *arg);
static sai_status_t mlnx_port_sched_get(_In_ const sai_object_key_t   *key,
                                        _Inout_ sai_attribute_value_t *value,
                                        _In_ uint32_t                  attr_index,
                                        _Inout_ vendor_cache_t        *cache,
                                        void                          *arg);
static sai_status_t mlnx_port_sched_set(_In_ const sai_object_key_t      *key,
                                        _In_ const sai_attribute_value_t *value,
                                        void                             *arg);
static sai_status_t mlnx_port_ingress_buffer_profile_list_get(_In_ const sai_object_key_t   *key,
                                                              _Inout_ sai_attribute_value_t *value,
                                                              _In_ uint32_t                  attr_index,
                                                              _Inout_ vendor_cache_t        *cache,
                                                              void                          *arg);
static sai_status_t mlnx_port_ingress_buffer_profile_list_set(_In_ const sai_object_key_t      *key,
                                                              _In_ const sai_attribute_value_t *value,
                                                              void                             *arg);
static sai_status_t mlnx_port_egress_buffer_profile_list_get(_In_ const sai_object_key_t   *key,
                                                             _Inout_ sai_attribute_value_t *value,
                                                             _In_ uint32_t                  attr_index,
                                                             _Inout_ vendor_cache_t        *cache,
                                                             void                          *arg);
static sai_status_t mlnx_port_egress_buffer_profile_list_set(_In_ const sai_object_key_t      *key,
                                                             _In_ const sai_attribute_value_t *value,
                                                             void                             *arg);
static sai_status_t mlnx_port_storm_control_policer_attr_set(_In_ const sai_object_key_t      *key,
                                                             _In_ const sai_attribute_value_t *value,
                                                             _In_ void                        *arg);
static sai_status_t mlnx_port_storm_control_policer_attr_get(_In_ const sai_object_key_t   *key,
                                                             _Inout_ sai_attribute_value_t *value,
                                                             _In_ uint32_t                  attr_index,
                                                             _Inout_ vendor_cache_t        *cache,
                                                             _In_ void                     *arg);
static sai_status_t mlnx_port_egress_block_set(_In_ const sai_object_key_t      *key,
                                               _In_ const sai_attribute_value_t *value,
                                               _In_ void                        *arg);
static sai_status_t mlnx_port_egress_block_get(_In_ const sai_object_key_t   *key,
                                               _Inout_ sai_attribute_value_t *value,
                                               _In_ uint32_t                  attr_index,
                                               _Inout_ vendor_cache_t        *cache,
                                               _In_ void                     *arg);
static sai_status_t mlnx_port_isolation_group_set(_In_ const sai_object_key_t      *key,
                                                  _In_ const sai_attribute_value_t *value,
                                                  _In_ void                        *arg);
static sai_status_t mlnx_port_isolation_group_get(_In_ const sai_object_key_t   *key,
                                                  _Inout_ sai_attribute_value_t *value,
                                                  _In_ uint32_t                  attr_index,
                                                  _Inout_ vendor_cache_t        *cache,
                                                  _In_ void                     *arg);
static sai_status_t mlnx_port_egress_block_sai_ports_to_sx(_In_ sx_port_log_id_t       sx_ing_port_id,
                                                           _In_ const sai_object_id_t *egress_ports,
                                                           _In_ uint32_t               egress_ports_count,
                                                           _In_ uint32_t               attr_index,
                                                           _Out_ sx_port_log_id_t     *sx_egress_ports);
static sai_status_t mlnx_port_egress_block_set_impl(_In_ sx_port_log_id_t        sx_ing_port_id,
                                                    _In_ const sx_port_log_id_t *sx_egress_block_port_list,
                                                    _In_ uint32_t                egress_ports_count);
static sai_status_t mlnx_port_egress_block_get_impl(_In_ sx_port_log_id_t   sx_ing_port_id,
                                                    _Out_ sx_port_log_id_t *sx_egress_block_ports,
                                                    _Inout_ uint32_t       *sx_egress_block_ports_count);
static sai_status_t mlnx_port_mirror_session_clear(_In_ sx_port_log_id_t      port_id,
                                                   _In_ sx_mirror_direction_t sx_mirror_direction);
static sai_status_t mlnx_port_mirror_session_apply(_In_ sx_port_log_id_t      port_id,
                                                   _In_ sx_span_session_id_t  sx_span_session_id,
                                                   _In_ sx_mirror_direction_t sx_mirror_direction);
static sai_status_t mlnx_port_mirror_session_set_internal(_In_ sx_port_log_id_t      port_id,
                                                          _In_ sx_mirror_direction_t sx_mirror_direction,
                                                          _In_ sx_span_session_id_t  sx_span_session_id,
                                                          _In_ bool                  add);
static sai_status_t mlnx_port_samplepacket_session_set_internal(_In_ mlnx_port_config_t *port_config,
                                                                _In_ sx_port_log_id_t    sx_port_log_id,
                                                                _In_ uint32_t            samplepacket_obj_idx);
static sai_status_t mlnx_port_pool_attr_get(_In_ const sai_object_key_t   *key,
                                            _Inout_ sai_attribute_value_t *value,
                                            _In_ uint32_t                  attr_index,
                                            _Inout_ vendor_cache_t        *cache,
                                            _In_ void                     *arg);
static sai_status_t mlnx_port_speed_get_impl(_In_ sx_port_log_id_t sx_port,
                                             _Out_ uint32_t       *oper_speed,
                                             _Out_ uint32_t       *admin_speed);
static sai_status_t mlnx_port_supported_speeds_get_impl(_In_ sx_port_log_id_t sx_port, _Inout_ sai_u32_list_t *list);
sai_status_t mlnx_port_wred_mirror_set_impl(_In_ sx_port_log_id_t     sx_port,
                                            _In_ sx_span_session_id_t sx_session,
                                            _In_ bool                 is_add);
static sai_status_t mlnx_port_speed_get_sp(_In_ sx_port_log_id_t sx_port,
                                           _Out_ uint32_t       *oper_speed,
                                           _Out_ uint32_t       *admin_speed);
static sai_status_t mlnx_port_speed_get_sp2(_In_ sx_port_log_id_t sx_port,
                                            _Out_ uint32_t       *oper_speed,
                                            _Out_ uint32_t       *admin_speed);
static sai_status_t mlnx_port_supported_speeds_get_sp(_In_ sx_port_log_id_t sx_port,
                                                      _Out_ uint32_t       *speeds,
                                                      _Inout_ uint32_t     *speeds_count);
static sai_status_t mlnx_port_supported_speeds_get_sp2(_In_ sx_port_log_id_t sx_port,
                                                       _Out_ uint32_t       *speeds,
                                                       _Inout_ uint32_t     *speeds_count);
static sai_status_t mlnx_port_speed_bitmap_apply_sp(_In_ mlnx_port_config_t *port);
static sai_status_t mlnx_port_speed_bitmap_apply_sp2(_In_ mlnx_port_config_t *port);
static sai_status_t mlnx_port_wred_mirror_set_sp(_In_ sx_port_log_id_t     sx_port,
                                                 _In_ sx_span_session_id_t sx_session,
                                                 _In_ bool                 is_add);
static sai_status_t mlnx_port_wred_mirror_set_sp2(_In_ sx_port_log_id_t     sx_port,
                                                  _In_ sx_span_session_id_t sx_session,
                                                  _In_ bool                 is_add);
static sai_status_t mlnx_port_update_speed(_In_ mlnx_port_config_t *port);
static sai_status_t mlnx_port_update_speed_sp(_In_ sx_port_log_id_t sx_port,
                                              _In_ bool             auto_neg,
                                              _In_ uint64_t         bitmap);
static sai_status_t mlnx_port_update_speed_sp2(_In_ sx_port_log_id_t sx_port,
                                               _In_ bool             auto_neg,
                                               _In_ uint64_t         bitmap);
static sai_status_t mlnx_port_update_speed_sp4(_In_ sx_port_log_id_t sx_port,
                                               _In_ bool             auto_neg,
                                               _In_ uint64_t         bitmap);
static sai_status_t mlnx_port_dscp_remapping_uplink_detect(_In_ sx_port_log_id_t port_id,
                                                           _Out_ bool           *should_be_uplink_port);
static sai_status_t mlnx_port_lossless_buffer_count_get(_In_ sx_port_log_id_t port_id,
                                                        _Out_ uint32_t       *number_of_lossless_buffer);
static sai_status_t mlnx_port_tc_to_queue_update_for_dscp_remapping(_In_ sx_port_log_id_t port_id,
                                                                    _In_ sai_object_id_t  tc_to_queue_mapping_oid);
static sai_status_t mlnx_port_tc_to_pg_update_for_dscp_remapping(_In_ sx_port_log_id_t port_id,
                                                                 _In_ sai_object_id_t  tc_to_pg_mapping_oid);
static sai_status_t mlnx_port_do_dscp_rewriting(_In_ sx_port_log_id_t port_id,
                                                _In_ uint32_t         index);
static sai_status_t mlnx_port_remove_port_from_dscp_remapping_uplink_list(_In_ sx_port_log_id_t port_id);
static sai_status_t mlnx_port_add_port_to_dscp_remapping_uplink_list(_In_ sx_port_log_id_t port_id,
                                                                     _Out_ uint32_t       *index);
static sai_status_t mlnx_port_undo_dscp_rewriting(_In_ sx_port_log_id_t port_id,
                                                  _In_ uint32_t         index);
static bool mlnx_port_is_in_uplink_list(_In_ sx_port_log_id_t port_id);
static sai_status_t mlnx_check_and_store_ar_port_id(_In_ mlnx_port_config_t *port);


enum counter_type {
    CNT_2863    = 1 << 0,
    CNT_2819    = 1 << 1,
    CNT_802     = 1 << 2,
    CNT_PRIO    = 1 << 3,
    CNT_DISCARD = 1 << 4,
    CNT_PERF    = 1 << 5,
    CNT_3635    = 1 << 6,
    CNT_REDECN  = 1 << 7,
    CNT_BUFF    = 1 << 8,
    CNT_TC      = 1 << 9,
    CNT_IP      = 1 << 10,
};

enum mlnx_speed_bitmap_sp {
    SP_1GB_CX_SGMII  = 1 << 0,
    SP_1GB_KX        = 1 << 1,
    SP_10GB_CX4_XAUI = 1 << 2,
    SP_10GB_KX4      = 1 << 3,
    SP_10GB_KR       = 1 << 4,
    SP_20GB_KR2      = 1 << 5,
    SP_40GB_CR4      = 1 << 6,
    SP_40GB_KR4      = 1 << 7,
    SP_56GB_KR4      = 1 << 8,
    SP_56GB_KX4      = 1 << 9,
    SP_10GB_CR       = 1 << 10,
    SP_10GB_SR       = 1 << 11,
    SP_10GB_ER_LR    = 1 << 12,
    SP_40GB_SR4      = 1 << 13,
    SP_40GB_LR4_ER4  = 1 << 14,
    SP_100GB_CR4     = 1 << 15,
    SP_100GB_SR4     = 1 << 16,
    SP_100GB_KR4     = 1 << 17,
    SP_100GB_LR4_ER4 = 1 << 18,
    SP_25GB_CR       = 1 << 19,
    SP_25GB_KR       = 1 << 20,
    SP_25GB_SR       = 1 << 21,
    SP_50GB_CR2      = 1 << 22,
    SP_50GB_KR2      = 1 << 23,
    SP_50GB_SR2      = 1 << 24,
    SP_10MB_T        = 1 << 25,
    SP_100MB_TX      = 1 << 26,
    SP_1000MB_T      = 1 << 27,
    SP_auto          = 1 << 28,
    SP_MAX           = 1 << 29,
};

uint64_t mlnx_port_speed_bitmap_sp[MAX_NUM_PORT_SPEEDS] = {
    (0),
    (SP_100MB_TX),
    (SP_1GB_CX_SGMII | SP_1GB_KX | SP_1000MB_T),
    (SP_10GB_CX4_XAUI | SP_10GB_KR | SP_10GB_CR | SP_10GB_SR | SP_10GB_ER_LR | SP_10GB_KX4),
    (SP_20GB_KR2),
    (SP_25GB_CR | SP_25GB_SR | SP_25GB_KR),
    (SP_40GB_CR4 | SP_40GB_SR4 | SP_40GB_LR4_ER4 | SP_40GB_KR4),
    (SP_50GB_CR2 | SP_50GB_KR2 | SP_50GB_SR2),
    (SP_56GB_KR4 | SP_56GB_KX4),
    (SP_100GB_CR4 | SP_100GB_SR4 | SP_100GB_LR4_ER4 | SP_100GB_KR4),
    (0),
    (0),
    (0),
    (SP_10MB_T)
};

uint64_t mlnx_port_intf_bitmap_sp[MAX_NUM_PORT_INTFS] = {
    (SP_MAX - 1),
    (SP_10GB_CR | SP_25GB_CR | SP_10MB_T | SP_100MB_TX | SP_1000MB_T),
    (SP_50GB_CR2),
    (SP_40GB_CR4 | SP_100GB_CR4),
    (SP_10GB_SR | SP_25GB_SR),
    (SP_50GB_SR2),
    (SP_40GB_SR4 | SP_100GB_SR4),
    (SP_10GB_ER_LR),
    (SP_40GB_LR4_ER4 | SP_100GB_LR4_ER4),
    (SP_10GB_KR | SP_25GB_KR),
    (SP_40GB_KR4 | SP_56GB_KR4 | SP_100GB_KR4),
    (SP_100GB_CR4 | SP_100GB_KR4 | SP_100GB_LR4_ER4 | SP_100GB_SR4),
    (SP_1GB_CX_SGMII),
    (SP_10GB_CR | SP_10GB_KR | SP_10GB_ER_LR | SP_10GB_SR),
    (SP_40GB_CR4 | SP_40GB_KR4 | SP_40GB_LR4_ER4 | SP_40GB_SR4),
    (SP_20GB_KR2 | SP_50GB_KR2),
    (SP_100GB_CR4 | SP_100GB_KR4 | SP_100GB_LR4_ER4 | SP_100GB_SR4),
    (SP_10GB_CX4_XAUI | SP_10GB_KX4),
    (SP_10GB_ER_LR | SP_10GB_SR),
    (0),
    (0),
    (0),
    (0),
    (0)
};

uint64_t mlnx_port_lanes_speed_bitmask_sp[MAX_LANES_SPC3_4 + 1] = {
    (0),
    /* 1 lane */
    (SP_25GB_CR | SP_25GB_KR | SP_25GB_SR | SP_10GB_CR | SP_10GB_KR | SP_10GB_ER_LR | SP_10GB_SR | SP_1GB_CX_SGMII |
     SP_1GB_KX | SP_10MB_T | SP_100MB_TX | SP_1000MB_T),
    /* 2 lanes */
    (SP_50GB_CR2 | SP_50GB_KR2 | SP_50GB_SR2 | SP_20GB_KR2 |
     SP_25GB_CR | SP_25GB_KR | SP_25GB_SR | SP_10GB_CR | SP_10GB_KR | SP_10GB_ER_LR | SP_10GB_SR | SP_1GB_CX_SGMII |
     SP_1GB_KX | SP_10MB_T | SP_100MB_TX | SP_1000MB_T),
    (0),
    /* 4 lanes */
    (SP_100GB_CR4 | SP_100GB_KR4 | SP_100GB_LR4_ER4 | SP_100GB_SR4 | SP_56GB_KR4 | SP_56GB_KX4 | SP_40GB_CR4 |
     SP_40GB_KR4 | SP_40GB_LR4_ER4 | SP_40GB_SR4 | SP_10GB_CX4_XAUI | SP_10GB_KX4 |
     SP_50GB_CR2 | SP_50GB_KR2 | SP_50GB_SR2 | SP_20GB_KR2 |
     SP_25GB_CR | SP_25GB_KR | SP_25GB_SR | SP_10GB_CR | SP_10GB_KR | SP_10GB_ER_LR | SP_10GB_SR | SP_1GB_CX_SGMII |
     SP_1GB_KX | SP_10MB_T | SP_100MB_TX | SP_1000MB_T),
    (0),
    (0),
    (0),
    (0),
};

enum mlnx_speed_bitmap_sp2 {
    SP2_100M   = 1 << 0,
    SP2_1G     = 1 << 1,
    SP2_10G    = 1 << 2,
    SP2_25G    = 1 << 3,
    SP2_40G    = 1 << 4,
    SP2_50G    = 1 << 5,
    SP2_50Gx1  = 1 << 6,
    SP2_50Gx2  = 1 << 7,
    SP2_100G   = 1 << 8,
    SP2_100Gx2 = 1 << 9,
    SP2_100Gx4 = 1 << 10,
    SP2_200G   = 1 << 11,
    SP2_200Gx4 = 1 << 12,
    SP2_400G   = 1 << 13,
    SP2_400Gx8 = 1 << 14,
    SP2_auto   = 1 << 15,
    SP2_MAX    = 1 << 16
};

uint64_t mlnx_port_speed_bitmap_sp2[MAX_NUM_PORT_SPEEDS] = {
    (0),
    (SP2_100M),
    (SP2_1G),
    (SP2_10G),
    (0),
    (SP2_25G),
    (SP2_40G),
    (SP2_50G | SP2_50Gx1 | SP2_50Gx2),
    (0),
    (SP2_100G | SP2_100Gx2 | SP2_100Gx4),
    (SP2_200G | SP2_200Gx4),
    (SP2_400G | SP2_400Gx8),
    (0),
    (0)
};

uint64_t mlnx_port_intf_bitmap_sp2[MAX_NUM_PORT_INTFS] = {
    (SP2_MAX - 1),
    (SP2_100M | SP2_1G | SP2_10G | SP2_25G | SP2_50Gx1),
    (SP2_50Gx2 | SP2_100Gx2),
    (SP2_40G | SP2_100Gx4 | SP2_200Gx4 | SP2_400G),
    (SP2_100M | SP2_1G | SP2_10G | SP2_25G | SP2_50Gx1),
    (SP2_50Gx2 | SP2_100Gx2),
    (SP2_40G | SP2_100Gx4 | SP2_200Gx4 | SP2_400G),
    (SP2_100M | SP2_1G | SP2_10G | SP2_25G | SP2_50Gx1),
    (SP2_40G | SP2_100Gx4 | SP2_200Gx4 | SP2_400G),
    (SP2_100M | SP2_1G | SP2_10G | SP2_25G | SP2_50Gx1),
    (SP2_40G | SP2_100Gx4 | SP2_200Gx4 | SP2_400G),
    (SP2_100G | SP2_100Gx2 | SP2_100Gx4),
    (SP2_1G),
    (SP2_10G),
    (SP2_40G),
    (SP2_50Gx2 | SP2_100Gx2),
    (SP2_100Gx4),
    (0),
    (SP2_10G),
    (0),
    (SP2_400G | SP2_400Gx8),
    (SP2_400G | SP2_400Gx8),
    (SP2_400G | SP2_400Gx8),
    (SP2_400G | SP2_400Gx8)
};

uint64_t mlnx_port_lanes_speed_bitmask_sp2[MAX_LANES_SPC3_4 + 1] = {
    (0),
    /* 1 lane */
    (SP2_50Gx1 | SP2_25G | SP2_10G | SP2_1G | SP2_100M),
    /* 2 lanes */
    (SP2_100Gx2 | SP2_50Gx2 | SP2_50G |
     SP2_50Gx1 | SP2_25G | SP2_10G | SP2_1G | SP2_100M),
    (0),
    /* 4 lanes */
    (SP2_200G | SP2_200Gx4 | SP2_100Gx4 | SP2_100G | SP2_40G |
     SP2_100Gx2 | SP2_50Gx2 | SP2_50G |
     SP2_50Gx1 | SP2_25G | SP2_10G | SP2_1G | SP2_100M),
    (0),
    (0),
    (0),
    /* 8 lanes */
    (SP2_400G | SP2_400Gx8 |
     SP2_200G | SP2_200Gx4 | SP2_100Gx4 | SP2_100G | SP2_40G |
     SP2_100Gx2 | SP2_50Gx2 | SP2_50G |
     SP2_50Gx1 | SP2_25G | SP2_10G | SP2_1G | SP2_100M),
};

enum mlnx_speed_bitmap_sp4 {
    SP4_100M   = 1 << 0,
    SP4_1G     = 1 << 1,
    SP4_10G    = 1 << 2,
    SP4_25G    = 1 << 3,
    SP4_40G    = 1 << 4,
    SP4_50G    = 1 << 5,
    SP4_50Gx1  = 1 << 6,
    SP4_50Gx2  = 1 << 7,
    SP4_100G   = 1 << 8,
    SP4_100Gx2 = 1 << 9,
    SP4_100Gx4 = 1 << 10,
    SP4_200G   = 1 << 11,
    SP4_200Gx4 = 1 << 12,
    SP4_400G   = 1 << 13,
    SP4_400Gx8 = 1 << 14,
    SP4_100Gx1 = 1 << 15,
    SP4_200Gx2 = 1 << 16,
    SP4_400Gx4 = 1 << 17,
    SP4_800G   = 1 << 18,
    SP4_800Gx8 = 1 << 19,
    SP4_auto   = 1 << 20,
    SP4_MAX    = 1 << 21
};

uint64_t mlnx_port_speed_bitmap_sp4[MAX_NUM_PORT_SPEEDS] = {
    (0),
    (SP4_100M),
    (SP4_1G),
    (SP4_10G),
    (0),
    (SP4_25G),
    (SP4_40G),
    (SP4_50G | SP4_50Gx1 | SP4_50Gx2),
    (0),
    (SP4_100G | SP4_100Gx2 | SP4_100Gx4 | SP4_100Gx1),
    (SP4_200G | SP4_200Gx4 | SP4_200Gx2),
    (SP4_400G | SP4_400Gx8 | SP4_400Gx4),
    (SP4_800G | SP4_800Gx8),
    (0)
};

uint64_t mlnx_port_intf_bitmap_sp4[MAX_NUM_PORT_INTFS] = {
    (SP4_MAX - 1),
    (SP4_100M | SP4_1G | SP4_10G | SP4_25G | SP4_50Gx1 | SP4_100Gx1),
    (SP4_50Gx2 | SP4_100Gx2 | SP4_200Gx2),
    (SP4_40G | SP4_100Gx4 | SP4_200Gx4 | SP4_400Gx4),
    (SP4_100M | SP4_1G | SP4_10G | SP4_25G | SP4_50Gx1 | SP4_100Gx1),
    (SP4_50Gx2 | SP4_100Gx2 | SP4_200Gx2),
    (SP4_40G | SP4_100Gx4 | SP4_200Gx4 | SP4_400Gx4),
    (SP4_100M | SP4_1G | SP4_10G | SP4_25G | SP4_50Gx1 | SP4_100Gx1),
    (SP4_40G | SP4_100Gx4 | SP4_200Gx4 | SP4_400Gx4),
    (SP4_100M | SP4_1G | SP4_10G | SP4_25G | SP4_50Gx1),
    (SP4_40G | SP4_100Gx4 | SP4_200Gx4 | SP4_400G),
    (SP4_100G),
    (SP4_1G),
    (SP4_10G),
    (SP4_40G),
    (SP4_50Gx2 | SP4_100Gx2),
    (SP4_100Gx4),
    (0),
    (SP4_10G),
    (0),
    (SP4_400Gx8 | SP4_800Gx8 | SP4_800G),
    (SP4_400Gx8 | SP4_800Gx8 | SP4_800G),
    (SP4_400Gx8 | SP4_800Gx8 | SP4_800G),
    (SP4_400Gx8 | SP4_800Gx8 | SP4_800G)
};

uint64_t mlnx_port_lanes_speed_bitmask_sp4[MAX_LANES_SPC3_4 + 1] = {
    (0),
    /* 1 lane */
    (SP4_100Gx1 |
     SP4_50Gx1 | SP4_25G | SP4_10G | SP4_1G | SP4_100M),
    /* 2 lanes */
    (SP4_200Gx2 |
     SP4_100Gx2 | SP4_100Gx1 | SP4_50Gx2 | SP4_50G |
     SP4_50Gx1 | SP4_25G | SP4_10G | SP4_1G | SP4_100M),
    (0),
    /* 4 lanes */
    (SP4_400Gx4 |
     SP4_200G | SP4_200Gx4 | SP4_200Gx2 | SP4_100Gx4 | SP4_100G | SP4_40G |
     SP4_100Gx2 | SP4_100Gx1 | SP4_50Gx2 | SP4_50G |
     SP4_50Gx1 | SP4_25G | SP4_10G | SP4_1G | SP4_100M),
    (0),
    (0),
    (0),
    /* 8 lanes */
    (SP4_800G | SP4_800Gx8 |
     SP4_400G | SP4_400Gx8 | SP4_400Gx4 |
     SP4_200G | SP4_200Gx2 | SP4_200Gx4 | SP4_100Gx4 | SP4_100G | SP4_40G |
     SP4_100Gx2 | SP4_100Gx1 | SP4_50Gx2 | SP4_50G |
     SP4_50Gx1 | SP4_25G | SP4_10G | SP4_1G | SP4_100M),
};

typedef sai_status_t (*mlnx_port_speed_set_fn)(_In_ sx_port_log_id_t sx_port, _In_ uint32_t speed);
typedef sai_status_t (*mlnx_port_speed_get_fn)(_In_ sx_port_log_id_t sx_port, _Out_ uint32_t *oper_speed,
                                               _Out_ uint32_t *admin_speed);
typedef sai_status_t (*mlnx_port_supported_speeds_get_fn)(_In_ sx_port_log_id_t sx_port, _Out_ uint32_t *speeds,
                                                          _Inout_ uint32_t      *speeds_count);
typedef sai_status_t (*mlnx_port_speed_bitmap_apply_fn)(_In_ mlnx_port_config_t *port);
typedef sai_status_t (*mlnx_port_autoneg_set_fn)(_In_ sx_port_log_id_t sx_port, _In_ bool value);
typedef sai_status_t (*mlnx_port_autoneg_get_fn)(_In_ sx_port_log_id_t sx_port, _Out_ bool *value);
typedef sai_status_t (*mlnx_port_wred_mirror_set_fn)(_In_ sx_port_log_id_t sx_port,
                                                     _In_ sx_span_session_id_t sx_session, _In_ bool is_add);
typedef sai_status_t (*mlnx_port_update_speed_fn)(_In_ sx_port_log_id_t sx_port,
                                                  _In_ bool             auto_neg,
                                                  _In_ uint64_t         bitmap);

typedef struct _mlnx_port_cb_t {
    mlnx_port_speed_get_fn            speed_get;
    mlnx_port_supported_speeds_get_fn supported_speeds_get;
    mlnx_port_speed_bitmap_apply_fn   speed_bitmap_apply;
    mlnx_port_wred_mirror_set_fn      wred_mirror_set;
    mlnx_port_update_speed_fn         update_speed;
    uint64_t                         *speed_bitmap;
    uint64_t                         *intf_bitmap;
    uint64_t                         *lanes_speed_bitmap;
} mlnx_port_cb_table_t;

static mlnx_port_cb_table_t               mlnx_port_cb_sp = {
    mlnx_port_speed_get_sp,
    mlnx_port_supported_speeds_get_sp,
    mlnx_port_speed_bitmap_apply_sp,
    mlnx_port_wred_mirror_set_sp,
    mlnx_port_update_speed_sp,
    mlnx_port_speed_bitmap_sp,
    mlnx_port_intf_bitmap_sp,
    mlnx_port_lanes_speed_bitmask_sp,
};
static mlnx_port_cb_table_t               mlnx_port_cb_sp2 = {
    mlnx_port_speed_get_sp2,
    mlnx_port_supported_speeds_get_sp2,
    mlnx_port_speed_bitmap_apply_sp2,
    mlnx_port_wred_mirror_set_sp2,
    mlnx_port_update_speed_sp2,
    mlnx_port_speed_bitmap_sp2,
    mlnx_port_intf_bitmap_sp2,
    mlnx_port_lanes_speed_bitmask_sp2,
};
static mlnx_port_cb_table_t               mlnx_port_cb_sp4 = {
    mlnx_port_speed_get_sp2,
    mlnx_port_supported_speeds_get_sp2,
    mlnx_port_speed_bitmap_apply_sp2,
    mlnx_port_wred_mirror_set_sp2,
    mlnx_port_update_speed_sp4,
    mlnx_port_speed_bitmap_sp4,
    mlnx_port_intf_bitmap_sp4,
    mlnx_port_lanes_speed_bitmask_sp4,
};
static mlnx_port_cb_table_t             * mlnx_port_cb = NULL;
static const sai_vendor_attribute_entry_t port_vendor_attribs[] = {
    { SAI_PORT_ATTR_TYPE,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_type_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_OPER_STATUS,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_state_get, (void*)SAI_PORT_ATTR_OPER_STATUS,
      NULL, NULL },
    { SAI_PORT_ATTR_HW_LANE_LIST,
      { true, false, false, true },
      { true, false, false, true },
      mlnx_port_hw_lanes_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_SUPPORTED_BREAKOUT_MODE_TYPE,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_supported_breakout_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_CURRENT_BREAKOUT_MODE_TYPE,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_current_breakout_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_SUPPORTED_SPEED,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_supported_speed_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_SUPPORTED_FEC_MODE,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_supported_fec_mode_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_SUPPORTED_AUTO_NEG_MODE,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_supported_auto_neg_mode_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_SUPPORTED_FLOW_CONTROL_MODE,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_supported_flow_control_mode_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_NUMBER_OF_INGRESS_PRIORITY_GROUPS,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_number_of_priority_groups_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_INGRESS_PRIORITY_GROUP_LIST,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_priority_group_list_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_OPER_SPEED,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_oper_speed_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_SPEED,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_attr_get, (void*)SAI_PORT_ATTR_SPEED,
      mlnx_port_attr_set, (void*)SAI_PORT_ATTR_SPEED },
    { SAI_PORT_ATTR_FULL_DUPLEX_MODE,
      { false, false, false, true },
      { false, false, true, true },
      mlnx_port_duplex_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_AUTO_NEG_MODE,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_attr_get, (void*)SAI_PORT_ATTR_AUTO_NEG_MODE,
      mlnx_port_attr_set, (void*)SAI_PORT_ATTR_AUTO_NEG_MODE },
    { SAI_PORT_ATTR_ADVERTISED_SPEED,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_attr_get, (void*)SAI_PORT_ATTR_ADVERTISED_SPEED,
      mlnx_port_attr_set, (void*)SAI_PORT_ATTR_ADVERTISED_SPEED },
    { SAI_PORT_ATTR_INTERFACE_TYPE,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_attr_get, (void*)SAI_PORT_ATTR_INTERFACE_TYPE,
      mlnx_port_attr_set, (void*)SAI_PORT_ATTR_INTERFACE_TYPE },
    { SAI_PORT_ATTR_ADVERTISED_INTERFACE_TYPE,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_attr_get, (void*)SAI_PORT_ATTR_ADVERTISED_INTERFACE_TYPE,
      mlnx_port_attr_set, (void*)SAI_PORT_ATTR_ADVERTISED_INTERFACE_TYPE },
    { SAI_PORT_ATTR_ADMIN_STATE,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_state_get, (void*)SAI_PORT_ATTR_ADMIN_STATE,
      mlnx_port_state_set, NULL }, /*
                                    *  { SAI_PORT_ATTR_MEDIA_TYPE,
                                    *  { false, false, false, false },
                                    *  { false, false, true, true },
                                    *  NULL, NULL,
                                    *  NULL, NULL },*/
    { SAI_PORT_ATTR_PORT_VLAN_ID,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_lag_pvid_attr_get, NULL,
      mlnx_port_lag_pvid_attr_set, NULL },
    { SAI_PORT_ATTR_DEFAULT_VLAN_PRIORITY,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_lag_default_vlan_prio_get, NULL,
      mlnx_port_lag_default_vlan_prio_set, NULL },
    { SAI_PORT_ATTR_DROP_UNTAGGED,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_lag_drop_tags_get, (void*)SAI_PORT_ATTR_DROP_UNTAGGED,
      mlnx_port_lag_drop_tags_set, (void*)SAI_PORT_ATTR_DROP_UNTAGGED },
    { SAI_PORT_ATTR_DROP_TAGGED,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_lag_drop_tags_get, (void*)SAI_PORT_ATTR_DROP_TAGGED,
      mlnx_port_lag_drop_tags_set, (void*)SAI_PORT_ATTR_DROP_TAGGED },
    { SAI_PORT_ATTR_INTERNAL_LOOPBACK_MODE,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_internal_loopback_get, NULL,
      mlnx_port_internal_loopback_set, NULL },
    { SAI_PORT_ATTR_FEC_MODE,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_fec_get, NULL,
      mlnx_port_fec_set, NULL },
    { SAI_PORT_ATTR_UPDATE_DSCP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_update_dscp_get, NULL,
      mlnx_port_update_dscp_set, NULL },
    { SAI_PORT_ATTR_MTU,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_mtu_get, NULL,
      mlnx_port_mtu_set, NULL },
    { SAI_PORT_ATTR_FLOOD_STORM_CONTROL_POLICER_ID,
      { true, false, true, true},
      { true, false, true, true },
      mlnx_port_storm_control_policer_attr_get, (void*)MLNX_PORT_POLICER_TYPE_FLOOD_INDEX,
      mlnx_port_storm_control_policer_attr_set, (void*)MLNX_PORT_POLICER_TYPE_FLOOD_INDEX },
    { SAI_PORT_ATTR_BROADCAST_STORM_CONTROL_POLICER_ID,
      { true, false, true, true},
      { true, false, true, true },
      mlnx_port_storm_control_policer_attr_get, (void*)MLNX_PORT_POLICER_TYPE_BROADCAST_INDEX,
      mlnx_port_storm_control_policer_attr_set, (void*)MLNX_PORT_POLICER_TYPE_BROADCAST_INDEX },
    { SAI_PORT_ATTR_MULTICAST_STORM_CONTROL_POLICER_ID,
      { true, false, true, true},
      { true, false, true, true },
      mlnx_port_storm_control_policer_attr_get, (void*)MLNX_PORT_POLICER_TYPE_MULTICAST_INDEX,
      mlnx_port_storm_control_policer_attr_set, (void*)MLNX_PORT_POLICER_TYPE_MULTICAST_INDEX },
    { SAI_PORT_ATTR_GLOBAL_FLOW_CONTROL_MODE,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_global_flow_ctrl_get, NULL,
      mlnx_port_global_flow_ctrl_set, NULL },
    { SAI_PORT_ATTR_INGRESS_ACL,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_acl_bind_point_get, (void*)MLNX_ACL_BIND_POINT_TYPE_INGRESS_PORT,
      mlnx_acl_bind_point_set, (void*)MLNX_ACL_BIND_POINT_TYPE_INGRESS_PORT },
    { SAI_PORT_ATTR_EGRESS_ACL,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_acl_bind_point_get, (void*)MLNX_ACL_BIND_POINT_TYPE_EGRESS_PORT,
      mlnx_acl_bind_point_set, (void*)MLNX_ACL_BIND_POINT_TYPE_EGRESS_PORT },
    { SAI_PORT_ATTR_INGRESS_MIRROR_SESSION,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_mirror_session_get, (void*)MIRROR_INGRESS_PORT,
      mlnx_port_mirror_session_set, (void*)MIRROR_INGRESS_PORT },
    { SAI_PORT_ATTR_EGRESS_MIRROR_SESSION,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_mirror_session_get, (void*)MIRROR_EGRESS_PORT,
      mlnx_port_mirror_session_set, (void*)MIRROR_EGRESS_PORT },
    { SAI_PORT_ATTR_INGRESS_SAMPLEPACKET_ENABLE,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_samplepacket_session_get, (void*)SAMPLEPACKET_INGRESS_PORT,
      mlnx_port_samplepacket_session_set, (void*)SAMPLEPACKET_INGRESS_PORT },
    { SAI_PORT_ATTR_EGRESS_SAMPLEPACKET_ENABLE,
      { false, false, false, false },
      { false, false, false, false },
      NULL, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_QOS_DEFAULT_TC,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_default_tc_get, NULL,
      mlnx_port_qos_default_tc_set, NULL },
    { SAI_PORT_ATTR_QOS_DOT1P_TO_TC_MAP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_map_id_get, (void*)SAI_QOS_MAP_TYPE_DOT1P_TO_TC,
      mlnx_port_qos_map_id_set, (void*)SAI_QOS_MAP_TYPE_DOT1P_TO_TC },
    { SAI_PORT_ATTR_QOS_DOT1P_TO_COLOR_MAP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_map_id_get, (void*)SAI_QOS_MAP_TYPE_DOT1P_TO_COLOR,
      mlnx_port_qos_map_id_set, (void*)SAI_QOS_MAP_TYPE_DOT1P_TO_COLOR },
    { SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_map_id_get, (void*)SAI_QOS_MAP_TYPE_DSCP_TO_TC,
      mlnx_port_qos_map_id_set, (void*)SAI_QOS_MAP_TYPE_DSCP_TO_TC },
    { SAI_PORT_ATTR_QOS_DSCP_TO_COLOR_MAP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_map_id_get, (void*)SAI_QOS_MAP_TYPE_DSCP_TO_COLOR,
      mlnx_port_qos_map_id_set, (void*)SAI_QOS_MAP_TYPE_DSCP_TO_COLOR },
    { SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DOT1P_MAP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_map_id_get, (void*)SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DOT1P,
      mlnx_port_qos_map_id_set, (void*)SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DOT1P },
    { SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DSCP_MAP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_map_id_get, (void*)SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP,
      mlnx_port_qos_map_id_set, (void*)SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP },
    { SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_map_id_get, (void*)SAI_QOS_MAP_TYPE_TC_TO_QUEUE,
      mlnx_port_qos_map_id_set, (void*)SAI_QOS_MAP_TYPE_TC_TO_QUEUE },
    { SAI_PORT_ATTR_QOS_TC_TO_PRIORITY_GROUP_MAP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_map_id_get, (void*)SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP,
      mlnx_port_qos_map_id_set, (void*)SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP },
    { SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_map_id_get, (void*)SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP,
      mlnx_port_qos_map_id_set, (void*)SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP },
    { SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_QUEUE_MAP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_qos_map_id_get, (void*)SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_QUEUE,
      mlnx_port_qos_map_id_set, (void*)SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_QUEUE },
    { SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_MODE,
      { true, false, true, false },
      { true, false, true, true },
      NULL, NULL,
      mlnx_port_pfc_control_mode_set, NULL },
    { SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_pfc_control_get, (void*)SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL,
      mlnx_port_pfc_control_set, (void*)SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL },
    { SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_RX,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_pfc_control_get, (void*)SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_RX,
      mlnx_port_pfc_control_set, (void*)SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_RX },
    { SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_TX,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_pfc_control_get, (void*)SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_TX,
      mlnx_port_pfc_control_set, (void*)SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_TX },
    { SAI_PORT_ATTR_QOS_SCHEDULER_PROFILE_ID,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_sched_get, NULL,
      mlnx_port_sched_set, NULL },
    { SAI_PORT_ATTR_QOS_NUMBER_OF_QUEUES,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_queue_num_get, NULL,
      NULL, NULL},
    { SAI_PORT_ATTR_QOS_QUEUE_LIST,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_queue_list_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_QOS_NUMBER_OF_SCHEDULER_GROUPS,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_sched_groups_num_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_QOS_SCHEDULER_GROUP_LIST,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_sched_groups_list_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_QOS_INGRESS_BUFFER_PROFILE_LIST,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_ingress_buffer_profile_list_get, NULL,
      mlnx_port_ingress_buffer_profile_list_set, NULL },

    { SAI_PORT_ATTR_QOS_EGRESS_BUFFER_PROFILE_LIST,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_egress_buffer_profile_list_get, NULL,
      mlnx_port_egress_buffer_profile_list_set, NULL },
    { SAI_PORT_ATTR_POLICER_ID,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_storm_control_policer_attr_get, (void*)MLNX_PORT_POLICER_TYPE_REGULAR_INDEX,
      mlnx_port_storm_control_policer_attr_set, (void*)MLNX_PORT_POLICER_TYPE_REGULAR_INDEX },
    { SAI_PORT_ATTR_EGRESS_BLOCK_PORT_LIST,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_egress_block_get, NULL,
      mlnx_port_egress_block_set, NULL },
    { SAI_PORT_ATTR_ISOLATION_GROUP,
      { true, false, true, true },
      { true, false, true, true },
      mlnx_port_isolation_group_get, NULL,
      mlnx_port_isolation_group_set, NULL },
    { SAI_PORT_ATTR_PORT_POOL_LIST,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_pool_list_get, NULL,
      NULL, NULL },
    { SAI_PORT_ATTR_QOS_MAXIMUM_HEADROOM_SIZE,
      { false, false, false, true },
      { false, false, false, true },
      mlnx_port_max_headroom_size_get, NULL,
      NULL, NULL },
    { END_FUNCTIONALITY_ATTRIBS_ID,
      { false, false, false, false },
      { false, false, false, false },
      NULL, NULL,
      NULL, NULL }
};
static const mlnx_attr_enum_info_t        port_enum_info[] = {
    [SAI_PORT_ATTR_TYPE] =
        ATTR_ENUM_VALUES_ALL(),

    [SAI_PORT_ATTR_OPER_STATUS] = ATTR_ENUM_VALUES_LIST(
        SAI_PORT_OPER_STATUS_UP,
        SAI_PORT_OPER_STATUS_DOWN,
        SAI_PORT_OPER_STATUS_UNKNOWN),

    [SAI_PORT_ATTR_SUPPORTED_BREAKOUT_MODE_TYPE] =
        ATTR_ENUM_VALUES_ALL(),

    [SAI_PORT_ATTR_CURRENT_BREAKOUT_MODE_TYPE] = ATTR_ENUM_VALUES_LIST(
        SAI_PORT_BREAKOUT_MODE_TYPE_1_LANE),

    [SAI_PORT_ATTR_INTERNAL_LOOPBACK_MODE] = ATTR_ENUM_VALUES_LIST(
        SAI_PORT_INTERNAL_LOOPBACK_MODE_MAC,
        SAI_PORT_INTERNAL_LOOPBACK_MODE_NONE),

    [SAI_PORT_ATTR_FEC_MODE] = ATTR_ENUM_VALUES_LIST(
        SAI_PORT_FEC_MODE_NONE,
        SAI_PORT_FEC_MODE_FC,
        SAI_PORT_FEC_MODE_RS),

    [SAI_PORT_ATTR_GLOBAL_FLOW_CONTROL_MODE] = ATTR_ENUM_VALUES_LIST(
        SAI_PORT_FLOW_CONTROL_MODE_DISABLE,
        SAI_PORT_FLOW_CONTROL_MODE_TX_ONLY,
        SAI_PORT_FLOW_CONTROL_MODE_RX_ONLY,
        SAI_PORT_FLOW_CONTROL_MODE_BOTH_ENABLE),

    [SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_MODE] = ATTR_ENUM_VALUES_LIST(
        SAI_PORT_PRIORITY_FLOW_CONTROL_MODE_COMBINED,
        SAI_PORT_PRIORITY_FLOW_CONTROL_MODE_SEPARATE),
    [SAI_PORT_ATTR_SUPPORTED_FEC_MODE] = ATTR_ENUM_VALUES_LIST(
        SAI_PORT_FEC_MODE_NONE,
        SAI_PORT_FEC_MODE_FC,
        SAI_PORT_FEC_MODE_RS),
    [SAI_PORT_ATTR_SUPPORTED_FLOW_CONTROL_MODE] = ATTR_ENUM_VALUES_ALL(),

    [SAI_PORT_ATTR_INTERFACE_TYPE] = ATTR_ENUM_VALUES_LIST(
        SAI_PORT_INTERFACE_TYPE_NONE,
        SAI_PORT_INTERFACE_TYPE_CR,
        SAI_PORT_INTERFACE_TYPE_CR2,
        SAI_PORT_INTERFACE_TYPE_CR4,
        SAI_PORT_INTERFACE_TYPE_SR,
        SAI_PORT_INTERFACE_TYPE_SR2,
        SAI_PORT_INTERFACE_TYPE_SR4,
        SAI_PORT_INTERFACE_TYPE_LR,
        SAI_PORT_INTERFACE_TYPE_LR4,
        SAI_PORT_INTERFACE_TYPE_KR,
        SAI_PORT_INTERFACE_TYPE_KR2,
        SAI_PORT_INTERFACE_TYPE_KR4,
        SAI_PORT_INTERFACE_TYPE_GMII,
        SAI_PORT_INTERFACE_TYPE_CAUI,
        SAI_PORT_INTERFACE_TYPE_SFI,
        SAI_PORT_INTERFACE_TYPE_XLAUI,
        SAI_PORT_INTERFACE_TYPE_CAUI4,
        SAI_PORT_INTERFACE_TYPE_XAUI,
        SAI_PORT_INTERFACE_TYPE_XFI,
        SAI_PORT_INTERFACE_TYPE_XGMII,
        SAI_PORT_INTERFACE_TYPE_CR8,
        SAI_PORT_INTERFACE_TYPE_KR8,
        SAI_PORT_INTERFACE_TYPE_SR8,
        SAI_PORT_INTERFACE_TYPE_LR8
        ),

    [SAI_PORT_ATTR_ADVERTISED_INTERFACE_TYPE] = ATTR_ENUM_VALUES_LIST(
        SAI_PORT_INTERFACE_TYPE_NONE,
        SAI_PORT_INTERFACE_TYPE_CR,
        SAI_PORT_INTERFACE_TYPE_CR2,
        SAI_PORT_INTERFACE_TYPE_CR4,
        SAI_PORT_INTERFACE_TYPE_SR,
        SAI_PORT_INTERFACE_TYPE_SR2,
        SAI_PORT_INTERFACE_TYPE_SR4,
        SAI_PORT_INTERFACE_TYPE_LR,
        SAI_PORT_INTERFACE_TYPE_LR4,
        SAI_PORT_INTERFACE_TYPE_KR,
        SAI_PORT_INTERFACE_TYPE_KR2,
        SAI_PORT_INTERFACE_TYPE_KR4,
        SAI_PORT_INTERFACE_TYPE_GMII,
        SAI_PORT_INTERFACE_TYPE_CAUI,
        SAI_PORT_INTERFACE_TYPE_SFI,
        SAI_PORT_INTERFACE_TYPE_XLAUI,
        SAI_PORT_INTERFACE_TYPE_CAUI4,
        SAI_PORT_INTERFACE_TYPE_XAUI,
        SAI_PORT_INTERFACE_TYPE_XFI,
        SAI_PORT_INTERFACE_TYPE_XGMII,
        SAI_PORT_INTERFACE_TYPE_CR8,
        SAI_PORT_INTERFACE_TYPE_KR8,
        SAI_PORT_INTERFACE_TYPE_SR8,
        SAI_PORT_INTERFACE_TYPE_LR8
        ),
};
static const sai_stat_capability_t        port_stats_capabilities[] = {
    { SAI_PORT_STAT_IF_IN_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_IN_UCAST_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_IN_NON_UCAST_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_IN_DISCARDS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_IN_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_IN_UNKNOWN_PROTOS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_IN_BROADCAST_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_IN_MULTICAST_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_IN_VLAN_DISCARDS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_OUT_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_OUT_UCAST_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_OUT_NON_UCAST_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_OUT_DISCARDS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_OUT_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_OUT_BROADCAST_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IF_OUT_MULTICAST_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_DROP_EVENTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_MULTICAST_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_BROADCAST_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_FRAGMENTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1518_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_PKTS_1519_TO_2047_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_PKTS_4096_TO_9216_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_RX_OVERSIZE_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_TX_OVERSIZE_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_JABBERS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_COLLISIONS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_TX_NO_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_STATS_RX_NO_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_WRED_DROPPED_PACKETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ECN_MARKED_PACKETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_IN_PKTS_64_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_IN_PKTS_1024_TO_1518_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_IN_PKTS_1519_TO_2047_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_IN_PKTS_4096_TO_9216_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1518_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_OUT_PKTS_1519_TO_2047_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_ETHER_OUT_PKTS_4096_TO_9216_OCTETS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_IN_DROPPED_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_OUT_DROPPED_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PAUSE_RX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PAUSE_TX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_0_RX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_0_TX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_1_RX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_1_TX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_2_RX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_2_TX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_3_RX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_3_TX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_4_RX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_4_TX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_5_RX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_5_TX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_6_RX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_6_TX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_7_RX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_7_TX_PKTS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_0_RX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_0_TX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_1_RX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_1_TX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_2_RX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_2_TX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_3_RX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_3_TX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_4_RX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_4_TX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_5_RX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_5_TX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_6_RX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_6_TX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_7_RX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_PFC_7_TX_PAUSE_DURATION_US, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_ALIGNMENT_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_FCS_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_SINGLE_COLLISION_FRAMES, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_MULTIPLE_COLLISION_FRAMES, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_SQE_TEST_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_DEFERRED_TRANSMISSIONS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_LATE_COLLISIONS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_EXCESSIVE_COLLISIONS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_CARRIER_SENSE_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_FRAME_TOO_LONGS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_STATS_SYMBOL_ERRORS, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_STAT_DOT3_CONTROL_IN_UNKNOWN_OPCODES, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
};
static size_t port_info_print(_In_ const sai_object_key_t *key, _Out_ char *str, _In_ size_t max_len)
{
    mlnx_object_id_t mlnx_oid = *(mlnx_object_id_t*)&key->key.object_id;

    return snprintf(str, max_len, "[log_port:0x%X]", mlnx_oid.id.u32);
}
const mlnx_obj_type_attrs_info_t mlnx_port_obj_type_info =
{ port_vendor_attribs, OBJ_ATTRS_ENUMS_INFO(port_enum_info), OBJ_STAT_CAP_INFO(port_stats_capabilities),
  port_info_print};
static const sai_vendor_attribute_entry_t port_pool_vendor_attribs[] = {
    { SAI_PORT_POOL_ATTR_PORT_ID,
      { true, false, false, true },
      { true, false, false, true },
      mlnx_port_pool_attr_get, (void*)SAI_PORT_POOL_ATTR_PORT_ID,
      NULL, NULL },
    { SAI_PORT_POOL_ATTR_BUFFER_POOL_ID,
      { true, false, false, true },
      { true, false, false, true },
      mlnx_port_pool_attr_get, (void*)SAI_PORT_POOL_ATTR_BUFFER_POOL_ID,
      NULL, NULL },
    { SAI_PORT_POOL_ATTR_QOS_WRED_PROFILE_ID,
      { false, false, false, false },
      { true, false, true, true },
      NULL, NULL,
      NULL, NULL },
    { END_FUNCTIONALITY_ATTRIBS_ID,
      { false, false, false, false },
      { false, false, false, false },
      NULL, NULL,
      NULL, NULL}
};
static const sai_stat_capability_t        port_pool_stats_capabilities[] = {
    { SAI_PORT_POOL_STAT_CURR_OCCUPANCY_BYTES, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_POOL_STAT_WATERMARK_BYTES, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
    { SAI_PORT_POOL_STAT_SHARED_WATERMARK_BYTES, SAI_STATS_MODE_READ | SAI_STATS_MODE_READ_AND_CLEAR },
};
static size_t port_pool_info_print(_In_ const sai_object_key_t *key, _Out_ char *str, _In_ size_t max_len)
{
    mlnx_object_id_t mlnx_oid = *(mlnx_object_id_t*)&key->key.object_id;
    char            *pool_type;

    if (mlnx_oid.ext.bytes[1] == SAI_BUFFER_POOL_TYPE_INGRESS) {
        pool_type = "INGRESS";
    } else if (mlnx_oid.ext.bytes[1] == SAI_BUFFER_POOL_TYPE_EGRESS) {
        pool_type = "EGRESS";
    } else if (mlnx_oid.ext.bytes[1] == SAI_BUFFER_POOL_TYPE_BOTH) {
        pool_type = "BOTH";
    } else {
        pool_type = "<invalid>";
    }

    return snprintf(str, max_len, "[log_port:0x%X, pool_id:%u, pool_type:%s]",
                    mlnx_oid.id.u32,
                    mlnx_oid.ext.bytes[0],
                    pool_type);
}
const mlnx_obj_type_attrs_info_t mlnx_port_pool_obj_type_info =
{ port_pool_vendor_attribs, OBJ_ATTRS_ENUMS_INFO_EMPTY(), OBJ_STAT_CAP_INFO(port_pool_stats_capabilities),
  port_pool_info_print};

/* Admin Mode [bool] */
static sai_status_t mlnx_port_state_set(_In_ const sai_object_key_t      *key,
                                        _In_ const sai_attribute_value_t *value,
                                        void                             *arg)
{
    sx_port_log_id_t    port_id;
    sai_status_t        status;
    mlnx_port_config_t *port;
    bool                sdk_state = value->booldata;
    bool                is_warmboot_init_stage = false;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    sai_db_write_lock();

    is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                             (!g_sai_db_ptr->issu_end_called);

    status = mlnx_port_by_log_id(port_id, &port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed lookup port by log id %x\n", port_id);
        goto out;
    }

    port->admin_state = sdk_state;

    /* on warmboot control priority group buffer is handled just before issu end */
    if (!is_warmboot_init_stage) {
        status = mlnx_sai_buffer_update_pg9_buffer_sdk(port);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to update control PG headroom size on port state change.\n");
            goto out;
        }
    } else {
        status = mlnx_port_update_speed(port);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to update speed.\n");
            goto out;
        }
    }

    status = sx_api_port_state_set(gh_sdk, port_id, sdk_state ? SX_PORT_ADMIN_STATUS_UP : SX_PORT_ADMIN_STATUS_DOWN);
    if (SX_ERR(status)) {
        SX_LOG_ERR("Failed to set port admin state - %s.\n", SX_STATUS_MSG(status));
        status = sdk_to_sai(status);
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/* Port/LAG VLAN ID [sai_vlan_id_t]
 * Untagged ingress frames are tagged with Port VLAN ID (PVID)
 */
sai_status_t mlnx_port_lag_pvid_attr_set(_In_ const sai_object_key_t      *key,
                                         _In_ const sai_attribute_value_t *value,
                                         void                             *arg)
{
    sai_status_t        status;
    mlnx_port_config_t *port_config;
    bool                is_warmboot_init_stage = false;
    uint32_t            port_db_idx;

    SX_LOG_ENTER();

    sai_db_write_lock();

    is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                             (!g_sai_db_ptr->issu_end_called);
    if (is_warmboot_init_stage) {
        status = mlnx_port_idx_by_obj_id(key->key.object_id, &port_db_idx);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Error getting port idx from log id %" PRIx64 "\n", key->key.object_id);
            goto out;
        }
        if ((port_db_idx >= MAX_PORTS) && (port_db_idx < MAX_PORTS * 2) &&
            (0 == mlnx_ports_db[port_db_idx].logical)) {
            mlnx_ports_db[port_db_idx].issu_lag_attr.lag_pvid = value->u16;
            mlnx_ports_db[port_db_idx].issu_lag_attr.lag_pvid_changed = true;
            status = SAI_STATUS_SUCCESS;
            goto out;
        }
    }

    status = mlnx_port_by_obj_id(key->key.object_id, &port_config);
    if (SAI_ERR(status)) {
        goto out;
    }

    if (mlnx_port_is_lag_member(port_config)) {
        /* During ISSU, if port is added to LAG in SDK, but not in SAI, then error out */
        if (0 != port_config->lag_id) {
            SX_LOG_ERR(
                "Failed to set PVID to port [%lx] - port is a lag member. Please use SAI_LAG_ATTR_PORT_VLAN_ID\n",
                port_config->saiport);
            status = SAI_STATUS_FAILURE;
            goto out;
        } else {
            mlnx_ports_db[port_db_idx].issu_lag_attr.lag_pvid = value->u16;
            mlnx_ports_db[port_db_idx].issu_lag_attr.lag_pvid_changed = true;
            status = SAI_STATUS_SUCCESS;
            goto out;
        }
    }

    if (SX_STATUS_SUCCESS !=
        (status = sx_api_vlan_port_pvid_set(gh_sdk, SX_ACCESS_CMD_ADD, port_config->logical, value->u16))) {
        SX_LOG_ERR("Failed to set %s %x pvid - %s.\n", mlnx_port_type_str(port_config),
                   port_config->logical, SX_STATUS_MSG(status));
        status = sdk_to_sai(status);
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/* Default VLAN Priority [uint8_t]
 *  (default to 0) */
sai_status_t mlnx_port_lag_default_vlan_prio_set(_In_ const sai_object_key_t      *key,
                                                 _In_ const sai_attribute_value_t *value,
                                                 void                             *arg)
{
    sai_status_t        status;
    mlnx_port_config_t *port_config;
    bool                is_warmboot_init_stage = false;
    uint32_t            port_db_idx;

    SX_LOG_ENTER();

    sai_db_write_lock();

    is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                             (!g_sai_db_ptr->issu_end_called);
    if (is_warmboot_init_stage) {
        status = mlnx_port_idx_by_obj_id(key->key.object_id, &port_db_idx);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Error getting port idx from log id %" PRIx64 "\n", key->key.object_id);
            goto out;
        }
        if ((port_db_idx >= MAX_PORTS) && (port_db_idx < MAX_PORTS * 2) &&
            (0 == mlnx_ports_db[port_db_idx].logical)) {
            mlnx_ports_db[port_db_idx].issu_lag_attr.lag_default_vlan_priority = value->u8;
            mlnx_ports_db[port_db_idx].issu_lag_attr.lag_default_vlan_priority_changed = true;
            status = SAI_STATUS_SUCCESS;
            goto out;
        }
    }

    status = mlnx_port_by_obj_id(key->key.object_id, &port_config);
    if (SAI_ERR(status)) {
        goto out;
    }

    if (mlnx_port_is_lag_member(port_config)) {
        /* During ISSU, if port is added to LAG in SDK, but not in SAI, then error out */
        if (0 != port_config->lag_id) {
            SX_LOG_ERR(
                "Failed to set port [%lx] default prio - port is a lag member. Please use SAI_LAG_ATTR_DEFAULT_VLAN_PRIORITY\n",
                port_config->saiport);
            status = SAI_STATUS_FAILURE;
            goto out;
        } else {
            mlnx_ports_db[port_db_idx].issu_lag_attr.lag_default_vlan_priority = value->u8;
            mlnx_ports_db[port_db_idx].issu_lag_attr.lag_default_vlan_priority_changed = true;
            status = SAI_STATUS_SUCCESS;
            goto out;
        }
    }

    if (SX_STATUS_SUCCESS !=
        (status = sx_api_cos_port_default_prio_set(gh_sdk, port_config->logical, value->u8))) {
        SX_LOG_ERR("Failed to set port default prio - %s.\n", SX_STATUS_MSG(status));
        status = sdk_to_sai(status);
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_lag_drop_tags_set_impl(_In_ mlnx_port_config_t *port_config,
                                                     _In_ sai_attr_id_t       attr_id,
                                                     _In_ bool                enable)
{
    sx_status_t           sx_status;
    sx_vlan_frame_types_t frame_types = {0};

    assert((SAI_PORT_ATTR_DROP_UNTAGGED == attr_id) || (SAI_PORT_ATTR_DROP_TAGGED == attr_id) ||
           (SAI_LAG_ATTR_DROP_UNTAGGED == attr_id) || (SAI_LAG_ATTR_DROP_TAGGED == attr_id));
    assert(port_config);

    sx_status = sx_api_vlan_port_accptd_frm_types_get(gh_sdk, port_config->logical, &frame_types);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to get port accepted frame types - %s.\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    if ((SAI_PORT_ATTR_DROP_UNTAGGED == attr_id) || (SAI_LAG_ATTR_DROP_UNTAGGED == attr_id)) {
        frame_types.allow_untagged = !enable;
    } else {
        frame_types.allow_tagged = !enable;
    }

    sx_status = sx_api_vlan_port_accptd_frm_types_set(gh_sdk, port_config->logical, &frame_types);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to set port accepted frame types - %s.\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

/* Dropping of untagged frames on ingress [bool] */
/* Dropping of tagged frames on ingress [bool] */
sai_status_t mlnx_port_lag_drop_tags_set(_In_ const sai_object_key_t      *key,
                                         _In_ const sai_attribute_value_t *value,
                                         void                             *arg)
{
    sai_status_t        status;
    sai_attr_id_t       attr_id;
    mlnx_port_config_t *port_config;
    bool                is_warmboot_init_stage = false;
    uint32_t            port_db_idx;

    SX_LOG_ENTER();

    attr_id = (long)arg;

    assert((SAI_PORT_ATTR_DROP_UNTAGGED == attr_id) || (SAI_PORT_ATTR_DROP_TAGGED == attr_id) ||
           (SAI_LAG_ATTR_DROP_UNTAGGED == attr_id) || (SAI_LAG_ATTR_DROP_TAGGED == attr_id));

    sai_db_write_lock();

    is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                             (!g_sai_db_ptr->issu_end_called);
    if (is_warmboot_init_stage) {
        status = mlnx_port_idx_by_obj_id(key->key.object_id, &port_db_idx);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Error getting port idx from log id %" PRIx64 "\n", key->key.object_id);
            goto out;
        }
        if ((port_db_idx >= MAX_PORTS) && (port_db_idx < MAX_PORTS * 2) &&
            (0 == mlnx_ports_db[port_db_idx].logical)) {
            if (SAI_LAG_ATTR_DROP_UNTAGGED == attr_id) {
                mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_untagged = value->booldata;
                mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_untagged_changed = true;
            } else if (SAI_LAG_ATTR_DROP_TAGGED == attr_id) {
                mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_tagged = value->booldata;
                mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_tagged_changed = true;
            }
            status = SAI_STATUS_SUCCESS;
            goto out;
        }
    }

    status = mlnx_port_by_obj_id(key->key.object_id, &port_config);
    if (SAI_ERR(status)) {
        goto out;
    }

    if (mlnx_port_is_lag_member(port_config)) {
        /* During ISSU, if port is added to LAG in SDK, but not in SAI, then error out */
        if (0 != port_config->lag_id) {
            SX_LOG_ERR(
                "Failed to set port [%lx] drop tags info - port is a lag member. Please use corresponding LAG attribute\n",
                port_config->saiport);
            status = SAI_STATUS_FAILURE;
            goto out;
        } else {
            if (SAI_PORT_ATTR_DROP_UNTAGGED == attr_id) {
                mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_untagged = value->booldata;
                mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_untagged_changed = true;
                status = SAI_STATUS_SUCCESS;
                goto out;
            } else if (SAI_PORT_ATTR_DROP_TAGGED == attr_id) {
                mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_tagged = value->booldata;
                mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_tagged_changed = true;
                status = SAI_STATUS_SUCCESS;
                goto out;
            }
        }
    }

    status = mlnx_port_lag_drop_tags_set_impl(port_config, attr_id, value->booldata);
    if (SAI_ERR(status)) {
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_internal_loopback_set_impl(_In_ sx_port_log_id_t                  port_id,
                                                         _In_ sai_port_internal_loopback_mode_t loop_mode)
{
    sx_status_t             sx_status;
    sx_port_phys_loopback_t loop_val;

    switch (loop_mode) {
    case SAI_PORT_INTERNAL_LOOPBACK_MODE_NONE:
        loop_val = SX_PORT_PHYS_LOOPBACK_DISABLE;
        break;

    case SAI_PORT_INTERNAL_LOOPBACK_MODE_PHY:
        SX_LOG_ERR("Port internal phy loopback not supported\n");
        return SAI_STATUS_ATTR_NOT_SUPPORTED_0;

    case SAI_PORT_INTERNAL_LOOPBACK_MODE_MAC:
        loop_val = SX_PORT_PHYS_LOOPBACK_ENABLE_INTERNAL;
        break;

    default:
        SX_LOG_ERR("Invalid port internal loopback value %d\n", loop_mode);
        return SAI_STATUS_INVALID_ATTR_VALUE_0;
    }

    sx_status = sx_api_port_phys_loopback_set(gh_sdk, port_id, loop_val);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to set port physical loopback - %s.\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

/* Internal loopback control [sai_port_loopback_mode_t] */
static sai_status_t mlnx_port_internal_loopback_set(_In_ const sai_object_key_t      *key,
                                                    _In_ const sai_attribute_value_t *value,
                                                    void                             *arg)
{
    sai_status_t     status = SAI_STATUS_SUCCESS;
    sx_port_log_id_t port_id;

    SX_LOG_ENTER();

    sai_db_write_lock();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        goto out;
    }

    status = mlnx_port_internal_loopback_set_impl(port_id, value->s32);
    if (SAI_ERR(status)) {
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/* MTU [uint32_t] */
static sai_status_t mlnx_port_mtu_set(_In_ const sai_object_key_t      *key,
                                      _In_ const sai_attribute_value_t *value,
                                      void                             *arg)
{
    sai_status_t     status;
    sx_port_log_id_t port_id;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    if (SX_STATUS_SUCCESS != (status = sx_api_port_mtu_set(gh_sdk, port_id, (sx_port_mtu_t)value->u32))) {
        SX_LOG_ERR("Failed to set port mtu - %s.\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_global_flow_ctrl_set_impl(_In_ sx_port_log_id_t             port_id,
                                                        _In_ sai_port_flow_control_mode_t pfc_mode)
{
    sx_status_t              sx_status;
    sx_port_flow_ctrl_mode_t ctrl_mode = SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_DIS;

    switch (pfc_mode) {
    case SAI_PORT_FLOW_CONTROL_MODE_DISABLE:
        ctrl_mode = SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_DIS;
        break;

    case SAI_PORT_FLOW_CONTROL_MODE_TX_ONLY:
        ctrl_mode = SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_DIS;
        break;

    case SAI_PORT_FLOW_CONTROL_MODE_RX_ONLY:
        ctrl_mode = SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_EN;
        break;

    case SAI_PORT_FLOW_CONTROL_MODE_BOTH_ENABLE:
        ctrl_mode = SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_EN;
        break;

    default:
        SX_LOG_ERR("Invalid SAI global flow control mode %u\n", ctrl_mode);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    sx_status = sx_api_port_global_fc_enable_set(gh_sdk, port_id, ctrl_mode);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to set port global flow control - %s\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

/** [sai_port_flow_control_mode_t]
 *  (default to SAI_PORT_FLOW_CONTROL_DISABLE) */
static sai_status_t mlnx_port_global_flow_ctrl_set(_In_ const sai_object_key_t      *key,
                                                   _In_ const sai_attribute_value_t *value,
                                                   void                             *arg)
{
    sai_status_t        status;
    mlnx_port_config_t *port;
    sx_port_log_id_t    port_id;

    SX_LOG_ENTER();

    sai_db_write_lock();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert port oid to logical port id\n");
        goto out;
    }

    status = mlnx_port_by_log_id(port_id, &port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to lookup port by log id %x\n", port_id);
        goto out;
    }
    if (mlnx_port_is_lag_member(port)) {
        port_id = mlnx_port_get_lag_id(port);
    }

    status = mlnx_port_global_flow_ctrl_set_impl(port_id, value->s32);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to set port global flow control - %s\n", SX_STATUS_MSG(status));
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/* This function needs to be guarded by lock */
sai_status_t mlnx_wred_mirror_port_event(_In_ sx_port_log_id_t port_log_id, _In_ bool is_add)
{
    sai_status_t         sai_status = SAI_STATUS_FAILURE;
    uint32_t             mirror_cnt = 0;
    sai_object_id_t      sai_mirror_oid = SAI_NULL_OBJECT_ID;
    sx_span_session_id_t span_session_id = 0;
    uint32_t             ii = 0;
    uint32_t             sdk_mirror_id = 0;

    SX_LOG_ENTER();

    mirror_cnt = g_sai_db_ptr->trap_mirror_discard_wred_db.count;
    assert(SPAN_SESSION_MAX > mirror_cnt);

    for (ii = 0; ii < mirror_cnt; ii++) {
        sai_mirror_oid = g_sai_db_ptr->trap_mirror_discard_wred_db.mirror_oid[ii];
        sai_status = mlnx_object_to_type(sai_mirror_oid, SAI_OBJECT_TYPE_MIRROR_SESSION, &sdk_mirror_id, NULL);
        if (SAI_ERR(sai_status)) {
            SX_LOG_ERR("Error getting span session id from sai mirror id %" PRIx64 "\n", sai_mirror_oid);
            SX_LOG_EXIT();
            return sai_status;
        }

        span_session_id = (sx_span_session_id_t)sdk_mirror_id;

        sai_status = mlnx_port_wred_mirror_set_impl(port_log_id, span_session_id, is_add);
        if (SAI_ERR(sai_status)) {
            SX_LOG_EXIT();
            return sai_status;
        }
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

uint64_t mlnx_fec_mode_speeds_sp[3][20] = {
    { SX_PORT_PHY_SPEED_1GB, SX_PORT_PHY_SPEED_10GB, SX_PORT_PHY_SPEED_25GB, SX_PORT_PHY_SPEED_40GB,
      SX_PORT_PHY_SPEED_50GB, SX_PORT_PHY_SPEED_100GB, SX_PORT_PHY_SPEED_10MB, SX_PORT_PHY_SPEED_100MB,
      SX_PORT_PHY_SPEED_MAX + 1 },
    { SX_PORT_PHY_SPEED_25GB, SX_PORT_PHY_SPEED_50GB, SX_PORT_PHY_SPEED_100GB,
      SX_PORT_PHY_SPEED_MAX + 1 },
    { SX_PORT_PHY_SPEED_10GB, SX_PORT_PHY_SPEED_25GB, SX_PORT_PHY_SPEED_40GB, SX_PORT_PHY_SPEED_50GB,
      SX_PORT_PHY_SPEED_56GB, SX_PORT_PHY_SPEED_MAX + 1 },
};

uint64_t mlnx_fec_mode_speeds_sp2[3][20] = {
    { SX_PORT_PHY_SPEED_1GB, SX_PORT_PHY_SPEED_10GB, SX_PORT_PHY_SPEED_25GB, SX_PORT_PHY_SPEED_40GB,
      SX_PORT_PHY_SPEED_50GB, SX_PORT_PHY_SPEED_100GB, SX_PORT_PHY_SPEED_MAX + 1 },
    { SX_PORT_PHY_SPEED_25GB, SX_PORT_PHY_SPEED_50GB, SX_PORT_PHY_SPEED_50GB_1X, SX_PORT_PHY_SPEED_100GB,
      SX_PORT_PHY_SPEED_100GB_2X, SX_PORT_PHY_SPEED_200GB, SX_PORT_PHY_SPEED_400GB_8X, SX_PORT_PHY_SPEED_MAX + 1 },
    { SX_PORT_PHY_SPEED_10GB, SX_PORT_PHY_SPEED_25GB, SX_PORT_PHY_SPEED_40GB, SX_PORT_PHY_SPEED_50GB,
      SX_PORT_PHY_SPEED_56GB, SX_PORT_PHY_SPEED_MAX + 1 },
};

uint64_t mlnx_fec_mode_speeds_sp4[3][20] = {
    { SX_PORT_PHY_SPEED_1GB, SX_PORT_PHY_SPEED_10GB, SX_PORT_PHY_SPEED_25GB, SX_PORT_PHY_SPEED_40GB,
      SX_PORT_PHY_SPEED_50GB, SX_PORT_PHY_SPEED_100GB, SX_PORT_PHY_SPEED_MAX + 1 },
    { SX_PORT_PHY_SPEED_25GB, SX_PORT_PHY_SPEED_50GB, SX_PORT_PHY_SPEED_50GB_1X, SX_PORT_PHY_SPEED_100GB,
      SX_PORT_PHY_SPEED_100GB_2X, SX_PORT_PHY_SPEED_200GB, SX_PORT_PHY_SPEED_400GB_8X,
      SX_PORT_PHY_SPEED_100GB_1X, SX_PORT_PHY_SPEED_200GB_2X, SX_PORT_PHY_SPEED_400GB_4X,
      SX_PORT_PHY_SPEED_800GB_8X, SX_PORT_PHY_SPEED_MAX + 1 },
    { SX_PORT_PHY_SPEED_10GB, SX_PORT_PHY_SPEED_25GB, SX_PORT_PHY_SPEED_40GB, SX_PORT_PHY_SPEED_50GB,
      SX_PORT_PHY_SPEED_56GB, SX_PORT_PHY_SPEED_MAX + 1 },
};

sai_status_t mlnx_port_fec_set_impl(sx_port_log_id_t port_log_id, int32_t value)
{
    sx_status_t         status;
    sx_port_phy_mode_t  mode;
    sx_port_phy_speed_t speed;

    memset(&mode, 0, sizeof(mode));

    switch (value) {
    case SAI_PORT_FEC_MODE_NONE:
        mode.fec_mode = SX_PORT_FEC_MODE_NONE;
        break;

    case SAI_PORT_FEC_MODE_RS:
        mode.fec_mode = SX_PORT_FEC_MODE_RS;
        break;

    case SAI_PORT_FEC_MODE_FC:
        mode.fec_mode = SX_PORT_FEC_MODE_FC;
        break;

    default:
        SX_LOG_ERR("Invalid fec mode %d\n", value);
        return SAI_STATUS_INVALID_ATTR_VALUE_0;
    }

    uint64_t(*fec_mode_speeds)[20] =
        mlnx_chip_is_spc() ? mlnx_fec_mode_speeds_sp : mlnx_chip_is_spc4() ? mlnx_fec_mode_speeds_sp4 :
        mlnx_fec_mode_speeds_sp2;

    for (int32_t ii = 0; fec_mode_speeds[value][ii] != (SX_PORT_PHY_SPEED_MAX + 1); ii++) {
        speed = fec_mode_speeds[value][ii];
        status = sx_api_port_phy_mode_set(gh_sdk, port_log_id, speed, mode);
        if (SX_ERR(status)) {
            SX_LOG_ERR("Failed to set fec mode speed %d - %s.\n", speed, SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    }

    return SAI_STATUS_SUCCESS;
}

/* Forward Error Correction (FEC) control [sai_port_fec_mode_t] */
static sai_status_t mlnx_port_fec_set(_In_ const sai_object_key_t      *key,
                                      _In_ const sai_attribute_value_t *value,
                                      void                             *arg)
{
    sai_status_t     status;
    sx_port_log_id_t port_id;

/*TODO: remove when fixed */
#if defined IS_PLD || defined IS_SIMX
    /* FEC is not supported on Simx/PLD */
    SX_LOG_ERR("FEC is not supported on Simx/PLD.\n");
    return SAI_STATUS_NOT_SUPPORTED;
#endif

    SX_LOG_ENTER();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        SX_LOG_EXIT();
        return status;
    }

    status = mlnx_port_fec_set_impl(port_id, value->s32);

    SX_LOG_EXIT();
    return status;
}

/* Port type [sai_port_type_t] */
static sai_status_t mlnx_port_type_get(_In_ const sai_object_key_t   *key,
                                       _Inout_ sai_attribute_value_t *value,
                                       _In_ uint32_t                  attr_index,
                                       _Inout_ vendor_cache_t        *cache,
                                       void                          *arg)
{
    sai_status_t     status;
    sx_port_log_id_t port_id;
    sx_port_mode_t   port_mode;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    if (SX_STATUS_SUCCESS != (status = sx_api_port_mode_get(gh_sdk, port_id, &port_mode))) {
        SX_LOG_ERR("Failed to get port mode - %s.\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    switch (port_mode) {
    case SX_PORT_MODE_EXTERNAL:
        value->s32 = SAI_PORT_TYPE_LOGICAL;
        break;

    case SX_PORT_MODE_CPU:
        value->s32 = SAI_PORT_TYPE_CPU;
        break;

    /* TODO : add case for LAG */

    case SX_PORT_MODE_STACKING:
    case SX_PORT_MODE_TCA_CONNECTOR:
    default:
        SX_LOG_ERR("Unexpected port mode %d\n", port_mode);
        return SAI_STATUS_FAILURE;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Operational Status [sai_port_oper_status_t] */
/* Admin Mode [bool] */
static sai_status_t mlnx_port_state_get(_In_ const sai_object_key_t   *key,
                                        _Inout_ sai_attribute_value_t *value,
                                        _In_ uint32_t                  attr_index,
                                        _Inout_ vendor_cache_t        *cache,
                                        void                          *arg)
{
    sai_status_t           status;
    sx_port_log_id_t       port_id;
    sx_port_oper_state_t   state_oper;
    sx_port_admin_state_t  state_admin;
    sx_port_module_state_t state_module;

    SX_LOG_ENTER();

    assert((SAI_PORT_ATTR_OPER_STATUS == (long)arg) || (SAI_PORT_ATTR_ADMIN_STATE == (long)arg));

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    if (SX_STATUS_SUCCESS !=
        (status = sx_api_port_state_get(gh_sdk, port_id, &state_oper, &state_admin, &state_module))) {
        SX_LOG_ERR("Failed to get port state - %s.\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    if (SAI_PORT_ATTR_OPER_STATUS == (long)arg) {
        switch (state_oper) {
        case SX_PORT_OPER_STATUS_UP:
            value->s32 = SAI_PORT_OPER_STATUS_UP;
            break;

        case SX_PORT_OPER_STATUS_DOWN:
        case SX_PORT_OPER_STATUS_DOWN_BY_FAIL:
            value->s32 = SAI_PORT_OPER_STATUS_DOWN;
            break;

        default:
            value->s32 = SAI_PORT_OPER_STATUS_UNKNOWN;
        }
    } else {
        mlnx_port_config_t *port;

        sai_db_read_lock();

        status = mlnx_port_by_log_id(port_id, &port);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to lookup port by log id %x\n", port_id);
            goto out_unlock;
        }

        value->booldata = port->admin_state;

out_unlock:
        sai_db_unlock();
    }

    SX_LOG_EXIT();
    return status;
}

_Success_(return == SAI_STATUS_SUCCESS)
sai_status_t find_port_in_db(_In_ sai_object_id_t port, _Out_ uint32_t *index)
{
    mlnx_port_config_t *port_cfg;
    uint32_t            ii;

    if (NULL == index) {
        return SAI_STATUS_INVALID_PARAMETER;
    }

    mlnx_port_foreach(port_cfg, ii) {
        if (port == port_cfg->saiport) {
            *index = ii;
            return SAI_STATUS_SUCCESS;
        }
    }

    SX_LOG_ERR("Port %" PRIx64 " not found in DB\n", port);
    return SAI_STATUS_INVALID_PORT_NUMBER;
}

static uint32_t mlnx_port_max_lanes_get(void)
{
    sx_chip_types_t chip_type = g_sai_db_ptr->sx_chip_type;

    switch (chip_type) {
    case SX_CHIP_TYPE_SPECTRUM:
    case SX_CHIP_TYPE_SPECTRUM_A1:
    case SX_CHIP_TYPE_SPECTRUM2:
        return MAX_LANES_SPC1_2;

    case SX_CHIP_TYPE_SPECTRUM3:
    case SX_CHIP_TYPE_SPECTRUM4:
        return MAX_LANES_SPC3_4;

    default:
        MLNX_SAI_LOG_ERR("g_sai_db_ptr->sxd_chip_type = %s\n", SX_CHIP_TYPE_STR(chip_type));
        assert(false);
        return MAX_LANES_SPC1_2;
    }
}

static sai_status_t copy_port_hw_lanes(_In_ sx_port_mapping_t        *port_map,
                                       _In_ uint32_t                 *lanes,
                                       _Inout_ sai_attribute_value_t *value)
{
    uint32_t     ii = 0;
    uint32_t     jj;
    sai_status_t sai_status = SAI_STATUS_FAILURE;

    assert(NULL != port_map);
    assert(NULL != lanes);

    for (jj = 0; jj < mlnx_port_max_lanes_get(); jj++) {
        if (port_map->lane_bmap & (1 << jj)) {
            lanes[ii++] = port_map->module_port * mlnx_port_max_lanes_get() + jj;
        }
    }
    assert(ii == port_map->width);
    sai_status = mlnx_fill_u32list(lanes, port_map->width, &value->u32list);
    return sai_status;
}

/**
 * Function copies lanes list from hw into value u32list.list
 * DB read lock is needed
 *
 * @param port_id - sx port index
 * @param value   - it is expected that value.u32list.list [sai_u32_list_t] is allocated
 *                  and has enough space to keep all elements obtained from hw
 * @return        - #SAI_STATUS_SUCCESS on success Failure status code on error
 */
sai_status_t mlnx_sai_port_hw_lanes_get(_In_ sx_port_log_id_t *port_id, _Inout_ sai_attribute_value_t *value)
{
    sai_status_t      sai_status = SAI_STATUS_SUCCESS;
    uint32_t          port_db_idx = 0;
    uint32_t          lanes[MAX_LANES_SPC3_4] = {0};
    sx_status_t       sx_status = SX_STATUS_SUCCESS;
    sx_port_mapping_t port_map = {0};
    const bool        is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                               (!g_sai_db_ptr->issu_end_called);

    SX_LOG_ENTER();

    memset(lanes, 0, sizeof(lanes));

    if (is_warmboot_init_stage) {
        if (SAI_STATUS_SUCCESS !=
            (sai_status = mlnx_port_idx_by_log_id(*port_id, &port_db_idx))) {
            SX_LOG_ERR("Error getting port idx using port id %x\n", *port_id);
            goto bail;
        }
        sai_status = copy_port_hw_lanes(&(mlnx_ports_db[port_db_idx].port_map), lanes, value);
    } else {
        if (SX_STATUS_SUCCESS !=
            (sx_status = sx_api_port_mapping_get(gh_sdk, port_id, &port_map, 1))) {
            sai_status = sdk_to_sai(sx_status);
            goto bail;
        }
        sai_status = copy_port_hw_lanes(&port_map, lanes, value);
    }

bail:
    SX_LOG_EXIT();
    return sai_status;
}

/* Hardware Lane list [sai_u32_list_t] */
static sai_status_t mlnx_port_hw_lanes_get(_In_ const sai_object_key_t   *key,
                                           _Inout_ sai_attribute_value_t *value,
                                           _In_ uint32_t                  attr_index,
                                           _Inout_ vendor_cache_t        *cache,
                                           void                          *arg)
{
    sai_status_t     sai_status;
    sx_port_log_id_t port_id = 0;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (sai_status =
             mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        goto bail;
    }

    if (SAI_STATUS_SUCCESS !=
        (sai_status = mlnx_sai_port_hw_lanes_get(&port_id, value))) {
        goto bail;
    }

bail:
    SX_LOG_EXIT();
    return sai_status;
}

/* Breakout mode(s) supported [sai_s32_list_t] */
static sai_status_t mlnx_port_supported_breakout_get(_In_ const sai_object_key_t   *key,
                                                     _Inout_ sai_attribute_value_t *value,
                                                     _In_ uint32_t                  attr_index,
                                                     _Inout_ vendor_cache_t        *cache,
                                                     void                          *arg)
{
    int32_t             modes[SAI_PORT_BREAKOUT_MODE_TYPE_MAX];
    sx_port_log_id_t    port_log_id;
    uint32_t            modes_num;
    sai_status_t        status;
    mlnx_port_config_t *port;

    SX_LOG_ENTER();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_log_id, NULL);
    if (SAI_ERR(status)) {
        return status;
    }

    sai_db_read_lock();

    status = mlnx_port_by_log_id(port_log_id, &port);
    if (SAI_ERR(status)) {
        sai_db_unlock();
        return status;
    }

    modes[0] = SAI_PORT_BREAKOUT_MODE_TYPE_1_LANE;
    modes_num = 1;

    switch (port->breakout_modes) {
    case MLNX_PORT_BREAKOUT_CAPABILITY_NONE:
        break;

    case MLNX_PORT_BREAKOUT_CAPABILITY_TWO:
        modes[1] = SAI_PORT_BREAKOUT_MODE_TYPE_2_LANE;
        modes_num = 2;
        break;

    case MLNX_PORT_BREAKOUT_CAPABILITY_FOUR:
        modes[1] = SAI_PORT_BREAKOUT_MODE_TYPE_4_LANE;
        modes_num = 2;
        break;

    case MLNX_PORT_BREAKOUT_CAPABILITY_TWO_FOUR:
        modes[1] = SAI_PORT_BREAKOUT_MODE_TYPE_2_LANE;
        modes[2] = SAI_PORT_BREAKOUT_MODE_TYPE_4_LANE;
        modes_num = 3;
        break;

    default:
        SX_LOG_ERR("Invalid breakout capability %d port %" PRIx64 "\n",
                   port->breakout_modes, key->key.object_id);
        sai_db_unlock();
        return SAI_STATUS_FAILURE;
    }

    status = mlnx_fill_s32list(modes, modes_num, &value->s32list);

    sai_db_unlock();

    SX_LOG_EXIT();
    return status;
}

/* Current breakout mode [sai_port_breakout_mode_type_t] */
static sai_status_t mlnx_port_current_breakout_get(_In_ const sai_object_key_t   *key,
                                                   _Inout_ sai_attribute_value_t *value,
                                                   _In_ uint32_t                  attr_index,
                                                   _Inout_ vendor_cache_t        *cache,
                                                   void                          *arg)
{
    SX_LOG_ENTER();

    value->s32 = SAI_PORT_BREAKOUT_MODE_TYPE_1_LANE;

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Query list of supported port speed in Mbps [sai_u32_list_t] */
static sai_status_t mlnx_port_supported_speed_get(_In_ const sai_object_key_t   *key,
                                                  _Inout_ sai_attribute_value_t *value,
                                                  _In_ uint32_t                  attr_index,
                                                  _Inout_ vendor_cache_t        *cache,
                                                  void                          *arg)
{
    sai_status_t     status;
    sx_port_log_id_t port_id;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    status = mlnx_port_supported_speeds_get_impl(port_id, &value->u32list);

    SX_LOG_EXIT();
    return status;
}

/* Query list of supported port FEC mode [sai_s32_list_t] */
static sai_status_t mlnx_port_supported_fec_mode_get(_In_ const sai_object_key_t   *key,
                                                     _Inout_ sai_attribute_value_t *value,
                                                     _In_ uint32_t                  attr_index,
                                                     _Inout_ vendor_cache_t        *cache,
                                                     void                          *arg)
{
    int32_t             modes[] = { SAI_PORT_FEC_MODE_NONE, SAI_PORT_FEC_MODE_RS, SAI_PORT_FEC_MODE_FC };
    sai_status_t        status;
    sx_port_log_id_t    port_id;
    mlnx_port_config_t *port;

    SX_LOG_ENTER();

    sai_db_read_lock();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert port oid to logical port id\n");
        goto out;
    }

    status = mlnx_port_by_log_id(port_id, &port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to lookup port by log id %x\n", port_id);
        goto out;
    }

    /* RJ45 ports on 2201 #1-#48 don't support fec */
    if ((g_sai_db_ptr->platform_type == MLNX_PLATFORM_TYPE_2201) && (port->module < 48)) {
        value->s32list.count = 0;
    } else {
        status = mlnx_fill_s32list(modes, sizeof(modes) / sizeof(modes[0]), &value->s32list);
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_supported_auto_neg_mode_get(_In_ const sai_object_key_t   *key,
                                                          _Inout_ sai_attribute_value_t *value,
                                                          _In_ uint32_t                  attr_index,
                                                          _Inout_ vendor_cache_t        *cache,
                                                          void                          *arg)
{
    SX_LOG_ENTER();

    value->booldata = true;

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_supported_flow_control_mode_get(_In_ const sai_object_key_t   *key,
                                                              _Inout_ sai_attribute_value_t *value,
                                                              _In_ uint32_t                  attr_index,
                                                              _Inout_ vendor_cache_t        *cache,
                                                              void                          *arg)
{
    int32_t      modes[] = { SAI_PORT_FLOW_CONTROL_MODE_DISABLE,
                             SAI_PORT_FLOW_CONTROL_MODE_TX_ONLY,
                             SAI_PORT_FLOW_CONTROL_MODE_RX_ONLY,
                             SAI_PORT_FLOW_CONTROL_MODE_BOTH_ENABLE };
    sai_status_t status;

    SX_LOG_ENTER();

    status = mlnx_fill_s32list(modes, ARRAY_SIZE(modes), &value->s32list);

    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_number_of_priority_groups_get(_In_ const sai_object_key_t   *key,
                                                            _Inout_ sai_attribute_value_t *value,
                                                            _In_ uint32_t                  attr_index,
                                                            _Inout_ vendor_cache_t        *cache,
                                                            void                          *arg)
{
    sai_status_t     status;
    sx_port_log_id_t port_id;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    if (mlnx_log_port_is_cpu(port_id)) {
        value->u32 = 0;
    } else {
        value->u32 = mlnx_sai_get_buffer_resource_limits()->num_port_pg_buff;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_priority_group_list_get(_In_ const sai_object_key_t   *key,
                                                      _Inout_ sai_attribute_value_t *value,
                                                      _In_ uint32_t                  attr_index,
                                                      _Inout_ vendor_cache_t        *cache,
                                                      void                          *arg)
{
    sai_status_t     sai_status;
    sai_object_id_t  sai_pg = SAI_NULL_OBJECT_ID;
    uint8_t          port_pg_ind = 0;
    uint32_t         db_port_index;
    uint8_t          extended_data[EXTENDED_DATA_SIZE];
    sai_object_id_t* sai_pg_array = NULL;
    sx_port_log_id_t port_id;

    SX_LOG_ENTER();
    memset(extended_data, 0, sizeof(extended_data));

    if (SAI_STATUS_SUCCESS !=
        (sai_status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return sai_status;
    }

    if (mlnx_log_port_is_cpu(port_id)) {
        value->objlist.count = 0;
    } else {
        sai_pg_array = calloc(mlnx_sai_get_buffer_resource_limits()->num_port_pg_buff, sizeof(sai_object_id_t));
        if (NULL == sai_pg_array) {
            SX_LOG_ERR("Can't allocate pg array\n");
            sai_status = SAI_STATUS_NO_MEMORY;
            goto out;
        }
        cl_plock_acquire(&g_sai_db_ptr->p_lock);
        sai_status = mlnx_port_idx_by_obj_id(key->key.object_id, &db_port_index);
        cl_plock_release(&g_sai_db_ptr->p_lock);
        if (SAI_STATUS_SUCCESS != sai_status) {
            goto out;
        }
        for (port_pg_ind = 0; port_pg_ind < mlnx_sai_get_buffer_resource_limits()->num_port_pg_buff; port_pg_ind++) {
            extended_data[0] = port_pg_ind;
            if (SAI_STATUS_SUCCESS !=
                (sai_status =
                     mlnx_create_object(SAI_OBJECT_TYPE_INGRESS_PRIORITY_GROUP, db_port_index, extended_data,
                                        &sai_pg))) {
                goto out;
            }
            sai_pg_array[port_pg_ind] = sai_pg;
        }
        sai_status = mlnx_fill_objlist(sai_pg_array,
                                       mlnx_sai_get_buffer_resource_limits()->num_port_pg_buff, &value->objlist);
    }

out:
    if (sai_pg_array) {
        free(sai_pg_array);
    }
    SX_LOG_EXIT();
    return sai_status;
}

/* Operational speed in Mbps [uint32_t] */
static sai_status_t mlnx_port_oper_speed_get(_In_ const sai_object_key_t   *key,
                                             _Inout_ sai_attribute_value_t *value,
                                             _In_ uint32_t                  attr_index,
                                             _Inout_ vendor_cache_t        *cache,
                                             void                          *arg)
{
    sai_status_t     status;
    sx_port_log_id_t port_id;
    uint32_t         admin_speed;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    status = mlnx_port_speed_get_impl(port_id, &value->u32, &admin_speed);

    SX_LOG_EXIT();
    return status;
}

/* Full Duplex setting [bool] */
static sai_status_t mlnx_port_duplex_get(_In_ const sai_object_key_t   *key,
                                         _Inout_ sai_attribute_value_t *value,
                                         _In_ uint32_t                  attr_index,
                                         _Inout_ vendor_cache_t        *cache,
                                         void                          *arg)
{
    SX_LOG_ENTER();

    value->booldata = true;

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_attr_get(_In_ const sai_object_key_t   *key,
                                       _Inout_ sai_attribute_value_t *value,
                                       _In_ uint32_t                  attr_index,
                                       _Inout_ vendor_cache_t        *cache,
                                       void                          *arg)
{
    sai_status_t        status;
    sx_port_log_id_t    port_id;
    mlnx_port_config_t *port;
    long                attr_id = (long)arg;

    assert((SAI_PORT_ATTR_ADVERTISED_SPEED == attr_id) ||
           (SAI_PORT_ATTR_INTERFACE_TYPE == attr_id) ||
           (SAI_PORT_ATTR_ADVERTISED_INTERFACE_TYPE == attr_id) ||
           (SAI_PORT_ATTR_SPEED == attr_id) ||
           (SAI_PORT_ATTR_AUTO_NEG_MODE == attr_id));

    SX_LOG_ENTER();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        return status;
    }

    sai_db_read_lock();

    status = mlnx_port_by_log_id(port_id, &port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to lookup port by log id %x\n", port_id);
        goto out;
    }

    switch (attr_id) {
    case SAI_PORT_ATTR_SPEED:
        value->u32 = port->speed;
        break;

    case SAI_PORT_ATTR_AUTO_NEG_MODE:
        value->booldata = port->auto_neg == AUTO_NEG_DISABLE ? false : true;
        break;

    case SAI_PORT_ATTR_ADVERTISED_SPEED:
        status = mlnx_fill_u32list(port->adv_speeds, port->adv_speeds_num, &value->u32list);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to fill list.\n");
            goto out;
        }
        break;

    case SAI_PORT_ATTR_INTERFACE_TYPE:
        value->s32 = port->intf;
        break;

    case SAI_PORT_ATTR_ADVERTISED_INTERFACE_TYPE:
        status = mlnx_fill_s32list((int32_t *)port->adv_intfs, port->adv_intfs_num, &value->s32list);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to fill list.\n");
            goto out;
        }
        break;

    default:
        SX_LOG_ERR("Not supported attr_id: %ld\n", attr_id);
        status = SAI_STATUS_NOT_SUPPORTED;
        goto out;
    }

out:
    sai_db_unlock();
    return status;
}

static sai_status_t mlnx_port_attr_set(_In_ const sai_object_key_t      *key,
                                       _In_ const sai_attribute_value_t *value,
                                       void                             *arg)
{
    sai_status_t              status;
    sx_port_log_id_t          port_id;
    mlnx_port_config_t       *port = NULL;
    long                      attr_id = (long)arg;
    uint32_t                  old_speed = 0;
    sai_port_interface_type_t old_intf = 0;
    uint32_t                  old_adv_speeds[MAX_PORT_ATTR_ADV_SPEEDS_NUM] = {0};
    uint32_t                  old_adv_speeds_num = 0;
    sai_port_interface_type_t old_adv_intfs[MAX_PORT_ATTR_ADV_INTFS_NUM] = {0};
    uint32_t                  old_adv_intfs_num = 0;
    mlnx_port_autoneg_type_t  old_auto_neg = 0;
    bool                      is_warmboot_init_stage = false;

    assert((SAI_PORT_ATTR_ADVERTISED_SPEED == attr_id) ||
           (SAI_PORT_ATTR_INTERFACE_TYPE == attr_id) ||
           (SAI_PORT_ATTR_ADVERTISED_INTERFACE_TYPE == attr_id) ||
           (SAI_PORT_ATTR_SPEED == attr_id) ||
           (SAI_PORT_ATTR_AUTO_NEG_MODE == attr_id));

    SX_LOG_ENTER();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        return status;
    }

    sai_db_read_lock();

    is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                             (!g_sai_db_ptr->issu_end_called);

    status = mlnx_port_by_log_id(port_id, &port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to lookup port by log id %x\n", port_id);
        goto out;
    }

    switch (attr_id) {
    case SAI_PORT_ATTR_SPEED:
        old_speed = port->speed;
        port->speed = value->u32;
        break;

    case SAI_PORT_ATTR_AUTO_NEG_MODE:
        old_auto_neg = port->auto_neg;
        port->auto_neg = value->booldata ? AUTO_NEG_ENABLE : AUTO_NEG_DISABLE;
        break;

    case SAI_PORT_ATTR_ADVERTISED_SPEED:
        old_adv_speeds_num = port->adv_speeds_num;
        for (uint32_t ii = 0; ii < old_adv_speeds_num; ++ii) {
            old_adv_speeds[ii] = port->adv_speeds[ii];
        }
        port->adv_speeds_num = value->u32list.count;
        for (uint32_t ii = 0; ii < port->adv_speeds_num; ++ii) {
            port->adv_speeds[ii] = value->u32list.list[ii];
        }
        break;

    case SAI_PORT_ATTR_INTERFACE_TYPE:
        old_intf = port->intf;
        port->intf = value->s32;
        break;

    case SAI_PORT_ATTR_ADVERTISED_INTERFACE_TYPE:
        old_adv_intfs_num = port->adv_intfs_num;
        for (uint32_t ii = 0; ii < old_adv_intfs_num; ++ii) {
            old_adv_intfs[ii] = port->adv_intfs[ii];
        }
        port->adv_intfs_num = value->s32list.count;
        for (uint32_t ii = 0; ii < port->adv_intfs_num; ++ii) {
            port->adv_intfs[ii] = value->s32list.list[ii];
        }
        break;

    default:
        SX_LOG_ERR("Not supported attr_id: %ld\n", attr_id);
        status = SAI_STATUS_NOT_SUPPORTED;
        goto out;
    }

    if (!is_warmboot_init_stage) {
        status = mlnx_port_update_speed(port);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to update speed.\n");
            status = SAI_STATUS_INVALID_ATTR_VALUE_0;
            goto out;
        }
    }

out:
    if (SAI_ERR(status)) {
        if (port) {
            switch (attr_id) {
            case SAI_PORT_ATTR_SPEED:
                port->speed = old_speed;
                break;

            case SAI_PORT_ATTR_AUTO_NEG_MODE:
                port->auto_neg = old_auto_neg;
                break;

            case SAI_PORT_ATTR_ADVERTISED_SPEED:
                port->adv_speeds_num = old_adv_speeds_num;
                for (uint32_t ii = 0; ii < port->adv_speeds_num; ++ii) {
                    port->adv_speeds[ii] = old_adv_speeds[ii];
                }
                break;

            case SAI_PORT_ATTR_INTERFACE_TYPE:
                port->intf = old_intf;
                break;

            case SAI_PORT_ATTR_ADVERTISED_INTERFACE_TYPE:
                port->adv_intfs_num = old_adv_intfs_num;
                for (uint32_t ii = 0; ii < port->adv_intfs_num; ++ii) {
                    port->adv_intfs[ii] = old_adv_intfs[ii];
                }
                break;

            default:
                break;
            }
        }
    }
    sai_db_unlock();
    return status;
}


static sai_status_t mlnx_port_sai_offset_get(_In_ uint32_t speed, _Out_ uint64_t *offset)
{
    assert(offset);
    switch (speed) {
    case PORT_SPEED_800:
        *offset = 12;
        break;

    case PORT_SPEED_400:
        *offset = 11;
        break;

    case PORT_SPEED_200:
        *offset = 10;
        break;

    case PORT_SPEED_100:
        *offset = 9;
        break;

    case PORT_SPEED_56:
        *offset = 8;
        break;

    case PORT_SPEED_50:
        *offset = 7;
        break;

    case PORT_SPEED_40:
        *offset = 6;
        break;

    case PORT_SPEED_25:
        *offset = 5;
        break;

    case PORT_SPEED_20:
        *offset = 4;
        break;

    case PORT_SPEED_10:
        *offset = 3;
        break;

    case PORT_SPEED_1:
        *offset = 2;
        break;

    case PORT_SPEED_100M:
        *offset = 1;
        break;

    case PORT_SPEED_10M:
        *offset = 13;
        break;

    case PORT_SPEED_0:
        *offset = 0;
        break;

    default:
        SX_LOG_ERR("Unsupported speed [%u].\n", speed);
        return SAI_STATUS_FAILURE;
    }
    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_fill_speeds(_In_ uint32_t *speeds, _In_ uint32_t speeds_num, _Out_ uint64_t *bitmap)
{
    sai_status_t status;
    uint64_t     offset = 0;

    assert(bitmap);

    if (!speeds || (speeds_num == 0)) {
        for (int32_t ii = 0; ii < MAX_NUM_PORT_SPEEDS; ii++) {
            *bitmap |= mlnx_port_cb->speed_bitmap[ii];
        }
        return SAI_STATUS_SUCCESS;
    }

    for (uint32_t ii = 0; ii < speeds_num; ++ii) {
        status = mlnx_port_sai_offset_get(speeds[ii], &offset);
        if (SAI_ERR(status)) {
            return SAI_STATUS_FAILURE;
        }
        *bitmap |= mlnx_port_cb->speed_bitmap[offset];
    }

    return SAI_STATUS_SUCCESS;
}


static sai_status_t mlnx_port_fill_intfs(_In_ sai_port_interface_type_t *intfs,
                                         _In_ uint32_t                   intfs_num,
                                         _Out_ uint64_t                 *bitmap)
{
    assert(bitmap);

    if (!intfs || (intfs_num == 0)) {
        for (int32_t ii = 0; ii < MAX_NUM_PORT_INTFS; ii++) {
            *bitmap |= mlnx_port_cb->intf_bitmap[ii];
        }
        return SAI_STATUS_SUCCESS;
    }

    for (uint32_t ii = 0; ii < intfs_num; ++ii) {
        if ((intfs[ii] >= SAI_PORT_INTERFACE_TYPE_MAX) || (intfs[ii] < 0)) {
            SX_LOG_ERR("Unsupported interface type [%d].\n", intfs[ii]);
            return SAI_STATUS_FAILURE;
        }
        *bitmap |= mlnx_port_cb->intf_bitmap[intfs[ii]];
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_speed_intf_intersection(_In_ uint64_t  *speed_bitmap,
                                                      _In_ uint64_t  *intf_bitmap,
                                                      _Out_ uint64_t *result_bitmap)
{
    assert(speed_bitmap);
    assert(intf_bitmap);
    assert(result_bitmap);

    *result_bitmap = *speed_bitmap & *intf_bitmap;

    return *result_bitmap ? SAI_STATUS_SUCCESS : SAI_STATUS_NOT_SUPPORTED;
}

static sai_status_t mlnx_port_speed_intf_intersection_with_mask(_In_ uint32_t lanes_num, _Inout_ uint64_t *bitmap)
{
    assert(bitmap);

    if (lanes_num > MAX_LANES_SPC3_4) {
        SX_LOG_ERR("Unsupported lanes number [%u].\n", lanes_num);
        return SAI_STATUS_FAILURE;
    }

    *bitmap &= mlnx_port_cb->lanes_speed_bitmap[lanes_num];

    return *bitmap ? SAI_STATUS_SUCCESS : SAI_STATUS_NOT_SUPPORTED;
}

static sai_status_t mlnx_port_speeds_merge(_In_ bool                       auto_neg,
                                           _In_ uint32_t                   lanes_num,
                                           _In_ uint32_t                  *speeds,
                                           _In_ uint32_t                   speeds_num,
                                           _In_ sai_port_interface_type_t *intfs,
                                           _In_ uint32_t                   intfs_num,
                                           _In_ mlnx_port_config_t        *port,
                                           _Out_ uint64_t                 *bitmap)
{
    sai_status_t status;
    uint64_t     speed_bitmap = 0;
    uint64_t     intf_bitmap = 0;

    *bitmap = 0;

    status = mlnx_port_fill_speeds(speeds, speeds_num, &speed_bitmap);
    if (SAI_ERR(status)) {
        return status;
    }
    status = mlnx_port_fill_intfs(intfs, intfs_num, &intf_bitmap);
    if (SAI_ERR(status)) {
        return status;
    }
    status = mlnx_port_speed_intf_intersection(&speed_bitmap, &intf_bitmap, bitmap);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Port [Speed[0x%08" PRIx64 "] - Interface_Type[0x%08" PRIx64 "]] configuration is invalid.\n",
                   speed_bitmap,
                   intf_bitmap);
        return status;
    }
    status = mlnx_port_speed_intf_intersection_with_mask(lanes_num, bitmap);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Port [Speed-Interface_Type] configuration doesn't match mask for %d lanes.\n", lanes_num);
        return status;
    }

    /* turn off 1GB optics on RJ45 ports on 2201 #1-#48, as at force, SDK considers them as 2 different speeds */
    if ((g_sai_db_ptr->platform_type == MLNX_PLATFORM_TYPE_2201) && (port->module < 48)) {
        *bitmap &= ~(SP_1GB_CX_SGMII | SP_1GB_KX);
    }
    /* turn off 1000mb_t on spc1 non RJ45 */
    else if (mlnx_chip_is_spc()) {
        *bitmap &= ~(SP_1000MB_T);
    }

    return status;
}

static sai_status_t mlnx_port_speed_intf_bitmap_to_sx_sp(_In_ uint64_t                     bitmap,
                                                         _Out_ sx_port_speed_capability_t *sx_speed)
{
    assert(sx_speed);

    sx_speed->mode_1GB_CX_SGMII = !!(bitmap & SP_1GB_CX_SGMII);
    sx_speed->mode_1GB_KX = !!(bitmap & SP_1GB_KX);
    sx_speed->mode_10GB_CX4_XAUI = !!(bitmap & SP_10GB_CX4_XAUI);
    sx_speed->mode_10GB_KX4 = !!(bitmap & SP_10GB_KX4);
    sx_speed->mode_10GB_KR = !!(bitmap & SP_10GB_KR);
    sx_speed->mode_20GB_KR2 = !!(bitmap & SP_20GB_KR2);
    sx_speed->mode_40GB_CR4 = !!(bitmap & SP_40GB_CR4);
    sx_speed->mode_40GB_KR4 = !!(bitmap & SP_40GB_KR4);
    sx_speed->mode_56GB_KR4 = !!(bitmap & SP_56GB_KR4);
    sx_speed->mode_56GB_KX4 = !!(bitmap & SP_56GB_KX4);
    sx_speed->mode_10GB_CR = !!(bitmap & SP_10GB_CR);
    sx_speed->mode_10GB_SR = !!(bitmap & SP_10GB_SR);
    sx_speed->mode_10GB_ER_LR = !!(bitmap & SP_10GB_ER_LR);
    sx_speed->mode_40GB_SR4 = !!(bitmap & SP_40GB_SR4);
    sx_speed->mode_40GB_LR4_ER4 = !!(bitmap & SP_40GB_LR4_ER4);
    sx_speed->mode_100GB_CR4 = !!(bitmap & SP_100GB_CR4);
    sx_speed->mode_100GB_SR4 = !!(bitmap & SP_100GB_SR4);
    sx_speed->mode_100GB_KR4 = !!(bitmap & SP_100GB_KR4);
    sx_speed->mode_100GB_LR4_ER4 = !!(bitmap & SP_100GB_LR4_ER4);
    sx_speed->mode_25GB_CR = !!(bitmap & SP_25GB_CR);
    sx_speed->mode_25GB_KR = !!(bitmap & SP_25GB_KR);
    sx_speed->mode_25GB_SR = !!(bitmap & SP_25GB_SR);
    sx_speed->mode_50GB_CR2 = !!(bitmap & SP_50GB_CR2);
    sx_speed->mode_50GB_KR2 = !!(bitmap & SP_50GB_KR2);
    sx_speed->mode_50GB_SR2 = !!(bitmap & SP_50GB_SR2);
    sx_speed->mode_10MB_T = !!(bitmap & SP_10MB_T);
    sx_speed->mode_100MB_TX = !!(bitmap & SP_100MB_TX);
    sx_speed->mode_1000MB_T = !!(bitmap & SP_1000MB_T);
    sx_speed->mode_auto = !!(bitmap & SP_auto);

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_speed_intf_bitmap_to_sx_sp2(_In_ uint64_t bitmap, _Out_ sx_port_rate_bitmask_t *sx_speed)
{
    assert(sx_speed);

    sx_speed->rate_100M = !!(bitmap & SP2_100M);
    sx_speed->rate_1G = !!(bitmap & SP2_1G);
    sx_speed->rate_10G = !!(bitmap & SP2_10G);
    sx_speed->rate_25G = !!(bitmap & SP2_25G);
    sx_speed->rate_40G = !!(bitmap & SP2_40G);
    sx_speed->rate_50G = !!(bitmap & SP2_50G);
    sx_speed->rate_50Gx1 = !!(bitmap & SP2_50Gx1);
    sx_speed->rate_50Gx2 = !!(bitmap & SP2_50Gx2);
    sx_speed->rate_100G = !!(bitmap & SP2_100G);
    sx_speed->rate_100Gx2 = !!(bitmap & SP2_100Gx2);
    sx_speed->rate_100Gx4 = !!(bitmap & SP2_100Gx4);
    sx_speed->rate_200G = !!(bitmap & SP2_200G);
    sx_speed->rate_200Gx4 = !!(bitmap & SP2_200Gx4);
    sx_speed->rate_400G = !!(bitmap & SP2_400G);
    sx_speed->rate_400Gx8 = !!(bitmap & SP2_400Gx8);
    sx_speed->rate_auto = !!(bitmap & SP2_auto);

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_speed_intf_bitmap_to_sx_sp4(_In_ uint64_t bitmap, _Out_ sx_port_rate_bitmask_t *sx_speed)
{
    assert(sx_speed);

    sx_speed->rate_100M = !!(bitmap & SP4_100M);
    sx_speed->rate_1G = !!(bitmap & SP4_1G);
    sx_speed->rate_10G = !!(bitmap & SP4_10G);
    sx_speed->rate_25G = !!(bitmap & SP4_25G);
    sx_speed->rate_40G = !!(bitmap & SP4_40G);
    sx_speed->rate_50G = !!(bitmap & SP4_50G);
    sx_speed->rate_50Gx1 = !!(bitmap & SP4_50Gx1);
    sx_speed->rate_50Gx2 = !!(bitmap & SP4_50Gx2);
    sx_speed->rate_100G = !!(bitmap & SP4_100G);
    sx_speed->rate_100Gx1 = !!(bitmap & SP4_100Gx1);
    sx_speed->rate_100Gx2 = !!(bitmap & SP4_100Gx2);
    sx_speed->rate_100Gx4 = !!(bitmap & SP4_100Gx4);
    sx_speed->rate_200G = !!(bitmap & SP4_200G);
    sx_speed->rate_200Gx2 = !!(bitmap & SP4_200Gx2);
    sx_speed->rate_200Gx4 = !!(bitmap & SP4_200Gx4);
    sx_speed->rate_400G = !!(bitmap & SP4_400G);
    sx_speed->rate_400Gx4 = !!(bitmap & SP4_400Gx4);
    sx_speed->rate_400Gx8 = !!(bitmap & SP4_400Gx8);
    sx_speed->rate_800G = !!(bitmap & SP4_800G);
    sx_speed->rate_800Gx8 = !!(bitmap & SP4_800Gx8);
    sx_speed->rate_auto = !!(bitmap & SP4_auto);

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_update_speed_sp(_In_ sx_port_log_id_t sx_port, _In_ bool auto_neg, _In_ uint64_t bitmap)
{
    sai_status_t               status;
    sx_status_t                sx_status;
    sx_port_speed_capability_t sx_speed;

    status = mlnx_port_speed_intf_bitmap_to_sx_sp(bitmap, &sx_speed);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert speed bitmap [%" PRIx64 "] to SDK.\n", bitmap);
        return status;
    }

    sx_speed.force = !auto_neg;

    sx_status = sx_api_port_speed_admin_set(gh_sdk, sx_port, &sx_speed);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to set port speed - %s.\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_update_speed_sp2(_In_ sx_port_log_id_t sx_port, _In_ bool auto_neg, _In_ uint64_t bitmap)
{
    sai_status_t           status;
    sx_status_t            sx_status;
    sx_port_rate_bitmask_t sx_speed;

    status = mlnx_port_speed_intf_bitmap_to_sx_sp2(bitmap, &sx_speed);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert speed bitmap [%" PRIx64 "] to SDK.\n", bitmap);
        return status;
    }

    sx_speed.force = !auto_neg;

    if (sx_speed.rate_50Gx1 && sx_speed.rate_50Gx2) {
        sx_speed.rate_50G = true;
        sx_speed.rate_50Gx1 = false;
        sx_speed.rate_50Gx2 = false;
    }
    if (sx_speed.rate_100Gx2 && sx_speed.rate_100Gx4) {
        sx_speed.rate_100G = true;
        sx_speed.rate_100Gx2 = false;
        sx_speed.rate_100Gx4 = false;
    }
    if (sx_speed.rate_200G && sx_speed.rate_200Gx4) {
        sx_speed.rate_200G = true;
        sx_speed.rate_200Gx4 = false;
    }
    if (sx_speed.rate_400G && sx_speed.rate_400Gx8) {
        sx_speed.rate_400G = true;
        sx_speed.rate_400Gx8 = false;
    }

    sx_status = sx_api_port_rate_set(gh_sdk, sx_port, &sx_speed);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to set port %x rate - %s.\n", sx_port, SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_update_speed_sp4(_In_ sx_port_log_id_t sx_port, _In_ bool auto_neg, _In_ uint64_t bitmap)
{
    sai_status_t           status;
    sx_status_t            sx_status;
    sx_port_rate_bitmask_t sx_speed;

    status = mlnx_port_speed_intf_bitmap_to_sx_sp4(bitmap, &sx_speed);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert speed bitmap [%" PRIx64 "] to SDK.\n", bitmap);
        return status;
    }

    sx_speed.force = !auto_neg;

    if (sx_speed.rate_50Gx1 && sx_speed.rate_50Gx2) {
        sx_speed.rate_50G = true;
        sx_speed.rate_50Gx1 = false;
        sx_speed.rate_50Gx2 = false;
    }
    if (sx_speed.rate_100Gx2 && sx_speed.rate_100Gx1 && sx_speed.rate_100Gx4) {
        sx_speed.rate_100G = true;
        sx_speed.rate_100Gx1 = false;
        sx_speed.rate_100Gx2 = false;
        sx_speed.rate_100Gx4 = false;
    }
    if (sx_speed.rate_200G && sx_speed.rate_200Gx2 && sx_speed.rate_200Gx4) {
        sx_speed.rate_200G = true;
        sx_speed.rate_200Gx2 = false;
        sx_speed.rate_200Gx4 = false;
    }
    if (sx_speed.rate_400G && sx_speed.rate_400Gx4 && sx_speed.rate_400Gx8) {
        sx_speed.rate_400G = true;
        sx_speed.rate_400Gx4 = false;
        sx_speed.rate_400Gx8 = false;
    }
    if (sx_speed.rate_800G && sx_speed.rate_800Gx8) {
        sx_speed.rate_800G = true;
        sx_speed.rate_800Gx8 = false;
    }

    sx_status = sx_api_port_rate_set(gh_sdk, sx_port, &sx_speed);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to set port %x rate - %s.\n", sx_port, SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_update_speed(_In_ mlnx_port_config_t *port)
{
    sai_status_t               status;
    uint64_t                   bitmap = 0;
    bool                       autoneg = false;
    uint32_t                  *speeds = 0;
    uint32_t                   speeds_num = 0;
    sai_port_interface_type_t *intfs = 0;
    uint32_t                   intfs_num = 0;

    assert(port);

    switch (port->auto_neg) {
    case AUTO_NEG_DEFAULT:
        autoneg = true;
        speeds = &port->speed;
        speeds_num = 1;
        intfs = &port->intf;
        intfs_num = 1;
        break;

    case AUTO_NEG_DISABLE:
        autoneg = false;
        speeds = &port->speed;
        speeds_num = 1;
        intfs = &port->intf;
        intfs_num = 1;
        break;

    case AUTO_NEG_ENABLE:
        autoneg = true;
        speeds = port->adv_speeds;
        speeds_num = port->adv_speeds_num;
        intfs = port->adv_intfs;
        intfs_num = port->adv_intfs_num;
        break;

    default:
        SX_LOG_ERR("Unexpected auto_neg value: %d", port->auto_neg);
        return SAI_STATUS_FAILURE;
    }

    status = mlnx_port_speeds_merge(autoneg,
                                    port->port_map.width,
                                    speeds, speeds_num,
                                    intfs, intfs_num,
                                    port,
                                    &bitmap);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to merge speeds, interface types and auto_neg.\n");
        return status;
    }

    status = mlnx_port_cb->update_speed(port->logical, autoneg, bitmap);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to apply speed/intf bitmap [0x%" PRIx64 "] for port[0x%x].\n", bitmap, port->logical);
        return status;
    }

    return status;
}

/* Port/LAG VLAN ID [sai_vlan_id_t]
 * Untagged ingress frames are tagged with Port VLAN ID (PVID)
 */
sai_status_t mlnx_port_lag_pvid_attr_get(_In_ const sai_object_key_t   *key,
                                         _Inout_ sai_attribute_value_t *value,
                                         _In_ uint32_t                  attr_index,
                                         _Inout_ vendor_cache_t        *cache,
                                         void                          *arg)
{
    sai_status_t        status;
    mlnx_port_config_t *port_config;
    sx_vid_t            pvid;
    bool                is_warmboot_init_stage = false;
    uint32_t            port_db_idx;

    SX_LOG_ENTER();

    sai_db_read_lock();

    is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                             (!g_sai_db_ptr->issu_end_called);
    if (is_warmboot_init_stage) {
        status = mlnx_port_idx_by_obj_id(key->key.object_id, &port_db_idx);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Error getting port idx from log id %" PRIx64 "\n", key->key.object_id);
            goto out;
        }
        /* If the LAG does not have LAG member, then read from LAG DB */
        if ((port_db_idx >= MAX_PORTS) && (port_db_idx < MAX_PORTS * 2) &&
            (0 == mlnx_ports_db[port_db_idx].logical)) {
            value->u16 = mlnx_ports_db[port_db_idx].issu_lag_attr.lag_pvid;
            status = SAI_STATUS_SUCCESS;
            goto out;
        } else if ((port_db_idx < MAX_PORTS) && (0 != mlnx_ports_db[port_db_idx].before_issu_lag_id) &&
                   (0 == mlnx_ports_db[port_db_idx].lag_id)) {
            /* If port is a LAG member in SDK, but not in SAI, then read from port DB */
            value->u16 = mlnx_ports_db[port_db_idx].issu_lag_attr.lag_pvid;
            status = SAI_STATUS_SUCCESS;
            goto out;
        }
    }

    status = mlnx_port_by_obj_id(key->key.object_id, &port_config);
    if (SAI_ERR(status)) {
        goto out;
    }

    if (SX_STATUS_SUCCESS != (status = sx_api_vlan_port_pvid_get(gh_sdk, port_config->logical, &pvid))) {
        SX_LOG_ERR("Failed to get %s %x pvid - %s.\n", mlnx_port_type_str(port_config),
                   port_config->logical, SX_STATUS_MSG(status));
        status = sdk_to_sai(status);
        goto out;
    }

    value->u16 = pvid;

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/* Default VLAN Priority [uint8_t]
 *  (default to 0) */
sai_status_t mlnx_port_lag_default_vlan_prio_get(_In_ const sai_object_key_t   *key,
                                                 _Inout_ sai_attribute_value_t *value,
                                                 _In_ uint32_t                  attr_index,
                                                 _Inout_ vendor_cache_t        *cache,
                                                 void                          *arg)
{
    sai_status_t      status;
    sx_port_log_id_t  port_id;
    sx_cos_priority_t prio;
    bool              is_warmboot_init_stage = false;
    uint32_t          port_db_idx;

    SX_LOG_ENTER();

    is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                             (!g_sai_db_ptr->issu_end_called);
    if (is_warmboot_init_stage) {
        status = mlnx_port_idx_by_obj_id(key->key.object_id, &port_db_idx);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Error getting port idx from log id %" PRIx64 "\n", key->key.object_id);
            SX_LOG_EXIT();
            return status;
        }
        /* If the LAG does not have LAG member, then read from LAG DB */
        if ((port_db_idx >= MAX_PORTS) && (port_db_idx < MAX_PORTS * 2) &&
            (0 == mlnx_ports_db[port_db_idx].logical)) {
            value->u8 = mlnx_ports_db[port_db_idx].issu_lag_attr.lag_default_vlan_priority;
            SX_LOG_EXIT();
            return SAI_STATUS_SUCCESS;
        } else if ((port_db_idx < MAX_PORTS) && (0 != mlnx_ports_db[port_db_idx].before_issu_lag_id) &&
                   (0 == mlnx_ports_db[port_db_idx].lag_id)) {
            /* If port is a LAG member in SDK, but not in SAI, then read from port DB */
            value->u8 = mlnx_ports_db[port_db_idx].issu_lag_attr.lag_default_vlan_priority;
            SX_LOG_EXIT();
            return SAI_STATUS_SUCCESS;
        }
    }

    if (SAI_STATUS_SUCCESS != (status = mlnx_object_to_log_port(key->key.object_id, &port_id))) {
        SX_LOG_EXIT();
        return status;
    }

    if (SX_STATUS_SUCCESS != (status = sx_api_cos_port_default_prio_get(gh_sdk, port_id, &prio))) {
        SX_LOG_ERR("Failed to get port default prio - %s.\n", SX_STATUS_MSG(status));
        SX_LOG_EXIT();
        return sdk_to_sai(status);
    }

    value->u8 = prio;

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Dropping of untagged frames on ingress [bool] */
/* Dropping of tagged frames on ingress [bool] */
sai_status_t mlnx_port_lag_drop_tags_get(_In_ const sai_object_key_t   *key,
                                         _Inout_ sai_attribute_value_t *value,
                                         _In_ uint32_t                  attr_index,
                                         _Inout_ vendor_cache_t        *cache,
                                         void                          *arg)
{
    sai_status_t          status;
    sai_attr_id_t         attr_id;
    sx_vlan_frame_types_t frame_types;
    bool                  is_warmboot_init_stage = false;
    sx_port_log_id_t      port_id;
    uint32_t              port_db_idx;

    SX_LOG_ENTER();

    attr_id = (long)arg;

    assert((SAI_PORT_ATTR_DROP_UNTAGGED == attr_id) || (SAI_PORT_ATTR_DROP_TAGGED == attr_id) ||
           (SAI_LAG_ATTR_DROP_UNTAGGED == attr_id) || (SAI_LAG_ATTR_DROP_TAGGED == attr_id));

    is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                             (!g_sai_db_ptr->issu_end_called);
    if (is_warmboot_init_stage) {
        status = mlnx_port_idx_by_obj_id(key->key.object_id, &port_db_idx);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Error getting port idx from log id %" PRIx64 "\n", key->key.object_id);
            SX_LOG_EXIT();
            return status;
        }
        /* If the LAG does not have LAG member, then read from LAG DB */
        if ((port_db_idx >= MAX_PORTS) && (port_db_idx < MAX_PORTS * 2) &&
            (0 == mlnx_ports_db[port_db_idx].logical)) {
            if (SAI_LAG_ATTR_DROP_UNTAGGED == attr_id) {
                value->booldata = mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_untagged;
            } else if (SAI_LAG_ATTR_DROP_TAGGED == attr_id) {
                value->booldata = mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_tagged;
            }
            SX_LOG_EXIT();
            return SAI_STATUS_SUCCESS;
        } else if ((port_db_idx < MAX_PORTS) && (0 != mlnx_ports_db[port_db_idx].before_issu_lag_id) &&
                   (0 == mlnx_ports_db[port_db_idx].lag_id)) {
            /* If port is a LAG member in SDK, but not in SAI, then read from port DB */
            if (SAI_PORT_ATTR_DROP_UNTAGGED == attr_id) {
                value->booldata = mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_untagged;
            } else if (SAI_PORT_ATTR_DROP_TAGGED == attr_id) {
                value->booldata = mlnx_ports_db[port_db_idx].issu_lag_attr.lag_drop_tagged;
            }
            SX_LOG_EXIT();
            return SAI_STATUS_SUCCESS;
        }
    }

    if (SAI_STATUS_SUCCESS != (status = mlnx_object_to_log_port(key->key.object_id, &port_id))) {
        SX_LOG_EXIT();
        return status;
    }

    if (SX_STATUS_SUCCESS != (status = sx_api_vlan_port_accptd_frm_types_get(gh_sdk, port_id, &frame_types))) {
        SX_LOG_ERR("Failed to get port accepted frame types - %s.\n", SX_STATUS_MSG(status));
        SX_LOG_EXIT();
        return sdk_to_sai(status);
    }

    if ((SAI_PORT_ATTR_DROP_UNTAGGED == attr_id) || (SAI_LAG_ATTR_DROP_UNTAGGED == attr_id)) {
        value->booldata = !(frame_types.allow_untagged);
    } else {
        value->booldata = !(frame_types.allow_tagged);
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Internal loopback control [sai_port_internal_loopback_mode_t] */
static sai_status_t mlnx_port_internal_loopback_get(_In_ const sai_object_key_t   *key,
                                                    _Inout_ sai_attribute_value_t *value,
                                                    _In_ uint32_t                  attr_index,
                                                    _Inout_ vendor_cache_t        *cache,
                                                    void                          *arg)
{
    sai_status_t            status;
    sx_port_log_id_t        port_id;
    sx_port_phys_loopback_t loopback;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    if (SX_STATUS_SUCCESS != (status = sx_api_port_phys_loopback_get(gh_sdk, port_id, &loopback))) {
        SX_LOG_ERR("Failed to get port physical loopback - %s.\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    /* is internal loopback enabled bool */
    if ((loopback == SX_PORT_PHYS_LOOPBACK_ENABLE_INTERNAL) ||
        (loopback == SX_PORT_PHYS_LOOPBACK_ENABLE_BOTH) ||
        (loopback == SX_PORT_PHYS_LOOPBACK_ENABLE_INTERNAL_LR)) {
        value->s32 = SAI_PORT_INTERNAL_LOOPBACK_MODE_MAC;
    } else {
        value->s32 = SAI_PORT_INTERNAL_LOOPBACK_MODE_NONE;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* MTU [uint32_t] */
static sai_status_t mlnx_port_mtu_get(_In_ const sai_object_key_t   *key,
                                      _Inout_ sai_attribute_value_t *value,
                                      _In_ uint32_t                  attr_index,
                                      _Inout_ vendor_cache_t        *cache,
                                      void                          *arg)
{
    sai_status_t     status;
    sx_port_log_id_t port_id;
    sx_port_mtu_t    max_mtu;
    sx_port_mtu_t    oper_mtu;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    if (SX_STATUS_SUCCESS != (status = sx_api_port_mtu_get(gh_sdk, port_id, &max_mtu, &oper_mtu))) {
        SX_LOG_ERR("Failed to get port mtu - %s.\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    value->u32 = oper_mtu;

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/** [sai_port_flow_control_mode_t]
 *  (default to SAI_PORT_FLOW_CONTROL_DISABLE) */
static sai_status_t mlnx_port_global_flow_ctrl_get(_In_ const sai_object_key_t   *key,
                                                   _Inout_ sai_attribute_value_t *value,
                                                   _In_ uint32_t                  attr_index,
                                                   _Inout_ vendor_cache_t        *cache,
                                                   void                          *arg)
{
    sx_port_log_id_t         port_id;
    sai_status_t             status;
    sx_port_flow_ctrl_mode_t ctrl_mode;

    SX_LOG_ENTER();

    sai_db_read_lock();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        goto out;
    }

    status = sx_api_port_global_fc_enable_get(gh_sdk, port_id, &ctrl_mode);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get port global flow control - %s\n", SX_STATUS_MSG(status));
        status = sdk_to_sai(status);
        goto out;
    }

    switch (ctrl_mode) {
    case SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_DIS:
        value->s32 = SAI_PORT_FLOW_CONTROL_MODE_DISABLE;
        break;

    case SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_DIS:
        value->s32 = SAI_PORT_FLOW_CONTROL_MODE_TX_ONLY;
        break;

    case SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_EN:
        value->s32 = SAI_PORT_FLOW_CONTROL_MODE_RX_ONLY;
        break;

    case SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_EN:
        value->s32 = SAI_PORT_FLOW_CONTROL_MODE_BOTH_ENABLE;
        break;

    default:
        SX_LOG_ERR("Invalid SDK global flow control mode %u\n", ctrl_mode);
        status = SAI_STATUS_FAILURE;
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/* Forward Error Correction (FEC) control [sai_port_fec_mode_t] */
static sai_status_t mlnx_port_fec_get(_In_ const sai_object_key_t   *key,
                                      _Inout_ sai_attribute_value_t *value,
                                      _In_ uint32_t                  attr_index,
                                      _Inout_ vendor_cache_t        *cache,
                                      void                          *arg)
{
    sx_port_log_id_t   port_id;
    sai_status_t       status;
    sx_port_phy_mode_t admin, oper;

/*TODO: remove when fixed */
#if defined IS_PLD || defined IS_SIMX
    /* FEC is not supported on Simx/PLD */
    SX_LOG_ERR("FEC is not supported on Simx/PLD.\n");
    return SAI_STATUS_NOT_SUPPORTED;
#endif

    SX_LOG_ENTER();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    status = sx_api_port_phy_mode_get(gh_sdk, port_id, SX_PORT_PHY_SPEED_50GB, &admin, &oper);
    if (status != SAI_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get phy mode - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    switch (admin.fec_mode) {
    case SX_PORT_FEC_MODE_NONE:
        value->s32 = SAI_PORT_FEC_MODE_NONE;
        break;

    case SX_PORT_FEC_MODE_FC:
        value->s32 = SAI_PORT_FEC_MODE_FC;
        break;

    case SX_PORT_FEC_MODE_RS:
        value->s32 = SAI_PORT_FEC_MODE_RS;
        break;

    case SX_PORT_FEC_MODE_AUTO:
        SX_LOG_ERR("SDK FEC auto has no translation in SAI\n");
        status = SAI_STATUS_FAILURE;
        goto out;

    default:
        SX_LOG_ERR("Invalid SDK fec mode %u\n", admin.fec_mode);
        status = SAI_STATUS_FAILURE;
        goto out;
    }

out:
    SX_LOG_EXIT();
    return status;
}

/* Update DSCP [bool] */
static sai_status_t mlnx_port_update_dscp_get(_In_ const sai_object_key_t   *key,
                                              _Inout_ sai_attribute_value_t *value,
                                              _In_ uint32_t                  attr_index,
                                              _Inout_ vendor_cache_t        *cache,
                                              void                          *arg)
{
    sx_cos_rewrite_enable_t rewrite_enable;
    sx_port_log_id_t        port_id;
    sai_status_t            status;

    SX_LOG_ENTER();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    status = sx_api_cos_port_rewrite_enable_get(gh_sdk, port_id, &rewrite_enable);
    if (status != SAI_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get dscp rewrite enable - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    value->booldata = rewrite_enable.rewrite_dscp;

    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_update_dscp_set_impl(_In_ sx_port_log_id_t port_id, _In_ bool enable)
{
    sx_status_t             sx_status;
    sx_cos_rewrite_enable_t rewrite_enable;

    sx_status = sx_api_cos_port_rewrite_enable_get(gh_sdk, port_id, &rewrite_enable);
    if (sx_status != SAI_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get dscp rewrite enable - %s\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    rewrite_enable.rewrite_dscp = enable;
    sx_status = sx_api_cos_port_rewrite_enable_set(gh_sdk, port_id, rewrite_enable);
    if (sx_status != SAI_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to set dscp rewrite enable - %s\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

/* Update DSCP [bool] */
static sai_status_t mlnx_port_update_dscp_set(_In_ const sai_object_key_t      *key,
                                              _In_ const sai_attribute_value_t *value,
                                              void                             *arg)
{
    sx_port_log_id_t    port_id;
    sai_status_t        status;
    mlnx_port_config_t *port;

    SX_LOG_ENTER();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        return status;
    }

    sai_db_write_lock();

    status = mlnx_port_by_log_id(port_id, &port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed lookup port by log id %x\n", port_id);
        goto out;
    }

    /* In case the port is a LAG member - delegate it to the LAG */
    if (mlnx_port_is_lag_member(port)) {
        port_id = mlnx_port_get_lag_id(port);
    }

    status = mlnx_port_update_dscp_set_impl(port_id, value->booldata);
    if (SAI_ERR(status)) {
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_mirror_session_get(_In_ const sai_object_key_t   *key,
                                                 _Inout_ sai_attribute_value_t *value,
                                                 _In_ uint32_t                  attr_index,
                                                 _Inout_ vendor_cache_t        *cache,
                                                 void                          *arg)
{
    sai_status_t          status = SAI_STATUS_FAILURE;
    sx_span_session_id_t  sdk_mirror_obj_id = 0;
    sx_mirror_direction_t sdk_mirror_direction = SX_SPAN_MIRROR_INGRESS;
    sai_object_id_t       sai_mirror_obj_id = 0;
    const uint32_t        sai_mirror_session_count = 1;
    mlnx_port_config_t   *port;
    sx_port_log_id_t      port_id;

    SX_LOG_ENTER();

    assert((MIRROR_INGRESS_PORT == (long)arg) || (MIRROR_EGRESS_PORT == (long)arg));

    if (MIRROR_INGRESS_PORT == (long)arg) {
        sdk_mirror_direction = SX_SPAN_MIRROR_INGRESS;
    } else if (MIRROR_EGRESS_PORT == (long)arg) {
        sdk_mirror_direction = SX_SPAN_MIRROR_EGRESS;
    }

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    sai_db_read_lock();

    if (mlnx_log_port_is_cpu(port_id)) {
        value->objlist.count = 0;
    } else {
        status = mlnx_port_by_obj_id(key->key.object_id, &port);
        if (SAI_ERR(status)) {
            goto out;
        }

        /* In case if port is LAG member then use LAG logical id */
        if (mlnx_port_is_lag_member(port)) {
            port_id = mlnx_port_get_lag_id(port);
        }

        if (SX_STATUS_SUCCESS !=
            (status = sx_api_span_mirror_get(gh_sdk, port_id, sdk_mirror_direction,
                                             &sdk_mirror_obj_id))) {
            value->objlist.count = 0;
            /* when no session is attached, api returns entry not found */
            if (SX_STATUS_ENTRY_NOT_FOUND == status) {
                status = SAI_STATUS_SUCCESS;
            } else {
                SX_LOG_ERR("Error getting sdk mirror object id from sdk mirror port id %d %s\n",
                           port_id,
                           SX_STATUS_MSG(status));
                status = sdk_to_sai(status);
            }
            goto out;
        }

        if (SAI_STATUS_SUCCESS !=
            (status =
                 mlnx_create_object(SAI_OBJECT_TYPE_MIRROR_SESSION, sdk_mirror_obj_id, NULL, &sai_mirror_obj_id))) {
            SX_LOG_ERR("Error creating sai mirror obj id from sdk mirror obj id %d\n", sdk_mirror_obj_id);
            goto out;
        }

        if (SAI_STATUS_SUCCESS !=
            (status = mlnx_fill_objlist(&sai_mirror_obj_id, sai_mirror_session_count, &value->objlist))) {
            SX_LOG_ERR("Error filling object list using sai mirror obj id %" PRId64 "\n", sai_mirror_obj_id);
            goto out;
        }
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_mirror_dir_params_check(_In_ const mlnx_port_config_t *port1,
                                                      _In_ const mlnx_port_config_t *port2,
                                                      _In_ sx_mirror_direction_t     sx_direction)
{
    sx_status_t          sx_status;
    sx_span_session_id_t sx_mirror_session_id1, sx_mirror_session_id2;
    bool                 session1_present, session2_present;

    assert(port1);
    assert(port2);
    assert((SX_SPAN_MIRROR_INGRESS == sx_direction) || (SX_SPAN_MIRROR_EGRESS == sx_direction));

    sx_mirror_session_id1 = sx_mirror_session_id2 = 0;
    session1_present = session2_present = true;

    sx_status = sx_api_span_mirror_get(gh_sdk, port1->logical, sx_direction, &sx_mirror_session_id1);
    if (SX_ERR(sx_status)) {
        if (sx_status == SX_STATUS_ENTRY_NOT_FOUND) {
            session1_present = false;
        } else {
            SX_LOG_ERR("Failed to get ingress mirror id for port %x - %s\n", port1->logical, SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
    }

    sx_status = sx_api_span_mirror_get(gh_sdk, port2->logical, sx_direction, &sx_mirror_session_id2);
    if (SX_ERR(sx_status)) {
        if (sx_status == SX_STATUS_ENTRY_NOT_FOUND) {
            session2_present = false;
        } else {
            SX_LOG_ERR("Failed to get ingress mirror id port %x - %s\n", port2->logical, SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
    }

    if (session1_present != session2_present) {
        SX_LOG_ERR("Port oid %" PRIx64 " and port oid %" PRIx64 " have different mirror ingress session state\n",
                   port1->saiport, port2->saiport);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if ((session1_present && session2_present) &&
        (sx_mirror_session_id1 != sx_mirror_session_id2)) {
        SX_LOG_ERR(
            "Port oid %" PRIx64 " and port oid %" PRIx64 " have different mirror ingress session ids (%d, %d)\n",
            port1->saiport,
            port2->saiport,
            sx_mirror_session_id1,
            sx_mirror_session_id2);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_mirror_params_check(_In_ const mlnx_port_config_t *port1, _In_ const mlnx_port_config_t *port2)
{
    sai_status_t status;

    assert(port1);
    assert(port2);

    status = mlnx_port_mirror_dir_params_check(port1, port2, SX_SPAN_MIRROR_INGRESS);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_port_mirror_dir_params_check(port1, port2, SX_SPAN_MIRROR_EGRESS);
    if (SAI_ERR(status)) {
        return status;
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_mirror_params_clear(_In_ mlnx_port_config_t *port_config)
{
    sai_status_t status;

    assert(port_config);

    status = mlnx_port_mirror_session_clear(port_config->logical, SX_SPAN_MIRROR_INGRESS);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_port_mirror_session_clear(port_config->logical, SX_SPAN_MIRROR_EGRESS);
    if (SAI_ERR(status)) {
        return status;
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_mirror_sessions_clone(_In_ mlnx_port_config_t *to, _In_ const mlnx_port_config_t *from)
{
    sai_status_t         status;
    sx_status_t          sx_status;
    sx_span_session_id_t sx_span_session_id;

    assert(to);
    assert(from);

    status = mlnx_port_mirror_session_clear(to->logical, SX_SPAN_MIRROR_INGRESS);
    if (SAI_ERR(status)) {
        return status;
    }

    sx_status = sx_api_span_mirror_get(gh_sdk, from->logical, SX_SPAN_MIRROR_INGRESS, &sx_span_session_id);
    if ((sx_status != SX_STATUS_ENTRY_NOT_FOUND) && SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to get ingress mirror id - %s\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    if (sx_status != SX_STATUS_ENTRY_NOT_FOUND) {
        status = mlnx_port_mirror_session_apply(to->logical, sx_span_session_id, SX_SPAN_MIRROR_INGRESS);
        if (SAI_ERR(status)) {
            return status;
        }

        SX_LOG_DBG("Successfully cloned ingress mirror session from [%lx] to [%lx]\n",
                   from->saiport, to->saiport);
    }

    status = mlnx_port_mirror_session_clear(to->logical, SX_SPAN_MIRROR_EGRESS);
    if (SAI_ERR(status)) {
        return status;
    }

    sx_status = sx_api_span_mirror_get(gh_sdk, from->logical, SX_SPAN_MIRROR_EGRESS, &sx_span_session_id);
    if ((sx_status != SX_STATUS_ENTRY_NOT_FOUND) && SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to get egress mirror id - %s\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    if (sx_status != SX_STATUS_ENTRY_NOT_FOUND) {
        status = mlnx_port_mirror_session_apply(to->logical, sx_span_session_id, SX_SPAN_MIRROR_EGRESS);
        if (SAI_ERR(status)) {
            return status;
        }

        SX_LOG_DBG("Successfully cloned egress mirror session from [%lx] to [%lx]\n",
                   from->saiport, to->saiport);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_mirror_session_clear(_In_ sx_port_log_id_t      port_id,
                                                   _In_ sx_mirror_direction_t sx_mirror_direction)
{
    sai_status_t         status;
    sx_status_t          sx_status;
    sx_span_session_id_t sx_span_session_id;

    assert((SX_SPAN_MIRROR_INGRESS == sx_mirror_direction) ||
           (SX_SPAN_MIRROR_EGRESS == sx_mirror_direction));

    sx_status = sx_api_span_mirror_get(gh_sdk, port_id, sx_mirror_direction, &sx_span_session_id);
    if (SX_ERR(sx_status)) {
        if (SX_STATUS_ENTRY_NOT_FOUND == sx_status) {
            SX_LOG_DBG("No mirror session on port %x - nothing to clear\n", port_id);
            return SAI_STATUS_SUCCESS;
        }

        SX_LOG_ERR("Error getting mirror obj id from sdk mirror port id %x\n", port_id);
        return sdk_to_sai(sx_status);
    }

    status = mlnx_port_mirror_session_set_internal(port_id, sx_mirror_direction, sx_span_session_id, false);
    if (SAI_ERR(status)) {
        return status;
    }

    SX_LOG_DBG("Successfully deleted sdk mirror port %x on sdk mirror obj id %d\n",
               port_id, sx_span_session_id);

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_mirror_session_apply(_In_ sx_port_log_id_t      port_id,
                                                   _In_ sx_span_session_id_t  sx_span_session_id,
                                                   _In_ sx_mirror_direction_t sx_mirror_direction)
{
    sai_status_t status;

    status =
        mlnx_port_mirror_session_set_internal(port_id, sx_mirror_direction, sx_span_session_id, true);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Error setting mirror session %d on port %x\n", sx_span_session_id, port_id);
        return status;
    }

    SX_LOG_DBG("Successfully set sdk mirror port %x on sdk mirror obj id %d\n",
               port_id, sx_span_session_id);

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_mirror_session_set_internal(_In_ sx_port_log_id_t      port_id,
                                                          _In_ sx_mirror_direction_t sx_mirror_direction,
                                                          _In_ sx_span_session_id_t  sx_span_session_id,
                                                          _In_ bool                  add)
{
    sai_status_t         status;
    sx_status_t          sx_status;
    sx_access_cmd_t      sx_cmd;
    sx_span_session_id_t sx_existing_span_session_id;

    sx_cmd = (add) ? SX_ACCESS_CMD_ADD : SX_ACCESS_CMD_DELETE;

    if (add) {
        /* In SONiC, user will try to set mirror session for all LAG members of a LAG.
         * Internally in SAI, we always promote LAG member to LAG when trying to set mirror session.
         * This will cause delay since we repeatedly clear the mirror session and reapply the same mirror session for the same LAG.
         * This will also print out error message in SDK when trying to set mirror session before clear, which SONiC does not like.
         * Thus, for LAG, we check if the same mirror session has already been applied, and skip it. */
        if (SX_PORT_TYPE_ID_GET(port_id) == SX_PORT_TYPE_LAG) {
            sx_status = sx_api_span_mirror_get(gh_sdk, port_id, sx_mirror_direction, &sx_existing_span_session_id);
            if (SX_ERR(sx_status)) {
                if (sx_status != SX_STATUS_ENTRY_NOT_FOUND) {
                    SX_LOG_ERR("Error getting existing span session on port %x\n", port_id);
                    return sdk_to_sai(sx_status);
                }
            } else if (sx_existing_span_session_id == sx_span_session_id) {
                SX_LOG_DBG("Trying to apply the same span session id %d on port %x, ignoring this command\n",
                           sx_span_session_id,
                           port_id);
                return SAI_STATUS_SUCCESS;
            } else {
                SX_LOG_DBG("LAG %x already has a mirror session - need to clear\n", port_id);
                status = mlnx_port_mirror_session_clear(port_id, sx_mirror_direction);
                if (SAI_ERR(status)) {
                    SX_LOG_ERR("Failed to clear mirror session on port %x\n", port_id);
                    return status;
                }
            }
        }
        sx_status = sx_api_span_mirror_set(gh_sdk, sx_cmd, port_id,
                                           sx_mirror_direction, sx_span_session_id);
        if (SX_ERR(sx_status)) {
            if (sx_status == SX_STATUS_ENTRY_ALREADY_EXISTS) {
                SX_LOG_DBG("Port %x already has a mirror session - need to clear\n", port_id);
                status = mlnx_port_mirror_session_clear(port_id, sx_mirror_direction);
                if (SAI_ERR(status)) {
                    SX_LOG_ERR("Failed to clear mirror session on port %x\n", port_id);
                    return status;
                }

                sx_status = sx_api_span_mirror_set(gh_sdk, sx_cmd, port_id,
                                                   sx_mirror_direction, sx_span_session_id);
                if (SX_ERR(sx_status)) {
                    SX_LOG_ERR("Failed to %s sdk mirror port %x for sdk mirror obj id %d\n",
                               SX_ACCESS_CMD_STR(sx_cmd), port_id, sx_span_session_id);
                    return sdk_to_sai(sx_status);
                }
            } else {
                SX_LOG_ERR("Failed to %s sdk mirror port %x for sdk mirror obj id %d\n",
                           SX_ACCESS_CMD_STR(sx_cmd), port_id, sx_span_session_id);
                return sdk_to_sai(sx_status);
            }
        }

        sx_status = sx_api_span_mirror_state_set(gh_sdk, port_id, sx_mirror_direction, add);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to update mirror port state to %s on sdk mirror port id %x\n",
                       (add) ? "true" : "false", port_id);
            return sdk_to_sai(sx_status);
        }
    } else {
        sx_status = sx_api_span_mirror_state_set(gh_sdk, port_id, sx_mirror_direction, add);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to update mirror port state to %s on sdk mirror port id %x\n",
                       (add) ? "true" : "false", port_id);
            return sdk_to_sai(sx_status);
        }

        sx_status = sx_api_span_mirror_set(gh_sdk, sx_cmd, port_id,
                                           sx_mirror_direction, sx_span_session_id);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to %s sdk mirror port %x for sdk mirror obj id %d\n",
                       SX_ACCESS_CMD_STR(sx_cmd), port_id, sx_span_session_id);
            return sdk_to_sai(sx_status);
        }
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_mirror_session_set_impl(_In_ sx_port_log_id_t             port_id,
                                                      _In_ sx_mirror_direction_t        sdk_mirror_direction,
                                                      _In_ const sai_attribute_value_t *value)
{
    sai_status_t         status;
    uint32_t             sdk_mirror_obj_id_u32;
    sx_span_session_id_t sx_span_session_id;

    assert(value);

    if (value->objlist.count > 1) {
        SX_LOG_ERR("Only one mirror session can be associated to a mirror port\n");
        return SAI_STATUS_NOT_SUPPORTED;
    }

    if (0 == value->objlist.count) {
        status = mlnx_port_mirror_session_clear(port_id, sdk_mirror_direction);
        if (SAI_ERR(status)) {
            return status;
        }
    } else {
        status = mlnx_object_to_type(value->objlist.list[0], SAI_OBJECT_TYPE_MIRROR_SESSION,
                                     &sdk_mirror_obj_id_u32, NULL);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Invalid sai mirror obj id %" PRId64 "\n", value->objlist.list[0]);
            return status;
        }

        sx_span_session_id = sdk_mirror_obj_id_u32;

        status = mlnx_port_mirror_session_apply(port_id, sx_span_session_id, sdk_mirror_direction);
        if (SAI_ERR(status)) {
            return status;
        }
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_mirror_session_set(_In_ const sai_object_key_t      *key,
                                                 _In_ const sai_attribute_value_t *value,
                                                 void                             *arg)
{
    sai_status_t          status = SAI_STATUS_FAILURE;
    mlnx_port_config_t   *port_config;
    sx_mirror_direction_t sdk_mirror_direction = SX_SPAN_MIRROR_INGRESS;
    sx_port_log_id_t      port_id;

    SX_LOG_ENTER();

    assert((MIRROR_INGRESS_PORT == (long)arg) || (MIRROR_EGRESS_PORT == (long)arg));

    if (MIRROR_INGRESS_PORT == (long)arg) {
        sdk_mirror_direction = SX_SPAN_MIRROR_INGRESS;
    } else if (MIRROR_EGRESS_PORT == (long)arg) {
        sdk_mirror_direction = SX_SPAN_MIRROR_EGRESS;
    }

    sai_db_write_lock();

    status = mlnx_port_by_obj_id(key->key.object_id, &port_config);
    if (SAI_ERR(status)) {
        goto out;
    }

    port_id = port_config->logical;

    if (mlnx_port_is_lag_member(port_config)) {
        port_id = mlnx_port_get_lag_id(port_config);
    }

    status = mlnx_port_mirror_session_set_impl(port_id, sdk_mirror_direction, value);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Error setting mirror session on port %x\n", port_id);
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_samplepacket_session_get(_In_ const sai_object_key_t   *key,
                                                       _Inout_ sai_attribute_value_t *value,
                                                       _In_ uint32_t                  attr_index,
                                                       _Inout_ vendor_cache_t        *cache,
                                                       void                          *arg)
{
    sai_status_t        status = SAI_STATUS_FAILURE;
    mlnx_port_config_t *port_config;
    sx_port_log_id_t    port_id;

    SX_LOG_ENTER();

    assert(SAMPLEPACKET_INGRESS_PORT == (long)arg);

    assert(NULL != g_sai_db_ptr);

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    sai_db_read_lock();

    if (mlnx_log_port_is_cpu(port_id)) {
        value->oid = SAI_NULL_OBJECT_ID;
    } else {
        status = mlnx_port_by_obj_id(key->key.object_id, &port_config);
        if (SAI_ERR(status)) {
            goto cleanup;
        }

        if (mlnx_port_is_sai_lag_member(port_config)) {
            status = mlnx_port_fetch_lag_if_lag_member(&port_config);
            if (SAI_ERR(status)) {
                goto cleanup;
            }
        }

        if (mlnx_port_is_sdk_lag_member_not_sai(port_config)) {
            port_id = mlnx_port_get_lag_id(port_config);
        }

        if (MLNX_INVALID_SAMPLEPACKET_SESSION == port_config->internal_ingress_samplepacket_obj_idx) {
            value->oid = SAI_NULL_OBJECT_ID;
            goto cleanup;
        }

        if (SAI_STATUS_SUCCESS !=
            (status =
                 mlnx_create_object(SAI_OBJECT_TYPE_SAMPLEPACKET, port_config->internal_ingress_samplepacket_obj_idx,
                                    NULL, &value->oid))) {
            goto cleanup;
        }
    }

cleanup:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/*
 * A soft clear only updates a SAI DB
 * It is needed when port is joining to the LAG (sflow will be cleared by SDK internally)
 */
sai_status_t mlnx_port_samplepacket_params_clear(_In_ mlnx_port_config_t *port_config, _In_ bool is_soft)
{
    sai_status_t status;

    assert(port_config);

    if (is_soft) {
        port_config->internal_ingress_samplepacket_obj_idx = MLNX_INVALID_SAMPLEPACKET_SESSION;
    } else {
        status = mlnx_port_samplepacket_session_set_internal(port_config,
                                                             port_config->logical,
                                                             MLNX_INVALID_SAMPLEPACKET_SESSION);
        if (SAI_ERR(status)) {
            return status;
        }
    }

    /* Make sure that egress sample packet session is empty */
    if (MLNX_INVALID_SAMPLEPACKET_SESSION != port_config->internal_egress_samplepacket_obj_idx) {
        SX_LOG_ERR("Invalid internal_egress_samplepacket_obj_idx [%d] - "
                   "Egress sample packet session is not supported but id is not invalid (%d)\n",
                   port_config->internal_egress_samplepacket_obj_idx,
                   MLNX_INVALID_SAMPLEPACKET_SESSION);
        return SAI_STATUS_FAILURE;
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_samplepacket_params_clone(_In_ mlnx_port_config_t *to, _In_ const mlnx_port_config_t *from)
{
    return mlnx_port_samplepacket_session_set_internal(to, to->logical, from->internal_ingress_samplepacket_obj_idx);
}

static sai_status_t mlnx_port_samplepacket_session_set_internal(_In_ mlnx_port_config_t *port_config,
                                                                _In_ sx_port_log_id_t    sx_port_log_id,
                                                                _In_ uint32_t            samplepacket_obj_idx)
{
    sx_status_t            sx_status;
    sx_access_cmd_t        sdk_cmd;
    sx_port_sflow_params_t sdk_sflow_params;

    assert(port_config);

    memset(&sdk_sflow_params, 0, sizeof(sx_port_sflow_params_t));

    if (MLNX_INVALID_SAMPLEPACKET_SESSION == samplepacket_obj_idx) {
        if (MLNX_INVALID_SAMPLEPACKET_SESSION ==
            port_config->internal_ingress_samplepacket_obj_idx) {
            SX_LOG_DBG("No internal ingress samplepacket object has been associated to sdk samplepacket port id %d\n",
                       sx_port_log_id);
            return SAI_STATUS_SUCCESS;
        } else {
            sx_status = sx_api_port_sflow_set(gh_sdk, SX_ACCESS_CMD_DELETE, sx_port_log_id, &sdk_sflow_params);
            if (SX_ERR(sx_status)) {
                SX_LOG_ERR(
                    "Error disassociating sdk port id %x (SAI OID %" PRIx64 ") with internal samplepacket obj idx %d\n",
                    sx_port_log_id,
                    port_config->saiport,
                    port_config->internal_ingress_samplepacket_obj_idx);
                return sdk_to_sai(sx_status);
            }
        }

        SX_LOG_DBG(
            "Successfully disassociated sdk port id %d (SAI OID %" PRIx64 ") with internal samplepacket obj idx %d\n",
            sx_port_log_id,
            port_config->saiport,
            port_config->internal_ingress_samplepacket_obj_idx);
    } else {
        if (MLNX_INVALID_SAMPLEPACKET_SESSION == port_config->internal_ingress_samplepacket_obj_idx) {
            sdk_cmd = SX_ACCESS_CMD_ADD;
        } else {
            sdk_cmd = SX_ACCESS_CMD_EDIT;
        }

        sdk_sflow_params.ratio =
            g_sai_db_ptr->mlnx_samplepacket_session[samplepacket_obj_idx].sai_sample_rate;
        sdk_sflow_params.deviation = 0;
        sdk_sflow_params.packet_types.uc = true;
        sdk_sflow_params.packet_types.mc = true;
        sdk_sflow_params.packet_types.bc = true;
        sdk_sflow_params.packet_types.uuc = true;
        sdk_sflow_params.packet_types.umc = true;

        sx_status = sx_api_port_sflow_set(gh_sdk, sdk_cmd, sx_port_log_id, &sdk_sflow_params);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR(
                "Error associating sdk port id %d (SAI OID %" PRIx64 ") with internal samplepacket obj idx %d\n",
                sx_port_log_id,
                port_config->saiport,
                samplepacket_obj_idx);
            return sdk_to_sai(sx_status);
        }
    }

    port_config->internal_ingress_samplepacket_obj_idx = samplepacket_obj_idx;

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_samplepacket_session_set_impl(_In_ mlnx_port_config_t *port_config,
                                                            _In_ sx_port_log_id_t    sx_port_log_id,
                                                            _In_ sai_object_id_t     sp_oid)
{
    sai_status_t status;
    uint32_t     internal_samplepacket_obj_idx = 0;

    assert(port_config);

    if (SAI_NULL_OBJECT_ID == sp_oid) {
        internal_samplepacket_obj_idx = MLNX_INVALID_SAMPLEPACKET_SESSION;
    } else {
        status = mlnx_object_to_type(sp_oid, SAI_OBJECT_TYPE_SAMPLEPACKET, &internal_samplepacket_obj_idx, NULL);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Invalid sai samplepacket obj idx %" PRIx64 "\n", sp_oid);
            return status;
        }
    }

    status = mlnx_port_samplepacket_session_set_internal(port_config, sx_port_log_id, internal_samplepacket_obj_idx);
    if (SAI_ERR(status)) {
        return status;
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_samplepacket_session_set(_In_ const sai_object_key_t      *key,
                                                       _In_ const sai_attribute_value_t *value,
                                                       void                             *arg)
{
    sai_status_t        status = SAI_STATUS_FAILURE;
    mlnx_port_config_t *port_config;
    sx_port_log_id_t    port_id;

    SX_LOG_ENTER();

    assert(SAMPLEPACKET_INGRESS_PORT == (long)arg);

    assert(NULL != g_sai_db_ptr);

    sai_db_write_lock();

    status = mlnx_port_by_obj_id(key->key.object_id, &port_config);
    if (SAI_ERR(status)) {
        goto out;
    }

    if (mlnx_port_is_sai_lag_member(port_config)) {
        status = mlnx_port_fetch_lag_if_lag_member(&port_config);
        if (SAI_ERR(status)) {
            goto out;
        }
    }

    port_id = port_config->logical;

    if (mlnx_port_is_sdk_lag_member_not_sai(port_config)) {
        port_id = mlnx_port_get_lag_id(port_config);
    }

    status = mlnx_port_samplepacket_session_set_impl(port_config, port_id, value->oid);
    if (SAI_ERR(status)) {
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/** Port default Traffic class Mapping [sai_uint8_t], Default TC=0*/
static sai_status_t mlnx_port_qos_default_tc_get(_In_ const sai_object_key_t   *key,
                                                 _Inout_ sai_attribute_value_t *value,
                                                 _In_ uint32_t                  attr_index,
                                                 _Inout_ vendor_cache_t        *cache,
                                                 void                          *arg)
{
    sai_status_t status;

    SX_LOG_ENTER();

    status = mlnx_port_tc_get(key->key.object_id, &value->u8);

    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_qos_default_tc_set_impl(_In_ mlnx_port_config_t *port_config,
                                                      _In_ sx_port_log_id_t    port_id,
                                                      _In_ uint8_t             tc)
{
    sai_status_t status;
    uint8_t      set_tc = tc;

    assert(port_config);

    if (!SX_CHECK_MAX(tc, MAX_PORT_PRIO)) {
        SX_LOG_ERR("Invalid tc(%u)\n", tc);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (tc == 0) {
        set_tc = g_sai_db_ptr->switch_default_tc;
    }

    status = mlnx_port_tc_set(port_id, set_tc);
    if (SAI_ERR(status)) {
        return status;
    }
    port_config->default_tc = tc;

    return SAI_STATUS_SUCCESS;
}

/** Port default Traffic class Mapping [sai_uint8_t], Default TC=0*/
static sai_status_t mlnx_port_qos_default_tc_set(_In_ const sai_object_key_t      *key,
                                                 _In_ const sai_attribute_value_t *value,
                                                 void                             *arg)
{
    mlnx_port_config_t *port;
    sai_status_t        status;
    sx_port_log_id_t    log_port;

    SX_LOG_ENTER();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &log_port, NULL);
    if (SAI_ERR(status)) {
        return status;
    }

    sai_db_write_lock();

    status = mlnx_port_by_log_id(log_port, &port);
    if (SAI_ERR(status)) {
        goto out;
    }

    /* In case the port is a LAG member - apply TC on the LAG */
    if (mlnx_port_is_lag_member(port)) {
        log_port = mlnx_port_get_lag_id(port);
    }

    status = mlnx_port_qos_default_tc_set_impl(port, log_port, value->u8);
    if (SAI_ERR(status)) {
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/* db read lock is needed */
static sai_status_t db_port_qos_map_id_get(_In_ const sai_object_id_t port_id,
                                           sai_qos_map_type_t         qos_map_type,
                                           sai_object_id_t           *oid)
{
    mlnx_port_config_t *port;
    sai_status_t        status;
    uint32_t            qos_map_id;
    sx_port_log_id_t    log_port;

    status = mlnx_object_to_type(port_id, SAI_OBJECT_TYPE_PORT, &log_port, NULL);
    if (SAI_ERR(status)) {
        return status;
    }

    if (mlnx_log_port_is_cpu(log_port)) {
        *oid = SAI_NULL_OBJECT_ID;
        return SAI_STATUS_SUCCESS;
    }

    status = mlnx_port_by_log_id(log_port, &port);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    qos_map_id = port->qos_maps[qos_map_type];
    if (!qos_map_id) {
        *oid = SAI_NULL_OBJECT_ID;
        return SAI_STATUS_SUCCESS;
    }

    return mlnx_create_object(SAI_OBJECT_TYPE_QOS_MAP, qos_map_id, NULL, oid);
}

/* db read/write lock is needed */
static sai_status_t db_port_qos_map_id_set(_In_ const sai_object_id_t port_id,
                                           sai_qos_map_type_t         qos_map_type,
                                           sai_object_id_t            oid)
{
    uint32_t            qos_map_id;
    sai_status_t        status;
    mlnx_port_config_t *port;
    sx_port_log_id_t    log_port;

    if (oid == SAI_NULL_OBJECT_ID) {
        qos_map_id = 0;
    } else {
        status = mlnx_object_to_type(oid, SAI_OBJECT_TYPE_QOS_MAP, &qos_map_id, NULL);
        if (status != SAI_STATUS_SUCCESS) {
            return status;
        }
    }

    status = mlnx_object_to_type(port_id, SAI_OBJECT_TYPE_PORT, &log_port, NULL);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_port_by_log_id(log_port, &port);
    if (SAI_ERR(status)) {
        return status;
    }

    if (mlnx_port_is_lag_member(port)) {
        status = mlnx_port_by_log_id(port->lag_id, &port);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed lookup port config by log id %x\n", port->lag_id);
            return status;
        }
    }

    port->qos_maps[qos_map_type] = qos_map_id;
    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_qos_params_clear(_In_ mlnx_port_config_t *port_config)
{
    assert(port_config);

    memset(port_config->qos_maps, 0, sizeof(port_config->qos_maps));

    return SAI_STATUS_SUCCESS;
}

/** QoS Map Id [sai_object_id_t] */
static sai_status_t mlnx_port_qos_map_id_get(_In_ const sai_object_key_t   *key,
                                             _Inout_ sai_attribute_value_t *value,
                                             _In_ uint32_t                  attr_index,
                                             _Inout_ vendor_cache_t        *cache,
                                             void                          *arg)
{
    sai_qos_map_type_t qos_map_type = (sai_qos_map_type_t)arg;
    sai_status_t       status;
    uint32_t           map_idx;

    assert(qos_map_type < MLNX_QOS_MAP_TYPES_MAX);

    SX_LOG_ENTER();

    sai_db_read_lock();

    status = db_port_qos_map_id_get(key->key.object_id, qos_map_type, &value->oid);
    if (status != SAI_STATUS_SUCCESS) {
        goto out;
    }

    if (value->oid != SAI_NULL_OBJECT_ID) {
        goto out;
    }

    map_idx = g_sai_db_ptr->switch_qos_maps[qos_map_type];
    if (!map_idx) {
        value->oid = SAI_NULL_OBJECT_ID;
        goto out;
    }

    status = mlnx_create_object(SAI_OBJECT_TYPE_QOS_MAP, map_idx, NULL, &value->oid);

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/* db read lock is needed */
static sai_status_t mlnx_port_qos_map_trust_level_set(sx_port_log_id_t port_id, mlnx_qos_map_t *qos_map, bool enabled)
{
    sx_cos_trust_level_t curr_level;
    sx_cos_trust_level_t level;
    sai_status_t         status;
    const bool           is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                                  (!g_sai_db_ptr->issu_end_called);

    if ((qos_map->type == SAI_QOS_MAP_TYPE_DOT1P_TO_TC) ||
        (qos_map->type == SAI_QOS_MAP_TYPE_DOT1P_TO_COLOR)) {
        level = SX_COS_TRUST_LEVEL_L2;
    } else if ((qos_map->type == SAI_QOS_MAP_TYPE_DSCP_TO_TC) ||
               (qos_map->type == SAI_QOS_MAP_TYPE_DSCP_TO_COLOR)) {
        level = SX_COS_TRUST_LEVEL_L3;
    } else {
        assert(false);
    }

    status = sx_api_cos_port_trust_get(gh_sdk, port_id, &curr_level);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get trust level - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    if (enabled) {
        if (((curr_level == SX_COS_TRUST_LEVEL_L2) || (curr_level == SX_COS_TRUST_LEVEL_L3)) &&
            (curr_level != level)) {
            curr_level = SX_COS_TRUST_LEVEL_BOTH;
        } else if (curr_level == SX_COS_TRUST_LEVEL_PORT) {
            curr_level = level;
        }
    } else if (!is_warmboot_init_stage) {
        /* We are not able to access port config for the LAG during ISSU, we don't expect default QoS map to be set
         * during initialization, so skip it */
        mlnx_port_config_t *port_cfg = NULL;

        status = mlnx_port_by_log_id(port_id, &port_cfg);
        if (SAI_ERR(status)) {
            return status;
        }

        /* Don't disable trust level if one of {DOT1P,DSCP}_TO_{TC,COLOR} mapping is set */
        if ((level == SX_COS_TRUST_LEVEL_L2) && (qos_map->type == SAI_QOS_MAP_TYPE_DOT1P_TO_TC) &&
            (port_cfg->qos_maps[SAI_QOS_MAP_TYPE_DOT1P_TO_COLOR] != SAI_NULL_OBJECT_ID)) {
            return SAI_STATUS_SUCCESS;
        } else if ((level == SX_COS_TRUST_LEVEL_L2) && (qos_map->type == SAI_QOS_MAP_TYPE_DOT1P_TO_COLOR) &&
                   (port_cfg->qos_maps[SAI_QOS_MAP_TYPE_DOT1P_TO_TC] != SAI_NULL_OBJECT_ID)) {
            return SAI_STATUS_SUCCESS;
        } else if ((level == SX_COS_TRUST_LEVEL_L3) && (qos_map->type == SAI_QOS_MAP_TYPE_DSCP_TO_TC) &&
                   (port_cfg->qos_maps[SAI_QOS_MAP_TYPE_DSCP_TO_COLOR] != SAI_NULL_OBJECT_ID)) {
            return SAI_STATUS_SUCCESS;
        } else if ((level == SX_COS_TRUST_LEVEL_L3) && (qos_map->type == SAI_QOS_MAP_TYPE_DSCP_TO_COLOR) &&
                   (port_cfg->qos_maps[SAI_QOS_MAP_TYPE_DSCP_TO_TC] != SAI_NULL_OBJECT_ID)) {
            return SAI_STATUS_SUCCESS;
        }

        if (curr_level == SX_COS_TRUST_LEVEL_BOTH) {
            if (level == SX_COS_TRUST_LEVEL_L2) {
                curr_level = SX_COS_TRUST_LEVEL_L3;
            } else if (level == SX_COS_TRUST_LEVEL_L3) {
                curr_level = SX_COS_TRUST_LEVEL_L2;
            }
        } else if (curr_level == level) {
            curr_level = SX_COS_TRUST_LEVEL_PORT;
        }
    }

    status = sx_api_cos_port_trust_set(gh_sdk, port_id, curr_level);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to change trust level - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    return SAI_STATUS_SUCCESS;
}

/* db lock is needed */
static sai_status_t mlnx_port_qos_map_assign_dot1p_to_tc_color(sx_port_log_id_t port_id, mlnx_qos_map_t *qos_map)
{
    sx_status_t             status;
    uint32_t                count = (COS_PCP_MAX_NUM + 1) * (COS_DEI_MAX_NUM + 1);
    sx_cos_pcp_dei_t        pcp_dei[(COS_PCP_MAX_NUM + 1) * (COS_DEI_MAX_NUM + 1)];
    sx_cos_priority_color_t prio_color[(COS_PCP_MAX_NUM + 1) * (COS_DEI_MAX_NUM + 1)];
    uint32_t                ii, jj;

    status = sx_api_cos_port_pcpdei_to_prio_get(gh_sdk, port_id, pcp_dei, prio_color, &count);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get pcp to prio qos map - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    for (ii = 0; ii < qos_map->count; ii++) {
        for (jj = 0; jj < count; jj++) {
            if (qos_map->from.pcp_dei[ii].pcp != pcp_dei[jj].pcp) {
                continue;
            }

            if (qos_map->type == SAI_QOS_MAP_TYPE_DOT1P_TO_TC) {
                prio_color[jj].priority = qos_map->to.prio_color[ii].priority;
            } else if (qos_map->type == SAI_QOS_MAP_TYPE_DOT1P_TO_COLOR) {
                prio_color[jj].color = qos_map->to.prio_color[ii].color;
            } else {
                /* We should not reach here but who knows ...*/
                assert(false);
            }
        }
    }

    status = sx_api_cos_port_pcpdei_to_prio_set(gh_sdk, port_id, pcp_dei, prio_color, count);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to set pcp to prio qos map - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_qos_map_assign_dscp_to_tc_color(sx_port_log_id_t port_id, mlnx_qos_map_t *qos_map)
{
    sx_status_t             status;
    uint32_t                count = SX_COS_PORT_DSCP_MAX + 1;
    sx_cos_dscp_t           dscp[SX_COS_PORT_DSCP_MAX + 1];
    sx_cos_priority_color_t prio_color[SX_COS_PORT_DSCP_MAX + 1];
    uint32_t                ii, jj;

    status = sx_api_cos_port_dscp_to_prio_get(gh_sdk, port_id, dscp, prio_color, &count);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get dscp to prio qos map - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    for (ii = 0; ii < qos_map->count; ii++) {
        for (jj = 0; jj < count; jj++) {
            if (qos_map->from.dscp[ii] != dscp[jj]) {
                continue;
            }

            if (qos_map->type == SAI_QOS_MAP_TYPE_DSCP_TO_TC) {
                prio_color[jj].priority = qos_map->to.prio_color[ii].priority;
            } else if (qos_map->type == SAI_QOS_MAP_TYPE_DSCP_TO_COLOR) {
                prio_color[jj].color = qos_map->to.prio_color[ii].color;
            } else {
                /* We should not reach here but who knows ...*/
                assert(false);
            }
        }
    }

    status = sx_api_cos_port_dscp_to_prio_set(gh_sdk, port_id, dscp, prio_color, count);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to set dscp to prio qos map - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_qos_map_assign_tc_to_queue(sx_port_log_id_t port_id, mlnx_qos_map_t *qos_map)
{
    sx_status_t status;
    uint32_t    ii;

    for (ii = 0; ii < qos_map->count; ii++) {
        status = sx_api_cos_port_tc_prio_map_set(gh_sdk,
                                                 SX_ACCESS_CMD_ADD,
                                                 port_id,
                                                 qos_map->from.prio_color[ii].priority,
                                                 qos_map->to.queue[ii]);

        if (status != SX_STATUS_SUCCESS) {
            SX_LOG_ERR("Failed to set tc[%u]=%u -> queue[%u]=%u mapping - %s\n",
                       ii, qos_map->from.prio_color[ii].priority,
                       ii, qos_map->to.queue[ii], SX_STATUS_MSG(status));

            return sdk_to_sai(status);
        }
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_qos_map_assign_tc_color_to_dscp(sx_port_log_id_t port_id, mlnx_qos_map_t *qos_map)
{
    sx_status_t status;

    if (mlnx_tunnel_dscp_remapping_enabled() && mlnx_port_is_tc_to_dscp_rewrite_done(port_id)) {
        SX_LOG_NTC("Skip set tc to dscp map for port id %x as it is rewrote by dscp remapping.\n",
                   port_id);
        return SAI_STATUS_SUCCESS;
    }

    status = sx_api_cos_port_prio_to_dscp_rewrite_set(gh_sdk,
                                                      port_id,
                                                      qos_map->from.prio_color,
                                                      qos_map->to.dscp,
                                                      qos_map->count);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to set prio/color to dscp qos map - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_qos_map_assign_tc_color_to_dot1p(sx_port_log_id_t port_id, mlnx_qos_map_t *qos_map)
{
    sx_status_t status;

    status = sx_api_cos_port_prio_to_pcpdei_rewrite_set(gh_sdk, port_id,
                                                        qos_map->from.prio_color,
                                                        qos_map->to.pcp_dei,
                                                        qos_map->count);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to set tc & color to dot1p qos map - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_qos_map_assign_pfc_to_pg(sx_port_log_id_t port_id, const mlnx_qos_map_t *qos_map)
{
    sx_cos_port_prio_buff_t prio_buff = {0};
    sx_status_t             sx_status = SX_STATUS_SUCCESS;
    uint32_t                ii = 0, pri = 0;

    sx_status = sx_api_cos_port_prio_buff_map_get(gh_sdk, port_id, &prio_buff);
    if (sx_status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get prio to buff qos map - %s\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    for (ii = 0; ii < qos_map->count; ii++) {
        uint8_t            pfc = qos_map->from.pfc[ii];
        uint8_t            pg = qos_map->to.pg[ii];
        sx_cos_priority_t  prios[SXD_COS_PORT_PRIO_MAX + 1] = {0};
        sx_cos_ieee_prio_t ieees[SXD_COS_PORT_PRIO_MAX + 1] = {0};
        uint32_t           count = 0;

        for (pri = 0; pri < SXD_COS_PORT_PRIO_MAX + 1; pri++) {
            if (pg != prio_buff.prio_to_buff[pri]) {
                continue;
            }

            ieees[count] = pfc;
            prios[count] = pri;
            count++;
        }

        if (!count) {
            continue;
        }

        sx_status = sx_api_cos_prio_to_ieeeprio_set(gh_sdk, prios, ieees, count);
        if (sx_status != SX_STATUS_SUCCESS) {
            SX_LOG_ERR("Failed to set prio to ieee qos map - %s\n", SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
    }

    memcpy(&g_sai_db_ptr->qos_maps_db[MLNX_QOS_MAP_PFC_PG_INDEX], qos_map, sizeof(*qos_map));
    g_sai_db_ptr->qos_maps_db[MLNX_QOS_MAP_PFC_PG_INDEX].is_set = true;

    return sdk_to_sai(sx_status);
}

static sai_status_t mlnx_port_qos_map_assign_pfc_to_queue(sx_port_log_id_t port_id, mlnx_qos_map_t *qos_map)
{
    sx_cos_priority_t  prios[SXD_COS_PORT_PRIO_MAX + 1];
    sx_cos_ieee_prio_t ieees[SXD_COS_PORT_PRIO_MAX + 1];
    uint32_t           count = SXD_COS_PORT_PRIO_MAX + 1;
    sx_status_t        status;
    uint32_t           ii, jj;

    status = sx_api_cos_prio_to_ieeeprio_get(gh_sdk, prios, ieees, &count);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get prio to ieee qos map - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    for (ii = 0; ii < qos_map->count; ii++) {
        for (jj = 0; jj < count; jj++) {
            if (ieees[jj] != qos_map->from.pfc[ii]) {
                continue;
            }

            status = sx_api_cos_port_tc_prio_map_set(gh_sdk,
                                                     SX_ACCESS_CMD_ADD,
                                                     port_id,
                                                     prios[jj],
                                                     qos_map->to.queue[ii]);

            if (status != SX_STATUS_SUCCESS) {
                SX_LOG_ERR("Failed to set tc[%u]=%u -> queue[%u]=%u mapping - %s\n",
                           ii, prios[jj],
                           ii, qos_map->to.queue[ii], SX_STATUS_MSG(status));

                return sdk_to_sai(status);
            }
        }
    }

    memcpy(&g_sai_db_ptr->qos_maps_db[MLNX_QOS_MAP_PFC_QUEUE_INDEX], qos_map, sizeof(*qos_map));
    g_sai_db_ptr->qos_maps_db[MLNX_QOS_MAP_PFC_QUEUE_INDEX].is_set = true;

    return status;
}

static sai_status_t mlnx_port_qos_map_assign_tc_to_pg(sx_port_log_id_t port_id, mlnx_qos_map_t *qos_map)
{
    sx_cos_port_prio_buff_t prio_buff;
    sai_status_t            sai_status;
    sx_status_t             sx_status;
    uint32_t                ii;
    uint8_t                 pg;
    mlnx_port_config_t     *mlnx_port_config = NULL;

    sx_status = sx_api_cos_port_prio_buff_map_get(gh_sdk, port_id, &prio_buff);
    if (sx_status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get prio to buff qos map - %s\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }
    sai_status = mlnx_port_by_log_id(port_id, &mlnx_port_config);
    if (SAI_STATUS_SUCCESS != sai_status) {
        SX_LOG_ERR("Error retrieve mlnx port config from sx port id 0x%x\n", port_id);
        return sai_status;
    }
    if (NULL == mlnx_port_config) {
        SX_LOG_ERR("Null mlnx_port_config\n");
        return SAI_STATUS_FAILURE;
    }

    for (ii = 0; ii < qos_map->count; ii++) {
        uint8_t pri = qos_map->from.prio_color[ii].priority;
        pg = qos_map->to.pg[ii];
        prio_buff.prio_to_buff[pri] = pg;
        if (MAX_LOSSLESS_SP <= pri) {
            SX_LOG_ERR("pri %d is greater or equal than SP limit %d\n",
                       pri, MAX_LOSSLESS_SP);
            return SAI_STATUS_FAILURE;
        }
        if (MAX_PG <= pg) {
            SX_LOG_ERR("pg %d is greater or equal than PG limit %d\n",
                       pg, MAX_PG);
            return SAI_STATUS_FAILURE;
        }
#ifdef ACS_OS
        if ((mlnx_port_config->lossless_pg[pg]) &&
            !g_sai_db_ptr->is_switch_priority_lossless[pri]) {
            sai_status = set_mc_sp_zero(pri);
            if (SAI_STATUS_SUCCESS != sai_status) {
                SX_LOG_ERR("Error setting multicast size of switch priority %d to zero\n", pri);
                return sai_status;
            }
            g_sai_db_ptr->is_switch_priority_lossless[pri] = true;
        }
#endif
    }

    sx_status = sx_api_cos_port_prio_buff_map_set(gh_sdk, SX_ACCESS_CMD_SET, port_id, &prio_buff);
    if (sx_status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to set prio to buff qos map - %s\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    /* Reapply PFC->PG, PFC->Queue maps, since they are dependent on TC->PG value, and in case they were applied prior
     *  the values would be incorrect. */
    if (g_sai_db_ptr->qos_maps_db[MLNX_QOS_MAP_PFC_PG_INDEX].is_set) {
        SX_LOG_INF("Reapplying PFC->PG\n");
        sai_status =
            mlnx_port_qos_map_assign_pfc_to_pg(port_id, &g_sai_db_ptr->qos_maps_db[MLNX_QOS_MAP_PFC_PG_INDEX]);
        if (SAI_ERR(sai_status)) {
            SX_LOG_ERR("Failed to reapply PFC to PG\n");
            return sai_status;
        }
    }

    if (g_sai_db_ptr->qos_maps_db[MLNX_QOS_MAP_PFC_QUEUE_INDEX].is_set) {
        SX_LOG_INF("Reapplying PFC->Queue\n");
        if (mlnx_port_is_lag_member(mlnx_port_config)) {
            port_id = mlnx_port_get_lag_id(mlnx_port_config);
        }

        sai_status =
            mlnx_port_qos_map_assign_pfc_to_queue(port_id,
                                                  &g_sai_db_ptr->qos_maps_db[MLNX_QOS_MAP_PFC_QUEUE_INDEX]);
        if (SAI_ERR(sai_status)) {
            SX_LOG_ERR("Failed to reapply PFC to QUEUE\n");
            return sai_status;
        }
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_qos_map_id_set_impl(_In_ sai_object_id_t    port,
                                                  _In_ sai_object_id_t    qos_map_id,
                                                  _In_ sai_qos_map_type_t qos_map_type)
{
    sai_status_t status;

    assert(qos_map_type < MLNX_QOS_MAP_TYPES_MAX);

    status = mlnx_port_qos_map_apply(port, qos_map_id, qos_map_type);
    if (SAI_ERR(status)) {
        return status;
    }

    status = db_port_qos_map_id_set(port, qos_map_type, qos_map_id);
    if (SAI_ERR(status)) {
        return status;
    }

    return status;
}

/*
 * Routine Description:
 *   Apply QoS params on the port (db read lock is needed).
 *
 * Arguments:
 *    [in] port - Port Id
 *    [in] qos_map_id - QoS Map Id
 *    [in] qos_map_type QoS Map Type
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
sai_status_t mlnx_port_qos_map_apply(_In_ const sai_object_id_t    port,
                                     _In_ const sai_object_id_t    qos_map_id,
                                     _In_ const sai_qos_map_type_t qos_map_type)
{
    bool                is_map_enabled = true;
    mlnx_qos_map_t      default_map;
    mlnx_qos_map_t     *qos_map = NULL;
    sai_status_t        status;
    sx_port_log_id_t    port_id;
    mlnx_port_config_t *port_cfg;

    status = mlnx_object_to_log_port(port, &port_id);
    if (status != SAI_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to convert port oid to logical port id\n");
        return status;
    }

    status = mlnx_port_by_log_id(port_id, &port_cfg);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed lookup port config by log id %x\n", port_id);
        return status;
    }

    /* Check if we can delegate applying QoS map to the LAG, in case of
     * SDK does not support applying it on LAG but to the port only */
    if ((qos_map_type == SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP) ||
        (qos_map_type == SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP)) {
        if (mlnx_port_is_lag(port_cfg)) {
            /* Nothing to be done for LAG */
            return SAI_STATUS_SUCCESS;
        }
    } else {
        /* in case the port is a LAG member - delegate QoS settings to the LAG */
        if (mlnx_port_is_lag_member(port_cfg)) {
            port_id = mlnx_port_get_lag_id(port_cfg);
        }
    }

    if (qos_map_id != SAI_NULL_OBJECT_ID) {
        status = mlnx_qos_map_get_by_id(qos_map_id, &qos_map);
        if (status != SAI_STATUS_SUCCESS) {
            SX_LOG_ERR("Invalid qos_map_id\n");
            return status;
        }
    } else if (g_sai_db_ptr->switch_qos_maps[qos_map_type]) {
        uint32_t        map_idx = g_sai_db_ptr->switch_qos_maps[qos_map_type];
        sai_object_id_t map_oid;

        status = mlnx_create_object(SAI_OBJECT_TYPE_QOS_MAP, map_idx, NULL, &map_oid);
        if (status != SAI_STATUS_SUCCESS) {
            SX_LOG_ERR("Failed create oid by switch qos map_idx=%u\n", map_idx);
            return status;
        }

        status = mlnx_qos_map_get_by_id(map_oid, &qos_map);
        if (status != SAI_STATUS_SUCCESS) {
            SX_LOG_ERR("Invalid qos_map_id from switch qos map oid=%" PRIx64 "\n", map_oid);
            return status;
        }
    } else {
        is_map_enabled = false;
        qos_map = &default_map;
        qos_map->type = qos_map_type;

        status = mlnx_qos_map_set_default(qos_map);
        if (status != SAI_STATUS_SUCCESS) {
            SX_LOG_ERR("Failed to set default QoS map\n");
            return status;
        }
    }

    if (qos_map->type != qos_map_type) {
        SX_LOG_ERR("Specified QoS map's type does not match port's QoS map attr type\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    switch (qos_map_type) {
    case SAI_QOS_MAP_TYPE_DOT1P_TO_TC:
    case SAI_QOS_MAP_TYPE_DOT1P_TO_COLOR:
        status = mlnx_port_qos_map_assign_dot1p_to_tc_color(port_id, qos_map);
        if (status != SAI_STATUS_SUCCESS) {
            return status;
        }

        status = mlnx_port_qos_map_trust_level_set(port_id, qos_map, is_map_enabled);
        break;

    case SAI_QOS_MAP_TYPE_DSCP_TO_TC:
    case SAI_QOS_MAP_TYPE_DSCP_TO_COLOR:
        status = mlnx_port_qos_map_assign_dscp_to_tc_color(port_id, qos_map);
        if (status != SAI_STATUS_SUCCESS) {
            return status;
        }

        status = mlnx_port_qos_map_trust_level_set(port_id, qos_map, is_map_enabled);
        break;

    case SAI_QOS_MAP_TYPE_TC_TO_QUEUE:
        status = mlnx_port_qos_map_assign_tc_to_queue(port_id, qos_map);
        if ((SAI_STATUS_SUCCESS == status) && mlnx_tunnel_dscp_remapping_enabled()) {
            status = mlnx_port_tc_to_queue_update_for_dscp_remapping(port_id, qos_map_id);
        }
        break;

    case SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP:
        status = mlnx_port_qos_map_assign_tc_color_to_dscp(port_id, qos_map);
        break;

    case SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DOT1P:
        status = mlnx_port_qos_map_assign_tc_color_to_dot1p(port_id, qos_map);
        break;

    case SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP:
        if (is_map_enabled) {
            status = mlnx_port_qos_map_assign_tc_to_pg(port_id, qos_map);
            if ((SAI_STATUS_SUCCESS == status) && mlnx_tunnel_dscp_remapping_enabled()) {
                status = mlnx_port_tc_to_pg_update_for_dscp_remapping(port_id, qos_map_id);
            }
        }
        break;

    case SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP:
        if (is_map_enabled) {
            status = mlnx_port_qos_map_assign_pfc_to_pg(port_id, qos_map);
        }
        break;

    case SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_QUEUE:
        if (is_map_enabled) {
            status = mlnx_port_qos_map_assign_pfc_to_queue(port_id, qos_map);
        }
        break;

    case SAI_QOS_MAP_TYPE_CUSTOM_RANGE_BASE:
    default:
        status = SAI_STATUS_NOT_SUPPORTED;
        SX_LOG_ERR("Not supported qos_map_type (%u)\n", qos_map_type);
        return status;
    }

    return status;
}

/*
 * Routine Description:
 *   Set default traffic class on the port
 *
 * Arguments:
 *    [in] port - Port Id
 *    [in] tc - traffic class
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
sai_status_t mlnx_port_tc_set(sx_port_log_id_t port_id, _In_ const uint8_t tc)
{
    sai_status_t status;

    status = sx_api_cos_port_default_prio_set(gh_sdk, port_id, tc);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to set port's default tc(%u) - %s\n", tc, SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    return SAI_STATUS_SUCCESS;
}

/*
 * Routine Description:
 *   Get default traffic class on the port
 *
 * Arguments:
 *    [in] port - Port Id
 *    [out] tc - traffic class
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_port_tc_get(_In_ const sai_object_id_t port, _Out_ uint8_t *tc)
{
    sx_port_log_id_t port_id;
    sai_status_t     status;

    status = mlnx_object_to_type(port, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    status = sx_api_cos_port_default_prio_get(gh_sdk, port_id, tc);
    if (status != SX_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to get port's default traffic class - %s\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    return SAI_STATUS_SUCCESS;
}

/** QoS Map Id [sai_object_id_t] */
static sai_status_t mlnx_port_qos_map_id_set(_In_ const sai_object_key_t      *key,
                                             _In_ const sai_attribute_value_t *value,
                                             void                             *arg)
{
    sai_qos_map_type_t qos_map_type = (sai_qos_map_type_t)arg;
    sx_port_log_id_t   port_id;
    sai_status_t       status;

    SX_LOG_ENTER();

    assert(qos_map_type < MLNX_QOS_MAP_TYPES_MAX);

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        return status;
    }

    sai_db_write_lock();

    status = mlnx_port_qos_map_id_set_impl(key->key.object_id, value->oid, qos_map_type);
    if (status != SAI_STATUS_SUCCESS) {
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/** bit vector enable/disable port PFC [sai_uint8_t].
 * Valid from bit 0 to bit 7 */
static sai_status_t mlnx_port_pfc_control_get(_In_ const sai_object_key_t   *key,
                                              _Inout_ sai_attribute_value_t *value,
                                              _In_ uint32_t                  attr_index,
                                              _Inout_ vendor_cache_t        *cache,
                                              void                          *arg)
{
    sx_port_log_id_t         port_id;
    uint8_t                  pfc_ctrl_map = 0;
    sai_status_t             status;
    uint8_t                  pfc_prio;
    sx_port_flow_ctrl_mode_t flow_mode;

    SX_LOG_ENTER();

    assert((SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL == (long)arg) || (SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_RX == (long)arg) ||
           (SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_TX == (long)arg));

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        SX_LOG_ERR("Failed to convert port oid to logical port id\n");
        return status;
    }

    for (pfc_prio = 0; pfc_prio < COS_IEEE_PRIO_MAX_NUM + 1; pfc_prio++) {
        status = sx_api_port_pfc_enable_get(gh_sdk, port_id, pfc_prio, &flow_mode);
        if (status != SAI_STATUS_SUCCESS) {
            SX_LOG_ERR("Failed to get pfc control for prio=%u\n", pfc_prio);
            return sdk_to_sai(status);
        }

        if ((SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_EN == flow_mode) ||
            ((SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_RX == (long)arg) &&
             (SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_EN == flow_mode)) ||
            ((SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_TX == (long)arg) &&
             (SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_DIS == flow_mode))) {
            pfc_ctrl_map |= (1 << pfc_prio);
        }
    }

    value->u8 = pfc_ctrl_map;

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_pfc_control_mode_set_impl(_In_ sx_port_log_id_t                      port_id,
                                                        _In_ sai_port_priority_flow_control_mode_t pfc_mode)
{
    switch (pfc_mode) {
    case SAI_PORT_PRIORITY_FLOW_CONTROL_MODE_COMBINED:
    case SAI_PORT_PRIORITY_FLOW_CONTROL_MODE_SEPARATE:
        break;

    default:
        SX_LOG_ERR("Invalid pfc control mode %d\n", pfc_mode);
        return SAI_STATUS_INVALID_ATTR_VALUE_0;
    }

    /* mode doesn't exist in SDK and not really needed for DB, attributes are always valid */

    return SAI_STATUS_SUCCESS;
}

/** Combined or separate Bit vectors for port PFC RX/TX [sai_port_priority_flow_control_mode_t] */
static sai_status_t mlnx_port_pfc_control_mode_set(_In_ const sai_object_key_t      *key,
                                                   _In_ const sai_attribute_value_t *value,
                                                   void                             *arg)
{
    sai_status_t     status;
    sx_port_log_id_t port_id;

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert port oid to logical port id\n");
        return status;
    }

    sai_db_write_lock();

    status = mlnx_port_pfc_control_mode_set_impl(port_id, value->s32);
    if (SAI_ERR(status)) {
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_pfc_control_set_impl(_In_ sx_port_log_id_t             port_id,
                                                   _In_ const sai_attribute_value_t *value,
                                                   _In_ int32_t                      arg)
{
    sx_status_t              sx_status;
    uint8_t                  pfc_prio;
    sx_port_flow_ctrl_mode_t flow_mode;

    assert((SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL == arg) || (SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_RX == arg) ||
           (SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_TX == arg));
    assert(value);

    for (pfc_prio = 0; pfc_prio < COS_IEEE_PRIO_MAX_NUM + 1; pfc_prio++) {
        /* controlling both RX and TX values */
        if (SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL == arg) {
            if (value->u8 & (1 << pfc_prio)) {
                flow_mode = SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_EN;
            } else {
                flow_mode = SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_DIS;
            }
        } else {
            sx_status = sx_api_port_pfc_enable_get(gh_sdk, port_id, pfc_prio, &flow_mode);
            if (SX_ERR(sx_status)) {
                SX_LOG_ERR("Failed to get pfc control for prio=%u\n", pfc_prio);
                return sdk_to_sai(sx_status);
            }

            /* controlling just RX value, not changing TX */
            if (SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_RX == arg) {
                if (value->u8 & (1 << pfc_prio)) {
                    if (SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_DIS == flow_mode) {
                        flow_mode = SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_EN;
                    } else if (SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_DIS == flow_mode) {
                        flow_mode = SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_EN;
                    }
                } else {
                    if (SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_EN == flow_mode) {
                        flow_mode = SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_DIS;
                    } else if (SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_EN == flow_mode) {
                        flow_mode = SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_DIS;
                    }
                }
            }

            /* controlling just TX value, not changing RX */
            if (SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_TX == arg) {
                if (value->u8 & (1 << pfc_prio)) {
                    if (SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_DIS == flow_mode) {
                        flow_mode = SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_DIS;
                    } else if (SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_EN == flow_mode) {
                        flow_mode = SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_EN;
                    }
                } else {
                    if (SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_DIS == flow_mode) {
                        flow_mode = SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_DIS;
                    } else if (SX_PORT_FLOW_CTRL_MODE_TX_EN_RX_EN == flow_mode) {
                        flow_mode = SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_EN;
                    }
                }
            }
        }

        sx_status = sx_api_port_pfc_enable_set(gh_sdk, port_id, pfc_prio, flow_mode);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to enable/disable pfc control for prio=%u\n", pfc_prio);
            return sdk_to_sai(sx_status);
        }
    }

    return SAI_STATUS_SUCCESS;
}

/** bit vector enable/disable port PFC [sai_uint8_t].
 * Valid from bit 0 to bit 7 */
static sai_status_t mlnx_port_pfc_control_set(_In_ const sai_object_key_t      *key,
                                              _In_ const sai_attribute_value_t *value,
                                              void                             *arg)
{
    sx_port_log_id_t    port_id;
    sai_status_t        status;
    mlnx_port_config_t *port;
    int32_t             pfc_type = (long)arg;

    SX_LOG_ENTER();

    assert((SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL == (long)arg) || (SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_RX == (long)arg) ||
           (SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_TX == (long)arg));

    sai_db_write_lock();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert port oid to logical port id\n");
        goto out;
    }

    status = mlnx_port_by_log_id(port_id, &port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to lookup port by log id %x\n", port_id);
        goto out;
    }
    if (mlnx_port_is_lag_member(port)) {
        port_id = mlnx_port_get_lag_id(port);
    }

    status = mlnx_port_pfc_control_set_impl(port_id, value, pfc_type);
    if (SAI_ERR(status)) {
        goto out;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

/* Get number of queues for the port */
static sai_status_t mlnx_port_queue_num_get(_In_ const sai_object_key_t   *key,
                                            _Inout_ sai_attribute_value_t *value,
                                            _In_ uint32_t                  attr_index,
                                            _Inout_ vendor_cache_t        *cache,
                                            void                          *arg)
{
    sai_status_t     status = SAI_STATUS_SUCCESS;
    sx_port_log_id_t port_id;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }
    if (mlnx_log_port_is_cpu(port_id)) {
        value->u32 = 0;
    } else {
        value->u32 = g_resource_limits.cos_port_ets_traffic_class_max + 1;
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/* Get list of queues for the port */
static sai_status_t mlnx_port_queue_list_get(_In_ const sai_object_key_t   *key,
                                             _Inout_ sai_attribute_value_t *value,
                                             _In_ uint32_t                  attr_index,
                                             _Inout_ vendor_cache_t        *cache,
                                             void                          *arg)
{
    sai_status_t     status = SAI_STATUS_SUCCESS;
    sx_port_log_id_t port_id;
    uint32_t         ii = 0;
    sai_object_id_t *port_queues = NULL;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    if (mlnx_log_port_is_cpu(port_id)) {
        value->objlist.count = 0;
    } else {
        port_queues = malloc(sizeof(sai_object_id_t) * (g_resource_limits.cos_port_ets_traffic_class_max + 1));
        if (!port_queues) {
            SX_LOG_ERR("Failed to alloc memory for port queues\n");
            return SAI_STATUS_NO_MEMORY;
        }

        for (ii = 0; ii <= g_resource_limits.cos_port_ets_traffic_class_max; ii++) {
            if (SAI_STATUS_SUCCESS != (status = mlnx_create_queue_object(port_id, ii, &port_queues[ii]))) {
                SX_LOG_ERR("Failed to create SAI object for port 0x%x TC = %u", port_id, ii);
                goto out;
            }
        }

        status = mlnx_fill_objlist(port_queues, ii, &value->objlist);
    }

out:
    SX_LOG_EXIT();
    if (port_queues) {
        free(port_queues);
    }
    return status;
}

/* Get list of pools for the port */
static sai_status_t mlnx_port_pool_list_get(_In_ const sai_object_key_t   *key,
                                            _Inout_ sai_attribute_value_t *value,
                                            _In_ uint32_t                  attr_index,
                                            _Inout_ vendor_cache_t        *cache,
                                            void                          *arg)
{
    sai_status_t     status = SAI_STATUS_SUCCESS;
    sx_port_log_id_t port_id;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        goto bail;
    }

    /* port pool is not implemented so return empty list */
    value->objlist.count = 0;

bail:
    SX_LOG_EXIT();
    return status;
}

/* Get port pg buffer capability */
static sai_status_t mlnx_port_max_headroom_size_get(_In_ const sai_object_key_t   *key,
                                                    _Inout_ sai_attribute_value_t *value,
                                                    _In_ uint32_t                  attr_index,
                                                    _Inout_ vendor_cache_t        *cache,
                                                    void                          *arg)
{
    sai_status_t              sai_status = SAI_STATUS_SUCCESS;
    sx_status_t               sx_status = SX_STATUS_SUCCESS;
    sx_cos_port_buffer_attr_t sx_port_reserved_buff_attr = {0};
    sx_port_log_id_t          port_id = -1;
    uint32_t                  count = 1;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (sai_status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        goto bail;
    }

    sx_port_reserved_buff_attr.type =
        SX_COS_PORT_BUFF_CAPABILITIES_E;
    sx_port_reserved_buff_attr.attr.port_buff_cap.ingress_port_pg_buff_cap.max_headroom_size = 0;

    if (SX_STATUS_SUCCESS !=
        (sx_status = sx_api_cos_port_buff_type_get(gh_sdk, port_id,
                                                   &sx_port_reserved_buff_attr,
                                                   &count))) {
        SX_LOG_ERR("Failed to get port pg buffer capabilities by log id: %x - %s \n",
                   port_id, SX_STATUS_MSG(sx_status));
        sai_status = sdk_to_sai(sx_status);
        goto bail;
    }

    value->u32 = mlnx_cells_to_bytes(
        sx_port_reserved_buff_attr.attr.port_buff_cap.ingress_port_pg_buff_cap.max_headroom_size);

bail:
    SX_LOG_EXIT();
    return sai_status;
}

static uint32_t sched_groups_count(mlnx_port_config_t *port)
{
    uint32_t count = 0;
    uint32_t ii;

    for (ii = 0; ii < MAX_SCHED_LEVELS; ii++) {
        count += port->sched_hierarchy.groups_count[ii];
    }

    return count;
}

/** Number of Scheduler groups on port [uint32_t]*/
static sai_status_t mlnx_port_sched_groups_num_get(_In_ const sai_object_key_t   *key,
                                                   _Inout_ sai_attribute_value_t *value,
                                                   _In_ uint32_t                  attr_index,
                                                   _Inout_ vendor_cache_t        *cache,
                                                   void                          *arg)
{
    sai_status_t        status;
    sx_port_log_id_t    port_id;
    mlnx_port_config_t *port;

    SX_LOG_ENTER();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    sai_qos_db_read_lock();

    if (mlnx_log_port_is_cpu(port_id)) {
        value->u32 = 0;
    } else {
        status = mlnx_port_by_log_id(port_id, &port);
        if (SAI_ERR(status)) {
            goto out;
        }

        if (mlnx_port_is_sai_lag_member(port)) {
            status = mlnx_port_fetch_lag_if_lag_member(&port);
            if (SAI_ERR(status)) {
                goto out;
            }
        }
        value->u32 = sched_groups_count(port);
    }

out:
    sai_qos_db_unlock();
    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/** List of Scheduler groups for the port[sai_object_list_t] */
static sai_status_t mlnx_port_sched_groups_list_get(_In_ const sai_object_key_t   *key,
                                                    _Inout_ sai_attribute_value_t *value,
                                                    _In_ uint32_t                  attr_index,
                                                    _Inout_ vendor_cache_t        *cache,
                                                    void                          *arg)
{
    sai_status_t        status = SAI_STATUS_SUCCESS;
    mlnx_port_config_t *port;
    sx_port_log_id_t    port_id;
    uint32_t            lvl, idx, count;
    sai_object_id_t    *groups = NULL;

    SX_LOG_ENTER();

    status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    sai_qos_db_read_lock();

    if (mlnx_log_port_is_cpu(port_id)) {
        value->objlist.count = 0;
    } else {
        status = mlnx_port_by_log_id(port_id, &port);
        if (SAI_ERR(status)) {
            goto out;
        }
        if (mlnx_port_is_sai_lag_member(port)) {
            status = mlnx_port_fetch_lag_if_lag_member(&port);
            if (SAI_ERR(status)) {
                goto out;
            }
        }

        count = sched_groups_count(port);

        groups = malloc(count * sizeof(sai_object_id_t));
        if (!groups) {
            SX_LOG_ERR("Failed to allocate scheduler groups list\n");
            status = SAI_STATUS_NO_MEMORY;
            goto out;
        }

        for (lvl = 0, idx = 0; lvl < MAX_SCHED_LEVELS; lvl++) {
            uint8_t count = MAX_SCHED_CHILD_GROUPS;
            uint8_t ii;

            for (ii = 0; ii < count; ii++) {
                sai_status_t status;

                if (!port->sched_hierarchy.groups[lvl][ii].is_used) {
                    continue;
                }

                status = mlnx_create_sched_group(port_id, lvl, ii, &groups[idx++]);
                if (SAI_ERR(status)) {
                    goto out;
                }
            }
        }

        status = mlnx_fill_objlist(groups, count, &value->objlist);
    }

out:
    sai_qos_db_unlock();
    SX_LOG_EXIT();
    if (groups) {
        free(groups);
    }
    return status;
}

/** Scheduler for port [sai_object_id_t], Default no limits.
 * SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_RATE & SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_BURST_RATE
 * attributes alone valid. Rest will be ignored */
static sai_status_t mlnx_port_sched_get(_In_ const sai_object_key_t   *key,
                                        _Inout_ sai_attribute_value_t *value,
                                        _In_ uint32_t                  attr_index,
                                        _Inout_ vendor_cache_t        *cache,
                                        void                          *arg)
{
    sx_port_log_id_t    port_log_id;
    sai_status_t        status;
    mlnx_port_config_t *port;

    status = mlnx_object_to_log_port(key->key.object_id, &port_log_id);
    if (status != SAI_STATUS_SUCCESS) {
        return status;
    }

    if (mlnx_log_port_is_cpu(port_log_id)) {
        value->oid = SAI_NULL_OBJECT_ID;
    } else {
        sai_qos_db_read_lock();

        status = mlnx_port_by_log_id(port_log_id, &port);
        if (status != SAI_STATUS_SUCCESS) {
            sai_qos_db_unlock();
            return status;
        }
        value->oid = port->scheduler_id;

        sai_qos_db_unlock();
    }

    return status;
}

/** Scheduler for port [sai_object_id_t], Default no limits.
 * SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_RATE & SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_BURST_RATE
 * attributes alone valid. Rest will be ignored */
static sai_status_t mlnx_port_sched_set(_In_ const sai_object_key_t      *key,
                                        _In_ const sai_attribute_value_t *value,
                                        void                             *arg)
{
    return mlnx_scheduler_to_port_apply(value->oid, key->key.object_id);
}

sai_status_t mlnx_convert_counter_types_bitmap_to_sx_bulk_read(_In_ sai_object_id_t             port_id,
                                                               _In_ uint64_t                    counter_types,
                                                               _Out_ sx_bulk_cntr_buffer_key_t *port_bulk_read_key,
                                                               _Out_ sx_bulk_cntr_buffer_key_t *flow_bulk_read_key)
{
    sai_status_t         status;
    sx_port_log_id_t     sx_log_port;
    sx_flow_counter_id_t base_counter_id;

    assert(port_bulk_read_key);
    assert(flow_bulk_read_key);

    memset(port_bulk_read_key, 0, sizeof(*port_bulk_read_key));

    status = mlnx_object_to_type(port_id, SAI_OBJECT_TYPE_PORT, &sx_log_port, NULL);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to get log port ID.\n");
        return status;
    }

    port_bulk_read_key->type = SX_BULK_CNTR_KEY_TYPE_PORT_E;
    port_bulk_read_key->key.port_key.port_list_cnt = 1;
    port_bulk_read_key->key.port_key.port_list[0] = sx_log_port;

    if (counter_types & CNT_2863) {
        port_bulk_read_key->key.port_key.grp_bitmap |= SX_BULK_CNTR_PORT_GRP_RFC_2863_E;
    }
    if (counter_types & CNT_2819) {
        port_bulk_read_key->key.port_key.grp_bitmap |= SX_BULK_CNTR_PORT_GRP_RFC_2819_E;
    }
    if (counter_types & CNT_3635) {
        port_bulk_read_key->key.port_key.grp_bitmap |= SX_BULK_CNTR_PORT_GRP_RFC_3635_E;
    }
    if (counter_types & CNT_PRIO) {
        sx_cos_ieee_prio_t prio_list[] = {
            SX_PORT_PRIO_ID_0,
            SX_PORT_PRIO_ID_1,
            SX_PORT_PRIO_ID_2,
            SX_PORT_PRIO_ID_3,
            SX_PORT_PRIO_ID_4,
            SX_PORT_PRIO_ID_5,
            SX_PORT_PRIO_ID_6,
            SX_PORT_PRIO_ID_7,
        };
        port_bulk_read_key->key.port_key.prio_id_list_cnt = sizeof(prio_list) / sizeof(*prio_list);
        memcpy(port_bulk_read_key->key.port_key.prio_id_list, prio_list, sizeof(prio_list));
        port_bulk_read_key->key.port_key.grp_bitmap |= SX_BULK_CNTR_PORT_GRP_PRIO_E;
    }
    if (counter_types & CNT_TC) {
        sx_port_tc_id_t tc_list[] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
        };
        port_bulk_read_key->key.port_key.tc_id_list_cnt = sizeof(tc_list) / sizeof(*tc_list);
        memcpy(port_bulk_read_key->key.port_key.tc_id_list, tc_list, sizeof(tc_list));
        port_bulk_read_key->key.port_key.grp_bitmap |= SX_BULK_CNTR_PORT_GRP_TC_E;
    }
    if (counter_types & CNT_BUFF) {
        sx_cos_priority_group_t pg_list[] = {
            0, 1, 2, 3, 4, 5, 6, 7
        };
        port_bulk_read_key->key.port_key.prio_group_list_cnt = sizeof(pg_list) / sizeof(*pg_list);
        memcpy(port_bulk_read_key->key.port_key.prio_group_list, pg_list, sizeof(pg_list));
        port_bulk_read_key->key.port_key.grp_bitmap |= SX_BULK_CNTR_PORT_GRP_BUFF_E;
    }
    if (counter_types & CNT_PERF) {
        port_bulk_read_key->key.port_key.grp_bitmap |= SX_BULK_CNTR_PORT_GRP_PERF_E;
    }
    if (counter_types & CNT_DISCARD) {
        port_bulk_read_key->key.port_key.grp_bitmap |= SX_BULK_CNTR_PORT_GRP_DISCARD_E;
    }
    if (counter_types & CNT_802) {
        port_bulk_read_key->key.port_key.grp_bitmap |= SX_BULK_CNTR_PORT_GRP_IEEE_802_DOT_3_E;
    }

    if (counter_types & CNT_IP) {
        if (mlnx_perport_ipcnt_is_enable()) {
            status = mlnx_perport_ipcnt_get_counter_base_id_by_port(port_id, &base_counter_id);
            if (SAI_ERR(status)) {
                SX_LOG_ERR("Failed to get per-port IP counter for port 0x%x.\n", sx_log_port);
                return status;
            }

            flow_bulk_read_key->type = SX_BULK_CNTR_KEY_TYPE_FLOW_E;
            flow_bulk_read_key->key.flow_key.base_counter_id = base_counter_id;
            flow_bulk_read_key->key.flow_key.num_of_counters = PERPORT_IPCNT_NUMBER_IN_PORT;
        } else {
            SX_LOG_ERR("The per-port IP counter is not enabled.\n");
            return SAI_STATUS_UNINITIALIZED;
        }
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_stats_bulk_read(_In_ sai_object_id_t          port_id,
                                       _In_ sx_access_cmd_t          cmd,
                                       _In_ uint64_t                 counter_types,
                                       _In_ uint32_t                 number_of_counters,
                                       _In_ const sai_stat_id_t     *counter_ids,
                                       _In_ sx_bulk_cntr_read_key_t *key_get,
                                       _In_ sx_bulk_cntr_buffer_t   *bulk_buff_port,
                                       _In_ sx_bulk_cntr_buffer_t   *bulk_buff_flow,
                                       _Out_ uint64_t               *counters)
{
    sx_status_t                              sx_status = -1;
    sx_bulk_cntr_data_t                      counter_bulk = {0};
    sx_bulk_cntr_read_key_t                  bulk_read_key_port = {0};
    sx_bulk_cntr_read_key_t                  bulk_read_key_flow = {0};
    sai_status_t                             status = SAI_STATUS_FAILURE;
    sx_port_log_id_t                         sx_log_port = 0;
    sx_port_cntr_rfc_2863_t                  cnts_2863 = {0};
    sx_port_cntr_rfc_2819_t                  cnts_2819 = {0};
    sx_port_cntr_rfc_3635_t                  cnts_3635 = {0};
    sx_port_cntr_prio_t                      cnts_prio[COS_IEEE_PRIO_MAX_NUM + 1] = {0};
    sx_port_traffic_cntr_t                   cnts_tc[17] = {0};
    sx_port_cntr_buff_t                      cnts_buff[8] = {0};
    sx_port_cntr_ieee_802_dot_3_t            cnts_802 = {0};
    sx_cos_redecn_port_counters_t            cnts_redecn = {0};
    sx_port_cntr_discard_t                   cnts_discard = {0};
    sx_port_cntr_perf_t                      cnts_perf = {0};
    sx_flow_counter_set_t                    cnts_ip[PERPORT_IPCNT_NUMBER_IN_PORT] = {0};
    const mlnx_sai_buffer_resource_limits_t *buffer_limits = mlnx_sai_get_buffer_resource_limits();
    sx_flow_counter_id_t                     base_counter_id;

    assert(counter_ids);
    assert(key_get);
    assert(bulk_buff_port);
    assert(bulk_buff_flow);
    assert(counters);

    memset(&bulk_read_key_port, 0, sizeof(bulk_read_key_port));

    status = mlnx_object_to_type(port_id, SAI_OBJECT_TYPE_PORT, &sx_log_port, NULL);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to get log port ID.\n");
        return status;
    }

    bulk_read_key_port.type = SX_BULK_CNTR_KEY_TYPE_PORT_E;
    bulk_read_key_port.key.port_key.log_port = sx_log_port;

    if (counter_types & CNT_2863) {
        bulk_read_key_port.key.port_key.grp = SX_BULK_CNTR_PORT_GRP_RFC_2863_E;
        sx_status = sx_api_bulk_counter_transaction_get(gh_sdk,
                                                        &bulk_read_key_port,
                                                        bulk_buff_port,
                                                        &counter_bulk);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to read RFC2863 counters: %s.\n", SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
        memcpy(&cnts_2863, counter_bulk.data.port_counters.port_cntr_rfc_2863_p, sizeof(cnts_2863));
    }
    if (counter_types & CNT_2819) {
        bulk_read_key_port.key.port_key.grp = SX_BULK_CNTR_PORT_GRP_RFC_2819_E;
        sx_status = sx_api_bulk_counter_transaction_get(gh_sdk,
                                                        &bulk_read_key_port,
                                                        bulk_buff_port,
                                                        &counter_bulk);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to read RFC2819 counters: %s.\n", SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
        memcpy(&cnts_2819, counter_bulk.data.port_counters.port_cntr_rfc_2819_p, sizeof(cnts_2819));
    }
    if (counter_types & CNT_3635) {
        bulk_read_key_port.key.port_key.grp = SX_BULK_CNTR_PORT_GRP_RFC_3635_E;
        sx_status = sx_api_bulk_counter_transaction_get(gh_sdk,
                                                        &bulk_read_key_port,
                                                        bulk_buff_port,
                                                        &counter_bulk);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to read RFC3635 counters: %s.\n", SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
        memcpy(&cnts_3635, counter_bulk.data.port_counters.port_cntr_rfc_3635_p, sizeof(cnts_3635));
    }
    if (counter_types & CNT_PRIO) {
        for (int ii = 0; ii < 8; ii++) {
            bulk_read_key_port.key.port_key.grp = SX_BULK_CNTR_PORT_GRP_PRIO_E;
            bulk_read_key_port.key.port_key.grp_ex_param.prio_id = SX_PORT_PRIO_ID_0 + ii;
            sx_status = sx_api_bulk_counter_transaction_get(gh_sdk,
                                                            &bulk_read_key_port,
                                                            bulk_buff_port,
                                                            &counter_bulk);
            if (SX_ERR(sx_status)) {
                SX_LOG_ERR("Failed to read PRIO[%d] counters: %s.\n", ii, SX_STATUS_MSG(sx_status));
                return sdk_to_sai(sx_status);
            }
            memcpy(&cnts_prio[ii], counter_bulk.data.port_counters.port_cntr_prio_p, sizeof(cnts_prio[ii]));
        }
    }
    if (counter_types & CNT_TC) {
        for (uint32_t ii = 0; ii < g_resource_limits.cos_port_ets_traffic_class_max; ++ii) {
            bulk_read_key_port.key.port_key.grp = SX_BULK_CNTR_PORT_GRP_TC_E;
            bulk_read_key_port.key.port_key.grp_ex_param.tc_id = ii;
            sx_status = sx_api_bulk_counter_transaction_get(gh_sdk,
                                                            &bulk_read_key_port,
                                                            bulk_buff_port,
                                                            &counter_bulk);
            if (SX_ERR(sx_status)) {
                SX_LOG_ERR("Failed to read TC[%u] counters: %s.\n", ii, SX_STATUS_MSG(sx_status));
                return sdk_to_sai(sx_status);
            }
            memcpy(&cnts_tc[ii], counter_bulk.data.port_counters.port_cntr_tc_p, sizeof(cnts_tc[ii]));
        }
    }
    if (counter_types & CNT_BUFF) {
        for (uint32_t ii = 0; ii < buffer_limits->num_port_pg_buff; ++ii) {
            bulk_read_key_port.key.port_key.grp = SX_BULK_CNTR_PORT_GRP_BUFF_E;
            bulk_read_key_port.key.port_key.grp_ex_param.prio_group = ii;
            sx_status = sx_api_bulk_counter_transaction_get(gh_sdk,
                                                            &bulk_read_key_port,
                                                            bulk_buff_port,
                                                            &counter_bulk);
            if (SX_ERR(sx_status)) {
                SX_LOG_ERR("Failed to read BUFF[%u] counters: %s.\n", ii, SX_STATUS_MSG(sx_status));
                return sdk_to_sai(sx_status);
            }
            memcpy(&cnts_buff[ii], counter_bulk.data.port_counters.port_cntr_buff_p, sizeof(cnts_buff[ii]));
        }
    }
    if (counter_types & CNT_PERF) {
        bulk_read_key_port.key.port_key.grp = SX_BULK_CNTR_PORT_GRP_PERF_E;
        sx_status = sx_api_bulk_counter_transaction_get(gh_sdk,
                                                        &bulk_read_key_port,
                                                        bulk_buff_port,
                                                        &counter_bulk);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to read PERF counters: %s.\n", SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
        memcpy(&cnts_perf, counter_bulk.data.port_counters.port_cntr_perf_p, sizeof(cnts_perf));
    }
    if (counter_types & CNT_DISCARD) {
        bulk_read_key_port.key.port_key.grp = SX_BULK_CNTR_PORT_GRP_DISCARD_E;
        sx_status = sx_api_bulk_counter_transaction_get(gh_sdk,
                                                        &bulk_read_key_port,
                                                        bulk_buff_port,
                                                        &counter_bulk);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to read DISCARD counters: %s.\n", SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
        memcpy(&cnts_discard, counter_bulk.data.port_counters.port_cntr_discard_p, sizeof(cnts_discard));
    }
    if (counter_types & CNT_802) {
        bulk_read_key_port.key.port_key.grp = SX_BULK_CNTR_PORT_GRP_IEEE_802_DOT_3_E;
        sx_status = sx_api_bulk_counter_transaction_get(gh_sdk,
                                                        &bulk_read_key_port,
                                                        bulk_buff_port,
                                                        &counter_bulk);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to read IEEE802 counters: %s.\n", SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
        memcpy(&cnts_802, counter_bulk.data.port_counters.port_cntr_ieee_802_p, sizeof(cnts_802));
    }
    if (counter_types & CNT_REDECN) {
        sx_port_log_id_t    red_port_id;
        mlnx_port_config_t *port;

        /* In case if port is LAG member then use LAG logical id for redecn counters */
        sai_db_read_lock();
        status = mlnx_port_by_log_id(sx_log_port, &port);
        if (SAI_ERR(status)) {
            sai_db_unlock();
            return status;
        }
        if (mlnx_port_is_lag_member(port)) {
            red_port_id = mlnx_port_get_lag_id(port);
        } else {
            red_port_id = sx_log_port;
        }
        sai_db_unlock();

        if (SX_STATUS_SUCCESS !=
            (status = sx_api_cos_redecn_counters_get(gh_sdk, cmd, red_port_id, &cnts_redecn))) {
            SX_LOG_ERR("Failed to get port redecn counters - %s.\n", SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    }
    if (counter_types & CNT_IP) {
        status = mlnx_perport_ipcnt_get_counter_base_id_by_port(port_id, &base_counter_id);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to get per-port IP counter for port 0x%x.\n", sx_log_port);
            return status;
        }
        bulk_read_key_flow.type = SX_BULK_CNTR_KEY_TYPE_FLOW_E;
        bulk_read_key_flow.key.flow_key.cntr_id = base_counter_id;

        sx_status = sx_api_bulk_counter_transaction_get(gh_sdk,
                                                        &bulk_read_key_flow,
                                                        bulk_buff_flow,
                                                        &counter_bulk);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to read IP counters: %s.\n", SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
        memcpy(cnts_ip, counter_bulk.data.flow_counters.flow_cntr_p, sizeof(cnts_ip));
    }

    for (uint32_t ii = 0; ii < number_of_counters; ii++) {
        switch ((int)counter_ids[ii]) {
        case SAI_PORT_STAT_IF_IN_OCTETS:
            counters[ii] = cnts_2863.if_in_octets;
            break;

        case SAI_PORT_STAT_IF_IN_UCAST_PKTS:
            counters[ii] = cnts_2863.if_in_ucast_pkts;
            break;

        case SAI_PORT_STAT_IF_IN_NON_UCAST_PKTS:
            counters[ii] = cnts_2863.if_in_broadcast_pkts + cnts_2863.if_in_multicast_pkts;
            break;

        case SAI_PORT_STAT_IF_IN_DISCARDS:
            counters[ii] = cnts_2863.if_in_discards;
            if (g_sai_db_ptr->aggregate_bridge_drops) {
                counters[ii] += cnts_discard.ingress_general + cnts_discard.ingress_policy_engine +
                                cnts_discard.ingress_vlan_membership +
                                cnts_discard.ingress_tag_frame_type + cnts_discard.ingress_tx_link_down;
            }
            break;

        case SAI_PORT_STAT_IF_IN_ERRORS:
            counters[ii] = cnts_2863.if_in_errors;
            break;

        case SAI_PORT_STAT_IF_IN_UNKNOWN_PROTOS:
            counters[ii] = cnts_2863.if_in_unknown_protos;
            break;

        case SAI_PORT_STAT_IF_IN_BROADCAST_PKTS:
            counters[ii] = cnts_2863.if_in_broadcast_pkts;
            break;

        case SAI_PORT_STAT_IF_IN_MULTICAST_PKTS:
            counters[ii] = cnts_2863.if_in_multicast_pkts;
            break;

        case SAI_PORT_STAT_IF_OUT_OCTETS:
            counters[ii] = cnts_2863.if_out_octets;
            break;

        case SAI_PORT_STAT_IF_OUT_UCAST_PKTS:
            counters[ii] = cnts_2863.if_out_ucast_pkts;
            break;

        case SAI_PORT_STAT_IF_OUT_NON_UCAST_PKTS:
            counters[ii] = cnts_2863.if_out_broadcast_pkts + cnts_2863.if_out_multicast_pkts;
            break;

        case SAI_PORT_STAT_IF_OUT_DISCARDS:
            counters[ii] = cnts_2863.if_out_discards;
            break;

        case SAI_PORT_STAT_IF_OUT_ERRORS:
            counters[ii] = cnts_2863.if_out_errors;
            break;

        case SAI_PORT_STAT_IF_OUT_BROADCAST_PKTS:
            counters[ii] = cnts_2863.if_out_broadcast_pkts;
            break;

        case SAI_PORT_STAT_IF_OUT_MULTICAST_PKTS:
            counters[ii] = cnts_2863.if_out_multicast_pkts;
            break;

        case SAI_PORT_STAT_ETHER_STATS_DROP_EVENTS:
            counters[ii] = cnts_2819.ether_stats_drop_events;
            break;

        case SAI_PORT_STAT_ETHER_STATS_MULTICAST_PKTS:
            counters[ii] = cnts_2819.ether_stats_multicast_pkts;
            break;

        case SAI_PORT_STAT_ETHER_STATS_BROADCAST_PKTS:
            counters[ii] = cnts_2819.ether_stats_broadcast_pkts;
            break;

        case SAI_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS:
            counters[ii] = cnts_2819.ether_stats_undersize_pkts;
            break;

        case SAI_PORT_STAT_ETHER_STATS_FRAGMENTS:
            counters[ii] = cnts_2819.ether_stats_fragments;
            break;

        case SAI_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_64_OCTETS:
            counters[ii] = cnts_2819.ether_stats_pkts64octets;
            break;

        case SAI_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS:
            counters[ii] = cnts_2819.ether_stats_pkts65to127octets;
            break;

        case SAI_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS:
            counters[ii] = cnts_2819.ether_stats_pkts128to255octets;
            break;

        case SAI_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS:
            counters[ii] = cnts_2819.ether_stats_pkts256to511octets;
            break;

        case SAI_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS:
            counters[ii] = cnts_2819.ether_stats_pkts512to1023octets;
            break;

        case SAI_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1518_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_1024_TO_1518_OCTETS:
            counters[ii] = cnts_2819.ether_stats_pkts1024to1518octets;
            break;

        case SAI_PORT_STAT_ETHER_STATS_PKTS_1519_TO_2047_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_1519_TO_2047_OCTETS:
            counters[ii] = cnts_2819.ether_stats_pkts1519to2047octets;
            break;

        case SAI_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS:
            counters[ii] = cnts_2819.ether_stats_pkts2048to4095octets;
            break;

        case SAI_PORT_STAT_ETHER_STATS_PKTS_4096_TO_9216_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_4096_TO_9216_OCTETS:
            counters[ii] = cnts_2819.ether_stats_pkts4096to8191octets;
            break;

        /* by definition, standard 2819 oversize is RX only. for tx, we drop all oversize, so always 0 */
        case SAI_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS:
        case SAI_PORT_STAT_ETHER_RX_OVERSIZE_PKTS:
            counters[ii] = cnts_2819.ether_stats_oversize_pkts;
            break;

        case SAI_PORT_STAT_ETHER_TX_OVERSIZE_PKTS:
            counters[ii] = 0;
            break;

        case SAI_PORT_STAT_ETHER_STATS_JABBERS:
            counters[ii] = cnts_2819.ether_stats_jabbers;
            break;

        case SAI_PORT_STAT_ETHER_STATS_OCTETS:
            counters[ii] = cnts_2819.ether_stats_octets;
            break;

        case SAI_PORT_STAT_ETHER_STATS_PKTS:
            counters[ii] = cnts_2819.ether_stats_pkts;
            break;

        case SAI_PORT_STAT_ETHER_STATS_COLLISIONS:
            counters[ii] = cnts_2819.ether_stats_collisions;
            break;

        case SAI_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS:
            counters[ii] = cnts_2819.ether_stats_crc_align_errors;
            break;

        case SAI_PORT_STAT_ETHER_STATS_TX_NO_ERRORS:
            counters[ii] = cnts_802.a_frames_transmitted_ok;
            break;

        case SAI_PORT_STAT_ETHER_STATS_RX_NO_ERRORS:
            counters[ii] = cnts_802.a_frames_received_ok;
            break;

        case SAI_PORT_STAT_PAUSE_RX_PKTS:
            counters[ii] = cnts_802.a_pause_mac_ctrl_frames_received;
            break;

        case SAI_PORT_STAT_PAUSE_TX_PKTS:
            counters[ii] = cnts_802.a_pause_mac_ctrl_frames_transmitted;
            break;

        case SAI_PORT_STAT_GREEN_WRED_DROPPED_PACKETS:
        case SAI_PORT_STAT_GREEN_WRED_DROPPED_BYTES:
        case SAI_PORT_STAT_YELLOW_WRED_DROPPED_PACKETS:
        case SAI_PORT_STAT_YELLOW_WRED_DROPPED_BYTES:
        case SAI_PORT_STAT_RED_WRED_DROPPED_PACKETS:
        case SAI_PORT_STAT_RED_WRED_DROPPED_BYTES:
        case SAI_PORT_STAT_WRED_DROPPED_BYTES:
            SX_LOG_INF("Port counter %d set item %u not supported\n", counter_ids[ii], ii);
            return SAI_STATUS_ATTR_NOT_SUPPORTED_0;

        case SAI_PORT_STAT_WRED_DROPPED_PACKETS:
            counters[ii] = 0;
            for (uint64_t jj = 0; jj < g_resource_limits.cos_port_ets_traffic_class_max + 1; jj++) {
                counters[ii] += cnts_redecn.tc_red_dropped_packets[jj];
            }
            break;

        case SAI_PORT_STAT_ECN_MARKED_PACKETS:
            counters[ii] = cnts_redecn.ecn_marked_packets;
            break;

        case SAI_PORT_STAT_PFC_0_RX_PKTS:
        case SAI_PORT_STAT_PFC_1_RX_PKTS:
        case SAI_PORT_STAT_PFC_2_RX_PKTS:
        case SAI_PORT_STAT_PFC_3_RX_PKTS:
        case SAI_PORT_STAT_PFC_4_RX_PKTS:
        case SAI_PORT_STAT_PFC_5_RX_PKTS:
        case SAI_PORT_STAT_PFC_6_RX_PKTS:
        case SAI_PORT_STAT_PFC_7_RX_PKTS:
            counters[ii] = cnts_prio[(counter_ids[ii] - SAI_PORT_STAT_PFC_0_RX_PKTS) / 2].rx_pause;
            break;

        case SAI_PORT_STAT_PFC_0_TX_PKTS:
        case SAI_PORT_STAT_PFC_1_TX_PKTS:
        case SAI_PORT_STAT_PFC_2_TX_PKTS:
        case SAI_PORT_STAT_PFC_3_TX_PKTS:
        case SAI_PORT_STAT_PFC_4_TX_PKTS:
        case SAI_PORT_STAT_PFC_5_TX_PKTS:
        case SAI_PORT_STAT_PFC_6_TX_PKTS:
        case SAI_PORT_STAT_PFC_7_TX_PKTS:
            counters[ii] = cnts_prio[(counter_ids[ii] - SAI_PORT_STAT_PFC_0_TX_PKTS) / 2].tx_pause;
            break;

        case SAI_PORT_STAT_PFC_0_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_1_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_2_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_3_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_4_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_5_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_6_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_7_RX_PAUSE_DURATION_US:
            counters[ii] =
                cnts_prio[(counter_ids[ii] - SAI_PORT_STAT_PFC_0_RX_PAUSE_DURATION_US) / 2].rx_pause_duration;
            break;

        case SAI_PORT_STAT_PFC_0_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_1_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_2_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_3_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_4_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_5_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_6_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_7_TX_PAUSE_DURATION_US:
            counters[ii] =
                cnts_prio[(counter_ids[ii] - SAI_PORT_STAT_PFC_0_TX_PAUSE_DURATION_US) / 2].tx_pause_duration;
            break;

        case SAI_PORT_STAT_IF_IN_VLAN_DISCARDS:
            counters[ii] = cnts_discard.ingress_vlan_membership;
            break;

        case SAI_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS:
            counters[ii] = cnts_perf.tx_stats_pkts64octets;
            break;

        case SAI_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS:
            counters[ii] = cnts_perf.tx_stats_pkts65to127octets;
            break;

        case SAI_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS:
            counters[ii] = cnts_perf.tx_stats_pkts128to255octets;
            break;

        case SAI_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS:
            counters[ii] = cnts_perf.tx_stats_pkts256to511octets;
            break;

        case SAI_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS:
            counters[ii] = cnts_perf.tx_stats_pkts512to1023octets;
            break;

        case SAI_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1518_OCTETS:
            counters[ii] = cnts_perf.tx_stats_pkts1024to1518octets;
            break;

        case SAI_PORT_STAT_ETHER_OUT_PKTS_1519_TO_2047_OCTETS:
            counters[ii] = cnts_perf.tx_stats_pkts1519to2047octets;
            break;

        case SAI_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS:
            counters[ii] = cnts_perf.tx_stats_pkts2048to4095octets;
            break;

        case SAI_PORT_STAT_ETHER_OUT_PKTS_4096_TO_9216_OCTETS:
            counters[ii] = cnts_perf.tx_stats_pkts4096to8191octets;
            break;

        case SAI_PORT_STAT_DOT3_STATS_ALIGNMENT_ERRORS:
            counters[ii] = cnts_3635.dot3stats_alignment_errors;
            break;

        case SAI_PORT_STAT_DOT3_STATS_FCS_ERRORS:
            counters[ii] = cnts_3635.dot3stats_fcs_errors;
            break;

        case SAI_PORT_STAT_DOT3_STATS_SINGLE_COLLISION_FRAMES:
            counters[ii] = cnts_3635.dot3stats_single_collision_frames;
            break;

        case SAI_PORT_STAT_DOT3_STATS_MULTIPLE_COLLISION_FRAMES:
            counters[ii] = cnts_3635.dot3stats_multiple_collision_frames;
            break;

        case SAI_PORT_STAT_DOT3_STATS_SQE_TEST_ERRORS:
            counters[ii] = cnts_3635.dot3stats_sqe_test_errors;
            break;

        case SAI_PORT_STAT_DOT3_STATS_DEFERRED_TRANSMISSIONS:
            counters[ii] = cnts_3635.dot3stats_deferred_transmissions;
            break;

        case SAI_PORT_STAT_DOT3_STATS_LATE_COLLISIONS:
            counters[ii] = cnts_3635.dot3stats_late_collisions;
            break;

        case SAI_PORT_STAT_DOT3_STATS_EXCESSIVE_COLLISIONS:
            counters[ii] = cnts_3635.dot3stats_excessive_collisions;
            break;

        case SAI_PORT_STAT_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS:
            counters[ii] = cnts_3635.dot3stats_internal_mac_transmit_errors;
            break;

        case SAI_PORT_STAT_DOT3_STATS_CARRIER_SENSE_ERRORS:
            counters[ii] = cnts_3635.dot3stats_carrier_sense_errors;
            break;

        case SAI_PORT_STAT_DOT3_STATS_FRAME_TOO_LONGS:
            counters[ii] = cnts_3635.dot3stats_frame_too_longs;
            break;

        case SAI_PORT_STAT_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS:
            counters[ii] = cnts_3635.dot3stats_internal_mac_receive_errors;
            break;

        case SAI_PORT_STAT_DOT3_STATS_SYMBOL_ERRORS:
            counters[ii] = cnts_3635.dot3stats_symbol_errors;
            break;

        case SAI_PORT_STAT_DOT3_CONTROL_IN_UNKNOWN_OPCODES:
            counters[ii] = cnts_3635.dot3control_in_unknown_opcodes;
            break;

        case SAI_PORT_STAT_IN_DROPPED_PKTS:
            counters[ii] = 0;
            for (uint32_t jj = 0; jj < buffer_limits->num_port_pg_buff; jj++) {
                counters[ii] += cnts_buff[jj].rx_buffer_discard + cnts_buff[jj].rx_shared_buffer_discard;
            }
            break;

        case SAI_PORT_STAT_OUT_DROPPED_PKTS:
            counters[ii] = 0;
            for (uint32_t jj = 0; jj < g_resource_limits.cos_port_ets_traffic_class_max; jj++) {
                counters[ii] += cnts_tc[jj].tx_no_buffer_discard_uc;
            }
            break;

        case SAI_PORT_STAT_PFC_0_RX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_1_RX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_2_RX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_3_RX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_4_RX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_5_RX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_6_RX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_7_RX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_0_TX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_1_TX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_2_TX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_3_TX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_4_TX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_5_TX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_6_TX_PAUSE_DURATION:
        case SAI_PORT_STAT_PFC_7_TX_PAUSE_DURATION:
        case SAI_PORT_STAT_IF_OUT_QLEN:
        case SAI_PORT_STAT_ETHER_STATS_PKTS_9217_TO_16383_OCTETS:
        case SAI_PORT_STAT_IP_IN_DISCARDS:
        case SAI_PORT_STAT_IP_OUT_DISCARDS:
        case SAI_PORT_STAT_IPV6_IN_DISCARDS:
        case SAI_PORT_STAT_IPV6_OUT_DISCARDS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_9217_TO_16383_OCTETS:
        case SAI_PORT_STAT_ETHER_OUT_PKTS_9217_TO_16383_OCTETS:
        case SAI_PORT_STAT_IN_CURR_OCCUPANCY_BYTES:
        case SAI_PORT_STAT_IN_WATERMARK_BYTES:
        case SAI_PORT_STAT_IN_SHARED_CURR_OCCUPANCY_BYTES:
        case SAI_PORT_STAT_IN_SHARED_WATERMARK_BYTES:
        case SAI_PORT_STAT_OUT_CURR_OCCUPANCY_BYTES:
        case SAI_PORT_STAT_OUT_WATERMARK_BYTES:
        case SAI_PORT_STAT_OUT_SHARED_CURR_OCCUPANCY_BYTES:
        case SAI_PORT_STAT_OUT_SHARED_WATERMARK_BYTES:
        case SAI_PORT_STAT_PFC_0_ON2OFF_RX_PKTS:
        case SAI_PORT_STAT_PFC_1_ON2OFF_RX_PKTS:
        case SAI_PORT_STAT_PFC_2_ON2OFF_RX_PKTS:
        case SAI_PORT_STAT_PFC_3_ON2OFF_RX_PKTS:
        case SAI_PORT_STAT_PFC_4_ON2OFF_RX_PKTS:
        case SAI_PORT_STAT_PFC_5_ON2OFF_RX_PKTS:
        case SAI_PORT_STAT_PFC_6_ON2OFF_RX_PKTS:
        case SAI_PORT_STAT_PFC_7_ON2OFF_RX_PKTS:
        case SAI_PORT_STAT_EEE_TX_EVENT_COUNT:
        case SAI_PORT_STAT_EEE_RX_EVENT_COUNT:
        case SAI_PORT_STAT_EEE_TX_DURATION:
        case SAI_PORT_STAT_EEE_RX_DURATION:
            SX_LOG_INF("Port counter %d set item %u not implemented\n", counter_ids[ii], ii);
            return SAI_STATUS_NOT_IMPLEMENTED;

        case SAI_PORT_STAT_IP_IN_RECEIVES:
            counters[ii] = cnts_ip[PERPORT_IPCNT_IN_IP_UCAST].flow_counter_packets +
                           cnts_ip[PERPORT_IPCNT_IN_IP_NON_UCAST].flow_counter_packets;
            break;

        case SAI_PORT_STAT_IP_IN_OCTETS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_IN_IP_UCAST].flow_counter_bytes +
                           cnts_ip[PERPORT_IPCNT_IN_IP_NON_UCAST].flow_counter_bytes;
            break;

        case SAI_PORT_STAT_IP_IN_UCAST_PKTS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_IN_IP_UCAST].flow_counter_packets;
            break;

        case SAI_PORT_STAT_IP_IN_NON_UCAST_PKTS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_IN_IP_NON_UCAST].flow_counter_packets;
            break;

        case SAI_PORT_STAT_IP_OUT_OCTETS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_OUT_IP_UCAST].flow_counter_bytes +
                           cnts_ip[PERPORT_IPCNT_OUT_IP_NON_UCAST].flow_counter_bytes;
            break;

        case SAI_PORT_STAT_IP_OUT_UCAST_PKTS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_OUT_IP_UCAST].flow_counter_packets;
            break;

        case SAI_PORT_STAT_IP_OUT_NON_UCAST_PKTS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_OUT_IP_NON_UCAST].flow_counter_packets;
            break;

        case SAI_PORT_STAT_IPV6_IN_RECEIVES:
            counters[ii] = cnts_ip[PERPORT_IPCNT_IN_IP6_UCAST].flow_counter_packets +
                           cnts_ip[PERPORT_IPCNT_IN_IP6_NON_UCAST].flow_counter_packets;
            break;

        case SAI_PORT_STAT_IPV6_IN_OCTETS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_IN_IP6_UCAST].flow_counter_bytes +
                           cnts_ip[PERPORT_IPCNT_IN_IP6_NON_UCAST].flow_counter_bytes;
            break;

        case SAI_PORT_STAT_IPV6_IN_UCAST_PKTS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_IN_IP6_UCAST].flow_counter_packets;
            break;

        case SAI_PORT_STAT_IPV6_IN_NON_UCAST_PKTS:
        case SAI_PORT_STAT_IPV6_IN_MCAST_PKTS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_IN_IP6_NON_UCAST].flow_counter_packets;
            break;

        case SAI_PORT_STAT_IPV6_OUT_OCTETS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_OUT_IP6_UCAST].flow_counter_bytes +
                           cnts_ip[PERPORT_IPCNT_OUT_IP6_NON_UCAST].flow_counter_bytes;
            break;

        case SAI_PORT_STAT_IPV6_OUT_UCAST_PKTS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_OUT_IP6_UCAST].flow_counter_packets;
            break;

        case SAI_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS:
        case SAI_PORT_STAT_IPV6_OUT_MCAST_PKTS:
            counters[ii] = cnts_ip[PERPORT_IPCNT_OUT_IP6_NON_UCAST].flow_counter_packets;
            break;

        default:
            SX_LOG_ERR("Invalid port counter %d\n", counter_ids[ii]);
            return SAI_STATUS_INVALID_PARAMETER;
        }
    }

    return SAI_STATUS_SUCCESS;
}

static void mlnx_is_bulk_required(_In_ uint64_t counter_types,
                                  _Out_ bool   *bulk_required_port,
                                  _Out_ bool   *bulk_required_flow)
{
    *bulk_required_port = (counter_types & CNT_802) ||
                          (counter_types & CNT_2819) ||
                          (counter_types & CNT_2863) ||
                          (counter_types & CNT_3635) ||
                          (counter_types & CNT_BUFF) ||
                          (counter_types & CNT_DISCARD) ||
                          (counter_types & CNT_PERF) ||
                          (counter_types & CNT_PRIO) ||
                          (counter_types & CNT_TC);
    *bulk_required_flow = (counter_types & CNT_IP);
}

static sai_status_t mlnx_convert_sai_stats_to_bitmap(_In_ uint32_t             number_of_counters,
                                                     _In_ const sai_stat_id_t *counter_ids,
                                                     _Out_ uint64_t           *counter_types)
{
    sai_status_t status = SAI_STATUS_SUCCESS;

    *counter_types = 0;

    assert(counter_types);

    if ((number_of_counters > 0) && !counter_ids) {
        return SAI_STATUS_INVALID_PARAMETER;
    }

    for (uint32_t ii = 0; ii < number_of_counters; ii++) {
        switch ((int)counter_ids[ii]) {
        case SAI_PORT_STAT_IF_IN_OCTETS:
        case SAI_PORT_STAT_IF_IN_UCAST_PKTS:
        case SAI_PORT_STAT_IF_IN_NON_UCAST_PKTS:
        case SAI_PORT_STAT_IF_IN_ERRORS:
        case SAI_PORT_STAT_IF_IN_UNKNOWN_PROTOS:
        case SAI_PORT_STAT_IF_IN_BROADCAST_PKTS:
        case SAI_PORT_STAT_IF_IN_MULTICAST_PKTS:
        case SAI_PORT_STAT_IF_OUT_OCTETS:
        case SAI_PORT_STAT_IF_OUT_UCAST_PKTS:
        case SAI_PORT_STAT_IF_OUT_NON_UCAST_PKTS:
        case SAI_PORT_STAT_IF_OUT_DISCARDS:
        case SAI_PORT_STAT_IF_OUT_ERRORS:
        case SAI_PORT_STAT_IF_OUT_BROADCAST_PKTS:
        case SAI_PORT_STAT_IF_OUT_MULTICAST_PKTS:
            *counter_types |= CNT_2863;
            break;

        case SAI_PORT_STAT_IF_IN_DISCARDS:
            if (g_sai_db_ptr->aggregate_bridge_drops) {
                *counter_types |= CNT_DISCARD;
            }
            *counter_types |= CNT_2863;
            break;

        case SAI_PORT_STAT_ETHER_STATS_DROP_EVENTS:
        case SAI_PORT_STAT_ETHER_STATS_MULTICAST_PKTS:
        case SAI_PORT_STAT_ETHER_STATS_BROADCAST_PKTS:
        case SAI_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS:
        case SAI_PORT_STAT_ETHER_STATS_FRAGMENTS:
        case SAI_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_64_OCTETS:
        case SAI_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS:
        case SAI_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS:
        case SAI_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS:
        case SAI_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS:
        case SAI_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1518_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_1024_TO_1518_OCTETS:
        case SAI_PORT_STAT_ETHER_STATS_PKTS_1519_TO_2047_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_1519_TO_2047_OCTETS:
        case SAI_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS:
        case SAI_PORT_STAT_ETHER_STATS_PKTS_4096_TO_9216_OCTETS:
        case SAI_PORT_STAT_ETHER_IN_PKTS_4096_TO_9216_OCTETS:
        case SAI_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS:
        case SAI_PORT_STAT_ETHER_RX_OVERSIZE_PKTS:
        case SAI_PORT_STAT_ETHER_STATS_JABBERS:
        case SAI_PORT_STAT_ETHER_STATS_OCTETS:
        case SAI_PORT_STAT_ETHER_STATS_PKTS:
        case SAI_PORT_STAT_ETHER_STATS_COLLISIONS:
        case SAI_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS:
            *counter_types |= CNT_2819;
            break;

        case SAI_PORT_STAT_ETHER_STATS_TX_NO_ERRORS:
        case SAI_PORT_STAT_ETHER_STATS_RX_NO_ERRORS:
        case SAI_PORT_STAT_PAUSE_RX_PKTS:
        case SAI_PORT_STAT_PAUSE_TX_PKTS:
            *counter_types |= CNT_802;
            break;

        case SAI_PORT_STAT_WRED_DROPPED_PACKETS:
        case SAI_PORT_STAT_ECN_MARKED_PACKETS:
            *counter_types |= CNT_REDECN;
            break;

        case SAI_PORT_STAT_PFC_0_RX_PKTS:
        case SAI_PORT_STAT_PFC_1_RX_PKTS:
        case SAI_PORT_STAT_PFC_2_RX_PKTS:
        case SAI_PORT_STAT_PFC_3_RX_PKTS:
        case SAI_PORT_STAT_PFC_4_RX_PKTS:
        case SAI_PORT_STAT_PFC_5_RX_PKTS:
        case SAI_PORT_STAT_PFC_6_RX_PKTS:
        case SAI_PORT_STAT_PFC_7_RX_PKTS:
            *counter_types |= CNT_PRIO;
            break;

        case SAI_PORT_STAT_PFC_0_TX_PKTS:
        case SAI_PORT_STAT_PFC_1_TX_PKTS:
        case SAI_PORT_STAT_PFC_2_TX_PKTS:
        case SAI_PORT_STAT_PFC_3_TX_PKTS:
        case SAI_PORT_STAT_PFC_4_TX_PKTS:
        case SAI_PORT_STAT_PFC_5_TX_PKTS:
        case SAI_PORT_STAT_PFC_6_TX_PKTS:
        case SAI_PORT_STAT_PFC_7_TX_PKTS:
            *counter_types |= CNT_PRIO;
            break;

        case SAI_PORT_STAT_PFC_0_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_1_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_2_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_3_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_4_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_5_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_6_RX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_7_RX_PAUSE_DURATION_US:
            *counter_types |= CNT_PRIO;
            break;

        case SAI_PORT_STAT_PFC_0_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_1_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_2_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_3_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_4_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_5_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_6_TX_PAUSE_DURATION_US:
        case SAI_PORT_STAT_PFC_7_TX_PAUSE_DURATION_US:
            *counter_types |= CNT_PRIO;
            break;

        case SAI_PORT_STAT_IF_IN_VLAN_DISCARDS:
            *counter_types |= CNT_DISCARD;
            break;

        case SAI_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS:
        case SAI_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS:
        case SAI_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS:
        case SAI_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS:
        case SAI_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS:
        case SAI_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1518_OCTETS:
        case SAI_PORT_STAT_ETHER_OUT_PKTS_1519_TO_2047_OCTETS:
        case SAI_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS:
        case SAI_PORT_STAT_ETHER_OUT_PKTS_4096_TO_9216_OCTETS:
            *counter_types |= CNT_PERF;
            break;

        case SAI_PORT_STAT_DOT3_STATS_ALIGNMENT_ERRORS:
        case SAI_PORT_STAT_DOT3_STATS_FCS_ERRORS:
        case SAI_PORT_STAT_DOT3_STATS_SINGLE_COLLISION_FRAMES:
        case SAI_PORT_STAT_DOT3_STATS_MULTIPLE_COLLISION_FRAMES:
        case SAI_PORT_STAT_DOT3_STATS_SQE_TEST_ERRORS:
        case SAI_PORT_STAT_DOT3_STATS_DEFERRED_TRANSMISSIONS:
        case SAI_PORT_STAT_DOT3_STATS_LATE_COLLISIONS:
        case SAI_PORT_STAT_DOT3_STATS_EXCESSIVE_COLLISIONS:
        case SAI_PORT_STAT_DOT3_STATS_INTERNAL_MAC_TRANSMIT_ERRORS:
        case SAI_PORT_STAT_DOT3_STATS_CARRIER_SENSE_ERRORS:
        case SAI_PORT_STAT_DOT3_STATS_FRAME_TOO_LONGS:
        case SAI_PORT_STAT_DOT3_STATS_INTERNAL_MAC_RECEIVE_ERRORS:
        case SAI_PORT_STAT_DOT3_STATS_SYMBOL_ERRORS:
        case SAI_PORT_STAT_DOT3_CONTROL_IN_UNKNOWN_OPCODES:
            *counter_types |= CNT_3635;
            break;

        case SAI_PORT_STAT_IN_DROPPED_PKTS:
            *counter_types |= CNT_BUFF;
            break;

        case SAI_PORT_STAT_OUT_DROPPED_PKTS:
            *counter_types |= CNT_TC;
            break;

        case SAI_PORT_STAT_ETHER_TX_OVERSIZE_PKTS:
            break;

        case SAI_PORT_STAT_IP_IN_RECEIVES:
        case SAI_PORT_STAT_IP_IN_OCTETS:
        case SAI_PORT_STAT_IP_IN_UCAST_PKTS:
        case SAI_PORT_STAT_IP_IN_NON_UCAST_PKTS:
        case SAI_PORT_STAT_IP_OUT_OCTETS:
        case SAI_PORT_STAT_IP_OUT_UCAST_PKTS:
        case SAI_PORT_STAT_IP_OUT_NON_UCAST_PKTS:
        case SAI_PORT_STAT_IPV6_IN_RECEIVES:
        case SAI_PORT_STAT_IPV6_IN_OCTETS:
        case SAI_PORT_STAT_IPV6_IN_UCAST_PKTS:
        case SAI_PORT_STAT_IPV6_IN_NON_UCAST_PKTS:
        case SAI_PORT_STAT_IPV6_IN_MCAST_PKTS:
        case SAI_PORT_STAT_IPV6_OUT_OCTETS:
        case SAI_PORT_STAT_IPV6_OUT_UCAST_PKTS:
        case SAI_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS:
        case SAI_PORT_STAT_IPV6_OUT_MCAST_PKTS:
            *counter_types |= CNT_IP;
            break;

        default:
            status = SAI_STATUS_INVALID_PARAMETER;
            SX_LOG_INF("Port counter %d set item %u not implemented\n", counter_ids[ii], ii);
            break;
        }
    }

    return status;
}

/*
 * Routine Description:
 *   Set port attribute value.
 *
 * Arguments:
 *    [in] port_id - port id
 *    [in] attr - attribute
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_set_port_attribute(_In_ sai_object_id_t port_id, _In_ const sai_attribute_t *attr)
{
    const sai_object_key_t key = { .key.object_id = port_id };

    return sai_set_attribute(&key, SAI_OBJECT_TYPE_PORT, attr);
}


/*
 * Routine Description:
 *   Get port attribute value.
 *
 * Arguments:
 *    [in] port_id - port id
 *    [in] attr_count - number of attributes
 *    [inout] attr_list - array of attributes
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_get_port_attribute(_In_ sai_object_id_t     port_id,
                                            _In_ uint32_t            attr_count,
                                            _Inout_ sai_attribute_t *attr_list)
{
    const sai_object_key_t key = { .key.object_id = port_id };

    return sai_get_attributes(&key, SAI_OBJECT_TYPE_PORT, attr_count, attr_list);
}

/**
 * @brief Get port statistics counters extended.
 *
 * @param[in] port_id Port id
 * @param[in] number_of_counters Number of counters in the array
 * @param[in] counter_ids Specifies the array of counter ids
 * @param[in] mode Statistics mode
 * @param[out] counters Array of resulting counter values.
 *
 * @return #SAI_STATUS_SUCCESS on success, failure status code on error
 */
sai_status_t mlnx_get_port_stats_ext(_In_ sai_object_id_t      port_id,
                                     _In_ uint32_t             number_of_counters,
                                     _In_ const sai_stat_id_t *counter_ids,
                                     _In_ sai_stats_mode_t     mode,
                                     _Out_ uint64_t           *counters)
{
    sai_status_t              status = SAI_STATUS_FAILURE;
    sx_bulk_cntr_buffer_key_t bulk_read_key = {0}, bulk_read_key_flow = {0};
    sx_bulk_cntr_buffer_t     bulk_read_buff = {0}, bulk_read_buff_flow = {0};
    sx_bulk_cntr_read_key_t   key_get = {0};
    uint64_t                  counter_types = 0;
    sx_access_cmd_t           cmd;
    char                      key_str[MAX_KEY_STR_LEN];
    bool                      bulk_required_port = false, bulk_required_flow = false;

    SX_LOG_ENTER();

    oid_to_str(port_id, key_str);
    SX_LOG_DBG("Get stats %s\n", key_str);

    if (NULL == counter_ids) {
        SX_LOG_ERR("NULL counter ids array param.\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (NULL == counters) {
        SX_LOG_ERR("NULL counters array param.\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    status = mlnx_translate_sai_stats_mode_to_sdk(mode, &cmd);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert mode.\n");
        return status;
    }

    status = mlnx_convert_sai_stats_to_bitmap(number_of_counters,
                                              counter_ids,
                                              &counter_types);
    if (SAI_ERR(status)) {
        SX_LOG_INF("Failed to convert counter IDs to counter types bitmap.\n");
        return status;
    }

    mlnx_is_bulk_required(counter_types, &bulk_required_port, &bulk_required_flow);

    if (bulk_required_port || bulk_required_flow) {
        status = mlnx_convert_counter_types_bitmap_to_sx_bulk_read(port_id,
                                                                   counter_types,
                                                                   &bulk_read_key,
                                                                   &bulk_read_key_flow);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to convert counter IDs to SDK.\n");
            return status;
        }

        if (bulk_required_port) {
            status = mlnx_prepare_bulk_counter_read(MLNX_BULK_TYPE_PORT,
                                                    cmd,
                                                    &bulk_read_key,
                                                    &bulk_read_buff);
            if (SAI_ERR(status)) {
                /* no need deallocate buffer, mlnx_prepare_bulk_counter_read shall do it */
                /* if any error occurs */
                SX_LOG_ERR("Failed to prepare bulk counter for port stats.\n");
                goto out;
            }
        }

        if (bulk_required_flow) {
            status = mlnx_prepare_bulk_counter_read(MLNX_BULK_TYPE_FLOW,
                                                    cmd,
                                                    &bulk_read_key_flow,
                                                    &bulk_read_buff_flow);
            if (SAI_ERR(status)) {
                /* no need deallocate buffer, mlnx_prepare_bulk_counter_read shall do it */
                /* if any error occurs */
                SX_LOG_ERR("Failed to prepare bulk counter for port stats.\n");
                /* to free port type counter */
                goto out2;
            }
        }
    }

    status = mlnx_port_stats_bulk_read(port_id,
                                       cmd,
                                       counter_types,
                                       number_of_counters,
                                       counter_ids,
                                       &key_get,
                                       &bulk_read_buff,
                                       &bulk_read_buff_flow,
                                       counters);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to read bulk counters.\n");
    }

    if (bulk_required_flow) {
        status = mlnx_deallocate_sx_bulk_buffer(&bulk_read_buff_flow);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to deallocate SDK flow buffer.\n");
        }
    }

out2:
    if (bulk_required_port) {
        sai_status_t status2;
        status2 = mlnx_deallocate_sx_bulk_buffer(&bulk_read_buff);
        if (SAI_ERR(status2)) {
            SX_LOG_ERR("Failed to deallocate SDK buffer.\n");
        }
        if (!SAI_ERR(status)) {
            status = status2;
        }
    }

out:
    return status;
}

/*
 * Routine Description:
 *   Get port statistics counters.
 *
 * Arguments:
 *    [in] port_id - port id
 *    [in] number_of_counters - number of counters in the array
 *    [in] counter_ids - specifies the array of counter ids
 *    [out] counters - array of resulting counter values.
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_get_port_stats(_In_ sai_object_id_t      port_id,
                                        _In_ uint32_t             number_of_counters,
                                        _In_ const sai_stat_id_t *counter_ids,
                                        _Out_ uint64_t           *counters)
{
    return mlnx_get_port_stats_ext(port_id, number_of_counters, counter_ids, SAI_STATS_MODE_READ, counters);
}

/*
 * Routine Description:
 *   Clear port statistics counters.
 *
 * Arguments:
 *    [in] port_id - port id
 *    [in] number_of_counters - number of counters in the array
 *    [in] counter_ids - specifies the array of counter ids
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_clear_port_stats(_In_ sai_object_id_t      port_id,
                                          _In_ uint32_t             number_of_counters,
                                          _In_ const sai_stat_id_t *counter_ids)
{
    return SAI_STATUS_NOT_IMPLEMENTED;
}

/*
 * Routine Description:
 *   Clear port's all statistics counters.
 *
 * Arguments:
 *    [in] port_id - port id
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
static sai_status_t mlnx_clear_port_all_stats(_In_ sai_object_id_t port_id)
{
    sai_status_t status;
    uint32_t     port_data;

    SX_LOG_ENTER();

    MLNX_LOG_OID("Clear all stats", port_id);

    if (SAI_STATUS_SUCCESS != (status = mlnx_object_to_type(port_id, SAI_OBJECT_TYPE_PORT, &port_data, NULL))) {
        return status;
    }

    if (SX_STATUS_SUCCESS !=
        (status = sx_api_port_counter_clear_set(gh_sdk, port_data, false, SX_PORT_CNTR_GRP_ALL))) {
        SX_LOG_ERR("Failed to clear all port counters - %s.\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_log_set(sx_verbosity_level_t level)
{
    sai_status_t status;

    LOG_VAR_NAME(__MODULE__) = level;

    if (gh_sdk) {
        status = sdk_to_sai(sx_api_port_log_verbosity_level_set(gh_sdk,
                                                                SX_LOG_VERBOSITY_BOTH,
                                                                level,
                                                                level));
        if (SAI_ERR(status)) {
            return status;
        }
        return sdk_to_sai(sx_api_bulk_counter_log_verbosity_level_set(gh_sdk,
                                                                      SX_LOG_VERBOSITY_BOTH,
                                                                      level,
                                                                      level));
    } else {
        return SAI_STATUS_SUCCESS;
    }
}

/*
 * Get index of port configuration in port qos db
 *
 * Arguments:
 *    [in]  log_port_id - logical port id
 *    [out] index       - index of the port in qos db
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS
 *    SAI_STATUS_FAILURE
 *
 */
/* DB read lock is needed */
sai_status_t mlnx_port_idx_by_log_id(sx_port_log_id_t log_port_id, uint32_t *index)
{
    mlnx_port_config_t *port;
    uint32_t            ii = 0;

    assert(index != NULL);

    mlnx_port_foreach(port, ii) {
        if (log_port_id == port->logical) {
            *index = ii;
            return SAI_STATUS_SUCCESS;
        }
    }

    SX_LOG_ERR("Port index not found in DB by log id 0x%x\n", log_port_id);
    return SAI_STATUS_INVALID_PORT_NUMBER;
}

/* DB read lock is needed */
sai_status_t mlnx_port_idx_by_obj_id(sai_object_id_t obj_id, uint32_t *index)
{
    mlnx_port_config_t *port;
    uint32_t            ii = 0;

    assert(index != NULL);

    mlnx_port_foreach(port, ii) {
        if (obj_id == port->saiport) {
            *index = ii;
            return SAI_STATUS_SUCCESS;
        }
    }

    SX_LOG_ERR("Port index not found in DB by obj id %" PRIx64 "\n", obj_id);
    return SAI_STATUS_INVALID_PORT_NUMBER;
}

/* DB read lock is needed */
sai_status_t mlnx_port_by_log_id_soft(sx_port_log_id_t log_id, mlnx_port_config_t **port)
{
    mlnx_port_config_t *port_cfg;
    uint32_t            ii;

    assert(port != NULL);

    mlnx_port_foreach(port_cfg, ii) {
        if (port_cfg->logical == log_id) {
            *port = port_cfg;
            return SAI_STATUS_SUCCESS;
        }
    }

    return SAI_STATUS_INVALID_PORT_NUMBER;
}

/* DB read lock is needed */
sai_status_t mlnx_port_by_log_id(sx_port_log_id_t log_id, mlnx_port_config_t **port)
{
    sai_status_t status;

    status = mlnx_port_by_log_id_soft(log_id, port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed lookup port config by log id 0x%x\n", log_id);
    }

    return status;
}

/* DB read lock is needed */
sai_status_t mlnx_lag_by_log_id(sx_port_log_id_t log_id, mlnx_port_config_t **lag)
{
    mlnx_port_config_t *lag_cfg;
    uint32_t            ii;

    assert(lag != NULL);

    mlnx_lag_foreach(lag_cfg, ii) {
        if (lag_cfg->logical == log_id) {
            *lag = lag_cfg;
            return SAI_STATUS_SUCCESS;
        }
    }

    SX_LOG_ERR("Failed lookup port config for lag by log id 0x%x\n", log_id);
    return SAI_STATUS_INVALID_PORT_NUMBER;
}

/* DB read lock is needed */
sai_status_t mlnx_port_by_obj_id(sai_object_id_t obj_id, mlnx_port_config_t **port)
{
    mlnx_port_config_t *port_cfg;
    uint32_t            ii;

    assert(port != NULL);

    mlnx_port_foreach(port_cfg, ii) {
        if (port_cfg->saiport == obj_id) {
            *port = port_cfg;
            return SAI_STATUS_SUCCESS;
        }
    }

    SX_LOG_ERR("Failed lookup port config by object id %" PRIx64 "\n", obj_id);
    return SAI_STATUS_INVALID_PORT_NUMBER;
}

/* DB read lock is needed */
sai_status_t mlnx_port_fetch_lag_if_lag_member(_Inout_ mlnx_port_config_t **port_config)
{
    sai_status_t        status;
    mlnx_port_config_t *lag;
    sx_port_log_id_t    lag_id;
    const bool          is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                                 (!g_sai_db_ptr->issu_end_called);

    assert(port_config);

    if (!is_warmboot_init_stage) {
        lag_id = (*port_config)->lag_id;
    } else {
        lag_id = (*port_config)->before_issu_lag_id;
    }

    if (mlnx_port_is_lag_member(*port_config)) {
        status = mlnx_lag_by_log_id(lag_id, &lag);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to find a lag port config for sai port %" PRIx64 "\n", (*port_config)->saiport);
            return status;
        }

        *port_config = lag;
    }

    return SAI_STATUS_SUCCESS;
}

sx_port_log_id_t mlnx_port_get_lag_id(const mlnx_port_config_t *port)
{
    const bool is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                        (!g_sai_db_ptr->issu_end_called);

    assert(NULL != port);
    assert(mlnx_port_is_lag_member(port));
    if (!mlnx_port_is_lag_member(port)) {
        SX_LOG_ERR("Port is not a LAG member\n");
        return 0;
    }
    if (!is_warmboot_init_stage) {
        return port->lag_id;
    } else {
        return port->before_issu_lag_id;
    }
    return 0;
}

uint32_t mlnx_port_idx_get(const mlnx_port_config_t *port)
{
    return port->index;
}

bool mlnx_port_is_phy(const mlnx_port_config_t *port)
{
    return SX_PORT_TYPE_ID_GET(port->logical) == SX_PORT_TYPE_NETWORK;
}

bool mlnx_port_is_virt(const mlnx_port_config_t *port)
{
    return SX_PORT_TYPE_ID_GET(port->logical) == SX_PORT_TYPE_VPORT;
}

bool mlnx_port_is_lag(const mlnx_port_config_t *port)
{
    return SX_PORT_TYPE_ID_GET(port->logical) == SX_PORT_TYPE_LAG;
}

bool mlnx_port_is_lag_member(const mlnx_port_config_t *port)
{
    return !!port->lag_id || !!port->before_issu_lag_id;
}

bool mlnx_port_is_sai_lag_member(const mlnx_port_config_t *port)
{
    return !!port->lag_id;
}

/* During ISSU, port may be a LAG member in SDK but not in SAI */
bool mlnx_port_is_sdk_lag_member_not_sai(const mlnx_port_config_t *port)
{
    return !!port->before_issu_lag_id && !port->lag_id;
}

bool mlnx_log_port_is_cpu(sx_port_log_id_t log_id)
{
    return log_id == CPU_PORT;
}

bool mlnx_log_port_is_vport(sx_port_log_id_t log_id)
{
    return SX_PORT_TYPE_ID_GET(log_id) == SX_PORT_TYPE_VPORT;
}

const char * mlnx_port_type_str(const mlnx_port_config_t *port)
{
    if (mlnx_port_is_lag(port)) {
        return "lag";
    }

    return "port";
}

/** Ingress buffer profiles for port [sai_object_list_t]
 *  There can be up to SAI_SWITCH_ATTR_INGRESS_BUFFER_POOL_NUM profiles */
static sai_status_t mlnx_port_ingress_buffer_profile_list_get(_In_ const sai_object_key_t   *key,
                                                              _Inout_ sai_attribute_value_t *value,
                                                              _In_ uint32_t                  attr_index,
                                                              _Inout_ vendor_cache_t        *cache,
                                                              void                          *arg)
{
    return mlnx_buffer_port_profile_list_get(key->key.object_id, value, true);
}

/** Ingress buffer profiles for port [sai_object_list_t]
 *  There can be up to SAI_SWITCH_ATTR_INGRESS_BUFFER_POOL_NUM profiles */
static sai_status_t mlnx_port_ingress_buffer_profile_list_set(_In_ const sai_object_key_t      *key,
                                                              _In_ const sai_attribute_value_t *value,
                                                              void                             *arg)
{
    return mlnx_buffer_port_profile_list_set(key->key.object_id, value, true);
}

/** Egress buffer profiles for port [sai_object_list_t]
 *  There can be up to SAI_SWITCH_ATTR_EGRESS_BUFFER_POOL_NUM profiles */
static sai_status_t mlnx_port_egress_buffer_profile_list_get(_In_ const sai_object_key_t   *key,
                                                             _Inout_ sai_attribute_value_t *value,
                                                             _In_ uint32_t                  attr_index,
                                                             _Inout_ vendor_cache_t        *cache,
                                                             void                          *arg)
{
    return mlnx_buffer_port_profile_list_get(key->key.object_id, value, false);
}

/** Egress buffer profiles for port [sai_object_list_t]
 *  There can be up to SAI_SWITCH_ATTR_EGRESS_BUFFER_POOL_NUM profiles */
static sai_status_t mlnx_port_egress_buffer_profile_list_set(_In_ const sai_object_key_t      *key,
                                                             _In_ const sai_attribute_value_t *value,
                                                             void                             *arg)
{
    return mlnx_buffer_port_profile_list_set(key->key.object_id, value, false);
}

/*
 * A soft clear only updates a SAI DB
 * It is needed when port is joining to the LAG (storm control policers will be cleared by SDK internally)
 */
sai_status_t mlnx_port_storm_control_policer_params_clear(_In_ mlnx_port_config_t *port_config, _In_ bool is_soft)
{
    sai_status_t             status;
    mlnx_policer_bind_params bind_params;
    mlnx_port_policer_type   policer_type;

    assert(port_config);

    for (policer_type = MLNX_PORT_POLICER_TYPE_REGULAR_INDEX;
         policer_type < MLNX_PORT_POLICER_TYPE_MAX;
         policer_type++) {
        if (is_soft) {
            port_config->port_policers[policer_type] = SAI_NULL_OBJECT_ID;
        } else {
            bind_params.port_policer_type = policer_type;
            status = mlnx_sai_unbind_policer_from_port(port_config->saiport, &bind_params);
            if (SAI_ERR(status)) {
                return status;
            }
        }
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_storm_control_policer_params_clone(_In_ mlnx_port_config_t       *to,
                                                          _In_ const mlnx_port_config_t *from)
{
    sai_status_t             status;
    mlnx_policer_bind_params bind_params;
    mlnx_port_policer_type   policer_type;
    sai_object_id_t          policer_id;

    assert(to);
    assert(from);

    for (policer_type = MLNX_PORT_POLICER_TYPE_REGULAR_INDEX;
         policer_type < MLNX_PORT_POLICER_TYPE_MAX;
         policer_type++) {
        bind_params.port_policer_type = policer_type;
        policer_id = from->port_policers[policer_type];

        status = mlnx_sai_unbind_policer_from_port(to->saiport, &bind_params);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to unbind policer from %x before cloning from %x to %x\n",
                       to->logical,
                       from->logical,
                       to->logical);
            return status;
        }

        if (SAI_NULL_OBJECT_ID != policer_id) {
            status = mlnx_sai_bind_policer_to_port(to->saiport, policer_id, &bind_params);
            if (SAI_ERR(status)) {
                SX_LOG_ERR("Failed to clone policers from %x to %x\n", from->logical, to->logical);
                return status;
            }
        }
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_storm_control_policer_attr_set(_In_ const sai_object_key_t      *key,
                                                             _In_ const sai_attribute_value_t *value,
                                                             _In_ void                        *arg)
{
    sai_status_t           status;
    mlnx_port_policer_type policer_function;

    SX_LOG_ENTER();
    policer_function = (mlnx_port_policer_type)arg;

    sai_db_write_lock();
    status = mlnx_sai_policer_bind_set_impl(key->key.object_id, value->oid, policer_function);
    sai_db_unlock();

    SX_LOG_EXIT();

    return status;
}

static sai_status_t mlnx_port_storm_control_policer_attr_get(_In_ const sai_object_key_t   *key,
                                                             _Inout_ sai_attribute_value_t *value,
                                                             _In_ uint32_t                  attr_index,
                                                             _Inout_ vendor_cache_t        *cache,
                                                             _In_ void                     *arg)
{
    sai_status_t             status;
    mlnx_port_config_t      *port_config;
    sai_object_id_t          sai_policer;
    mlnx_port_policer_type   policer_function;
    mlnx_policer_db_entry_t *policer_db_entry = NULL;
    sx_port_log_id_t         port_id;

    SX_LOG_ENTER();

    policer_function = (mlnx_port_policer_type)arg;
    if (policer_function >= MLNX_PORT_POLICER_TYPE_MAX) {
        SX_LOG_ERR("Invalid policer type:%d passed for port:%" PRIx64 "\n", policer_function, key->key.object_id);
        SX_LOG_EXIT();
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    sai_db_read_lock();

    if (mlnx_log_port_is_cpu(port_id)) {
        value->oid = SAI_NULL_OBJECT_ID;
    } else {
        status = mlnx_port_by_obj_id(key->key.object_id, &port_config);
        if (SAI_ERR(status)) {
            goto out;
        }

        if (mlnx_port_is_sai_lag_member(port_config)) {
            status = mlnx_port_fetch_lag_if_lag_member(&port_config);
            if (SAI_ERR(status)) {
                goto out;
            }
        }

        sai_policer = port_config->port_policers[policer_function];

        if (SAI_NULL_OBJECT_ID == sai_policer) {
            SX_LOG_DBG("port:%" PRIx64 ", port_db[%d] contains NULL policer\n", key->key.object_id, policer_function);
            value->oid = sai_policer;
            goto out;
        }

        if (SAI_STATUS_SUCCESS != (status = db_get_sai_policer_data(sai_policer, &policer_db_entry))) {
            SX_LOG_ERR("Failed to obtain db entry for sai_policer:%" PRIx64 "\n", sai_policer);
            goto out;
        }

        value->oid = sai_policer;
    }

out:
    sai_db_unlock();
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_egress_block_set(_In_ const sai_object_key_t      *key,
                                               _In_ const sai_attribute_value_t *value,
                                               _In_ void                        *arg)
{
    sai_status_t        status;
    mlnx_port_config_t *port;
    sx_port_log_id_t   *sx_egress_ports = NULL;
    sx_port_log_id_t    port_id;

    SX_LOG_ENTER();

    sai_db_write_lock();
    status = mlnx_validate_port_isolation_api(PORT_ISOLATION_API_EGRESS_BLOCK_PORT);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Isolation group in use\n");
        goto out;
    }

    status = mlnx_port_by_obj_id(key->key.object_id, &port);
    if (SAI_ERR(status)) {
        goto out;
    }

    /* In case if port is LAG member then use LAG logical id */
    port_id = port->logical;
    if (mlnx_port_is_lag_member(port)) {
        port_id = mlnx_port_get_lag_id(port);
    }

    sx_egress_ports = calloc(MAX_PORTS, sizeof(*sx_egress_ports));
    if (!sx_egress_ports) {
        SX_LOG_ERR("Failed to allocate memory\n");
        status = SAI_STATUS_NO_MEMORY;
        goto out;
    }

    status = mlnx_port_egress_block_sai_ports_to_sx(port_id,
                                                    value->objlist.list,
                                                    value->objlist.count,
                                                    0,
                                                    sx_egress_ports);
    if (SAI_ERR(status)) {
        goto out;
    }

    status = mlnx_port_egress_block_set_impl(port_id, sx_egress_ports, value->objlist.count);
    if (SAI_ERR(status)) {
        goto out;
    }

out:
    sai_db_unlock();
    free(sx_egress_ports);
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_egress_block_get(_In_ const sai_object_key_t   *key,
                                               _Inout_ sai_attribute_value_t *value,
                                               _In_ uint32_t                  attr_index,
                                               _Inout_ vendor_cache_t        *cache,
                                               _In_ void                     *arg)
{
    sai_status_t        status;
    mlnx_port_config_t *port;
    sx_port_log_id_t   *sx_egress_block_ports = NULL;
    sai_object_id_t    *sai_egress_block_ports = NULL;
    uint32_t            egress_block_ports_count, ii;
    sx_port_log_id_t    port_id;

    SX_LOG_ENTER();

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT, &port_id, NULL))) {
        return status;
    }

    sai_db_read_lock();

    if (mlnx_log_port_is_cpu(port_id)) {
        value->objlist.count = 0;
    } else {
        status = mlnx_port_by_obj_id(key->key.object_id, &port);
        if (SAI_ERR(status)) {
            goto out;
        }

        /* In case if port is LAG member then use LAG logical id */

        if (mlnx_port_is_lag_member(port)) {
            port_id = mlnx_port_get_lag_id(port);
        }

        sx_egress_block_ports = calloc(MAX_PORTS, sizeof(*sx_egress_block_ports));
        if (!sx_egress_block_ports) {
            SX_LOG_ERR("Failed to allocate memory\n");
            status = SAI_STATUS_NO_MEMORY;
            goto out;
        }

        sai_egress_block_ports = calloc(MAX_PORTS, sizeof(*sai_egress_block_ports));
        if (!sai_egress_block_ports) {
            SX_LOG_ERR("Failed to allocate memory\n");
            status = SAI_STATUS_NO_MEMORY;
            goto out;
        }

        egress_block_ports_count = 0;
        status = mlnx_port_egress_block_get_impl(port_id,
                                                 sx_egress_block_ports,
                                                 &egress_block_ports_count);
        if (SAI_ERR(status)) {
            goto out;
        }

        for (ii = 0; ii < egress_block_ports_count; ii++) {
            status = mlnx_create_object(SAI_OBJECT_TYPE_PORT, sx_egress_block_ports[ii],
                                        NULL, &sai_egress_block_ports[ii]);
            if (SAI_ERR(status)) {
                goto out;
            }
        }

        status = mlnx_fill_objlist(sai_egress_block_ports, egress_block_ports_count, &value->objlist);
        if (SAI_ERR(status)) {
            goto out;
        }
    }

out:
    sai_db_unlock();
    free(sx_egress_block_ports);
    free(sai_egress_block_ports);
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_isolation_group_set(_In_ const sai_object_key_t      *key,
                                                  _In_ const sai_attribute_value_t *value,
                                                  _In_ void                        *arg)
{
    sai_status_t status;

    SX_LOG_ENTER();

    sai_db_write_lock();
    status = mlnx_set_port_isolation_group_impl(key->key.object_id, value->oid);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to set isolation group\n");
        goto out;
    }

out:
    sai_db_unlock();

    return status;
}

static sai_status_t mlnx_port_isolation_group_get(_In_ const sai_object_key_t   *key,
                                                  _Inout_ sai_attribute_value_t *value,
                                                  _In_ uint32_t                  attr_index,
                                                  _Inout_ vendor_cache_t        *cache,
                                                  _In_ void                     *arg)
{
    sai_status_t status = SAI_STATUS_SUCCESS;

    SX_LOG_ENTER();

    sai_db_read_lock();

    if (is_isolation_group_in_use()) {
        status = mlnx_get_port_isolation_group_impl(key->key.object_id, &value->oid);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to get port isolation group id\n");
            goto out;
        }
    } else {
        value->oid = SAI_NULL_OBJECT_ID;
    }
out:
    sai_db_unlock();

    return status;
}

static sai_status_t mlnx_port_egress_block_sai_ports_to_sx(_In_ sx_port_log_id_t       sx_ing_port_id,
                                                           _In_ const sai_object_id_t *egress_ports,
                                                           _In_ uint32_t               egress_ports_count,
                                                           _In_ uint32_t               attr_index,
                                                           _Out_ sx_port_log_id_t     *sx_egress_ports)
{
    sai_status_t status;
    uint32_t     ii;

    assert(sx_egress_ports);
    assert(sx_egress_ports || (egress_ports_count == 0));

    if (egress_ports_count > MAX_PORTS) {
        SX_LOG_ERR("Ports count is to big (%d), max allowed - %d\n", egress_ports_count, MAX_PORTS);
        return SAI_STATUS_INVALID_ATTR_VALUE_0 + attr_index;
    }

    for (ii = 0; ii < egress_ports_count; ii++) {
        status = mlnx_object_to_type(egress_ports[ii], SAI_OBJECT_TYPE_PORT, &sx_egress_ports[ii], NULL);
        if (SAI_ERR(status)) {
            return status;
        }

        if (sx_egress_ports[ii] == sx_ing_port_id) {
            SX_LOG_ERR("The port itself must not be in the EGRESS_BLOCK_LIST\n");
            return SAI_STATUS_INVALID_ATTR_VALUE_0 + attr_index;
        }
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_egress_block_set_single_port(_In_ sx_port_log_id_t        sx_ing_port_id,
                                                           _In_ sx_port_log_id_t        block_port_id,
                                                           _In_ const sx_port_log_id_t *sx_egress_block_port_list,
                                                           _In_ uint32_t                egress_ports_count)
{
    sx_status_t     sx_status;
    sx_access_cmd_t sx_cmd;
    uint32_t        ii;
    bool            need_to_block;

    SX_LOG_ENTER();

    if (block_port_id == sx_ing_port_id) {
        SX_LOG_EXIT();
        return SAI_STATUS_SUCCESS;
    }

    need_to_block = false;
    for (ii = 0; ii < egress_ports_count; ii++) {
        if (block_port_id == sx_egress_block_port_list[ii]) {
            need_to_block = true;
            break;
        }
    }

    sx_cmd = need_to_block ? SX_ACCESS_CMD_ADD : SX_ACCESS_CMD_DELETE;

    SX_LOG_DBG("%s a port [%x] for port [%x] isolation group\n", SX_ACCESS_CMD_STR(
                   sx_cmd), sx_ing_port_id, block_port_id);

    sx_status = sx_api_port_isolate_set(gh_sdk, sx_cmd, block_port_id, &sx_ing_port_id, 1);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to %s a port [%x] for port [%x] isolation group - %s\n",
                   SX_ACCESS_CMD_STR(sx_cmd), sx_ing_port_id, block_port_id, SX_STATUS_MSG(sx_status));
        SX_LOG_EXIT();
        return sdk_to_sai(sx_status);
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_egress_block_set_impl(_In_ sx_port_log_id_t        sx_ing_port_id,
                                                    _In_ const sx_port_log_id_t *sx_egress_block_port_list,
                                                    _In_ uint32_t                egress_ports_count)
{
    sx_status_t               sx_status;
    sai_status_t              sai_status;
    const mlnx_port_config_t *port;
    uint32_t                  ii;
    const bool                is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                                       (!g_sai_db_ptr->issu_end_called);
    sx_port_log_id_t log_port_list[MAX_PORTS_DB * 2];
    uint32_t         port_list_cnt = MAX_PORTS_DB * 2;
    sx_port_type_t   port_type;

    assert(sx_egress_block_port_list || (egress_ports_count == 0));

    SX_LOG_ENTER();

    if (!is_warmboot_init_stage) {
        mlnx_port_not_in_lag_foreach(port, ii) {
            sai_status = mlnx_port_egress_block_set_single_port(sx_ing_port_id,
                                                                port->logical,
                                                                sx_egress_block_port_list,
                                                                egress_ports_count);
            if (SAI_ERR(sai_status)) {
                SX_LOG_ERR("Error setting egress block on single port %x for egress block port %x\n",
                           sx_ing_port_id, port->logical);
                SX_LOG_EXIT();
                return sai_status;
            }
        }
    } else {
        /* For ISSU, we first loop through all non-LAG-member and non-LAG ports */
        mlnx_phy_port_not_in_lag_foreach(port, ii) {
            sai_status = mlnx_port_egress_block_set_single_port(sx_ing_port_id,
                                                                port->logical,
                                                                sx_egress_block_port_list,
                                                                egress_ports_count);
            if (SAI_ERR(sai_status)) {
                SX_LOG_ERR("Error setting egress block on single port %x for egress block port %x\n",
                           sx_ing_port_id, port->logical);
                SX_LOG_EXIT();
                return sai_status;
            }
        }
        /* Then we loop through all LAGs in SDK port list, because they might not be in SAI port db yet during ISSU */
        sx_status = sx_api_port_swid_port_list_get(gh_sdk, DEFAULT_ETH_SWID, log_port_list, &port_list_cnt);
        if (SX_STATUS_SUCCESS != sx_status) {
            SX_LOG_ERR("Error getting port swid port list: %s\n",
                       SX_STATUS_MSG(sx_status));
            sai_status = sdk_to_sai(sx_status);
            SX_LOG_EXIT();
            return sai_status;
        }

        for (ii = 0; ii < port_list_cnt; ii++) {
            port_type = SX_PORT_TYPE_ID_GET(log_port_list[ii]);
            if (SX_PORT_TYPE_LAG & port_type) {
                sai_status = mlnx_port_egress_block_set_single_port(sx_ing_port_id,
                                                                    log_port_list[ii],
                                                                    sx_egress_block_port_list,
                                                                    egress_ports_count);
                if (SAI_ERR(sai_status)) {
                    SX_LOG_ERR("Error setting egress block for port %x on egress block port %x\n",
                               sx_ing_port_id,
                               log_port_list[ii]);
                    SX_LOG_EXIT();
                    return sai_status;
                }
            }
        }
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/*
 * Returns a list of ports that have sx_ing_port_id as member of isolation group
 * sx_egress_block_ports should have at least MAX_PORTS elements
 */
static sai_status_t mlnx_port_egress_block_get_impl(_In_ sx_port_log_id_t   sx_ing_port_id,
                                                    _Out_ sx_port_log_id_t *sx_egress_block_ports,
                                                    _Out_ uint32_t         *sx_egress_block_ports_count)
{
    sai_status_t              status = SAI_STATUS_SUCCESS;
    sx_status_t               sx_status;
    sx_port_log_id_t         *sx_port_isolation_group = NULL;
    const mlnx_port_config_t *port;
    uint32_t                  sx_port_isolation_group_size, egress_block_ports_count;
    uint32_t                  ii, jj;
    const bool                is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                                       (!g_sai_db_ptr->issu_end_called);

    assert(sx_egress_block_ports);
    assert(sx_egress_block_ports_count);

    sx_port_isolation_group = calloc(MAX_PORTS, sizeof(*sx_port_isolation_group));
    if (!sx_port_isolation_group) {
        SX_LOG_ERR("Failed to allocate memory\n");
        return SAI_STATUS_NO_MEMORY;
    }

    egress_block_ports_count = 0;
    mlnx_port_foreach(port, ii) {
        if (port->logical == sx_ing_port_id) {
            continue;
        }
        /* Skip LAG which only exists in SDK but not in SAI */
        if (is_warmboot_init_stage && (0 == port->logical)) {
            continue;
        }

        sx_port_isolation_group_size = MAX_PORTS;
        sx_status = sx_api_port_isolate_get(gh_sdk, port->logical, sx_port_isolation_group,
                                            &sx_port_isolation_group_size);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to get isolation group for port [%x] - %s\n", port->logical, SX_STATUS_MSG(sx_status));
            status = sdk_to_sai(sx_status);
            goto out;
        }

        SX_LOG_DBG("Got isolation group for port %x, size = %d\n", port->logical, sx_port_isolation_group_size);

        for (jj = 0; jj < sx_port_isolation_group_size; jj++) {
            if (sx_ing_port_id == sx_port_isolation_group[jj]) {
                SX_LOG_DBG("port %x has a port %x in isolation group\n", sx_port_isolation_group[jj], sx_ing_port_id);

                sx_egress_block_ports[egress_block_ports_count] = port->logical;
                egress_block_ports_count++;
                break;
            }
        }
    }

    *sx_egress_block_ports_count = egress_block_ports_count;

out:
    free(sx_port_isolation_group);
    return status;
}

/*
 * is_in_use = true if sx_port_id is a member of another port's EGRESS_PORT_BLOCK_LIST
 */
sai_status_t mlnx_port_egress_block_is_in_use(_In_ sx_port_log_id_t sx_port_id, _Out_ bool            *is_in_use)
{
    sx_status_t sx_status;
    uint32_t    sx_port_isolation_group_size;

    assert(is_in_use);

    sx_status = sx_api_port_isolate_get(gh_sdk, sx_port_id, NULL, &sx_port_isolation_group_size);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to get isolation group for port [%x] - %s\n", sx_port_id, SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    *is_in_use = (sx_port_isolation_group_size != 0);

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_egress_block_clear(_In_ sx_port_log_id_t sx_port_id)
{
    sai_status_t status = SAI_STATUS_SUCCESS;

    if (is_egress_block_in_use()) {
        SX_LOG_DBG("Clear egress block on %x\n", sx_port_id);
        return mlnx_port_egress_block_set_impl(sx_port_id, NULL, 0);
    }

    return status;
}

sai_status_t mlnx_sx_port_list_compare(_In_ const sx_port_log_id_t *ports1,
                                       _In_ uint32_t                ports1_count,
                                       _In_ const sx_port_log_id_t *ports2,
                                       _In_ uint32_t                ports2_count,
                                       _Out_ bool                  *equal)
{
    uint32_t ii, jj;

    assert(ports1);
    assert(ports2);
    assert(equal);

    if (ports1_count != ports2_count) {
        SX_LOG_ERR("Ports counts are not equal (%d and %d)\n", ports1_count, ports2_count);
        *equal = false;
        return SAI_STATUS_SUCCESS;
    }

    *equal = true;
    for (ii = 0; ii < ports1_count; ii++) {
        for (jj = ii + 1; jj < ports2_count; jj++) {
            if (ports1[ii] == ports2[jj]) {
                *equal = false;
                return SAI_STATUS_SUCCESS;
            }
        }
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_egress_block_clone(_In_ mlnx_port_config_t *to, _In_ const mlnx_port_config_t *from)
{
    sai_status_t      status = SAI_STATUS_SUCCESS;
    sx_port_log_id_t *sx_port_egress_block_ports = NULL;
    uint32_t          sx_port_egress_block_ports_count;

    assert(to);
    assert(from);

    SX_LOG_DBG("Clone egress block list from [%lx] to [%lx]\n", from->saiport, to->saiport);

    sx_port_egress_block_ports = calloc(MAX_PORTS, sizeof(*sx_port_egress_block_ports));
    if (!sx_port_egress_block_ports) {
        SX_LOG_ERR("Failed to allocate memory\n");
        return SAI_STATUS_NO_MEMORY;
    }

    status = mlnx_port_egress_block_get_impl(from->logical,
                                             sx_port_egress_block_ports,
                                             &sx_port_egress_block_ports_count);
    if (SAI_ERR(status)) {
        goto out;
    }

    status =
        mlnx_port_egress_block_set_impl(to->logical, sx_port_egress_block_ports, sx_port_egress_block_ports_count);
    if (SAI_ERR(status)) {
        goto out;
    }

out:
    free(sx_port_egress_block_ports);
    return status;
}

mlnx_port_config_t * mlnx_port_by_idx(uint16_t id)
{
    return &mlnx_ports_db[id];
}

mlnx_port_config_t * mlnx_port_by_local_id(uint16_t local_port)
{
    mlnx_port_config_t *port;
    uint32_t            ii;

    for (ii = 0; ii < MAX_PORTS; ii++) {
        port = &mlnx_ports_db[ii];

        if (port->port_map.local_port == local_port) {
            return port;
        }
    }

    assert(false);
    return NULL;
}

static uint32_t mlnx_platform_max_speed_get(void)
{
    switch (g_sai_db_ptr->sx_chip_type) {
    case SX_CHIP_TYPE_SPECTRUM:
    case SX_CHIP_TYPE_SPECTRUM_A1:
        return PORT_SPEED_MAX_SP;

    case SX_CHIP_TYPE_SPECTRUM2:
        if ((g_sai_db_ptr->platform_type == MLNX_PLATFORM_TYPE_3800) ||
            (g_sai_db_ptr->platform_type == MLNX_PLATFORM_TYPE_3420)) {
            return PORT_SPEED_100;
        }
        return PORT_SPEED_MAX_SP2;

    case SX_CHIP_TYPE_SPECTRUM3:
        if ((g_sai_db_ptr->platform_type == MLNX_PLATFORM_TYPE_4700) ||
            (g_sai_db_ptr->platform_type == MLNX_PLATFORM_TYPE_4410)) {
            return PORT_SPEED_400;
        }
        if (g_sai_db_ptr->platform_type == MLNX_PLATFORM_TYPE_4600C) {
            return PORT_SPEED_100;
        }
        if (g_sai_db_ptr->platform_type == MLNX_PLATFORM_TYPE_4600) {
            return PORT_SPEED_200;
        }
        return PORT_SPEED_MAX_SP3;

    case SX_CHIP_TYPE_SPECTRUM4:
        return PORT_SPEED_800;

    default:
        SX_LOG_ERR("g_sai_db_ptr->sx_chip_type = %s\n", SX_CHIP_TYPE_STR(g_sai_db_ptr->sx_chip_type));
        return 0;
    }
}

static sai_status_t mlnx_port_rate_bitmask_to_speeds(_In_ const sx_port_rate_bitmask_t *sx_rate_bitmask,
                                                     _Out_ uint32_t                    *speeds,
                                                     _Inout_ uint32_t                  *speeds_count)
{
    uint32_t speeds_count_tmp = 0;

    assert(sx_rate_bitmask);
    assert(speeds);
    assert(speeds_count && (*speeds_count >= MAX_NUM_PORT_SPEEDS));

    if (sx_rate_bitmask->rate_auto) {
        speeds[speeds_count_tmp++] = mlnx_platform_max_speed_get();
    }

    if (sx_rate_bitmask->rate_800G) {
        speeds[speeds_count_tmp++] = PORT_SPEED_800;
    }

    if (sx_rate_bitmask->rate_400G) {
        speeds[speeds_count_tmp++] = PORT_SPEED_400;
    }

    if (sx_rate_bitmask->rate_200G) {
        speeds[speeds_count_tmp++] = PORT_SPEED_200;
    }

    if (sx_rate_bitmask->rate_100G) {
        speeds[speeds_count_tmp++] = PORT_SPEED_100;
    }

    if (sx_rate_bitmask->rate_50G) {
        speeds[speeds_count_tmp++] = PORT_SPEED_50;
    }

    if (sx_rate_bitmask->rate_40G) {
        speeds[speeds_count_tmp++] = PORT_SPEED_40;
    }

    if (sx_rate_bitmask->rate_25G) {
        speeds[speeds_count_tmp++] = PORT_SPEED_25;
    }

    if (sx_rate_bitmask->rate_10G) {
        speeds[speeds_count_tmp++] = PORT_SPEED_10;
    }

    if (sx_rate_bitmask->rate_1G) {
        speeds[speeds_count_tmp++] = PORT_SPEED_1;
    }

    *speeds_count = speeds_count_tmp;

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_speed_bitmap_to_rate_bitmask(_In_ uint32_t                 speed_bitmap,
                                                           _Out_ sx_port_rate_bitmask_t *sx_rate_bitmask)
{
    assert(sx_rate_bitmask);

    memset(sx_rate_bitmask, 0, sizeof(*sx_rate_bitmask));

    if (speed_bitmap & 1 << 1) {
        sx_rate_bitmask->rate_1G = true;
    }

    if (speed_bitmap & 1 << 4) {
        sx_rate_bitmask->rate_10G = true;
    }

    if (speed_bitmap & 1 << 5) {
        sx_rate_bitmask->rate_40G = true;
    }

    if (speed_bitmap & 1 << 6) {
        sx_rate_bitmask->rate_25G = true;
    }

    if ((speed_bitmap & 1 << 7) ||
        (speed_bitmap & 1 << 8)) {
        sx_rate_bitmask->rate_50G = true;
    }

    if ((speed_bitmap & 1 << 9) ||
        (speed_bitmap & 1 << 10)) {
        sx_rate_bitmask->rate_100G = true;
    }

    if (speed_bitmap & 1 << 12) {
        sx_rate_bitmask->rate_200G = true;
    }

    if (speed_bitmap & 1 << 15) {
        sx_rate_bitmask->rate_400G = true;
    }

    if (speed_bitmap & 1 << 18) {
        sx_rate_bitmask->rate_800G = true;
    }


    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_speed_convert_bitmap_to_capability(const sx_port_speed_t       speed_bitmap,
                                                                 sx_port_speed_capability_t* speed_capability)
{
    if (NULL == speed_capability) {
        SX_LOG_ERR("NULL pointer: Port Speed Capability");
        return SAI_STATUS_FAILURE;
    }

    memset(speed_capability, 0, sizeof(*speed_capability));
    if (speed_bitmap & 1) {
        speed_capability->mode_1GB_CX_SGMII = TRUE;
    }
    if (speed_bitmap & 1 << 1) {
        speed_capability->mode_1GB_KX = TRUE;
    }
    if (speed_bitmap & 1 << 2) {
        speed_capability->mode_10GB_CX4_XAUI = TRUE;
    }
    if (speed_bitmap & 1 << 3) {
        speed_capability->mode_10GB_KX4 = TRUE;
    }
    if (speed_bitmap & 1 << 4) {
        speed_capability->mode_10GB_KR = TRUE;
    }
    if (speed_bitmap & 1 << 5) {
        speed_capability->mode_20GB_KR2 = TRUE;
    }
    if (speed_bitmap & 1 << 6) {
        speed_capability->mode_40GB_CR4 = TRUE;
    }
    if (speed_bitmap & 1 << 7) {
        speed_capability->mode_40GB_KR4 = TRUE;
    }
    if (speed_bitmap & 1 << 8) {
        speed_capability->mode_56GB_KR4 = TRUE;
    }
    if (speed_bitmap & 1 << 9) {
        speed_capability->mode_56GB_KX4 = TRUE;
    }
    if (speed_bitmap & 1 << 10) {
        speed_capability->mode_10MB_T = TRUE;
    }
    if (speed_bitmap & 1 << 11) {
        speed_capability->mode_100MB_TX = TRUE;
    }
    if (speed_bitmap & 1 << 12) {
        speed_capability->mode_10GB_CR = TRUE;
    }
    if (speed_bitmap & 1 << 13) {
        speed_capability->mode_10GB_SR = TRUE;
    }
    if (speed_bitmap & 1 << 14) {
        speed_capability->mode_10GB_ER_LR = TRUE;
    }
    if (speed_bitmap & 1 << 15) {
        speed_capability->mode_40GB_SR4 = TRUE;
    }
    if (speed_bitmap & 1 << 16) {
        speed_capability->mode_40GB_LR4_ER4 = TRUE;
    }
    if (speed_bitmap & 1 << 17) {
        speed_capability->mode_1000MB_T = TRUE;
    }
    if (speed_bitmap & 1 << 18) {
        speed_capability->mode_50GB_SR2 = TRUE;
    }
    if (speed_bitmap & 1 << 20) {
        speed_capability->mode_100GB_CR4 = TRUE;
    }
    if (speed_bitmap & 1 << 21) {
        speed_capability->mode_100GB_SR4 = TRUE;
    }
    if (speed_bitmap & 1 << 22) {
        speed_capability->mode_100GB_KR4 = TRUE;
    }
    if (speed_bitmap & 1 << 23) {
        speed_capability->mode_100GB_LR4_ER4 = TRUE;
    }
    if (speed_bitmap & 1 << 27) {
        speed_capability->mode_25GB_CR = TRUE;
    }
    if (speed_bitmap & 1 << 28) {
        speed_capability->mode_25GB_KR = TRUE;
    }
    if (speed_bitmap & 1 << 29) {
        speed_capability->mode_25GB_SR = TRUE;
    }
    if (speed_bitmap & 1 << 30) {
        speed_capability->mode_50GB_CR2 = TRUE;
    }
    if (speed_bitmap & 1 << 31) {
        speed_capability->mode_50GB_KR2 = TRUE;
    }
    if (speed_bitmap == 0xFFFFFFFF) {
        speed_capability->mode_auto = TRUE;
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_bitmap_to_speeds_sp(_In_ const sx_port_speed_t speed_bitmap,
                                                  _Out_ uint32_t            *speeds,
                                                  _Inout_ uint32_t          *speeds_count)
{
    sai_status_t               status;
    sx_port_speed_capability_t sx_speed;
    uint32_t                   speeds_count_tmp = 0;

    memset(&sx_speed, 0, sizeof(sx_speed));

    status = mlnx_port_speed_convert_bitmap_to_capability(speed_bitmap, &sx_speed);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert port speed bitmap %d\n", speed_bitmap);
        return status;
    }

    assert(speeds);
    assert(speeds_count && (*speeds_count >= MAX_NUM_PORT_SPEEDS));

    if (sx_speed.mode_auto) {
        speeds[speeds_count_tmp++] = mlnx_platform_max_speed_get();
    }

    if (sx_speed.mode_100GB_CR4 ||
        sx_speed.mode_100GB_KR4 ||
        sx_speed.mode_100GB_SR4 ||
        sx_speed.mode_100GB_LR4_ER4) {
        speeds[speeds_count_tmp++] = PORT_SPEED_100;
    }

    if (sx_speed.mode_56GB_KR4 ||
        sx_speed.mode_56GB_KX4) {
        speeds[speeds_count_tmp++] = PORT_SPEED_56;
    }

    if (sx_speed.mode_50GB_CR2 ||
        sx_speed.mode_50GB_KR2 ||
        sx_speed.mode_50GB_SR2) {
        speeds[speeds_count_tmp++] = PORT_SPEED_50;
    }

    if (sx_speed.mode_40GB_CR4 ||
        sx_speed.mode_40GB_KR4 ||
        sx_speed.mode_40GB_SR4 ||
        sx_speed.mode_40GB_LR4_ER4) {
        speeds[speeds_count_tmp++] = PORT_SPEED_40;
    }

    if (sx_speed.mode_25GB_CR ||
        sx_speed.mode_25GB_KR ||
        sx_speed.mode_25GB_SR) {
        speeds[speeds_count_tmp++] = PORT_SPEED_25;
    }

    if (sx_speed.mode_20GB_KR2) {
        speeds[speeds_count_tmp++] = PORT_SPEED_20;
    }

    if (sx_speed.mode_10GB_CR ||
        sx_speed.mode_10GB_CX4_XAUI ||
        sx_speed.mode_10GB_ER_LR ||
        sx_speed.mode_10GB_KR ||
        sx_speed.mode_10GB_KX4 ||
        sx_speed.mode_10GB_SR) {
        speeds[speeds_count_tmp++] = PORT_SPEED_10;
    }

    if (sx_speed.mode_1GB_CX_SGMII ||
        sx_speed.mode_1GB_KX ||
        sx_speed.mode_1000MB_T) {
        speeds[speeds_count_tmp++] = PORT_SPEED_1;
    }

    if (sx_speed.mode_100MB_TX) {
        speeds[speeds_count_tmp++] = PORT_SPEED_100M;
    }

    if (sx_speed.mode_10MB_T) {
        speeds[speeds_count_tmp++] = PORT_SPEED_10M;
    }

    *speeds_count = speeds_count_tmp;

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_bitmap_to_speeds_sp2(_In_ const sx_port_speed_t speed_bitmap,
                                                   _Out_ uint32_t            *speeds,
                                                   _Inout_ uint32_t          *speeds_count)
{
    sx_port_rate_bitmask_t sx_rate_bitmask;
    sai_status_t           status;

    status = mlnx_port_speed_bitmap_to_rate_bitmask(speed_bitmap, &sx_rate_bitmask);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_port_rate_bitmask_to_speeds(&sx_rate_bitmask, speeds, speeds_count);
    if (SAI_ERR(status)) {
        return status;
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_bitmap_to_speeds(_In_ const sx_port_speed_t speed_bitmap,
                                        _Out_ uint32_t            *speeds,
                                        _Inout_ uint32_t          *speeds_count)
{
    sx_chip_types_t chip_type = g_sai_db_ptr->sx_chip_type;

    assert(speeds);
    assert(speeds_count);

    switch (chip_type) {
    case SX_CHIP_TYPE_SPECTRUM:
    case SX_CHIP_TYPE_SPECTRUM_A1:
        return mlnx_port_bitmap_to_speeds_sp(speed_bitmap, speeds, speeds_count);

    case SX_CHIP_TYPE_SPECTRUM2:
    case SX_CHIP_TYPE_SPECTRUM3:
    case SX_CHIP_TYPE_SPECTRUM4:
        return mlnx_port_bitmap_to_speeds_sp2(speed_bitmap, speeds, speeds_count);

    default:
        return SAI_STATUS_FAILURE;
    }
    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_speed_bitmap_apply(_In_ mlnx_port_config_t *port)
{
    assert(mlnx_port_cb);

    return mlnx_port_cb->speed_bitmap_apply(port);
}

static sai_status_t mlnx_port_speed_get_sp(_In_ sx_port_log_id_t sx_port,
                                           _Out_ uint32_t       *oper_speed,
                                           _Out_ uint32_t       *admin_speed)
{
    sx_status_t                sx_status;
    sx_port_speed_capability_t speed_cap;
    sx_port_oper_speed_t       speed_oper;

    memset(&speed_cap, 0, sizeof(speed_cap));
    memset(&speed_oper, 0, sizeof(speed_oper));

    sx_status = sx_api_port_speed_get(gh_sdk, sx_port, &speed_cap, &speed_oper);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to get port %x speed - %s.\n", sx_port, SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    if (speed_cap.mode_100GB_CR4 || speed_cap.mode_100GB_SR4 || speed_cap.mode_100GB_KR4 ||
        speed_cap.mode_100GB_LR4_ER4) {
        *admin_speed = PORT_SPEED_100;
    } else if (speed_cap.mode_56GB_KX4 || speed_cap.mode_56GB_KR4) {
        *admin_speed = PORT_SPEED_56;
    } else if (speed_cap.mode_50GB_CR2 || speed_cap.mode_50GB_KR2 || speed_cap.mode_50GB_SR2) {
        *admin_speed = PORT_SPEED_50;
    } else if (speed_cap.mode_40GB_KR4 || speed_cap.mode_40GB_CR4 || speed_cap.mode_40GB_SR4 ||
               speed_cap.mode_40GB_LR4_ER4) {
        *admin_speed = PORT_SPEED_40;
    } else if (speed_cap.mode_25GB_CR || speed_cap.mode_25GB_KR || speed_cap.mode_25GB_SR) {
        *admin_speed = PORT_SPEED_25;
    } else if (speed_cap.mode_20GB_KR2) {
        *admin_speed = PORT_SPEED_20;
    } else if (speed_cap.mode_10GB_KR || speed_cap.mode_10GB_KX4 || speed_cap.mode_10GB_CX4_XAUI ||
               speed_cap.mode_10GB_CR || speed_cap.mode_10GB_SR || speed_cap.mode_10GB_ER_LR) {
        *admin_speed = PORT_SPEED_10;
    } else if (speed_cap.mode_1GB_CX_SGMII || speed_cap.mode_1GB_KX || speed_cap.mode_1000MB_T) {
        *admin_speed = PORT_SPEED_1;
    } else if (speed_cap.mode_100MB_TX) {
        *admin_speed = PORT_SPEED_100M;
    } else if (speed_cap.mode_10MB_T) {
        *admin_speed = PORT_SPEED_10M;
    } else if (speed_cap.mode_auto) {
        *admin_speed = PORT_SPEED_MAX_SP;
    } else {
        /* After warm boot, without explicit call to set speed, SDK returns empty bitmask, and this is valid */
        *admin_speed = PORT_SPEED_0;
    }

    switch (speed_oper) {
    case SX_PORT_SPEED_NA:
    case SX_PORT_SPEED_NOT_SUPPORTED:
        *oper_speed = PORT_SPEED_0;
        break;

    case SX_PORT_SPEED_10MB_T:
        *oper_speed = PORT_SPEED_10M;
        break;

    case SX_PORT_SPEED_100MB_TX:
        *oper_speed = PORT_SPEED_100M;
        break;

    case SX_PORT_SPEED_1GB_CX_SGMII:
    case SX_PORT_SPEED_1GB_KX:
    case SX_PORT_SPEED_1000MB_T:
        *oper_speed = PORT_SPEED_1;
        break;

    case SX_PORT_SPEED_10GB_CX4_XAUI:
    case SX_PORT_SPEED_10GB_KX4:
    case SX_PORT_SPEED_10GB_KR:
    case SX_PORT_SPEED_10GB_CR:
    case SX_PORT_SPEED_10GB_SR:
    case SX_PORT_SPEED_10GB_ER_LR:
        *oper_speed = PORT_SPEED_10;
        break;

    case SX_PORT_SPEED_20GB_KR2:
        *oper_speed = PORT_SPEED_20;
        break;

    case SX_PORT_SPEED_40GB_CR4:
    case SX_PORT_SPEED_40GB_KR4:
    case SX_PORT_SPEED_40GB_SR4:
    case SX_PORT_SPEED_40GB_LR4_ER4:
        *oper_speed = PORT_SPEED_40;
        break;

    case SX_PORT_SPEED_56GB_KR4:
    case SX_PORT_SPEED_56GB_KX4:
        *oper_speed = PORT_SPEED_56;
        break;

    case SX_PORT_SPEED_100GB_CR4:
    case SX_PORT_SPEED_100GB_SR4:
    case SX_PORT_SPEED_100GB_KR4:
    case SX_PORT_SPEED_100GB_LR4_ER4:
        *oper_speed = PORT_SPEED_100;
        break;

    case SX_PORT_SPEED_25GB_CR:
    case SX_PORT_SPEED_25GB_KR:
    case SX_PORT_SPEED_25GB_SR:
        *oper_speed = PORT_SPEED_25;
        break;

    case SX_PORT_SPEED_50GB_CR2:
    case SX_PORT_SPEED_50GB_KR2:
    case SX_PORT_SPEED_50GB_SR2:
        *oper_speed = PORT_SPEED_50;
        break;

    default:
        SX_LOG_ERR("Unexpected port oper speed %d\n", speed_oper);
        return SAI_STATUS_FAILURE;
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_speed_get_sp2(_In_ sx_port_log_id_t sx_port,
                                            _Out_ uint32_t       *oper_speed,
                                            _Out_ uint32_t       *admin_speed)
{
    sx_status_t                       sx_status;
    sx_port_rate_e                    oper_rate;
    sai_status_t                      status;
    sx_port_rate_bitmask_t            sx_admin_rate, sx_capab_rate;
    sx_port_phy_module_type_bitmask_t sx_capab_type;
    uint32_t                          speeds[MAX_NUM_PORT_SPEEDS] = {0}, speeds_count = MAX_NUM_PORT_SPEEDS;

    assert(oper_speed);
    assert(admin_speed);

    memset(&sx_admin_rate, 0, sizeof(sx_admin_rate));
    memset(&sx_capab_rate, 0, sizeof(sx_capab_rate));
    memset(&sx_capab_type, 0, sizeof(sx_capab_type));

    sx_status = sx_api_port_rate_capability_get(gh_sdk, sx_port, &sx_admin_rate, &sx_capab_rate, &sx_capab_type);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to get port %x rate - %s\n", sx_port, SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    status = mlnx_port_rate_bitmask_to_speeds(&sx_admin_rate, speeds, &speeds_count);
    if (SAI_ERR(status)) {
        return status;
    }

    if (speeds_count == 0) {
        /* After warm boot, without explicit call to set speed, SDK returns empty bitmask, and this is valid */
        *admin_speed = PORT_SPEED_0;
    } else {
        *admin_speed = speeds[0];
    }

    sx_status = sx_api_port_rate_get(gh_sdk, sx_port, &oper_rate);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to get port %x oper rate - %s\n", sx_port, SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    switch (oper_rate) {
    case SX_PORT_RATE_NA_E:
        *oper_speed = PORT_SPEED_0;
        break;

    case SX_PORT_RATE_100M_E:
        *oper_speed = PORT_SPEED_100M;
        break;

    case SX_PORT_RATE_1G_E:
        *oper_speed = PORT_SPEED_1;
        break;

    case SX_PORT_RATE_10G_E:
        *oper_speed = PORT_SPEED_10;
        break;

    case SX_PORT_RATE_25G_E:
        *oper_speed = PORT_SPEED_25;
        break;

    case SX_PORT_RATE_40G_E:
        *oper_speed = PORT_SPEED_40;
        break;

    case SX_PORT_RATE_50Gx1_E:
    case SX_PORT_RATE_50Gx2_E:
        *oper_speed = PORT_SPEED_50;
        break;

    case SX_PORT_RATE_100Gx1_E:
    case SX_PORT_RATE_100Gx2_E:
    case SX_PORT_RATE_100Gx4_E:
        *oper_speed = PORT_SPEED_100;
        break;

    case SX_PORT_RATE_200Gx2_E:
    case SX_PORT_RATE_200Gx4_E:
        *oper_speed = PORT_SPEED_200;
        break;

    case SX_PORT_RATE_400Gx4_E:
    case SX_PORT_RATE_400Gx8_E:
        *oper_speed = PORT_SPEED_400;
        break;

    case SX_PORT_RATE_800Gx8_E:
        *oper_speed = PORT_SPEED_800;
        break;

    default:
        SX_LOG_ERR("Unexpected port oper rate %d\n", oper_rate);
        return SAI_STATUS_FAILURE;
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_supported_speeds_get_sp(_In_ sx_port_log_id_t sx_port,
                                                      _Out_ uint32_t       *speeds,
                                                      _Inout_ uint32_t     *speeds_count)
{
    sx_status_t          sx_status;
    sx_port_capability_t speed_cap;
    uint32_t             speeds_count_tmp = 0;

    assert(speeds);
    assert(speeds_count && (*speeds_count >= MAX_NUM_PORT_SPEEDS));

    sx_status = sx_api_port_capability_get(gh_sdk, sx_port, &speed_cap);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to get port speed capability - %s.\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    if (speed_cap.speed_capability.mode_100GB_CR4 || speed_cap.speed_capability.mode_100GB_SR4 ||
        speed_cap.speed_capability.mode_100GB_KR4 ||
        speed_cap.speed_capability.mode_100GB_LR4_ER4) {
        speeds[speeds_count_tmp++] = PORT_SPEED_100;
    }
    if (speed_cap.speed_capability.mode_25GB_CR || speed_cap.speed_capability.mode_25GB_KR ||
        speed_cap.speed_capability.mode_25GB_SR) {
        speeds[speeds_count_tmp++] = PORT_SPEED_25;
    }
    if (speed_cap.speed_capability.mode_50GB_CR2 || speed_cap.speed_capability.mode_50GB_KR2 ||
        speed_cap.speed_capability.mode_50GB_SR2) {
        speeds[speeds_count_tmp++] = PORT_SPEED_50;
    }
    if (speed_cap.speed_capability.mode_56GB_KX4 || speed_cap.speed_capability.mode_56GB_KR4) {
        speeds[speeds_count_tmp++] = PORT_SPEED_56;
    }
    if (speed_cap.speed_capability.mode_40GB_KR4 || speed_cap.speed_capability.mode_40GB_CR4 ||
        speed_cap.speed_capability.mode_40GB_SR4 ||
        speed_cap.speed_capability.mode_40GB_LR4_ER4) {
        speeds[speeds_count_tmp++] = PORT_SPEED_40;
    }
    if (speed_cap.speed_capability.mode_20GB_KR2) {
        speeds[speeds_count_tmp++] = PORT_SPEED_20;
    }
    if (speed_cap.speed_capability.mode_10GB_KR || speed_cap.speed_capability.mode_10GB_KX4 ||
        speed_cap.speed_capability.mode_10GB_CX4_XAUI ||
        speed_cap.speed_capability.mode_10GB_CR || speed_cap.speed_capability.mode_10GB_SR ||
        speed_cap.speed_capability.mode_10GB_ER_LR) {
        speeds[speeds_count_tmp++] = PORT_SPEED_10;
    }
    if (speed_cap.speed_capability.mode_1GB_CX_SGMII || speed_cap.speed_capability.mode_1GB_KX ||
        speed_cap.speed_capability.mode_1000MB_T) {
        speeds[speeds_count_tmp++] = PORT_SPEED_1;
    }
    if (speed_cap.speed_capability.mode_100MB_TX) {
        speeds[speeds_count_tmp++] = PORT_SPEED_100M;
    }
    if (speed_cap.speed_capability.mode_10MB_T) {
        speeds[speeds_count_tmp++] = PORT_SPEED_10M;
    }

    *speeds_count = speeds_count_tmp;

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_supported_speeds_get_sp2(_In_ sx_port_log_id_t sx_port,
                                                       _Out_ uint32_t       *speeds,
                                                       _Inout_ uint32_t     *speeds_count)
{
    sx_status_t                       sx_status;
    sx_port_rate_bitmask_t            sx_admin_rate, sx_capab_rate;
    sx_port_phy_module_type_bitmask_t sx_capab_type;

    assert(speeds);
    assert(speeds_count);

    memset(&sx_admin_rate, 0, sizeof(sx_admin_rate));
    memset(&sx_capab_rate, 0, sizeof(sx_capab_rate));
    memset(&sx_capab_type, 0, sizeof(sx_capab_type));

    sx_status = sx_api_port_rate_capability_get(gh_sdk, sx_port, &sx_admin_rate, &sx_capab_rate, &sx_capab_type);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to get port %x rate - %s\n", sx_port, SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return mlnx_port_rate_bitmask_to_speeds(&sx_capab_rate, speeds, speeds_count);
}

static sai_status_t mlnx_port_speed_bitmap_apply_sp(_In_ mlnx_port_config_t *port)
{
    sai_status_t               status;
    sx_status_t                sx_status;
    sx_port_speed_capability_t sx_speed;

    assert(port);

    memset(&sx_speed, 0, sizeof(sx_speed));

    status = mlnx_port_speed_convert_bitmap_to_capability(port->speed_bitmap, &sx_speed);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to convert port %x speed bitmap %d\n", port->logical, port->speed_bitmap);
        return status;
    }

    sx_status = sx_api_port_speed_admin_set(gh_sdk, port->logical, &sx_speed);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to set port speed - %s.\n", SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_speed_bitmap_apply_sp2(_In_ mlnx_port_config_t *port)
{
    sai_status_t           status;
    sx_status_t            sx_status;
    sx_port_rate_bitmask_t sx_rate_bitmask;

    assert(port);

    status = mlnx_port_speed_bitmap_to_rate_bitmask(port->speed_bitmap, &sx_rate_bitmask);
    if (SAI_ERR(status)) {
        return status;
    }

    sx_status = sx_api_port_rate_set(gh_sdk, port->logical, &sx_rate_bitmask);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to set port %x rate - %s\n", port->logical, SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_wred_mirror_set_sp(_In_ sx_port_log_id_t     sx_port,
                                                 _In_ sx_span_session_id_t sx_session,
                                                 _In_ bool                 is_add)
{
    sx_status_t     sx_status;
    sx_access_cmd_t cmd = is_add ? SX_ACCESS_CMD_ADD : SX_ACCESS_CMD_DELETE;

    sx_status = sx_api_cos_redecn_mirroring_set(gh_sdk, cmd, sx_port, sx_session);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Error setting cos redecn mirror for port 0x%x: %s\n",
                   sx_port, SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_wred_mirror_set_sp2(_In_ sx_port_log_id_t     sx_port,
                                                  _In_ sx_span_session_id_t sx_session,
                                                  _In_ bool                 is_add)
{
    sx_status_t                    sx_status;
    sx_span_mirror_enable_object_t enable_object;
    sx_span_mirror_enable_attr_t   enable_attr;
    sx_access_cmd_t                enable_cmd = is_add ? SX_ACCESS_CMD_SET : SX_ACCESS_CMD_DELETE;
    uint32_t                       tc;

    assert(sx_session < SPAN_SESSION_MAX);

    memset(&enable_object, 0, sizeof(enable_object));
    memset(&enable_attr, 0, sizeof(enable_attr));

    for (tc = 0; tc < g_resource_limits.cos_port_ets_traffic_class_max + 1; tc++) {
        enable_object.type = SX_SPAN_MIRROR_ENABLE_ING_WRED_E;
        enable_object.object.ing_wred.port = sx_port;
        enable_object.object.ing_wred.tc = tc;

        sx_status = sx_api_span_mirror_enable_set(gh_sdk, enable_cmd, &enable_object, &enable_attr);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to enable mirroring on port 0x%x tc %u - %s\n", sx_port, tc, SX_STATUS_MSG(sx_status));
            return sdk_to_sai(sx_status);
        }
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_cb_table_init(void)
{
    sx_chip_types_t chip_type;

    (void)(mlnx_port_cb_sp2);
    (void)(mlnx_port_cb_sp4);

    chip_type = g_sai_db_ptr->sx_chip_type;

    switch (chip_type) {
    case SX_CHIP_TYPE_SPECTRUM:
    case SX_CHIP_TYPE_SPECTRUM_A1:
        mlnx_port_cb = &mlnx_port_cb_sp;
        break;

    case SX_CHIP_TYPE_SPECTRUM2:
        mlnx_port_cb = &mlnx_port_cb_sp2;
        break;

    case SX_CHIP_TYPE_SPECTRUM3:
        mlnx_port_cb = &mlnx_port_cb_sp2;
        break;

    case SX_CHIP_TYPE_SPECTRUM4:
        mlnx_port_cb = &mlnx_port_cb_sp4;
        break;

    default:
        MLNX_SAI_LOG_ERR("g_sai_db_ptr->sxd_chip_type = %s\n", SX_CHIP_TYPE_STR(chip_type));
        return SAI_STATUS_FAILURE;
    }

    return SAI_STATUS_SUCCESS;
}

static sai_status_t mlnx_port_speed_get_impl(_In_ sx_port_log_id_t sx_port,
                                             _Out_ uint32_t       *oper_speed,
                                             _Out_ uint32_t       *admin_speed)
{
    assert(mlnx_port_cb);

    return mlnx_port_cb->speed_get(sx_port, oper_speed, admin_speed);
}

static sai_status_t mlnx_port_supported_speeds_get_impl(_In_ sx_port_log_id_t sx_port, _Inout_ sai_u32_list_t *list)
{
    sai_status_t status;
    uint32_t     speeds[MAX_NUM_PORT_SPEEDS] = {0}, speeds_count = MAX_NUM_PORT_SPEEDS;

    assert(list);
    assert(mlnx_port_cb);

    status = mlnx_port_cb->supported_speeds_get(sx_port, speeds, &speeds_count);
    if (SAI_ERR(status)) {
        return status;
    }

    return mlnx_fill_u32list(speeds, speeds_count, list);
}

sai_status_t mlnx_port_wred_mirror_set_impl(_In_ sx_port_log_id_t     sx_port,
                                            _In_ sx_span_session_id_t sx_session,
                                            _In_ bool                 is_add)
{
    assert(mlnx_port_cb);

    return mlnx_port_cb->wred_mirror_set(sx_port, sx_session, is_add);
}

sai_status_t mlnx_port_crc_params_apply(const mlnx_port_config_t *port, bool init)
{
    sx_status_t          sx_status;
    sx_port_crc_params_t crc_params;

    if (mlnx_chip_is_spc2or3or4()) {
        SX_LOG_NTC("%s is not executed on SPC2\n", __FUNCTION__);
        return SAI_STATUS_SUCCESS;
    }

    if (init) {
        /* No need to apply default values on port init
         * If a port is removed/created it gets default values in SDK
         */
        if (g_sai_db_ptr->crc_check_enable && g_sai_db_ptr->crc_recalc_enable) {
            return SAI_STATUS_SUCCESS;
        }
    }

    memset(&crc_params, 0, sizeof(crc_params));

    crc_params.bad_crc_ingress_mode = g_sai_db_ptr->crc_check_enable ? SX_PORT_BAD_CRC_INGRESS_MODE_DROP :
                                      SX_PORT_BAD_CRC_INGRESS_MODE_FORWARD;
    crc_params.crc_egress_recalc_mode = g_sai_db_ptr->crc_recalc_enable ? SX_PORT_CRC_EGRESS_RECALC_MODE_ALLOW :
                                        SX_PORT_CRC_EGRESS_RECALC_MODE_PREVENT;

    sx_status = sx_api_port_crc_params_set(gh_sdk, port->logical, &crc_params);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to set crc params for port %x - %s\n", port->logical, SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_update_issu_port_db()
{
    sx_status_t      sx_status = SX_STATUS_ERROR;
    sai_status_t     sai_status = SAI_STATUS_FAILURE;
    sx_port_log_id_t log_port_list[MAX_PORTS_DB * 2];
    uint32_t         port_list_cnt = MAX_PORTS_DB * 2;
    uint32_t         ii = 0, jj = 0;
    sx_port_type_t   port_type;
    uint32_t         port_in_lag_cnt;
    uint32_t         port_db_idx;
    sx_port_log_id_t port_in_lag_list[MAX_PORTS_DB];

    SX_LOG_ENTER();

    sx_status = sx_api_port_swid_port_list_get(gh_sdk, DEFAULT_ETH_SWID, log_port_list, &port_list_cnt);
    if (SX_STATUS_SUCCESS != sx_status) {
        SX_LOG_ERR("Error getting port swid port list: %s\n",
                   SX_STATUS_MSG(sx_status));
        sai_status = sdk_to_sai(sx_status);
        return sai_status;
    }

    for (ii = 0; ii < port_list_cnt; ii++) {
        port_type = SX_PORT_TYPE_ID_GET(log_port_list[ii]);
        SX_LOG_DBG("SDK Port idx: %d, id: %x, port type: %d\n", ii, log_port_list[ii], port_type);
        if (SX_PORT_TYPE_LAG & port_type) {
            port_in_lag_cnt = MAX_PORTS;
            sx_status = sx_api_lag_port_group_get(gh_sdk, DEFAULT_ETH_SWID, log_port_list[ii], port_in_lag_list,
                                                  &port_in_lag_cnt);

            if (SX_STATUS_SUCCESS != sx_status) {
                SX_LOG_ERR("Error getting LAG port group: %s\n",
                           SX_STATUS_MSG(sx_status));
            }
            for (jj = MAX_PORTS; jj < MAX_PORTS * 2; jj++) {
                if (!mlnx_ports_db[jj].is_present && !mlnx_ports_db[jj].sdk_port_added) {
                    mlnx_ports_db[jj].sdk_port_added = true;
                    sai_status = mlnx_create_object(SAI_OBJECT_TYPE_LAG,
                                                    jj,
                                                    NULL,
                                                    &(mlnx_ports_db[jj].saiport));
                    if (SAI_ERR(sai_status)) {
                        SX_LOG_ERR("Error creating object for LAG port %x\n",
                                   log_port_list[ii]);
                    }
                    break;
                }
            }

            for (jj = 0; jj < port_in_lag_cnt; jj++) {
                sai_status = mlnx_port_idx_by_log_id(port_in_lag_list[jj], &port_db_idx);
                if (SAI_ERR(sai_status)) {
                    SX_LOG_ERR("Error getting port idx from log id %x\n", port_in_lag_list[jj]);
                }
                mlnx_ports_db[port_db_idx].before_issu_lag_id = log_port_list[ii];
            }
        } else {
            for (jj = 0; jj < MAX_PORTS; jj++) {
                if (mlnx_ports_db[jj].logical == log_port_list[ii]) {
                    mlnx_ports_db[jj].sdk_port_added = true;
                    break;
                }
            }
            if (MAX_PORTS == jj) {
                SX_LOG_ERR("Error getting port idx from log id %x\n", log_port_list[ii]);
                SX_LOG_EXIT();
                return SAI_STATUS_FAILURE;
            }
        }
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_config_init_mandatory(mlnx_port_config_t *port)
{
    sai_status_t status;
    sxd_status_t sxd_ret = SXD_STATUS_SUCCESS;
    const bool   is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                          (!g_sai_db_ptr->issu_end_called);

    assert(port != NULL);

    if (mlnx_port_is_lag(port)) {
        port->admin_state = true;
    }

    port->start_queues_index = port->index * (MAX_ETS_TC + 1);

    /* Configure regular (network) port type only */
    if (mlnx_port_is_phy(port)) {
        if (is_warmboot_init_stage) {
            sxd_ret = sxd_dpt_set_access_control(SX_DEVICE_ID, READ_ONLY);
            if (SXD_CHECK_FAIL(sxd_ret)) {
                SX_LOG_ERR("Failed to set dpt access control - %s.\n", SXD_STATUS_MSG(sxd_ret));
                return SAI_STATUS_FAILURE;
            }
        }

        if (!is_warmboot_init_stage) {
            status = sx_api_port_swid_bind_set(gh_sdk, port->logical, DEFAULT_ETH_SWID);
            if (SX_ERR(status)) {
                SX_LOG_ERR("Port swid bind %x failed - %s\n", port->logical, SX_STATUS_MSG(status));
                return sdk_to_sai(status);
            }
        }

        status = sx_api_port_init_set(gh_sdk, port->logical);
        if (SX_ERR(status)) {
            SX_LOG_ERR("Port init set %x failed - %s\n", port->logical, SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }

        if (is_warmboot_init_stage) {
            sxd_ret = sxd_dpt_set_access_control(SX_DEVICE_ID, READ_WRITE);
            if (SXD_CHECK_FAIL(sxd_ret)) {
                SX_LOG_ERR("Failed to set dpt access control - %s.\n", SXD_STATUS_MSG(sxd_ret));
                return SAI_STATUS_FAILURE;
            }
        }
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_config_init(mlnx_port_config_t *port)
{
    sx_port_admin_state_t     state = SX_PORT_ADMIN_STATUS_DOWN;
    sx_port_forwarding_mode_t fowrarding_mode;
    sai_status_t              status;
    sxd_status_t              sxd_ret = SXD_STATUS_SUCCESS;
    const bool                is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                                       (!g_sai_db_ptr->issu_end_called);

#ifdef ACS_OS
    sx_cos_port_buffer_attr_t        sx_port_reserved_buff_attr;
    sx_cos_port_shared_buffer_attr_t sx_port_shared_buff_attr;
#endif

    assert(port != NULL);

    if (mlnx_port_is_lag(port)) {
        state = SX_PORT_ADMIN_STATUS_UP;
        port->admin_state = true;
    }

    memset(&fowrarding_mode, 0, sizeof(fowrarding_mode));

    port->start_queues_index = port->index * (MAX_ETS_TC + 1);

    /* Configure regular (network) port type only */
    if (mlnx_port_is_phy(port)) {
        if (!is_warmboot_init_stage) {
            status = sx_api_port_phys_loopback_set(gh_sdk, port->logical, SX_PORT_PHYS_LOOPBACK_DISABLE);
            if (SX_ERR(status)) {
                SX_LOG_ERR("Port phys loopback set %x failed - %s\n", port->logical, SX_STATUS_MSG(status));
                return sdk_to_sai(status);
            }
        }

        /* reset MC settings on port creation, as this is set after set and the only place this settings are called,
         *  it is safe also on warmboot */
#ifdef ACS_OS
        memset(&sx_port_reserved_buff_attr, 0, sizeof(sx_port_reserved_buff_attr));
        memset(&sx_port_shared_buff_attr, 0, sizeof(sx_port_shared_buff_attr));

        sx_port_reserved_buff_attr.type = SX_COS_MULTICAST_PORT_ATTR_E;
        sx_port_reserved_buff_attr.attr.multicast_port_buff_attr.pool_id =
            g_sai_buffer_db_ptr->buffer_pool_ids.default_multicast_pool_id;
        sx_port_reserved_buff_attr.attr.multicast_port_buff_attr.size = 0;
        status = sx_api_cos_port_buff_type_set(gh_sdk,
                                               SX_ACCESS_CMD_SET,
                                               port->logical,
                                               &sx_port_reserved_buff_attr,
                                               1);
        if (SX_ERR(status)) {
            SX_LOG_ERR("Failed to set MC reserved, logical:%x - %s\n", port->logical, SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }

        sx_port_shared_buff_attr.type = SX_COS_MULTICAST_PORT_ATTR_E;
        sx_port_shared_buff_attr.attr.multicast_port_shared_buff_attr.pool_id =
            g_sai_buffer_db_ptr->buffer_pool_ids.default_multicast_pool_id;
        sx_port_shared_buff_attr.attr.multicast_port_shared_buff_attr.max.mode =
            SX_COS_BUFFER_MAX_MODE_BUFFER_UNITS_E;
        sx_port_shared_buff_attr.attr.multicast_port_shared_buff_attr.max.max.size = 0;
        status =
            sx_api_cos_port_shared_buff_type_set(gh_sdk, SX_ACCESS_CMD_SET, port->logical, &sx_port_shared_buff_attr,
                                                 1);
        if (SX_ERR(status)) {
            SX_LOG_ERR("Failed to set MC shared, logical:%x - %s\n", port->logical, SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
#endif
    }

    /* SDK default discarding, SAI default forwarding */
    status =
        mlnx_stp_port_state_set_impl(port->logical, SX_MSTP_INST_PORT_STATE_FORWARDING, mlnx_stp_get_default_stp());
    if (SX_ERR(status)) {
        return status;
    }

    /* LAG has to be enabled also in warmboot for Sonic */
    if ((!is_warmboot_init_stage) || (mlnx_port_is_lag(port))) {
        status = sx_api_port_state_set(gh_sdk, port->logical, state);
        if (SX_ERR(status)) {
            SX_LOG_ERR("Port state set %x failed - %s\n", port->logical, SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    }

    if (!is_warmboot_init_stage) {
        status = sx_api_vlan_port_pvid_set(gh_sdk, SX_ACCESS_CMD_ADD, port->logical, DEFAULT_VLAN);
        if (SX_ERR(status)) {
            SX_LOG_ERR("port pvid set %x failed - %s\n", port->logical, SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    }

    if (is_warmboot_init_stage) {
        sxd_ret = sxd_dpt_set_access_control(SX_DEVICE_ID, READ_ONLY);
        if (SXD_CHECK_FAIL(sxd_ret)) {
            SX_LOG_ERR("Failed to set dpt access control - %s.\n", SXD_STATUS_MSG(sxd_ret));
            return SAI_STATUS_FAILURE;
        }
    }

    /* SDK default ingress filter disabled
     * When ingress filter is disabled, each router port allocates 4K VID<->FID entries (svfa), which
     * consumes all resources in issu enabled mode. Therefor we always work with filter enabled.
     * In ISSU, if vlan membership is not standard, setting filter could affect traffic.
     * This API can be used AFTER VLAN membership is reconfigured (to the values pre-issu),
     * or in this case, we set it with DPT RO, so HW is not affected, but SDK will still allocate
     * regular resources and not 4K
     * Does not allow LAG member to be configured */
    if (!is_warmboot_init_stage || (0 == port->before_issu_lag_id)) {
        status = sx_api_vlan_port_ingr_filter_set(gh_sdk, port->logical, SX_INGR_FILTER_ENABLE);
        if (SX_ERR(status)) {
            SX_LOG_ERR("Port ingress filter set %x failed - %s\n", port->logical, SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    }

    if (is_warmboot_init_stage) {
        sxd_ret = sxd_dpt_set_access_control(SX_DEVICE_ID, READ_WRITE);
        if (SXD_CHECK_FAIL(sxd_ret)) {
            SX_LOG_ERR("Failed to set dpt access control - %s.\n", SXD_STATUS_MSG(sxd_ret));
            return SAI_STATUS_FAILURE;
        }
    }

    if (!is_warmboot_init_stage) {
        fowrarding_mode.packet_store = g_sai_db_ptr->packet_storing_mode;
        status = sx_api_port_forwarding_mode_set(gh_sdk, port->logical, fowrarding_mode);
        if (SX_ERR(status)) {
            SX_LOG_ERR("Failed to set port %x forwarding mode to %d - %s\n",
                       port->logical,
                       fowrarding_mode.packet_store,
                       SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    }

    port->internal_ingress_samplepacket_obj_idx = MLNX_INVALID_SAMPLEPACKET_SESSION;
    port->internal_egress_samplepacket_obj_idx = MLNX_INVALID_SAMPLEPACKET_SESSION;

    if (!mlnx_port_is_lag(port)) {
        status = mlnx_apply_descriptor_buffer_to_port(port->logical, true);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to unbind default descriptor buffer to port %x\n", port->logical);
            return status;
        }

        status = mlnx_apply_descriptor_buffer_to_port(port->logical, false);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to bind descriptor buffer to port %x\n", port->logical);
            return status;
        }
    }

    port->is_present = true;

    if (!mlnx_port_is_virt(port)) {
        if (is_warmboot_init_stage) {
            sxd_ret = sxd_dpt_set_access_control(SX_DEVICE_ID, READ_ONLY);
            if (SXD_CHECK_FAIL(sxd_ret)) {
                SX_LOG_ERR("Failed to set dpt access control - %s.\n", SXD_STATUS_MSG(sxd_ret));
                return SAI_STATUS_FAILURE;
            }
        }

        /* SDK default trust PCP, SAI default trust port
         * Sonic always sets DSCP */
        if (!is_warmboot_init_stage || (0 == port->before_issu_lag_id)) {
            status = sx_api_cos_port_trust_set(gh_sdk, port->logical, SX_COS_TRUST_LEVEL_PORT);
            if (SX_ERR(status)) {
                SX_LOG_ERR("Port trust level set %x failed - %s\n", port->logical, SX_STATUS_MSG(status));
                return sdk_to_sai(status);
            }
        } else {
            status = sx_api_cos_port_trust_set(gh_sdk, port->before_issu_lag_id, SX_COS_TRUST_LEVEL_PORT);
            if (SX_ERR(status)) {
                SX_LOG_ERR("Port trust level set %x failed - %s\n", port->before_issu_lag_id, SX_STATUS_MSG(status));
                return sdk_to_sai(status);
            }
        }

        if (is_warmboot_init_stage) {
            sxd_ret = sxd_dpt_set_access_control(SX_DEVICE_ID, READ_WRITE);
            if (SXD_CHECK_FAIL(sxd_ret)) {
                SX_LOG_ERR("Failed to set dpt access control - %s.\n", SXD_STATUS_MSG(sxd_ret));
                return SAI_STATUS_FAILURE;
            }
        }

        if (!is_warmboot_init_stage) {
            status = sx_api_port_global_fc_enable_set(gh_sdk, port->logical,
                                                      SX_PORT_FLOW_CTRL_MODE_TX_DIS_RX_DIS);
            if (SX_ERR(status)) {
                SX_LOG_ERR("Failed to init port global flow control - %s\n", SX_STATUS_MSG(status));
                return sdk_to_sai(status);
            }
        }

        /* PCP -> switch prio. SDK default i->i, SAI i->0
         * DSCP -> switch prio. SDK default i->i/8, SAI i->0
         * Switch prio -> TC. SDK default i->i, SAI i->0
         * Apply the 3 maps where SDK default different than SAI
         * When init warm boot, apply to SDK but not to HW. Applying to SDK is needed for
         * later clone. Clone sets SAI default on to port in SDK in case from map is null.
         * Therefor the from map must be set to SDK, otherwise there will be mismatch.
         * Sonic always sets DSCP -> switch prio, switch prio -> TC and doesn't
         * use PCP -> switch prio */
        if (is_warmboot_init_stage) {
            sxd_ret = sxd_dpt_set_access_control(SX_DEVICE_ID, READ_ONLY);
            if (SXD_CHECK_FAIL(sxd_ret)) {
                SX_LOG_ERR("Failed to set dpt access control - %s.\n", SXD_STATUS_MSG(sxd_ret));
                return SAI_STATUS_FAILURE;
            }
        }

        /* For ISSU initialization, a port may already be a LAG member in SDK,
         * but SAI LAG is not created yet, thus we are not able to retrieve SAI LAG entry
         * from SAI port DB, thus we skip the trust level setting */
        status = mlnx_port_qos_map_apply(port->saiport, SAI_NULL_OBJECT_ID, SAI_QOS_MAP_TYPE_DOT1P_TO_TC);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to apply dot1p to tc qos map for port %" PRIx64 "\n", port->saiport);
            return status;
        }
        status = mlnx_port_qos_map_apply(port->saiport, SAI_NULL_OBJECT_ID, SAI_QOS_MAP_TYPE_DSCP_TO_TC);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to apply dscp to tc qos map for port %" PRIx64 "\n", port->saiport);
            return status;
        }
        status = mlnx_port_qos_map_apply(port->saiport, SAI_NULL_OBJECT_ID, SAI_QOS_MAP_TYPE_TC_TO_QUEUE);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to apply tc to queue qos map for port %" PRIx64 "\n", port->saiport);
            return status;
        }

        if (is_warmboot_init_stage) {
            sxd_ret = sxd_dpt_set_access_control(SX_DEVICE_ID, READ_WRITE);
            if (SXD_CHECK_FAIL(sxd_ret)) {
                SX_LOG_ERR("Failed to set dpt access control - %s.\n", SXD_STATUS_MSG(sxd_ret));
                return SAI_STATUS_FAILURE;
            }
        }

        status = mlnx_sched_group_port_init(port, is_warmboot_init_stage);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed init SAI scheduler groups for log port 0x%x\n", port->logical);
            return status;
        }
    }

    if (mlnx_port_is_phy(port)) {
        status = mlnx_check_and_store_ar_port_id(port);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed check adaptive routing enable for log port 0x%x\n", port->logical);
            return status;
        }
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_add(mlnx_port_config_t *port, bool is_lag)
{
    sai_status_t status;
    const bool   is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                          (!g_sai_db_ptr->issu_end_called);

    /* For ISSU, all non-LAG physical ports has called mlnx_port_config_init_mandatory
     * during dvs_stage, thus no need to call again */
    if (!is_warmboot_init_stage || is_lag) {
        status = mlnx_port_config_init_mandatory(port);
        if (SAI_ERR(status)) {
            return status;
        }
    }

    status = mlnx_port_config_init(port);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_acl_port_lag_event_handle_locked(port, ACL_EVENT_TYPE_PORT_LAG_ADD);
    if (SAI_ERR(status)) {
        return status;
    }

    return SAI_STATUS_SUCCESS;
}

static void mlnx_port_reset_buffer_refs(uint32_t *buff_refs, uint32_t count)
{
    uint32_t ii;

    for (ii = 0; ii < count; ii++) {
        buff_refs[ii] = SENTINEL_BUFFER_DB_ENTRY_INDEX;
    }
}

sai_status_t mlnx_port_config_uninit(mlnx_port_config_t *port)
{
    mlnx_policer_bind_params bind_params;
    sx_port_mapping_t        port_map;
    sai_status_t             status;
    sx_status_t              sx_status;
    sx_vid_t                 pvid;
    bool                     is_internal_acls_rollback_needed = false;
    internal_acl_op_types    op_type;
    const bool               is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                                      (!g_sai_db_ptr->issu_end_called);

    /* Reset Policer's */
    bind_params.port_policer_type = MLNX_PORT_POLICER_TYPE_FLOOD_INDEX;
    status = mlnx_sai_unbind_policer_from_port(port->saiport, &bind_params);
    if (SAI_ERR(status)) {
        return status;
    }
    bind_params.port_policer_type = MLNX_PORT_POLICER_TYPE_BROADCAST_INDEX;
    status = mlnx_sai_unbind_policer_from_port(port->saiport, &bind_params);
    if (SAI_ERR(status)) {
        return status;
    }
    bind_params.port_policer_type = MLNX_PORT_POLICER_TYPE_MULTICAST_INDEX;
    status = mlnx_sai_unbind_policer_from_port(port->saiport, &bind_params);
    if (SAI_ERR(status)) {
        return status;
    }
    bind_params.port_policer_type = MLNX_PORT_POLICER_TYPE_REGULAR_INDEX;
    status = mlnx_sai_unbind_policer_from_port(port->saiport, &bind_params);
    if (SAI_ERR(status)) {
        return status;
    }


    if (!is_warmboot_init_stage) {
        status = sx_api_vlan_port_pvid_get(gh_sdk, port->logical, &pvid);
        if (!SX_ERR(status)) {
            status = sx_api_vlan_port_pvid_set(gh_sdk, SX_ACCESS_CMD_DELETE, port->logical, pvid);
            if (SX_ERR(status) && (status != SX_STATUS_ENTRY_NOT_FOUND)) {
                SX_LOG_ERR("Port pvid un-set %x failed - %s\n", port->logical, SX_STATUS_MSG(status));
                return sdk_to_sai(status);
            }
        }

        status = mlnx_port_egress_block_clear(port->logical);
        if (SAI_ERR(status)) {
            return status;
        }

        status =
            mlnx_stp_port_state_set_impl(port->logical, SX_MSTP_INST_PORT_STATE_FORWARDING,
                                         mlnx_stp_get_default_stp());
        if (SX_ERR(status)) {
            return status;
        }
    }
    if (!is_warmboot_init_stage) {
        op_type = mlnx_port_is_lag(port) ? INTERNAL_ACL_OP_DEL_LAG : INTERNAL_ACL_OP_DEL_PORT;
        status = mlnx_internal_acls_bind(op_type, port->saiport);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to unbind internal ACLs to port 0x%x\n", port->logical);
            return status;
        }
        is_internal_acls_rollback_needed = true;
    }

    if (mlnx_port_is_phy(port)) {
        if (!is_warmboot_init_stage) {
            sx_status = sx_api_port_deinit_set(gh_sdk, port->logical);
            if (SX_ERR(sx_status)) {
                SX_LOG_ERR("Port de-init set %x failed - %s\n", port->logical, SX_STATUS_MSG(sx_status));
                status = sdk_to_sai(sx_status);
                goto out;
            }

            sx_status = sx_api_port_swid_bind_set(gh_sdk, port->logical, SX_SWID_ID_DISABLED);
            if (SX_ERR(sx_status)) {
                SX_LOG_ERR("Port swid bind %x failed - %s\n", port->logical, SX_STATUS_MSG(sx_status));
                status = sdk_to_sai(sx_status);
                goto out;
            }
        }

        memset(&port_map, 0, sizeof(port_map));
        port_map.mapping_mode = SX_PORT_MAPPING_MODE_DISABLE;
        port_map.local_port = port->port_map.local_port;
        port_map.module_port = port->module;
        port_map.lane_bmap = 0x0;

        if (!is_warmboot_init_stage) {
            sx_status = sx_api_port_mapping_set(gh_sdk, &port->logical, &port_map, 1);
            if (SX_ERR(sx_status)) {
                SX_LOG_ERR("Set disable port mapping %x failed - %s\n", port->logical, SX_STATUS_MSG(sx_status));
                status = sdk_to_sai(sx_status);
                goto out;
            }
        }
    }

    port->is_present = false;

    if (!SAI_ERR(status)) {
        uint32_t                *buff_refs = NULL;
        uint32_t                 buff_count = 0;
        mlnx_qos_queue_config_t *queue;
        uint32_t                 ii;

        /* Reset QoS */
        memset(&port->sched_hierarchy, 0, sizeof(port->sched_hierarchy));
        memset(port->qos_maps, 0, sizeof(port->qos_maps));
        port->scheduler_id = SAI_NULL_OBJECT_ID;

        port->internal_ingress_samplepacket_obj_idx = MLNX_INVALID_SAMPLEPACKET_SESSION;
        port->internal_egress_samplepacket_obj_idx = MLNX_INVALID_SAMPLEPACKET_SESSION;

        if (mlnx_port_is_phy(port)) {
            uint32_t port_index = mlnx_port_idx_get(port);

            /* Reset buffers */
            /* PG Buff's */
            status = mlnx_sai_get_port_buffer_index_array(port_index, PORT_BUFF_TYPE_PG, &buff_refs);
            if (SAI_ERR(status)) {
                goto out;
            }
            buff_count = mlnx_sai_get_buffer_resource_limits()->num_port_pg_buff;
            mlnx_port_reset_buffer_refs(buff_refs, buff_count);

            /* Ingress Buff's */
            status = mlnx_sai_get_port_buffer_index_array(port_index, PORT_BUFF_TYPE_INGRESS, &buff_refs);
            if (SAI_ERR(status)) {
                goto out;
            }
            buff_count = mlnx_sai_get_buffer_resource_limits()->num_ingress_pools;
            mlnx_port_reset_buffer_refs(buff_refs, buff_count);

            /* Egress Buff's */
            status = mlnx_sai_get_port_buffer_index_array(port_index, PORT_BUFF_TYPE_EGRESS, &buff_refs);
            if (SAI_ERR(status)) {
                goto out;
            }
            buff_count = mlnx_sai_get_buffer_resource_limits()->num_egress_pools;
            mlnx_port_reset_buffer_refs(buff_refs, buff_count);
        }

        /* Reset port's queues */
        port_queues_foreach(port, queue, ii) {
            queue->buffer_id = SAI_NULL_OBJECT_ID;
            queue->wred_id = SAI_NULL_OBJECT_ID;
        }
    }

out:
    if (SAI_ERR(status)) {
        if (is_internal_acls_rollback_needed) {
            op_type = mlnx_port_is_lag(port) ? INTERNAL_ACL_OP_ADD_LAG : INTERNAL_ACL_OP_ADD_PORT;
            mlnx_internal_acls_bind(op_type, port->saiport);
        }
    }
    return status;
}

sai_status_t mlnx_port_del(mlnx_port_config_t *port)
{
    sai_status_t status;
    const bool   is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                          (!g_sai_db_ptr->issu_end_called);

    if (!is_warmboot_init_stage) {
        status = sx_api_port_state_set(gh_sdk, port->logical, SX_PORT_ADMIN_STATUS_DOWN);
        if (SX_ERR(status)) {
            SX_LOG_ERR("Set port %x down failed - %s\n", port->logical, SX_STATUS_MSG(status));
            return sdk_to_sai(status);
        }
    }

    status = mlnx_acl_port_lag_event_handle_unlocked(port, ACL_EVENT_TYPE_PORT_LAG_DEL);
    if (SAI_ERR(status)) {
        return status;
    }

    status = mlnx_port_config_uninit(port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed un-init port %x\n", port->logical);
        return status;
    }

    return SAI_STATUS_SUCCESS;
}

#define SPAN_MAX_COUNT \
    (g_resource_limits.span_session_id_max_internal + g_resource_limits.span_session_id_max_external)

sai_status_t mlnx_port_isolation_is_in_use(const mlnx_port_config_t *port, bool *is_in_use)
{
    sai_status_t status;

    if (is_egress_block_in_use()) {
        status = mlnx_port_egress_block_is_in_use(port->logical, is_in_use);
        if (SAI_ERR(status)) {
            return status;
        }
    } else if (is_isolation_group_in_use()) {
        *is_in_use = port->isolation_group_port_refcount > 0;
    } else {
        *is_in_use = false;
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_in_use_check(const mlnx_port_config_t *port)
{
    sai_status_t status = SAI_STATUS_SUCCESS;
    bool         is_in_use_for_port_isolation = true;

    if (mlnx_port_is_in_bridge_1q(port)) {
        SX_LOG_ERR("Failed remove port oid %" PRIx64 " - is under bridge\n", port->saiport);
        return SAI_STATUS_OBJECT_IN_USE;
    }
    if (port->lag_id) {
        SX_LOG_ERR("Failed remove port oid %" PRIx64 " - is a LAG member\n", port->saiport);
        return SAI_STATUS_OBJECT_IN_USE;
    }
    if (port->rifs) {
        SX_LOG_ERR("Failed remove port oid %" PRIx64 " - is a router interface\n", port->saiport);
        return SAI_STATUS_OBJECT_IN_USE;
    }

    if (port->acl_refs > 0) {
        SX_LOG_ERR("Failed remove port oid %" PRIx64 " - is in use for %d ACL field(s)/action(s)\n",
                   port->saiport, port->acl_refs);
        return SAI_STATUS_OBJECT_IN_USE;
    }

    if (port->hostif_table_refcount > 0) {
        SX_LOG_ERR("Failed remove port oid %" PRIx64 " - is used for hostif table entry\n",
                   port->saiport);
        return SAI_STATUS_OBJECT_IN_USE;
    }

    if (mlnx_port_is_lag(port)) {
        return SAI_STATUS_SUCCESS;
    }

    if (port->is_span_analyzer_port) {
        SX_LOG_ERR("Failed remove port oid %" PRIx64 " - is Mirror analyzer(monitor) port\n", port->saiport);
        return SAI_STATUS_OBJECT_IN_USE;
    }

    status = mlnx_port_isolation_is_in_use(port, &is_in_use_for_port_isolation);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to check port isolation usage\n");
        return status;
    }

    if (is_in_use_for_port_isolation) {
        SX_LOG_ERR("Failed remove port oid %" PRIx64 " - is a member another port's EGRESS_BLOCK_LISTS\n",
                   port->saiport);
        return SAI_STATUS_OBJECT_IN_USE;
    }

    return status;
}

static inline uint32_t lane2module(uint32_t lane)
{
    return lane / mlnx_port_max_lanes_get();
}

static inline uint32_t sai_lane2phy_lane(uint32_t lane)
{
    return (lane - lane2module(lane) * mlnx_port_max_lanes_get());
}

static mlnx_port_config_t * mlnx_port_by_module(uint32_t module)
{
    mlnx_port_config_t *port;
    uint32_t            ii;

    mlnx_port_local_foreach(port, ii) {
        if (port->width && (port->module == module)) {
            return port;
        }
    }

    return NULL;
}

/* TODO: replace with cap_num_local_ports_in_2x when available */
static uint32_t mlnx_platform_num_local_ports_in_2x_get(void)
{
    /* SPC2 non gearbox is step 2, all the rest 1 */
    if (mlnx_chip_is_spc2()) {
        if (g_sai_db_ptr->platform_type != MLNX_PLATFORM_TYPE_3800) {
            return 2;
        }
    }

    return 1;
}

/* TODO: replace with cap_num_local_ports_in_4x when available */
static uint32_t mlnx_platform_num_local_ports_in_4x_get(void)
{
    /* SPC2 non gearbox is step 4 all the rest 2 */
    if (mlnx_chip_is_spc2()) {
        if (g_sai_db_ptr->platform_type != MLNX_PLATFORM_TYPE_3800) {
            return 4;
        }
    }

    return 2;
}

static uint32_t mlnx_platform_num_local_ports_in_8x_get(void)
{
    /* SPC2 non gearbox is step 8 all the rest 4 */
    if (mlnx_chip_is_spc2()) {
        if (g_sai_db_ptr->platform_type != MLNX_PLATFORM_TYPE_3800) {
            return 8;
        }
    }

    return 4;
}
/* module -> port local idx
 * Spectrum 1 : (1, 2) / (1, 2, 3, 4)
 * Spectrum 2 : (1, 2) or (1, 3) / (1, 2, 3, 4)
 */
static mlnx_port_config_t * mlnx_port_split_idx_to_local_port(_In_ const mlnx_port_config_t *father,
                                                              _In_ uint32_t                  base_lane_idx,
                                                              _In_ uint32_t                  lane_count)
{
    uint32_t step;

    if (lane_count == 2) {
        step = mlnx_platform_num_local_ports_in_2x_get();
    } else if (lane_count == 4) {
        step = mlnx_platform_num_local_ports_in_4x_get();
    } else if (lane_count == 8) {
        step = mlnx_platform_num_local_ports_in_8x_get();
    } else {
        step = 1;
    }

    if ((father->port_map.local_port + base_lane_idx * step) > MAX_PORTS) {
        SX_LOG_ERR("Failed to find a port by local port %u\n", father->port_map.local_port + base_lane_idx * step);
        return NULL;
    }

    return mlnx_port_by_local_id(father->port_map.local_port + base_lane_idx * step);
}

static mlnx_port_config_t * sai_lane2child_port(mlnx_port_config_t *father, const sai_u32_list_t *lanes)
{
    uint32_t new_port_idx = sai_lane2phy_lane(lanes->list[0]) / lanes->count;

    return mlnx_port_split_idx_to_local_port(father, new_port_idx, lanes->count);
}

sai_status_t mlnx_port_auto_split(mlnx_port_config_t *port)
{
    uint8_t  lanes_per_port = port->port_map.width / port->split_count;
    uint8_t  orig_lanes = port->port_map.lane_bmap;
    uint32_t ii, ll;

    SX_LOG_NTC("Splitting local port 0x%x to %u ...\n", port->port_map.local_port, port->split_count);

    for (ii = 0; ii < port->split_count; ii++) {
        mlnx_port_config_t *new_port;

        new_port = mlnx_port_split_idx_to_local_port(port, ii, lanes_per_port);
        if (!new_port) {
            return SAI_STATUS_FAILURE;
        }

        new_port->port_map.lane_bmap = 0x0;
        new_port->port_map.width = 0;

        /* Borrow lanes from the initial port */
        for (ll = 0; ll < mlnx_port_max_lanes_get(); ll++) {
            if (new_port->port_map.width < lanes_per_port) {
                if (orig_lanes & (1 << ll)) {
                    new_port->port_map.lane_bmap |= (1 << ll);
                    new_port->port_map.width++;
                    orig_lanes &= ~(1 << ll);
                }
            }
        }

        if (!new_port->is_present) {
            g_sai_db_ptr->ports_number++;
        }

        /* Inherit module & speed from the init port */
        new_port->port_map.mapping_mode = SX_PORT_MAPPING_MODE_ENABLE;
        new_port->speed_bitmap = port->speed_bitmap;
        new_port->port_map.module_port = port->module;
        new_port->is_present = true;
        new_port->is_split = true;
        new_port->auto_neg = AUTO_NEG_DEFAULT;
        new_port->intf = SAI_PORT_INTERFACE_TYPE_NONE;
    }

    return SAI_STATUS_SUCCESS;
}

/**
 * Routine Description:
 *    @brief Create port
 *
 * Arguments:
 *    @param[out] port_id - port id
 *    @param[in] attr_count - number of attributes
 *    @param[in] attr_list - array of attributes
 *
 * Return Values:
 *    @return SAI_STATUS_SUCCESS on success
 *            Failure status code on error
 *
 */
static sai_status_t mlnx_create_port(_Out_ sai_object_id_t     * port_id,
                                     _In_ sai_object_id_t        switch_id,
                                     _In_ uint32_t               attr_count,
                                     _In_ const sai_attribute_t *attr_list)
{
    const sai_attribute_value_t *lanes_list = NULL;
    const sai_attribute_value_t *attr_ing_acl = NULL;
    const sai_attribute_value_t *attr_egr_acl = NULL;
    const sai_attribute_value_t *egress_block_list = NULL;
    const sai_attribute_value_t *isolation_group = NULL;
    const sai_attribute_value_t *value = NULL;

/* TODO : FEC is not supported on Simx/PLD(main model), add when ready */
#if !defined IS_PLD && !defined IS_SIMX
    uint32_t                     fec_index;
    const sai_attribute_value_t *fec;
#endif
    uint32_t            lane_index, acl_attr_index;
    uint32_t            lanes_count, egress_block_list_index, isolation_group_index;
    uint32_t            index;
    sx_port_log_id_t   *sx_egress_block_port_list = NULL;
    mlnx_port_config_t *father_port;
    mlnx_port_config_t *new_port = NULL;
    sx_port_mapping_t  *port_map;
    sai_status_t        status;
    sx_status_t         sx_status;
    acl_index_t         ing_acl_index = ACL_INDEX_INVALID, egr_acl_index = ACL_INDEX_INVALID;
    uint32_t            module;
    uint32_t            ii;
    const bool          is_warmboot_init_stage = (BOOT_TYPE_WARM == g_sai_db_ptr->boot_type) &&
                                                 (!g_sai_db_ptr->issu_end_called);

    SX_LOG_ENTER();

    status = check_attribs_on_create(attr_count, attr_list, SAI_OBJECT_TYPE_PORT, port_id);
    if (SAI_ERR(status)) {
        return status;
    }
    MLNX_LOG_ATTRS(attr_count, attr_list,  SAI_OBJECT_TYPE_PORT);

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_HW_LANE_LIST, &lanes_list, &lane_index);
    if (SAI_ERR(status)) {
        goto out;
    }

    lanes_count = lanes_list->u32list.count;

    if ((lanes_count == 0) || (lanes_count == 3)) {
        SX_LOG_ERR("Port HW lanes count %u is invalid (supported only 1,2,4)\n", lanes_count);
        status = SAI_STATUS_INVALID_PARAMETER;
        goto out;
    }
    if (lanes_count > mlnx_port_max_lanes_get()) {
        SX_LOG_ERR("Port HW lanes count %u is bigger than %u\n", lanes_count, mlnx_port_max_lanes_get());
        status = SAI_STATUS_INVALID_PARAMETER;
        goto out;
    }

    module = lane2module(lanes_list->u32list.list[0]);

    /* Validate lanes list */
    for (ii = 0; ii < lanes_count; ii++) {
        if ((ii > 0) && (lane2module(lanes_list->u32list.list[ii]) != module)) {
            SX_LOG_ERR("Port HW lanes belongs to the different modules\n");
            status = SAI_STATUS_INVALID_PARAMETER;
            goto out;
        }
    }

    sai_db_write_lock();
    acl_global_lock();

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_INGRESS_ACL,
                                 &attr_ing_acl, &acl_attr_index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_acl_bind_point_attrs_check_and_fetch(attr_ing_acl->oid,
                                                           MLNX_ACL_BIND_POINT_TYPE_INGRESS_PORT,
                                                           acl_attr_index,
                                                           &ing_acl_index);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_EGRESS_ACL,
                                 &attr_egr_acl, &acl_attr_index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_acl_bind_point_attrs_check_and_fetch(attr_egr_acl->oid,
                                                           MLNX_ACL_BIND_POINT_TYPE_EGRESS_PORT,
                                                           acl_attr_index,
                                                           &egr_acl_index);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    father_port = mlnx_port_by_module(module);
    if (!father_port) {
        SX_LOG_ERR("Failed to find father's port by module %u\n", module);
        status = SAI_STATUS_INVALID_PARAMETER;
        goto out_unlock;
    }

    new_port = sai_lane2child_port(father_port, &lanes_list->u32list);
    if (!new_port) {
        status = SAI_STATUS_FAILURE;
        goto out_unlock;
    }

    if (new_port->is_present) {
        SX_LOG_ERR("Failed create port - lanes already allocated by port oid %lx (local %u, module %u)\n",
                   new_port->saiport, new_port->port_map.local_port, new_port->module);
        status = SAI_STATUS_INVALID_PARAMETER;
        goto out_unlock;
    }

    port_map = &new_port->port_map;

    port_map->mapping_mode = SX_PORT_MAPPING_MODE_ENABLE;
    port_map->module_port = father_port->module;
    port_map->width = lanes_count;
    port_map->lane_bmap = 0x0;

    /* Map local lanes to the new port */
    for (ii = 0; ii < lanes_count; ii++) {
        port_map->lane_bmap |= 1 << sai_lane2phy_lane(lanes_list->u32list.list[ii]);
    }

    if (!is_warmboot_init_stage) {
        status = sx_api_port_mapping_set(gh_sdk, &new_port->logical, port_map, 1);
        if (SX_ERR(status)) {
            SX_LOG_ERR(
                "Failed enable port mapping (lane_bmap 0x%x,  width %u, module %u, local port %u) for port %x - %s\n",
                port_map->lane_bmap,
                port_map->width,
                port_map->module_port,
                port_map->local_port,
                new_port->logical,
                SX_STATUS_MSG(status));
            status = sdk_to_sai(status);
            goto out_unlock;
        }
    }

    SX_LOG_NTC("Initialize new port oid %" PRIx64 "\n", new_port->saiport);

    status = mlnx_port_add(new_port, false);
    if (SAI_ERR(status)) {
        goto out_unlock;
    }
    g_sai_db_ptr->ports_number++;

    /* If split port is already a LAG member, do not apply hash config to the split port,
     * because hash config has already been applied to LAG */
    if (!is_warmboot_init_stage || (0 == new_port->before_issu_lag_id)) {
        status = mlnx_internal_acls_bind(INTERNAL_ACL_OP_ADD_PORT, new_port->saiport);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to bind internal ACLs to port 0x%x\n", new_port->logical);
            goto out_unlock;
        }

        status = mlnx_hash_config_apply_to_port(new_port->logical);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to apply hash config on port %x\n", new_port->logical);
            goto out_unlock;
        }
    }

    status = mlnx_port_crc_params_apply(new_port, true);
    if (SAI_ERR(status)) {
        goto out_unlock;
    }

    if (attr_ing_acl) {
        status = mlnx_acl_port_lag_rif_bind_point_set(new_port->saiport, MLNX_ACL_BIND_POINT_TYPE_INGRESS_PORT,
                                                      ing_acl_index);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
        new_port->ingress_acl_index = ing_acl_index;
    }

    if (attr_egr_acl) {
        status = mlnx_acl_port_lag_rif_bind_point_set(new_port->saiport, MLNX_ACL_BIND_POINT_TYPE_EGRESS_PORT,
                                                      egr_acl_index);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
        new_port->egress_acl_index = egr_acl_index;
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_SPEED, &value, &index);
    if (SAI_ERR(status)) {
        goto out_unlock;
    }
    new_port->speed = value->u32;

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_AUTO_NEG_MODE, &value, &index);
    if (SAI_OK(status)) {
        new_port->auto_neg = value->booldata ? AUTO_NEG_ENABLE : AUTO_NEG_DISABLE;
    } else {
        new_port->auto_neg = AUTO_NEG_DEFAULT;
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_INTERFACE_TYPE, &value, &index);
    if (SAI_OK(status)) {
        new_port->intf = value->u32;
    } else {
        new_port->intf = SAI_PORT_INTERFACE_TYPE_NONE;
    }

    new_port->adv_speeds_num = 0;
    memset(new_port->adv_speeds, 0, sizeof(new_port->adv_speeds));
    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_ADVERTISED_SPEED, &value, &index);
    if (SAI_OK(status)) {
        new_port->adv_speeds_num = value->u32list.count;
        for (uint32_t ii = 0; ii < value->u32list.count; ++ii) {
            new_port->adv_speeds[ii] = value->u32list.list[ii];
        }
    }

    new_port->adv_intfs_num = 0;
    memset(new_port->adv_intfs, 0, sizeof(new_port->adv_intfs));
    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_ADVERTISED_INTERFACE_TYPE, &value, &index);
    if (SAI_OK(status)) {
        new_port->adv_intfs_num = value->s32list.count;
        for (uint32_t ii = 0; ii < value->s32list.count; ++ii) {
            new_port->adv_intfs[ii] = value->s32list.list[ii];
        }
    }

    if (!is_warmboot_init_stage) {
        status = mlnx_port_update_speed(new_port);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to update speed.\n");
            goto out_unlock;
        }
    }

    status = mlnx_wred_mirror_port_event(new_port->logical, true);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Error setting port mirror wred discard for new port 0x%x\n", new_port->logical);
        goto out_unlock;
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_ADMIN_STATE, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        sx_status = sx_api_port_state_set(gh_sdk,
                                          new_port->logical,
                                          value->booldata ? SX_PORT_ADMIN_STATUS_UP : SX_PORT_ADMIN_STATUS_DOWN);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to set port admin state - %s.\n", SX_STATUS_MSG(sx_status));
            status = sdk_to_sai(sx_status);
            goto out_unlock;
        }
        new_port->admin_state = value->booldata;
    }

    /* on warmboot control PG buffer and lossy PG buffers are handled just before issu end */
    if (!is_warmboot_init_stage) {
        status = mlnx_sai_buffer_update_port_buffers_internal(new_port);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to update lossy PG0 and control PG9 buffers on port creation.\n");
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_PORT_VLAN_ID, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        sx_status = sx_api_vlan_port_pvid_set(gh_sdk, SX_ACCESS_CMD_ADD, new_port->logical, value->u16);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to set %s %x pvid - %s.\n", mlnx_port_type_str(new_port),
                       new_port->logical, SX_STATUS_MSG(sx_status));
            status = sdk_to_sai(sx_status);
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_DEFAULT_VLAN_PRIORITY, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        sx_status = sx_api_cos_port_default_prio_set(gh_sdk, new_port->logical, value->u8);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to set port default prio - %s.\n", SX_STATUS_MSG(sx_status));
            status = sdk_to_sai(sx_status);
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_DROP_UNTAGGED, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_lag_drop_tags_set_impl(new_port, SAI_PORT_ATTR_DROP_UNTAGGED, value->booldata);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_DROP_TAGGED, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_lag_drop_tags_set_impl(new_port, SAI_PORT_ATTR_DROP_TAGGED, value->booldata);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_INTERNAL_LOOPBACK_MODE, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_internal_loopback_set_impl(new_port->logical, value->s32);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_UPDATE_DSCP, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_update_dscp_set_impl(new_port->logical, value->booldata);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_MTU, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        sx_status = sx_api_port_mtu_set(gh_sdk, new_port->logical, (sx_port_mtu_t)value->u32);
        if (SX_ERR(sx_status)) {
            SX_LOG_ERR("Failed to set port mtu - %s.\n", SX_STATUS_MSG(sx_status));
            status = sdk_to_sai(sx_status);
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_FLOOD_STORM_CONTROL_POLICER_ID, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_sai_policer_bind_set_impl(new_port->saiport, value->oid, MLNX_PORT_POLICER_TYPE_FLOOD_INDEX);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count,
                                 attr_list,
                                 SAI_PORT_ATTR_BROADCAST_STORM_CONTROL_POLICER_ID,
                                 &value,
                                 &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_sai_policer_bind_set_impl(new_port->saiport, value->oid, MLNX_PORT_POLICER_TYPE_BROADCAST_INDEX);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count,
                                 attr_list,
                                 SAI_PORT_ATTR_MULTICAST_STORM_CONTROL_POLICER_ID,
                                 &value,
                                 &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_sai_policer_bind_set_impl(new_port->saiport, value->oid, MLNX_PORT_POLICER_TYPE_MULTICAST_INDEX);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_GLOBAL_FLOW_CONTROL_MODE, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_global_flow_ctrl_set_impl(new_port->logical, value->s32);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_INGRESS_MIRROR_SESSION, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status =
            mlnx_port_mirror_session_set_impl(new_port->logical, (sx_mirror_direction_t)MIRROR_INGRESS_PORT, value);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_EGRESS_MIRROR_SESSION, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status =
            mlnx_port_mirror_session_set_impl(new_port->logical, (sx_mirror_direction_t)MIRROR_EGRESS_PORT, value);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_INGRESS_SAMPLEPACKET_ENABLE, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_samplepacket_session_set_impl(new_port, new_port->logical, value->oid);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_POLICER_ID, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_sai_policer_bind_set_impl(new_port->saiport, value->oid, MLNX_PORT_POLICER_TYPE_REGULAR_INDEX);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_DEFAULT_TC, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_default_tc_set_impl(new_port, new_port->logical, value->u8);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_DOT1P_TO_TC_MAP, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_map_id_set_impl(new_port->saiport, value->oid, SAI_QOS_MAP_TYPE_DOT1P_TO_TC);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_DOT1P_TO_COLOR_MAP, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_map_id_set_impl(new_port->saiport, value->oid, SAI_QOS_MAP_TYPE_DOT1P_TO_COLOR);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_DSCP_TO_TC_MAP, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_map_id_set_impl(new_port->saiport, value->oid, SAI_QOS_MAP_TYPE_DSCP_TO_TC);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_DSCP_TO_COLOR_MAP, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_map_id_set_impl(new_port->saiport, value->oid, SAI_QOS_MAP_TYPE_DSCP_TO_COLOR);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DOT1P_MAP, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_map_id_set_impl(new_port->saiport, value->oid, SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DOT1P);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_TC_AND_COLOR_TO_DSCP_MAP, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_map_id_set_impl(new_port->saiport, value->oid, SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_TC_TO_QUEUE_MAP, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_map_id_set_impl(new_port->saiport, value->oid, SAI_QOS_MAP_TYPE_TC_TO_QUEUE);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_TC_TO_PRIORITY_GROUP_MAP, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_map_id_set_impl(new_port->saiport, value->oid, SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count,
                                 attr_list,
                                 SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_PRIORITY_GROUP_MAP,
                                 &value,
                                 &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_map_id_set_impl(new_port->saiport,
                                               value->oid,
                                               SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_PRIORITY_GROUP);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_PFC_PRIORITY_TO_QUEUE_MAP, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_qos_map_id_set_impl(new_port->saiport, value->oid, SAI_QOS_MAP_TYPE_PFC_PRIORITY_TO_QUEUE);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_SCHEDULER_PROFILE_ID, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_scheduler_to_port_apply_unlocked(value->oid, new_port->saiport);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_INGRESS_BUFFER_PROFILE_LIST, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_buffer_port_profile_list_set_unlocked(new_port->saiport, value, true);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_QOS_EGRESS_BUFFER_PROFILE_LIST, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_buffer_port_profile_list_set_unlocked(new_port->saiport, value, false);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_MODE, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_pfc_control_mode_set_impl(new_port->logical, value->s32);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_pfc_control_set_impl(new_port->logical, value, SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_RX, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_pfc_control_set_impl(new_port->logical, value, SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_RX);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_TX, &value, &index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_pfc_control_set_impl(new_port->logical, value, SAI_PORT_ATTR_PRIORITY_FLOW_CONTROL_TX);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }
/* TODO : FEC is not supported on Simx/PLD(main model), add when ready */
#if !defined IS_PLD && !defined IS_SIMX
    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_FEC_MODE, &fec, &fec_index);
    if (status == SAI_STATUS_SUCCESS) {
        status = mlnx_port_fec_set_impl(new_port->logical, fec->s32);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    } else {
        if ((!is_warmboot_init_stage) && (mlnx_chip_is_spc2or3or4())) {
            status = mlnx_port_fec_set_impl(new_port->logical, SAI_PORT_FEC_MODE_NONE);
            if (SAI_ERR(status)) {
                goto out_unlock;
            }
        }
    }
#endif

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_EGRESS_BLOCK_PORT_LIST,
                                 &egress_block_list, &egress_block_list_index);
    if (!SAI_ERR(status)) {
        status = mlnx_validate_port_isolation_api(PORT_ISOLATION_API_EGRESS_BLOCK_PORT);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Isolation group in use\n");
            goto out_unlock;
        }

        sx_egress_block_port_list = calloc(MAX_PORTS, sizeof(*sx_egress_block_port_list));
        if (!sx_egress_block_port_list) {
            SX_LOG_ERR("Failed to allocate memory\n");
            status = SAI_STATUS_NO_MEMORY;
            goto out_unlock;
        }

        status = mlnx_port_egress_block_sai_ports_to_sx(new_port->logical, egress_block_list->objlist.list,
                                                        egress_block_list->objlist.count, egress_block_list_index,
                                                        sx_egress_block_port_list);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }

        status = mlnx_port_egress_block_set_impl(new_port->logical, sx_egress_block_port_list,
                                                 egress_block_list->objlist.count);
        if (SAI_ERR(status)) {
            goto out_unlock;
        }
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_ATTR_ISOLATION_GROUP, &isolation_group,
                                 &isolation_group_index);
    if (!SAI_ERR(status)) {
        status = mlnx_set_port_isolation_group_impl(new_port->saiport, isolation_group->oid);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to set port isolation group\n");
            goto out_unlock;
        }
    }

    /* Mark port as splitted only if the new width != initial width */
    new_port->is_split = new_port->width != port_map->width;

    SX_LOG_NTC("Created PORT 0x%" PRIx64 ": local=%u, width=%u, module=%u, lane_bitmap=0x%x\n",
               new_port->saiport, new_port->port_map.local_port, port_map->width,
               port_map->module_port, port_map->lane_bmap);

    *port_id = new_port->saiport;

    status = SAI_STATUS_SUCCESS;

out_unlock:
    acl_global_unlock();
    sai_db_unlock();
out:
    free(sx_egress_block_port_list);
    SX_LOG_EXIT();
    return status;
}

/**
 * Routine Description:
 *    @brief Remove port
 *
 * Arguments:
 *    @param[in] port_id - port id
 *
 * Return Values:
 *    @return SAI_STATUS_SUCCESS on success
 *            Failure status code on error
 */
sai_status_t mlnx_remove_port(_In_ sai_object_id_t port_id)
{
    sai_status_t        status = SAI_STATUS_SUCCESS;
    sx_port_log_id_t    port_log_id;
    mlnx_port_config_t *port;
    sai_object_id_t     isolation_group;
    uint32_t            ar_port_index;

    SX_LOG_ENTER();

    MLNX_LOG_OID_REMOVE(port_id);

    status = mlnx_object_to_type(port_id, SAI_OBJECT_TYPE_PORT, &port_log_id, NULL);
    if (SAI_ERR(status)) {
        goto out;
    }

    sai_db_write_lock();

    status = mlnx_port_by_log_id(port_log_id, &port);
    if (SAI_ERR(status)) {
        goto out_unlock;
    }

    status = mlnx_port_in_use_check(port);
    if (SAI_ERR(status)) {
        goto out_unlock;
    }

    if (is_isolation_group_in_use()) {
        status = mlnx_get_port_isolation_group_impl(port_id, &isolation_group);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to get sai isolation group\n");
            goto out_unlock;
        }

        if (isolation_group != SAI_NULL_OBJECT_ID) {
            status = mlnx_set_port_isolation_group_impl(port_id, SAI_NULL_OBJECT_ID);
            if (SAI_ERR(status)) {
                SX_LOG_ERR("Failed to unset port isolation group\n");
                goto out_unlock;
            }
        }
    }

    status = mlnx_wred_mirror_port_event(port->logical, false);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Error removing port mirror wred discard for port 0x%x\n", port->logical);
        goto out_unlock;
    }

    status = mlnx_port_del(port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed remove port 0x%x\n", port->logical);
        goto out_unlock;
    }

    SX_LOG_NTC("Removed port %" PRIx64 ": local=%u, width=%u, module=%u, lane_bmap=0x%x\n",
               port->saiport, port->port_map.local_port, port->port_map.width,
               port->port_map.module_port,
               port->port_map.lane_bmap);

    port->is_split = false;

    g_sai_db_ptr->ports_number--;

    if (mlnx_find_ar_port_by_id(port_log_id, &ar_port_index, NULL)) {
        g_sai_db_ptr->ar_db.ar_port_list[ar_port_index].port_id = SX_INVALID_PORT;
    }

out_unlock:
    sai_db_unlock();
out:
    SX_LOG_EXIT();
    return status;
}

/**
 * @brief Create port pool
 *
 * @param[out] port_pool_id Port pool id
 * @param[in] switch_id Switch id
 * @param[in] attr_count Number of attributes
 * @param[in] attr_list Array of attributes
 *
 * @return #SAI_STATUS_SUCCESS on success, failure status code on error
 */
static sai_status_t mlnx_create_port_pool(_Out_ sai_object_id_t      *port_pool_id,
                                          _In_ sai_object_id_t        switch_id,
                                          _In_ uint32_t               attr_count,
                                          _In_ const sai_attribute_t *attr_list)
{
    const sai_attribute_value_t *port_attr = NULL;
    const sai_attribute_value_t *pool_attr = NULL;
    uint32_t                     pool_idx;
    uint32_t                     port_idx;
    sx_port_log_id_t             port_id;
    uint32_t                     pool_id;
    sai_status_t                 status;
    mlnx_port_config_t          *port;
    mlnx_sai_buffer_pool_attr_t  sai_pool_attr;
    uint8_t                      extended_data[EXTENDED_DATA_SIZE];

    SX_LOG_ENTER();

    status = check_attribs_on_create(attr_count, attr_list, SAI_OBJECT_TYPE_PORT_POOL, port_pool_id);
    if (SAI_ERR(status)) {
        return status;
    }
    MLNX_LOG_ATTRS(attr_count, attr_list, SAI_OBJECT_TYPE_PORT_POOL);

    /* Mandatory attributes */
    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_POOL_ATTR_BUFFER_POOL_ID, &pool_attr, &pool_idx);
    assert(SAI_STATUS_SUCCESS == status);

    status = find_attrib_in_list(attr_count, attr_list, SAI_PORT_POOL_ATTR_PORT_ID, &port_attr, &port_idx);
    assert(SAI_STATUS_SUCCESS == status);

    status = mlnx_object_to_type(port_attr->oid, SAI_OBJECT_TYPE_PORT, &port_id, NULL);
    if (SAI_ERR(status)) {
        goto out;
    }

    status = mlnx_object_to_type(pool_attr->oid, SAI_OBJECT_TYPE_BUFFER_POOL, &pool_id, NULL);
    if (SAI_ERR(status)) {
        goto out;
    }

    sai_db_read_lock();

    /* validate port existence */
    status = mlnx_port_by_log_id(port_id, &port);
    if (SAI_ERR(status)) {
        sai_db_unlock();
        goto out;
    }

    if (SAI_STATUS_SUCCESS != (status = mlnx_get_sai_pool_data(pool_attr->oid, &sai_pool_attr))) {
        sai_db_unlock();
        goto out;
    }

    sai_db_unlock();

    extended_data[0] = (uint8_t)pool_id;
    extended_data[1] = (uint8_t)sai_pool_attr.pool_type;
    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_create_object(SAI_OBJECT_TYPE_PORT_POOL, port_id, extended_data,
                                     port_pool_id))) {
        goto out;
    }

    MLNX_LOG_OID_CREATED(*port_pool_id);

out:
    SX_LOG_EXIT();
    return status;
}

/**
 * @brief Remove port pool
 *
 * @param[in] port_pool_id Port pool id
 *
 * @return #SAI_STATUS_SUCCESS on success, failure status code on error
 */
static sai_status_t mlnx_remove_port_pool(_In_ sai_object_id_t port_pool_id)
{
    sai_status_t     status;
    sx_port_log_id_t port_id;

    SX_LOG_ENTER();

    MLNX_LOG_OID_REMOVE(port_pool_id);

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(port_pool_id, SAI_OBJECT_TYPE_PORT_POOL, &port_id, NULL))) {
        goto out;
    }

out:
    SX_LOG_EXIT();
    return status;
}

/**
 * @brief Set port pool attribute value.
 *
 * @param[in] port_pool_id Port pool id
 * @param[in] attr Attribute
 *
 * @return #SAI_STATUS_SUCCESS on success, failure status code on error
 */
static sai_status_t mlnx_set_port_pool_attribute(_In_ sai_object_id_t port_pool_id, _In_ const sai_attribute_t *attr)
{
    const sai_object_key_t key = { .key.object_id = port_pool_id };

    return sai_set_attribute(&key, SAI_OBJECT_TYPE_PORT_POOL, attr);
}

/**
 * @brief Get port pool attribute value.
 *
 * @param[in] port_pool_id Port pool id
 * @param[in] attr_count Number of attributes
 * @param[inout] attr_list Array of attributes
 *
 * @return #SAI_STATUS_SUCCESS on success, failure status code on error
 */
static sai_status_t mlnx_get_port_pool_attribute(_In_ sai_object_id_t     port_pool_id,
                                                 _In_ uint32_t            attr_count,
                                                 _Inout_ sai_attribute_t *attr_list)
{
    const sai_object_key_t key = { .key.object_id = port_pool_id };

    return sai_get_attributes(&key, SAI_OBJECT_TYPE_PORT_POOL, attr_count, attr_list);
}

static sai_status_t mlnx_port_pool_attr_get(_In_ const sai_object_key_t   *key,
                                            _Inout_ sai_attribute_value_t *value,
                                            _In_ uint32_t                  attr_index,
                                            _Inout_ vendor_cache_t        *cache,
                                            _In_ void                     *arg)
{
    sai_status_t     status;
    sai_attr_id_t    attr_id;
    sx_port_log_id_t port_id;
    uint8_t          extended_data[EXTENDED_DATA_SIZE] = { 0 };

    SX_LOG_ENTER();

    attr_id = (long)arg;

    assert((SAI_PORT_POOL_ATTR_BUFFER_POOL_ID == attr_id) || (SAI_PORT_POOL_ATTR_PORT_ID == attr_id));

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_object_to_type(key->key.object_id, SAI_OBJECT_TYPE_PORT_POOL, &port_id, extended_data))) {
        return status;
    }

    if (SAI_PORT_POOL_ATTR_PORT_ID == attr_id) {
        status = mlnx_create_object(SAI_OBJECT_TYPE_PORT, port_id, NULL, &value->oid);
    } else {
        status = mlnx_create_object(SAI_OBJECT_TYPE_BUFFER_POOL, extended_data[0], NULL, &value->oid);
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/**
 * @brief Get port pool statistics counters extended.
 *
 * @param[in] port_pool_id Port pool id
 * @param[in] number_of_counters Number of counters in the array
 * @param[in] counter_ids Specifies the array of counter ids
 * @param[in] mode Statistics mode
 * @param[out] counters Array of resulting counter values.
 *
 * @return #SAI_STATUS_SUCCESS on success, failure status code on error
 */
sai_status_t mlnx_get_port_pool_stats_ext(_In_ sai_object_id_t      port_pool_id,
                                          _In_ uint32_t             number_of_counters,
                                          _In_ const sai_stat_id_t *counter_ids,
                                          _In_ sai_stats_mode_t     mode,
                                          _Out_ uint64_t           *counters)
{
    sai_status_t                        status;
    uint8_t                             ext_data[EXTENDED_DATA_SIZE] = {0};
    sx_cos_pool_id_t                    pool_num;
    sx_port_log_id_t                    port_num;
    uint32_t                            ii;
    char                                key_str[MAX_KEY_STR_LEN];
    sx_port_statistic_usage_params_t    stats_usage;
    sx_port_occupancy_statistics_t      occupancy_stats;
    uint32_t                            usage_cnt = 1;
    uint32_t                            db_port_index, pool_base_ind, buff_ind;
    uint32_t                           *port_buff_profile_refs = NULL;
    mlnx_sai_db_buffer_profile_entry_t *buff_db_entry = NULL;
    sx_access_cmd_t                     cmd;

    SX_LOG_ENTER();

    oid_to_str(port_pool_id, key_str);
    SX_LOG_DBG("Get stats %s\n", key_str);

    if (NULL == counter_ids) {
        SX_LOG_ERR("NULL counter ids array param\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (NULL == counters) {
        SX_LOG_ERR("NULL counters array param\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (SAI_STATUS_SUCCESS !=
        (status = mlnx_translate_sai_stats_mode_to_sdk(mode, &cmd))) {
        return status;
    }

    if (SAI_STATUS_SUCCESS != mlnx_object_to_type(port_pool_id, SAI_OBJECT_TYPE_PORT_POOL, &port_num, ext_data)) {
        return SAI_STATUS_INVALID_PARAMETER;
    }
    pool_num = ext_data[0];

    memset(&stats_usage, 0, sizeof(stats_usage));
    stats_usage.port_cnt = 1;
    stats_usage.log_port_list_p = &port_num;
    stats_usage.sx_port_params.port_params_cnt = 1;
    if (SAI_BUFFER_POOL_TYPE_INGRESS == ext_data[1]) {
        stats_usage.sx_port_params.port_params_type = SX_COS_INGRESS_PORT_ATTR_E;
        stats_usage.sx_port_params.port_param.ingress_port_pool_list_p = &pool_num;
    } else {
        stats_usage.sx_port_params.port_params_type = SX_COS_EGRESS_PORT_ATTR_E;
        stats_usage.sx_port_params.port_param.egress_port_pool_list_p = &pool_num;
    }

    if (SX_STATUS_SUCCESS !=
        (status = sx_api_cos_port_buff_type_statistic_get(gh_sdk, cmd, &stats_usage, 1,
                                                          &occupancy_stats, &usage_cnt))) {
        SX_LOG_ERR("Failed to get port buff statistics - %s.\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    for (ii = 0; ii < number_of_counters; ii++) {
        switch (counter_ids[ii]) {
        case SAI_PORT_POOL_STAT_IF_OCTETS:
        case SAI_PORT_POOL_STAT_GREEN_WRED_DROPPED_PACKETS:
        case SAI_PORT_POOL_STAT_GREEN_WRED_DROPPED_BYTES:
        case SAI_PORT_POOL_STAT_YELLOW_WRED_DROPPED_PACKETS:
        case SAI_PORT_POOL_STAT_YELLOW_WRED_DROPPED_BYTES:
        case SAI_PORT_POOL_STAT_RED_WRED_DROPPED_PACKETS:
        case SAI_PORT_POOL_STAT_RED_WRED_DROPPED_BYTES:
        case SAI_PORT_POOL_STAT_WRED_DROPPED_PACKETS:
        case SAI_PORT_POOL_STAT_WRED_DROPPED_BYTES:
        case SAI_PORT_POOL_STAT_GREEN_WRED_ECN_MARKED_PACKETS:
        case SAI_PORT_POOL_STAT_GREEN_WRED_ECN_MARKED_BYTES:
        case SAI_PORT_POOL_STAT_YELLOW_WRED_ECN_MARKED_PACKETS:
        case SAI_PORT_POOL_STAT_YELLOW_WRED_ECN_MARKED_BYTES:
        case SAI_PORT_POOL_STAT_RED_WRED_ECN_MARKED_PACKETS:
        case SAI_PORT_POOL_STAT_RED_WRED_ECN_MARKED_BYTES:
        case SAI_PORT_POOL_STAT_WRED_ECN_MARKED_PACKETS:
        case SAI_PORT_POOL_STAT_WRED_ECN_MARKED_BYTES:
        case SAI_PORT_POOL_STAT_SHARED_CURR_OCCUPANCY_BYTES:
        case SAI_PORT_POOL_STAT_DROPPED_PKTS:
            SX_LOG_NTC("Port pool counter %d set item %u not supported\n", counter_ids[ii], ii);
            return SAI_STATUS_ATTR_NOT_SUPPORTED_0;

        case SAI_PORT_POOL_STAT_CURR_OCCUPANCY_BYTES:
            counters[ii] = (uint64_t)occupancy_stats.statistics.curr_occupancy *
                           g_resource_limits.shared_buff_buffer_unit_size;
            break;

        case SAI_PORT_POOL_STAT_WATERMARK_BYTES:
            counters[ii] = (uint64_t)occupancy_stats.statistics.watermark *
                           g_resource_limits.shared_buff_buffer_unit_size;
            break;

        case SAI_PORT_POOL_STAT_SHARED_WATERMARK_BYTES:
            counters[ii] = (uint64_t)occupancy_stats.statistics.watermark *
                           g_resource_limits.shared_buff_buffer_unit_size;

            sai_db_read_lock();

            status = mlnx_port_idx_by_log_id(port_num, &db_port_index);
            if (status != SAI_STATUS_SUCCESS) {
                sai_db_unlock();
                SX_LOG_EXIT();
                return status;
            }

            if (SAI_STATUS_SUCCESS !=
                (status = mlnx_sai_get_port_buffer_index_array(db_port_index,
                                                               (SAI_BUFFER_POOL_TYPE_INGRESS ==
                                                                ext_data[1]) ? PORT_BUFF_TYPE_INGRESS :
                                                               PORT_BUFF_TYPE_EGRESS, &port_buff_profile_refs))) {
                sai_db_unlock();
                SX_LOG_EXIT();
                return status;
            }

            if (SAI_BUFFER_POOL_TYPE_INGRESS == ext_data[1]) {
                pool_base_ind = g_sai_buffer_db_ptr->buffer_pool_ids.base_ingress_user_sx_pool_id;
            } else {
                pool_base_ind = g_sai_buffer_db_ptr->buffer_pool_ids.base_egress_user_sx_pool_id;
            }
            buff_ind = port_buff_profile_refs[pool_num - pool_base_ind];

            if (SENTINEL_BUFFER_DB_ENTRY_INDEX != buff_ind) {
                buff_db_entry = &g_sai_buffer_db_ptr->buffer_profiles[buff_ind];
                if (counters[ii] > buff_db_entry->reserved_size) {
                    counters[ii] -= buff_db_entry->reserved_size;
                } else {
                    counters[ii] = 0;
                }
            }

            sai_db_unlock();
            break;

        default:
            SX_LOG_ERR("Invalid port pool counter %d\n", counter_ids[ii]);
            return SAI_STATUS_INVALID_PARAMETER;
        }
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/**
 * @brief Get port pool statistics counters.
 *
 * @param[in] port_pool_id Port pool id
 * @param[in] number_of_counters Number of counters in the array
 * @param[in] counter_ids Specifies the array of counter ids
 * @param[out] counters Array of resulting counter values.
 *
 * @return #SAI_STATUS_SUCCESS on success, failure status code on error
 */
static sai_status_t mlnx_get_port_pool_stats(_In_ sai_object_id_t      port_pool_id,
                                             _In_ uint32_t             number_of_counters,
                                             _In_ const sai_stat_id_t *counter_ids,
                                             _Out_ uint64_t           *counters)
{
    return mlnx_get_port_pool_stats_ext(port_pool_id, number_of_counters, counter_ids, SAI_STATS_MODE_READ, counters);
}

/**
 * @brief Clear port pool statistics counters.
 *
 * @param[in] port_pool_id Port pool id
 * @param[in] number_of_counters Number of counters in the array
 * @param[in] counter_ids Specifies the array of counter ids
 *
 * @return #SAI_STATUS_SUCCESS on success, failure status code on error
 */
static sai_status_t mlnx_clear_port_pool_stats(_In_ sai_object_id_t      port_pool_id,
                                               _In_ uint32_t             number_of_counters,
                                               _In_ const sai_stat_id_t *counter_ids)
{
    sai_status_t                     status;
    uint8_t                          ext_data[EXTENDED_DATA_SIZE] = { 0 };
    sx_cos_pool_id_t                 pool_num;
    sx_port_log_id_t                 port_num;
    sx_port_statistic_usage_params_t stats_usage;
    sx_port_occupancy_statistics_t   occupancy_stats;
    uint32_t                         usage_cnt = 1;

    SX_LOG_ENTER();

    MLNX_LOG_OID("Clear stats", port_pool_id);

    if (NULL == counter_ids) {
        SX_LOG_ERR("NULL counter ids array param\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (SAI_STATUS_SUCCESS != mlnx_object_to_type(port_pool_id, SAI_OBJECT_TYPE_PORT_POOL, &port_num, ext_data)) {
        return SAI_STATUS_INVALID_PARAMETER;
    }
    pool_num = ext_data[0];

    memset(&stats_usage, 0, sizeof(stats_usage));
    stats_usage.port_cnt = 1;
    stats_usage.log_port_list_p = &port_num;
    stats_usage.sx_port_params.port_params_cnt = 1;
    if (SAI_BUFFER_POOL_TYPE_INGRESS == ext_data[1]) {
        stats_usage.sx_port_params.port_params_type = SX_COS_INGRESS_PORT_ATTR_E;
        stats_usage.sx_port_params.port_param.ingress_port_pool_list_p = &pool_num;
    } else {
        stats_usage.sx_port_params.port_params_type = SX_COS_EGRESS_PORT_ATTR_E;
        stats_usage.sx_port_params.port_param.egress_port_pool_list_p = &pool_num;
    }

    if (SX_STATUS_SUCCESS !=
        (status = sx_api_cos_port_buff_type_statistic_get(gh_sdk, SX_ACCESS_CMD_READ_CLEAR, &stats_usage, 1,
                                                          &occupancy_stats, &usage_cnt))) {
        SX_LOG_ERR("Failed to get clear port pool buff statistics - %s.\n", SX_STATUS_MSG(status));
        return sdk_to_sai(status);
    }

    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

/**
 * @brief Bind/unbind internal ACLs
 * 1. for the per-port IP counter
 * 2. for the VxLAN srcport
 *
 * @param[in] op_type     Operation type
 * @param[in] sx_port_id  Port id
 *
 * @return #SAI_STATUS_SUCCESS on success, failure status code on error
 */
sai_status_t mlnx_internal_acls_bind(_In_ internal_acl_op_types op_type, _In_ sai_object_id_t sai_port_id)
{
    sai_status_t        sai_status;
    sx_status_t         sx_status;
    sx_port_log_id_t    sx_port_id;
    mlnx_port_config_t *port_cfg;
    uint32_t            ii, sai_db_idx_start = 0, sai_db_idx_end = MLNX_MAX_TUNNEL_NVE;
    sx_access_cmd_t     cmd;

    assert(op_type == INTERNAL_ACL_OP_ADD_PORT ||
           op_type == INTERNAL_ACL_OP_DEL_PORT ||
           op_type == INTERNAL_ACL_OP_ADD_LAG ||
           op_type == INTERNAL_ACL_OP_DEL_LAG ||
           op_type == INTERNAL_ACL_OP_ADD_PORT_TO_LAG ||
           op_type == INTERNAL_ACL_OP_DEL_PORT_FROM_LAG);

    SX_LOG_ENTER();

    sai_status = mlnx_port_by_obj_id(sai_port_id, &port_cfg);
    if (SAI_ERR(sai_status)) {
        goto out;
    }
    sx_port_id = port_cfg->logical;

    /* for per-port IP counter */
    if (mlnx_perport_ipcnt_is_enable_nolock()) {
        sai_status = mlnx_perport_ipcnt_ops(sx_port_id, port_cfg->index, op_type);
        if (SAI_ERR(sai_status)) {
            goto out;
        }
    } else {
        SX_LOG_NTC("The per-port IP counter is not enabled.\n");
    }

    /* for VxLAN srcport */
    if ((op_type == INTERNAL_ACL_OP_ADD_PORT) ||
        (op_type == INTERNAL_ACL_OP_ADD_LAG) ||
        (op_type == INTERNAL_ACL_OP_DEL_PORT_FROM_LAG)) {
        cmd = SX_ACCESS_CMD_ADD;
    } else {
        /* INTERNAL_ACL_OP_DEL_PORT
         * INTERNAL_ACL_OP_DEL_LAG
         * INTERNAL_ACL_OP_ADD_PORT_TO_LAG
         */
        cmd = SX_ACCESS_CMD_DELETE;
    }

    for (ii = sai_db_idx_start; ii < sai_db_idx_end; ++ii) {
        if (g_sai_tunnel_db_ptr->tunnel_entry_db[ii].vxlan_acl.is_acl_created) {
            sx_status = sx_api_acl_port_bind_set(gh_sdk,
                                                 cmd,
                                                 sx_port_id,
                                                 g_sai_tunnel_db_ptr->tunnel_entry_db[ii].vxlan_acl.acl_group);
            if (SX_ERR(sx_status)) {
                SX_LOG_ERR("Failed to %s VxLAN srcport ACL to port(%x). %s\n",
                           cmd == SX_ACCESS_CMD_DELETE ? "unbind" : "bind",
                           sx_port_id,
                           SX_STATUS_MSG(sx_status));
                sai_status = sdk_to_sai(sx_status);
                goto out;
            }
        }
    }

out:
    SX_LOG_EXIT();
    return sai_status;
}

sai_status_t mlnx_port_get_qos_map_id_by_log_port(_In_ sx_port_log_id_t   port_id,
                                                  _In_ sai_qos_map_type_t qos_map_type,
                                                  _Out_ sai_object_id_t  *oid)
{
    mlnx_port_config_t *port;
    sai_status_t        status;
    uint32_t            qos_map_id;


    if (mlnx_log_port_is_cpu(port_id)) {
        *oid = SAI_NULL_OBJECT_ID;
        return SAI_STATUS_SUCCESS;
    }

    status = mlnx_port_by_log_id(port_id, &port);
    if (SAI_ERR(status)) {
        return status;
    }

    qos_map_id = port->qos_maps[qos_map_type];
    if (!qos_map_id) {
        *oid = SAI_NULL_OBJECT_ID;
        return SAI_STATUS_SUCCESS;
    }

    return mlnx_create_object(SAI_OBJECT_TYPE_QOS_MAP, qos_map_id, NULL, oid);
}

static sai_status_t mlnx_port_lossless_buffer_count_get(_In_ sx_port_log_id_t port_id,
                                                        _Out_ uint32_t       *number_of_lossless_buffer)
{
    uint32_t                            local_number_of_lossless_buffer = 0;
    uint32_t                            db_port_index;
    sai_status_t                        sai_status = SAI_STATUS_SUCCESS;
    uint8_t                             port_pg_ind = 0;
    uint32_t                           *port_pg_profile_refs = NULL;
    uint32_t                            db_buffer_profile_index = 0;
    bool                                is_buffer_lossless;
    mlnx_sai_db_buffer_profile_entry_t *buff_db_entry;

    SX_LOG_ENTER();

    if (!mlnx_log_port_is_cpu(port_id)) {
        sai_status = mlnx_port_idx_by_log_id(port_id, &db_port_index);
        if (SAI_ERR(sai_status)) {
            SX_LOG_DBG("Failed to get port index for port id %x, set lossless buffer number 0.\n", port_id);
            sai_status = SAI_STATUS_SUCCESS;
            goto out;
        }
        sai_status = mlnx_sai_get_port_buffer_index_array(db_port_index, PORT_BUFF_TYPE_PG, &port_pg_profile_refs);
        if (SAI_ERR(sai_status)) {
            SX_LOG_DBG("Failed to get port buffer index array for port db index %x, set lossless buffer number 0.\n",
                       db_port_index);
            sai_status = SAI_STATUS_SUCCESS;
            goto out;
        }
        for (port_pg_ind = 0; port_pg_ind < mlnx_sai_get_buffer_resource_limits()->num_port_pg_buff; port_pg_ind++) {
            db_buffer_profile_index = port_pg_profile_refs[port_pg_ind];
            if (SENTINEL_BUFFER_DB_ENTRY_INDEX == db_buffer_profile_index) {
                continue;
            }
            buff_db_entry = &g_sai_buffer_db_ptr->buffer_profiles[db_buffer_profile_index];
            is_buffer_lossless = ((0 != buff_db_entry->xoff) && (0 != buff_db_entry->xon));
            if (is_buffer_lossless) {
                local_number_of_lossless_buffer++;
            }
        }
    }

out:
    if (number_of_lossless_buffer != NULL) {
        *number_of_lossless_buffer = local_number_of_lossless_buffer;
        SX_LOG_DBG("Get lossless buffer number %u for port id %x.\n",
                   local_number_of_lossless_buffer,
                   port_id);
    }

    SX_LOG_EXIT();
    return sai_status;
}

static sai_status_t mlnx_port_dscp_remapping_uplink_detect(_In_ sx_port_log_id_t port_id,
                                                           _Out_ bool           *should_be_uplink_port)
{
    sai_status_t sai_status = SAI_STATUS_SUCCESS;
    uint32_t     number_of_lossless_buffer = 0;
    bool         local_should_be_uplink_port;

    SX_LOG_ENTER();

    sai_status = mlnx_port_lossless_buffer_count_get(port_id, &number_of_lossless_buffer);
    if (SAI_ERR(sai_status)) {
        SX_LOG_ERR("Failed to get port lossless buffer number for port id %x.\n", port_id);
        goto out;
    }
    local_should_be_uplink_port = (UPLINK_LOSSLESS_PG_COUNT == number_of_lossless_buffer);
    if (should_be_uplink_port != NULL) {
        *should_be_uplink_port = local_should_be_uplink_port;
        SX_LOG_DBG("Port id %x is dscp remapping uplink: %s.\n", port_id, local_should_be_uplink_port ? "Yes" : "No");
    }

out:
    SX_LOG_EXIT();
    return sai_status;
}

static sai_status_t mlnx_port_add_port_to_dscp_remapping_uplink_list(_In_ sx_port_log_id_t port_id,
                                                                     _Out_ uint32_t       *index)
{
    uint32_t            ii = 0;
    port_qos_db_t      *port_qos_db;
    sai_status_t        status = SAI_STATUS_SUCCESS;
    sai_object_id_t     tc_to_pg_mapping_oid;
    sai_object_id_t     tc_to_queue_mapping_oid;
    mlnx_port_config_t *port_cfg;
    sx_port_log_id_t    port_or_lag_id;

    SX_LOG_ENTER();

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;

    for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
        if (port_qos_db->uplink_port_list_in_use[ii] &&
            (port_qos_db->uplink_port_list[ii] == port_id)) {
            goto out;
        }
    }

    status = mlnx_port_by_log_id(port_id, &port_cfg);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed lookup port config by log id %x.\n", port_id);
        goto out;
    }
    if (mlnx_port_is_lag_member(port_cfg)) {
        port_or_lag_id = mlnx_port_get_lag_id(port_cfg);
    } else {
        port_or_lag_id = port_id;
    }

    status = mlnx_port_get_qos_map_id_by_log_port(port_or_lag_id,
                                                  SAI_QOS_MAP_TYPE_TC_TO_PRIORITY_GROUP,
                                                  &tc_to_pg_mapping_oid);
    if (SAI_ERR(status)) {
        tc_to_pg_mapping_oid = SAI_NULL_OBJECT_ID;
    }

    status = mlnx_port_get_qos_map_id_by_log_port(port_or_lag_id,
                                                  SAI_QOS_MAP_TYPE_TC_TO_QUEUE,
                                                  &tc_to_queue_mapping_oid);
    if (SAI_ERR(status)) {
        tc_to_queue_mapping_oid = SAI_NULL_OBJECT_ID;
    }

    for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
        if (!port_qos_db->uplink_port_list_in_use[ii]) {
            port_qos_db->uplink_port_list_in_use[ii] = true;
            port_qos_db->uplink_port_list[ii] = port_id;
            port_qos_db->uplink_tc_to_pg_mapping[ii] = tc_to_pg_mapping_oid;
            port_qos_db->uplink_tc_to_queue_mapping[ii] = tc_to_queue_mapping_oid;
            status = SAI_STATUS_SUCCESS;
            if (index) {
                *index = ii;
            }
            goto out;
        }
    }

    status = SAI_STATUS_TABLE_FULL;

out:
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_remove_port_from_dscp_remapping_uplink_list(_In_ sx_port_log_id_t port_id)
{
    uint32_t       ii;
    port_qos_db_t *port_qos_db;
    sai_status_t   status = SAI_STATUS_SUCCESS;

    SX_LOG_ENTER();

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;
    for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
        if (port_qos_db->uplink_port_list_in_use[ii] && (port_qos_db->uplink_port_list[ii] == port_id)) {
            port_qos_db->uplink_port_list_in_use[ii] = false;
            port_qos_db->uplink_port_list[ii] = SX_INVALID_PORT;
            port_qos_db->uplink_tc_to_pg_mapping[ii] = SAI_NULL_OBJECT_ID;
            port_qos_db->uplink_tc_to_queue_mapping[ii] = SAI_NULL_OBJECT_ID;
            status = mlnx_port_undo_dscp_rewriting(port_id, ii);
            goto out;
        }
    }
    status = SAI_STATUS_ITEM_NOT_FOUND;

out:
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_do_dscp_rewriting(_In_ sx_port_log_id_t port_id, _In_ uint32_t index)
{
    port_qos_db_t      *port_qos_db;
    sai_status_t        status = SAI_STATUS_SUCCESS;
    tunnel_qos_data_t  *tunnel_qos_data;
    mlnx_port_config_t *port_cfg;

    SX_LOG_ENTER();

    tunnel_qos_data = &g_sai_tunnel_db_ptr->dscp_remapping_db->tunnel_qos_data;
    if (SAI_NULL_OBJECT_ID == tunnel_qos_data->encap_tc_to_dscp_mapping) {
        goto out;
    }

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;
    if (port_qos_db->uplink_port_rewrite_done[index]) {
        goto out;
    }

    status = mlnx_port_by_log_id(port_id, &port_cfg);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed lookup port config by log id %x.\n", port_id);
        goto out;
    }

    /* Apply will be effect on LAG if port is a LAG member */
    status = mlnx_port_qos_map_apply(port_cfg->saiport,
                                     tunnel_qos_data->encap_tc_to_dscp_mapping,
                                     SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to rewrite tc to dscp mapping for port id %x.\n", port_id);
        goto out;
    }

    port_qos_db->uplink_port_rewrite_done[index] = true;

out:
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_undo_dscp_rewriting(_In_ sx_port_log_id_t port_id, _In_ uint32_t index)
{
    uint32_t            qos_map_id;
    sai_object_id_t     qos_map_oid;
    sai_status_t        status = SAI_STATUS_SUCCESS;
    port_qos_db_t      *port_qos_db;
    mlnx_port_config_t *port_cfg;
    mlnx_port_config_t *lag_cfg;
    sx_port_log_id_t    lag_id;

    SX_LOG_ENTER();

    status = mlnx_port_by_log_id(port_id, &port_cfg);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed lookup port config by log id %x.\n", port_id);
        goto out;
    }

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;
    port_qos_db->uplink_port_rewrite_done[index] = false;

    if (mlnx_port_is_lag_member(port_cfg)) {
        /* Only resume the qos for the last uplink port in the LAG */
        lag_id = mlnx_port_get_lag_id(port_cfg);
        if (mlnx_port_is_tc_to_dscp_rewrite_done(lag_id)) {
            SX_LOG_NTC("Skip undo dscp rewriting for log id %x because it's not the last lag member.\n", port_id);
            goto out;
        } else {
            status = mlnx_port_by_log_id(lag_id, &lag_cfg);
            if (SAI_ERR(status)) {
                SX_LOG_ERR("Failed lookup lag config by log id %x.\n", lag_id);
                goto out;
            }
            qos_map_id = lag_cfg->qos_maps[SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP];
        }
    } else {
        qos_map_id = port_cfg->qos_maps[SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP];
    }

    if (!qos_map_id) {
        qos_map_oid = SAI_NULL_OBJECT_ID;
    } else {
        status = mlnx_create_object(SAI_OBJECT_TYPE_QOS_MAP, qos_map_id, NULL, &qos_map_oid);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to create oid from tc to queue map id %x.\n", qos_map_id);
            goto out;
        }
    }

    status = mlnx_port_qos_map_apply(port_cfg->saiport, qos_map_oid, SAI_QOS_MAP_TYPE_TC_AND_COLOR_TO_DSCP);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to recover tc to dscp mapping for port id %x.\n", port_id);
    }

out:
    SX_LOG_EXIT();
    return status;
}

static bool mlnx_port_is_in_uplink_list(_In_ sx_port_log_id_t port_id)
{
    uint32_t       ii = 0;
    port_qos_db_t *port_qos_db;
    bool           is_found = false;

    SX_LOG_ENTER();

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;
    for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
        if (port_qos_db->uplink_port_list_in_use[ii] && (port_qos_db->uplink_port_list[ii] == port_id)) {
            is_found = true;
            goto out;
        }
    }

out:
    SX_LOG_EXIT();
    return is_found;
}

sai_status_t mlnx_port_on_dscp_remapping_uplink_update(_In_ sx_port_log_id_t port_id)
{
    sai_status_t status = SAI_STATUS_SUCCESS;
    bool         should_be_uplink_port = false;
    bool         is_uplink_port;
    uint32_t     index;

    SX_LOG_ENTER();

    status = mlnx_port_dscp_remapping_uplink_detect(port_id, &should_be_uplink_port);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to detect uplink for port id %x.\n", port_id);
        goto out;
    }

    is_uplink_port = mlnx_port_is_in_uplink_list(port_id);

    if (should_be_uplink_port && !is_uplink_port) {
        SX_LOG_NTC("Add uplink port for port id %x.\n", port_id);
        status = mlnx_port_add_port_to_dscp_remapping_uplink_list(port_id, &index);
        if (SAI_ERR(status)) {
            goto out;
        }
        status = mlnx_port_do_dscp_rewriting(port_id, index);
        if (SAI_ERR(status)) {
            goto out;
        }
        status = mlnx_tunnel_update_dscp_remapping_acl_rules();
        if (SAI_ERR(status)) {
            goto out;
        }
    } else if (!should_be_uplink_port && is_uplink_port) {
        SX_LOG_NTC("Delete uplink port for port id %x.\n", port_id);
        status = mlnx_port_remove_port_from_dscp_remapping_uplink_list(port_id);
        if (SAI_ERR(status)) {
            goto out;
        }
        status = mlnx_tunnel_update_dscp_remapping_acl_rules();
        if (SAI_ERR(status)) {
            goto out;
        }
    }

out:
    SX_LOG_EXIT();
    return status;
}

bool mlnx_port_is_tc_to_dscp_rewrite_done(_In_ sx_port_log_id_t port_id)
{
    bool                is_rewrite_done = false;
    port_qos_db_t      *port_qos_db;
    uint32_t            ii = 0;
    mlnx_port_config_t *port_cfg;
    mlnx_port_config_t *loop_port_cfg;
    sai_status_t        status = SAI_STATUS_SUCCESS;

    SX_LOG_ENTER();

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;
    status = mlnx_port_by_log_id(port_id, &port_cfg);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed lookup port config by log id %x.\n", port_id);
        goto out;
    }

    if (mlnx_port_is_lag(port_cfg)) {
        for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
            if (port_qos_db->uplink_port_list_in_use[ii]) {
                status = mlnx_port_by_log_id(port_qos_db->uplink_port_list[ii], &loop_port_cfg);
                if (SAI_ERR(status)) {
                    SX_LOG_ERR("Failed lookup port config by log id %x.\n", port_id);
                    continue;
                }
                if (mlnx_port_is_lag_member(loop_port_cfg) && (port_id == mlnx_port_get_lag_id(loop_port_cfg)) &&
                    port_qos_db->uplink_port_rewrite_done[ii]) {
                    is_rewrite_done = true;
                    break;
                }
            }
        }
    } else {
        for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
            if (port_qos_db->uplink_port_list_in_use[ii] &&
                port_qos_db->uplink_port_rewrite_done[ii] &&
                (port_qos_db->uplink_port_list[ii] == port_id)) {
                is_rewrite_done = true;
                break;
            }
        }
    }

out:
    SX_LOG_EXIT();
    return is_rewrite_done;
}

sai_status_t mlnx_port_dscp_remapping_uplink_list_init(void)
{
    uint32_t            ii;
    bool                should_be_uplink_port = false;
    sai_status_t        status = SAI_STATUS_SUCCESS;
    uint32_t            uplink_index;
    mlnx_port_config_t *port;


    SX_LOG_ENTER();

    mlnx_port_phy_foreach(port, ii) {
        status = mlnx_port_dscp_remapping_uplink_detect(port->logical, &should_be_uplink_port);
        if (SAI_ERR(status)) {
            SX_LOG_ERR("Failed to detect uplink for port id %x.\n", port->logical);
            goto out;
        }
        if (should_be_uplink_port) {
            SX_LOG_NTC("Add uplink port for port id %x in uplink list init.\n", port->logical);
            status = mlnx_port_add_port_to_dscp_remapping_uplink_list(port->logical, &uplink_index);
            if (SAI_ERR(status)) {
                SX_LOG_ERR("Failed to add uplink for port id %x.\n", port->logical);
                goto out;
            }
        }
    }

out:
    SX_LOG_EXIT();
    return status;
}

void mlnx_port_dscp_remapping_uplink_list_clear(void)
{
    uint32_t       ii;
    port_qos_db_t *port_qos_db;

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;
    for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
        if (port_qos_db->uplink_port_list_in_use[ii]) {
            port_qos_db->uplink_port_list_in_use[ii] = false;
            port_qos_db->uplink_port_list[ii] = SX_INVALID_PORT;
            port_qos_db->uplink_tc_to_pg_mapping[ii] = SAI_NULL_OBJECT_ID;
            port_qos_db->uplink_tc_to_queue_mapping[ii] = SAI_NULL_OBJECT_ID;
        }
    }

    port_qos_db->effective_tc_to_pg_mapping = SAI_NULL_OBJECT_ID;
    port_qos_db->effective_tc_to_queue_mapping = SAI_NULL_OBJECT_ID;
}

sai_status_t mlnx_port_do_dscp_rewriting_for_all_uplink_ports(void)
{
    uint32_t       ii;
    port_qos_db_t *port_qos_db;
    sai_status_t   status = SAI_STATUS_SUCCESS;

    SX_LOG_ENTER();

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;
    for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
        if (port_qos_db->uplink_port_list_in_use[ii]) {
            status = mlnx_port_do_dscp_rewriting(port_qos_db->uplink_port_list[ii], ii);
            if (SAI_ERR(status)) {
                goto out;
            }
        }
    }

out:
    SX_LOG_EXIT();
    return status;
}

sai_status_t mlnx_port_undo_dscp_rewriting_for_all_uplink_ports(void)
{
    uint32_t       ii;
    port_qos_db_t *port_qos_db;
    sai_status_t   status = SAI_STATUS_SUCCESS;

    SX_LOG_ENTER();

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;
    for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
        if (port_qos_db->uplink_port_list_in_use[ii] && port_qos_db->uplink_port_rewrite_done[ii]) {
            status = mlnx_port_undo_dscp_rewriting(port_qos_db->uplink_port_list[ii], ii);
            if (SAI_ERR(status)) {
                SX_LOG_ERR("Failed to undo dscp rewriting for port id %x, ii %u.\n",
                           port_qos_db->uplink_port_list[ii], ii);
                goto out;
            }
        }
    }

out:
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_tc_to_queue_update_for_dscp_remapping(_In_ sx_port_log_id_t port_id,
                                                                    _In_ sai_object_id_t  tc_to_queue_mapping_oid)
{
    port_qos_db_t      *port_qos_db;
    uint32_t            ii = 0;
    mlnx_port_config_t *port_cfg;
    mlnx_port_config_t *loop_port_cfg;
    bool                is_lag_member_update = false;
    sai_status_t        status = SAI_STATUS_SUCCESS;

    SX_LOG_ENTER();

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;

    status = mlnx_port_by_log_id(port_id, &port_cfg);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed lookup port config by log id %x.\n", port_id);
        goto out;
    }

    SX_LOG_INF("QoS map (tc->queue) updated for port id %x, is lag %s.\n", port_id,
               mlnx_port_is_lag(port_cfg) ? "true" : "false");

    if (mlnx_port_is_lag(port_cfg)) {
        for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
            if (port_qos_db->uplink_port_list_in_use[ii]) {
                status = mlnx_port_by_log_id(port_qos_db->uplink_port_list[ii], &loop_port_cfg);
                if (SAI_ERR(status)) {
                    SX_LOG_ERR("Failed lookup port config by log id %x.\n", port_id);
                    goto out;
                }
                if (mlnx_port_is_lag_member(loop_port_cfg) && (port_id == mlnx_port_get_lag_id(loop_port_cfg))) {
                    port_qos_db->uplink_tc_to_queue_mapping[ii] = tc_to_queue_mapping_oid;
                    is_lag_member_update = true;
                }
            }
        }
        if (is_lag_member_update) {
            status = mlnx_tunnel_update_dscp_remapping_acl_rules();
            if (SAI_ERR(status)) {
                goto out;
            }
        }
    } else {
        for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
            if (port_qos_db->uplink_port_list_in_use[ii] && (port_qos_db->uplink_port_list[ii] == port_id)) {
                port_qos_db->uplink_tc_to_queue_mapping[ii] = tc_to_queue_mapping_oid;
                status = mlnx_tunnel_update_dscp_remapping_acl_rules();
                if (SAI_ERR(status)) {
                    goto out;
                }
                break;
            }
        }
    }

out:
    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_port_tc_to_pg_update_for_dscp_remapping(_In_ sx_port_log_id_t port_id,
                                                                 _In_ sai_object_id_t  tc_to_pg_mapping_oid)
{
    port_qos_db_t *port_qos_db;
    uint32_t       ii;
    sai_status_t   status = SAI_STATUS_SUCCESS;

    SX_LOG_ENTER();

    port_qos_db = &g_sai_tunnel_db_ptr->dscp_remapping_db->port_qos_db;
    for (ii = 0; ii < MAX_UPLINK_PORTS; ii++) {
        if (port_qos_db->uplink_port_list_in_use[ii] && (port_qos_db->uplink_port_list[ii] == port_id)) {
            port_qos_db->uplink_tc_to_pg_mapping[ii] = tc_to_pg_mapping_oid;
            status = mlnx_tunnel_update_dscp_remapping_acl_rules();
            if (SAI_ERR(status)) {
                goto out;
            }
            break;
        }
    }

out:
    SX_LOG_EXIT();
    return status;
}

sai_status_t mlnx_bulk_remove_ports(_In_ uint32_t                 object_count,
                                    _In_ const sai_object_id_t   *object_id,
                                    _In_ sai_bulk_op_error_mode_t mode,
                                    _Out_ sai_status_t           *object_statuses)
{
    sai_status_t return_status = SAI_STATUS_SUCCESS;

    if (object_count > MAX_PORTS) {
        SX_LOG_ERR("Number of ports [%u] is bigger than the maximum allowed [%u]", object_count, MAX_PORTS);
        return SAI_STATUS_FAILURE;
    }

    if (!object_id) {
        SX_LOG_ERR("Object ID is NULL.\n");
        return SAI_STATUS_FAILURE;
    }

    if (!object_statuses) {
        SX_LOG_ERR("Object statuses is NULL.\n");
        return SAI_STATUS_FAILURE;
    }

    if ((mode != SAI_BULK_OP_ERROR_MODE_IGNORE_ERROR) &&
        (mode != SAI_BULK_OP_ERROR_MODE_STOP_ON_ERROR)) {
        SX_LOG_ERR("Unsupported bulk mode: %u.\n", mode);
        return SAI_STATUS_FAILURE;
    }

    if (mode == SAI_BULK_OP_ERROR_MODE_STOP_ON_ERROR) {
        for (uint32_t ii = 0; ii < object_count; ii++) {
            object_statuses[ii] = SAI_STATUS_NOT_EXECUTED;
        }
    }

    for (uint32_t ii = 0; ii < object_count; ii++) {
        object_statuses[ii] = mlnx_remove_port(object_id[ii]);

        if (SAI_ERR(object_statuses[ii])) {
            return_status = object_statuses[ii];
            if (mode == SAI_BULK_OP_ERROR_MODE_STOP_ON_ERROR) {
                return return_status;
            }
        }
    }

    return return_status;
}

sai_status_t mlnx_port_ar_link_util_percentage_to_kbps(_In_ sx_port_log_id_t port_id,
                                                       _In_ uint32_t         percentage_number,
                                                       _Out_ uint32_t       *link_util)
{
    sai_status_t status;
    uint32_t     admin_speed;
    uint32_t     op_speed;

    assert(NULL != link_util);

    SX_LOG_ENTER();

    /* Operational speed in mbps [uint32_t] */
    status = mlnx_port_speed_get_impl(port_id, &op_speed, &admin_speed);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Failed to get port speed for log id 0x%x.\n", port_id);
        SX_LOG_EXIT();
        return status;
    }

    /* link util in k bps = op speed in mbps * 1000 / 100 * percentage_number */
    *link_util = op_speed * 10 * percentage_number;

    SX_LOG_EXIT();
    return status;
}

static sai_status_t mlnx_check_and_store_ar_port_id(_In_ mlnx_port_config_t *port)
{
    sai_status_t          status;
    sai_attribute_value_t value = {0};
    mlnx_ar_db_data_t    *ar_data = &g_sai_db_ptr->ar_db;
    ar_port_data_t       *ar_port;
    uint32_t              lane_buffer[MAX_LANES_SPC3_4] = {0};
    uint32_t              lanes[MAX_LANES_SPC3_4] = {0};

    SX_LOG_ENTER();

    value.u32list.list = &lane_buffer[0];
    value.u32list.count = MAX_LANES_SPC3_4;
    status = copy_port_hw_lanes(&port->port_map, lanes, &value);
    if (SAI_ERR(status)) {
        SX_LOG_ERR("Error copy port hardware lanes with port id 0x%x\n", port->logical);
        SX_LOG_EXIT();
        return status;
    }

    for (uint32_t ii = 0; ii < ar_data->ar_port_count; ii++) {
        ar_port = &ar_data->ar_port_list[ii];
        /* Do exact list matching */
        if (u32_list_equal(&ar_port->lane_list[0], ar_port->lane_count, value.u32list.list, value.u32list.count)) {
            ar_port->port_id = port->logical;
            SX_LOG_NTC("Port id 0x%x is ar enabled, lane count %u, first lane number %u.\n",
                       port->logical, ar_port->lane_count, ar_port->lane_list[0]);
            SX_LOG_EXIT();
            return SAI_STATUS_SUCCESS;
        }
    }

    SX_LOG_INF("Port id 0x%x is not ar enabled, lane count %u, first lane number %u.\n",
               port->logical, value.u32list.count, value.u32list.list[0]);
    SX_LOG_EXIT();
    return SAI_STATUS_SUCCESS;
}

sai_status_t mlnx_port_get_ar_link_util_kbps(_In_ sx_port_log_id_t port_id, _Out_ uint32_t  *link_util)
{
    sx_status_t                   sx_status;
    sx_ar_link_utilization_attr_t link_util_attr;

    assert(NULL != link_util);

    sx_status = sx_api_ar_link_utilization_threshold_get(gh_sdk, port_id, &link_util_attr);
    if (SX_ERR(sx_status)) {
        SX_LOG_ERR("Failed to get ar link utilization threshold for port id 0x%x - %s.\n", port_id,
                   SX_STATUS_MSG(sx_status));
        return sdk_to_sai(sx_status);
    }

    *link_util = link_util_attr.link_utilization_threshold;
    return SAI_STATUS_SUCCESS;
}

/* Must call with holding lock, index and link_util_percentage can be NULL */
bool mlnx_find_ar_port_by_id(_In_ sx_port_log_id_t port_id,
                             _Out_ uint32_t       *index,
                             _Out_ uint32_t       *link_util_percentage)
{
    for (uint32_t ii = 0; ii < g_sai_db_ptr->ar_db.ar_port_count; ii++) {
        if (0 == g_sai_db_ptr->ar_db.ar_port_list[ii].lane_count) {
            return false;
        } else if (port_id == g_sai_db_ptr->ar_db.ar_port_list[ii].port_id) {
            if (NULL != index) {
                *index = ii;
            }
            if (NULL != link_util_percentage) {
                *link_util_percentage = g_sai_db_ptr->ar_db.ar_port_list[ii].link_util_percentage;
            }
            return true;
        }
    }
    return false;
}

const sai_port_api_t mlnx_port_api = {
    mlnx_create_port,
    mlnx_remove_port,
    mlnx_set_port_attribute,
    mlnx_get_port_attribute,
    mlnx_get_port_stats,
    mlnx_get_port_stats_ext,
    mlnx_clear_port_stats,
    mlnx_clear_port_all_stats,
    mlnx_create_port_pool,
    mlnx_remove_port_pool,
    mlnx_set_port_pool_attribute,
    mlnx_get_port_pool_attribute,
    mlnx_get_port_pool_stats,
    mlnx_get_port_pool_stats_ext,
    mlnx_clear_port_pool_stats,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    mlnx_bulk_remove_ports,
    NULL,
    NULL,
};
