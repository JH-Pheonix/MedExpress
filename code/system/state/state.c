#include "state.h"

static void event_waitting_pos();
static void handle_nurse_qrcode();

static state_machine_event_e nxt_event = EVENT_WAITTING_POS_ONE;

static car_pos_t tar_pos[2]; // 用于存储目标位置车身姿态信息
static vint16 nurse_qrcode = 13;
static order_t order[2] = {
    {1, 1},
    {3, 3},
};
static int curr_point = 0; // 当前点和order[1]一致说明已经走完了
static int target_point = 1;

void state_machine_execute()
{
    switch (nxt_event)
    {
    case EVENT_WAITTING_POS_ONE:
    case EVENT_WAITTING_POS_THREE:
        event_waitting_pos();
        break;
    case EVENT_NURSE:
        path_work(0, 4);
        curr_point = 4;

        // TODO: 获取二维码信息
        handle_nurse_qrcode();

        target_point = order[0].bed; // 目标点是第一个床
        system_delay_ms(1000);

        nxt_event = EVENT_GO_TO_BED;
        break;
    case EVENT_GO_TO_BED:
        system_delay_ms(1000);

        path_work(curr_point, target_point);
        adjust_tof();

        curr_point = target_point;                                       // 到达当前点，当前点更新过去
        nxt_event = target_point == 1 ? EVENT_BED_ONE : EVENT_BED_THREE; // 根据目标点决定触发事件
        break;
    case EVENT_BED_ONE:
        system_delay_ms(2000);

        // TODO: 放药
        // TODO: 播报
        target_point = order[1].bed;                                       // 目标点无论如何更新为下一个点就行
        nxt_event = order[1].bed == 1 ? EVENT_BACK_HOME : EVENT_GO_TO_BED; // 如果下一个目标点是1号床就回家，否则继续去3号床

        break;
    case EVENT_BED_THREE:
        system_delay_ms(2000);

        // TODO: 放药
        // TODO: 播报
        target_point = order[1].bed;                                       // 目标点无论如何更新为下一个点就行
        nxt_event = order[1].bed == 3 ? EVENT_BACK_HOME : EVENT_GO_TO_BED; // 如果下一个目标点是3号床就回家，否则继续去1号床

        break;
    case EVENT_BACK_HOME:
        path_work(curr_point, 0);
        curr_point = 0;

        nxt_event = EVENT_STOP;
        break;
    case EVENT_STOP:
        // 这里可以写一些逻辑，比如更新状态进行下一次任务
        break;
    default:
        break;
    }
}

static void event_waitting_pos()
{
    if (keymsg.key == KEY_L)
    {
        system_delay_ms(1500);
        stp23l_frame_t lidar_frame;

        switch (nxt_event)
        {
        case EVENT_WAITTING_POS_ONE:
        {
            // 记录一号床位置
            stp23l_pop_frame(&lidar_front, &lidar_frame);
            tar_pos[0].front = lidar_frame.points[11].distance;
            stp23l_pop_frame(&lidar_right, &lidar_frame);
            tar_pos[0].side = lidar_frame.points[11].distance;
            tar_pos[0].yaw = attitude_get_yaw();
            nxt_event = EVENT_WAITTING_POS_THREE;
            break;
        }
        case EVENT_WAITTING_POS_THREE:
        {
            stp23l_pop_frame(&lidar_front, &lidar_frame);
            tar_pos[1].front = lidar_frame.points[11].distance;
            stp23l_pop_frame(&lidar_left, &lidar_frame);
            tar_pos[1].side = lidar_frame.points[11].distance;
            tar_pos[1].yaw = attitude_get_yaw();
            nxt_event = EVENT_NURSE;
            break;
        }
        default:
            break;
        }
    }
}

static void handle_nurse_qrcode()
{
    order[0].drug = nurse_qrcode % 10;
    order[0].bed = nurse_qrcode / 10;
    order[1].drug = order[0].drug == 1 ? 3 : 1;
    order[1].bed = order[0].bed == 1 ? 3 : 1;
}