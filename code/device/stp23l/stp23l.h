#ifndef _STP23L_H_
#define _STP23L_H_

#include "zf_driver_uart.h"

#define STP23L_PREAMBLE 0xAA
#define STP23L_GET_DISTANCE 0X02
#define STP23L_RESET_SYSTEM 0X0D
#define STP23L_STOP 0x0F
#define STP23L_ACK 0x10
#define STP23L_VERSION 0x14

#define LIDAR_POINT_NUM 12

#define STP23L_RX_BUF_SIZE 256
#define STP23L_TX_BUF_SIZE 64

enum stp23l_parser_state_e
{
    S_WAIT_PREAMBLE = 0,
    S_PREAMBLE_COUNT,
    S_ADDR,
    S_CMD,
    S_OFFSET_LO,
    S_OFFSET_HI,
    S_LEN_LO,
    S_LEN_HI,
    S_PAYLOAD,
    S_CHECKSUM
};

typedef struct
{
    uint8_t device_addr;
    uint8_t cmd;
    uint16_t offset;
    uint16_t data_len;
    const uint8_t *data;
} stp23l_packet_info_t;

// #pragma pack(push, 1) // 强制编译器按1字节对齐
typedef struct
{
    int16_t distance;
    uint16_t noise;
    uint32_t peak;
    uint8_t confidence;
    uint32_t intg;
    int16_t reftof;
} stp23l_data_t;
// #pragma pack(pop)

typedef struct
{
    stp23l_data_t points[LIDAR_POINT_NUM];
    uint32_t timestamp;
} stp23l_frame_t;

typedef struct
{
    uint8_t ack_cmd_id;
    uint8_t result; // 1成功，0失败
} stp23l_ack_result_t;

#ifdef __cplusplus
extern "C"
{
#endif

    int stp23l_send_reset_cmd(uart_index_enum uartn);
    int stp23l_get_version(uart_index_enum uartn);
    int stp23l_pop_frame(stp23l_frame_t *out);
    int stp23l_pop_ack(stp23l_ack_result_t *out);
    void stp23l_feed_byte(uint8_t byte);
    void stp23l_receiver_callback(uart_index_enum uartn);
    void test();
#ifdef __cplusplus
}
#endif

#endif // _STP23L_H_
