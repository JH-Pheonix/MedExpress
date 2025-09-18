#include "emm42.h"
#include "string.h"

// CRC-8 (poly 0x07, init 0x00, MSB-first)
static vuint8 emm42_crc8(const vuint8 *data, vuint8 len)
{
    vuint8 crc = 0x00;
    for (vuint8 i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (vuint8 j = 0; j < 8; j++)
        {
            if (crc & 0x80)
            {
                crc = (vuint8)((crc << 1) ^ 0x07);
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

emm42_obj_t emm42_init(uart_index_enum uartn, uart_rx_pin_enum rx_pin, uart_tx_pin_enum tx_pin, vuint32 baud, emm42_checksum_mode_t checksum_mode)
{
    emm42_obj_t emm42;
    emm42.uartn = uartn;
    emm42.tx_data_cnt = 0;
    emm42.rx_data_cnt = 0;

    memset(emm42.rx_buf, 0, EMM42_RX_BUF_SIZE);
    memset(emm42.tx_buf, 0, EMM42_TX_BUF_SIZE);

    emm42.checksum_mode = checksum_mode;

    uart_init(uartn, baud, tx_pin, rx_pin);
    uart_rx_interrupt(uartn, 1);

    return emm42;
}

void emm42_send_cmd(emm42_obj_t *device)
{
    // 根据 checksum_mode 计算并追加校验字节
    vuint8 checksum = 0;
    if (device->tx_data_cnt == 0)
    {
        return; // nothing to send
    }

    switch (device->checksum_mode)
    {
    case EMM42_CHKSUM_CONST_6B:
        checksum = 0x6B;
        break;
    case EMM42_CHKSUM_XOR:
    {
        vuint8 xr = 0x00;
        for (vuint8 i = 0; i < device->tx_data_cnt; i++)
        {
            xr ^= device->tx_buf[i];
        }
        checksum = xr;
        break;
    }
    case EMM42_CHKSUM_CRC8:
        checksum = emm42_crc8(device->tx_buf, device->tx_data_cnt);
        break;
    default:
        checksum = 0x6B;
        break;
    }

    device->tx_buf[device->tx_data_cnt] = checksum;
    device->tx_data_cnt += 1;

    uart_write_buffer(device->uartn, device->tx_buf, device->tx_data_cnt);
}

void emm42_enable_cmd(emm42_obj_t *device, vuint8 addr, vuint8 enable_state, vuint8 sync_flag)
{
    // 使能 01， 不使能 00
    memset(device->tx_buf, 0, EMM42_TX_BUF_SIZE);
    device->tx_buf[0] = addr;
    device->tx_buf[1] = 0xF3;
    device->tx_buf[2] = 0xAB;
    device->tx_buf[3] = enable_state;
    device->tx_buf[4] = sync_flag;

    device->tx_data_cnt = 5;
}

// 01 F6 01 05 DC 0A 00 6B
void emm42_velocity_mode_cmd(emm42_obj_t *device, vuint8 addr, vuint8 dir, vuint16 vel, vuint8 acc, vuint8 sync_flag)
{
    memset(device->tx_buf, 0, EMM42_TX_BUF_SIZE);
    device->tx_buf[0] = addr;
    device->tx_buf[1] = 0xF6;
    device->tx_buf[2] = dir;
    device->tx_buf[3] = (vuint8)(vel & 0x00FF);
    device->tx_buf[4] = (vuint8)((vel >> 8) & 0x00FF);
    device->tx_buf[5] = acc;
    device->tx_buf[6] = sync_flag;

    device->tx_data_cnt = 7;
}

void emm42_position_mode_cmd(emm42_obj_t *device, vuint8 addr, vuint8 dir, vuint16 vel, vuint8 acc, vuint32 pulse_cnt, vuint8 mode, vuint8 sync_flag)
{
    // 3200个脉冲电机旋转一圈
    memset(device->tx_buf, 0, EMM42_TX_BUF_SIZE);
    device->tx_buf[0] = addr;
    device->tx_buf[1] = 0xFD;
    device->tx_buf[2] = dir;
    device->tx_buf[3] = (vuint8)((vel >> 8) & 0x00FF);
    device->tx_buf[4] = (vuint8)(vel & 0x00FF);
    device->tx_buf[5] = acc;
    device->tx_buf[6] = (vuint8)((pulse_cnt >> 24) & 0x000000FF);
    device->tx_buf[7] = (vuint8)((pulse_cnt >> 16) & 0x000000FF);
    device->tx_buf[8] = (vuint8)((pulse_cnt >> 8) & 0x000000FF);
    device->tx_buf[9] = (vuint8)(pulse_cnt & 0x000000FF);
    device->tx_buf[10] = mode;
    device->tx_buf[11] = sync_flag;

    device->tx_data_cnt = 12;
}

void emm42_stop_cmd(emm42_obj_t *device, vuint8 addr, vuint8 sync_flag)
{
    memset(device->tx_buf, 0, EMM42_TX_BUF_SIZE);
    device->tx_buf[0] = addr;
    device->tx_buf[1] = 0xFE;
    device->tx_buf[2] = 0x98;
    device->tx_buf[3] = sync_flag;

    device->tx_data_cnt = 4;
}

void emm42_encoder_calib(emm42_obj_t *device, vuint8 addr)
{
    memset(device->tx_buf, 0, EMM42_TX_BUF_SIZE);
    device->tx_buf[0] = addr;
    device->tx_buf[1] = 0x06;
    device->tx_buf[2] = 0x45;

    device->tx_data_cnt = 3;
}