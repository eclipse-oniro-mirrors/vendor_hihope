/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stdarg.h>

/* ========================================================================= */
/* WiFi 裁剪相关弱引用实现                                                    */
/* ========================================================================= */

__attribute__((weak)) void at_str_to_hex(const char *param, unsigned int len, unsigned char *value)
{
    // 空实现，WiFi 功能已裁剪
    (void)param;
    (void)len;
    (void)value;
}

__attribute__((weak)) void dmac_tx_sched_timer_handler(void)
{
    // 空实现，WiFi DMAC 已裁剪
}

/* ========================================================================= */
/* mbedtls 裁剪相关弱引用实现                                                 */
/* ========================================================================= */

__attribute__((weak)) void mbedtls_cipher_adapt_register_func(void *harden_cipher_func)
{
    // 空实现，mbedtls cipher adapt 功能已裁剪
    (void)harden_cipher_func;
}

/* ========================================================================= */
/* 测试套件裁剪相关弱引用实现                                                 */
/* ========================================================================= */

__attribute__((weak)) void test_suite_uart_sendf(const char *fmt, ...)
{
    // 空实现，测试套件已裁剪
    (void)fmt;  // 避免未使用参数警告
}

/* ========================================================================= */
/* IoT Cloud BLE 回调弱引用实现                                               */
/* ========================================================================= */

__attribute__((weak)) int system_app_enable_ble(void)
{
    // IoT Cloud BLE 启用回调
    // TODO: 如果需要真正的 BLE 功能，需要在这里调用 btservice API
    return 0; // 返回 0 表示成功
}

__attribute__((weak)) int system_app_disable_ble(void)
{
    return 0;
}

/* ========================================================================= */
/* ROM 组件裁剪：蓝牙和相关功能弱引用实现                                      */
/* ========================================================================= */

__attribute__((weak)) void aes_add_round_key(void) { }
__attribute__((weak)) void aes_input(void) { }
__attribute__((weak)) void aes_key_expansion(void) { }
__attribute__((weak)) void aes_output(void) { }

__attribute__((weak)) int bgsub_pmu_init(void) { return 0; }
__attribute__((weak)) int bgtp_get_lowpower_clock_freq(void) { return 0; }
__attribute__((weak)) void bgtp_prevent_sleep_set(void) { }
__attribute__((weak)) void bgtp_sleep_time_calc(void) { }
__attribute__((weak)) void bgtp_start_sleep(void) { }

__attribute__((weak)) void ble_acl_calc_drift(void) { }
__attribute__((weak)) int ble_sca_get_cfg(void) { return 0; }
__attribute__((weak)) void bt_crypto_thread_handle(void) { }
__attribute__((weak)) void bt_rf_cali_rx_content(void) { }

__attribute__((weak)) void chnl_calc_ble_chnl_cls_to_chnl_map(void) { }
__attribute__((weak)) int chnl_calc_get_valid_map_num(void) { return 0; }

__attribute__((weak)) void coex_ble_acl_create_coex_process(void) { }
__attribute__((weak)) void coex_ble_acl_end_coex_process(void) { }
__attribute__((weak)) void coex_ble_acl_evt_coex_process(void) { }
__attribute__((weak)) void coex_ble_acl_start_coex_process(void) { }
__attribute__((weak)) void coex_ble_acl_state_set(void) { }

__attribute__((weak)) void coex_ble_adv_end_coex_process(void) { }
__attribute__((weak)) void coex_ble_adv_evt_coex_process(void) { }
__attribute__((weak)) void coex_ble_adv_start_coex_process(void) { }
__attribute__((weak)) void coex_ble_adv_state_set(void) { }

__attribute__((weak)) void coex_ble_scan_end_coex_process(void) { }
__attribute__((weak)) void coex_ble_scan_evt_coex_process(void) { }
__attribute__((weak)) void coex_ble_scan_start_coex_process(void) { }
__attribute__((weak)) void coex_ble_scan_state_set(void) { }

__attribute__((weak)) void coex_if_btstate_set(void) { }

__attribute__((weak)) int dm_ble_get_dft_data_ext_global_val(void) { return 0; }
__attribute__((weak)) int dm_ble_get_local_feats(void) { return 0; }
__attribute__((weak)) int dm_ble_get_local_states(void) { return 0; }
__attribute__((weak)) int dm_ble_get_pre_tbl(void) { return 0; }

__attribute__((weak)) int dm_co_get_local_supp_cmds_tbl(void) { return 0; }
__attribute__((weak)) int dm_co_get_local_supp_feats_tbl(void) { return 0; }
__attribute__((weak)) int dm_co_get_pre_tbl(void) { return 0; }
__attribute__((weak)) int dm_co_init(void) { return 0; }
__attribute__((weak)) void dm_co_reset(void) { }

