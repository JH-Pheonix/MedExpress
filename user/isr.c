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
 * 閺傚洣娆㈤崥宥囆�          isr
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

#include "stp23l.h"
#include "key.h"
#include "isr_config.h"
#include "isr.h"
#include "Attitude.h"
#include "pid_control.h"
#include "init.h"
#include "maixcam.h"

// 鐎甸�涚艾TC缁鍨妯款吇閺勵垯绗夐弨顖涘瘮娑擃厽鏌囧畵灞筋殰閻ㄥ嫸绱濈敮灞炬箿閺�顖涘瘮娑擃厽鏌囧畵灞筋殰闂囷拷鐟曚礁婀稉顓熸焽閸愬懍濞囬悽锟� interrupt_global_enable(0); 閺夈儱绱戦崥顖欒厬閺傤厼绁垫總锟�
// 缁狅拷閸楁洜鍋ｇ拠鏉戠杽闂勫懍绗傛潻娑樺弳娑擃厽鏌囬崥宥礐缁鍨惃鍕�栨禒鎯板殰閸斻劏鐨熼悽銊ょ啊 interrupt_global_disable(); 閺夈儲瀚嗙紒婵嗘惙鎼存柧鎹㈡担鏇犳畱娑擃厽鏌囬敍灞芥礈濮濄倝娓剁憰浣瑰灉娴狀剝鍤滃杈ㄥ閸斻劏鐨熼悽锟� interrupt_global_enable(0); 閺夈儱绱戦崥顖欒厬閺傤厾娈戦崫宥呯安閵嗭拷

// **************************** PIT娑擃厽鏌囬崙鑺ユ殶 ****************************
IFX_INTERRUPT(cc60_pit_ch0_isr, CCU6_0_CH0_INT_VECTAB_NUM, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    pit_clear_flag(CCU60_CH0);
    key_callback_func();
}
#include "pid_control.h"
IFX_INTERRUPT(cc60_pit_ch1_isr, CCU6_0_CH1_INT_VECTAB_NUM, CCU6_0_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    pit_clear_flag(CCU60_CH1);
    attitude_cal();
    float now_speed = 1500, d_isr = 0.002; // mm/s
    if (now_dir == Dir_front)
        position_X += d_isr * now_speed;
    if (now_dir == Dir_back)
        position_X -= d_isr * now_speed;
    if (now_dir == Dir_right)
        position_Y += d_isr * now_speed;
    if (now_dir == Dir_left)
        position_Y -= d_isr * now_speed;
}

IFX_INTERRUPT(cc61_pit_ch0_isr, CCU6_1_CH0_INT_VECTAB_NUM, CCU6_1_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    pit_clear_flag(CCU61_CH0);
    control_handler();
}

IFX_INTERRUPT(cc61_pit_ch1_isr, CCU6_1_CH1_INT_VECTAB_NUM, CCU6_1_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    pit_clear_flag(CCU61_CH1);
}
// **************************** PIT娑擃厽鏌囬崙鑺ユ殶 ****************************

// **************************** 婢舵牠鍎存稉顓熸焽閸戣姤鏆� ****************************
IFX_INTERRUPT(exti_ch0_ch4_isr, EXTI_CH0_CH4_INT_VECTAB_NUM, EXTI_CH0_CH4_INT_PRIO)
{
    interrupt_global_enable(0);            // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    if (exti_flag_get(ERU_CH0_REQ0_P15_4)) // 闁岸浜�0娑擃厽鏌�
    {
        exti_flag_clear(ERU_CH0_REQ0_P15_4);
    }

    if (exti_flag_get(ERU_CH4_REQ13_P15_5)) // 闁岸浜�4娑擃厽鏌�
    {
        exti_flag_clear(ERU_CH4_REQ13_P15_5);
    }
}

IFX_INTERRUPT(exti_ch1_ch5_isr, EXTI_CH1_CH5_INT_VECTAB_NUM, EXTI_CH1_CH5_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷

    if (exti_flag_get(ERU_CH1_REQ10_P14_3)) // 闁岸浜�1娑擃厽鏌�
    {
        exti_flag_clear(ERU_CH1_REQ10_P14_3);
    }

    if (exti_flag_get(ERU_CH5_REQ1_P15_8)) // 闁岸浜�5娑擃厽鏌�
    {
        exti_flag_clear(ERU_CH5_REQ1_P15_8);
    }
}

