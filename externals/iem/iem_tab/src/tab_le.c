/* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.

iem_tab written by Thomas Musil, Copyright (c) IEM KUG Graz Austria 2000 - 2006 */

#include "m_pd.h"
#include "iemlib.h"
#include "iem_tab.h"

/* -------------------------- tab_le ------------------------------ */

typedef struct _tab_le
{
  t_object  x_obj;
  int       x_size_src1;
  int       x_size_src2;
  int       x_size_dst;
  int       x_offset_src1;
  int       x_offset_src2;
  int       x_offset_dst;
  t_float   *x_beg_mem_src1;
  t_float   *x_beg_mem_src2;
  t_float   *x_beg_mem_dst;
  t_symbol  *x_sym_scr1;
  t_symbol  *x_sym_scr2;
  t_symbol  *x_sym_dst;
} t_tab_le;

static t_class *tab_le_class;

static void tab_le_src1(t_tab_le *x, t_symbol *s)
{
  x->x_sym_scr1 = s;
}

static void tab_le_src2(t_tab_le *x, t_symbol *s)
{
  x->x_sym_scr2 = s;
}

static void tab_le_dst(t_tab_le *x, t_symbol *s)
{
  x->x_sym_dst = s;
}

static void tab_le_bang(t_tab_le *x)
{
  int i, n;
  int ok_src1, ok_src2, ok_dst;
  t_float *vec_src1, *vec_src2, *vec_dst;
  
  ok_src1 = iem_tab_check_arrays(gensym("tab_le"), x->x_sym_scr1, &x->x_beg_mem_src1, &x->x_size_src1, 0);
  ok_src2 = iem_tab_check_arrays(gensym("tab_le"), x->x_sym_scr2, &x->x_beg_mem_src2, &x->x_size_src2, 0);
  ok_dst = iem_tab_check_arrays(gensym("tab_le"), x->x_sym_dst, &x->x_beg_mem_dst, &x->x_size_dst, 0);
  
  if(ok_src1 && ok_src2 && ok_dst)
  {
    if(x->x_size_src1 < x->x_size_dst)
      n = x->x_size_src1;
    else
      n = x->x_size_dst;
    if(x->x_size_src2 < n)
      n = x->x_size_src2;
    
    vec_src1 = x->x_beg_mem_src1;
    vec_src2 = x->x_beg_mem_src2;
    vec_dst = x->x_beg_mem_dst;
    if(n)
    {
      t_garray *a;
      
      for(i=0; i<n; i++)
      {
        if(vec_src1[i] <= vec_src2[i])
          vec_dst[i] = 1.0f;
        else
          vec_dst[i] = 0.0f;
      }
      outlet_bang(x->x_obj.ob_outlet);
      a = (t_garray *)pd_findbyclass(x->x_sym_dst, garray_class);
      garray_redraw(a);
    }
  }
}

static void tab_le_list(t_tab_le *x, t_symbol *s, int argc, t_atom *argv)
{
  int beg_src1, beg_src2, beg_dst;
  int i, n;
  int ok_src1, ok_src2, ok_dst;
  t_float *vec_src1, *vec_src2, *vec_dst;
  
  if((argc >= 4) &&
    IS_A_FLOAT(argv,0) &&
    IS_A_FLOAT(argv,1) &&
    IS_A_FLOAT(argv,2) &&
    IS_A_FLOAT(argv,3))
  {
    beg_src1 = (int)atom_getintarg(0, argc, argv);
    beg_src2 = (int)atom_getintarg(1, argc, argv);
    beg_dst = (int)atom_getintarg(2, argc, argv);
    n = (int)atom_getintarg(3, argc, argv);
    if(beg_src1 < 0)
      beg_src1 = 0;
    if(beg_src2 < 0)
      beg_src2 = 0;
    if(beg_dst < 0)
      beg_dst = 0;
    if(n < 0)
      n = 0;
    
    ok_src1 = iem_tab_check_arrays(gensym("tab_le"), x->x_sym_scr1, &x->x_beg_mem_src1, &x->x_size_src1, beg_src1+n);
    ok_src2 = iem_tab_check_arrays(gensym("tab_le"), x->x_sym_scr2, &x->x_beg_mem_src2, &x->x_size_src2, beg_src2+n);
    ok_dst = iem_tab_check_arrays(gensym("tab_le"), x->x_sym_dst, &x->x_beg_mem_dst, &x->x_size_dst, beg_dst+n);
    
    if(ok_src1 && ok_src2 && ok_dst)
    {
      vec_src1 = x->x_beg_mem_src1 + beg_src1;
      vec_src2 = x->x_beg_mem_src2 + beg_src2;
      vec_dst = x->x_beg_mem_dst + beg_dst;
      if(n)
      {
        t_garray *a;
        
        for(i=0; i<n; i++)
        {
          if(vec_src1[i] <= vec_src2[i])
            vec_dst[i] = 1.0f;
          else
            vec_dst[i] = 0.0f;
        }
        outlet_bang(x->x_obj.ob_outlet);
        a = (t_garray *)pd_findbyclass(x->x_sym_dst, garray_class);
        garray_redraw(a);
      }
    }
  }
  else
  {
    post("tab_le-ERROR: list need 4 float arguments:");
    post("  source1_offset + source2_offset + destination_offset + number_of_samples_to_compare");
  }
}

static void tab_le_free(t_tab_le *x)
{
}

static void *tab_le_new(t_symbol *s, int argc, t_atom *argv)
{
  t_tab_le *x = (t_tab_le *)pd_new(tab_le_class);
  t_symbol  *src1, *src2, *dst;
  t_float time;
  
  if((argc >= 3) &&
    IS_A_SYMBOL(argv,0) &&
    IS_A_SYMBOL(argv,1) &&
    IS_A_SYMBOL(argv,2))
  {
    src1 = (t_symbol *)atom_getsymbolarg(0, argc, argv);
    src2 = (t_symbol *)atom_getsymbolarg(1, argc, argv);
    dst = (t_symbol *)atom_getsymbolarg(2, argc, argv);
  }
  else if((argc >= 2) &&
    IS_A_SYMBOL(argv,0) &&
    IS_A_SYMBOL(argv,1))
  {
    src1 = (t_symbol *)atom_getsymbolarg(0, argc, argv);
    dst = src1;
    src2 = (t_symbol *)atom_getsymbolarg(1, argc, argv);
  }
  else
  {
    post("tab_le-ERROR: need 3 symbols arguments:");
    post("  source1_array_name + source2_array_name + destination_array_name");
    return(0);
  }
  
  x->x_sym_scr1 = src1;
  x->x_sym_scr2 = src2;
  x->x_sym_dst = dst;
  outlet_new(&x->x_obj, &s_bang);
  return(x);
}

void tab_le_setup(void)
{
  tab_le_class = class_new(gensym("tab_le"), (t_newmethod)tab_le_new, (t_method)tab_le_free,
    sizeof(t_tab_le), 0, A_GIMME, 0);
  class_addbang(tab_le_class, (t_method)tab_le_bang);
  class_addlist(tab_le_class, (t_method)tab_le_list);
  class_addmethod(tab_le_class, (t_method)tab_le_src1, gensym("src1"), A_DEFSYMBOL, 0);
  class_addmethod(tab_le_class, (t_method)tab_le_src2, gensym("src2"), A_DEFSYMBOL, 0);
  class_addmethod(tab_le_class, (t_method)tab_le_dst, gensym("dst"), A_DEFSYMBOL, 0);
//  class_sethelpsymbol(tab_le_class, gensym("iemhelp2/tab_le-help"));
}
