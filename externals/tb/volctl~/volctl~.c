/* Copyright (c) 2004 Tim Blechmann.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "gpl.txt" in this distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * See file LICENSE for further informations on licensing terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Based on PureData by Miller Puckette and others.
 *
 * coded while listening to: Julien Ottavi: Nervure Magnetique
 *                                                                    */


#include "m_pd.h"
#include "m_simd.h"


/* ----------------------------- volctl ----------------------------- */

static t_class *volctl_class;

typedef struct _volctl
{
    t_object x_obj;
    t_float x_f;

    t_float x_h; //interpolation time
    t_float x_value; //current factor
    t_float x_target; //target factor
    
    int x_ticksleft; //dsp ticks to go
    t_float x_samples_per_ms; //ms per sample
    t_float x_slope; //slope
	t_float * x_slopes; //slopes for simd
	t_float x_slope_step;
    int x_line; 
	int x_blocksize;
	t_float x_1overblocksize;
} t_volctl;

void *volctl_new(t_symbol *s, int argc, t_atom *argv)
{
    if (argc > 3) post("volctl~: extra arguments ignored");

    t_volctl *x = (t_volctl *)pd_new(volctl_class);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("f1"));
    inlet_settip(x->x_obj.ob_inlet,gensym("factor"));
    x->x_value = atom_getfloatarg(0, argc, argv);
    
    t_inlet * time = floatinlet_new(&x->x_obj, &x->x_h);
    inlet_settip(time,gensym("interpolation_time"));
    x->x_h = atom_getfloatarg(1, argc, argv);

    x->x_samples_per_ms = 44100.f / 1000.f; // assume default samplerate
	x->x_blocksize = 64;
	x->x_1overblocksize = 1.f/64.f;

    outlet_new(&x->x_obj, &s_signal);
    x->x_f = 0;
	
	x->x_slopes = getalignedbytes(4*sizeof(t_float));

    return (x);
}

static void volctl_free(t_volctl *x)
{
	freealignedbytes(x->x_slopes, 4*sizeof(t_float));
}

static t_int *volctl_perform(t_int *w)
{
    t_volctl * x = (t_volctl *)(w[1]);
    t_float *in = (t_float *)(w[2]);
    t_float *out = (t_float *)(w[3]);
    int n = (int)(w[4]);
    

    if (x->x_ticksleft)
    {
		t_float f = x->x_value;
		t_float x_slope = x->x_slope;
		
		x->x_ticksleft--;
		while (n--)
		{
			f+=x_slope;
			*out++ = *in++ * f;
		}
		x->x_value = f;
    }
    else
	{
		t_float f = x->x_target;
		while (n--) *out++ = *in++ * f; 
	}
	
    return (w+5);
}
    

static t_int *volctl_perf8(t_int *w)
{
    t_volctl * x = (t_volctl *)(w[1]);
    t_float *in = (t_float *)(w[2]);
    t_float *out = (t_float *)(w[3]);
    int n = (int)(w[4]);

    if (x->x_ticksleft)
    {
		t_float f = x->x_value;

		t_float x_slope = x->x_slope;
		x->x_ticksleft--;
		n = n>>3;
		while (n--)
		{
			*out++ = *in++ * f;
			f+=x_slope;
			*out++ = *in++ * f;
			f+=x_slope;
			*out++ = *in++ * f;
			f+=x_slope;
			*out++ = *in++ * f;
			f+=x_slope;
			*out++ = *in++ * f;
			f+=x_slope;
			*out++ = *in++ * f;
			f+=x_slope;
			*out++ = *in++ * f;
			f+=x_slope;
			*out++ = *in++ * f;
			f+=x_slope;
		}
		x->x_value = f;
    }
    else
    {
		t_float f = x->x_target;

		if (f)
			for (; n; n -= 8, in += 8, out += 8)
			{
				t_float f0 = in[0], f1 = in[1], f2 = in[2], f3 = in[3];
				t_float f4 = in[4], f5 = in[5], f6 = in[6], f7 = in[7];
				
				out[0] = f0 * f; out[1] = f1 * f; out[2] = f2 * f; out[3] = f3 * f;
				out[4] = f4 * f; out[5] = f5 * f; out[6] = f6 * f; out[7] = f7 * f;
			}
		else
			for (; n; n -= 8, in += 8, out += 8)
			{
				out[0] = 0; out[1] = 0; out[2] = 0; out[3] = 0;
				out[4] = 0; out[5] = 0; out[6] = 0; out[7] = 0;
			}

    }
    return (w+5);
}


