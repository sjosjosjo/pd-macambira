/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flattr.cpp
    \brief Attribute handling for the flext base class
*/
 
#include "flext.h"
#include <string.h>

#ifdef __MWERKS__
#define STD std
#else
#define STD
#endif

flext_base::attritem::attritem(const t_symbol *t,metharg tp,methfun f,int fl):
	item(t,0,NULL),
	flags(fl),argtp(tp),fun(f)
{}

flext_base::attritem::~attritem()
{
//	if(nxt) delete nxt;
}

/*
void flext_base::AddAttrItem(attritem *m)
{
	int ix = m->Hash();
	post("attr index %x",ix);
	attritem *&aix = attrhead[ix];

	if(aix) {
		attritem *mi;
		for(mi = aix; mi->nxt; mi = mi->nxt) {}
		mi->nxt = m;
	}
	else 
		aix = m;
//	m->th->attrcnt++;
}
*/

//! Add get and set attributes
void flext_base::AddAttrib(itemarr *aa,itemarr *ma,const char *attr,metharg tp,methfun gfun,methfun sfun)
{
	const t_symbol *asym = MakeSymbol(attr);

//	if(sfun) // if commented out, there will be a warning at run-time (more user-friendly)
	{
		attritem *a = new attritem(asym,tp,sfun,attritem::afl_bothexist|attritem::afl_set);
		aa->Add(a); 

		// bind attribute to a method
		methitem *mi = new methitem(0,asym,a);
		mi->SetArgs(sfun,1,&tp);
		ma->Add(mi);
	}

//	if(gfun) // if commented out, there will be a warning at run-time (more user-friendly)
	{
		attritem *a = new attritem(asym,tp,gfun,attritem::afl_bothexist|attritem::afl_get);
		aa->Add(a); 

		static char tmp[256] = "get";
		strcpy(tmp+3,attr);

		// bind attribute to a method
		methitem *mi = new methitem(0,MakeSymbol(tmp),a);
		mi->SetArgs(gfun,0,NULL);
		ma->Add(mi);
	}
}

void flext_base::AddAttrib(const char *attr,metharg tp,methfun gfun,methfun sfun)
{
	if(procattr)
		AddAttrib(ThAttrs(),ThMeths(),attr,tp,gfun,sfun);
	else
		error("%s - attribute procession is not enabled!",thisName());
}

void flext_base::AddAttrib(t_classid c,const char *attr,metharg tp,methfun gfun,methfun sfun)
{
	AddAttrib(ClAttrs(c),ClMeths(c),attr,tp,gfun,sfun);
}

int flext_base::ListAttr(AtomList &la) const
{
	int cnt = attrhead?attrhead->Count():0;
	int ccnt = clattrhead?clattrhead->Count():0;
	la(ccnt+cnt);

	int ix = 0;
	for(int i = 0; i <= 1; ++i) {
		itemarr *a = i?attrhead:clattrhead;
		if(a) {
			for(int ai = 0; ai < a->Size(); ++ai) {
				for(item *l = a->Item(ai); l; l = l->nxt) 
				{
					attritem *aa = (attritem *)l;
					if(!aa->BothExist() || aa->IsGet())
						SetSymbol(la[ix++],aa->tag);
				}
			}
		}
	}
	return ix;
}


int flext_base::CheckAttrib(int argc,const t_atom *argv)
{
	int offs = 0;
	for(; offs < argc; ++offs)
		if(IsString(argv[offs]) && *GetString(argv[offs]) == '@') break;
	return offs;
}

bool flext_base::InitAttrib(int argc,const t_atom *argv)
{
	int cur,nxt;
	for(cur = 0; cur < argc; cur = nxt) {
		// find next @symbol
		for(nxt = cur+1; nxt < argc; ++nxt)
			if(IsString(argv[nxt]) && *GetString(argv[nxt]) == '@') break;

		const t_symbol *tag = MakeSymbol(GetString(argv[cur])+1);
		SetAttrib(tag,nxt-cur-1,argv+cur+1);
	}
	return true;
}

