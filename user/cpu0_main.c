/*********************************************************************************************************************
 * TC377 Opensourec Library 閸楃绱橳C377 瀵拷濠ф劕绨遍敍澶嬫Ц娑擄拷娑擃亜鐔�娴滃骸鐣奸弬锟� SDK 閹恒儱褰涢惃鍕儑娑撳鏌熷锟藉┃鎰氨
 * Copyright (c) 2022 SEEKFREE 闁劙顥ｇ粔鎴炲Η
 *
 * 閺堫剚鏋冩禒鑸垫Ц TC377 瀵拷濠ф劕绨遍惃鍕闁劌鍨�
 *
 * TC377 瀵拷濠ф劕绨� 閺勵垰鍘ょ拹纭呰拫娴狅拷
 * 閹劌褰叉禒銉︾壌閹诡喛鍤滈悽杈拫娴犺泛鐔�闁叉垳绱伴崣鎴濈閻拷 GPL閿涘湙NU General Public License閿涘苯宓� GNU闁氨鏁ら崗顒�鍙＄拋绋垮讲鐠囦緤绱氶惃鍕蒋濞嗭拷
 * 閸楋拷 GPL 閻ㄥ嫮顑�3閻楀牞绱欓崡锟� GPL3.0閿涘鍨ㄩ敍鍫熷亶闁瀚ㄩ惃鍕剁礆娴犺缍嶉崥搴㈡降閻ㄥ嫮澧楅張顒婄礉闁插秵鏌婇崣鎴濈閸滐拷/閹存牔鎱ㄩ弨鐟扮暊
 *
 * 閺堫剙绱戝┃鎰氨閻ㄥ嫬褰傜敮鍐╂Ц鐢本婀滅�瑰啳鍏橀崣鎴炲皩娴ｆ粎鏁ら敍灞肩稻楠炶埖婀�电懓鍙炬担婊�鎹㈡担鏇犳畱娣囨繆鐦�
 * 閻㈡俺鍤﹀▽鈩冩箒闂呮劕鎯堥惃鍕拷鍌炴敘閹勫灗闁倸鎮庨悧鐟扮暰閻€劑锟芥梻娈戞穱婵婄槈
 * 閺囨潙顦跨紒鍡氬Ν鐠囧嘲寮憴锟� GPL
 *
 * 閹劌绨茬拠銉ユ躬閺�璺哄煂閺堫剙绱戝┃鎰氨閻ㄥ嫬鎮撻弮鑸垫暪閸掗绔存禒锟� GPL 閻ㄥ嫬澹囬張锟�
 * 婵″倹鐏夊▽鈩冩箒閿涘矁顕崣鍌炴<https://www.gnu.org/licenses/>
 *
 * 妫版繂顦诲▔銊︽閿涳拷
 * 閺堫剙绱戝┃鎰氨娴ｈ法鏁� GPL3.0 瀵拷濠ф劘顔忛崣顖濈槈閸楀繗顔� 娴犮儰绗傜拋绋垮讲閻㈣櫕妲戞稉楦跨槯閺傚洨澧楅張锟�
 * 鐠佺褰查悽铏閼昏鲸鏋冮悧鍫濇躬 libraries/doc 閺傚洣娆㈡径閫涚瑓閻拷 GPL3_permission_statement.txt 閺傚洣娆㈡稉锟�
 * 鐠佺褰茬拠浣稿閺堫剙婀� libraries 閺傚洣娆㈡径閫涚瑓 閸楀疇顕氶弬鍥︽婢堕�涚瑓閻拷 LICENSE 閺傚洣娆�
 * 濞嗐垼绻嬮崥鍕秴娴ｈ法鏁ら獮鏈电炊閹绢厽婀扮粙瀣碍 娴ｅ棔鎱ㄩ弨鐟板敶鐎硅妞傝箛鍛淬�忔穱婵堟殌闁劙顥ｇ粔鎴炲Η閻ㄥ嫮澧楅弶鍐紣閺勫函绱欓崡铏拱婢圭増妲戦敍锟�
 *
 * 閺傚洣娆㈤崥宥囆�          cpu0_main
 * 閸忣剙寰冮崥宥囆�          閹存劙鍏橀柅鎰邦棧缁夋垶濡ч張澶愭閸忣剙寰�
 * 閻楀牊婀版穱鈩冧紖          閺屻儳婀� libraries/doc 閺傚洣娆㈡径鐟板敶 version 閺傚洣娆� 閻楀牊婀扮拠瀛樻
 * 瀵拷閸欐垹骞嗘晶锟�          ADS v1.10.2
 * 闁倻鏁ら獮鍐插酱          TC377TP
 * 鎼存鎽甸柧鐐复          https://seekfree.taobao.com/
 *
 * 娣囶喗鏁肩拋鏉跨秿
 * 閺冦儲婀�              娴ｆ粏锟斤拷                婢跺洦鏁�
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
// 鐏忓棙婀扮拠顓炲綖娑擄拷#pragma section all restore鐠囶厼褰炴稊瀣？閻ㄥ嫬鍙忕仦锟介崣姗�鍣洪柈鑺ユ杹閸︹�揚U0閻ㄥ嚧AM娑擄拷

stp23l_frame_t lidar1_frame;
stp23l_frame_t lidar2_frame;
// 閺堫兛绶ョ粙瀣Ц瀵拷濠ф劕绨辩粚鍝勪紣缁嬶拷 閸欘垳鏁ゆ担婊呅╁宥嗗灗閼板懏绁寸拠鏇炴倗缁鍞存径鏍啎
// 閺堫兛绶ョ粙瀣Ц瀵拷濠ф劕绨辩粚鍝勪紣缁嬶拷 閸欘垳鏁ゆ担婊呅╁宥嗗灗閼板懏绁寸拠鏇炴倗缁鍞存径鏍啎
// 閺堫兛绶ョ粙瀣Ц瀵拷濠ф劕绨辩粚鍝勪紣缁嬶拷 閸欘垳鏁ゆ担婊呅╁宥嗗灗閼板懏绁寸拠鏇炴倗缁鍞存径鏍啎
// **************************** 娴狅絿鐖滈崠鍝勭厵 ****************************
int core0_main(void)
{
    clock_init(); // 閼惧嘲褰囬弮鍫曟寭妫版垹宸�<閸斺�崇箑娣囨繄鏆�>
    debug_init(); // 閸掓繂顫愰崠鏍帛鐠併倛鐨熺拠鏇氳閸欙拷
    // 濮濄倕顦╃紓鏍у晸閻€劍鍩涙禒锝囩垳 娓氬顩ф径鏍啎閸掓繂顫愰崠鏍﹀敩閻胶鐡�

    system_init(); // 缁崵绮洪崚婵嗩潗閸栵拷

    // 濮濄倕顦╃紓鏍у晸閻€劍鍩涙禒锝囩垳 娓氬顩ф径鏍啎閸掓繂顫愰崠鏍﹀敩閻胶鐡�
    cpu_wait_event_ready(); // 缁涘绶熼幍锟介張澶嬬壋韫囧啫鍨垫慨瀣鐎瑰本鐦�
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
        maixcam_message_t data;
        data = maixcam_pop_data();
        lcd_show_uint(0, 9, data.cmd, 3);
        lcd_show_uint(8, 9, data.data, 3);
        if(data_camera==0&&data.data!=0)data_camera=data.data;
        maixcam_clear();
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
// **************************** 娴狅絿鐖滈崠鍝勭厵 ****************************
