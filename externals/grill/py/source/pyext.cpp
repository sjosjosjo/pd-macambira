/* 

py/pyext - python script object for PD and Max/MSP

Copyright (c)2002-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include "pyext.h"
#include <flinternal.h>

FLEXT_LIB_V("pyext pyext. pyx pyx.",pyext)


static const t_symbol *sym_get;

void pyext::Setup(t_classid c)
{
    sym_get = flext::MakeSymbol("get");
    
    FLEXT_CADDMETHOD_(c,0,"reload.",m_reload);
	FLEXT_CADDMETHOD_(c,0,"reload",m_reload_);
	FLEXT_CADDMETHOD_(c,0,"dir",m_dir);
	FLEXT_CADDMETHOD_(c,0,"dir+",m_dir_);
	FLEXT_CADDMETHOD_(c,0,"doc",m_doc);
	FLEXT_CADDMETHOD_(c,0,"doc+",m_doc_);

  	FLEXT_CADDATTR_VAR(c,"args",args,ms_args);
	FLEXT_CADDATTR_GET(c,"dir+",mg_dir_);

#ifdef FLEXT_THREADS
	FLEXT_CADDATTR_VAR1(c,"detach",detach);
	FLEXT_CADDMETHOD_(c,0,"stop",m_stop);
#endif

	FLEXT_CADDMETHOD_(c,0,"get",m_get);
	FLEXT_CADDMETHOD_(c,0,"set",m_set);

  	FLEXT_CADDATTR_VAR1(c,"respond",respond);

	// ----------------------------------------------------

	// register/initialize pyext base class along with module
	class_dict = PyDict_New();
	PyObject *className = PyString_FromString(PYEXT_CLASS);
	PyMethodDef *def;

	// add setattr/getattr to class 
	for(def = attr_tbl; def->ml_name; def++) {
			PyObject *func = PyCFunction_New(def, NULL);
			PyDict_SetItemString(class_dict, def->ml_name, func);
			Py_DECREF(func);
	}

	class_obj = PyClass_New(NULL, class_dict, className);
	Py_DECREF(className);

	// add methods to class 
	for (def = meth_tbl; def->ml_name != NULL; def++) {
		PyObject *func = PyCFunction_New(def, NULL);
		PyObject *method = PyMethod_New(func, NULL, class_obj); // increases class_obj ref count by 1
		PyDict_SetItemString(class_dict, def->ml_name, method);
		Py_DECREF(func);
		Py_DECREF(method);
	}

#if PY_VERSION_HEX >= 0x02020000
	// not absolutely necessary, existent in python 2.2 upwards
	// make pyext functions available in class scope
	PyDict_Merge(class_dict,module_dict,0);
#endif
	// after merge so that it's not in class_dict as well...
	PyDict_SetItemString(module_dict, PYEXT_CLASS,class_obj); // increases class_obj ref count by 1

	PyDict_SetItemString(class_dict,"__doc__",PyString_FromString(pyext_doc));
}

pyext *pyext::GetThis(PyObject *self)
{
	PyObject *th = PyObject_GetAttrString(self,"_this");
    if(th) {
	    pyext *ret = static_cast<pyext *>(PyLong_AsVoidPtr(th));
	    Py_DECREF(th);
        return ret;
    }
    else {
    	PyErr_Clear();
        return NULL;
    }
}

void pyext::SetThis()
{
	// remember the this pointer
	PyObject *th = PyLong_FromVoidPtr(this); 
	int ret = PyObject_SetAttrString(pyobj,"_this",th); // ref is taken
}


#if FLEXT_SYS == FLEXT_SYS_MAX
static short patcher_myvol(t_patcher *x)
{
    t_box *w;
    if (x->p_vol)
        return x->p_vol;
    else if (w = (t_box *)x->p_vnewobj)
        return patcher_myvol(w->b_patcher);
    else
        return 0;
}
#endif


PyObject *pyext::class_obj = NULL;
PyObject *pyext::class_dict = NULL;

pyext::pyext(int argc,const t_atom *argv):
	pyobj(NULL),pythr(NULL),
	inlets(-1),outlets(-1),
	methname(NULL)
{ 
    int apre = 0;

    if(argc >= apre+2 && CanbeInt(argv[apre]) && CanbeInt(argv[apre+1])) {
        inlets = GetAInt(argv[apre]);
        outlets = GetAInt(argv[apre+1]);
        apre += 2;
    }

    const t_atom *clname = NULL;

    PyThreadState *state = PyLock();

	// init script module
	if(argc > apre) {
		char dir[1024];

#if FLEXT_SYS == FLEXT_SYS_PD
		// add dir of current patch to path
		AddToPath(GetString(canvas_getdir(thisCanvas())));
		// add current dir to path
		AddToPath(GetString(canvas_getcurrentdir()));
#elif FLEXT_SYS == FLEXT_SYS_MAX 
        short path = patcher_myvol(thisCanvas());
        path_topathname(path,NULL,dir); 
		AddToPath(dir);       
#else 
        #pragma message("Adding current dir to path is not implemented")
#endif
        const t_atom &scr = argv[apre];

		if(!IsString(scr)) 
			post("%s - script name argument is invalid",thisName());
		else {
		    GetModulePath(GetString(scr),dir,sizeof(dir));
		    // add to path
		    AddToPath(dir);

			SetArgs(0,NULL);

			ImportModule(GetString(scr));
		}

        ++apre;

        // check for alias creation names
        if(strrchr(thisName(),'.')) clname = &scr;
	}

 	Register("_pyext");

	if(argc > apre || clname) {
        if(!clname) clname = &argv[apre++];
    
		// class name
		if(!IsString(*clname)) 
			post("%s - class name argument is invalid",thisName());
		else
			methname = GetSymbol(*clname);
	}

	if(argc > apre) args(argc-apre,argv+apre);

	if(methname) {
		MakeInstance();

        if(pyobj) {
            if(inlets >= 0) {
                // set number of inlets
			    PyObject *res = PyInt_FromLong(inlets);
                int ret = PyObject_SetAttrString(pyobj,"_inlets",res);
                FLEXT_ASSERT(!ret);
            }
            if(outlets >= 0) {
                // set number of outlets
			    PyObject *res = PyInt_FromLong(outlets);
			    int ret = PyObject_SetAttrString(pyobj,"_outlets",res);
                FLEXT_ASSERT(!ret);
            }

            DoInit(); // call __init__ constructor
            // __init__ can override the number of inlets and outlets

            if(inlets < 0) {
		        // get number of inlets
		        inlets = 1;
			    PyObject *res = PyObject_GetAttrString(pyobj,"_inlets"); // get ref
			    if(res) {
				    if(PyCallable_Check(res)) {
					    PyObject *fres = PyEval_CallObject(res,NULL);
					    Py_DECREF(res);
					    res = fres;
				    }
				    if(PyInt_Check(res)) 
					    inlets = PyInt_AsLong(res);
				    Py_DECREF(res);
			    }
			    else 
				    PyErr_Clear();
            }
            if(outlets < 0) {
                // get number of outlets
                outlets = 1;
			    PyObject *res = PyObject_GetAttrString(pyobj,"_outlets"); // get ref
			    if(res) {
				    if(PyCallable_Check(res)) {
					    PyObject *fres = PyEval_CallObject(res,NULL);
					    Py_DECREF(res);
					    res = fres;
				    }
				    if(PyInt_Check(res))
					    outlets = PyInt_AsLong(res);
				    Py_DECREF(res);
			    }
			    else
				    PyErr_Clear();
            }
        }
	}
    else 
        inlets = outlets = 0;

	PyUnlock(state);
	
    if(inlets < 0 || outlets < 0)
        InitProblem();
    else {
    	AddInAnything(1+inlets);  
	    AddOutAnything(outlets);  
    }

	if(!pyobj)
		InitProblem();
}

pyext::~pyext()
{
	PyThreadState *state = PyLock();

	ClearBinding();
    
    if(pyobj) {
        if(pyobj->ob_refcnt > 1) post("%s - Python object is still referenced",thisName());
    	Py_DECREF(pyobj);  // opposite of SetClssMeth
    }

    Unregister("_pyext");
	UnimportModule();

	PyUnlock(state);
}

bool pyext::DoInit()
{
    SetThis();

    // call init now, after _this has been set, which is
	// important for eventual callbacks from __init__ to c
	PyObject *pargs = MakePyArgs(NULL,args.Count(),args.Atoms(),-1,true);
	if(!pargs) PyErr_Print();

	PyObject *init = PyObject_GetAttrString(pyobj,"__init__"); // get ref
    if(init) {
        if(PyCallable_Check(init)) {
			PyObject *res = PyEval_CallObject(init,pargs);
			if(!res)
				PyErr_Print();
			else
				Py_DECREF(res);
        }
        Py_DECREF(init);
	}
    
	Py_XDECREF(pargs);
    return true;
}

bool pyext::MakeInstance()
{
	// pyobj should already have been decref'd / cleared before getting here!!
	
	if(module && methname) {
		PyObject *pref = PyObject_GetAttrString(module,const_cast<char *>(GetString(methname)));  
		if(!pref) 
			PyErr_Print();
        else {
            if(PyClass_Check(pref)) {
			    // make instance, but don't call __init__ 
			    pyobj = PyInstance_NewRaw(pref,NULL);

			    if(!pyobj) PyErr_Print();
            }
            else
			    post("%s - Type of \"%s\" is unhandled!",thisName(),GetString(methname));

		    Py_DECREF(pref);
		}
		return true;
	}
	else
		return false;
}

void pyext::Reload()
{
	ClearBinding();
	Py_XDECREF(pyobj);

	// by here, the Python class destructor should have been called!

	SetArgs(0,NULL);
	ReloadModule();
	
	MakeInstance();
}


void pyext::m_reload()
{
	PyThreadState *state = PyLock();

	Unregister("_pyext"); // self

	Reload();

	Reregister("_pyext"); // the others
	Register("_pyext"); // self

    SetThis();

	PyUnlock(state);
}

void pyext::m_reload_(int argc,const t_atom *argv)
{
	args(argc,argv);
	m_reload();
}

void pyext::m_get(const t_symbol *s)
{
    PyThreadState *state = PyLock();

	PyObject *pvar  = PyObject_GetAttrString(pyobj,const_cast<char *>(GetString(s))); /* fetch bound method */
	if(!pvar) {
		PyErr_Clear(); // no method found
        post("%s - get: Python variable %s not found",thisName(),GetString(s));
	}
	else {
        AtomList *lst = GetPyArgs(pvar);
        if(lst) {
            // dump value to attribute outlet
            AtomAnything out(sym_get,lst->Count()+1);
            SetSymbol(out[0],s);
            out.Set(lst->Count(),lst->Atoms(),1);
            delete lst;

            ToOutAnything(GetOutAttr(),out);
        }
        else
            post("%s - get: List could not be created",thisName());
        Py_DECREF(pvar);
    }

    PyUnlock(state);
}

