#include "stp23l.h"
#include "pin.h"
#include <string.h>

stp23l_obj_t stp23l_init(uart_index_enum uartn, uart_rx_pin_enum rx_pin, uart_tx_pin_enum tx_pin, vuint32 baud)
{
    stp23l_obj_t dev;
    memset(&dev, 0, sizeof(dev));

    dev.uartn = uartn;
    memset(dev.rx_buf, 0, STP23L_RX_BUF_SIZE);
    dev.rx_idx = 0;
    dev.parser_state = S_WAIT_PREAMBLE;
    dev.preamble_count = 0;
    dev.cmd = 0;
    dev.data_len = 0;
    dev.frame_seq = 0;
    dev.ack_seq = 0;

    uart_init(uartn, baud, tx_pin, rx_pin);
    uart_rx_interrupt(uartn, 1); // 使能接收中断，回调层需要把 dev 传入中断处理

    return dev;
}

// write helpers per-device
static void stp23l_push_frame(stp23l_obj_t *dev, const stp23l_frame_t *f)
{
    dev->frame_seq = 1;
    dev->latest_frame = *f;
    dev->frame_seq = 0;
}

static void stp23l_push_ack(stp23l_obj_t *dev, const stp23l_ack_result_t *a)
{
    dev->ack_seq = 1;
    dev->latest_ack = *a;
    dev->ack_seq = 0;
}

// helper to reset parser state for a device
static void stp23l_reset_parser(stp23l_obj_t *dev)
{
    dev->parser_state = S_WAIT_PREAMBLE;
    dev->preamble_count = 0;
    dev->rx_idx = 0;
    dev->data_len = 0;
}

// 校验和计算
static uint8_t stp23l_calc_checksum(uint8_t sum_init,
                                    const uint8_t *data,
                                    int len)
{
    uint8_t sum = sum_init;
    for (int i = 0; i < len; ++i)
        sum += data[i];
    return sum;
}

// 构建协议数据包
static int stp23l_build_packet(uint8_t device_addr,
                               uint8_t cmd,
                               uint16_t offset,
                               const uint8_t *data,
                               uint16_t data_len,
                               uint8_t *out_buf)
{
    int idx = 0;
    out_buf[idx++] = 0xAA;
    out_buf[idx++] = 0xAA;
    out_buf[idx++] = 0xAA;
    out_buf[idx++] = 0xAA;
    out_buf[idx++] = device_addr;
    out_buf[idx++] = cmd;
    out_buf[idx++] = (uint8_t)(offset & 0xFF);
    out_buf[idx++] = (uint8_t)((offset >> 8) & 0xFF);
    out_buf[idx++] = (uint8_t)(data_len & 0xFF);
    out_buf[idx++] = (uint8_t)((data_len >> 8) & 0xFF);
    if (data_len && data)
    {
        // ensure we don't copy from a NULL pointer
        memcpy(&out_buf[idx], data, data_len);
        idx += data_len;
    }
    out_buf[idx++] = stp23l_calc_checksum(0, &out_buf[4], 6 + data_len);
    return idx;
}

// 发送命令包
static int stp23l_send_cmd(uart_index_enum uartn,
                           uint8_t device_addr,
                           uint8_t cmd,
                           uint16_t offset,
                           const uint8_t *data,
                           uint16_t data_len)
{
    uint8_t tx_buf[STP23L_TX_BUF_SIZE];

    int tx_len =
        stp23l_build_packet(device_addr, cmd, offset, data, data_len, tx_buf);
    uart_write_buffer(uartn, tx_buf, tx_len);
    return tx_len;
}

