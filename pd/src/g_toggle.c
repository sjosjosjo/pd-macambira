/* Copyright (c) 1997-1999 Miller Puckette.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution. */

/* g_7_guis.c written by Thomas Musil (c) IEM KUG Graz Austria 2000-2001 */
/* thanks to Miller Puckette, Guenther Geiger and Krzystof Czaja */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "m_imp.h"
#include "g_canvas.h"
#include "t_tk.h"
#include "g_all_guis.h"
#include <math.h>

#ifdef NT
#include <io.h>
#else
#include <unistd.h>
#endif

/* --------------- tgl     gui-toggle ------------------------- */

t_widgetbehavior toggle_widgetbehavior;
static t_class *toggle_class;

/* widget helper functions */

void toggle_draw_update(t_toggle *x, t_glist *glist)
{
    if(glist_isvisible(glist))
    {
	t_canvas *canvas=glist_getcanvas(glist);

	sys_vgui(".x%x.c itemconfigure %xX1 -fill #%6.6x\n", canvas, x,
		 (x->x_on!=0.0)?x->x_gui.x_fcol:x->x_gui.x_bcol);
	sys_vgui(".x%x.c itemconfigure %xX2 -fill #%6.6x\n", canvas, x,
		 (x->x_on!=0.0)?x->x_gui.x_fcol:x->x_gui.x_bcol);
    }
}

void toggle_draw_new(t_toggle *x, t_glist *glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int w=1, xx=text_xpix(&x->x_gui.x_obj, glist), yy=text_ypix(&x->x_gui.x_obj, glist);

    if(x->x_gui.x_w >= 30)
	w = 2;
    if(x->x_gui.x_w >= 60)
	w = 3;
    sys_vgui(".x%x.c create rectangle %d %d %d %d -fill #%6.6x -tags %xBASE\n",
	     canvas, xx, yy, xx + x->x_gui.x_w, yy + x->x_gui.x_h,
	     x->x_gui.x_bcol, x);
    sys_vgui(".x%x.c create line %d %d %d %d -width %d -fill #%6.6x -tags %xX1\n",
	     canvas, xx+w+1, yy+w+1, xx + x->x_gui.x_w-w, yy + x->x_gui.x_h-w, w,
	     (x->x_on!=0.0)?x->x_gui.x_fcol:x->x_gui.x_bcol, x);
    sys_vgui(".x%x.c create line %d %d %d %d -width %d -fill #%6.6x -tags %xX2\n",
	     canvas, xx+w+1, yy + x->x_gui.x_h-w-1, xx + x->x_gui.x_w-w, yy+w, w,
	     (x->x_on!=0.0)?x->x_gui.x_fcol:x->x_gui.x_bcol, x);
    sys_vgui(".x%x.c create text %d %d -text {%s} -anchor w \
	     -font {%s %d bold} -fill #%6.6x -tags %xLABEL\n",
	     canvas, xx+x->x_gui.x_ldx,
	     yy+x->x_gui.x_ldy,
	     strcmp(x->x_gui.x_lab->s_name, "empty")?x->x_gui.x_lab->s_name:"",
	     x->x_gui.x_font, x->x_gui.x_fontsize, x->x_gui.x_lcol, x);
    if(!x->x_gui.x_fsf.x_snd_able)
        sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xOUT%d\n",
	     canvas, xx, yy + x->x_gui.x_h-1, xx + IOWIDTH, yy + x->x_gui.x_h, x, 0);
    if(!x->x_gui.x_fsf.x_rcv_able)
	sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xIN%d\n",
	     canvas, xx, yy, xx + IOWIDTH, yy+1, x, 0);
}

