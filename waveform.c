/*
 * waveform.h
 *
 *  Created on: 05.09.2014
 *      Author: andreas.meier
 */

#ifndef WAVEFORM_H_
#define WAVEFORM_H_

#include "waveform.h"
#include "types.h"
#include "config.h"
#include "UC8156.h"

u8 waveform_debug[WAVEFORM_LENGTH]= { \
0xAA, 0x6A, 0xAA, 0xAA, \
0x22, 0x22, 0x22, 0x22, \
0x00, 0x00, 0x00, 0x00, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0x11, 0x11, 0x11, 0x11, \
0xF1, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
};

u8 waveform_long_null[WAVEFORM_LENGTH] = { \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xAA, 0xAA, 0xAA, 0xAA, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0xF1, \
                0x01, \
                0x01, \
                0x01, \
                0x01, \
                0x01, \
                0x01, \
                0x01, \
                0x01, \
                0x01, \
};

u8 waveform_test[WAVEFORM_LENGTH]= { \
0xA9, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xAA, 0xAA, 0xAA, 0xAA, \
0xF1, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01, \
0x01 \
};

u8 waveform_default[WAVEFORM_LENGTH]= { \
0xAA, 0xAA, 0xA9, 0xA6,\
0xAA, 0xAA, 0xA5, 0xA6,\
0xAA, 0xA9, 0xA5, 0x96,\
0xA8, 0xA4, 0x94, 0x56,\
0xA0, 0xA0, 0x50, 0x52,\
0xA0, 0x90, 0x50, 0x52,\
0x80, 0x40, 0x40, 0x42,\
0x01, 0x01, 0x01, 0x01,\
0x05, 0x05, 0x05, 0x05,\
0x15, 0x15, 0x15, 0x15,\
0xAA, 0xAA, 0xAA, 0xAA,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0xFF, 0xFF, 0xFF, 0xFF,\
0x41,\
0x31,\
0x41,\
0x41,\
0x31,\
0x41,\
0x41,\
0x41,\
0x71,\
0x41,\
0x41,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00,\
0x00\
};

#endif /* WAVEFORM_H_ */