__attribute__((weak)) void dts_bg_veto_sys_sleep(void) { }
__attribute__((weak)) void dts_bg_veto_sys_wakeup(void) { }
__attribute__((weak)) void dts_ble_intc_delete(void) { }
__attribute__((weak)) int dts_ble_intc_init(void) { return 0; }
__attribute__((weak)) void dts_ble_int_clear_pending_irq(void) { }
__attribute__((weak)) void dts_coex_exit_bgle_lowpower_process(void) { }
__attribute__((weak)) void dts_disable_int(void) { }
__attribute__((weak)) void dts_enable_int(void) { }
__attribute__((weak)) void dts_gle_intc_delete(void) { }
__attribute__((weak)) int dts_gle_intc_init(void) { return 0; }
__attribute__((weak)) void dts_gle_int_clear_pending_irq(void) { }
__attribute__((weak)) void dts_hci_write_to_self(void) { }
__attribute__((weak)) int dts_interrupt_init(void) { return 0; }
__attribute__((weak)) int dts_lowpower_feature_get(void) { return 0; }
__attribute__((weak)) void dts_task_create(void) { }
__attribute__((weak)) void dts_task_destroy(void) { }

__attribute__((weak)) void em_offset_calc(void) { }

__attribute__((weak)) void encrypt_clear_sm3_tasks(void) { }
__attribute__((weak)) void encrypt_init_sm3(void) { }

__attribute__((weak)) int es_finetimer_get_first_target_time(void) { return 0; }
__attribute__((weak)) void es_finetimer_prog_time(void) { }
__attribute__((weak)) int es_get_ee_type(void) { return 0; }
__attribute__((weak)) int es_get_es_type(void) { return 0; }
__attribute__((weak)) int es_get_priority_by_prio_idx(void) { return 0; }

__attribute__((weak)) int evt_prog_init(void) { return 0; }
__attribute__((weak)) void evt_prog_pop_elt_match_isr(void) { }
__attribute__((weak)) void evt_prog_program_eeq(void) { }
__attribute__((weak)) void evt_sched_check_offset(void) { }
__attribute__((weak)) void evt_sched_create_intv(void) { }
__attribute__((weak)) int evt_sched_get_intv(void) { return 0; }
__attribute__((weak)) int evt_sched_get_offset(void) { return 0; }
__attribute__((weak)) void evt_sched_insert_intv(void) { }
__attribute__((weak)) void evt_sched_remove_intv(void) { }
__attribute__((weak)) void evt_sched_resched_check(void) { }
__attribute__((weak)) void evt_sched_scan_update_param(void) { }
__attribute__((weak)) void evt_sched_trigger(void) { }
__attribute__((weak)) void evt_sched_trigger_cancel(void) { }
__attribute__((weak)) void evt_sched_update_acl_param_with_instant(void) { }

__attribute__((weak)) int evt_task_ble_acl_get_llcp_ack_tbl(void) { return 0; }
__attribute__((weak)) int evt_task_ble_acl_get_llcp_desc_tbl(void) { return 0; }
__attribute__((weak)) int evt_task_ble_acl_get_pre_tbl(void) { return 0; }
__attribute__((weak)) int evt_task_ble_acl_get_sta_tbl(void) { return 0; }
__attribute__((weak)) int evt_task_ble_acl_get_unknown_llcp_tbl(void) { return 0; }
__attribute__((weak)) void evt_task_ble_acl_refresh_llcp_instant_ram(void) { }
__attribute__((weak)) void evt_task_ble_acl_update_active_tx_octets(void) { }

__attribute__((weak)) void evt_task_ble_adv_create(void) { }
__attribute__((weak)) int evt_task_ble_adv_get_dur_tbl_val(void) { return 0; }
__attribute__((weak)) int evt_task_ble_adv_get_intv_tbl_val(void) { return 0; }
__attribute__((weak)) void evt_task_ble_adv_set_adv_en(void) { }

__attribute__((weak)) void evt_task_ble_ext_adv_config_extheader_syncinfo(void) { }
__attribute__((weak)) int evt_task_ble_ext_adv_config_txpld_get_extheader_flag(void) { return 0; }
__attribute__((weak)) void evt_task_ble_ext_adv_set_adv_en(void) { }

__attribute__((weak)) int evt_task_ble_initiate_get_sta_tbl(void) { return 0; }
__attribute__((weak)) void evt_task_ble_initiate_start(void) { }

__attribute__((weak)) int evt_task_ble_scan_get_sta_tbl(void) { return 0; }
__attribute__((weak)) void evt_task_ble_scan_start(void) { }

__attribute__((weak)) void evt_task_comm_create_intv(void) { }
__attribute__((weak)) int evt_task_comm_env_get(void) { return 0; }
__attribute__((weak)) void evt_task_comm_env_set(void) { }
__attribute__((weak)) void evt_task_comm_fsm_except_handler(void) { }
__attribute__((weak)) void evt_task_comm_rx_error_record(void) { }
__attribute__((weak)) void evt_task_comm_task_end(void) { }
__attribute__((weak)) void evt_task_comm_task_start(void) { }

