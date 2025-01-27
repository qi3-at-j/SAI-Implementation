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

#include "mlnx_sai.h"
#include <sx/utils/dbg_utils.h>
#include "assert.h"

static void SAI_dump_policer_getdb(_Out_ mlnx_policer_shm_array_entry_t* policers_db, _Out_ uint32_t* p_count)
{
    assert(NULL != policers_db);
    assert(NULL != g_sai_db_ptr);
    assert(NULL != p_count);

    uint32_t                        ii, policer_entry_cnt = mlnx_shm_rm_array_size_get(MLNX_SHM_RM_ARRAY_TYPE_POLICER);
    uint32_t                        copied = 0;
    mlnx_policer_shm_array_entry_t* tmp_ptr_policer;
    sai_status_t                    sai_status;

    sai_db_read_lock();
    for (ii = 0; ii < policer_entry_cnt; ii++) {
        sai_status = mlnx_shm_rm_array_type_idx_to_ptr(MLNX_SHM_RM_ARRAY_TYPE_POLICER, ii,
                                                       (void **)&tmp_ptr_policer);
        if (SAI_ERR(sai_status)) {
            continue;
        }

        if (tmp_ptr_policer->array_hdr.is_used) {
            policers_db[copied] = *tmp_ptr_policer;
            copied++;
        }
    }
    *p_count = copied;
    sai_db_unlock();
}

static void SAI_dump_sx_policer_meter_enum_to_str(_In_ sx_policer_meter_t meter, _Out_ char *str)
{
    assert(NULL != str);

    switch (meter) {
    case SX_POLICER_METER_PACKETS:
        strcpy(str, "packets");
        break;

    case SX_POLICER_METER_TRAFFIC:
        strcpy(str, "traffic");
        break;

    default:
        strcpy(str, "unknown");
        break;
    }
}

static void SAI_dump_sx_policer_action_enum_to_str(_In_ sx_policer_action_t action, _Out_ char *str)
{
    assert(NULL != str);

    switch (action) {
    case SX_POLICER_ACTION_FORWARD:
        strcpy(str, "forward");
        break;

    case SX_POLICER_ACTION_DISCARD:
        strcpy(str, "discard");
        break;

    case SX_POLICER_ACTION_FORWARD_SET_RED_COLOR:
        strcpy(str, "fwd set color");
        break;

    default:
        strcpy(str, "unknown");
        break;
    }
}

static void SAI_dump_sx_policer_rate_type_enum_to_str(_In_ sx_policer_rate_type_e type, _Out_ char *str)
{
    assert(NULL != str);

    switch (type) {
    case SX_POLICER_RATE_TYPE_SX_E:
        strcpy(str, "sx");
        break;

    case SX_POLICER_RATE_TYPE_SINGLE_RATE_E:
        strcpy(str, "single");
        break;

    case SX_POLICER_RATE_TYPE_DUAL_RATE_E:
        strcpy(str, "dual");
        break;

    default:
        strcpy(str, "unknown");
        break;
    }
}

static void SAI_dump_sx_policer_ir_units_enum_to_str(_In_ sx_policer_ir_units_e units, _Out_ char *str)
{
    assert(NULL != str);

    switch (units) {
    case SX_POLICER_IR_UNITS_10_POWER_6_E:
        strcpy(str, "10^6");
        break;

    case SX_POLICER_IR_UNITS_10_POWER_3_E:
        strcpy(str, "10^3");
        break;

    default:
        strcpy(str, "unknown");
        break;
    }
}

