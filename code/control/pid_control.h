#ifndef _CONTROL_PID_CONTROL_H_
#define _CONTROL_PID_CONTROL_H_

#include "zf_common_typedef.h"
#include "motor.h"
#include "stp23l.h"

typedef enum
{
    WAITING_ANGLE,
    WAITING_RUNNING,
    RUNNING_X,
    RUNNING_Y,
    ROTATING,
    CHECKING,
    RETURNING,
    STOP
} control_running_mode_e;

typedef struct
{
    vint16 motor1_pwm;
    vint16 motor2_pwm;
    vint16 motor3_pwm;
    vint16 motor4_pwm;
} control_motor_status_t;

void control_init(void);
void control_handler(void);
float control_angle_pid(void);
float control_pos_pid(int16 distance, float);
vuint8 control_pid_to_pos(stp23l_obj_t *lidar, motor_obj_t *motor_a, motor_obj_t *motor_b, float pos_target);
vuint8 control_stable_rolling(float angle_target, float yaw);

extern float angle_tar;
extern float x_tar;
extern float y_tar;
extern control_running_mode_e curr_state;

#endif