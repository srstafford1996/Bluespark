/*
 * H.26L/H.264/AVC/JVT/14496-10/... encoder/decoder
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * Context Adaptive Binary Arithmetic Coder.
 */

#include <string.h>

#include "libavutil/common.h"
#include "libavutil/timer.h"
#include "get_bits.h"
#include "cabac.h"
#include "cabac_functions.h"

const uint8_t ff_h264_cabac_tables[512 + 4*2*64 + 4*64 + 63] = {
    9,8,7,7,6,6,6,6,5,5,5,5,5,5,5,5,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    // LPS range
    -128,    -128,    -128,    -128,    -128,    -128,    123,     123,
    116,     116,     111,     111,     105,     105,     100,     100,
    95,      95,      90,      90,      85,      85,      81,      81,
    77,      77,      73,      73,      69,      69,      66,      66,
    62,      62,      59,      59,      56,      56,      53,      53,
    51,      51,      48,      48,      46,      46,      43,      43,
    41,      41,      39,      39,      37,      37,      35,      35,
    33,      33,      32,      32,      30,      30,      29,      29,
    27,      27,      26,      26,      24,      24,      23,      23,
    22,      22,      21,      21,      20,      20,      19,      19,
    18,      18,      17,      17,      16,      16,      15,      15,
    14,      14,      14,      14,      13,      13,      12,      12,
    12,      12,      11,      11,      11,      11,      10,      10,
    10,      10,      9,       9,       9,       9,       8,       8,
    8,       8,       7,       7,       7,       7,       7,       7,
    6,       6,       6,       6,       6,       6,       2,       2,
    -80,     -80,     -89,     -89,     -98,     -98,     -106,    -106,
    -114,    -114,    -121,    -121,    -128,    -128,    122,     122,
    116,     116,     110,     110,     104,     104,     99,      99,
    94,      94,      89,      89,      85,      85,      80,      80,
    76,      76,      72,      72,      69,      69,      65,      65,
    62,      62,      59,      59,      56,      56,      53,      53,
    50,      50,      48,      48,      45,      45,      43,      43,
    41,      41,      39,      39,      37,      37,      35,      35,
    33,      33,      31,      31,      30,      30,      28,      28,
    27,      27,      26,      26,      24,      24,      23,      23,
    22,      22,      21,      21,      20,      20,      19,      19,
    18,      18,      17,      17,      16,      16,      15,      15,
    14,      14,      14,      14,      13,      13,      12,      12,
    12,      12,      11,      11,      11,      11,      10,      10,
    9,       9,       9,       9,       9,       9,       8,       8,
    8,       8,       7,       7,       7,       7,       2,       2,
    -48,     -48,     -59,     -59,     -69,     -69,     -78,     -78,
    -87,     -87,     -96,     -96,     -104,    -104,    -112,    -112,
    -119,    -119,    -126,    -126,    123,     123,     117,     117,
    111,     111,     105,     105,     100,     100,     95,      95,
    90,      90,      86,      86,      81,      81,      77,      77,
    73,      73,      69,      69,      66,      66,      63,      63,
    59,      59,      56,      56,      54,      54,      51,      51,
    48,      48,      46,      46,      43,      43,      41,      41,
    39,      39,      37,      37,      35,      35,      33,      33,
    32,      32,      30,      30,      29,      29,      27,      27,
    26,      26,      25,      25,      23,      23,      22,      22,
    21,      21,      20,      20,      19,      19,      18,      18,
    17,      17,      16,      16,      15,      15,      15,      15,
    14,      14,      13,      13,      12,      12,      12,      12,
    11,      11,      11,      11,      10,      10,      10,      10,
    9,       9,       9,       9,       8,       8,       2,       2,
    -16,     -16,     -29,     -29,     -40,     -40,     -51,     -51,
    -61,     -61,     -71,     -71,     -81,     -81,     -90,     -90,
    -98,     -98,     -106,    -106,    -114,    -114,    -121,    -121,
    -128,    -128,    122,     122,     116,     116,     110,     110,
    104,     104,     99,      99,      94,      94,      89,      89,
    85,      85,      80,      80,      76,      76,      72,      72,
    69,      69,      65,      65,      62,      62,      59,      59,
    56,      56,      53,      53,      50,      50,      48,      48,
    45,      45,      43,      43,      41,      41,      39,      39,
    37,      37,      35,      35,      33,      33,      31,      31,
    30,      30,      28,      28,      27,      27,      25,      25,
    24,      24,      23,      23,      22,      22,      21,      21,
    20,      20,      19,      19,      18,      18,      17,      17,
    16,      16,      15,      15,      14,      14,      14,      14,
    13,      13,      12,      12,      12,      12,      11,      11,
    11,      11,      10,      10,      9,       9,       2,       2,
    // mlps state
    127,     126,     77,      76,      77,      76,      75,      74,
    75,      74,      75,      74,      73,      72,      73,      72,
    73,      72,      71,      70,      71,      70,      71,      70,
    69,      68,      69,      68,      67,      66,      67,      66,
    67,      66,      65,      64,      65,      64,      63,      62,
    61,      60,      61,      60,      61,      60,      59,      58,
    59,      58,      57,      56,      55,      54,      55,      54,
    53,      52,      53,      52,      51,      50,      49,      48,
    49,      48,      47,      46,      45,      44,      45,      44,
    43,      42,      43,      42,      39,      38,      39,      38,
    37,      36,      37,      36,      33,      32,      33,      32,
    31,      30,      31,      30,      27,      26,      27,      26,
    25,      24,      23,      22,      23,      22,      19,      18,
    19,      18,      17,      16,      15,      14,      13,      12,
    11,      10,      9,       8,       9,       8,       5,       4,
    5,       4,       3,       2,       1,       0,       0,       1,
    2,       3,       4,       5,       6,       7,       8,       9,
    10,      11,      12,      13,      14,      15,      16,      17,
    18,      19,      20,      21,      22,      23,      24,      25,
    26,      27,      28,      29,      30,      31,      32,      33,
    34,      35,      36,      37,      38,      39,      40,      41,
    42,      43,      44,      45,      46,      47,      48,      49,
    50,      51,      52,      53,      54,      55,      56,      57,
    58,      59,      60,      61,      62,      63,      64,      65,
    66,      67,      68,      69,      70,      71,      72,      73,
    74,      75,      76,      77,      78,      79,      80,      81,
    82,      83,      84,      85,      86,      87,      88,      89,
    90,      91,      92,      93,      94,      95,      96,      97,
    98,      99,      100,     101,     102,     103,     104,     105,
    106,     107,     108,     109,     110,     111,     112,     113,
    114,     115,     116,     117,     118,     119,     120,     121,
    122,     123,     124,     125,     124,     125,     126,     127,
    // last_coeff_flag_offset_8x8
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8
};

