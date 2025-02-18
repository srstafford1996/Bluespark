/*
 * Copyright (c) 2011 Stefano Sabatini
 * Copyright (c) 2010 Baptiste Coudurier
 * Copyright (c) 2003 Michael Zucchi <notzed@ximian.com>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with FFmpeg if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * @file
 * temporal field interlace filter, ported from MPlayer/libmpcodecs
 */

#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libavutil/avassert.h"
#include "avfilter.h"
#include "internal.h"
#include "tinterlace.h"

#define OFFSET(x) offsetof(TInterlaceContext, x)
#define FLAGS AV_OPT_FLAG_FILTERING_PARAM|AV_OPT_FLAG_VIDEO_PARAM
#define TINTERLACE_FLAG_VLPF 01
#define TINTERLACE_FLAG_EXACT_TB 2

static const AVOption tinterlace_options[] = {
    {"mode",              "select interlace mode", OFFSET(mode), AV_OPT_TYPE_INT, {.i64=MODE_MERGE}, 0, MODE_NB-1, FLAGS, "mode"},
    {"merge",             "merge fields",                                 0, AV_OPT_TYPE_CONST, {.i64=MODE_MERGE},             INT_MIN, INT_MAX, FLAGS, "mode"},
    {"drop_even",         "drop even fields",                             0, AV_OPT_TYPE_CONST, {.i64=MODE_DROP_EVEN},         INT_MIN, INT_MAX, FLAGS, "mode"},
    {"drop_odd",          "drop odd fields",                              0, AV_OPT_TYPE_CONST, {.i64=MODE_DROP_ODD},          INT_MIN, INT_MAX, FLAGS, "mode"},
    {"pad",               "pad alternate lines with black",               0, AV_OPT_TYPE_CONST, {.i64=MODE_PAD},               INT_MIN, INT_MAX, FLAGS, "mode"},
    {"interleave_top",    "interleave top and bottom fields",             0, AV_OPT_TYPE_CONST, {.i64=MODE_INTERLEAVE_TOP},    INT_MIN, INT_MAX, FLAGS, "mode"},
    {"interleave_bottom", "interleave bottom and top fields",             0, AV_OPT_TYPE_CONST, {.i64=MODE_INTERLEAVE_BOTTOM}, INT_MIN, INT_MAX, FLAGS, "mode"},
    {"interlacex2",       "interlace fields from two consecutive frames", 0, AV_OPT_TYPE_CONST, {.i64=MODE_INTERLACEX2},       INT_MIN, INT_MAX, FLAGS, "mode"},
    {"mergex2",           "merge fields keeping same frame rate",         0, AV_OPT_TYPE_CONST, {.i64=MODE_MERGEX2},           INT_MIN, INT_MAX, FLAGS, "mode"},

    {"flags",             "set flags", OFFSET(flags), AV_OPT_TYPE_FLAGS, {.i64 = 0}, 0, INT_MAX, 0, "flags" },
    {"low_pass_filter",   "enable vertical low-pass filter",              0, AV_OPT_TYPE_CONST, {.i64 = TINTERLACE_FLAG_VLPF}, INT_MIN, INT_MAX, FLAGS, "flags" },
    {"vlpf",              "enable vertical low-pass filter",              0, AV_OPT_TYPE_CONST, {.i64 = TINTERLACE_FLAG_VLPF}, INT_MIN, INT_MAX, FLAGS, "flags" },
    {"exact_tb",          "force a timebase which can represent timestamps exactly", 0, AV_OPT_TYPE_CONST, {.i64 = TINTERLACE_FLAG_EXACT_TB}, INT_MIN, INT_MAX, FLAGS, "flags" },

    {NULL}
};

AVFILTER_DEFINE_CLASS(tinterlace);

#define FULL_SCALE_YUVJ_FORMATS \
    AV_PIX_FMT_YUVJ420P, AV_PIX_FMT_YUVJ422P, AV_PIX_FMT_YUVJ444P, AV_PIX_FMT_YUVJ440P

static const enum AVPixelFormat full_scale_yuvj_pix_fmts[] = {
    FULL_SCALE_YUVJ_FORMATS, AV_PIX_FMT_NONE
};

static const AVRational standard_tbs[] = {
    {1, 25},
    {1, 30},
    {1001, 30000},
};

