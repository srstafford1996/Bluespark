/*
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

#include "float.h"

#include "libavutil/eval.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/opt.h"
#include "avfilter.h"
#include "formats.h"
#include "internal.h"
#include "video.h"

typedef struct DrawGraphContext {
    const AVClass *class;

    char          *key[4];
    float         min, max;
    char          *fg_str[4];
    AVExpr        *fg_expr[4];
    uint8_t       bg[4];
    int           mode;
    int           slide;
    int           w, h;

    AVFrame       *out;
    int           x;
    int           prev_y[4];
    int           first;
} DrawGraphContext;

#define OFFSET(x) offsetof(DrawGraphContext, x)
#define FLAGS AV_OPT_FLAG_VIDEO_PARAM|AV_OPT_FLAG_FILTERING_PARAM

static const AVOption drawgraph_options[] = {
    { "m1", "set 1st metadata key", OFFSET(key[0]), AV_OPT_TYPE_STRING, {.str=""}, CHAR_MIN, CHAR_MAX, FLAGS },
    { "fg1", "set 1st foreground color expression", OFFSET(fg_str[0]), AV_OPT_TYPE_STRING, {.str="0xffff0000"}, CHAR_MIN, CHAR_MAX, FLAGS },
    { "m2", "set 2nd metadata key", OFFSET(key[1]), AV_OPT_TYPE_STRING, {.str=""}, CHAR_MIN, CHAR_MAX, FLAGS },
    { "fg2", "set 2nd foreground color expression", OFFSET(fg_str[1]), AV_OPT_TYPE_STRING, {.str="0xff00ff00"}, CHAR_MIN, CHAR_MAX, FLAGS },
    { "m3", "set 3rd metadata key", OFFSET(key[2]), AV_OPT_TYPE_STRING, {.str=""}, CHAR_MIN, CHAR_MAX, FLAGS },
    { "fg3", "set 3rd foreground color expression", OFFSET(fg_str[2]), AV_OPT_TYPE_STRING, {.str="0xffff00ff"}, CHAR_MIN, CHAR_MAX, FLAGS },
    { "m4", "set 4th metadata key", OFFSET(key[3]), AV_OPT_TYPE_STRING, {.str=""}, CHAR_MIN, CHAR_MAX, FLAGS },
    { "fg4", "set 4th foreground color expression", OFFSET(fg_str[3]), AV_OPT_TYPE_STRING, {.str="0xffffff00"}, CHAR_MIN, CHAR_MAX, FLAGS },
    { "bg", "set background color", OFFSET(bg), AV_OPT_TYPE_COLOR, {.str="white"}, CHAR_MIN, CHAR_MAX, FLAGS },
    { "min", "set minimal value", OFFSET(min), AV_OPT_TYPE_FLOAT, {.dbl=-1.}, INT_MIN, INT_MAX, FLAGS },
    { "max", "set maximal value", OFFSET(max), AV_OPT_TYPE_FLOAT, {.dbl=1.}, INT_MIN, INT_MAX, FLAGS },
    { "mode", "set graph mode", OFFSET(mode), AV_OPT_TYPE_INT, {.i64=2}, 0, 2, FLAGS, "mode" },
        {"bar", "draw bars", OFFSET(mode), AV_OPT_TYPE_CONST, {.i64=0}, 0, 0, FLAGS, "mode"},
        {"dot", "draw dots", OFFSET(mode), AV_OPT_TYPE_CONST, {.i64=1}, 0, 0, FLAGS, "mode"},
        {"line", "draw lines", OFFSET(mode), AV_OPT_TYPE_CONST, {.i64=2}, 0, 0, FLAGS, "mode"},
    { "slide", "set slide mode", OFFSET(slide), AV_OPT_TYPE_INT, {.i64=0}, 0, 3, FLAGS, "slide" },
        {"frame", "draw new frames", OFFSET(slide), AV_OPT_TYPE_CONST, {.i64=0}, 0, 0, FLAGS, "slide"},
        {"replace", "replace old columns with new", OFFSET(slide), AV_OPT_TYPE_CONST, {.i64=1}, 0, 0, FLAGS, "slide"},
        {"scroll", "scroll from right to left", OFFSET(slide), AV_OPT_TYPE_CONST, {.i64=2}, 0, 0, FLAGS, "slide"},
        {"rscroll", "scroll from left to right", OFFSET(slide), AV_OPT_TYPE_CONST, {.i64=3}, 0, 0, FLAGS, "slide"},
    { "size", "set graph size", OFFSET(w), AV_OPT_TYPE_IMAGE_SIZE, {.str="900x256"}, 0, 0, FLAGS },
    { "s", "set graph size", OFFSET(w), AV_OPT_TYPE_IMAGE_SIZE, {.str="900x256"}, 0, 0, FLAGS },
    { NULL }
};

static const char *const var_names[] = {   "MAX",   "MIN",   "VAL", NULL };
enum                                   { VAR_MAX, VAR_MIN, VAR_VAL, VAR_VARS_NB };

static av_cold int init(AVFilterContext *ctx)
{
    DrawGraphContext *s = ctx->priv;
    int ret, i;

    if (s->max <= s->min) {
        av_log(ctx, AV_LOG_ERROR, "max is same or lower than min\n");
        return AVERROR(EINVAL);
    }

    for (i = 0; i < 4; i++) {
        if (s->fg_str[i]) {
            ret = av_expr_parse(&s->fg_expr[i], s->fg_str[i], var_names,
                                NULL, NULL, NULL, NULL, 0, ctx);

            if (ret < 0)
                return ret;
        }
    }

    s->first = 1;

    return 0;
}

static int query_formats(AVFilterContext *ctx)
{
    AVFilterLink *outlink = ctx->outputs[0];
    static const enum AVPixelFormat pix_fmts[] = {
        AV_PIX_FMT_RGBA,
        AV_PIX_FMT_NONE
    };
    int ret;

    AVFilterFormats *fmts_list = ff_make_format_list(pix_fmts);
    if ((ret = ff_formats_ref(fmts_list, &outlink->in_formats)) < 0)
        return ret;

    return 0;
}

static void clear_image(DrawGraphContext *s, AVFrame *out, AVFilterLink *outlink)
{
    int i, j;
    int bg = AV_RN32(s->bg);

    for (i = 0; i < out->height; i++)
        for (j = 0; j < out->width; j++)
            AV_WN32(out->data[0] + i * out->linesize[0] + j * 4, bg);
}

static inline void draw_dot(int fg, int x, int y, AVFrame *out)
{
    AV_WN32(out->data[0] + y * out->linesize[0] + x * 4, fg);
}

static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    DrawGraphContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVDictionary *metadata;
    AVDictionaryEntry *e;
    AVFrame *out = s->out;
    int i;

    if (!s->out || s->out->width  != outlink->w ||
                   s->out->height != outlink->h) {
        av_frame_free(&s->out);
        s->out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        out = s->out;
        if (!s->out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }

        clear_image(s, out, outlink);
    }
    av_frame_copy_props(out, in);

    metadata = av_frame_get_metadata(in);

    for (i = 0; i < 4; i++) {
        double values[VAR_VARS_NB];
        int j, y, x, old;
        uint32_t fg, bg;
        float vf;

        e = av_dict_get(metadata, s->key[i], NULL, 0);
        if (!e || !e->value)
            continue;

        if (sscanf(e->value, "%f", &vf) != 1)
            continue;

        vf = av_clipf(vf, s->min, s->max);

        values[VAR_MIN] = s->min;
        values[VAR_MAX] = s->max;
        values[VAR_VAL] = vf;

        fg = av_expr_eval(s->fg_expr[i], values, NULL);
        bg = AV_RN32(s->bg);

        if (i == 0 && (s->x >= outlink->w || s->slide == 3)) {
            if (s->slide == 0 || s->slide == 1)
                s->x = 0;

            if (s->slide == 2) {
                s->x = outlink->w - 1;
                for (j = 0; j < outlink->h; j++) {
                    memmove(out->data[0] + j * out->linesize[0] ,
                            out->data[0] + j * out->linesize[0] + 4,
                            (outlink->w - 1) * 4);
                }
            } else if (s->slide == 3) {
                s->x = 0;
                for (j = 0; j < outlink->h; j++) {
                    memmove(out->data[0] + j * out->linesize[0] + 4,
                            out->data[0] + j * out->linesize[0],
                            (outlink->w - 1) * 4);
                }
            } else if (s->slide == 0) {
                clear_image(s, out, outlink);
            }
        }

        x = s->x;
        y = (outlink->h - 1) * (1 - ((vf - s->min) / (s->max - s->min)));

        switch (s->mode) {
        case 0:
            if (i == 0 && (s->slide > 0))
                for (j = 0; j < outlink->h; j++)
                    draw_dot(bg, x, j, out);

            old = AV_RN32(out->data[0] + y * out->linesize[0] + x * 4);
            for (j = y; j < outlink->h; j++) {
                if (old != bg &&
                    (AV_RN32(out->data[0] + j * out->linesize[0] + x * 4) != old) ||
                    AV_RN32(out->data[0] + FFMIN(j+1, outlink->h - 1) * out->linesize[0] + x * 4) != old) {
                    draw_dot(fg, x, j, out);
                    break;
                }
                draw_dot(fg, x, j, out);
            }
            break;
        case 1:
            if (i == 0 && (s->slide > 0))
                for (j = 0; j < outlink->h; j++)
                    draw_dot(bg, x, j, out);
            draw_dot(fg, x, y, out);
            break;
        case 2:
            if (s->first) {
                s->first = 0;
                s->prev_y[i] = y;
            }

            if (i == 0 && (s->slide > 0)) {
                for (j = 0; j < y; j++)
                    draw_dot(bg, x, j, out);
                for (j = outlink->h - 1; j > y; j--)
                    draw_dot(bg, x, j, out);
            }
            if (y <= s->prev_y[i]) {
                for (j = y; j <= s->prev_y[i]; j++)
                    draw_dot(fg, x, j, out);
            } else {
                for (j = s->prev_y[i]; j <= y; j++)
                    draw_dot(fg, x, j, out);
            }
            s->prev_y[i] = y;
            break;
        }
    }

    s->x++;

    av_frame_free(&in);
    return ff_filter_frame(outlink, av_frame_clone(s->out));
}

static int config_output(AVFilterLink *outlink)
{
    DrawGraphContext *s = outlink->src->priv;

    outlink->w = s->w;
    outlink->h = s->h;
    outlink->sample_aspect_ratio = (AVRational){1,1};

    return 0;
}

static av_cold void uninit(AVFilterContext *ctx)
{
    DrawGraphContext *s = ctx->priv;
    int i;

    for (i = 0; i < 4; i++)
        av_expr_free(s->fg_expr[i]);
    av_frame_free(&s->out);
}

#if CONFIG_DRAWGRAPH_FILTER

AVFILTER_DEFINE_CLASS(drawgraph);

static const AVFilterPad drawgraph_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .filter_frame = filter_frame,
    },
    { NULL }
};

static const AVFilterPad drawgraph_outputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = config_output,
    },
    { NULL }
};

AVFilter ff_vf_drawgraph = {
    .name          = "drawgraph",
    .description   = NULL_IF_CONFIG_SMALL("Draw a graph using input video metadata."),
    .priv_size     = sizeof(DrawGraphContext),
    .priv_class    = &drawgraph_class,
    .query_formats = query_formats,
    .init          = init,
    .uninit        = uninit,
    .inputs        = drawgraph_inputs,
    .outputs       = drawgraph_outputs,
};

#endif // CONFIG_DRAWGRAPH_FILTER

#if CONFIG_ADRAWGRAPH_FILTER

#define adrawgraph_options drawgraph_options
AVFILTER_DEFINE_CLASS(adrawgraph);

static const AVFilterPad adrawgraph_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_AUDIO,
        .filter_frame = filter_frame,
    },
    { NULL }
};

static const AVFilterPad adrawgraph_outputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = config_output,
    },
    { NULL }
};

AVFilter ff_avf_adrawgraph = {
    .name          = "adrawgraph",
    .description   = NULL_IF_CONFIG_SMALL("Draw a graph using input audio metadata."),
    .priv_size     = sizeof(DrawGraphContext),
    .priv_class    = &adrawgraph_class,
    .query_formats = query_formats,
    .init          = init,
    .uninit        = uninit,
    .inputs        = adrawgraph_inputs,
    .outputs       = adrawgraph_outputs,
};
#endif // CONFIG_ADRAWGRAPH_FILTER