bool flext_base::ListAttrib()
{
	if(procattr) {
		AtomList la;
		int c = ListAttr(la);
		ToOutAnything(GetOutAttr(),MakeSymbol("attributes"),c,la.Atoms());
		return true;
	}
	else
		return false;
}

flext_base::attritem *flext_base::FindAttr(const t_symbol *tag,bool get) const
{
	attritem *a = (attritem *)attrhead->Find(tag);
	while(a && (a->tag != tag || a->inlet != 0 || (get?a->IsSet():a->IsGet()))) a = (attritem *)a->nxt;
	if(!a) {
		a = (attritem *)clattrhead->Find(tag);	
		while(a && (a->tag != tag || a->inlet != 0 || (get?a->IsSet():a->IsGet()))) a = (attritem *)a->nxt;
	}
	return a;
}

bool flext_base::SetAttrib(const t_symbol *tag,int argc,const t_atom *argv)
{
	// search for matching attribute
	attritem *a = FindAttr(tag,false);
	if(a) 
		return SetAttrib(a,argc,argv);
	else {
		error("%s - %s: attribute not found",thisName(),GetString(tag));
		return true;
	}
}

bool flext_base::SetAttrib(attritem *a,int argc,const t_atom *argv)
{
	if(a->fun) {
		bool ok = true;

		AtomList la;
		t_any any;
		switch(a->argtp) {
		case a_float:
			if(argc == 1 && CanbeFloat(argv[0])) {
				any.ft = GetAFloat(argv[0]);
				((methfun_1)a->fun)(this,any);				
			}
			else ok = false;
			break;
		case a_int:
			if(argc == 1 && CanbeInt(argv[0])) {
				any.it = GetAInt(argv[0]);
				((methfun_1)a->fun)(this,any);				
			}
			else ok = false;
			break;
		case a_symbol:
			if(argc == 1 && IsSymbol(argv[0])) {
				any.st = GetSymbol(argv[0]);
				((methfun_1)a->fun)(this,any);				
			}
			else ok = false;
			break;
		case a_LIST:
			any.vt = &(la(argc,argv));
			((methfun_1)a->fun)(this,any);				
			break;
		default:
			ERRINTERNAL();
		}

		if(!ok)
			post("%s - wrong arguments for attribute %s",thisName(),GetString(a->tag));
	}
	else
		post("%s - attribute %s has no get method",thisName(),GetString(a->tag));
	return true;
}

/*
bool flext_base::GetAttrib(const t_symbol *tag)
{
	if(argc)
		post("%s - %s: arguments ignored",thisName(),GetString(tag));

#ifdef FLEXT_DEBUG
	if(strncmp(GetString(tag),"get",3)) {
		post("%s - %s: tag has no 'get' prefix",thisName(),GetString(tag));
		return false;
	}
#endif

	const t_symbol *mtag = MakeSymbol(GetString(a->tag)+3);

	// search for attribute
	attritem *a = (attritem *)attrhead->Find(mtag);
	if(!a) a = (attritem *)clattrhead->Find(mtag);	
}
*/

bool flext_base::GetAttrib(attritem *a)
{
	// main attribute tag
	if(a) {
		if(a->fun) {
			AtomList la;
			t_any any;
			switch(a->argtp) {
			case a_float: {
				((methfun_1)a->fun)(this,any);				
				la(1);
				SetFloat(la[0],any.ft);
				break;
			}
			case a_int: {
				((methfun_1)a->fun)(this,any);				
				la(1);
				SetInt(la[0],any.it);
				break;
			}
			case a_symbol: {
				((methfun_1)a->fun)(this,any);				
				la(1);
				SetSymbol(la[0],any.st);
				break;
			}
			case a_LIST: {
				any.vt = &la;
				((methfun_1)a->fun)(this,any);				
				break;
			}
			default:
				ERRINTERNAL();
			}
			ToOutAnything(GetOutAttr(),a->tag,la.Count(),la.Atoms());
		}
		else 
			post("%s - attribute %s has no set method",thisName(),GetString(a->tag));
	}
	else
		error("%s - %s: attribute not found",thisName(),GetString(a->tag));
	return true;
}