static int query_formats(AVFilterContext *ctx)
{
    static const enum AVPixelFormat pix_fmts[] = {
        AV_PIX_FMT_YUV410P, AV_PIX_FMT_YUV411P,
        AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUV422P,
        AV_PIX_FMT_YUV440P, AV_PIX_FMT_YUV444P,
        AV_PIX_FMT_YUVA420P, AV_PIX_FMT_YUVA422P, AV_PIX_FMT_YUVA444P,
        AV_PIX_FMT_GRAY8, FULL_SCALE_YUVJ_FORMATS,
        AV_PIX_FMT_NONE
    };

    AVFilterFormats *fmts_list = ff_make_format_list(pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}

static void lowpass_line_c(uint8_t *dstp, ptrdiff_t width, const uint8_t *srcp,
                           const uint8_t *srcp_above, const uint8_t *srcp_below)
{
    int i;
    for (i = 0; i < width; i++) {
        // this calculation is an integer representation of
        // '0.5 * current + 0.25 * above + 0.25 * below'
        // '1 +' is for rounding.
        dstp[i] = (1 + srcp[i] + srcp[i] + srcp_above[i] + srcp_below[i]) >> 2;
    }
}

static av_cold void uninit(AVFilterContext *ctx)
{
    TInterlaceContext *tinterlace = ctx->priv;

    av_frame_free(&tinterlace->cur );
    av_frame_free(&tinterlace->next);
    av_freep(&tinterlace->black_data[0]);
}

static int config_out_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = outlink->src->inputs[0];
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(outlink->format);
    TInterlaceContext *tinterlace = ctx->priv;
    int i;

    tinterlace->vsub = desc->log2_chroma_h;
    outlink->w = inlink->w;
    outlink->h = tinterlace->mode == MODE_MERGE || tinterlace->mode == MODE_PAD || tinterlace->mode == MODE_MERGEX2?
        inlink->h*2 : inlink->h;
    if (tinterlace->mode == MODE_MERGE || tinterlace->mode == MODE_PAD || tinterlace->mode == MODE_MERGEX2)
        outlink->sample_aspect_ratio = av_mul_q(inlink->sample_aspect_ratio,
                                                av_make_q(2, 1));

    if (tinterlace->mode == MODE_PAD) {
        uint8_t black[4] = { 16, 128, 128, 16 };
        int i, ret;
        if (ff_fmt_is_in(outlink->format, full_scale_yuvj_pix_fmts))
            black[0] = black[3] = 0;
        ret = av_image_alloc(tinterlace->black_data, tinterlace->black_linesize,
                             outlink->w, outlink->h, outlink->format, 16);
        if (ret < 0)
            return ret;

        /* fill black picture with black */
        for (i = 0; i < 4 && tinterlace->black_data[i]; i++) {
            int h = i == 1 || i == 2 ? AV_CEIL_RSHIFT(outlink->h, desc->log2_chroma_h) : outlink->h;
            memset(tinterlace->black_data[i], black[i],
                   tinterlace->black_linesize[i] * h);
        }
    }
    if ((tinterlace->flags & TINTERLACE_FLAG_VLPF)
            && !(tinterlace->mode == MODE_INTERLEAVE_TOP
              || tinterlace->mode == MODE_INTERLEAVE_BOTTOM)) {
        av_log(ctx, AV_LOG_WARNING, "low_pass_filter flag ignored with mode %d\n",
                tinterlace->mode);
        tinterlace->flags &= ~TINTERLACE_FLAG_VLPF;
    }
    tinterlace->preout_time_base = inlink->time_base;
    if (tinterlace->mode == MODE_INTERLACEX2) {
        tinterlace->preout_time_base.den *= 2;
        outlink->frame_rate = av_mul_q(inlink->frame_rate, (AVRational){2,1});
        outlink->time_base  = av_mul_q(inlink->time_base , (AVRational){1,2});
    } else if (tinterlace->mode == MODE_MERGEX2) {
        outlink->frame_rate = inlink->frame_rate;
        outlink->time_base  = inlink->time_base;
    } else if (tinterlace->mode != MODE_PAD) {
        outlink->frame_rate = av_mul_q(inlink->frame_rate, (AVRational){1,2});
        outlink->time_base  = av_mul_q(inlink->time_base , (AVRational){2,1});
    }

    for (i = 0; i<FF_ARRAY_ELEMS(standard_tbs); i++){
        if (!av_cmp_q(standard_tbs[i], outlink->time_base))
            break;
    }
    if (i == FF_ARRAY_ELEMS(standard_tbs) ||
        (tinterlace->flags & TINTERLACE_FLAG_EXACT_TB))
        outlink->time_base = tinterlace->preout_time_base;

    if (tinterlace->flags & TINTERLACE_FLAG_VLPF) {
        tinterlace->lowpass_line = lowpass_line_c;
        if (ARCH_X86)
            ff_tinterlace_init_x86(tinterlace);
    }

    av_log(ctx, AV_LOG_VERBOSE, "mode:%d filter:%s h:%d -> h:%d\n",
           tinterlace->mode, (tinterlace->flags & TINTERLACE_FLAG_VLPF) ? "on" : "off",
           inlink->h, outlink->h);

    return 0;
}

