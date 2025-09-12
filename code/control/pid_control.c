#include "pid_control.h"
#include "pid.h"
#include "Attitude.h"
#include "motor.h"
#include "init.h"

int32 base_vel = 3000;
pid_type_def angle_pid;
pid_type_def pos_pid;
float pos_pid_params[3] = {5.0, 0.0f, 0.0f};
float angle_pid_params[3] = {200.0f, 0.0f, 0.0f};
float diff = 0.0f;
float vel = 0.0f;
float pos_tar = 800.0f;
float angle_tar = 0.0f;

vuint8 flag = 0;

void control_init(void)
{
    pid_init(&angle_pid, angle_pid_params, 9999, 9999);
    pid_init(&pos_pid, pos_pid_params, 5000, 9999);
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

float control_pos_pid(int16 distance)
{
    float pos_out = pid_position(&pos_pid, distance, pos_tar);
    return pos_out;
}

void control_handler(void)
{
    // if (flag == 1)
    //     return;
    // stp23l_frame_t lidar_frame;
    // stp23l_pop_frame(&lidar1, &lidar_frame);
    // if (lidar_frame.points[0].distance - pos_tar <= 10 && lidar_frame.points[0].distance != 0)
    // {
    //     flag = 1;
    //     motor_set_pwm(&motor3, 0);
    //     motor_set_pwm(&motor4, 0);
    //     return;
    // }
    // diff = control_angle_pid();
    // vel = control_pos_pid(lidar_frame.points[0].distance + 100);
    // motor_set_pwm(&motor3, vel + diff);
    // motor_set_pwm(&motor4, vel - diff);
    control_test_circle();
}

void control_test_forward(void)
{
    if (flag == 1)
        return;
    stp23l_frame_t lidar_frame;
    stp23l_pop_frame(&lidar1, &lidar_frame);
    if (lidar_frame.points[0].distance - pos_tar <= 10 && lidar_frame.points[0].distance != 0)
    {
        flag = 1;
        motor_set_pwm(&motor3, 0);
        motor_set_pwm(&motor4, 0);
        return;
    }
    diff = control_angle_pid();
    vel = control_pos_pid(lidar_frame.points[0].distance + 100);
    motor_set_pwm(&motor3, vel + diff);
    motor_set_pwm(&motor4, vel - diff);
}
void control_test_circle(void)
{
    if (flag == 1)
        return;
    if (angle_tar - g_euler_angle.yaw >= 0.1 || angle_tar - g_euler_angle.yaw <= -0.1)
    {
        if (angle_tar - g_euler_angle.yaw >= 0)
        {
            motor_set_pwm(&motor1, -2000);
            motor_set_pwm(&motor2, -2000);
            motor_set_pwm(&motor3, -2000);
            motor_set_pwm(&motor4, -2000);
        }
        else if (angle_tar - g_euler_angle.yaw < 0)
        {
            motor_set_pwm(&motor1, 2000);
            motor_set_pwm(&motor2, 2000);
            motor_set_pwm(&motor3, 2000);
            motor_set_pwm(&motor4, 2000);
        }
    }
    else
    {
        flag = 1;
        motor_set_pwm(&motor1, 0);
        motor_set_pwm(&motor2, 0);
        motor_set_pwm(&motor3, 0);
        motor_set_pwm(&motor4, 0);
    }
}