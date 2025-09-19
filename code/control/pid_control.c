#include "pid_control.h"
#include "pid.h"
#include "attitude.h"

#include "init.h"

pid_type_def angle_pid;
pid_type_def pos_pid;
float pos_pid_params[3] = {8.0, 0.0f, 0.0f};
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

float control_angle_pid()
{
    // if (g_euler_angle.yaw > 180.0f)
    //     g_euler_angle.yaw -= 360.0f;
    // else if (g_euler_angle.yaw < -180.0f)
    //     g_euler_angle.yaw += 360.0f;
    // float angle_out = pid_position(&angle_pid, g_euler_angle.yaw, angle_tar);
    float angle_out = pid_position(&angle_pid, g_euler_angle.yaw, 0);

    return angle_out;
}

float control_pos_pid(int16 distance, float pos_tar)
{
    float pos_out = pid_position(&pos_pid, distance, pos_tar);
    return pos_out;
}
void checking_tof(float motor_vel, float ok_area)
{
    while (1)
    {
        if (status2)
            status2 = control_pid_pos_2(&lidar1, x_tar, Dir_x, motor_vel, ok_area);
        else if (status1)
            status1 = control_pid_pos_2(&lidar2, y_tar, Dir_y, motor_vel, ok_area);

        if (status1 == 0 && status2 == 0)
        {
            // curr_state = RETURNING;
            status1 = status2 = 4;
            motor_set_pwm(&motor1, 0);
            motor_set_pwm(&motor2, 0);
            motor_set_pwm(&motor3, 0);
            motor_set_pwm(&motor4, 0);
            return;
        }
        system_delay_ms(2);
    }
}
void roting(void)
{
    while (1)
    {
        int status = control_rolling(angle_tar);
        if (status == 0)
        {
            return;
        }
        system_delay_ms(2);
    }
}
void adjust_tof(void)
{
    roting();
    checking_tof(3000, 300);
    roting();
    checking_tof(600, 150);
    roting();
    checking_tof(300, 60);

    system_delay_ms(400);
    roting();
    system_delay_ms(300);
    checking_tof(100, 20);

    position_X = 5600, position_Y = -2200;
}
vuint8 control_pid_pos_2(stp23l_obj_t *lidar, float pos_target, vuint8 Dir_now, float motor_vel, float ok_area)
{
    stp23l_frame_t lidar_frame;
    stp23l_pop_frame(lidar, &lidar_frame);
    int status_now = 1 + (lidar_frame.points[11].distance > pos_target); // 1 is back,2 is front
    if (Dir_now == Dir_x && status_now == 1)
        Dir_now = Dir_back;
    if (Dir_now == Dir_x && status_now == 2)
        Dir_now = Dir_front;
    if (Dir_now == Dir_y && status_now == 1)
        Dir_now = Dir_right; // which one bed to choose
    if (Dir_now == Dir_y && status_now == 2)
        Dir_now = Dir_left; // which one bed to choose
    if (lidar_frame.points[11].distance - pos_target <= ok_area &&
        lidar_frame.points[11].distance - pos_target >= -ok_area)
    {
        motor_set_pwm(&motor1, 0);
        motor_set_pwm(&motor2, 0);
        motor_set_pwm(&motor3, 0);
        motor_set_pwm(&motor4, 0);
        return 0;
    }
    /*if(curr_state!=CHECKING)
        if (lidar_frame.points[11].distance- pos_target<=60&&lidar_frame.points[11].distance- pos_target>=-60)
        {
            motor_set_pwm(&motor1, 0);
            motor_set_pwm(&motor2, 0);
            motor_set_pwm(&motor3, 0);
            motor_set_pwm(&motor4, 0);
            return 0;
        }*/
    // if(curr_state!=CHECKING) motor_vel=4000;

    open_dir_motor(Dir_now, motor_vel, 0);
    return status_now;
}
vuint8 control_pid_pos(stp23l_obj_t *lidar, motor_obj_t *motor_a, motor_obj_t *motor_b, float pos_target)
{
    stp23l_frame_t lidar_frame;
    stp23l_pop_frame(lidar, &lidar_frame);
    if (lidar_frame.points[11].distance - pos_target <= 10 && lidar_frame.points[11].distance - pos_target >= -10 && lidar_frame.points[11].distance != 0)
    {
        motor_set_pwm(motor_a, 0);
        motor_set_pwm(motor_b, 0);
        return 0; // 鍒拌揪
    }
    float turn_diff = control_angle_pid();
    float motor_vel = control_pos_pid(lidar_frame.points[11].distance + 10, pos_target);
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
            motor_set_pwm(&motor1, -150);
            motor_set_pwm(&motor2, -150);
            motor_set_pwm(&motor3, -150);
            motor_set_pwm(&motor4, -150);
        }
        else if (angle_target - g_euler_angle.yaw < 0)
        {
            motor_set_pwm(&motor1, 150);
            motor_set_pwm(&motor2, 150);
            motor_set_pwm(&motor3, 150);
            motor_set_pwm(&motor4, 150);
        }
        return 1; // 鏈埌杈�
    }
    motor_set_pwm(&motor1, 0);
    motor_set_pwm(&motor2, 0);
    motor_set_pwm(&motor3, 0);
    motor_set_pwm(&motor4, 0);

    return 0; // 鍒拌揪
}

