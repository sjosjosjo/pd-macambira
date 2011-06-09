/* This is a version hacked by Martin Peach 20110120 martin.peach@sympatico.ca */
/* Reformmatted the code and added some debug messages. Changed the name of the class to pdlua */
/** @file lua.c 
 *  @brief pdlua -- a Lua embedding for Pd.
 *  @author Claude Heiland-Allen <claudiusmaximus@goto10.org>
 *  @date 2008
 *  @version 0.6~svn
 *
 * Copyright (C) 2007,2008 Claude Heiland-Allen <claudiusmaximus@goto10.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */ 

/* various C stuff, mainly for reading files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#include <io.h>
#define read _read
#define close _close
#define ssize_t int
#else
#include <unistd.h>
#endif
/* we use Lua */
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* we use Pd */
#include "m_pd.h"
#include "s_stuff.h" // for sys_register_loader()
#ifdef _MSC_VER
#include <s_stuff.h>
#endif
/* BAD: support for Pd < 0.41 */

#if PD_MAJOR_VERSION == 0
# if PD_MINOR_VERSION >= 41
#  define PDLUA_PD41
/* use new garray support that is 64-bit safe */
#  define PDLUA_ARRAYGRAB garray_getfloatwords
#  define PDLUA_ARRAYTYPE t_word
#  define PDLUA_ARRAYELEM(arr,idx) ((arr)[(idx)].w_float)
# elif PD_MINOR_VERSION >= 40
#  define PDLUA_PD40
/* use old garray support, not 64-bit safe */
#  define PDLUA_ARRAYGRAB garray_getfloatarray
#  define PDLUA_ARRAYTYPE t_float
#  define PDLUA_ARRAYELEM(arr,idx) ((arr)[(idx)])
# elif PD_MINOR_VERSION >= 39
#  define PDLUA_PD39
/* use old garray support, not 64-bit safe */
#  define PDLUA_ARRAYGRAB garray_getfloatarray
#  define PDLUA_ARRAYTYPE t_float
#  define PDLUA_ARRAYELEM(arr,idx) ((arr)[(idx)])
# else
#  error "Pd version is too old, please upgrade"
# endif
#else
# error "Pd version is too new, please file a bug report"
#endif

/* BAD: end of bad section */

/* EVIL: TODO: File requests/patches to Pd so that this becomes unnecessary. */

/** Pd loader type, defined in pd/src/s_loader.c but not exported. */
//typedef int (*loader_t)(t_canvas *, char *);

/** Pd loader registration, defined in pd/src/s_loader.c but not exported. */
//void sys_register_loader(loader_t loader);

/** Pd extern dir declaration (for help finding), defined in pd/src/m_class.c but not exported. */
//void class_set_extern_dir(t_symbol *s);

/** Pd [value] getter, defined in pd/src/x_connective.c but not exported. */
//int value_getfloat(t_symbol *s, t_float *f);

/** Pd [value] setter, defined in pd/src/x_connective.c but not exported. */
//int value_setfloat(t_symbol *s, t_float f);

/* EVIL: end of evil section. */

/* If defined, PDLUA_DEBUG lets pdlua post a lot of text */
// #define PDLUA_DEBUG
/** Global Lua interpreter state, needed in the constructor. */
static lua_State *L;

/** State for the Lua file reader. */
typedef struct pdlua_readerdata
{
    int         fd; /**< File descriptor to read from. */
    char        buffer[MAXPDSTRING]; /**< Buffer to read into. */
} t_pdlua_readerdata;

/** Lua file reader callback. */
static const char *pdlua_reader
(
    lua_State *L, /**< Lua interpreter state. */
    void *rr, /**< Lua file reader state. */
    size_t *size /**< How much data we have read. */
)
{
    t_pdlua_readerdata  *r = rr;
    ssize_t             s;
#ifdef PDLUA_DEBUG
    post("pdlua_reader: fd is %d", r->fd);
#endif // PDLUA_DEBUG
    s = read(r->fd, r->buffer, MAXPDSTRING-2);
#ifdef PDLUA_DEBUG
    post("pdlua_reader: s is %ld", s);////////
#endif // PDLUA_DEBUG
    if (s <= 0)
    {
        *size = 0;
        return NULL;
    }
    else
    {
        *size = s;
        return r->buffer;
    }
}

/* declare some stuff in advance */
struct pdlua_proxyinlet;
struct pdlua_proxyreceive;
struct pdlua_proxyclock;

/** Pd object data. */
typedef struct pdlua 
{
    t_object                pd; /**< We are a Pd object. */
    unsigned int            inlets; /**< Number of inlets. */
    struct pdlua_proxyinlet *in; /**< The inlets themselves. */
    unsigned int            outlets; /**< Number of outlets. */
    t_outlet                **out; /**< The outlets themselves. */
    t_canvas                *canvas; /**< The canvas that the object was created on. */
} t_pdlua;

/* more forward declarations */
static void pdlua_dispatch(t_pdlua *o, unsigned int inlet, t_symbol *s, int argc, t_atom *argv);
static void pdlua_receivedispatch(struct pdlua_proxyreceive *r, t_symbol *s, int argc, t_atom *argv);
static void pdlua_clockdispatch(struct pdlua_proxyclock *clock);

/** Proxy inlet class pointer. */
static t_class *pdlua_proxyinlet_class;