#define FIELD_UPPER           0
#define FIELD_LOWER           1
#define FIELD_UPPER_AND_LOWER 2

/**
 * Copy picture field from src to dst.
 *
 * @param src_field copy from upper, lower field or both
 * @param interleave leave a padding line between each copied line
 * @param dst_field copy to upper or lower field,
 *        only meaningful when interleave is selected
 * @param flags context flags
 */
static inline
void copy_picture_field(TInterlaceContext *tinterlace,
                        uint8_t *dst[4], int dst_linesize[4],
                        const uint8_t *src[4], int src_linesize[4],
                        enum AVPixelFormat format, int w, int src_h,
                        int src_field, int interleave, int dst_field,
                        int flags)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(format);
    int hsub = desc->log2_chroma_w;
    int plane, vsub = desc->log2_chroma_h;
    int k = src_field == FIELD_UPPER_AND_LOWER ? 1 : 2;
    int h;

    for (plane = 0; plane < desc->nb_components; plane++) {
        int lines = plane == 1 || plane == 2 ? AV_CEIL_RSHIFT(src_h, vsub) : src_h;
        int cols  = plane == 1 || plane == 2 ? AV_CEIL_RSHIFT(    w, hsub) : w;
        uint8_t *dstp = dst[plane];
        const uint8_t *srcp = src[plane];

        lines = (lines + (src_field == FIELD_UPPER)) / k;
        if (src_field == FIELD_LOWER)
            srcp += src_linesize[plane];
        if (interleave && dst_field == FIELD_LOWER)
            dstp += dst_linesize[plane];
        if (flags & TINTERLACE_FLAG_VLPF) {
            // Low-pass filtering is required when creating an interlaced destination from
            // a progressive source which contains high-frequency vertical detail.
            // Filtering will reduce interlace 'twitter' and Moire patterning.
            int srcp_linesize = src_linesize[plane] * k;
            int dstp_linesize = dst_linesize[plane] * (interleave ? 2 : 1);
            for (h = lines; h > 0; h--) {
                const uint8_t *srcp_above = srcp - src_linesize[plane];
                const uint8_t *srcp_below = srcp + src_linesize[plane];
                if (h == lines) srcp_above = srcp; // there is no line above
                if (h == 1) srcp_below = srcp;     // there is no line below

                tinterlace->lowpass_line(dstp, cols, srcp, srcp_above, srcp_below);
                dstp += dstp_linesize;
                srcp += srcp_linesize;
            }
        } else {
            av_image_copy_plane(dstp, dst_linesize[plane] * (interleave ? 2 : 1),
                                srcp, src_linesize[plane]*k, cols, lines);
        }
    }
}

