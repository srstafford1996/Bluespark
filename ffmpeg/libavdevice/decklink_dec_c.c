/*
 * Blackmagic DeckLink output
 * Copyright (c) 2014 Deti Fliegl
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

#include "libavformat/avformat.h"
#include "libavutil/opt.h"

#include "decklink_common_c.h"
#include "decklink_dec.h"

#define OFFSET(x) offsetof(struct decklink_cctx, x)
#define DEC AV_OPT_FLAG_DECODING_PARAM

static const AVOption options[] = {
    { "list_devices", "list available devices"  , OFFSET(list_devices), AV_OPT_TYPE_INT   , { .i64 = 0   }, 0, 1, DEC },
    { "list_formats", "list supported formats"  , OFFSET(list_formats), AV_OPT_TYPE_INT   , { .i64 = 0   }, 0, 1, DEC },
    { "bm_v210",      "v210 10 bit per channel" , OFFSET(v210),         AV_OPT_TYPE_INT   , { .i64 = 0   }, 0, 1, DEC },
    { "teletext_lines", "teletext lines bitmask", OFFSET(teletext_lines), AV_OPT_TYPE_INT64, { .i64 = 0   }, 0, 0x7ffffffffLL, DEC, "teletext_lines"},
    { "standard",     NULL,                                           0,  AV_OPT_TYPE_CONST, { .i64 = 0x7fff9fffeLL}, 0, 0,    DEC, "teletext_lines"},
    { "all",          NULL,                                           0,  AV_OPT_TYPE_CONST, { .i64 = 0x7ffffffffLL}, 0, 0,    DEC, "teletext_lines"},
    { "channels",     "number of audio channels", OFFSET(audio_channels), AV_OPT_TYPE_INT , { .i64 = 2   }, 2, 16, DEC },
    { NULL },
};

static const AVClass decklink_demuxer_class = {
    .class_name = "Blackmagic DeckLink demuxer",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
    .category   = AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

AVInputFormat ff_decklink_demuxer = {
    .name           = "decklink",
    .long_name      = NULL_IF_CONFIG_SMALL("Blackmagic DeckLink input"),
    .flags          = AVFMT_NOFILE | AVFMT_RAWPICTURE,
    .priv_class     = &decklink_demuxer_class,
    .priv_data_size = sizeof(struct decklink_cctx),
    .read_header   = ff_decklink_read_header,
    .read_packet   = ff_decklink_read_packet,
    .read_close    = ff_decklink_read_close,
};
