#ifndef MAX30102_ALGO_H
#define MAX30102_ALGO_H

#include "zf_common_typedef.h"

#define FS 100
#define BUFFER_SIZE (FS * 5)
#define HR_FIFO_SIZE 7
#define MA4_SIZE 4     // DO NOT CHANGE
#define HAMMING_SIZE 5 // DO NOT CHANGE
#define min(x, y) ((x) < (y) ? (x) : (y))

extern const vuint16 auw_hamm[31]; // Hamm=  long16(512* hamming(5)');
// uch_spo2_table is computed as  -45.060*ratioAverage* ratioAverage + 30.354 *ratioAverage + 94.845 ;
extern const vuint8 uch_spo2_table[184];

void maxim_heart_rate_and_oxygen_saturation(vuint32 *pun_ir_buffer, vint32 n_ir_buffer_length, vuint32 *pun_red_buffer, vint32 *pn_spo2, int8_t *pch_spo2_valid, vint32 *pn_heart_rate, int8_t *pch_hr_valid);
void maxim_find_peaks(vint32 *pn_locs, vint32 *pn_npks, vint32 *pn_x, vint32 n_size, vint32 n_min_height, vint32 n_min_distance, vint32 n_max_num);
void maxim_peaks_above_min_height(vint32 *pn_locs, vint32 *pn_npks, vint32 *pn_x, vint32 n_size, vint32 n_min_height);
void maxim_remove_close_peaks(vint32 *pn_locs, vint32 *pn_npks, vint32 *pn_x, vint32 n_min_distance);
void maxim_sort_ascend(vint32 *pn_x, vint32 n_size);
void maxim_sort_indices_descend(vint32 *pn_x, vint32 *pn_indx, vint32 n_size);

#endif