void toggle_draw_move(t_toggle *x, t_glist *glist)
{
    t_canvas *canvas=glist_getcanvas(glist);
    int w=1, xx=text_xpix(&x->x_gui.x_obj, glist), yy=text_ypix(&x->x_gui.x_obj, glist);

    if(x->x_gui.x_w >= 30)
	w = 2;

    if(x->x_gui.x_w >= 60)
	w = 3;
    sys_vgui(".x%x.c coords %xBASE %d %d %d %d\n",
	     canvas, x, xx, yy, xx + x->x_gui.x_w, yy + x->x_gui.x_h);
    sys_vgui(".x%x.c itemconfigure %xX1 -width %d\n", canvas, x, w);
    sys_vgui(".x%x.c coords %xX1 %d %d %d %d\n",
	     canvas, x, xx+w+1, yy+w+1, xx + x->x_gui.x_w-w, yy + x->x_gui.x_h-w);
    sys_vgui(".x%x.c itemconfigure %xX2 -width %d\n", canvas, x, w);
    sys_vgui(".x%x.c coords %xX2 %d %d %d %d\n",
	     canvas, x, xx+w+1, yy + x->x_gui.x_h-w-1, xx + x->x_gui.x_w-w, yy+w);
    sys_vgui(".x%x.c coords %xLABEL %d %d\n",
	     canvas, x, xx+x->x_gui.x_ldx, yy+x->x_gui.x_ldy);
    if(!x->x_gui.x_fsf.x_snd_able)
        sys_vgui(".x%x.c coords %xOUT%d %d %d %d %d\n",
	     canvas, x, 0, xx, yy + x->x_gui.x_h-1, xx + IOWIDTH, yy + x->x_gui.x_h);
    if(!x->x_gui.x_fsf.x_rcv_able)
	sys_vgui(".x%x.c coords %xIN%d %d %d %d %d\n",
	     canvas, x, 0, xx, yy, xx + IOWIDTH, yy+1);
}

void toggle_draw_erase(t_toggle* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);

    sys_vgui(".x%x.c delete %xBASE\n", canvas, x);
    sys_vgui(".x%x.c delete %xX1\n", canvas, x);
    sys_vgui(".x%x.c delete %xX2\n", canvas, x);
    sys_vgui(".x%x.c delete %xLABEL\n", canvas, x);
    if(!x->x_gui.x_fsf.x_snd_able)
        sys_vgui(".x%x.c delete %xOUT%d\n", canvas, x, 0);
    if(!x->x_gui.x_fsf.x_rcv_able)
	sys_vgui(".x%x.c delete %xIN%d\n", canvas, x, 0);
}

void toggle_draw_config(t_toggle* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);

    sys_vgui(".x%x.c itemconfigure %xLABEL -font {%s %d bold} -fill #%6.6x -text {%s} \n",
	     canvas, x, x->x_gui.x_font, x->x_gui.x_fontsize,
	     x->x_gui.x_fsf.x_selected?IEM_GUI_COLOR_SELECTED:x->x_gui.x_lcol,
	     strcmp(x->x_gui.x_lab->s_name, "empty")?x->x_gui.x_lab->s_name:"");
    sys_vgui(".x%x.c itemconfigure %xBASE -fill #%6.6x\n", canvas, x,
	     x->x_gui.x_bcol);
    sys_vgui(".x%x.c itemconfigure %xX1 -fill #%6.6x\n", canvas, x,
	     x->x_on?x->x_gui.x_fcol:x->x_gui.x_bcol);
    sys_vgui(".x%x.c itemconfigure %xX2 -fill #%6.6x\n", canvas, x,
	     x->x_on?x->x_gui.x_fcol:x->x_gui.x_bcol);
}

void toggle_draw_io(t_toggle* x, t_glist* glist, int old_snd_rcv_flags)
{
    int xpos=text_xpix(&x->x_gui.x_obj, glist);
    int ypos=text_ypix(&x->x_gui.x_obj, glist);
    t_canvas *canvas=glist_getcanvas(glist);

    if((old_snd_rcv_flags & IEM_GUI_OLD_SND_FLAG) && !x->x_gui.x_fsf.x_snd_able)
        sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xOUT%d\n",
	     canvas, xpos,
	     ypos + x->x_gui.x_h-1, xpos + IOWIDTH,
	     ypos + x->x_gui.x_h, x, 0);
    if(!(old_snd_rcv_flags & IEM_GUI_OLD_SND_FLAG) && x->x_gui.x_fsf.x_snd_able)
        sys_vgui(".x%x.c delete %xOUT%d\n", canvas, x, 0);
    if((old_snd_rcv_flags & IEM_GUI_OLD_RCV_FLAG) && !x->x_gui.x_fsf.x_rcv_able)
        sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xIN%d\n",
	     canvas, xpos, ypos,
	     xpos + IOWIDTH, ypos+1, x, 0);
    if(!(old_snd_rcv_flags & IEM_GUI_OLD_RCV_FLAG) && x->x_gui.x_fsf.x_rcv_able)
	sys_vgui(".x%x.c delete %xIN%d\n", canvas, x, 0);
}