__attribute__((weak)) void hci_ble_rx_acl_data(void) { }
__attribute__((weak)) void hci_bt_send_evt_msg(void) { }
__attribute__((weak)) void hci_bt_send_evt_msg_without_log(void) { }

__attribute__((weak)) int hw_em_get_ble_tx_buff_free_list(void) { return 0; }
__attribute__((weak)) int hw_em_get_ble_tx_buff_node(void) { return 0; }
__attribute__((weak)) int hw_em_get_gle_tx_buff_free_list(void) { return 0; }
__attribute__((weak)) int hw_em_get_gle_tx_buff_node(void) { return 0; }
__attribute__((weak)) int hw_em_get_tx_s_buff_free_list(void) { return 0; }
__attribute__((weak)) int hw_em_get_tx_s_buff_node(void) { return 0; }
__attribute__((weak)) int hw_em_tx_desc_get_txpd_free_list(void) { return 0; }
__attribute__((weak)) int hw_em_tx_desc_get_txpd_node(void) { return 0; }

__attribute__((weak)) int is_sleep_prevent(void) { return 0; }

__attribute__((weak)) int lm_ble_acl_conn_update_get_sta_tbl(void) { return 0; }
__attribute__((weak)) void lm_ble_acl_create_desc(void) { }
__attribute__((weak)) int lm_ble_acl_enable_encryption_get_sta_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_acl_feats_get_sta_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_acl_get_exception_hci_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_acl_get_llcp_reject_ext_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_acl_get_llcp_unknown_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_acl_get_pre_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_acl_get_sta_tbl(void) { return 0; }
__attribute__((weak)) void lm_ble_acl_init_desc_for_pwr_ctrl(void) { }
__attribute__((weak)) int lm_ble_acl_map_get_sta_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_acl_phy_upd_get_sta_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_acl_srv_data_length_get_pre_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_acl_srv_data_length_get_sta_tbl(void) { return 0; }
__attribute__((weak)) void lm_ble_acl_start(void) { }
__attribute__((weak)) int lm_ble_acl_ver_get_sta_tbl(void) { return 0; }

__attribute__((weak)) void lm_ble_adv_clear_assigned_adv_act(void) { }
__attribute__((weak)) int lm_ble_adv_comm_fsm_get_sta_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_adv_fsm_get_sta_tbl(void) { return 0; }
__attribute__((weak)) void lm_ble_adv_notify_per_adv_fsm(void) { }

__attribute__((weak)) void lm_ble_ext_initiate_set_acl_con_info(void) { }
__attribute__((weak)) int lm_ble_initiate_fsm_get_sta_tbl(void) { return 0; }
__attribute__((weak)) int lm_ble_per_sync_mgr_fsm_get_id(void) { return 0; }
__attribute__((weak)) int lm_ble_scan_fsm_get_sta_tbl(void) { return 0; }

__attribute__((weak)) void lm_chnl_scan_add_map_assessment_module(void) { }
__attribute__((weak)) int lm_chnl_scan_get_ble_chnl_cls(void) { return 0; }
__attribute__((weak)) int lm_chnl_scan_get_ble_chnl_map(void) { return 0; }
__attribute__((weak)) void lm_chnl_scan_remove_map_assessment_module(void) { }

__attribute__((weak)) void mem_frees(void) { }
__attribute__((weak)) void mem_news(void) { }
__attribute__((weak)) void new0fun(void) { }

__attribute__((weak)) void pwr_ctrl_convert_tx_pwr_val_to_less_level(void) { }
__attribute__((weak)) int pwr_ctrl_get_ble_tx_estimate_power_value(void) { return 0; }

__attribute__((weak)) int rxpd_get_rx_phy(void) { return 0; }
__attribute__((weak)) int rxpd_get_rx_phy_info(void) { return 0; }

__attribute__((weak)) void sed_ble_set_rx_phy(void) { }
__attribute__((weak)) void sed_ble_set_tx_phy(void) { }

__attribute__((weak)) void smp_aes128(void) { }
__attribute__((weak)) void smp_cmac_reverse(void) { }
__attribute__((weak)) void smp_rand(void) { }
__attribute__((weak)) void smp_reverse_octets(void) { }
__attribute__((weak)) void smp_xor(void) { }

__attribute__((weak)) void txpd_ble_adv_set_txphy(void) { }

/* ========================================================================= */
/* 安全驱动裁剪相关弱引用实现                                                 */
/* ========================================================================= */

__attribute__((weak)) void uapi_drv_cipher_env_init(void)
{
    // Stub implementation for removed security_unified component
}

__attribute__((weak)) int32_t mbedtls_adapt_register_func(void)
{
    // Stub implementation for removed security_unified component
    return 0;
}

__attribute__((weak)) int uapi_drv_cipher_trng_get_random(unsigned char *output, unsigned int len)
{
    // Stub implementation for removed security_unified component
    (void)output;
    (void)len;
    return 0;
}
