#include "init.h"

#define PIT_KEY_T 10
#define PIT_ATTITUDE_T 2
#define PIT_CONTROL_T 10

motor_obj_t motor1;
motor_obj_t motor2;
motor_obj_t motor3;
motor_obj_t motor4;

stp23l_obj_t lidar1;
stp23l_obj_t lidar2;
stp23l_obj_t lidar3;

maixcam_obj_t maixcam1;

asrpro_obj_t asrpro1;

emm42_obj_t emm42_1;

servo_obj_t servo_left;
servo_obj_t servo_right;

MAX30102_obj_t MAX30102;

void system_init(void)
{
    // servo_left = servo_init(ATOM1_CH2_P33_11, 50, 0, 0.5, 2.5, 180);
    // servo_right = servo_init(ATOM3_CH1_P33_5, 50, 0, 0.5, 2.5, 180); // 50Hz, 0.5ms~2.5ms, 360度

    // emm42_1 = emm42_init(UART_5, UART5_RX_P22_3, UART5_TX_P22_2, 115200, EMM42_CHKSUM_CONST_6B);

    // system_delay_ms(1000);

    // while (1)
    //     ;
    MAX30102 = MAX30102_init(P13_0, P14_6, MODE_HR_ONLY);

    // lcd_init();

    // motor1 = motor_init(ATOM2_CH5_P13_0, P14_6, 10000, 500, 1);
    // motor2 = motor_init(ATOM0_CH0_P14_5, P14_4, 10000, 500, -1);
    // motor3 = motor_init(ATOM0_CH2_P14_3, P14_2, 10000, 500, -1);
    // motor4 = motor_init(ATOM0_CH4_P14_1, P14_0, 10000, 500, 1);

    // lidar1 = stp23l_init(UART_8, UART8_RX_P33_6, UART8_TX_P33_7, 230400);
    // lidar2 = stp23l_init(UART_6, UART6_RX_P23_1, UART6_TX_P22_0, 230400);

    // maixcam1 = maixcam_uart_init(UART_10, UART10_RX_P13_1, UART10_TX_P00_8, 115200);

    // asrpro1 = asrpro_init(UART_2, UART2_RX_P33_8, UART2_TX_P33_9, 9600);

    // imu_init(IMU_DEVICE_963RA);
    // attitude_init(ATTITUDE_EKF);
    // pit_ms_init(CCU60_CH1, PIT_ATTITUDE_T);
    // pit_enable(CCU60_CH1);

    // key_init_rewrite(KEY_NUM); // 初始化按键
    // pit_ms_init(CCU60_CH0, PIT_KEY_T);
    // pit_enable(CCU60_CH0);

    // control_init();
    // pit_ms_init(CCU61_CH0, PIT_CONTROL_T);
    // pit_enable(CCU61_CH0);

    // pit_ms_init(CCU61_CH1, 10);
    // pit_enable(CCU61_CH1);
}