void toggle_draw_select(t_toggle* x, t_glist* glist)
{
    t_canvas *canvas=glist_getcanvas(glist);

    if(x->x_gui.x_fsf.x_selected)
    {
	sys_vgui(".x%x.c itemconfigure %xBASE -outline #%6.6x\n", canvas, x, IEM_GUI_COLOR_SELECTED);
	sys_vgui(".x%x.c itemconfigure %xLABEL -fill #%6.6x\n", canvas, x, IEM_GUI_COLOR_SELECTED);
    }
    else
    {
	sys_vgui(".x%x.c itemconfigure %xBASE -outline #%6.6x\n", canvas, x, IEM_GUI_COLOR_NORMAL);
	sys_vgui(".x%x.c itemconfigure %xLABEL -fill #%6.6x\n", canvas, x, x->x_gui.x_lcol);
    }
}

void toggle_draw(t_toggle *x, t_glist *glist, int mode)
{
    if(mode == IEM_GUI_DRAW_MODE_UPDATE)
	toggle_draw_update(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_MOVE)
	toggle_draw_move(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_NEW)
	toggle_draw_new(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_SELECT)
	toggle_draw_select(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_ERASE)
	toggle_draw_erase(x, glist);
    else if(mode == IEM_GUI_DRAW_MODE_CONFIG)
	toggle_draw_config(x, glist);
    else if(mode >= IEM_GUI_DRAW_MODE_IO)
	toggle_draw_io(x, glist, mode - IEM_GUI_DRAW_MODE_IO);
}

/* ------------------------ tgl widgetbehaviour----------------------------- */

static void toggle_getrect(t_gobj *z, t_glist *glist, int *xp1, int *yp1, int *xp2, int *yp2)
{
    t_toggle *x = (t_toggle *)z;

    *xp1 = text_xpix(&x->x_gui.x_obj, glist);
    *yp1 = text_ypix(&x->x_gui.x_obj, glist);
    *xp2 = *xp1 + x->x_gui.x_w;
    *yp2 = *yp1 + x->x_gui.x_h;
}

static void toggle_save(t_gobj *z, t_binbuf *b)
{
    t_toggle *x = (t_toggle *)z;
    int bflcol[3], *ip1, *ip2;
    t_symbol *srl[3];

    iemgui_save(&x->x_gui, srl, bflcol);
    ip1 = (int *)(&x->x_gui.x_isa);
    ip2 = (int *)(&x->x_gui.x_fsf);
    binbuf_addv(b, "ssiisiisssiiiiiiiff", gensym("#X"),gensym("obj"),
		(t_int)x->x_gui.x_obj.te_xpix,
		(t_int)x->x_gui.x_obj.te_ypix,
		gensym("tgl"), x->x_gui.x_w,
		(*ip1)&IEM_INIT_ARGS_ALL,
		srl[0], srl[1], srl[2],
		x->x_gui.x_ldx, x->x_gui.x_ldy,
		(*ip2)&IEM_FSTYLE_FLAGS_ALL, x->x_gui.x_fontsize,
		bflcol[0], bflcol[1], bflcol[2], x->x_on, x->x_nonzero);
    binbuf_addv(b, ";");
}

static void toggle_properties(t_gobj *z, t_glist *owner)
{
    t_toggle *x = (t_toggle *)z;
    char buf[800];
    t_symbol *srl[3];

    iemgui_properties(&x->x_gui, srl);
    sprintf(buf, "pdtk_iemgui_dialog %%s TOGGLE \
	    ----------dimensions(pix):----------- %d %d size: 0 0 empty \
	    -----------non-zero-value:----------- %g value: 0.0 empty %g \
	    -1 lin log %d %d empty %d \
	    %s %s \
	    %s %d %d \
	    %d %d \
	    %d %d %d\n",
	    x->x_gui.x_w, IEM_GUI_MINSIZE,
	    x->x_nonzero, 1.0,/*non_zero-schedule*/
	    x->x_gui.x_isa.x_loadinit, -1, -1,/*no multi*/
	    srl[0]->s_name, srl[1]->s_name,
	    srl[2]->s_name, x->x_gui.x_ldx, x->x_gui.x_ldy,
	    x->x_gui.x_fsf.x_font_style, x->x_gui.x_fontsize,
	    0xffffff & x->x_gui.x_bcol, 0xffffff & x->x_gui.x_fcol, 0xffffff & x->x_gui.x_lcol);
    gfxstub_new(&x->x_gui.x_obj.ob_pd, x, buf);
}

