#include "test.h"

void test_emm42(void)
{
    system_delay_ms(1000);
    emm42_position_mode_cmd(&emm42_1, 0, 0, 1500, 245, 7 * 3200, 0, 0);
    emm42_send_cmd(&emm42_1);
    system_delay_ms(2000);
    emm42_position_mode_cmd(&emm42_1, 0, 1, 1500, 245, 7 * 3200, 0, 0);
    emm42_send_cmd(&emm42_1);
}

void test_stp23l(stp23l_obj_t *l, stp23l_obj_t *f, stp23l_obj_t *r)
{
    stp23l_frame_t f_l, f_f, f_r;
    while (keymsg.key != KEY_L)
    {
        stp23l_pop_frame(l, &f_l);
        stp23l_pop_frame(f, &f_f);
        stp23l_pop_frame(r, &f_r);
        printf("%d, %d, %d\n", f_l.points[0].distance, f_f.points[0].distance, f_r.points[0].distance);

        system_delay_ms(100);
    }
}

void test_encoder(encoder_obj_t *e_x, encoder_obj_t *e_y)
{
    while (keymsg.key != KEY_L)
    {
        system_delay_ms(100);
        // printf("dddd\n");
        printf("%ld, %ld\n", encoder_get_data(e_x), encoder_get_data(e_y));
    }
}

void test_motor(motor_obj_t *m1, motor_obj_t *m2, motor_obj_t *m3, motor_obj_t *m4)
{
    while (keymsg.key != KEY_L)
    {
        motor_set_pwm(m1, 1000);
        motor_set_pwm(m2, 1000);
        motor_set_pwm(m3, 1000);
        motor_set_pwm(m4, 1000);
    }
}

void test_servo(servo_obj_t *s)
{
    float angle = 0;
    while (keymsg.key != KEY_L)
    {
        for (angle = 0; angle <= 180; angle += 10)
        {
            servo_set_angle(s, angle);
            system_delay_ms(500);
        }
        for (angle = 180; angle >= 0; angle -= 10)
        {
            servo_set_angle(s, angle);
            system_delay_ms(500);
        }
    }
}