// 閻㈠彉绨幗鍕剼婢剁clk瀵洝鍓兼妯款吇閸楃姷鏁ゆ禍锟� 2闁岸浜鹃敍宀�鏁ゆ禍搴ば曢崣鎱廙A閿涘苯娲滃銈堢箹闁插奔绗夐崘宥呯暰娑斿鑵戦弬顓炲毐閺侊拷
// IFX_INTERRUPT(exti_ch2_ch6_isr, EXTI_CH2_CH6_INT_VECTAB_NUM, EXTI_CH2_CH6_INT_PRIO)
// {
//  interrupt_global_enable(0);                     // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
//  if(exti_flag_get(ERU_CH2_REQ7_P00_4))           // 闁岸浜�2娑擃厽鏌�
//  {
//      exti_flag_clear(ERU_CH2_REQ7_P00_4);
//  }
//  if(exti_flag_get(ERU_CH6_REQ9_P20_0))           // 闁岸浜�6娑擃厽鏌�
//  {
//      exti_flag_clear(ERU_CH6_REQ9_P20_0);
//  }
// }

IFX_INTERRUPT(exti_ch3_ch7_isr, EXTI_CH3_CH7_INT_VECTAB_NUM, EXTI_CH3_CH7_INT_PRIO)
{
    interrupt_global_enable(0);            // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    if (exti_flag_get(ERU_CH3_REQ6_P02_0)) // 闁岸浜�3娑擃厽鏌�
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
    }
    if (exti_flag_get(ERU_CH7_REQ16_P15_1)) // 闁岸浜�7娑擃厽鏌�
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);
    }
}
// **************************** 婢舵牠鍎存稉顓熸焽閸戣姤鏆� ****************************

// **************************** DMA娑擃厽鏌囬崙鑺ユ殶 ****************************
IFX_INTERRUPT(dma_ch5_isr, DMA_INT_VECTAB_NUM, DMA_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}
// **************************** DMA娑擃厽鏌囬崙鑺ユ殶 ****************************

// **************************** 娑撴彃褰涙稉顓熸焽閸戣姤鏆� ****************************
// 娑撴彃褰�0姒涙顓绘担婊�璐熺拫鍐槸娑撴彃褰�
IFX_INTERRUPT(uart0_tx_isr, UART0_INT_VECTAB_NUM, UART0_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}
// IFX_INTERRUPT(uart0_rx_isr, UART0_INT_VECTAB_NUM, UART0_RX_INT_PRIO)
//{
//     interrupt_global_enable(0);                     // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
//
// #if DEBUG_UART_USE_INTERRUPT                        // 婵″倹鐏夊锟介崥锟� debug 娑撴彃褰涙稉顓熸焽
//         debug_interrupr_handler();                  // 鐠嬪啰鏁� debug 娑撴彃褰涢幒銉︽暪婢跺嫮鎮婇崙鑺ユ殶 閺佺増宓佹导姘愁潶 debug 閻滎垰鑸扮紓鎾冲暱閸栭缚顕伴崣锟�
// #endif                                              // 婵″倹鐏夋穱顔芥暭娴滐拷 DEBUG_UART_INDEX 闁綀绻栧▓鍏稿敩閻線娓剁憰浣规杹閸掓澘顕惔鏃傛畱娑撴彃褰涙稉顓熸焽閸橈拷
// }

// 娑撴彃褰�1姒涙顓绘潻鐐村复閸掔増鎲氶崓蹇撱仈闁板秶鐤嗘稉鎻掑經
IFX_INTERRUPT(uart1_tx_isr, UART1_INT_VECTAB_NUM, UART1_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}
IFX_INTERRUPT(uart1_rx_isr, UART1_INT_VECTAB_NUM, UART1_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    camera_uart_handler();      // 閹藉嫬鍎氭径鏉戝棘閺佷即鍘ょ純顔剧埠娑擄拷閸ョ偠鐨熼崙鑺ユ殶
}