static int stp23l_parse_frame(const uint8_t *payload, int payload_len, stp23l_frame_t *frame_data)
{
    const int per_point_bytes = 2 + 2 + 4 + 1 + 4 + 2;              /* 15 */
    const int expected_len = LIDAR_POINT_NUM * per_point_bytes + 4; /* +4 for timestamp */

    if (payload == NULL || frame_data == NULL)
        return -1;

    if (payload_len < expected_len)
        return -1;

    for (int i = 0; i < LIDAR_POINT_NUM; ++i)
    {
        int p = i * per_point_bytes;

        int16_t distance = (int16_t)((uint16_t)payload[p] | ((uint16_t)payload[p + 1] << 8));
        uint16_t noise = (uint16_t)payload[p + 2] | ((uint16_t)payload[p + 3] << 8);
        uint32_t peak = (uint32_t)payload[p + 4] | ((uint32_t)payload[p + 5] << 8) | ((uint32_t)payload[p + 6] << 16) | ((uint32_t)payload[p + 7] << 24);
        uint8_t confidence = payload[p + 8];
        uint32_t intg = (uint32_t)payload[p + 9] | ((uint32_t)payload[p + 10] << 8) | ((uint32_t)payload[p + 11] << 16) | ((uint32_t)payload[p + 12] << 24);
        int16_t reftof = (int16_t)((uint16_t)payload[p + 13] | ((uint16_t)payload[p + 14] << 8));

        frame_data->points[i].distance = distance;
        frame_data->points[i].noise = noise;
        frame_data->points[i].peak = peak;
        frame_data->points[i].confidence = confidence;
        frame_data->points[i].intg = intg;
        frame_data->points[i].reftof = reftof;
    }

    int ts_off = LIDAR_POINT_NUM * per_point_bytes;
    uint32_t timestamp = (uint32_t)payload[ts_off] | ((uint32_t)payload[ts_off + 1] << 8) | ((uint32_t)payload[ts_off + 2] << 16) | ((uint32_t)payload[ts_off + 3] << 24);
    frame_data->timestamp = timestamp;

    return 0;
}

static int stp23l_parse_ack(const uint8_t *payload, int payload_len, stp23l_ack_result_t *ack)
{
    if (payload == NULL || ack == NULL)
        return -1;

    if (payload_len < 2)
        return -1;

    ack->ack_cmd_id = payload[0];
    ack->result = payload[1];
    return 0;
}

static int stp23l_process_data(stp23l_obj_t *dev, const uint8_t *payload, int payload_len, uint8_t cmd)
{
    switch (cmd)
    {
    case STP23L_GET_DISTANCE:
    {
        stp23l_frame_t temp;
        if (stp23l_parse_frame(payload, payload_len, &temp) == 0)
        {
            stp23l_push_frame(dev, &temp);
            return 0;
        }
        break;
    }
    case STP23L_VERSION:
    {
        stp23l_ack_result_t temp;
        if (stp23l_parse_ack(payload, payload_len, &temp) == 0)
        {
            stp23l_push_ack(dev, &temp);
            return 0;
        }
        break;
    }
    default:
        break;
    }
    return -1;
}

// 开始测量数据命令
int stp23l_get_start(stp23l_obj_t *dev)
{
    if (!dev)
        return -1;
    return stp23l_send_cmd(dev->uartn, 0, STP23L_GET_DISTANCE, 0, NULL, 0);
}

int stp23l_send_stop_cmd(stp23l_obj_t *dev)
{
    if (!dev)
        return -1;
    return stp23l_send_cmd(dev->uartn, 0, STP23L_STOP, 0, NULL, 0);
}

// 复位命令
int stp23l_send_reset_cmd(stp23l_obj_t *dev)
{
    if (!dev)
        return -1;
    return stp23l_send_cmd(dev->uartn, 0, STP23L_RESET_SYSTEM, 0, NULL, 0);
}

// 获取传感器信息
int stp23l_get_version(stp23l_obj_t *dev)
{
    if (!dev)
        return -1;
    return stp23l_send_cmd(dev->uartn, 0, STP23L_VERSION, 0, NULL, 0);
}

