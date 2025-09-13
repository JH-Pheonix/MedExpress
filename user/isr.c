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
 * 鏂囦欢鍚嶇О          isr
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

#include "stp23l.h"
#include "key.h"
#include "isr_config.h"
#include "isr.h"
#include "Attitude.h"
#include "pid_control.h"
#include "init.h"

// 瀵逛簬TC绯诲垪榛樿鏄笉鏀寔涓柇宓屽鐨勶紝甯屾湜鏀寔涓柇宓屽闇�瑕佸湪涓柇鍐呬娇鐢� interrupt_global_enable(0); 鏉ュ紑鍚腑鏂祵濂�
// 绠�鍗曠偣璇村疄闄呬笂杩涘叆涓柇鍚嶵C绯诲垪鐨勭‖浠惰嚜鍔ㄨ皟鐢ㄤ簡 interrupt_global_disable(); 鏉ユ嫆缁濆搷搴斾换浣曠殑涓柇锛屽洜姝ら渶瑕佹垜浠嚜宸辨墜鍔ㄨ皟鐢� interrupt_global_enable(0); 鏉ュ紑鍚腑鏂殑鍝嶅簲銆�

// **************************** PIT涓柇鍑芥暟 ****************************
IFX_INTERRUPT(cc60_pit_ch0_isr, CCU6_0_CH0_INT_VECTAB_NUM, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    pit_clear_flag(CCU60_CH0);
    key_callback_func();
}
#include "pid_control.h"
IFX_INTERRUPT(cc60_pit_ch1_isr, CCU6_0_CH1_INT_VECTAB_NUM, CCU6_0_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    pit_clear_flag(CCU60_CH1);
    attitude_cal();
    float now_speed=100,d_isr=0.02;// mm/s
    if(now_dir==Dir_front) position_X+=d_isr*now_speed;
    if(now_dir==Dir_back) position_X-=d_isr*now_speed;
    if(now_dir==Dir_right) position_Y+=d_isr*now_speed;
    if(now_dir==Dir_left) position_Y-=d_isr*now_speed;
}

IFX_INTERRUPT(cc61_pit_ch0_isr, CCU6_1_CH0_INT_VECTAB_NUM, CCU6_1_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    pit_clear_flag(CCU61_CH0);
    control_handler();
}

IFX_INTERRUPT(cc61_pit_ch1_isr, CCU6_1_CH1_INT_VECTAB_NUM, CCU6_1_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    pit_clear_flag(CCU61_CH1);
}
// **************************** PIT涓柇鍑芥暟 ****************************

// **************************** 澶栭儴涓柇鍑芥暟 ****************************
IFX_INTERRUPT(exti_ch0_ch4_isr, EXTI_CH0_CH4_INT_VECTAB_NUM, EXTI_CH0_CH4_INT_PRIO)
{
    interrupt_global_enable(0);            // 寮�鍚腑鏂祵濂�
    if (exti_flag_get(ERU_CH0_REQ0_P15_4)) // 閫氶亾0涓柇
    {
        exti_flag_clear(ERU_CH0_REQ0_P15_4);
    }

    if (exti_flag_get(ERU_CH4_REQ13_P15_5)) // 閫氶亾4涓柇
    {
        exti_flag_clear(ERU_CH4_REQ13_P15_5);
    }
}

IFX_INTERRUPT(exti_ch1_ch5_isr, EXTI_CH1_CH5_INT_VECTAB_NUM, EXTI_CH1_CH5_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�

    if (exti_flag_get(ERU_CH1_REQ10_P14_3)) // 閫氶亾1涓柇
    {
        exti_flag_clear(ERU_CH1_REQ10_P14_3);
    }

    if (exti_flag_get(ERU_CH5_REQ1_P15_8)) // 閫氶亾5涓柇
    {
        exti_flag_clear(ERU_CH5_REQ1_P15_8);
    }
}

