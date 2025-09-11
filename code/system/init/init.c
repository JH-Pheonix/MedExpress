#include "init.h"

#define PIT_KEY_T 10
#define PIT_ATTITUDE_T 2
#define PIT_CONTROL_T 10

motor_obj_t motor1;
motor_obj_t motor2;
motor_obj_t motor3;
motor_obj_t motor4;

void system_init(void)
{
    imu_init(IMU_DEVICE_963RA);
    lcd_init();
    control_init();

    motor1 = motor_init(ATOM2_CH5_P13_0, P14_6, 10000, 0, 1);
    motor2 = motor_init(ATOM0_CH0_P14_5, P14_4, 10000, 0, -1);
    motor3 = motor_init(ATOM0_CH2_P14_3, P14_2, 10000, 0, 1);
    motor4 = motor_init(ATOM0_CH4_P14_1, P14_0, 10000, 0, -1);

    // motor_set_pwm(&motor1, 1000);
    // motor_set_pwm(&motor2, 1000);
    // motor_set_pwm(&motor3, 1000);
    // motor_set_pwm(&motor4, 1000);

    attitude_init(ATTITUDE_EKF);
    key_init_rewrite(KEY_NUM); // 初始化按键
    pit_ms_init(CCU60_CH0, PIT_KEY_T);
    pit_enable(CCU60_CH0);
    pit_ms_init(CCU60_CH1, PIT_ATTITUDE_T);
    pit_enable(CCU60_CH1);
    pit_ms_init(CCU61_CH0, PIT_CONTROL_T);
    pit_enable(CCU61_CH0);
}