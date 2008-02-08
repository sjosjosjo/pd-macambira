//////////////////////////////////////////////////////////////////////
// this is the standard blablabla 
// made for pd
// Gnu Public Licence
// cyrille.henry@la-kitchen.fr
//
// pmpd~
// The same than pmpd, but with audio data for input and output
// It can be used for particular physical modeling and for modal sound synthesis
//////////////////////////////////////////////////////////////////////


#include "m_pd.h"
#include "stdio.h"

#define max(a,b) ( ((a) > (b)) ? (a) : (b) ) 
#define min(a,b) ( ((a) < (b)) ? (a) : (b) ) 

#define nb_max_link   2000
#define nb_max_mass   2000
#define nb_max_out    200
#define nb_max_in     200
#define nb_max_outlet 20
#define nb_max_inlet  20 // hard-coded on the methods definition

static t_class *pmpd_tilde_class;

typedef struct _mass {
	t_int Id;
	t_float invM;
	t_float speedX;
	t_float posX;
	t_float forceX;
} foo;

typedef struct _link {
	t_int Id;
	struct _mass *mass1;
	struct _mass *mass2;
	t_float Ke, K1, D1, K2, D2;
} foo1 ;

typedef struct _out {
	// TODO ajouter un type pour diferencier les outlets en forces et celles en position
	t_int Id;
	t_int nbr_outlet;
	struct _mass *mass1;
	t_float influence;
} foo2;

typedef struct _in {
	// TODO ajouter un type pour diferencier les inlets en forces et celles en position
	t_int Id;
	t_int nbr_inlet;
	struct _mass *mass1;
	t_float influence;
} foo3;

typedef struct _pmpd_tilde {
	t_object  x_obj;
	struct _link link[nb_max_link];
	struct _mass mass[nb_max_mass];
	struct _out out[nb_max_out];
	struct _in in[nb_max_in];
	t_float outlet[nb_max_outlet];
	t_sample *outlet_vector[nb_max_outlet];
	t_sample *inlet_vector[nb_max_inlet];
	int nb_link, nb_mass, nb_inlet, nb_outlet, nb_in, nb_out;
	t_sample f; // used for signal inlet
	t_int loop, nb_loop; // to be able not to compute everything a each iteration
	unsigned int x_state; // random
    t_float x_f; // random
} t_pmpd_tilde;

static int makeseed_pmpd_tilde(void)
{
    static unsigned int random_nextseed = 1489853723;
    random_nextseed = random_nextseed * 435898247 + 938284287;
    return (random_nextseed & 0x7fffffff);
}

static float random_bang_pmpd_tilde(t_pmpd_tilde *x)
{
    int nval;
    int range = 2000000;
	float rnd;
	unsigned int randval = x->x_state;
	x->x_state = randval = randval * 472940017 + 832416023;
    nval = ((double)range) * ((double)randval)
    	* (1./4294967296.);
    if (nval >= range) nval = range-1;

	rnd=nval;

	rnd-=1000000;
	rnd=rnd/1000000.;	//pour mettre entre -1 et 1;
    return (rnd);
}

