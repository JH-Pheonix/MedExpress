#include "pid_control.h"
#include "pid.h"
#include "Attitude.h"

#include "init.h"

pid_type_def angle_pid;
pid_type_def pos_pid;
float pos_pid_params[3] = {5.0, 0.0f, 0.0f};
float angle_pid_params[3] = {200.0f, 0.0f, 0.0f};

float x_tar = 0.0f;
float y_tar = 0.0f;
float angle_tar = 0.0f;

vuint32 checking_timeout = 0;

static vuint32 cnt = 0;
vuint8 flag = 0;
vuint8 status1 = 4;
vuint8 status2 = 4;

control_running_mode_e curr_state = WAITING_ANGLE;

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
    //float angle_out = pid_position(&angle_pid, g_euler_angle.yaw, angle_tar);
    float angle_out = pid_position(&angle_pid, g_euler_angle.yaw, 0);

    return angle_out;
}

float control_pos_pid(int16 distance, float pos_tar)
{
    float pos_out = pid_position(&pos_pid, distance, pos_tar);
    return pos_out;
}

void control_handler()
{
    vuint8 status = 1;
    switch (curr_state)
    {
    case WAITING_ANGLE:
    case WAITING_RUNNING:
        motor_set_pwm(&motor1, 0);
        motor_set_pwm(&motor2, 0);
        motor_set_pwm(&motor3, 0);
        motor_set_pwm(&motor4, 0);
        break;
    case RUNNING_X:
        status = control_pid_pos(&lidar1, &motor3, &motor4, x_tar);
        if (status == 0)
        {
            curr_state = RUNNING_Y;
        }
        break;
    case RUNNING_Y:
        status = control_pid_pos(&lidar2, &motor2, &motor1, y_tar);
        if (status == 0)
        {
            curr_state = ROTATING;
        }
        break;

    case ROTATING:
        status = control_rolling(angle_tar);
        if (status == 0)
        {
            curr_state = CHECKING;
        }
        break;
    case CHECKING:

        // checking_timeout++;
        //  if (checking_timeout > 300)
        //  {
        //      curr_state = RETURNING;
        //      break;
        //  }
        if (status1)
            status1 = control_pid_pos_2(&lidar2, &motor2, &motor1, y_tar, status1);
        else if (status2)
            status2 = control_pid_pos_2(&lidar1, &motor3, &motor4, x_tar, status2);
        if (status1 == 0 && status2 == 0)
        {
            //curr_state = RETURNING;
            status1 = status2 = 4;
            motor_set_pwm(&motor1, 0);
            motor_set_pwm(&motor2, 0);
            motor_set_pwm(&motor3, 0);
            motor_set_pwm(&motor4, 0);
            position_X=5600,position_Y=-2200;
            curr_state = RUNNING_GUANDAO;
        }
        break;

    case RETURNING:
        // if (flag == 0)
        // {
        if (cnt < 500)
        {
            cnt++;
            motor_set_pwm(&motor1, 0);
            motor_set_pwm(&motor2, 0);
            motor_set_pwm(&motor3, 0);
            motor_set_pwm(&motor4, 0);
        }
        else if (cnt < 1300)
        {
            motor_set_pwm(&motor1, 1000);
            motor_set_pwm(&motor2, -1000);
            motor_set_pwm(&motor3, -1000);
            motor_set_pwm(&motor4, 1000);
            cnt++;
        }
        else if (cnt >= 1300 && cnt < 1500)
        {
            cnt++;
            motor_set_pwm(&motor1, 0);
            motor_set_pwm(&motor2, 0);
            motor_set_pwm(&motor3, 0);
            motor_set_pwm(&motor4, 0);
        }
        else
        {
            cnt = 0;
            curr_state = RUNNING_X;
            // flag = 1;
        }
        // }
        break;
    case STOP:
        motor_set_pwm(&motor1, 0);
        motor_set_pwm(&motor2, 0);
        motor_set_pwm(&motor3, 0);
        motor_set_pwm(&motor4, 0);
        break;
    default:
        break;
    }
}
vuint8 control_pid_pos_2(stp23l_obj_t *lidar, motor_obj_t *motor_a, motor_obj_t *motor_b, float pos_target, vuint8 status_now)
{
    stp23l_frame_t lidar_frame;
    stp23l_pop_frame(lidar, &lidar_frame);
    if (status_now == 4)
        status_now = 1 + (lidar_frame.points[0].distance > pos_target); // 1 is back,2 is front

    if (lidar_frame.points[0].distance- pos_target<=15&&lidar_frame.points[0].distance- pos_target>=-15)
    {
        motor_set_pwm(motor_a, 0);
        motor_set_pwm(motor_b, 0);
        return 0;
    }
    float motor_vel = -550;
    if (status_now == 1)
        motor_vel = -motor_vel;
    motor_set_pwm(motor_a, -motor_vel);
    motor_set_pwm(motor_b, motor_vel);
    // motor_set_pwm(motor_a, -motor_vel - turn_diff);
    // motor_set_pwm(motor_b, motor_vel - turn_diff);
    return status_now;
}
vuint8 control_pid_pos(stp23l_obj_t *lidar, motor_obj_t *motor_a, motor_obj_t *motor_b, float pos_target)
{
    stp23l_frame_t lidar_frame;
    stp23l_pop_frame(lidar, &lidar_frame);
    if (lidar_frame.points[0].distance - pos_target <= 10 && lidar_frame.points[0].distance - pos_target >= -10 && lidar_frame.points[0].distance != 0)
    {
        motor_set_pwm(motor_a, 0);
        motor_set_pwm(motor_b, 0);
        return 0; // 鍒拌揪
    }
    float turn_diff = control_angle_pid();
    float motor_vel = control_pos_pid(lidar_frame.points[0].distance + 100, pos_target);
    motor_set_pwm(motor_a, -motor_vel - turn_diff);
    motor_set_pwm(motor_b, motor_vel - turn_diff);
    return 1;
}

