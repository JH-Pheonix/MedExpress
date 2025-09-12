#ifndef _CONTROL_PID_CONTROL_H_
#define _CONTROL_PID_CONTROL_H_

#include "zf_common_typedef.h"

void control_init(void);
void control_handler(void);
float control_angle_pid(void);
void control_test_circle();

extern float diff;
extern float vel;
extern vuint8 flag;
extern float angle_tar;

#endif