static t_int *volctl_perf_simd(t_int *w)
{
    t_volctl * x = (t_volctl *)(w[1]);
    t_float *in = (t_float *)(w[2]);
    t_float *out = (t_float *)(w[3]);

    if (x->x_ticksleft)
    {
		int n = (int)(w[4]);
	
		x->x_ticksleft--;
		
		asm(
			".set T_FLOAT,4                          \n"
			"movss     (%3),%%xmm0                   \n" /* value */
			"shufps    $0, %%xmm0, %%xmm0            \n"
			"movaps    (%4), %%xmm1                  \n" /* x_slopes */
			"addps     %%xmm0, %%xmm1                \n"
			
			"movss     (%5), %%xmm0                  \n"
			"shufps    $0, %%xmm0, %%xmm0            \n" /* x_slope_step */
			
			"shrl      $4, %2                        \n" /* n>>4 */
			
			"1:                                      \n"
			"movaps    (%0), %%xmm2                  \n"
			"mulps     %%xmm1, %%xmm2                \n"
			"movaps    %%xmm2, (%1)                  \n"
			"addps     %%xmm0, %%xmm1                \n"
			
			"movaps    4*T_FLOAT(%0), %%xmm2         \n"
			"mulps     %%xmm1, %%xmm2                \n"
			"movaps    %%xmm2, 4*T_FLOAT(%1)         \n"
			"addps     %%xmm0, %%xmm1                \n"
			
			"movaps    8*T_FLOAT(%0), %%xmm2         \n"
			"mulps     %%xmm1, %%xmm2                \n"
			"movaps    %%xmm2, 8*T_FLOAT(%1)         \n"
			"addps     %%xmm0, %%xmm1                \n"
			
			"movaps    12*T_FLOAT(%0), %%xmm2        \n"
			"mulps     %%xmm1, %%xmm2                \n"
			"movaps    %%xmm2, 12*T_FLOAT(%1)        \n"
			"addps     %%xmm0, %%xmm1                \n" /* one instr. obsolete */
			
			"addl      $16*T_FLOAT, %0               \n"
			"addl      $16*T_FLOAT, %1               \n"
			"loop      1b                            \n"
			
			:
			:"r"(in), "r"(out), "c"(n), "r"(&(t_float)(x->x_value)),
			"r"((t_float*)x->x_slopes), "r"(&(t_float)(x->x_slope_step))
			:"%xmm0", "%xmm1", "%xmm2");
		
		x->x_value += n*(x->x_slope);
	}
    else
    {
		switch(x->x_target)
		{
		case 0:
			asm(
				".set T_FLOAT,4                          \n"
			
				"xorps     %%xmm0, %%xmm0                \n"
				"shrl      $4, %1                        \n"
			
				"1:                                      \n"
				"movaps    %%xmm0, (%0)                  \n" 
				"movaps    %%xmm0, 4*T_FLOAT(%0)         \n"
				"movaps    %%xmm0, 8*T_FLOAT(%0)         \n"
				"movaps    %%xmm0, 12*T_FLOAT(%0)        \n"
				"addl      $16*T_FLOAT, %0               \n"
				"loop      1b                            \n"
				:
				:"r"(out),
				"c"(w[4])
				: "%xmm0");
			break;

		case 1:
			asm(
				".set T_FLOAT,4                          \n"
			
				"shrl      $4, %1                        \n"

				"1:                                        \n"
				"movaps    (%1), %%xmm0                    \n"
				"movaps    4*T_FLOAT(%1), %%xmm1           \n"
				"movaps    8*T_FLOAT(%1), %%xmm2           \n"
				"movaps    12*T_FLOAT(%1), %%xmm3          \n"
				"movaps    %%xmm0, (%2)                    \n"
				"movaps    %%xmm1, 4*T_FLOAT(%2)           \n"
				"movaps    %%xmm2, 8*T_FLOAT(%2)           \n"
				"movaps    %%xmm3, 12*T_FLOAT(%2)          \n"
				
				"addl      $16*T_FLOAT,%1                  \n"
				"addl      $16*T_FLOAT,%2                  \n"
				"loop      1b                              \n"
				:
				:"c"(w[4]),"r"(in),"r"(out)
				:"%xmm0","%xmm1","%xmm2","%xmm3");
			break;

		default:
			asm(
				".set T_FLOAT,4                          \n"
			
				"movss     (%3), %%xmm0                  \n"
				"shufps    $0, %%xmm0, %%xmm0            \n"
				"shrl      $4, %2                        \n"
			
				"1:                                      \n"
				"movaps    (%0), %%xmm1                  \n"
				"mulps     %%xmm0, %%xmm1                \n"
				"movaps    %%xmm1, (%1)                  \n" 
				"movaps    4*T_FLOAT(%0), %%xmm2         \n"
				"mulps     %%xmm0, %%xmm2                \n"
				"movaps    %%xmm2, 4*T_FLOAT(%1)         \n"
				"movaps    8*T_FLOAT(%0), %%xmm3         \n"
				"mulps     %%xmm0, %%xmm3                \n"
				"movaps    %%xmm3, 8*T_FLOAT(%1)         \n"
				"movaps    12*T_FLOAT(%0), %%xmm4        \n"
				"mulps     %%xmm0, %%xmm4                \n"
				"movaps    %%xmm4, 12*T_FLOAT(%1)        \n"
				"addl      $16*T_FLOAT, %0               \n"
				"addl      $16*T_FLOAT, %1               \n"
				"loop      1b                            \n"
				:
				: "r"(in), "r"(out),
				"c"(w[4]),"r"(&(t_float)(x->x_target))
				: "%xmm0", "%xmm1","%xmm2","%xmm3","%xmm4");
		}
    }
    return (w+5);
}


