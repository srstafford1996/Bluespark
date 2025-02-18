/*
 * VPK demuxer
 * Copyright (c) 2015 Paul B Mahol
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

#include "libavutil/intreadwrite.h"
#include "avformat.h"
#include "internal.h"

typedef struct VPKDemuxContext {
    unsigned block_count;
    unsigned current_block;
    unsigned last_block_size;
} VPKDemuxContext;

static int vpk_probe(AVProbeData *p)
{
    if (AV_RL32(p->buf) != MKBETAG('V','P','K',' '))
        return 0;

    return AVPROBE_SCORE_MAX / 3 * 2;
}

static int vpk_read_header(AVFormatContext *s)
{
    VPKDemuxContext *vpk = s->priv_data;
    unsigned offset;
    unsigned samples_per_block;
    AVStream *st;

    vpk->current_block = 0;
    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    avio_skip(s->pb, 4);
    st->duration           = avio_rl32(s->pb) * 28 / 16;
    offset                 = avio_rl32(s->pb);
    st->codec->codec_type  = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id    = AV_CODEC_ID_ADPCM_PSX;
    st->codec->block_align = avio_rl32(s->pb);
    st->codec->sample_rate = avio_rl32(s->pb);
    if (st->codec->sample_rate <= 0)
        return AVERROR_INVALIDDATA;
    st->codec->channels    = avio_rl32(s->pb);
    if (st->codec->channels <= 0)
        return AVERROR_INVALIDDATA;
    samples_per_block      = ((st->codec->block_align / st->codec->channels) * 28) / 16;
    if (samples_per_block <= 0)
        return AVERROR_INVALIDDATA;
    vpk->block_count       = (st->duration + (samples_per_block - 1)) / samples_per_block;
    vpk->last_block_size   = (st->duration % samples_per_block) * 16 * st->codec->channels / 28;
    avio_skip(s->pb, offset - avio_tell(s->pb));
    avpriv_set_pts_info(st, 64, 1, st->codec->sample_rate);

    return 0;
}

static int vpk_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    AVCodecContext *codec = s->streams[0]->codec;
    VPKDemuxContext *vpk = s->priv_data;
    int ret, i;

    vpk->current_block++;
    if (vpk->current_block == vpk->block_count) {
        unsigned size = vpk->last_block_size / codec->channels;
        unsigned skip = (codec->block_align - vpk->last_block_size) / codec->channels;

        ret = av_new_packet(pkt, vpk->last_block_size);
        if (ret < 0)
            return ret;
        for (i = 0; i < codec->channels; i++) {
            ret = avio_read(s->pb, pkt->data + i * size, size);
            avio_skip(s->pb, skip);
            if (ret != size) {
                av_packet_unref(pkt);
                ret = AVERROR(EIO);
                break;
            }
        }
        pkt->stream_index = 0;
    } else if (vpk->current_block < vpk->block_count) {
        ret = av_get_packet(s->pb, pkt, codec->block_align);
        pkt->stream_index = 0;
    } else {
        return AVERROR_EOF;
    }

    return ret;
}

AVInputFormat ff_vpk_demuxer = {
    .name           = "vpk",
    .long_name      = NULL_IF_CONFIG_SMALL("Sony PS2 VPK"),
    .priv_data_size = sizeof(VPKDemuxContext),
    .read_probe     = vpk_probe,
    .read_header    = vpk_read_header,
    .read_packet    = vpk_read_packet,
    .extensions     = "vpk",
};
