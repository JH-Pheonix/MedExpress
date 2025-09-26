#include "stable.h"

// 原先返回status的写法需要多个控制帧，现在这样写直接阻塞好了
void stable_roting(float target_angle, vuint16 duty)
{
    float curr_angle = attitude_get_yaw();
    while (target_angle - curr_angle >= 0.1 || target_angle - curr_angle <= -0.1)
    {
        if (target_angle - curr_angle >= 0)
        {
            motor_set_pwm(&motor1, 150);
            motor_set_pwm(&motor2, 150);
            motor_set_pwm(&motor3, 150);
            motor_set_pwm(&motor4, 150);
        }
        else if (target_angle - curr_angle < 0)
        {
            motor_set_pwm(&motor1, -150);
            motor_set_pwm(&motor2, -150);
            motor_set_pwm(&motor3, -150);
            motor_set_pwm(&motor4, -150);
        }
    }

    motor_set_pwm(&motor1, 0);
    motor_set_pwm(&motor2, 0);
    motor_set_pwm(&motor3, 0);
    motor_set_pwm(&motor4, 0);
}