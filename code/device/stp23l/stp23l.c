#include "stp23l.h"
#include "zf_driver_uart.h"

vuint8 receive_data_buffer[STP23L_RX_BUF_SIZE];
vuint8 receive_data_count = 0;

// 环形队列参数
#define STP23L_FRAME_QUEUE_SIZE 8
#define STP23L_ACK_QUEUE_SIZE 4
static stp23l_frame_t frame_queue[STP23L_FRAME_QUEUE_SIZE];
static volatile int frame_queue_head = 0;
static volatile int frame_queue_tail = 0;
static stp23l_ack_result_t ack_queue[STP23L_ACK_QUEUE_SIZE];
static volatile int ack_queue_head = 0;
static volatile int ack_queue_tail = 0;

// 入队（中断/回调里调用）
static void stp23l_push_frame(const stp23l_frame_t *f)
{
    int next = (frame_queue_head + 1) % STP23L_FRAME_QUEUE_SIZE;
    if (next != frame_queue_tail)
    {
        frame_queue[frame_queue_head] = *f;
        frame_queue_head = next;
    }
}
static void stp23l_push_ack(const stp23l_ack_result_t *a)
{
    int next = (ack_queue_head + 1) % STP23L_ACK_QUEUE_SIZE;
    if (next != ack_queue_tail)
    {
        ack_queue[ack_queue_head] = *a;
        ack_queue_head = next;
    }
}

// 出队（主循环里调用）
int stp23l_pop_frame(stp23l_frame_t *out)
{
    if (frame_queue_tail == frame_queue_head)
        return 0;
    *out = frame_queue[frame_queue_tail];
    frame_queue_tail = (frame_queue_tail + 1) % STP23L_FRAME_QUEUE_SIZE;
    return 1;
}
int stp23l_pop_ack(stp23l_ack_result_t *out)
{
    if (ack_queue_tail == ack_queue_head)
        return 0;
    *out = ack_queue[ack_queue_tail];
    ack_queue_tail = (ack_queue_tail + 1) % STP23L_ACK_QUEUE_SIZE;
    return 1;
}

static int stp23l_parser_state = S_WAIT_PREAMBLE;
static int stp23l_preamble_count = 0;
static uint8_t stp23l_cmd = 0;
static uint16_t stp23l_data_len = 0;
static int stp23l_buf_idx = 0;

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
    if (payload_len < (int)sizeof(stp23l_frame_t))
        return -1;
    memcpy(frame_data, payload, sizeof(stp23l_frame_t));
    return 0;
}

static int stp23l_parse_ack(const uint8_t *payload, int payload_len, stp23l_ack_result_t *ack)
{
    if (payload_len < (int)sizeof(stp23l_ack_result_t))
        return -1;
    memcpy(ack, payload, sizeof(stp23l_ack_result_t));
    return 0;
}

