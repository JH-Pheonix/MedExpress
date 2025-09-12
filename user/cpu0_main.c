/*********************************************************************************************************************
 * TC377 Opensourec Library 即（TC377 开源库）是一个基于官方 SDK 接口的第三方开源库
 * Copyright (c) 2022 SEEKFREE 逐飞科技
 *
 * 本文件是 TC377 开源库的一部分
 *
 * TC377 开源库 是免费软件
 * 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
 * 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
 *
 * 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
 * 甚至没有隐含的适销性或适合特定用途的保证
 * 更多细节请参见 GPL
 *
 * 您应该在收到本开源库的同时收到一份 GPL 的副本
 * 如果没有，请参阅<https://www.gnu.org/licenses/>
 *
 * 额外注明：
 * 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
 * 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
 * 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
 * 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
 *
 * 文件名称          cpu0_main
 * 公司名称          成都逐飞科技有限公司
 * 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
 * 开发环境          ADS v1.10.2
 * 适用平台          TC377TP
 * 店铺链接          https://seekfree.taobao.com/
 *
 * 修改记录
 * 日期              作者                备注
 * 2022-11-03       pudding            first version
 ********************************************************************************************************************/
#include "zf_common_headfile.h"
#include "init.h"
#include "imu.h"
#include "lcd.h"
#include "key.h"
#include "Attitude.h"
#include "pid_control.h"
#include "stp23l.h"

#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

stp23l_frame_t lidar1_frame;
stp23l_frame_t lidar2_frame;
// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设
// **************************** 代码区域 ****************************
int core0_main(void)
{
    clock_init(); // 获取时钟频率<务必保留>
    debug_init(); // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等

    system_init(); // 系统初始化

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready(); // 等待所有核心初始化完毕
    while (TRUE)
    {
        system_delay_ms(100);
        // stp23l_pop_frame(&lidar1, &lidar1_frame);
        // stp23l_pop_frame(&lidar2, &lidar2_frame);

        // lcd_show_int(0, 0, lidar1_frame.points[0].distance, 7);
        // lcd_show_int(8, 0, lidar2_frame.points[0].distance, 7);
        if (keymsg.key == KEY_L)
        {
            if (curr_state == WAITING_RUNNING)
            {
                system_delay_ms(1000);
                curr_state = RUNNING_X;
            }
            else if (curr_state == WAITING_ANGLE)
            {
                system_delay_ms(1000);
                curr_state = WAITING_RUNNING;
                angle_tar = g_euler_angle.yaw;
                stp23l_frame_t lidar_frame;
                stp23l_pop_frame(&lidar1, &lidar_frame);
                x_tar = lidar_frame.points[0].distance;
                stp23l_pop_frame(&lidar2, &lidar_frame);
                y_tar = lidar_frame.points[0].distance;
            }
        }

        lcd_show_string(0, 0, "status:");
        lcd_show_int(8, 0, curr_state, 1);
        lcd_show_string(0, 1, "atar:");
        lcd_show_float(7, 1, angle_tar, 4, 3);
        lcd_show_string(0, 2, "xtar:");
        lcd_show_float(7, 2, x_tar, 4, 3);
        lcd_show_string(0, 3, "ytar:");
        lcd_show_float(7, 3, y_tar, 4, 3);
        lcd_show_string(0, 4, "yaw:");
        lcd_show_float(7, 4, g_euler_angle.yaw, 4, 3);
        stp23l_frame_t tmp;
        stp23l_pop_frame(&lidar1, &tmp);
        lcd_show_string(0, 5, "lidar1:");
        lcd_show_int(7, 5, tmp.points[0].distance, 6);
        lcd_show_string(0, 6, "lidar2:");
        stp23l_pop_frame(&lidar2, &tmp);
        lcd_show_int(7, 6, tmp.points[0].distance, 6);
        // lcd_show_float(0, 1, diff, 3, 3);
    }
}

#pragma section all restore
// **************************** 代码区域 ****************************