// 鐢变簬鎽勫儚澶磒clk寮曡剼榛樿鍗犵敤浜� 2閫氶亾锛岀敤浜庤Е鍙慏MA锛屽洜姝よ繖閲屼笉鍐嶅畾涔変腑鏂嚱鏁�
// IFX_INTERRUPT(exti_ch2_ch6_isr, EXTI_CH2_CH6_INT_VECTAB_NUM, EXTI_CH2_CH6_INT_PRIO)
// {
//  interrupt_global_enable(0);                     // 寮�鍚腑鏂祵濂�
//  if(exti_flag_get(ERU_CH2_REQ7_P00_4))           // 閫氶亾2涓柇
//  {
//      exti_flag_clear(ERU_CH2_REQ7_P00_4);
//  }
//  if(exti_flag_get(ERU_CH6_REQ9_P20_0))           // 閫氶亾6涓柇
//  {
//      exti_flag_clear(ERU_CH6_REQ9_P20_0);
//  }
// }

IFX_INTERRUPT(exti_ch3_ch7_isr, EXTI_CH3_CH7_INT_VECTAB_NUM, EXTI_CH3_CH7_INT_PRIO)
{
    interrupt_global_enable(0);            // 寮�鍚腑鏂祵濂�
    if (exti_flag_get(ERU_CH3_REQ6_P02_0)) // 閫氶亾3涓柇
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
    }
    if (exti_flag_get(ERU_CH7_REQ16_P15_1)) // 閫氶亾7涓柇
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);
    }
}
// **************************** 澶栭儴涓柇鍑芥暟 ****************************

// **************************** DMA涓柇鍑芥暟 ****************************
IFX_INTERRUPT(dma_ch5_isr, DMA_INT_VECTAB_NUM, DMA_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}
// **************************** DMA涓柇鍑芥暟 ****************************

// **************************** 涓插彛涓柇鍑芥暟 ****************************
// 涓插彛0榛樿浣滀负璋冭瘯涓插彛
IFX_INTERRUPT(uart0_tx_isr, UART0_INT_VECTAB_NUM, UART0_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}
// IFX_INTERRUPT(uart0_rx_isr, UART0_INT_VECTAB_NUM, UART0_RX_INT_PRIO)
//{
//     interrupt_global_enable(0);                     // 寮�鍚腑鏂祵濂�
//
// #if DEBUG_UART_USE_INTERRUPT                        // 濡傛灉寮�鍚� debug 涓插彛涓柇
//         debug_interrupr_handler();                  // 璋冪敤 debug 涓插彛鎺ユ敹澶勭悊鍑芥暟 鏁版嵁浼氳 debug 鐜舰缂撳啿鍖鸿鍙�
// #endif                                              // 濡傛灉淇敼浜� DEBUG_UART_INDEX 閭ｈ繖娈典唬鐮侀渶瑕佹斁鍒板搴旂殑涓插彛涓柇鍘�
// }

// 涓插彛1榛樿杩炴帴鍒版憚鍍忓ご閰嶇疆涓插彛
IFX_INTERRUPT(uart1_tx_isr, UART1_INT_VECTAB_NUM, UART1_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}
IFX_INTERRUPT(uart1_rx_isr, UART1_INT_VECTAB_NUM, UART1_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    camera_uart_handler();      // 鎽勫儚澶村弬鏁伴厤缃粺涓�鍥炶皟鍑芥暟
}

// 涓插彛2榛樿杩炴帴鍒版棤绾胯浆涓插彛妯″潡
IFX_INTERRUPT(uart2_tx_isr, UART2_INT_VECTAB_NUM, UART2_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart2_rx_isr, UART2_INT_VECTAB_NUM, UART2_RX_INT_PRIO)
{
    interrupt_global_enable(0);     // 寮�鍚腑鏂祵濂�
    wireless_module_uart_handler(); // 鏃犵嚎妯″潡缁熶竴鍥炶皟鍑芥暟
}
// 涓插彛3榛樿杩炴帴鍒癎PS瀹氫綅妯″潡
IFX_INTERRUPT(uart3_tx_isr, UART3_INT_VECTAB_NUM, UART3_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart3_rx_isr, UART3_INT_VECTAB_NUM, UART3_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    gnss_uart_callback();       // GNSS涓插彛鍥炶皟鍑芥暟
}