float position_X = 0, position_Y = 0; // 0front 1back 2left 3right 4stop
int now_dir = 5;
int data_camera = 0, move_mode = 1; // 0simple 1diffcult
void move_guandao(float target_pos, int Dir_now)
{
    if (Dir_now == Dir_x && target_pos > position_X)
        Dir_now = Dir_front;
    if (Dir_now == Dir_x && target_pos <= position_X)
        Dir_now = Dir_back;
    if (Dir_now == Dir_y && target_pos > position_Y)
        Dir_now = Dir_right;
    if (Dir_now == Dir_y && target_pos <= position_Y)
        Dir_now = Dir_left;
    now_dir = Dir_now;

    while (1)
    {
        if (Dir_now == Dir_front && position_X > target_pos)
            break;
        if (Dir_now == Dir_back && position_X < target_pos)
            break;
        if (Dir_now == Dir_left && position_Y < target_pos)
            break;
        if (Dir_now == Dir_right && position_Y > target_pos)
            break;
        float turn_diff = control_angle_pid();
        float motor_vel = 6500 - move_mode * 1000;
        open_dir_motor(Dir_now, motor_vel, turn_diff);
    }

    motor_set_pwm(&motor1, 0);
    motor_set_pwm(&motor2, 0);
    motor_set_pwm(&motor3, 0);
    motor_set_pwm(&motor4, 0);
    now_dir = Dir_stop;
}
void open_dir_motor(int Dir_now, float motor_vel, float turn_diff)
{
    if (Dir_now == Dir_front && move_mode == 0)
    {
        motor_set_pwm(&motor3, motor_vel - turn_diff);
        motor_set_pwm(&motor4, -motor_vel - turn_diff);
    }
    if (Dir_now == Dir_back && move_mode == 0)
    {
        motor_set_pwm(&motor3, -motor_vel - turn_diff);
        motor_set_pwm(&motor4, motor_vel - turn_diff);
    }
    if (Dir_now == Dir_left && move_mode == 0)
    {
        motor_set_pwm(&motor1, -motor_vel - turn_diff);
        motor_set_pwm(&motor2, motor_vel - turn_diff);
    }
    if (Dir_now == Dir_right && move_mode == 0)
    {
        motor_set_pwm(&motor1, motor_vel - turn_diff);
        motor_set_pwm(&motor2, -motor_vel - turn_diff);
    }

    if (Dir_now == Dir_front && move_mode == 1)
    {
        motor_set_pwm(&motor3, motor_vel - turn_diff);
        motor_set_pwm(&motor4, -motor_vel - turn_diff); // front

        motor_set_pwm(&motor1, -motor_vel - turn_diff);
        motor_set_pwm(&motor2, motor_vel - turn_diff); // left
    }
    if (Dir_now == Dir_back && move_mode == 1)
    {
        motor_set_pwm(&motor3, -motor_vel - turn_diff);
        motor_set_pwm(&motor4, motor_vel - turn_diff); // back

        motor_set_pwm(&motor1, motor_vel - turn_diff);
        motor_set_pwm(&motor2, -motor_vel - turn_diff); // right
    }
    if (Dir_now == Dir_left && move_mode == 1)
    {
        motor_set_pwm(&motor1, -motor_vel - turn_diff);
        motor_set_pwm(&motor2, motor_vel - turn_diff); // left

        motor_set_pwm(&motor3, -motor_vel - turn_diff);
        motor_set_pwm(&motor4, motor_vel - turn_diff); // back
    }
    if (Dir_now == Dir_right && move_mode == 1)
    {
        motor_set_pwm(&motor1, motor_vel - turn_diff);
        motor_set_pwm(&motor2, -motor_vel - turn_diff); // right

        motor_set_pwm(&motor3, motor_vel - turn_diff);
        motor_set_pwm(&motor4, -motor_vel - turn_diff); // front
    }
}
void path_work(int st_point, int ed_point)
{
    int nurse_x = 1000, midroad_x = 4200;
    int bed_y1 = -2000, bed_y3 = 2000, bed_x = 5500;
    int obstacl_y_L = -2500, obstacl_y_R = 1200;
    if (st_point == 0 && ed_point == 4)
    {
        move_guandao(nurse_x, Dir_x);
    }
    if ((st_point == 0 || st_point == 4) && ed_point == 1)
    {
        move_guandao(obstacl_y_L, Dir_y); // input of obstacle
        move_guandao(midroad_x, Dir_x);
        move_guandao(bed_y1, Dir_y);
        move_guandao(bed_x, Dir_x);
    }
    if ((st_point == 0 || st_point == 4) && ed_point == 3)
    {
        move_guandao(obstacl_y_R, Dir_y); // input of obstacle
        move_guandao(midroad_x, Dir_x);
        move_guandao(bed_y3, Dir_y);
        move_guandao(bed_x, Dir_x);
    }
    if (st_point == 3 && ed_point == 0)
    {
        move_guandao(midroad_x, Dir_x);
        move_guandao(obstacl_y_R, Dir_y); // input of obstacle
        move_guandao(0, Dir_x);
        move_guandao(0, Dir_y);
    }
    if (st_point == 1 && ed_point == 0)
    {
        move_guandao(midroad_x, Dir_x);
        move_guandao(obstacl_y_L, Dir_y); // input of obstacle
        move_guandao(0, Dir_x);
        move_guandao(0, Dir_y);
    }
    if (st_point == 1 && ed_point == 3)
    {
        move_guandao(midroad_x, Dir_x);
        move_guandao(bed_y3, Dir_y);
        move_guandao(bed_x, Dir_x);
    }
    if (st_point == 3 && ed_point == 1)
    {
        move_guandao(midroad_x, Dir_x);
        move_guandao(bed_y1, Dir_y);
        move_guandao(bed_x, Dir_x);
    }
    // curr_state=RUNNING_X;
}
void control_init(void)
{
    pid_init(&angle_pid, angle_pid_params, 9999, 9999);
    pid_init(&pos_pid, pos_pid_params, 5000, 9999);
    if (move_mode == 0)
        pos_pid_params[0] = 8.0;
    else
        pos_pid_params[0] = 0.7;
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
        /*status = control_pid_pos(&lidar1, &motor3, &motor4, x_tar);
        if (status == 0)
        {
            curr_state = RUNNING_Y;
        }*/
        status = control_rolling(angle_tar);
        if (status == 0)
        {
            curr_state = RUNNING_Y;
        }
        break;
    case RUNNING_Y:
        /*if (status2)
           status2 = control_pid_pos_2(&lidar1, x_tar, Dir_x,550);
        else if (status1)
            status1 = control_pid_pos_2(&lidar2, y_tar, Dir_y,550);*/

        if (status1 == 0 && status2 == 0)
        {
            // curr_state = RETURNING;
            status1 = status2 = 4;
            motor_set_pwm(&motor1, 0);
            motor_set_pwm(&motor2, 0);
            motor_set_pwm(&motor3, 0);
            motor_set_pwm(&motor4, 0);
            curr_state = ROTATING;
        }
        /*status = control_pid_pos(&lidar2, &motor2, &motor1, y_tar);
        if (status == 0)
        {
            curr_state = ROTATING;
        }*/
        break;

    case ROTATING:
        status = control_rolling(angle_tar);
        if (status == 0)
        {
            curr_state = CHECKING;
        }
        break;
    case CHECKING:

        /*if (status2)
           status2 = control_pid_pos_2(&lidar1, x_tar, Dir_x,550);
        else if (status1)
            status1 = control_pid_pos_2(&lidar2, y_tar, Dir_y,550);*/

        if (status1 == 0 && status2 == 0)
        {
            // curr_state = RETURNING;
            status1 = status2 = 4;
            motor_set_pwm(&motor1, 0);
            motor_set_pwm(&motor2, 0);
            motor_set_pwm(&motor3, 0);
            motor_set_pwm(&motor4, 0);
            position_X = 5600, position_Y = -2200;
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
