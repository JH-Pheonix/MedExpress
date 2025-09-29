#ifndef _SYSTEM_TEST_H_
#define _SYSTEM_TEST_H_

#include "zf_common_typedef.h"
#include "emm42.h"
#include "init.h"
#include "encoder.h"
#include "key.h"
#include "test.h"

void test_emm42(void);
void test_stp23l(stp23l_obj_t *l, stp23l_obj_t *f, stp23l_obj_t *r);
void test_encoder(encoder_obj_t *e_x, encoder_obj_t *e_y);
void test_motor(motor_obj_t *m1, motor_obj_t *m2, motor_obj_t *m3, motor_obj_t *m4);
void test_servo(servo_obj_t *s);

#endif