// 娑撴彃褰�2姒涙顓绘潻鐐村复閸掔増妫ょ痪鑳祮娑撴彃褰涘Ο鈥虫健
IFX_INTERRUPT(uart2_tx_isr, UART2_INT_VECTAB_NUM, UART2_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart2_rx_isr, UART2_INT_VECTAB_NUM, UART2_RX_INT_PRIO)
{
    interrupt_global_enable(0);     // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    wireless_module_uart_handler(); // 閺冪姷鍤庡Ο鈥虫健缂佺喍绔撮崶鐐剁殶閸戣姤鏆�
}
// 娑撴彃褰�3姒涙顓绘潻鐐村复閸掔檸PS鐎规矮缍呭Ο鈥虫健
IFX_INTERRUPT(uart3_tx_isr, UART3_INT_VECTAB_NUM, UART3_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart3_rx_isr, UART3_INT_VECTAB_NUM, UART3_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    gnss_uart_callback();       // GNSS娑撴彃褰涢崶鐐剁殶閸戣姤鏆�
}

IFX_INTERRUPT(uart4_tx_isr, UART4_INT_VECTAB_NUM, UART4_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart4_rx_isr, UART4_INT_VECTAB_NUM, UART4_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart5_tx_isr, UART5_INT_VECTAB_NUM, UART5_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart5_rx_isr, UART5_INT_VECTAB_NUM, UART5_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart6_tx_isr, UART6_INT_VECTAB_NUM, UART6_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart6_rx_isr, UART6_INT_VECTAB_NUM, UART6_RX_INT_PRIO)
{
    interrupt_global_enable(0);        // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    stp23l_receiver_callback(&lidar2); // STP23L濠碉拷閸忓娴勬潏鍙ヨ閸欙絽娲栫拫锟�
}

IFX_INTERRUPT(uart8_tx_isr, UART8_INT_VECTAB_NUM, UART8_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart8_rx_isr, UART8_INT_VECTAB_NUM, UART8_RX_INT_PRIO)
{
    interrupt_global_enable(0);        // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    stp23l_receiver_callback(&lidar1); // STP23L濠碉拷閸忓娴勬潏鍙ヨ閸欙絽娲栫拫锟�
}

IFX_INTERRUPT(uart9_tx_isr, UART9_INT_VECTAB_NUM, UART9_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart9_rx_isr, UART9_INT_VECTAB_NUM, UART9_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart10_tx_isr, UART10_INT_VECTAB_NUM, UART10_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart10_rx_isr, UART10_INT_VECTAB_NUM, UART10_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    maixcam_uart_handler(&maixcam1);   // MaixCam濠碉拷閸忓娴勬潏鍙ヨ閸欙絽娲栫拫锟�
}

IFX_INTERRUPT(uart11_tx_isr, UART11_INT_VECTAB_NUM, UART11_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}

IFX_INTERRUPT(uart11_rx_isr, UART11_INT_VECTAB_NUM, UART11_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
}
// 娑撴彃褰涢柅姘愁唵闁挎瑨顕ゆ稉顓熸焽
IFX_INTERRUPT(uart0_er_isr, UART0_INT_VECTAB_NUM, UART0_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart0_handle);
}
IFX_INTERRUPT(uart1_er_isr, UART1_INT_VECTAB_NUM, UART1_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart1_handle);
}
IFX_INTERRUPT(uart2_er_isr, UART2_INT_VECTAB_NUM, UART2_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart2_handle);
}
IFX_INTERRUPT(uart3_er_isr, UART3_INT_VECTAB_NUM, UART3_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart3_handle);
}
IFX_INTERRUPT(uart4_er_isr, UART4_INT_VECTAB_NUM, UART4_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart4_handle);
}
IFX_INTERRUPT(uart5_er_isr, UART5_INT_VECTAB_NUM, UART5_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart5_handle);
}
IFX_INTERRUPT(uart6_er_isr, UART6_INT_VECTAB_NUM, UART6_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart6_handle);
}
IFX_INTERRUPT(uart8_er_isr, UART8_INT_VECTAB_NUM, UART8_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart8_handle);
}
IFX_INTERRUPT(uart9_er_isr, UART9_INT_VECTAB_NUM, UART9_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart9_handle);
}
IFX_INTERRUPT(uart10_er_isr, UART10_INT_VECTAB_NUM, UART10_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart10_handle);
}
IFX_INTERRUPT(uart11_er_isr, UART11_INT_VECTAB_NUM, UART11_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 瀵拷閸氼垯鑵戦弬顓炵サ婵傦拷
    IfxAsclin_Asc_isrError(&uart11_handle);
}
// **************************** 娑撴彃褰涙稉顓熸焽閸戣姤鏆� ****************************