static void toggle_bang(t_toggle *x)
{
    x->x_on = (x->x_on==0.0)?x->x_nonzero:0.0;
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
    outlet_float(x->x_gui.x_obj.ob_outlet, x->x_on);
    if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
	pd_float(x->x_gui.x_snd->s_thing, x->x_on);
}

static void toggle_dialog(t_toggle *x, t_symbol *s, int argc, t_atom *argv)
{
    t_symbol *srl[3];
    int a = (int)atom_getintarg(0, argc, argv);
    float nonzero = (float)atom_getfloatarg(2, argc, argv);
    int sr_flags;

    if(nonzero == 0.0)
	nonzero = 1.0;
    x->x_nonzero = nonzero;
    if(x->x_on != 0.0)
	x->x_on = x->x_nonzero;
    sr_flags = iemgui_dialog(&x->x_gui, srl, argc, argv);
    x->x_gui.x_w = iemgui_clip_size(a);
    x->x_gui.x_h = x->x_gui.x_w;
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_CONFIG);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_IO + sr_flags);
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_MOVE);
    canvas_fixlinesfor(glist_getcanvas(x->x_gui.x_glist), (t_text*)x);
}

static void toggle_click(t_toggle *x, t_floatarg xpos, t_floatarg ypos, t_floatarg shift, t_floatarg ctrl, t_floatarg alt)
{toggle_bang(x);}

static int toggle_newclick(t_gobj *z, struct _glist *glist, int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
    if(doit)
	toggle_click((t_toggle *)z, (t_floatarg)xpix, (t_floatarg)ypix, (t_floatarg)shift, 0, (t_floatarg)alt);
    return (1);
}

static void toggle_set(t_toggle *x, t_floatarg f)
{
    x->x_on = f;
    if(f != 0.0)
	x->x_nonzero = f;
    (*x->x_gui.x_draw)(x, x->x_gui.x_glist, IEM_GUI_DRAW_MODE_UPDATE);
}

static void toggle_float(t_toggle *x, t_floatarg f)
{
    toggle_set(x, f);
    if(x->x_gui.x_fsf.x_put_in2out)
    {
	outlet_float(x->x_gui.x_obj.ob_outlet, x->x_on);
	if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
	    pd_float(x->x_gui.x_snd->s_thing, x->x_on);
    }
}

static void toggle_fout(t_toggle *x, t_floatarg f)
{
    toggle_set(x, f);
    outlet_float(x->x_gui.x_obj.ob_outlet, x->x_on);
    if(x->x_gui.x_fsf.x_snd_able && x->x_gui.x_snd->s_thing)
	pd_float(x->x_gui.x_snd->s_thing, x->x_on);
}

static void toggle_loadbang(t_toggle *x)
{
    if(!sys_noloadbang && x->x_gui.x_isa.x_loadinit)
	toggle_fout(x, (float)x->x_on);
}