/** Proxy inlet object data. */
typedef struct pdlua_proxyinlet
{
    t_pd            pd; /**< Minimal Pd object. */
    struct pdlua    *owner; /**< The owning object to forward inlet messages to. */
    unsigned int    id; /**< The number of this inlet. */
} t_pdlua_proxyinlet;

/** Proxy inlet 'anything' method. */
static void pdlua_proxyinlet_anything
(
    t_pdlua_proxyinlet  *p, /**< The proxy inlet that received the message. */
    t_symbol            *s, /**< The message selector. */
    int                 argc, /**< The message length. */
    t_atom              *argv /**< The atoms in the message. */
)
{
    pdlua_dispatch(p->owner, p->id, s, argc, argv);
}

/** Proxy inlet initialization. */
static void pdlua_proxyinlet_init
(
    t_pdlua_proxyinlet  *p, /**< The proxy inlet to initialize. */
    struct pdlua        *owner, /**< The owning object. */
    unsigned int        id /**< The inlet number. */
)
{
    p->pd = pdlua_proxyinlet_class;
    p->owner = owner;
    p->id = id;
}

/** Register the proxy inlet class with Pd. */
static void pdlua_proxyinlet_setup(void)
{
    pdlua_proxyinlet_class = class_new(gensym("pdlua proxy inlet"), 0, 0, sizeof(t_pdlua_proxyinlet), 0, 0);
    class_addanything(pdlua_proxyinlet_class, pdlua_proxyinlet_anything);
}

/** Proxy receive class pointer. */
static t_class *pdlua_proxyreceive_class;

/** Proxy receive object data. */
typedef struct pdlua_proxyreceive
{
    t_pd            pd; /**< Minimal Pd object. */
    struct pdlua    *owner; /**< The owning object to forward received messages to. */
    t_symbol        *name; /**< The receive-symbol to bind to. */
} t_pdlua_proxyreceive;

/** Proxy receive 'anything' method. */
static void pdlua_proxyreceive_anything(
    t_pdlua_proxyreceive    *r, /**< The proxy receive that received the message. */
    t_symbol                *s, /**< The message selector. */
    int                     argc, /**< The message length. */
    t_atom                  *argv /**< The atoms in the message. */
)
{
    pdlua_receivedispatch(r, s, argc, argv);
}

/** Proxy receive allocation and initialization. */
static t_pdlua_proxyreceive *pdlua_proxyreceive_new
(
    struct pdlua    *owner, /**< The owning object. */
    t_symbol        *name /**< The symbol to bind to. */
)
{
    t_pdlua_proxyreceive *r = malloc(sizeof(t_pdlua_proxyreceive));
    r->pd = pdlua_proxyreceive_class;
    r->owner = owner;
    r->name = name;
    pd_bind(&r->pd, r->name);
    return r;
}

/** Proxy receive cleanup and deallocation. */
static void pdlua_proxyreceive_free(t_pdlua_proxyreceive *r /**< The proxy receive to free. */)
{
    pd_unbind(&r->pd, r->name);
    r->pd = NULL;
    r->owner = NULL;
    r->name = NULL;
    free(r);
}

/** Register the proxy receive class with Pd. */
static void pdlua_proxyreceive_setup()
{
    pdlua_proxyreceive_class = class_new(gensym("pdlua proxy receive"), 0, 0, sizeof(t_pdlua_proxyreceive), 0, 0);
    class_addanything(pdlua_proxyreceive_class, pdlua_proxyreceive_anything);
}


/** Proxy clock class pointer. */
static t_class *pdlua_proxyclock_class;

/** Proxy clock object data. */
typedef struct pdlua_proxyclock
{
    t_pd            pd; /**< Minimal Pd object. */
    struct pdlua    *owner; /**< Object to forward messages to. */
    t_clock         *clock; /** Pd clock to use. */
} t_pdlua_proxyclock;

/** Proxy clock 'bang' method. */
static void pdlua_proxyclock_bang(t_pdlua_proxyclock *c /**< The proxy clock that received the message. */)
{
    pdlua_clockdispatch(c);
}

/** Proxy clock allocation and initialization. */
static t_pdlua_proxyclock *pdlua_proxyclock_new
(
    struct pdlua *owner /**< The object to forward messages to. */
)
{
    t_pdlua_proxyclock *c = malloc(sizeof(t_pdlua_proxyclock));
    c->pd = pdlua_proxyclock_class;
    c->owner = owner;
    c->clock = clock_new(c, (t_method) pdlua_proxyclock_bang);
    return c;
}

/** Register the proxy clock class with Pd. */
static void pdlua_proxyclock_setup(void)
{
    pdlua_proxyclock_class = class_new(gensym("pdlua proxy clock"), 0, 0, sizeof(t_pdlua_proxyclock), 0, 0);
}

/** Dump an array of atoms into a Lua table. */
static void pdlua_pushatomtable
(
    int     argc, /**< The number of atoms in the array. */
    t_atom  *argv /**< The array of atoms. */
)
{
    int i;

    lua_newtable(L);
    for (i = 0; i < argc; ++i)
    {
        lua_pushnumber(L, i+1);
        switch (argv[i].a_type)
        {
            case A_FLOAT:
                lua_pushnumber(L, argv[i].a_w.w_float);
                break;
            case A_SYMBOL:
                lua_pushstring(L, argv[i].a_w.w_symbol->s_name);
                break;
            case A_POINTER: /* FIXME: check experimentality */
                lua_pushlightuserdata(L, argv[i].a_w.w_gpointer);
            break;
            default:
                error("lua: zomg weasels!");
                lua_pushnil(L);
            break;
        }
        lua_settable(L, -3);
    }
}

