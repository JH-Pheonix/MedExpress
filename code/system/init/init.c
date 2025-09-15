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

maixcam_obj_t maixcam1;

void system_init(void)
{

    lcd_init();

    motor1 = motor_init(ATOM2_CH5_P13_0, P14_6, 10000, 500, 1);
    motor2 = motor_init(ATOM0_CH0_P14_5, P14_4, 10000, 500, -1);
    motor3 = motor_init(ATOM0_CH2_P14_3, P14_2, 10000, 500, -1);
    motor4 = motor_init(ATOM0_CH4_P14_1, P14_0, 10000, 500, 1);

    lidar1 = stp23l_init(UART_8, UART8_RX_P33_6, UART8_TX_P33_7, 230400);
    lidar2 = stp23l_init(UART_6, UART6_RX_P23_1, UART6_TX_P22_0, 230400);

    maixcam1 = maixcam_uart_init(UART_10, UART10_RX_P13_1, UART10_TX_P00_8, 115200);
    // motor_set_pwm(&motor1, 510);
    // motor_set_pwm(&motor2, 510);
    // motor_set_pwm(&motor3, 510);
    // motor_set_pwm(&motor4, 510);

    imu_init(IMU_DEVICE_963RA);
    attitude_init(ATTITUDE_EKF);
    pit_ms_init(CCU60_CH1, PIT_ATTITUDE_T);
    pit_enable(CCU60_CH1);

    key_init_rewrite(KEY_NUM); // 初始化按键
    pit_ms_init(CCU60_CH0, PIT_KEY_T);
    pit_enable(CCU60_CH0);

    control_init();
    pit_ms_init(CCU61_CH0, PIT_CONTROL_T);
    pit_enable(CCU61_CH0);
}