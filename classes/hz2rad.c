// Porres 2016
 
#include "m_pd.h"
#include <math.h>

static t_class *hz2rad_class;

typedef struct _hz2rad
{
  t_object x_obj;
  t_float   x_radps;
  t_outlet *float_outlet;
  t_int bytes;
  t_atom *output_list;
  t_float f;
} t_hz2rad;

void *hz2rad_new(t_floatarg f1);
void hz2rad_free(t_hz2rad *x);
void hz2rad_float(t_hz2rad *x, t_floatarg f);
void hz2rad_bang(t_hz2rad *x);
void hz2rad_set(t_hz2rad *x, t_floatarg f);

static t_float convert(t_hz2rad *x, t_float f);

void hz2rad_float(t_hz2rad *x, t_floatarg f)
{
  x->f = f;
  outlet_float(x->float_outlet, convert(x,f));
}

t_float convert(t_hz2rad *x, t_float f)
{
    return f * x->x_radps;
}



void hz2rad_list(t_hz2rad *x, t_symbol *s, int argc, t_atom *argv)
{
  int old_bytes = x->bytes, i = 0;
  x->bytes = argc*sizeof(t_atom);
  x->output_list = (t_atom *)t_resizebytes(x->output_list,old_bytes,x->bytes);
  for(i=0;i<argc;i++)
    SETFLOAT(x->output_list+i,convert(x,atom_getfloatarg(i,argc,argv)));
  outlet_list(x->float_outlet,0,argc,x->output_list);
}

void hz2rad_set(t_hz2rad *x, t_float f)
{
  x->f = f;
}

void hz2rad_bang(t_hz2rad *x)
{
  outlet_float(x->float_outlet,convert(x, x->f));
}

void *hz2rad_new(t_floatarg f1)
{
  t_hz2rad *x = (t_hz2rad *) pd_new(hz2rad_class);
  x->x_radps = 2 * M_PI / sys_getsr();
  x->f = f1;
  x->float_outlet = outlet_new(&x->x_obj, 0);
  x->bytes = sizeof(t_atom);
  x->output_list = (t_atom *)getbytes(x->bytes);
  return (x);
}

void hz2rad_free(t_hz2rad *x)
{
  t_freebytes(x->output_list,x->bytes);
}

void hz2rad_setup(void)
{
  hz2rad_class = class_new(gensym("hz2rad"), (t_newmethod)hz2rad_new,
			  (t_method)hz2rad_free,sizeof(t_hz2rad),0, A_DEFFLOAT, 0);
  class_addfloat(hz2rad_class,(t_method)hz2rad_float);
  class_addlist(hz2rad_class,(t_method)hz2rad_list);
  class_addmethod(hz2rad_class,(t_method)hz2rad_set,gensym("set"),A_DEFFLOAT,0);
  class_addbang(hz2rad_class,(t_method)hz2rad_bang);
}