/**
 *
 * @param buf_size size of buf in bits
 */
void ff_init_cabac_encoder(CABACContext *c, uint8_t *buf, int buf_size){
    init_put_bits(&c->pb, buf, buf_size);

    c->low= 0;
    c->range= 0x1FE;
    c->outstanding_count= 0;
    c->pb.bit_left++; //avoids firstBitFlag
}

/**
 *
 * @param buf_size size of buf in bits
 */
int ff_init_cabac_decoder(CABACContext *c, const uint8_t *buf, int buf_size){
    c->bytestream_start=
    c->bytestream= buf;
    c->bytestream_end= buf + buf_size;

#if CABAC_BITS == 16
    c->low =  (*c->bytestream++)<<18;
    c->low+=  (*c->bytestream++)<<10;
    // Keep our fetches on a 2-byte boundry as this should avoid ever having to
    // do unaligned loads if the compiler (or asm) optimises the double byte
    // load into a single instruction
    if(((uintptr_t)c->bytestream & 1) == 0) {
        c->low += (1 << 9);
    }
    else {
        c->low += ((*c->bytestream++) << 2) + 2;
    }
#else
    c->low =  (*c->bytestream++)<<10;
    c->low+= ((*c->bytestream++)<<2) + 2;
#endif
    c->range= 0x1FE;
    if ((c->range<<(CABAC_BITS+1)) < c->low)
        return AVERROR_INVALIDDATA;
    return 0;
}

