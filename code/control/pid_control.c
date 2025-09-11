#include "pid_control.h"
#include "pid.h"
#include "Attitude.h"
#include "motor.h"
#include "init.h"

int32 base_vel = 3000;
pid_type_def angle_pid;
float angle_pid_params[3] = {200.0f, 0.0f, 0.0f};
float diff = 0.0f;

void control_init(void)
{
    pid_init(&angle_pid, angle_pid_params, 9999, 9999);
}

float control_angle_pid()
{
    // if (g_euler_angle.yaw > 180.0f)
    //     g_euler_angle.yaw -= 360.0f;
    // else if (g_euler_angle.yaw < -180.0f)
    //     g_euler_angle.yaw += 360.0f;

    float angle_out = pid_position(&angle_pid, g_euler_angle.yaw, 0.0f);
    return angle_out;
}

void control_handler(void)
{

    diff = control_angle_pid();
    motor_set_pwm(&motor3, base_vel + diff);
    motor_set_pwm(&motor4, base_vel - diff);
}