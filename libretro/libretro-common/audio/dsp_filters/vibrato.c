/* Copyright  (C) 2010-2020 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (vibrato.c).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <retro_miscellaneous.h>
#include <libretro_dspfilter.h>
#include <string/stdstring.h>

#define sqr(a) ((a) * (a))

const float BASE_DELAY_SEC = 0.002; // 2 ms
const float VIBRATO_FREQUENCY_DEFAULT_HZ = 2;
const float VIBRATO_FREQUENCY_MAX_HZ = 14;
const float VIBRATO_DEPTH_DEFAULT_PERCENT = 50;
const int add_delay = 3;

float hermite_interp(float x, float *y)
{
	float c0, c1, c2, c3;
	c0 = y[1];
	c1 = (1.0 / 2.0)*(y[2] - y[0]);
	c2 = (y[0] - (5.0 / 2.0)*y[1]) + (2.0*y[2] - (1.0 / 2.0)*y[3]);
	c3 = (1.0 / 2.0)*(y[3] - y[0]) + (3.0 / 2.0)*(y[1] - y[2]);
	return ((c3*x + c2)*x + c1)*x + c0;
}

struct vibrato_core
{
      float freq;
      float samplerate;
      int phase;
      float depth;
	float* buffer;
	int writeindex;
      int size;
};

struct vibrato
{
   struct vibrato_core left, right;
};

static void vibrato_free(void *data)
{
   struct vibrato *vib = (struct vibrato*)data;
   free(vib->left.buffer);
   free(vib->right.buffer);
   free(data);
}

static void vibratocore_init(struct vibrato_core *core,float depth,int samplerate,float freq)
{
	core->size = BASE_DELAY_SEC * samplerate * 2;
	core->buffer = malloc((core->size + add_delay)*sizeof(float));
	memset(core->buffer, 0, (core->size + add_delay) * sizeof(float));
	core->samplerate = samplerate;
	core->freq = freq;
	core->depth = depth;
	core->phase = 0;
	core->writeindex = 0;
}

float vibratocore_core(struct vibrato_core *core,float in)
{
            float M = core->freq / core->samplerate;
		int maxphase = core->samplerate / core->freq;
		float lfo = sin(M * 2. * M_PI * core->phase++);
		core->phase = core->phase % maxphase;
	      lfo = (lfo + 1) * 1.; // transform from [-1; 1] to [0; 1]
		int maxdelay = BASE_DELAY_SEC * core->samplerate;
		float delay =  lfo * core->depth * maxdelay;
		delay += add_delay;
		float readindex = core->writeindex - 1 - delay;
		while (readindex < 0)readindex += core->size;
		while (readindex >= core->size)readindex -= core->size;
		int ipart = (int)readindex; // integer part of the delay
		float fpart = readindex - ipart; // fractional part of the delay
		float value = hermite_interp(fpart, &(core->buffer[ipart]));
		core->buffer[core->writeindex] = in;
		if (core->writeindex < add_delay){
			core->buffer[core->size + core->writeindex] = in;
		}
		core->writeindex++;
		if (core->writeindex == core->size) {
			core->writeindex = 0;
		}
            return value;
}

static void vibrato_process(void *data, struct dspfilter_output *output,
      const struct dspfilter_input *input)
{
   unsigned i;
   float *out;
   struct vibrato *vib = (struct vibrato*)data;

   output->samples         = input->samples;
   output->frames          = input->frames;
   out                     = output->samples;

   for (i = 0; i < input->frames; i++, out += 2)
   {
      float in[2] = { out[0], out[1] };

      out[0] = vibratocore_core(&vib->left, in[0]);
      out[1] = vibratocore_core(&vib->right, in[1]);
   }
}

static void *vibrato_init(const struct dspfilter_info *info,
      const struct dspfilter_config *config, void *userdata)
{
   float freq, depth;
   struct vibrato *vib   = (struct vibrato*)calloc(1, sizeof(*vib));
   if (!vib)
      return NULL;

   config->get_float(userdata, "freq", &freq,5.0f);
   config->get_float(userdata, "depth", &depth, 0.5f);
   vibratocore_init(&vib->left,depth,info->input_rate,freq);
   vibratocore_init(&vib->right,depth,info->input_rate,freq);
   return vib;
}

static const struct dspfilter_implementation vibrato_plug = {
   vibrato_init,
   vibrato_process,
   vibrato_free,

   DSPFILTER_API_VERSION,
   "Vibrato",
   "vibrato",
};

#ifdef HAVE_FILTERS_BUILTIN
#define dspfilter_get_implementation vibrato_dspfilter_get_implementation
#endif

const struct dspfilter_implementation *dspfilter_get_implementation(dspfilter_simd_mask_t mask)
{
   (void)mask;
   return &vibrato_plug;
}

#undef dspfilter_get_implementation