void pyext::m_set(int argc,const t_atom *argv)
{
    PyThreadState *state = PyLock();

    if(argc < 2 || !IsString(argv[0]))
        post("%s - Syntax: set varname arguments...",thisName());
    else if(*GetString(argv[0]) == '_')
        post("%s - set: variables with leading _ are reserved and can't be set",thisName());
    else {
        char *ch = const_cast<char *>(GetString(argv[0]));
        if(!PyObject_HasAttrString(pyobj,ch)) {
		    PyErr_Clear(); // no method found
            post("%s - set: Python variable %s not found",thisName(),ch);
	    }
	    else {
            PyObject *pval = MakePyArgs(NULL,argc-1,argv+1,-1,false);

            if(!pval)
			    PyErr_Print();
		    else {
                if(PySequence_Size(pval) == 1) {
                    // reduce lists of one element to element itself

                    PyObject *val1 = PySequence_GetItem(pval,0); // new reference
                    Py_DECREF(pval);
                    pval = val1;
                }

                PyObject_SetAttrString(pyobj,ch,pval);
                Py_DECREF(pval);
            }
        }
    }

    PyUnlock(state);
}


bool pyext::m_method_(int n,const t_symbol *s,int argc,const t_atom *argv)
{
    bool ret = false;
	if(pyobj && n >= 1)
		ret = work(n,s,argc,argv);
    else
		post("%s - no method for type '%s' into inlet %i",thisName(),GetString(s),n);
    return ret;
}