// per-device 字节喂入处理
static void stp23l_feed_byte(stp23l_obj_t *dev, uint8_t byte)
{
    switch (dev->parser_state)
    {
    case S_WAIT_PREAMBLE:
        if (byte == STP23L_PREAMBLE)
        {
            dev->preamble_count = 1;
            dev->rx_idx = 0;
            memset(dev->rx_buf, 0, STP23L_RX_BUF_SIZE);
            dev->rx_buf[dev->rx_idx++] = byte;
            dev->parser_state = S_PREAMBLE_COUNT;
        }
        break;
    case S_PREAMBLE_COUNT:
        if (byte == STP23L_PREAMBLE && dev->preamble_count < 4)
        {
            dev->rx_buf[dev->rx_idx++] = byte;
            dev->preamble_count++;
            if (dev->preamble_count == 4)
                dev->parser_state = S_ADDR;
        }
        else
        {
            if (byte == STP23L_PREAMBLE)
            {
                dev->preamble_count = 1;
                dev->rx_idx = 0;
                memset(dev->rx_buf, 0, STP23L_RX_BUF_SIZE);
                dev->rx_buf[dev->rx_idx++] = byte;
                dev->parser_state = S_PREAMBLE_COUNT;
            }
            else
            {
                stp23l_reset_parser(dev);
            }
        }
        break;
    case S_ADDR:
        dev->rx_buf[dev->rx_idx++] = byte; // device addr
        dev->parser_state = S_CMD;
        break;
    case S_CMD:
        dev->cmd = byte;
        dev->rx_buf[dev->rx_idx++] = byte;
        dev->parser_state = S_OFFSET_LO;
        break;
    case S_OFFSET_LO:
        dev->rx_buf[dev->rx_idx++] = byte;
        dev->parser_state = S_OFFSET_HI;
        break;
    case S_OFFSET_HI:
        dev->rx_buf[dev->rx_idx++] = byte;
        dev->parser_state = S_LEN_LO;
        break;
    case S_LEN_LO:
        dev->rx_buf[dev->rx_idx++] = byte;
        dev->data_len = byte;
        dev->parser_state = S_LEN_HI;
        break;
    case S_LEN_HI:
        dev->rx_buf[dev->rx_idx++] = byte;
        dev->data_len |= ((uint16_t)byte << 8);
        if (dev->data_len > (uint16_t)(STP23L_RX_BUF_SIZE - STP23L_HEADER_SIZE - 1))
        {
            stp23l_reset_parser(dev);
            break;
        }
        if (dev->data_len == 0)
            dev->parser_state = S_CHECKSUM;
        else
            dev->parser_state = S_PAYLOAD;
        break;
    case S_PAYLOAD:
        if (dev->rx_idx < STP23L_RX_BUF_SIZE)
        {
            dev->rx_buf[dev->rx_idx++] = byte;
        }
        if (dev->rx_idx >= STP23L_HEADER_SIZE + dev->data_len)
        {
            dev->parser_state = S_CHECKSUM;
        }
        break;
    case S_CHECKSUM:
        if (dev->rx_idx < STP23L_RX_BUF_SIZE)
            dev->rx_buf[dev->rx_idx++] = byte;

        if (dev->rx_idx > 4)
        {
            uint8_t calc = stp23l_calc_checksum(0, &dev->rx_buf[4], (int)(dev->rx_idx - 5));
            uint8_t recv = dev->rx_buf[dev->rx_idx - 1];
            if (calc == recv)
            {
                int payload_offset = STP23L_HEADER_SIZE;
                int payload_len = dev->data_len;
                if (payload_offset + payload_len <= dev->rx_idx - 1)
                {
                    stp23l_process_data(dev, &dev->rx_buf[payload_offset], payload_len, dev->cmd);
                }
            }
        }

        stp23l_reset_parser(dev);
        break;
    default:
        stp23l_reset_parser(dev);
        break;
    }
}

// 串口回调：现在直接接收设备对象指针，由调用者（注册中断时）传入对应的对象
void stp23l_receiver_callback(stp23l_obj_t *dev)
{
    if (!dev)
        return;
    uint8_t byte;
    while (uart_query_byte(dev->uartn, &byte))
    {
        stp23l_feed_byte(dev, byte);
    }
}

// 读取最新数据（从设备对象中复制出来）
int stp23l_pop_frame(stp23l_obj_t *dev, stp23l_frame_t *out)
{
    if (!dev || !out)
        return 0;
    uint8_t s1, s2;
    do
    {
        s1 = dev->frame_seq;
        if (s1 != 0)
            continue;
        memcpy(out, &dev->latest_frame, sizeof(*out));
        s2 = dev->frame_seq;
    } while (s1 != s2);
    return 1;
}

int stp23l_pop_ack(stp23l_obj_t *dev, stp23l_ack_result_t *out)
{
    if (!dev || !out)
        return 0;
    uint8_t s1, s2;
    do
    {
        s1 = dev->ack_seq;
        if (s1 != 0)
            continue;
        memcpy(out, &dev->latest_ack, sizeof(*out));
        s2 = dev->ack_seq;
    } while (s1 != s2);
    return 1;
}