static int filter_frame(AVFilterLink *inlink, AVFrame *picref)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    TInterlaceContext *tinterlace = ctx->priv;
    AVFrame *cur, *next, *out;
    int field, tff, ret;

    av_frame_free(&tinterlace->cur);
    tinterlace->cur  = tinterlace->next;
    tinterlace->next = picref;

    cur = tinterlace->cur;
    next = tinterlace->next;
    /* we need at least two frames */
    if (!tinterlace->cur)
        return 0;

    switch (tinterlace->mode) {
    case MODE_MERGEX2: /* move the odd frame into the upper field of the new image, even into
                        * the lower field, generating a double-height video at same framerate */
    case MODE_MERGE: /* move the odd frame into the upper field of the new image, even into
             * the lower field, generating a double-height video at half framerate */
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out, cur);
        out->height = outlink->h;
        out->interlaced_frame = 1;
        out->top_field_first = 1;
        out->sample_aspect_ratio = av_mul_q(cur->sample_aspect_ratio, av_make_q(2, 1));

        /* write odd frame lines into the upper field of the new frame */
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)cur->data, cur->linesize,
                           inlink->format, inlink->w, inlink->h,
                           FIELD_UPPER_AND_LOWER, 1, tinterlace->mode == MODE_MERGEX2 ? inlink->frame_count & 1 ? FIELD_LOWER : FIELD_UPPER : FIELD_UPPER, tinterlace->flags);
        /* write even frame lines into the lower field of the new frame */
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)next->data, next->linesize,
                           inlink->format, inlink->w, inlink->h,
                           FIELD_UPPER_AND_LOWER, 1, tinterlace->mode == MODE_MERGEX2 ? inlink->frame_count & 1 ? FIELD_UPPER : FIELD_LOWER : FIELD_LOWER, tinterlace->flags);
        if (tinterlace->mode != MODE_MERGEX2)
            av_frame_free(&tinterlace->next);
        break;

    case MODE_DROP_ODD:  /* only output even frames, odd  frames are dropped; height unchanged, half framerate */
    case MODE_DROP_EVEN: /* only output odd  frames, even frames are dropped; height unchanged, half framerate */
        out = av_frame_clone(tinterlace->mode == MODE_DROP_EVEN ? cur : next);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_free(&tinterlace->next);
        break;

    case MODE_PAD: /* expand each frame to double height, but pad alternate
                    * lines with black; framerate unchanged */
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out, cur);
        out->height = outlink->h;
        out->sample_aspect_ratio = av_mul_q(cur->sample_aspect_ratio, av_make_q(2, 1));

        field = (1 + tinterlace->frame) & 1 ? FIELD_UPPER : FIELD_LOWER;
        /* copy upper and lower fields */
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)cur->data, cur->linesize,
                           inlink->format, inlink->w, inlink->h,
                           FIELD_UPPER_AND_LOWER, 1, field, tinterlace->flags);
        /* pad with black the other field */
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)tinterlace->black_data, tinterlace->black_linesize,
                           inlink->format, inlink->w, inlink->h,
                           FIELD_UPPER_AND_LOWER, 1, !field, tinterlace->flags);
        break;

        /* interleave upper/lower lines from odd frames with lower/upper lines from even frames,
         * halving the frame rate and preserving image height */
    case MODE_INTERLEAVE_TOP:    /* top    field first */
    case MODE_INTERLEAVE_BOTTOM: /* bottom field first */
        tff = tinterlace->mode == MODE_INTERLEAVE_TOP;
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out, cur);
        out->interlaced_frame = 1;
        out->top_field_first = tff;

        /* copy upper/lower field from cur */
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)cur->data, cur->linesize,
                           inlink->format, inlink->w, inlink->h,
                           tff ? FIELD_UPPER : FIELD_LOWER, 1, tff ? FIELD_UPPER : FIELD_LOWER,
                           tinterlace->flags);
        /* copy lower/upper field from next */
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)next->data, next->linesize,
                           inlink->format, inlink->w, inlink->h,
                           tff ? FIELD_LOWER : FIELD_UPPER, 1, tff ? FIELD_LOWER : FIELD_UPPER,
                           tinterlace->flags);
        av_frame_free(&tinterlace->next);
        break;
    case MODE_INTERLACEX2: /* re-interlace preserving image height, double frame rate */
        /* output current frame first */
        out = av_frame_clone(cur);
        if (!out)
            return AVERROR(ENOMEM);
        out->interlaced_frame = 1;
        if (cur->pts != AV_NOPTS_VALUE)
            out->pts = cur->pts*2;

        out->pts = av_rescale_q(out->pts, tinterlace->preout_time_base, outlink->time_base);
        if ((ret = ff_filter_frame(outlink, out)) < 0)
            return ret;

        /* output mix of current and next frame */
        tff = next->top_field_first;
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out)
            return AVERROR(ENOMEM);
        av_frame_copy_props(out, next);
        out->interlaced_frame = 1;
        out->top_field_first = !tff;

        if (next->pts != AV_NOPTS_VALUE && cur->pts != AV_NOPTS_VALUE)
            out->pts = cur->pts + next->pts;
        else
            out->pts = AV_NOPTS_VALUE;
        /* write current frame second field lines into the second field of the new frame */
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)cur->data, cur->linesize,
                           inlink->format, inlink->w, inlink->h,
                           tff ? FIELD_LOWER : FIELD_UPPER, 1, tff ? FIELD_LOWER : FIELD_UPPER,
                           tinterlace->flags);
        /* write next frame first field lines into the first field of the new frame */
        copy_picture_field(tinterlace, out->data, out->linesize,
                           (const uint8_t **)next->data, next->linesize,
                           inlink->format, inlink->w, inlink->h,
                           tff ? FIELD_UPPER : FIELD_LOWER, 1, tff ? FIELD_UPPER : FIELD_LOWER,
                           tinterlace->flags);
        break;
    default:
        av_assert0(0);
    }

    out->pts = av_rescale_q(out->pts, tinterlace->preout_time_base, outlink->time_base);
    ret = ff_filter_frame(outlink, out);
    tinterlace->frame++;

    return ret;
}

static const AVFilterPad tinterlace_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .filter_frame = filter_frame,
    },
    { NULL }
};

static const AVFilterPad tinterlace_outputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = config_out_props,
    },
    { NULL }
};

AVFilter ff_vf_tinterlace = {
    .name          = "tinterlace",
    .description   = NULL_IF_CONFIG_SMALL("Perform temporal field interlacing."),
    .priv_size     = sizeof(TInterlaceContext),
    .uninit        = uninit,
    .query_formats = query_formats,
    .inputs        = tinterlace_inputs,
    .outputs       = tinterlace_outputs,
    .priv_class    = &tinterlace_class,
};