/** Pd object constructor. */
static t_pdlua *pdlua_new
(
    t_symbol    *s, /**< The construction message selector. */
    int         argc, /**< The construction message atom count. */
    t_atom      *argv /**< The construction message atoms. */
)
{
    int i;
#ifdef PDLUA_DEBUG
    post("pdlua_new: s->s_name is %s", s->s_name);
#endif // PDLUA_DEBUG
    for (i = 0; i < argc; ++i)
    {
        switch (argv[i].a_type)
        {
        case A_FLOAT:
#ifdef PDLUA_DEBUG
            post("argv[%d]: %f", i, argv[i].a_w.w_float);
#endif // PDLUA_DEBUG
            break;
        case A_SYMBOL:
#ifdef PDLUA_DEBUG
            post("argv[%d]: %s", i, argv[i].a_w.w_symbol->s_name);
#endif // PDLUA_DEBUG
            break;
        default:
            error("pdlua_new: bad argument type"); // should never happen
            return NULL;
        }
    }
    lua_getglobal(L, "pd");
    lua_getfield(L, -1, "_constructor");
    lua_pushstring(L, s->s_name);
    pdlua_pushatomtable(argc, argv);
    if (lua_pcall(L, 2, 1, 0))
    {
        error("pdlua_new: error in constructor for `%s':\n%s", s->s_name, lua_tostring(L, -1));
        lua_pop(L, 1);
        return NULL;
    }
    else
    {
        t_pdlua *object = NULL;
#ifdef PDLUA_DEBUG
        post("pdlua_new: done lua_pcall(L, 2, 1, 0)");
#endif // PDLUA_DEBUG
        if (lua_islightuserdata(L, -1))
        {
            object = lua_touserdata(L, -1);
            lua_pop(L, 1);
#ifdef PDLUA_DEBUG
            post("pdlua_new: TRUE lua_islightuserdata(L, -1) object = %p", object);
#endif // PDLUA_DEBUG
            return object;
        }
        else
        {
            lua_pop(L, 1);
#ifdef PDLUA_DEBUG
            post("pdlua_new: done FALSE lua_islightuserdata(L, -1)");
#endif // PDLUA_DEBUG
            return NULL;
        }
    }
}

