#ifndef _CONTROL_PID_CONTROL_H_
#define _CONTROL_PID_CONTROL_H_

#include "zf_common_typedef.h"
#include "motor.h"
#include "stp23l.h"

typedef enum
{
    WAITING_ANGLE,
    WAITING_RUNNING,
    RUNNING_GUANDAO,
    RUNNING_X,
    RUNNING_Y,
    ROTATING,
    CHECKING,
    RETURNING,
    STOP
} control_running_mode_e;

void control_init(void);
void control_handler(void);
float pid_angle_control(void);
float pid_pos_control(int16 distance, float);
vuint8 control_pid_pos(stp23l_obj_t *lidar, motor_obj_t *motor_a, motor_obj_t *motor_b, float pos_target);
vuint8 control_rolling(float angle_target);

extern float angle_tar;
extern float x_tar;
extern float y_tar;
extern control_running_mode_e curr_state;

extern float position_X, position_Y; // 0front 1back 2left 3right 4stop
typedef enum
{
    Dir_front,
    Dir_back,
    Dir_left,
    Dir_right,
    Dir_stop,
    Dir_x,
    Dir_y
} control_running_dir_e;
void move_guandao(float target_pos, int Dir_now);
extern int now_dir;
extern int data_camera;
extern void path_work(int st_point, int ed_point);
void open_dir_motor(int Dir_now, float motor_vel, float turn_diff);
extern int move_mode;
extern void roting(void);
extern void adjust_tof(void);
extern void checking_tof(float motor_vel, float ok_area);
vuint8 control_pid_pos_2(stp23l_obj_t *lidar, float pos_target, vuint8 status_now, float motor_vel, float ok_area);
#endif
