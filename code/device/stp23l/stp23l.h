#ifndef _DEVICE_STP23L_H_
#define _DEVICE_STP23L_H_

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
// header: 4*preamble + addr(1) + cmd(1) + offset(2) + len(2) = 10 bytes
#define STP23L_HEADER_SIZE 10

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
    uint16_t noise;     // 环境噪声
    uint32_t peak;      // 光强度
    uint8_t confidence; // 由环境噪声和接收强度信息融合后的测量点的可信度
    uint32_t intg;      // 当前传感器测量的积分次数
    int16_t reftof;     // 芯片内部温度
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

    // 设备对象，保存每个设备的解析状态与最新数据
    typedef struct stp23l_obj_t
    {
        uart_index_enum uartn;
        // 接收解析缓冲
        uint8_t rx_buf[STP23L_RX_BUF_SIZE];
        int rx_idx;
        int parser_state;
        int preamble_count;
        uint8_t cmd;
        uint16_t data_len;

        // 最新数据与同步标志
        stp23l_frame_t latest_frame;
        volatile uint8_t frame_seq; // 0: stable, 1: writing

        stp23l_ack_result_t latest_ack;
        volatile uint8_t ack_seq;
    } stp23l_obj_t;

    stp23l_obj_t stp23l_init(uart_index_enum uartn, uart_rx_pin_enum rx_pin, uart_tx_pin_enum tx_pin, vuint32 baud);

    // 控制 / 查询接口，均需传入设备对象
    int stp23l_send_reset_cmd(stp23l_obj_t *dev);
    int stp23l_get_version(stp23l_obj_t *dev);
    int stp23l_get_start(stp23l_obj_t *dev);
    int stp23l_send_stop_cmd(stp23l_obj_t *dev);

    // 读取最新数据（从设备对象中复制出来）
    int stp23l_pop_frame(stp23l_obj_t *dev, stp23l_frame_t *out);
    int stp23l_pop_ack(stp23l_obj_t *dev, stp23l_ack_result_t *out);

    // receiver callback（由串口中断/驱动层调用，传设备对象指针）
    void stp23l_receiver_callback(stp23l_obj_t *dev);

#ifdef __cplusplus
}
#endif

#endif // _STP23L_H_
