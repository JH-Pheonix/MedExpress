#ifndef _SYSTEM_INIT_H_
#define _SYSTEM_INIT_H_

#include "imu.h"
#include "motor.h"
#include "key.h"
#include "lcd.h"
#include "attitude.h"
#include "stp23l.h"
#include "maixcam.h"
#include "asrpro.h"
#include "emm42.h"
#include "servo.h"
#include "MAX30102.h"

void system_init(void);

extern motor_obj_t motor1;
extern motor_obj_t motor2;
extern motor_obj_t motor3;
extern motor_obj_t motor4;

extern stp23l_obj_t lidar1;
extern stp23l_obj_t lidar2;
extern stp23l_obj_t lidar3;

extern maixcam_obj_t maixcam1;

extern asrpro_obj_t asrpro1;

extern emm42_obj_t emm42_1;

extern MAX30102_obj_t MAX30102;

#define lidar_left lidar2
#define lidar_right lidar1
#define lidar_front lidar3

#endif