/** Pd object destructor. */
static void pdlua_free( t_pdlua *o /**< The object to destruct. */)
{
    lua_getglobal(L, "pd");
    lua_getfield (L, -1, "_destructor");
    lua_pushlightuserdata(L, o);
    if (lua_pcall(L, 1, 0, 0))
    {
        error("lua: error in destructor:\n%s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    return;
}

/** Lua class registration. */
static int pdlua_class_new(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Class name string.
  * \par Outputs:
  * \li \c 1 Pd class pointer.
  * */
{
    const char  *name;
    t_class     *c;

#ifdef PDLUA_DEBUG
    post("pdlua_class_new:");
#endif // PDLUA_DEBUG
    name = luaL_checkstring(L, 1);
    c = class_new(gensym((char *) name), (t_newmethod) pdlua_new,
        (t_method) pdlua_free, sizeof(t_pdlua), CLASS_NOINLET, A_GIMME, 0);
    lua_pushlightuserdata(L, c);
    return 1;
}

/** Lua object creation. */
static int pdlua_object_new(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd class pointer.
  * \par Outputs:
  * \li \c 2 Pd object pointer.
  * */
{
#ifdef PDLUA_DEBUG
    post("pdlua_object_new:");
#endif // PDLUA_DEBUG
    if (lua_islightuserdata(L, 1))
    {
        t_class *c = lua_touserdata(L, 1);
        if (c)
        {
            t_pdlua *o = (t_pdlua *) pd_new(c);
            if (o)
            {
                o->inlets = 0;
                o->in = NULL;
                o->outlets = 0;
                o->out = NULL;
                o->canvas = canvas_getcurrent();
                lua_pushlightuserdata(L, o);
                return 1;
            }
        }
    }
    return 0;
}

/** Lua object inlet creation. */
static int pdlua_object_createinlets(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd object pointer.
  * \li \c 2 Number of inlets.
  * */
{
    unsigned int i;

#ifdef PDLUA_DEBUG
    post("pdlua_object_createinlets:");
#endif // PDLUA_DEBUG
    if (lua_islightuserdata(L, 1))
    {
        t_pdlua *o = lua_touserdata(L, 1);
        if (o)
        {
            o->inlets = luaL_checknumber(L, 2);
            o->in = malloc(o->inlets * sizeof(t_pdlua_proxyinlet));
            for (i = 0; i < o->inlets; ++i)
            {
                pdlua_proxyinlet_init(&o->in[i], o, i);
                inlet_new(&o->pd, &o->in[i].pd, 0, 0);
            }
        }
    }
    return 0;
}

/** Lua object outlet creation. */
static int pdlua_object_createoutlets(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd object pointer.
  * \li \c 2 Number of outlets.
  * */
{
    unsigned int i;

#ifdef PDLUA_DEBUG
    post("pdlua_object_createoutlets:");
#endif // PDLUA_DEBUG
    if (lua_islightuserdata(L, 1))
    {
        t_pdlua *o = lua_touserdata(L, 1);
        if (o)
        {
            o->outlets = luaL_checknumber(L, 2);
            if (o->outlets > 0)
            {
                o->out = malloc(o->outlets * sizeof(t_outlet *));
                for (i = 0; i < o->outlets; ++i) o->out[i] = outlet_new(&o->pd, 0);
            }
            else o->out = NULL;
        }
    }
    return 0;
}

/** Lua object receive creation. */
static int pdlua_receive_new(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd object pointer.
  * \li \c 2 Receive name string.
  * \par Outputs:
  * \li \c 1 Pd receive pointer.
  * */
{
#ifdef PDLUA_DEBUG
    post("pdlua_receive_new:");
#endif // PDLUA_DEBUG
    if (lua_islightuserdata(L, 1))
    {
        t_pdlua *o = lua_touserdata(L, 1);
        if (o)
        {
            const char *name = luaL_checkstring(L, 2);
            if (name)
            {
                t_pdlua_proxyreceive *r =  pdlua_proxyreceive_new(o, gensym((char *) name)); /* const cast */
                lua_pushlightuserdata(L, r);
                return 1;
            }
        }
    }
    return 0;
}

/** Lua object receive destruction. */
static int pdlua_receive_free(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd recieve pointer.
  * */
{
#ifdef PDLUA_DEBUG
    post("pdlua_receive_free:");
#endif // PDLUA_DEBUG
    if (lua_islightuserdata(L, 1))
    {
        t_pdlua_proxyreceive *r = lua_touserdata(L, 1);
        if (r) pdlua_proxyreceive_free(r);
    }
    return 0;
}

/** Lua object clock creation. */
static int pdlua_clock_new(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd object pointer.
  * \par Outputs:
  * \li \c 1 Pd clock pointer.
  * */
{
    if (lua_islightuserdata(L, 1))
    {
        t_pdlua *o = lua_touserdata(L, 1);
        if (o)
        {
            t_pdlua_proxyclock *c =  pdlua_proxyclock_new(o);
            lua_pushlightuserdata(L, c);
            return 1;
        }
    }
    return 0;
}

/** Lua proxy clock delay. */
static int pdlua_clock_delay(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd clock pointer.
  * \li \c 2 Number of milliseconds to delay.
  * */
{
    if (lua_islightuserdata(L, 1))
    {
        t_pdlua_proxyclock *c = lua_touserdata(L, 1);
        if (c)
        {
            double delaytime = luaL_checknumber(L, 2);
            clock_delay(c->clock, delaytime);
        }
    }
    return 0;
}

/** Lua proxy clock set. */
static int pdlua_clock_set(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd clock pointer.
  * \li \c 2 Number to set the clock.
  * */
{
    if (lua_islightuserdata(L, 1))
    {
        t_pdlua_proxyclock *c = lua_touserdata(L, 1);
        if (c)
        {
            double systime = luaL_checknumber(L, 2);
            clock_set(c->clock, systime);
        }
    }
    return 0;
}

/** Lua proxy clock unset. */
static int pdlua_clock_unset(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd clock pointer.
  * */
{
    if (lua_islightuserdata(L, 1))
    {
        t_pdlua_proxyclock *c = lua_touserdata(L, 1);
        if (c) clock_unset(c->clock);
    }
    return 0;
}

/** Lua proxy clock destruction. */
static int pdlua_clock_free(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd clock pointer.
  * */
{
    if (lua_islightuserdata(L, 1))
    {
        t_pdlua_proxyclock *c = lua_touserdata(L, 1);
        if (c)
        {
            clock_free(c->clock);
            free(c);
        }
    }
    return 0;
}

/** Lua object destruction. */
static int pdlua_object_free(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd object pointer.
  * */
{
    unsigned int i;

#ifdef PDLUA_DEBUG
    post("pdlua_object_free:");
#endif // PDLUA_DEBUG
    if (lua_islightuserdata(L, 1))
    {
        t_pdlua *o = lua_touserdata(L, 1);
        if (o)
        {
            if (o->in) free(o->in);
            if(o->out)
            {
                for (i = 0; i < o->outlets; ++i) outlet_free(o->out[i]);
                free(o->out);
                o->out = NULL;
            }
        }
    }
    return 0;
}

/** Dispatch Pd inlet messages to Lua objects. */
static void pdlua_dispatch
(
    t_pdlua         *o, /**< The object that received the message. */
    unsigned int    inlet, /**< The inlet that the message arrived at. */
    t_symbol        *s, /**< The message selector. */
    int             argc, /**< The message length. */
    t_atom          *argv /**< The atoms in the message. */
)
{
#ifdef PDLUA_DEBUG
    post("pdlua_dispatch:");
#endif // PDLUA_DEBUG
    lua_getglobal(L, "pd");
    lua_getfield (L, -1, "_dispatcher");
    lua_pushlightuserdata(L, o);
    lua_pushnumber(L, inlet + 1); /* C has 0.., Lua has 1.. */
    lua_pushstring(L, s->s_name);
    pdlua_pushatomtable(argc, argv);
    if (lua_pcall(L, 4, 0, 0))
    {
        pd_error(o, "lua: error in dispatcher:\n%s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    return;  
}

/** Dispatch Pd receive messages to Lua objects. */
static void pdlua_receivedispatch
(
    t_pdlua_proxyreceive    *r, /**< The proxy receive that received the message. */
    t_symbol                *s, /**< The message selector. */
    int                     argc, /**< The message length. */
    t_atom                  *argv /**< The atoms in the message. */
)
{
#ifdef PDLUA_DEBUG
    post("pdlua_receivedispatch:");
#endif // PDLUA_DEBUG
    lua_getglobal(L, "pd");
    lua_getfield (L, -1, "_receivedispatch");
    lua_pushlightuserdata(L, r);
    lua_pushstring(L, s->s_name);
    pdlua_pushatomtable(argc, argv);
    if (lua_pcall(L, 3, 0, 0))
    {
        pd_error(r->owner, "lua: error in receive dispatcher:\n%s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    return;  
}

/** Dispatch Pd clock messages to Lua objects. */
static void pdlua_clockdispatch( t_pdlua_proxyclock *clock)
/**< The proxy clock that received the message. */
{
    lua_getglobal(L, "pd");
    lua_getfield (L, -1, "_clockdispatch");
    lua_pushlightuserdata(L, clock);
    if (lua_pcall(L, 1, 0, 0))
    {
        pd_error(clock->owner, "lua: error in clock dispatcher:\n%s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    return;  
}

/** Convert a Lua table into a Pd atom array. */
static t_atom *pdlua_popatomtable
(
    lua_State   *L, /**< Lua interpreter state.
  * \par Inputs:
  * \li \c -1 Table to convert.
  * */
    int         *count, /**< Where to store the array length. */
    t_pdlua     *o /**< Object reference for error messages. */
)
{
    int         i;
    int         ok = 1;
    t_float     f;
    const char  *s;
    void        *p;
    size_t      sl;
    t_atom      *atoms = NULL;

    if (lua_istable(L, -1))
    {
        *count = lua_objlen(L, -1);
        if (*count > 0) atoms = malloc(*count * sizeof(t_atom));
        i = 0;
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            if (i == *count)
            {
                pd_error(o, "lua: error: too many table elements");
                ok = 0;
                break;
            }
            switch (lua_type(L, -1))
            {
                case (LUA_TNUMBER):
                    f = lua_tonumber(L, -1);
                    SETFLOAT(&atoms[i], f);
                    break;
                case (LUA_TSTRING):
                    s = lua_tolstring(L, -1, &sl);
                    if (s)
                    {
                        if (strlen(s) != sl) pd_error(o, "lua: warning: symbol munged (contains \\0 in body)");
                        SETSYMBOL(&atoms[i], gensym((char *) s));
                    }
                    else
                    {
                        pd_error(o, "lua: error: null string in table");
                        ok = 0;
                    }
                    break;
                case (LUA_TLIGHTUSERDATA): /* FIXME: check experimentality */
                    p = lua_touserdata(L, -1);
                    SETPOINTER(&atoms[i], p);
                    break;
                default:
                    pd_error(o, "lua: error: table element must be number or string or pointer");
                    ok = 0;
                    break;
            }
            lua_pop(L, 1);
            ++i;
        }
        if (i != *count)
        {
            pd_error(o, "lua: error: too few table elements");
            ok = 0;
        }
    }
    else 
    {
        pd_error(o, "lua: error: not a table");
        ok = 0;
    }
    lua_pop(L, 1);
    if (ok) return atoms;
    if (atoms) free(atoms);
    return NULL;
}

/** Send a message from a Lua object outlet. */
static int pdlua_outlet(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd object pointer.
  * \li \c 2 Outlet number.
  * \li \c 3 Message selector string.
  * \li \c 4 Message atom table.
  * */
{
    t_pdlua         *o;
    int             out;
    size_t          sl;
    const char      *s;
    t_symbol        *sym;
    int             count;
    t_atom          *atoms;

#ifdef PDLUA_DEBUG
    post("pdlua_outlet:");
#endif // PDLUA_DEBUG
    if (lua_islightuserdata(L, 1))
    {
        o = lua_touserdata(L, 1);
        if (o) 
        {
            if (lua_isnumber(L, 2)) out = lua_tonumber(L, 2) - 1; /* C has 0.., Lua has 1.. */
            else
            {
                pd_error(o, "lua: error: outlet must be a number");
                return 0;
            }
            if (0 <= out && out < o->outlets) 
            {
                if (lua_isstring(L, 3)) 
                {
                    s = lua_tolstring(L, 3, &sl);
                    sym = gensym((char *) s); /* const cast */
                    if (s)
                    {
                        if (strlen(s) != sl) pd_error(o, "lua: warning: symbol munged (contains \\0 in body)");
                        lua_pushvalue(L, 4);
                        atoms = pdlua_popatomtable(L, &count, o);
                        if (count == 0 || atoms) outlet_anything(o->out[out], sym, count, atoms);
                        else pd_error(o, "lua: error: no atoms??");
                        if (atoms) 
                        {
                            free(atoms);
                            return 0;
                        }
                    }
                    else pd_error(o, "lua: error: null selector");
                }
                else pd_error(o, "lua: error: selector must be a string");
            }
            else pd_error(o, "lua: error: outlet out of range");
        }
        else error("lua: error: no object to outlet from");
    }
    else error("lua: error: bad arguments to outlet");
    return 0;
}

/** Send a message from a Lua object to a Pd receiver. */
static int pdlua_send(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Receiver string.
  * \li \c 2 Message selector string.
  * \li \c 3 Message atom table.
  * */
{
    size_t          receivenamel;
    const char      *receivename;
    t_symbol        *receivesym;
    size_t          selnamel;
    const char      *selname;
    t_symbol        *selsym;
    int             count;
    t_atom          *atoms;

#ifdef PDLUA_DEBUG
    post("pdlua_send:");
#endif // PDLUA_DEBUG
    if (lua_isstring(L, 1)) 
    {
        receivename = lua_tolstring(L, 1, &receivenamel);
        receivesym = gensym((char *) receivename); /* const cast */
        if (receivesym) 
        {
            if (strlen(receivename) != receivenamel) error("lua: warning: symbol munged (contains \\0 in body)");
            if (lua_isstring(L, 2)) 
            {
                selname = lua_tolstring(L, 2, &selnamel);
                selsym = gensym((char *) selname); /* const cast */
                if (selsym)
                {
                    if (strlen(selname) != selnamel) error("lua: warning: symbol munged (contains \\0 in body)");
                    lua_pushvalue(L, 3);
                    atoms = pdlua_popatomtable(L, &count, NULL);
                    if ((count == 0 || atoms) && (receivesym->s_thing)) typedmess(receivesym->s_thing, selsym, count, atoms);
                    else error("lua: error: no atoms??");
                    if (atoms) 
                    {
                        free(atoms);
                        return 0;
                    }
                }
                else error("lua: error: null selector");
            }
            else error("lua: error: selector must be a string");
        }
        else error("lua: error: null receive name");
    }
    else error("lua: error: receive name must be string");
    return 0;
}

/** Set a [value] object's value. */
static int pdlua_setvalue(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Value name string.
  * \li \c 2 Value number.
  * \par Outputs:
  * \li \c 1 success (usually depends on a [value] existing or not).
  */
{
    const char  *str = luaL_checkstring(L, 1);
    t_float     val = luaL_checknumber(L, 2);
    int         err = value_setfloat(gensym(str), val);

    lua_pushboolean(L, !err);
    return 1;
}

/** Get a [value] object's value. */
static int pdlua_getvalue(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Value name string.
  * \par Outputs:
  * \li \c 1 Value number, or nil for failure.
  * */
{
    const char  *str = luaL_checkstring(L, 1);
    t_float     val;
    int         err = value_getfloat(gensym(str), &val);

    if (!err) lua_pushnumber(L, val);
    else lua_pushnil(L);
    return 1;
}

/** Get a [table] object's array. */
static int pdlua_getarray(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Table name string.
  * \par Outputs:
  * \li \c 1 Table length, or < 0 for failure.
  * \li \c 2 Table pointer, or nil for failure.
  * */
{
    t_garray        *a;
    int             n;
    PDLUA_ARRAYTYPE *v;
    const char      *str = luaL_checkstring(L, 1);

    if (!(a = (t_garray *) pd_findbyclass(gensym(str), garray_class))) 
    {
        lua_pushnumber(L, -1);
        return 1;
    }
    else if (!PDLUA_ARRAYGRAB(a, &n, &v)) 
    {
        lua_pushnumber(L, -2);
        return 1;
    }
    else 
    {
        lua_pushnumber(L, n);
        lua_pushlightuserdata(L, v);
        return 2;
    }
}

/** Read from a [table] object's array. */
static int pdlua_readarray(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Table length number.
  * \li \c 2 Table array pointer.
  * \li \c 3 Table index number.
  * \par Outputs:
  * \li \c 1 Table element value, or nil for index out of range.
  * */
{
    int             n = luaL_checknumber(L, 1);
    PDLUA_ARRAYTYPE *v = lua_islightuserdata(L, 2) ? lua_touserdata(L, 2) : NULL;
    int             i = luaL_checknumber(L, 3);

    if (0 <= i && i < n && v) 
    {
        lua_pushnumber(L, PDLUA_ARRAYELEM(v, i));
        return 1;
    }
    return 0;
}

/** Write to a [table] object's array. */
static int pdlua_writearray(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Table length number.
  * \li \c 2 Table array pointer.
  * \li \c 3 Table index number.
  * \li \c 4 Table element value number.
  * */
{
    int                 n = luaL_checknumber(L, 1);
    PDLUA_ARRAYTYPE     *v = lua_islightuserdata(L, 2) ? lua_touserdata(L, 2) : NULL;
    int                 i = luaL_checknumber(L, 3);
    t_float             x = luaL_checknumber(L, 4);

    if (0 <= i && i < n && v) PDLUA_ARRAYELEM(v, i) = x;
    return 0;
}

/** Redraw a [table] object's graph. */
static int pdlua_redrawarray(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Table name string.
  * */
{
    t_garray    *a;
    const char  *str = luaL_checkstring(L, 1);

    if ((a = (t_garray *) pd_findbyclass(gensym(str), garray_class))) garray_redraw(a);
    return 0;
}

/** Post to Pd's console. */
static int pdlua_post(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Message string.
  * */
{
    const char *str = luaL_checkstring(L, 1);
    post("%s", str);
    return 0;
}

/** Report an error from a Lua object to Pd's console. */
static int pdlua_error(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd object pointer.
  * \li \c 2 Message string.
  * */
{
    t_pdlua     *o;
    const char  *s;

    if (lua_islightuserdata(L, 1))
    {
        o = lua_touserdata(L, 1);
        if (o)
        {
            s = luaL_checkstring(L, 2);
            if (s) pd_error(o, "%s", s);
            else pd_error(o, "lua: error: null string in error function");
        }
        else error("lua: error: null object in error function");
    }
    else error("lua: error: bad arguments to error function");
    return 0;
}

void pdlua_setrequirepath
( /* FIXME: documentation */
    lua_State   *L,
    const char  *path
)
{
    lua_getglobal(L, "pd");
    lua_pushstring(L, "_setrequirepath");
    lua_gettable(L, -2);
    lua_pushstring(L, path);
    if (lua_pcall(L, 1, 0, 0) != 0)
    {
        error("lua: internal error in `pd._setrequirepath': %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

void pdlua_clearrequirepath
( /* FIXME: documentation */
    lua_State *L
)
{
    lua_getglobal(L, "pd");
    lua_pushstring(L, "_clearrequirepath");
    lua_gettable(L, -2);
    if (lua_pcall(L, 0, 0, 0) != 0)
    {
        error("lua: internal error in `pd._clearrequirepath': %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

/** Run a Lua script using Pd's path. */
static int pdlua_dofile(lua_State *L)
/**< Lua interpreter state.
  * \par Inputs:
  * \li \c 1 Pd object pointer.
  * \li \c 2 Filename string.
  * \par Outputs:
  * \li \c * Determined by the script.
  * */
{
    char                buf[MAXPDSTRING];
    char                *ptr;
    t_pdlua_readerdata  reader;
    int                 fd;
    int                 n;
    const char          *filename;
    t_pdlua             *o;

#ifdef PDLUA_DEBUG
    post("pdlua_dofile:");
#endif // PDLUA_DEBUG
    n = lua_gettop(L);
    if (lua_islightuserdata(L, 1))
    {
        o = lua_touserdata(L, 1);
        if (o)
        {
            filename = luaL_optstring(L, 2, NULL);
            fd = canvas_open(o->canvas, filename, "", buf, &ptr, MAXPDSTRING, 1);
            if (fd >= 0)
            {
                pdlua_setrequirepath(L, buf);
                reader.fd = fd;
                if (lua_load(L, pdlua_reader, &reader, filename))
                {
                    close(fd);
                    pdlua_clearrequirepath(L);
                    lua_error(L);
                }
                else
                {
                    if (lua_pcall(L, 0, LUA_MULTRET, 0))
                    {
                        pd_error(o, "lua: error running `%s':\n%s", filename, lua_tostring(L, -1));
                        lua_pop(L, 1);
                        close(fd);
                        pdlua_clearrequirepath(L);
                    }
                    else
                    {
                        /* succeeded */
                        close(fd);
                        pdlua_clearrequirepath(L);
                    }
                }
            }
            else pd_error(o, "lua: error loading `%s': canvas_open() failed", filename);
        }
        else error("lua: error in object:dofile() - object is null");
    }
    else error("lua: error in object:dofile() - object is wrong type");
    return lua_gettop(L) - n;
}

/** Initialize the pd API for Lua. */
static void pdlua_init(lua_State *L)
/**< Lua interpreter state. */
{
    lua_newtable(L);
    lua_setglobal(L, "pd");
    lua_getglobal(L, "pd");
    lua_pushstring(L, "_iswindows");
#ifdef MSW
    lua_pushboolean(L, 1);
#else
    lua_pushboolean(L, 0);
#endif
    lua_settable(L, -3);
    lua_pushstring(L, "_register");
    lua_pushcfunction(L, pdlua_class_new);
    lua_settable(L, -3);
    lua_pushstring(L, "_create");
    lua_pushcfunction(L, pdlua_object_new);
    lua_settable(L, -3);
    lua_pushstring(L, "_createinlets");
    lua_pushcfunction(L, pdlua_object_createinlets);
    lua_settable(L, -3);
    lua_pushstring(L, "_createoutlets");
    lua_pushcfunction(L, pdlua_object_createoutlets);
    lua_settable(L, -3);
    lua_pushstring(L, "_destroy");
    lua_pushcfunction(L, pdlua_object_free);
    lua_settable(L, -3);
    lua_pushstring(L, "_outlet");
    lua_pushcfunction(L, pdlua_outlet);
    lua_settable(L, -3);
    lua_pushstring(L, "_createreceive");
    lua_pushcfunction(L, pdlua_receive_new);
    lua_settable(L, -3);
    lua_pushstring(L, "_receivefree");
    lua_pushcfunction(L, pdlua_receive_free);
    lua_settable(L, -3);
    lua_pushstring(L, "_createclock");
    lua_pushcfunction(L, pdlua_clock_new);
    lua_settable(L, -3);
    lua_pushstring(L, "_clockfree");
    lua_pushcfunction(L, pdlua_clock_free);
    lua_settable(L, -3);
    lua_pushstring(L, "_clockset");
    lua_pushcfunction(L, pdlua_clock_set);
    lua_settable(L, -3);
    lua_pushstring(L, "_clockunset");
    lua_pushcfunction(L, pdlua_clock_unset);
    lua_settable(L, -3);
    lua_pushstring(L, "_clockdelay");
    lua_pushcfunction(L, pdlua_clock_delay);
    lua_settable(L, -3);
    lua_pushstring(L, "_dofile");
    lua_pushcfunction(L, pdlua_dofile);
    lua_settable(L, -3);
    lua_pushstring(L, "send");
    lua_pushcfunction(L, pdlua_send);
    lua_settable(L, -3);
    lua_pushstring(L, "getvalue");
    lua_pushcfunction(L, pdlua_getvalue);
    lua_settable(L, -3);
    lua_pushstring(L, "setvalue");
    lua_pushcfunction(L, pdlua_setvalue);
    lua_settable(L, -3);
    lua_pushstring(L, "_getarray");
    lua_pushcfunction(L, pdlua_getarray);
    lua_settable(L, -3);
    lua_pushstring(L, "_readarray");
    lua_pushcfunction(L, pdlua_readarray);
    lua_settable(L, -3);
    lua_pushstring(L, "_writearray");
    lua_pushcfunction(L, pdlua_writearray);
    lua_settable(L, -3);
    lua_pushstring(L, "_redrawarray");
    lua_pushcfunction(L, pdlua_redrawarray);
    lua_settable(L, -3);
    lua_pushstring(L, "post");
    lua_pushcfunction(L, pdlua_post);
    lua_settable(L, -3);
    lua_pushstring(L, "_error");
    lua_pushcfunction(L, pdlua_error);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

/** Pd loader hook for loading and executing Lua scripts. */
static int pdlua_loader
(
    t_canvas    *canvas, /**< Pd canvas to use to find the script. */
    char        *name /**< The name of the script (without .pd_lua extension). */
)
{
    char                dirbuf[MAXPDSTRING];
    char                *ptr;
    int                 fd;
    t_pdlua_readerdata  reader;

#ifdef PDLUA_DEBUG
    post("pdlua_loader:");
#endif // PDLUA_DEBUG
    fd = canvas_open(canvas, name, ".pd_lua", dirbuf, &ptr, MAXPDSTRING, 1);
    if (fd >= 0)
    {
        class_set_extern_dir(gensym(dirbuf));
        pdlua_setrequirepath(L, dirbuf);
        reader.fd = fd;
        if (lua_load(L, pdlua_reader, &reader, name) || lua_pcall(L, 0, 0, 0))
        {
            error("lua: error loading `%s':\n%s", name, lua_tostring(L, -1));
            lua_pop(L, 1);
            close(fd);
            pdlua_clearrequirepath(L);
            class_set_extern_dir(&s_);
            return 0;
        }
        close(fd);
        pdlua_clearrequirepath(L);
        class_set_extern_dir(&s_);
        return 1;
    }
    return 0;
}

/** Start the Lua runtime and register our loader hook. */
//EXPORT void lua_setup(void) {
void pdlua_setup(void)
{
    char                buf[MAXPDSTRING];
    char                *ptr;
    t_pdlua_readerdata  reader;
    int                 fd;
    int                 result;

    post("pdlua 0.6 (GPL) 2011 Martin Peach, based on");
    post("lua 0.6~svn (GPL) 2008 Claude Heiland-Allen <claudiusmaximus@goto10.org>");
    post("pdlua: compiled for pd-%d.%d on %s %s", PD_MAJOR_VERSION, PD_MINOR_VERSION, __DATE__, __TIME__);
    pdlua_proxyinlet_setup();
#ifdef PDLUA_DEBUG
    post("pdlua pdlua_proxyinlet_setup done");
#endif // PDLUA_DEBUG
    pdlua_proxyreceive_setup();
#ifdef PDLUA_DEBUG
    post("pdlua pdlua_proxyreceive_setup done");
#endif // PDLUA_DEBUG
    pdlua_proxyclock_setup();
#ifdef PDLUA_DEBUG
    post("pdlua pdlua_proxyclock_setup done");
#endif // PDLUA_DEBUG
    L = lua_open();
#ifdef PDLUA_DEBUG
    post("pdlua lua_open done L = %p", L);
#endif // PDLUA_DEBUG
    luaL_openlibs(L);
#ifdef PDLUA_DEBUG
    post("pdlua luaL_openlibs done");
#endif // PDLUA_DEBUG
    pdlua_init(L);
#ifdef PDLUA_DEBUG
    post("pdlua pdlua_init done");
#endif // PDLUA_DEBUG
    /* "pd.lua" is the Lua part of pdlua, want to keep the C part minimal */
    fd = canvas_open(0, "pd", ".lua", buf, &ptr, MAXPDSTRING, 1);
#ifdef PDLUA_DEBUG
    post ("pd.lua loaded from %s", buf);
    post("pdlua canvas_open done fd = %d", fd);
#endif // PDLUA_DEBUG
    if (fd >= 0)
    {
        reader.fd = fd;
        result = lua_load(L, pdlua_reader, &reader, "pd.lua");
#ifdef PDLUA_DEBUG
        post ("pdlua lua_load returned %d", result);
#endif // PDLUA_DEBUG
        if (0 == result)
        {
            result = lua_pcall(L, 0, 0, 0);
#ifdef PDLUA_DEBUG
            post ("pdlua lua_pcall returned %d", result);
#endif // PDLUA_DEBUG
        }
        if (0 != result)
        //if (lua_load(L, pdlua_reader, &reader, "pd.lua") || lua_pcall(L, 0, 0, 0))
        {
            error("lua: error loading `pd.lua':\n%s", lua_tostring(L, -1));
            error("lua: loader will not be registered!");
            error("lua: (is `pd.lua' in Pd's path list?)");
            lua_pop(L, 1);
            close(fd);
        }
        else
        {
            close(fd);
            sys_register_loader(pdlua_loader);
        }
    }
    else
    {
        error("lua: error loading `pd.lua': canvas_open() failed");
        error("lua: loader will not be registered!");
    }
}

/* EOF */