#ifdef TEST
#define SIZE 10240

#include "libavutil/lfg.h"
#include "avcodec.h"

static inline void put_cabac_bit(CABACContext *c, int b){
    put_bits(&c->pb, 1, b);
    for(;c->outstanding_count; c->outstanding_count--){
        put_bits(&c->pb, 1, 1-b);
    }
}

static inline void renorm_cabac_encoder(CABACContext *c){
    while(c->range < 0x100){
        //FIXME optimize
        if(c->low<0x100){
            put_cabac_bit(c, 0);
        }else if(c->low<0x200){
            c->outstanding_count++;
            c->low -= 0x100;
        }else{
            put_cabac_bit(c, 1);
            c->low -= 0x200;
        }

        c->range+= c->range;
        c->low += c->low;
    }
}

static void put_cabac(CABACContext *c, uint8_t * const state, int bit){
    int RangeLPS= ff_h264_lps_range[2*(c->range&0xC0) + *state];

    if(bit == ((*state)&1)){
        c->range -= RangeLPS;
        *state    = ff_h264_mlps_state[128 + *state];
    }else{
        c->low += c->range - RangeLPS;
        c->range = RangeLPS;
        *state= ff_h264_mlps_state[127 - *state];
    }

    renorm_cabac_encoder(c);
}

/**
 * @param bit 0 -> write zero bit, !=0 write one bit
 */
static void put_cabac_bypass(CABACContext *c, int bit){
    c->low += c->low;

    if(bit){
        c->low += c->range;
    }
//FIXME optimize
    if(c->low<0x200){
        put_cabac_bit(c, 0);
    }else if(c->low<0x400){
        c->outstanding_count++;
        c->low -= 0x200;
    }else{
        put_cabac_bit(c, 1);
        c->low -= 0x400;
    }
}

/**
 *
 * @return the number of bytes written
 */
static int put_cabac_terminate(CABACContext *c, int bit){
    c->range -= 2;

    if(!bit){
        renorm_cabac_encoder(c);
    }else{
        c->low += c->range;
        c->range= 2;

        renorm_cabac_encoder(c);

        av_assert0(c->low <= 0x1FF);
        put_cabac_bit(c, c->low>>9);
        put_bits(&c->pb, 2, ((c->low>>7)&3)|1);

        flush_put_bits(&c->pb); //FIXME FIXME FIXME XXX wrong
    }

    return (put_bits_count(&c->pb)+7)>>3;
}

int main(void){
    CABACContext c;
    uint8_t b[9*SIZE];
    uint8_t r[9*SIZE];
    int i, ret = 0;
    uint8_t state[10]= {0};
    AVLFG prng;

    av_lfg_init(&prng, 1);
    ff_init_cabac_encoder(&c, b, SIZE);

    for(i=0; i<SIZE; i++){
        if(2*i<SIZE) r[i] = av_lfg_get(&prng) % 7;
        else         r[i] = (i>>8)&1;
    }

    for(i=0; i<SIZE; i++){
        put_cabac_bypass(&c, r[i]&1);
    }

    for(i=0; i<SIZE; i++){
        put_cabac(&c, state, r[i]&1);
    }

    i= put_cabac_terminate(&c, 1);
    b[i++] = av_lfg_get(&prng);
    b[i  ] = av_lfg_get(&prng);

    ff_init_cabac_decoder(&c, b, SIZE);

    memset(state, 0, sizeof(state));

    for(i=0; i<SIZE; i++){
        if( (r[i]&1) != get_cabac_bypass(&c) ) {
            av_log(NULL, AV_LOG_ERROR, "CABAC bypass failure at %d\n", i);
            ret = 1;
        }
    }

    for(i=0; i<SIZE; i++){
        if( (r[i]&1) != get_cabac_noinline(&c, state) ) {
            av_log(NULL, AV_LOG_ERROR, "CABAC failure at %d\n", i);
            ret = 1;
        }
    }
    if(!get_cabac_terminate(&c)) {
        av_log(NULL, AV_LOG_ERROR, "where's the Terminator?\n");
        ret = 1;
    }

    return ret;
}

#endif /* TEST */