static void SAI_dump_policer_print(_In_ FILE                           *file,
                                   _In_ mlnx_policer_shm_array_entry_t* policers_db,
                                   _In_ uint32_t                        count)
{
    uint32_t                       ii = 0;
    sai_object_id_t                obj_id = SAI_NULL_OBJECT_ID;
    mlnx_policer_shm_array_entry_t curr_policers_db;
    char                           meter_str[LINE_LENGTH];
    char                           y_action_str[LINE_LENGTH];
    char                           r_action_str[LINE_LENGTH];
    char                           rate_str[LINE_LENGTH];
    char                           ir_units_str[LINE_LENGTH];
    dbg_utils_table_columns_t      policer_clmns[] = {
        {"sai obj id",         16, PARAM_UINT64_E, &obj_id},
        {"db idx",             11, PARAM_UINT32_E, &ii},
        {"sx policer id trap", 18, PARAM_UINT64_E, &curr_policers_db.data.sx_policer_id_trap},
        {"sx policer id acl",  17, PARAM_UINT64_E, &curr_policers_db.data.sx_policer_id_acl},
        {"sx policer id acl mirror", 17, PARAM_UINT64_E, &curr_policers_db.data.sx_policer_id_acl_mirror},
        {"sx policer id span", 17, PARAM_UINT64_E, &curr_policers_db.data.sx_policer_id_span_session},
        {"meter type",         10, PARAM_STRING_E, &meter_str},
        {"cbs",                11, PARAM_UINT32_E, &curr_policers_db.data.sx_policer_attr.cbs},
        {"ebs",                11, PARAM_UINT32_E, &curr_policers_db.data.sx_policer_attr.ebs},
        {"cir",                11, PARAM_UINT32_E, &curr_policers_db.data.sx_policer_attr.cir},
        {"y action",           13, PARAM_STRING_E, &y_action_str},
        {"r action",           13, PARAM_STRING_E, &r_action_str},
        {"eir",                11, PARAM_UINT32_E, &curr_policers_db.data.sx_policer_attr.eir},
        {"rate type",          10, PARAM_STRING_E, &rate_str},
        {"color aware",        11, PARAM_UINT8_E,  &curr_policers_db.data.sx_policer_attr.color_aware},
        {"host ifc policer",   16, PARAM_UINT8_E,  &curr_policers_db.data.sx_policer_attr.is_host_ifc_policer},
        {"ir units",           11, PARAM_STRING_E, &ir_units_str},
        {NULL,                 0,  0,              NULL}
    };

    assert(NULL != policers_db);

    dbg_utils_print_general_header(file, "Policer");

    dbg_utils_print_secondary_header(file, "policers_db");

    dbg_utils_print_table_headline(file, policer_clmns);

    for (ii = 0; ii < count; ii++) {
        memcpy(&curr_policers_db, &policers_db[ii], sizeof(mlnx_policer_db_entry_t));

        if (SAI_STATUS_SUCCESS !=
            mlnx_create_object(SAI_OBJECT_TYPE_POLICER, ii, NULL, &obj_id)) {
            obj_id = SAI_NULL_OBJECT_ID;
        }
        SAI_dump_sx_policer_meter_enum_to_str(policers_db[ii].data.sx_policer_attr.meter_type,
                                              meter_str);
        SAI_dump_sx_policer_action_enum_to_str(policers_db[ii].data.sx_policer_attr.yellow_action,
                                               y_action_str);
        SAI_dump_sx_policer_action_enum_to_str(policers_db[ii].data.sx_policer_attr.red_action,
                                               r_action_str);
        SAI_dump_sx_policer_rate_type_enum_to_str(policers_db[ii].data.sx_policer_attr.rate_type,
                                                  rate_str);
        SAI_dump_sx_policer_ir_units_enum_to_str(policers_db[ii].data.sx_policer_attr.ir_units,
                                                 ir_units_str);
        dbg_utils_print_table_data_line(file, policer_clmns);
    }
}

void SAI_dump_policer(_In_ FILE *file)
{
    uint32_t                        policer_total_cnt = mlnx_shm_rm_array_size_get(MLNX_SHM_RM_ARRAY_TYPE_POLICER);
    uint32_t                        policer_cnt = 0;
    mlnx_policer_shm_array_entry_t *policers_db;

    policers_db = calloc(policer_total_cnt, sizeof(mlnx_policer_shm_array_entry_t));

    if (policers_db) {
        memset(policers_db, 0, policer_total_cnt * sizeof(mlnx_policer_shm_array_entry_t));

        SAI_dump_policer_getdb(policers_db, &policer_cnt);

        dbg_utils_print_module_header(file, "SAI Policer");

        SAI_dump_policer_print(file, policers_db, policer_cnt);
    }

    free(policers_db);
}