IFX_INTERRUPT(uart4_tx_isr, UART4_INT_VECTAB_NUM, UART4_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart4_rx_isr, UART4_INT_VECTAB_NUM, UART4_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart5_tx_isr, UART5_INT_VECTAB_NUM, UART5_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart5_rx_isr, UART5_INT_VECTAB_NUM, UART5_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart6_tx_isr, UART6_INT_VECTAB_NUM, UART6_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart6_rx_isr, UART6_INT_VECTAB_NUM, UART6_RX_INT_PRIO)
{
    interrupt_global_enable(0);        // 寮�鍚腑鏂祵濂�
    stp23l_receiver_callback(&lidar2); // STP23L婵�鍏夐浄杈句覆鍙ｅ洖璋�
}

IFX_INTERRUPT(uart8_tx_isr, UART8_INT_VECTAB_NUM, UART8_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart8_rx_isr, UART8_INT_VECTAB_NUM, UART8_RX_INT_PRIO)
{
    interrupt_global_enable(0);        // 寮�鍚腑鏂祵濂�
    stp23l_receiver_callback(&lidar1); // STP23L婵�鍏夐浄杈句覆鍙ｅ洖璋�
}

IFX_INTERRUPT(uart9_tx_isr, UART9_INT_VECTAB_NUM, UART9_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart9_rx_isr, UART9_INT_VECTAB_NUM, UART9_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart10_tx_isr, UART10_INT_VECTAB_NUM, UART10_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart10_rx_isr, UART10_INT_VECTAB_NUM, UART10_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart11_tx_isr, UART11_INT_VECTAB_NUM, UART11_TX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}

IFX_INTERRUPT(uart11_rx_isr, UART11_INT_VECTAB_NUM, UART11_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
}
// 涓插彛閫氳閿欒涓柇
IFX_INTERRUPT(uart0_er_isr, UART0_INT_VECTAB_NUM, UART0_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart0_handle);
}
IFX_INTERRUPT(uart1_er_isr, UART1_INT_VECTAB_NUM, UART1_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart1_handle);
}
IFX_INTERRUPT(uart2_er_isr, UART2_INT_VECTAB_NUM, UART2_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart2_handle);
}
IFX_INTERRUPT(uart3_er_isr, UART3_INT_VECTAB_NUM, UART3_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart3_handle);
}
IFX_INTERRUPT(uart4_er_isr, UART4_INT_VECTAB_NUM, UART4_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart4_handle);
}
IFX_INTERRUPT(uart5_er_isr, UART5_INT_VECTAB_NUM, UART5_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart5_handle);
}
IFX_INTERRUPT(uart6_er_isr, UART6_INT_VECTAB_NUM, UART6_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart6_handle);
}
IFX_INTERRUPT(uart8_er_isr, UART8_INT_VECTAB_NUM, UART8_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart8_handle);
}
IFX_INTERRUPT(uart9_er_isr, UART9_INT_VECTAB_NUM, UART9_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart9_handle);
}
IFX_INTERRUPT(uart10_er_isr, UART10_INT_VECTAB_NUM, UART10_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart10_handle);
}
IFX_INTERRUPT(uart11_er_isr, UART11_INT_VECTAB_NUM, UART11_ER_INT_PRIO)
{
    interrupt_global_enable(0); // 寮�鍚腑鏂祵濂�
    IfxAsclin_Asc_isrError(&uart11_handle);
}
// **************************** 涓插彛涓柇鍑芥暟 ****************************