t_int *pmpd_tilde_perform(t_int *w)
///////////////////////////////////////////////////////////////////////////////////
{
	t_pmpd_tilde *x = (t_pmpd_tilde *)(w[1]);
	int n = (int)(w[2]);

	t_float F;
	t_int i;
	struct _mass mass_1, mass_2;

	t_sample *out[nb_max_outlet]; 
	t_sample *in[nb_max_inlet];

	for (i=0; i<x->nb_inlet; i++)
		// TODO : ameliorer la copie du tableau a l'initialisation
		in[i]= x->inlet_vector[i];

	for (i=0; i<x->nb_outlet; i++)
		// TODO : ameliorer la copie du tableau
		out[i]= x->outlet_vector[i];

	while (n--) 
	{
		if (++x->loop == x->nb_loop)
		{
			x->loop = 0;

			for (i=0; i<x->nb_in; i++)
				if ( x->in[i].influence == 0)
				// get inlet value and make it a position to the specified mass
					x->in[i].mass1->posX = *in[x->in[i].nbr_inlet];
				else
				// get inlet value and make it a force to the specified mass
					x->in[i].mass1->forceX += x->in[i].influence * *in[x->in[i].nbr_inlet];

			for (i=0; i<x->nb_link; i++)
			// compute forces generated by links (spring / dashpot)
			{
				F  = x->link[i].K1 * ( x->link[i].mass1->posX   - x->link[i].mass2->posX  ) ;
				// spring

				F += x->link[i].D1 * ( x->link[i].mass1->speedX - x->link[i].mass2->speedX) ;
				// dashpot
			
				x->link[i].mass1->forceX -= F;
				x->link[i].mass2->forceX += F;
			}

			for (i=1; i<x->nb_mass; i++)
			// compute new masses position
				if (x->mass[i].Id >0) // only if Id >0
				{
					x->mass[i].speedX += x->mass[i].forceX * x->mass[i].invM;
					x->mass[i].forceX = random_bang_pmpd_tilde(x) * 1e-25;
					x->mass[i].posX += x->mass[i].speedX ;
				}
		}	

		for (i=0; i<x->nb_inlet; i++)
		// increase pointer to inlet vectors value
			in[i]++;

		for (i=0; i<x->nb_out; i++)
			x->outlet[x->out[i].nbr_outlet] += x->out[i].mass1->posX * x->out[i].influence;
			// compute output vector value		

		for (i=0; i<x->nb_outlet; i++)
		// send vector value to the vector pointer
		{
			*out[i]++ = x->outlet[i];
			x->outlet[i] = 0;
		}
	}
	return(w+3);
}

void pmpd_tilde_dsp(t_pmpd_tilde *x, t_signal **sp)
{
	int i;

	for (i=0; i<x->nb_inlet; i++)
		x->inlet_vector[i] = sp[i]->s_vec;

	for (i=0; i<x->nb_outlet; i++)
		x->outlet_vector[i] = sp[i+x->nb_inlet]->s_vec;

	dsp_add(pmpd_tilde_perform, 2, x, sp[0]->s_n);
//	post("%p, %p, %p", sp, sp2[2], sp[2]->s_vec);
}

void pmpd_tilde_bang(t_pmpd_tilde *x)
{
// add a unity force to all masses
	int i;
	for (i=0;i < x->nb_mass; i++)
		x->mass[i].forceX += 1;
}

void pmpd_tilde_float(t_pmpd_tilde *x, t_float force)
{
// add a unity force to all masses
	int i;
	for (i=0;i < x->nb_mass; i++)
		x->mass[i].forceX += force;
}

void pmpd_tilde_forceX(t_pmpd_tilde *x, t_float nbr_mass, t_float force)
{
// add a force to a specific mass
	nbr_mass = max(0, min( x->nb_mass, (int)nbr_mass));
	x->mass[(int)nbr_mass].forceX += force;
}

void pmpd_tilde_posX(t_pmpd_tilde *x, t_float nbr_mass, t_float posX)
{
// displace a mass to a certain position
	nbr_mass = max(0, min( x->nb_mass, (int)nbr_mass));
	x->mass[(int)nbr_mass].posX = posX;
}


void pmpd_tilde_mass(t_pmpd_tilde *x, t_float Id, t_float M, t_float posX)
// add a mass
// Id, invM speedX posX forceX
{
	if (M==0) M=1;
	x->mass[x->nb_mass].Id = (int)Id;
	x->mass[x->nb_mass].invM = 1/M;
	x->mass[x->nb_mass].speedX = 0;
	x->mass[x->nb_mass].posX = posX;
	x->mass[x->nb_mass].forceX = 0;

	x->nb_mass++ ;
	x->nb_mass = min ( nb_max_mass -1, x->nb_mass );
}