static void volctl_set(t_volctl *x, t_float f)
{
	t_float slope;
	int i;
	int samplesleft = x->x_h * x->x_samples_per_ms;
	samplesleft += x->x_blocksize - ( samplesleft & (x->x_blocksize - 1));
	x->x_ticksleft = (int) (t_float)samplesleft * x->x_1overblocksize;

    slope = (f - x->x_value) / samplesleft;
    x->x_slope = slope;
	
	for (i = 0; i != 4; ++i)
	{
		x->x_slopes[i] = i*slope;
	}
	x->x_slope_step = 4*slope;

	x->x_target = f;
}

static void volctl_dsp(t_volctl *x, t_signal **sp)
{
    const int n = sp[0]->s_n;
    if (n&7)
    	dsp_add(volctl_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, n);
    else 
    {
		if(SIMD_CHECK2(n,sp[0]->s_vec,sp[1]->s_vec))
			dsp_add(volctl_perf_simd, 4, x, sp[0]->s_vec, sp[1]->s_vec, n);
		else
			dsp_add(volctl_perf8, 4, x, sp[0]->s_vec, sp[1]->s_vec, n);
    }

	x->x_blocksize = n;
    x->x_1overblocksize = 1./n;
	x->x_samples_per_ms = sp[0]->s_sr / 1000.f;
}

void volctl_tilde_setup(void)
{
    volctl_class = class_new(gensym("volctl~"), (t_newmethod)volctl_new, 
							 (t_method)volctl_free, sizeof(t_volctl), 0, A_GIMME, 0);
    CLASS_MAINSIGNALIN(volctl_class, t_volctl, x_f);
    class_addmethod(volctl_class, (t_method)volctl_dsp, gensym("dsp"), 0);
    class_addmethod(volctl_class, (t_method)volctl_set, gensym("f1"),A_FLOAT,0);
    class_settip(volctl_class,gensym("signal"));
}