void pyext::m_help()
{
	post("");
	post("%s %s - python class object, (C)2002-2005 Thomas Grill",thisName(),PY__VERSION);
#ifdef FLEXT_DEBUG
	post("DEBUG VERSION, compiled on " __DATE__ " " __TIME__);
#endif

    post("Arguments: %s {inlets outlets} [script name] [class name] {args...}",thisName());

	post("Inlet 1: messages to control the pyext object");
	post("      2...: python inlets");
	post("Outlets: python outlets");	
	post("Methods:");
	post("\thelp: shows this help");
	post("\treload {args...}: reload python script");
	post("\treload. : reload with former arguments");
	post("\tdoc: display module doc string");
	post("\tdoc+: display class doc string");
	post("\tdir: dump module dictionary");
	post("\tdir+: dump class dictionary");
#ifdef FLEXT_THREADS
	post("\tdetach 0/1: detach threads");
	post("\tstop {wait time (ms)}: stop threads");
#endif
	post("");
}

bool pyext::callpy(PyObject *fun,PyObject *args)
{
    PyObject *ret = PyObject_Call(fun,args,NULL);
    if(ret == NULL) {
        // function not found resp. arguments not matching
        PyErr_Print();
        return false;
    }
    else {
		if(!PyObject_Not(ret)) post("pyext - returned value is ignored");
		Py_DECREF(ret);
        return true;
    }
} 


