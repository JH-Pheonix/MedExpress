/*********************************************************************************************************************
 * TC377 Opensourec Library 鍗筹紙TC377 寮�婧愬簱锛夋槸涓�涓熀浜庡畼鏂� SDK 鎺ュ彛鐨勭涓夋柟寮�婧愬簱
 * Copyright (c) 2022 SEEKFREE 閫愰绉戞妧
 *
 * 鏈枃浠舵槸 TC377 寮�婧愬簱鐨勪竴閮ㄥ垎
 *
 * TC377 寮�婧愬簱 鏄厤璐硅蒋浠�
 * 鎮ㄥ彲浠ユ牴鎹嚜鐢辫蒋浠跺熀閲戜細鍙戝竷鐨� GPL锛圙NU General Public License锛屽嵆 GNU閫氱敤鍏叡璁稿彲璇侊級鐨勬潯娆�
 * 鍗� GPL 鐨勭3鐗堬紙鍗� GPL3.0锛夋垨锛堟偍閫夋嫨鐨勶級浠讳綍鍚庢潵鐨勭増鏈紝閲嶆柊鍙戝竷鍜�/鎴栦慨鏀瑰畠
 *
 * 鏈紑婧愬簱鐨勫彂甯冩槸甯屾湜瀹冭兘鍙戞尌浣滅敤锛屼絾骞舵湭瀵瑰叾浣滀换浣曠殑淇濊瘉
 * 鐢氳嚦娌℃湁闅愬惈鐨勯�傞攢鎬ф垨閫傚悎鐗瑰畾鐢ㄩ�旂殑淇濊瘉
 * 鏇村缁嗚妭璇峰弬瑙� GPL
 *
 * 鎮ㄥ簲璇ュ湪鏀跺埌鏈紑婧愬簱鐨勫悓鏃舵敹鍒颁竴浠� GPL 鐨勫壇鏈�
 * 濡傛灉娌℃湁锛岃鍙傞槄<https://www.gnu.org/licenses/>
 *
 * 棰濆娉ㄦ槑锛�
 * 鏈紑婧愬簱浣跨敤 GPL3.0 寮�婧愯鍙瘉鍗忚 浠ヤ笂璁稿彲鐢虫槑涓鸿瘧鏂囩増鏈�
 * 璁稿彲鐢虫槑鑻辨枃鐗堝湪 libraries/doc 鏂囦欢澶逛笅鐨� GPL3_permission_statement.txt 鏂囦欢涓�
 * 璁稿彲璇佸壇鏈湪 libraries 鏂囦欢澶逛笅 鍗宠鏂囦欢澶逛笅鐨� LICENSE 鏂囦欢
 * 娆㈣繋鍚勪綅浣跨敤骞朵紶鎾湰绋嬪簭 浣嗕慨鏀瑰唴瀹规椂蹇呴』淇濈暀閫愰绉戞妧鐨勭増鏉冨０鏄庯紙鍗虫湰澹版槑锛�
 *
 * 鏂囦欢鍚嶇О          cpu0_main
 * 鍏徃鍚嶇О          鎴愰兘閫愰绉戞妧鏈夐檺鍏徃
 * 鐗堟湰淇℃伅          鏌ョ湅 libraries/doc 鏂囦欢澶瑰唴 version 鏂囦欢 鐗堟湰璇存槑
 * 寮�鍙戠幆澧�          ADS v1.10.2
 * 閫傜敤骞冲彴          TC377TP
 * 搴楅摵閾炬帴          https://seekfree.taobao.com/
 *
 * 淇敼璁板綍
 * 鏃ユ湡              浣滆��                澶囨敞
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
// 灏嗘湰璇彞涓�#pragma section all restore璇彞涔嬮棿鐨勫叏灞�鍙橀噺閮芥斁鍦–PU0鐨凴AM涓�

stp23l_frame_t lidar1_frame;
stp23l_frame_t lidar2_frame;
// 鏈緥绋嬫槸寮�婧愬簱绌哄伐绋� 鍙敤浣滅Щ妞嶆垨鑰呮祴璇曞悇绫诲唴澶栬
// 鏈緥绋嬫槸寮�婧愬簱绌哄伐绋� 鍙敤浣滅Щ妞嶆垨鑰呮祴璇曞悇绫诲唴澶栬
// 鏈緥绋嬫槸寮�婧愬簱绌哄伐绋� 鍙敤浣滅Щ妞嶆垨鑰呮祴璇曞悇绫诲唴澶栬
// **************************** 浠ｇ爜鍖哄煙 ****************************
int core0_main(void)
{
    clock_init(); // 鑾峰彇鏃堕挓棰戠巼<鍔″繀淇濈暀>
    debug_init(); // 鍒濆鍖栭粯璁よ皟璇曚覆鍙�
    // 姝ゅ缂栧啓鐢ㄦ埛浠ｇ爜 渚嬪澶栬鍒濆鍖栦唬鐮佺瓑

    system_init(); // 绯荤粺鍒濆鍖�

    // 姝ゅ缂栧啓鐢ㄦ埛浠ｇ爜 渚嬪澶栬鍒濆鍖栦唬鐮佺瓑
    cpu_wait_event_ready(); // 绛夊緟鎵�鏈夋牳蹇冨垵濮嬪寲瀹屾瘯
    while (TRUE)
    {
        system_delay_ms(2);
        // stp23l_pop_frame(&lidar1, &lidar1_frame);
        // stp23l_pop_frame(&lidar2, &lidar2_frame);

        // lcd_show_int(0, 0, lidar1_frame.points[0].distance, 7);
        // lcd_show_int(8, 0, lidar2_frame.points[0].distance, 7);
        if (keymsg.key == KEY_L)
        {
            if (curr_state == WAITING_RUNNING)
            {
                lcd_show_string(0, 0, "status:");
                lcd_show_int(8, 0, curr_state, 1);
                system_delay_ms(1500);
                curr_state = RUNNING_GUANDAO;
            }
            else if (curr_state == WAITING_ANGLE)
            {
                lcd_show_string(0, 0, "status:");
                lcd_show_int(8, 0, curr_state, 1);
                system_delay_ms(1500);
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

        lcd_show_string(0, 7, "PosX:");
        lcd_show_int(7, 7, position_X,6);
        lcd_show_string(0, 8, "PosY:");
        lcd_show_int(7, 8, position_Y, 6);
        // lcd_show_float(0, 1, diff, 3, 3);
    }
}

#pragma section all restore
// **************************** 浠ｇ爜鍖哄煙 ****************************