static void toggle_size(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{
    x->x_gui.x_w = iemgui_clip_size((int)atom_getintarg(0, ac, av));
    x->x_gui.x_h = x->x_gui.x_w;
    iemgui_size((void *)x, &x->x_gui);
}

static void toggle_delta(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{iemgui_delta((void *)x, &x->x_gui, s, ac, av);}

static void toggle_pos(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{iemgui_pos((void *)x, &x->x_gui, s, ac, av);}

static void toggle_color(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{iemgui_color((void *)x, &x->x_gui, s, ac, av);}

static void toggle_send(t_toggle *x, t_symbol *s)
{iemgui_send(x, &x->x_gui, s);}

static void toggle_receive(t_toggle *x, t_symbol *s)
{iemgui_receive(x, &x->x_gui, s);}

static void toggle_label(t_toggle *x, t_symbol *s)
{iemgui_label((void *)x, &x->x_gui, s);}

static void toggle_label_font(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{iemgui_label_font((void *)x, &x->x_gui, s, ac, av);}

static void toggle_label_pos(t_toggle *x, t_symbol *s, int ac, t_atom *av)
{iemgui_label_pos((void *)x, &x->x_gui, s, ac, av);}

static void toggle_init(t_toggle *x, t_floatarg f)
{
    x->x_gui.x_isa.x_loadinit = (f==0.0)?0:1;
}

static void toggle_nonzero(t_toggle *x, t_floatarg f)
{
    if(f != 0.0)
	x->x_nonzero = f;
}

static void *toggle_new(t_symbol *s, int argc, t_atom *argv)
{
    t_toggle *x = (t_toggle *)pd_new(toggle_class);
    int bflcol[]={-262144, -1, -1};
    t_symbol *srl[3];
    int a=IEM_GUI_DEFAULTSIZE, f=0;
    int ldx=0, ldy=-6;
    int fs=8, iinit=0, ifstyle=0;
    float on=0.0, nonzero=1.0;
    t_iem_init_symargs *init=(t_iem_init_symargs *)(&iinit);
    t_iem_fstyle_flags *fstyle=(t_iem_fstyle_flags *)(&ifstyle);
    char str[144];

    srl[0] = gensym("empty");
    srl[1] = gensym("empty");
    srl[2] = gensym("empty");

    if(((argc == 13)||(argc == 14))&&IS_A_FLOAT(argv,0)
       &&IS_A_FLOAT(argv,1)
       &&(IS_A_SYMBOL(argv,2)||IS_A_FLOAT(argv,2))
       &&(IS_A_SYMBOL(argv,3)||IS_A_FLOAT(argv,3))
       &&(IS_A_SYMBOL(argv,4)||IS_A_FLOAT(argv,4))
       &&IS_A_FLOAT(argv,5)&&IS_A_FLOAT(argv,6)
       &&IS_A_FLOAT(argv,7)&&IS_A_FLOAT(argv,8)&&IS_A_FLOAT(argv,9)
       &&IS_A_FLOAT(argv,10)&&IS_A_FLOAT(argv,11)&&IS_A_FLOAT(argv,12))
    {
	a = (int)atom_getintarg(0, argc, argv);
	iinit = (int)atom_getintarg(1, argc, argv);
	if(IS_A_SYMBOL(argv,2))
	    srl[0] = atom_getsymbolarg(2, argc, argv);
	else if(IS_A_FLOAT(argv,2))
	{
	    sprintf(str, "%d", (int)atom_getintarg(2, argc, argv));
	    srl[0] = gensym(str);
	}
	if(IS_A_SYMBOL(argv,3))
	    srl[1] = atom_getsymbolarg(3, argc, argv);
	else if(IS_A_FLOAT(argv,3))
	{
	    sprintf(str, "%d", (int)atom_getintarg(3, argc, argv));
	    srl[1] = gensym(str);
	}
	if(IS_A_SYMBOL(argv,4))
	    srl[2] = atom_getsymbolarg(4, argc, argv);
	else if(IS_A_FLOAT(argv,4))
	{
	    sprintf(str, "%d", (int)atom_getintarg(4, argc, argv));
	    srl[2] = gensym(str);
	}
	ldx = (int)atom_getintarg(5, argc, argv);
	ldy = (int)atom_getintarg(6, argc, argv);
	ifstyle = (int)atom_getintarg(7, argc, argv);
	fs = (int)atom_getintarg(8, argc, argv);
	bflcol[0] = (int)atom_getintarg(9, argc, argv);
	bflcol[1] = (int)atom_getintarg(10, argc, argv);
	bflcol[2] = (int)atom_getintarg(11, argc, argv);
	on = (float)atom_getfloatarg(12, argc, argv);
    }
    if((argc == 14)&&IS_A_FLOAT(argv,13))
	nonzero = (float)atom_getfloatarg(13, argc, argv);
    x->x_gui.x_draw = (t_iemfunptr)toggle_draw;
    iinit &= IEM_INIT_ARGS_ALL;
    ifstyle &= IEM_FSTYLE_FLAGS_ALL;

    fstyle->x_snd_able = 1;
    fstyle->x_rcv_able = 1;
    x->x_gui.x_glist = (t_glist *)canvas_getcurrent();
    x->x_gui.x_isa = *init;
    if(!strcmp(srl[0]->s_name, "empty")) fstyle->x_snd_able = 0;
    if(!strcmp(srl[1]->s_name, "empty")) fstyle->x_rcv_able = 0;
    x->x_gui.x_unique_num = 0;
    if(fstyle->x_font_style == 1) strcpy(x->x_gui.x_font, "helvetica");
    else if(fstyle->x_font_style == 2) strcpy(x->x_gui.x_font, "times");
    else { fstyle->x_font_style = 0;
	strcpy(x->x_gui.x_font, "courier"); }
    x->x_gui.x_fsf = *fstyle;
    x->x_nonzero = (nonzero!=0.0)?nonzero:1.0;
    if(x->x_gui.x_isa.x_loadinit)
	x->x_on = (on!=0.0)?nonzero:0.0;
    else
	x->x_on = 0.0;
    iemgui_first_dollararg2sym(&x->x_gui, srl);
    if(x->x_gui.x_fsf.x_rcv_able) pd_bind(&x->x_gui.x_obj.ob_pd, srl[1]);
    x->x_gui.x_snd = srl[0];
    x->x_gui.x_rcv = srl[1];
    x->x_gui.x_lab = srl[2];
    x->x_gui.x_ldx = ldx;
    x->x_gui.x_ldy = ldy;

    if(fs < 4)
	fs = 4;
    x->x_gui.x_fontsize = fs;
    x->x_gui.x_w = iemgui_clip_size(a);
    x->x_gui.x_h = x->x_gui.x_w;
    iemgui_all_colfromload(&x->x_gui, bflcol);
    iemgui_verify_snd_ne_rcv(&x->x_gui);
    outlet_new(&x->x_gui.x_obj, &s_float);
    return (x);
}

static void toggle_ff(t_toggle *x)
{
    if(x->x_gui.x_fsf.x_rcv_able)
	pd_unbind(&x->x_gui.x_obj.ob_pd, x->x_gui.x_rcv);
    gfxstub_deleteforkey(x);
}

void g_toggle_setup(void)
{
    toggle_class = class_new(gensym("tgl"), (t_newmethod)toggle_new,
			     (t_method)toggle_ff, sizeof(t_toggle), 0, A_GIMME, 0);
    class_addcreator((t_newmethod)toggle_new, gensym("toggle"), A_GIMME, 0);
    class_addbang(toggle_class, toggle_bang);
    class_addfloat(toggle_class, toggle_float);
    class_addmethod(toggle_class, (t_method)toggle_click, gensym("click"),
		    A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(toggle_class, (t_method)toggle_dialog, gensym("dialog"),
		    A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_loadbang, gensym("loadbang"), 0);
    class_addmethod(toggle_class, (t_method)toggle_set, gensym("set"), A_FLOAT, 0);
    class_addmethod(toggle_class, (t_method)toggle_size, gensym("size"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_delta, gensym("delta"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_pos, gensym("pos"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_color, gensym("color"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_send, gensym("send"), A_DEFSYM, 0);
    class_addmethod(toggle_class, (t_method)toggle_receive, gensym("receive"), A_DEFSYM, 0);
    class_addmethod(toggle_class, (t_method)toggle_label, gensym("label"), A_DEFSYM, 0);
    class_addmethod(toggle_class, (t_method)toggle_label_pos, gensym("label_pos"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_label_font, gensym("label_font"), A_GIMME, 0);
    class_addmethod(toggle_class, (t_method)toggle_init, gensym("init"), A_FLOAT, 0);
    class_addmethod(toggle_class, (t_method)toggle_nonzero, gensym("nonzero"), A_FLOAT, 0);
    toggle_widgetbehavior.w_getrectfn = toggle_getrect;
    toggle_widgetbehavior.w_displacefn = iemgui_displace;
    toggle_widgetbehavior.w_selectfn = iemgui_select;
    toggle_widgetbehavior.w_activatefn = NULL;
    toggle_widgetbehavior.w_deletefn = iemgui_delete;
    toggle_widgetbehavior.w_visfn = iemgui_vis;
    toggle_widgetbehavior.w_clickfn = toggle_newclick;
    toggle_widgetbehavior.w_propertiesfn = toggle_properties;
    toggle_widgetbehavior.w_savefn = toggle_save;
    class_setwidget(toggle_class, &toggle_widgetbehavior);
    class_sethelpsymbol(toggle_class, gensym("toggle"));
}