bool pyext::call(const char *meth,int inlet,const t_symbol *s,int argc,const t_atom *argv) 
{
	bool ret = false;

	PyObject *pmeth = PyObject_GetAttrString(pyobj,const_cast<char *>(meth)); /* fetch bound method */
	if(pmeth == NULL) {
		PyErr_Clear(); // no method found
	}
	else {
		PyObject *pargs = MakePyArgs(s,argc,argv,inlet?inlet:-1,true);
        if(!pargs) {
			PyErr_Print();
    		Py_DECREF(pmeth);
        }
		else 
            ret = gencall(pmeth,pargs);
	}
	return ret;
}

bool pyext::work(int n,const t_symbol *s,int argc,const t_atom *argv)
{
	bool ret = false;

    PyThreadState *state = PyLock();

    // should be enough...
	char str[256];

	{
		// try tag/inlet
		sprintf(str,"%s_%i",GetString(s),n);
		ret = call(str,0,NULL,argc,argv);
	}

	if(!ret) {
		// try anything/inlet
		sprintf(str,"_anything_%i",n);
		if(s == sym_bang && !argc) {
			t_atom argv;
			SetSymbol(argv,sym__);
			ret = call(str,0,s,1,&argv);
		}
		else
			ret = call(str,0,s,argc,argv);
	}
	if(!ret) {
		// try tag at any inlet
		sprintf(str,"%s_",GetString(s));
		ret = call(str,n,NULL,argc,argv);
	}
	if(!ret) {
		// try anything at any inlet
		const char *str1 = "_anything_";
		if(s == sym_bang && !argc) {
			t_atom argv;
			SetSymbol(argv,sym__);
			ret = call(str1,n,s,1,&argv);
		}
		else
			ret = call(str1,n,s,argc,argv);
	}

	if(!ret) 
		// no matching python method found
		post("%s - no matching method found for '%s' into inlet %i",thisName(),GetString(s),n);

	PyUnlock(state);

    Respond(ret);
	return ret;
}
