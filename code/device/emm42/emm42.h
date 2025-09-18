#ifndef _DEVICE_EMM42_H_
#define _DEVICE_EMM42_H_

#include "zf_common_typedef.h"
#include "zf_driver_uart.h"

#define EMM42_RX_BUF_SIZE 64
#define EMM42_TX_BUF_SIZE 64

typedef enum
{
    EMM42_CHKSUM_CONST_6B = 0,
    EMM42_CHKSUM_XOR,
    EMM42_CHKSUM_CRC8
} emm42_checksum_mode_t;

typedef struct
{
    uart_index_enum uartn;
    vuint8 rx_buf[EMM42_RX_BUF_SIZE];
    vuint8 tx_buf[EMM42_TX_BUF_SIZE];
    vuint8 tx_data_cnt;
    vuint8 rx_data_cnt;
    emm42_checksum_mode_t checksum_mode; // 校验模式
} emm42_obj_t;

emm42_obj_t emm42_init(uart_index_enum uartn, uart_rx_pin_enum rx_pin, uart_tx_pin_enum tx_pin, vuint32 baud, emm42_checksum_mode_t checksum_mode);

void emm42_enable_cmd(emm42_obj_t *device, vuint8 addr, vuint8 enable_state, vuint8 sync_flag);
void emm42_velocity_mode_cmd(emm42_obj_t *device, vuint8 addr, vuint8 dir, vuint16 vel, vuint8 acc, vuint8 sync_flag);
void emm42_position_mode_cmd(emm42_obj_t *device, vuint8 addr, vuint8 dir, vuint16 vel, vuint8 acc, vuint32 pulse_cnt, vuint8 mode, vuint8 sync_flag);
void emm42_stop_cmd(emm42_obj_t *device, vuint8 addr, vuint8 sync_flag);
void emm42_encoder_calib(emm42_obj_t *device, vuint8 addr);

void emm42_send_cmd(emm42_obj_t *device);

void emm42_parse_encoder_data(emm42_obj_t *device);
void emm42_uart_handler(emm42_obj_t *device);

#endif