void pmpd_tilde_link(t_pmpd_tilde *x, t_float Id, t_float mass_1, t_float mass_2, t_float K1, t_float D1)
// add a link
// Id, *mass1, *mass2, Ke, K1, D1, K2, D2;
{

	x->link[x->nb_link].Id = (int)Id;
	x->link[x->nb_link].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->link[x->nb_link].mass2 = &x->mass[max(0, min ( x->nb_mass, (int)mass_2))];
	x->link[x->nb_link].K1 = K1;
	x->link[x->nb_link].D1 = D1;

	x->nb_link++ ;
	x->nb_link = min ( nb_max_link -1, x->nb_link );
}


void pmpd_tilde_out(t_pmpd_tilde *x, t_float Id, t_float nb_outlet, t_float mass_1, t_float influence)
// add an output point
// Id, nbr_outlet, *mass1, influence;
{
	x->out[x->nb_out].Id = (int)Id;
	x->out[x->nb_out].nbr_outlet = max(0, min( x->nb_outlet,(int)nb_outlet));
	x->out[x->nb_out].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->out[x->nb_out].influence = influence;

	x->nb_out++ ;
	x->nb_out = min ( nb_max_out - 1, x->nb_out );
}

void pmpd_tilde_in(t_pmpd_tilde *x, t_float Id, t_float nb_inlet, t_float mass_1, t_float influence)
//add an input point
// Id, nbr_inlet, *mass1, influence;
{
	x->in[x->nb_in].Id = (int)Id;
	x->in[x->nb_in].nbr_inlet = max(0, min( x->nb_inlet,(int)nb_inlet));
	x->in[x->nb_in].mass1 = &x->mass[max(0, min ( x->nb_mass, (int)mass_1))];
	x->in[x->nb_in].influence = influence;

	x->nb_in++;
	x->nb_in = min ( nb_max_in - 1, x->nb_in );
}

void pmpd_tilde_reset(t_pmpd_tilde *x)
{
	x->nb_link = 0;
	x->nb_mass = 1;
	x->nb_out= 0;
	x->nb_in= 0;
	x->mass[x->nb_mass].posX = 1;
// ajouter le reset des paramettress de la masse
}

void *pmpd_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
	int i;

	t_pmpd_tilde *x = (t_pmpd_tilde *)pd_new(pmpd_tilde_class);

	pmpd_tilde_reset(x);
	makeseed_pmpd_tilde();
	
	x->nb_outlet= (int)atom_getfloatarg(1, argc, argv);
	x->nb_outlet= max(1, min(nb_max_outlet, x->nb_outlet) );
	for(i=0; i<x->nb_outlet; i++)
		outlet_new(&x->x_obj, &s_signal);

	x->nb_inlet = (int)atom_getfloatarg(0, argc, argv);
	x->nb_inlet= max(1, min(nb_max_inlet, x->nb_inlet) );
	for(i=0; i<x->nb_inlet-1; i++)
		inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

	x->nb_loop = max (1, (int)atom_getfloatarg(2, argc, argv) );
	x->loop = 0 ;

	
	return (void *)x;
}

void pmpd_tilde_setup(void) {
	pmpd_tilde_class = class_new(gensym("pmpd~"), (t_newmethod)pmpd_tilde_new, 0, sizeof(t_pmpd_tilde), CLASS_DEFAULT, A_GIMME, 0);

	CLASS_MAINSIGNALIN(pmpd_tilde_class, t_pmpd_tilde, f);

	class_addbang(pmpd_tilde_class, pmpd_tilde_bang);
	class_addfloat(pmpd_tilde_class, (t_method)pmpd_tilde_float);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_mass,		gensym("mass"),		A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_link,		gensym("link"),		A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_out,			gensym("out"),		A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_in,			gensym("in"),		A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_posX,		gensym("posX"),      A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_forceX,		gensym("forceX"),    A_DEFFLOAT, A_DEFFLOAT, 0);

	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_reset,		gensym("reset"), 0);
	class_addmethod(pmpd_tilde_class, (t_method)pmpd_tilde_dsp,			gensym("dsp"), 0);
}