vuint8 control_rolling(float angle_target)
{
    if (angle_target - g_euler_angle.yaw >= 0.1 || angle_target - g_euler_angle.yaw <= -0.1)
    {
        if (angle_target - g_euler_angle.yaw >= 0)
        {
            motor_set_pwm(&motor1, -200);
            motor_set_pwm(&motor2, -200);
            motor_set_pwm(&motor3, -200);
            motor_set_pwm(&motor4, -200);
        }
        else if (angle_target - g_euler_angle.yaw < 0)
        {
            motor_set_pwm(&motor1, 200);
            motor_set_pwm(&motor2, 200);
            motor_set_pwm(&motor3, 200);
            motor_set_pwm(&motor4, 200);
        }
        return 1; // 鏈埌杈�
    }
    motor_set_pwm(&motor1, 0);
    motor_set_pwm(&motor2, 0);
    motor_set_pwm(&motor3, 0);
    motor_set_pwm(&motor4, 0);

    return 0; // 鍒拌揪
}


float position_X=0,position_Y=0;//0front 1back 2left 3right 4stop
int now_dir=5;
void move_guandao(float target_pos,int Dir_now){
    now_dir=Dir_now;

    while(1){
        if(Dir_now==Dir_front&&position_X>target_pos) break;
        if(Dir_now==Dir_back&&position_X<target_pos) break;
        if(Dir_now==Dir_left&&position_Y<target_pos) break;
        if(Dir_now==Dir_right&&position_Y>target_pos) break;

        float turn_diff = control_angle_pid();
        float motor_vel = 6000;
        if(Dir_now==Dir_front){
            motor_set_pwm(&motor3, motor_vel- turn_diff);
            motor_set_pwm(&motor4, -motor_vel- turn_diff);
        }
        if(Dir_now==Dir_back){
            motor_set_pwm(&motor3, -motor_vel- turn_diff);
            motor_set_pwm(&motor4, motor_vel- turn_diff);
        }
        if(Dir_now==Dir_left){
            motor_set_pwm(&motor1, -motor_vel- turn_diff);
            motor_set_pwm(&motor2, motor_vel- turn_diff);
        }
        if(Dir_now==Dir_right){
            motor_set_pwm(&motor1, motor_vel- turn_diff);
            motor_set_pwm(&motor2, -motor_vel- turn_diff);
        }
    }

    motor_set_pwm(&motor1, 0);
    motor_set_pwm(&motor2, 0);
    motor_set_pwm(&motor3, 0);
    motor_set_pwm(&motor4, 0);
    now_dir=Dir_stop;
}
