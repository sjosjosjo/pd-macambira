/* 

delsplit - split a delimited list-in-a-symbol

Copyright (c) 2002-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#define FLEXT_ATTRIBUTES 1

#include <flext.h>

#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 500)
#error You need at least flext version 0.5.0
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


#define VERSION "0.1.4"

#ifdef __MWERKS__
#define STD std
#else
#define STD 
#endif


class delsplit:
	public flext_base
{
	FLEXT_HEADER_S(delsplit,flext_base,Setup)

public:
	delsplit(int argc,const t_atom *argv);

protected:
	void m_list(const t_symbol *s);
    void m_del(const t_symbol *s,int argc,const t_atom *argv);	
	
	const t_symbol *delim;
	
	static void SetAtom(t_atom &l,const char *s);
private:
	static void Setup(t_classid c);

	static const t_symbol *sym__space;
	
	FLEXT_CALLBACK_S(m_list)
	FLEXT_CALLBACK_A(m_del)
    FLEXT_ATTRVAR_S(delim)
};

FLEXT_NEW_V("delsplit",delsplit)


const t_symbol *delsplit::sym__space = NULL;

void delsplit::Setup(t_classid c)
{
    sym__space = MakeSymbol(" ");

	FLEXT_CADDMETHOD(c,0,m_list);
	FLEXT_CADDMETHOD(c,1,m_del);
	FLEXT_CADDATTR_VAR1(c,"del",delim);
}

delsplit::delsplit(int argc,const t_atom *argv):
	delim(sym__)
{ 
	AddInAnything("Symbol in, representing the delimited list");
	AddInAnything("Set the Delimiter");
	AddOutList("The split list");

	m_del(sym_list,argc,argv);
}


/** \brief check whether string represents a number
	\ret 0..integer, 1..float, -1..no number
*/
static int chknum(const char *s)
{
	int num = 0,pts = 0;
	for(const char *si = s; *s; ++s) {
		if(*s == '.') ++pts;
		else if(isdigit(*s)) ++num;
		else { num = 0; break; }
	}
	return (num > 0 && pts <= 1)?pts:-1;
}

void delsplit::m_del(const t_symbol *s,int argc,const t_atom *argv) 
{	
    delim = NULL;
    if(s == sym_symbol) {
        FLEXT_ASSERT(argc == 1 && IsSymbol(argv[0]));
        delim = GetSymbol(argv[0]);
    }
    else if(s == sym_list) {
        if(argc == 0)
            delim = sym__space;
        else if(argc >= 1 && IsSymbol(argv[0]))
            delim = GetSymbol(argv[0]);
    }

    if(!delim) {
        post("%s - Argument must be a symbol, list or int/float/bang",thisName());
        delim = sym__space;
    }
}

void delsplit::SetAtom(t_atom &l,const char *s)
{
	int n = chknum(s);

	if(n < 0)
		SetString(l,s);
	else if(n == 0)
		SetInt(l,atoi(s));
	else
		SetFloat(l,(float)atof(s));
}

void delsplit::m_list(const t_symbol *sym)
{
    FLEXT_ASSERT(delim);
    
    t_atom lst[256];
	int cnt = 0;
	const char *sdel = GetString(delim);
	int ldel = strlen(sdel);
	char str[1024];
	strcpy(str,GetString(sym));
	
	for(const char *s = str; *s; ) {
		char *e = strstr(s,sdel);
		if(!e) {
			SetAtom(lst[cnt++],s);
			break;
		}
		else {
			*e = 0;
			SetAtom(lst[cnt++],s);
			s = e+ldel;
		}
	}
	
	ToOutList(0,cnt,lst);
}