// 3. 解析到数据时调用回调
static int stp23l_process_data(const uint8_t *payload, int payload_len, uint8_t cmd)
{
    switch (cmd)
    {
    case STP23L_GET_DISTANCE:
    {
        stp23l_frame_t temp;
        if (stp23l_parse_frame(payload, payload_len, &temp) == 0)
        {
            stp23l_push_frame(&temp);
            return 0;
        }
        break;
    }
    case STP23L_VERSION:
    {
        stp23l_ack_result_t temp;
        if (stp23l_parse_ack(payload, payload_len, &temp) == 0)
        {
            stp23l_push_ack(&temp);
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
int stp23l_get_start(uart_index_enum uartn)
{
    return stp23l_send_cmd(uartn, 0, STP23L_GET_DISTANCE, 0, NULL, 0);
}

// 停止命令
int stp23l_send_stop_cmd(uart_index_enum uartn)
{
    return stp23l_send_cmd(uartn, 0, STP23L_STOP, 0, NULL, 0);
}

// 复位命令
int stp23l_send_reset_cmd(uart_index_enum uartn)
{
    return stp23l_send_cmd(uartn, 0, STP23L_RESET_SYSTEM, 0, NULL, 0);
}

// 获取传感器信息
int stp23l_get_version(uart_index_enum uartn)
{
    return stp23l_send_cmd(uartn, 0, STP23L_VERSION, 0, NULL, 0);
}

void stp23l_feed_byte(uint8_t byte)
{
    switch (stp23l_parser_state)
    {
    case S_WAIT_PREAMBLE:
        if (byte == STP23L_PREAMBLE)
        {
            stp23l_preamble_count = 1;
            stp23l_buf_idx = 0;
            memset(receive_data_buffer, 0, STP23L_RX_BUF_SIZE);
            receive_data_buffer[stp23l_buf_idx++] = byte;
            stp23l_parser_state = S_PREAMBLE_COUNT;
        }
        break;
    case S_PREAMBLE_COUNT:
        if (byte == STP23L_PREAMBLE && stp23l_preamble_count < 4)
        {
            receive_data_buffer[stp23l_buf_idx++] = byte;
            stp23l_preamble_count++;
            if (stp23l_preamble_count == 4)
                stp23l_parser_state = S_ADDR;
        }
        else
        {
            if (byte == STP23L_PREAMBLE)
            {
                stp23l_preamble_count = 1;
                stp23l_buf_idx = 0;
                memset(receive_data_buffer, 0, STP23L_RX_BUF_SIZE);
                receive_data_buffer[stp23l_buf_idx++] = byte;
                stp23l_parser_state = S_PREAMBLE_COUNT;
            }
            else
            {
                stp23l_parser_state = S_WAIT_PREAMBLE;
            }
        }
        break;
    case S_ADDR:
        receive_data_buffer[stp23l_buf_idx++] = byte; // device addr
        stp23l_parser_state = S_CMD;
        break;
    case S_CMD:
        stp23l_cmd = byte;
        receive_data_buffer[stp23l_buf_idx++] = byte;
        stp23l_parser_state = S_OFFSET_LO;
        break;
    case S_OFFSET_LO:
        receive_data_buffer[stp23l_buf_idx++] = byte;
        stp23l_parser_state = S_OFFSET_HI;
        break;
    case S_OFFSET_HI:
        receive_data_buffer[stp23l_buf_idx++] = byte;
        stp23l_parser_state = S_LEN_LO;
        break;
    case S_LEN_LO:
        receive_data_buffer[stp23l_buf_idx++] = byte;
        stp23l_data_len = byte;
        stp23l_parser_state = S_LEN_HI;
        break;
    case S_LEN_HI:
        receive_data_buffer[stp23l_buf_idx++] = byte;
        stp23l_data_len |= ((uint16_t)byte << 8);
        if (stp23l_data_len > STP23L_RX_BUF_SIZE - 11)
        {
            // payload too large for buffer, reset
            stp23l_parser_state = S_WAIT_PREAMBLE;
            stp23l_preamble_count = 0;
            stp23l_buf_idx = 0;
            stp23l_data_len = 0;
            break;
        }
        if (stp23l_data_len == 0)
            stp23l_parser_state = S_CHECKSUM; // no payload
        else
            stp23l_parser_state = S_PAYLOAD;
        break;
    case S_PAYLOAD:
        receive_data_buffer[stp23l_buf_idx++] = byte;
        if (stp23l_buf_idx >= 4 + 1 + 1 + 2 + 2 + stp23l_data_len) // header(4)+addr+cmd+offset(2)+len(2)+payload
        {
            stp23l_parser_state = S_CHECKSUM;
        }
        break;
    case S_CHECKSUM:
        receive_data_buffer[stp23l_buf_idx++] = byte; // checksum at end

        {
            uint8_t calc = stp23l_calc_checksum(0, &receive_data_buffer[4], (int)(stp23l_buf_idx - 5));
            uint8_t recv = receive_data_buffer[stp23l_buf_idx - 1];
            if (calc == recv)
            {
                // index 10 (4 preamble + addr(1)+cmd(1)+offset(2)+len(2))
                int payload_offset = 10;
                int payload_len = stp23l_data_len;
                if (payload_offset + payload_len <= stp23l_buf_idx - 1)
                {
                    stp23l_process_data(&receive_data_buffer[payload_offset], payload_len, stp23l_cmd);
                }
            }
        }

        // reset
        stp23l_parser_state = S_WAIT_PREAMBLE;
        stp23l_preamble_count = 0;
        stp23l_buf_idx = 0;
        stp23l_data_len = 0;
        break;
    default:
        stp23l_parser_state = S_WAIT_PREAMBLE;
        break;
    }
}

// 放在串口中断里面
void stp23l_receiver_callback(uart_index_enum uartn)
{
    uint8_t byte;
    while (uart_query_byte(uartn, &byte))
    {
        stp23l_feed_byte(byte);
    }
}