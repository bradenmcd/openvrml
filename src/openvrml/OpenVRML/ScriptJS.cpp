//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 

//  ScriptJS.cpp
//  javascript script objects.
//  This needs a major clean up...
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

# if defined(_WIN32) && !defined(__CYGWIN__)
#include <winconfig.h>
#endif

# ifndef OPENVRML_DISABLE_JAVASCRIPT

# include <assert.h>
# include <stdio.h>
# include <string.h>

# if defined(_WIN32) && !defined(__CYGWIN__)
#  include <strstrea.h>
# else
#  include <strstream.h>
# endif


#include "ScriptJS.h"
#include "doc2.hpp"
#include "MathUtils.h"
#include "System.h"
#include "VrmlNodeScript.h"
#include "VrmlNamespace.h"
#include "VrmlNodeType.h"
#include "VrmlScene.h"
#include "field.h"

#include <js/jsapi.h>
// This is nominally a private include but I want to subclass Arrays...
#include <js/jsarray.h>


# define MAX_HEAP_BYTES 4L * 1024L * 1024L
# define STACK_CHUNK_BYTES 4024L

#ifndef NDEBUG
#define SCRIPTJS_DEBUG
#endif

namespace {

    namespace JavaScript_ {

        void initVrmlClasses(JSContext * cx, JSObject * obj);
        JSBool floatsToJSArray(size_t numFloats, const float * floats,
                               JSContext * cx, jsval * rval);
        VrmlField * createVrmlFieldFromJsval(JSContext * cx, jsval val,
				             VrmlField::VrmlFieldType fieldType);
        ScriptJS * objToScript(JSContext * cx, JSObject * obj);
        char * objToEventOut(JSContext * cx, JSObject * obj);
        void checkEventOut(JSContext * cx, JSObject * obj,
                           const VrmlField & val);

        namespace Global {
            JSClass jsclass = {
                "global", 0,
                JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
            };
            JSBool print(JSContext *, JSObject *, uintN, jsval *, jsval *);

        }

        namespace Browser {
            JSClass jsclass = {
                "Browser", 0,
                JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
            };
            JSBool getName(JSContext *, JSObject *, uintN, jsval *, jsval *);
            JSBool getVersion(JSContext *, JSObject *, uintN, jsval *, jsval *);
            JSBool getCurrentSpeed(JSContext *, JSObject *, uintN, jsval *, jsval *);
            JSBool getCurrentFrameRate(JSContext *, JSObject *, uintN, jsval*, jsval*);
            JSBool getWorldURL(JSContext *, JSObject *, uintN, jsval *, jsval *);
            JSBool replaceWorld(JSContext *, JSObject *, uintN, jsval *, jsval *);
            JSBool createVrmlFromString(JSContext *, JSObject *, uintN, jsval *, jsval *);
            JSBool createVrmlFromURL(JSContext *, JSObject *, uintN, jsval *, jsval *);
            JSBool addRoute(JSContext *, JSObject *, uintN, jsval *, jsval *);
            JSBool deleteRoute(JSContext *, JSObject *, uintN, jsval *, jsval *);
            JSBool loadURL(JSContext *, JSObject *, uintN, jsval *, jsval *);
            JSBool setDescription(JSContext *, JSObject *, uintN, jsval *, jsval *);
        }

        namespace Field {
            void finalize(JSContext * cx, JSObject * obj);
            JSBool toString(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval);
        }

        namespace SFColor {

            JSObject * initClass(JSContext * cx, JSObject * obj);

            namespace {
                using namespace Field;
                JSBool constructor(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval *);
                JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
                JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
                JSBool setHSV(JSContext * cx, JSObject * obj,
                                    uintN argc, jsval * argv, jsval *);
                JSBool getHSV(JSContext * cx, JSObject * obj,
                                    uintN argc, jsval * argv, jsval *rval);
            }

            JSClass jsclass = {
                "SFColor", JSCLASS_HAS_PRIVATE,
                JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }

        namespace SFImage {

            JSObject * initClass(JSContext * cx, JSObject * obj);

            namespace {
                using namespace Field;
                JSBool constructor(JSContext * cx, JSObject * obj,
	                           uintN argc, jsval * argv, jsval *);
                JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
                JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
            }

            JSClass jsclass = {
                "SFImage", JSCLASS_HAS_PRIVATE,
                JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }

        namespace SFNode {

            JSObject * initClass(JSContext * cx, JSObject * obj);

            namespace {
                using namespace Field;
                JSBool constructor(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval *);
                JSBool getProperty(JSContext * cx, JSObject * obj,
                                   jsval id, jsval * vp);
                JSBool setProperty(JSContext * cx, JSObject * obj,
                                   jsval id, jsval * vp);
            }

            JSClass jsclass = {
                "SFNode", JSCLASS_HAS_PRIVATE,
                JS_PropertyStub,  JS_PropertyStub, getProperty, setProperty,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }

        namespace SFRotation {

            JSObject * initClass(JSContext * cx, JSObject * obj);

            namespace {
                using namespace Field;
                JSBool constructor(JSContext * cx, JSObject * obj,
	                           uintN argc, jsval * argv, jsval *);
                JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
                JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
            }

            JSClass jsclass = {
                "SFRotation", JSCLASS_HAS_PRIVATE,
                JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }

        namespace SFVec2f {

            JSObject * initClass(JSContext * cx, JSObject * obj);

            namespace {
                using namespace Field;
                JSBool constructor(JSContext * cx, JSObject * obj,
	                           uintN argc, jsval * argv, jsval *);
                JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
                JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
            }

            JSClass jsclass = {
              "SFVec2f", JSCLASS_HAS_PRIVATE,
              JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
              JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }

        namespace SFVec3f {

            JSObject * initClass(JSContext * cx, JSObject * obj);

            namespace {
                using namespace Field;
                JSBool constructor(JSContext * cx, JSObject * obj,
	                           uintN argc, jsval * argv, jsval *);
                JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
                JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
            }

            JSClass jsclass = {
              "SFVec3f", JSCLASS_HAS_PRIVATE,
              JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
              JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }
    }
}

JSRuntime * ScriptJS::rt = 0; // Javascript runtime singleton object
size_t ScriptJS::nInstances = 0; // Number of distinct script objects




static void ErrorReporter(JSContext *, const char *, JSErrorReport *);



// Construct from inline script

ScriptJS::ScriptJS(VrmlNodeScript & scriptNode, const char * source ):
        ScriptObject(scriptNode), d_cx(0), d_globalObj(0), d_browserObj(0)
{
    using namespace JavaScript_;
  if (! rt)
    rt = JS_Init( MAX_HEAP_BYTES );

  ++nInstances;

  if (rt)
    d_cx = JS_NewContext( rt, STACK_CHUNK_BYTES );

  if (d_cx)
    {
      JS_SetErrorReporter(d_cx, ErrorReporter);

      // Define the global objects (builtins, Browser, SF*, MF*) ...
      d_globalObj = JS_NewObject( d_cx, &Global::jsclass, 0, 0 );
      JS_InitStandardClasses( d_cx, d_globalObj );
      
      static JSFunctionSpec globalFunctions[] = {
          { "print", Global::print, 0 },
          { 0, 0, 0 }
      };

      JS_DefineFunctions( d_cx, d_globalObj, globalFunctions );

      // VRML-like TRUE, FALSE syntax
      if (! JS_DefineProperty( d_cx, d_globalObj, "FALSE",
			       BOOLEAN_TO_JSVAL(false), 0, 0,
			       JSPROP_READONLY | JSPROP_PERMANENT ))
	theSystem->error("JS_DefineProp FALSE failed\n");
      if (! JS_DefineProperty( d_cx, d_globalObj, "TRUE",
			       BOOLEAN_TO_JSVAL(true), 0, 0,
			       JSPROP_READONLY | JSPROP_PERMANENT ))
	theSystem->error("JS_DefineProp TRUE failed\n");

      if (! JS_DefineProperty( d_cx, d_globalObj, "_script",
			       PRIVATE_TO_JSVAL(this), 0, 0,
			       JSPROP_READONLY | JSPROP_PERMANENT ))
	theSystem->error("JS_DefineProp _script failed\n");

      // Browser object
      static JSFunctionSpec browserFunctions[] = {
          { "getName", Browser::getName, 0 },
          { "getVersion", Browser::getVersion, 0 },
          { "getCurrentSpeed", Browser::getCurrentSpeed, 0 },
          { "getCurrentFrameRate", Browser::getCurrentFrameRate, 0 },
          { "getWorldURL", Browser::getWorldURL, 0 },
          { "replaceWorld", Browser::replaceWorld, 0 },
          { "createVrmlFromString", Browser::createVrmlFromString, 0 },
          { "createVrmlFromURL", Browser::createVrmlFromURL, 0 },
          { "addRoute", Browser::addRoute, 0 },
          { "deleteRoute", Browser::deleteRoute, 0 },
          { "loadURL", Browser::loadURL, 0 },
          { "setDescription", Browser::setDescription, 0 },
          { 0 }
      };
      d_browserObj = JS_DefineObject( d_cx, d_globalObj, "Browser",
				    &Browser::jsclass, 0, 0 );
      if (! JS_DefineProperty( d_cx, d_browserObj, "_script",
			       PRIVATE_TO_JSVAL(this), 0, 0,
			       JSPROP_READONLY | JSPROP_PERMANENT ))
	theSystem->error("JS_DefineProp _script failed\n");

      if (! JS_DefineFunctions( d_cx, d_browserObj, browserFunctions ))
	theSystem->error("JS_DefineFunctions failed\n");

      // Define SF*/MF* classes
      initVrmlClasses(this->d_cx, this->d_globalObj);

      // Define field/eventOut vars for this script
      defineFields();

      /* These should indicate source location for diagnostics. */
      char *filename = 0;
      uintN lineno = 0;

      jsval rval;
      if (! JS_EvaluateScript( d_cx, d_globalObj, source, strlen(source),
			       filename, lineno, &rval))
	theSystem->error("JS_EvaluateScript failed\n");
    }
}


ScriptJS::~ScriptJS()
{
  JS_DestroyContext(d_cx);
  if (--nInstances == 0) {
	  JS_Finish(rt);
	  rt = 0;
  }
}


static double s_timeStamp;	// go away...

/**
 * @brief Run a specified script.
 *
 * @todo Decide what to do if a call to a JavaScript function fails
 *      (probably due to an error in the script).
 */

void ScriptJS::activate(double timeStamp, const char * fname,
                        const size_t argc, const VrmlField * argv[]) {
    assert(this->d_cx);

    jsval fval, rval;

    try {
        if (!JS_LookupProperty(d_cx, d_globalObj, fname, &fval)) {
            throw std::bad_alloc();
        }

        //
        // The function may not be defined, in which case we should do nothing.
        //
        if (JSVAL_IS_VOID(fval)) {
            return;
        }

        jsval * const jsargv = new jsval[argc];

        d_timeStamp = timeStamp;
        s_timeStamp = timeStamp;	// this won't work for long...

        // convert VrmlField*'s to (gc-protected) jsvals
        size_t i;
        for (i = 0; i < argc; ++i) {
            jsargv[i] = argv[i]
                        ? vrmlFieldToJSVal(argv[i]->fieldType(), argv[i], true)
                        : JSVAL_NULL;
        }

        JSBool ok;
        ok = JS_CallFunctionValue(d_cx, d_globalObj, fval, argc, jsargv, &rval);
        //
        // What should we do at this point if a function call fails? For now,
        // just print a message for a debug build.
        //
# ifndef NDEBUG
        if (!ok) {
            cerr << "Call to " << fname << " in Script node "
                 << this->scriptNode.name() << " failed." << endl;
        }
# endif

        // Free up args
        for (i = 0; i < argc; ++i) {
            if (JSVAL_IS_GCTHING(jsargv[i])) {
                ok = JS_RemoveRoot(d_cx, JSVAL_TO_GCTHING(jsargv[i]));
                assert(ok);
            }
        }

        delete [] jsargv;

    } catch (std::bad_alloc & ex) {
# ifndef NDEBUG
        cerr << ex.what() << endl;
# endif
    }
}

//
//  Classes precede the methods since the methods may access
//  objects of the various classes.
//


// MFColor class

static JSBool
MFColorCons(JSContext *cx, JSObject *obj,
	    uintN argc, jsval *argv, jsval *rval)
{
  obj = JS_NewArrayObject(cx, (jsint) argc, argv);
  *rval = OBJECT_TO_JSVAL(obj);
  return obj == 0 ? JS_FALSE : JS_TRUE;
}

static JSClass MFColorClass = {
  "MFColor", 0,
  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  
  JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,  JS_FinalizeStub,
};

// MFFloat class

static JSBool
MFFloatCons(JSContext *cx, JSObject *obj,
	    uintN argc, jsval *argv, jsval *rval)
{
  obj = JS_NewArrayObject(cx, (jsint) argc, argv);
  *rval = OBJECT_TO_JSVAL(obj);
  return obj == 0 ? JS_FALSE : JS_TRUE;
}

static JSClass MFFloatClass = {
  "MFFloat", 0,
  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  
  JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,  JS_FinalizeStub,
};

// MFInt32 class

static JSBool
MFInt32Cons(JSContext *cx, JSObject *obj,
	    uintN argc, jsval *argv, jsval *rval)
{
  obj = JS_NewArrayObject(cx, (jsint) argc, argv);
  *rval = OBJECT_TO_JSVAL(obj);
  return obj == 0 ? JS_FALSE : JS_TRUE;
}

static JSClass MFInt32Class = {
  "MFInt32", 0,
  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  
  JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,  JS_FinalizeStub,
};

// MFNode class

static JSBool
MFNodeCons(JSContext *cx, JSObject *obj,
	   uintN argc, jsval *argv, jsval *rval)
{
  obj = JS_NewArrayObject(cx, (jsint) argc, argv);
  *rval = OBJECT_TO_JSVAL(obj);
  return obj == 0 ? JS_FALSE : JS_TRUE;
}

static JSClass MFNodeClass = {
  "MFNode", 0,
  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  
  JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,  JS_FinalizeStub,
};

// MFRotation class

static JSBool
MFRotationCons(JSContext *cx, JSObject *obj,
	       uintN argc, jsval *argv, jsval *rval)
{
  obj = JS_NewArrayObject(cx, (jsint) argc, argv);
  *rval = OBJECT_TO_JSVAL(obj);
  return obj == 0 ? JS_FALSE : JS_TRUE;
}

static JSClass MFRotationClass = {
  "MFRotation", 0,
  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  
  JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,  JS_FinalizeStub,
};

// MFString class

static JSBool
MFStringCons(JSContext *cx, JSObject *obj,
	     uintN argc, jsval *argv, jsval *rval)
{
  obj = JS_NewArrayObject(cx, (jsint) argc, argv);
  *rval = OBJECT_TO_JSVAL(obj);
  return obj == 0 ? JS_FALSE : JS_TRUE;
}

static JSClass MFStringClass = {
  "MFString", 0,
  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  
  JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,  JS_FinalizeStub,
};

// MFVec2f class

static JSBool
MFVec2fCons(JSContext *cx, JSObject *obj,
	     uintN argc, jsval *argv, jsval *rval)
{
  obj = JS_NewArrayObject(cx, (jsint) argc, argv);
  *rval = OBJECT_TO_JSVAL(obj);
  return obj == 0 ? JS_FALSE : JS_TRUE;
}

static JSClass MFVec2fClass = {
  "MFVec2f", 0,
  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  
  JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,  JS_FinalizeStub,
};

// MFVec3f class

static JSBool
MFVec3fCons(JSContext *cx, JSObject *obj,
	     uintN argc, jsval *argv, jsval *rval)
{
  obj = JS_NewArrayObject(cx, (jsint) argc, argv);
  *rval = OBJECT_TO_JSVAL(obj);
  return obj == 0 ? JS_FALSE : JS_TRUE;
}

static JSClass MFVec3fClass = {
  "MFVec3f", 0,
  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,
  JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,  JS_FinalizeStub,
};


// VrmlMatrix class ...


//
//  Methods
//

// SFColor methods


// SFRotation methods

static JSBool
rot_getAxis(JSContext *cx, JSObject *obj, uintN , jsval *,
	    jsval *rval)
{
    using JavaScript_::floatsToJSArray;
    
    const VrmlSFRotation * const rot =
            reinterpret_cast<const VrmlSFRotation *>(JS_GetPrivate(cx, obj));
    assert(rot);
    return floatsToJSArray(3, rot->get(), cx, rval);
}

static JSBool
rot_inverse(JSContext *cx, JSObject *obj, uintN , jsval *,
	    jsval *rval)
{
    using JavaScript_::SFRotation;
    
    VrmlSFRotation *r = (VrmlSFRotation *)JS_GetPrivate( cx, obj );
    JSObject *robj = 0;

    if (r &&
        (robj = JS_NewObject( cx, &SFRotation::jsclass, 0,
			      JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, robj, new VrmlSFRotation(r->inverse()));
        *rval = OBJECT_TO_JSVAL(robj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
rot_multiply(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
	     jsval *rval)
{
    using JavaScript_::SFRotation;
    
    const VrmlSFRotation * const r1 =
            reinterpret_cast<const VrmlSFRotation *>(JS_GetPrivate(cx, obj));
    const VrmlSFRotation * r2 = 0;
    JSObject *r3obj = 0;

    // Verify that we have 2 SFRotation's && create a result
    if (r1 && argc > 0 &&
        JSVAL_IS_OBJECT(argv[0]) &&
        &SFRotation::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
        (r2 = reinterpret_cast<const VrmlSFRotation *>(JS_GetPrivate( cx, JSVAL_TO_OBJECT(argv[0])))) &&
        (((VrmlField*)r2)->fieldType() == VrmlField::SFROTATION) &&
        (r3obj = JS_NewObject( cx, &SFRotation::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, r3obj, new VrmlSFRotation(r1->multiply(*r2)));
        *rval = OBJECT_TO_JSVAL(r3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
rot_multVec(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
	    jsval *rval)
{
    using JavaScript_::SFVec3f;
    
    VrmlSFRotation *r1 = (VrmlSFRotation *)JS_GetPrivate( cx, obj );
    VrmlSFVec3f *v2 = 0;
    JSObject *v3obj = 0;

    // Verify that we have an SFVec3f arg && create a result
    if (r1 && argc > 0 &&
        JSVAL_IS_OBJECT(argv[0]) &&
        &SFVec3f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
        (v2 = (VrmlSFVec3f *)JS_GetPrivate( cx, JSVAL_TO_OBJECT(argv[0]))) &&
        (((VrmlField*)v2)->fieldType() == VrmlField::SFVEC3F) &&
        (v3obj = JS_NewObject( cx, &SFVec3f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        VrmlSFVec3f *v3 = new VrmlSFVec3f;
        // multiply v2 by rotation matrix...
        //Vmatrix( v3->get(), r1->getMatrix(), v2->get() );
        JS_SetPrivate( cx, v3obj, v3 );
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}


// Should this create a new object or modify obj?...

static JSBool
rot_setAxis(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
	    jsval *rval)
{
    using JavaScript_::SFVec3f;
    
    VrmlSFRotation *r1 = (VrmlSFRotation *)JS_GetPrivate( cx, obj );
    VrmlSFVec3f *v2 = 0;

    // Verify that we have an SFVec3f arg && create a result
    if (r1 && argc > 0 &&
        JSVAL_IS_OBJECT(argv[0]) &&
        &SFVec3f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
        (v2 = (VrmlSFVec3f *)JS_GetPrivate( cx, JSVAL_TO_OBJECT(argv[0]))) &&
        (((VrmlField*)v2)->fieldType() == VrmlField::SFVEC3F) )
      {
        r1->setAxis(*v2);
        *rval = OBJECT_TO_JSVAL(obj); // return the same object
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
rot_slerp(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
	  jsval *rval)
{
    using JavaScript_::SFRotation;

    VrmlSFRotation *r1 = (VrmlSFRotation *)JS_GetPrivate( cx, obj );
    VrmlSFRotation *r2 = 0;
    JSObject *r3obj = 0;

    // Verify that we have 2 SFRotation's, a number, & create a result
    if (r1 && argc > 1 &&
        JSVAL_IS_OBJECT(argv[0]) &&
        &SFRotation::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
        (r2 = (VrmlSFRotation *)JS_GetPrivate( cx, JSVAL_TO_OBJECT(argv[0]))) &&
        (((VrmlField*)r2)->fieldType() == VrmlField::SFROTATION) &&
        JSVAL_IS_NUMBER(argv[1]) &&
        (r3obj = JS_NewObject( cx, &SFRotation::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        jsdouble factor;
        JS_ValueToNumber( cx, argv[1], &factor );
        JS_SetPrivate( cx, r3obj, new VrmlSFRotation(r1->slerp(*r2, factor)));
        *rval = OBJECT_TO_JSVAL(r3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}


// SFVec2f methods

static JSBool
vec2f_add(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    using JavaScript_::SFVec2f;
    
    VrmlSFVec2f *v1 = (VrmlSFVec2f *)JS_GetPrivate( cx, obj );
    VrmlSFVec2f *v2 = 0;
    JSObject *v3obj = 0;

    // Verify that we have 2 SFVec2f's && create a result
    if (v1 && argc > 0 &&
        JSVAL_IS_OBJECT(argv[0]) &&
        &SFVec2f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
        (v2 = (VrmlSFVec2f *)JS_GetPrivate( cx, JSVAL_TO_OBJECT(argv[0]))) &&
        (((VrmlField*)v2)->fieldType() == VrmlField::SFVEC2F) &&
        (v3obj = JS_NewObject( cx, &SFVec2f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec2f(v1->add(*v2)));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
vec2f_divide(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    using JavaScript_::SFVec2f;
    
    VrmlSFVec2f *v1 = (VrmlSFVec2f *)JS_GetPrivate( cx, obj );
    jsdouble d = 0.0;
    JSObject *v3obj = 0;

    // Verify that we have a number && create a result
    if (v1 && argc > 0 &&
        JS_ValueToNumber( cx, argv[0], &d ) &&
        (v3obj = JS_NewObject( cx, &SFVec2f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec2f(v1->divide(d)));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool vec2f_dot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                        jsval *rval) {
    using JavaScript_::SFVec2f;
    
    if (argc > 0 && JSVAL_IS_OBJECT(argv[0])
            && (&SFVec2f::jsclass == JS_GetClass(JSVAL_TO_OBJECT(argv[0])))) {
        
        const VrmlSFVec2f * const thisVec =
                reinterpret_cast<const VrmlSFVec2f *>(JS_GetPrivate(cx, obj));
        assert(thisVec);
        
        const VrmlSFVec2f * const argVec =
                reinterpret_cast<const VrmlSFVec2f *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
        assert(argVec);
        
        *rval = DOUBLE_TO_JSVAL(thisVec->dot(*argVec));

        return JS_TRUE;
    }
    
    return JS_FALSE;
}

static JSBool
vec2f_length(JSContext *cx, JSObject *obj, uintN, jsval *, jsval *rval)
{
  VrmlSFVec2f *v = (VrmlSFVec2f *)JS_GetPrivate( cx, obj );
  if (v)
    {
      *rval = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->length()));
      return JS_TRUE;
    }
  return JS_FALSE;
}

static JSBool
vec2f_multiply(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    using JavaScript_::SFVec2f;
    
    VrmlSFVec2f *v1 = (VrmlSFVec2f *)JS_GetPrivate( cx, obj );
    jsdouble d = 0.0;
    JSObject *v3obj = 0;

    // Verify that we have a number && create a result
    if (v1 && argc > 0 &&
        JS_ValueToNumber( cx, argv[0], &d ) &&
        (v3obj = JS_NewObject( cx, &SFVec2f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec2f(v1->multiply(d)));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
vec2f_negate(JSContext *cx, JSObject *obj, uintN, jsval *, jsval *rval)
{
    using JavaScript_::SFVec2f;
    
    VrmlSFVec2f *v1 = (VrmlSFVec2f *)JS_GetPrivate( cx, obj );
    JSObject *v3obj = 0;

    if (v1 &&
        (v3obj = JS_NewObject( cx, &SFVec2f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec2f(v1->negate()));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
vec2f_normalize(JSContext *cx, JSObject *obj, uintN, jsval *, jsval *rval)
{
    using JavaScript_::SFVec2f;
    
    VrmlSFVec2f *v1 = (VrmlSFVec2f *)JS_GetPrivate( cx, obj );
    JSObject *v3obj = 0;

    if (v1 &&
        (v3obj = JS_NewObject( cx, &SFVec2f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec2f(v1->normalize()));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
vec2f_subtract(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    using JavaScript_::SFVec2f;

    const VrmlSFVec2f * const v1 =
            reinterpret_cast<const VrmlSFVec2f *>(JS_GetPrivate(cx, obj));
    const VrmlSFVec2f * v2 = 0;
    JSObject *v3obj = 0;

    // Verify that we have 2 SFVec2f's && create a result
    if (v1 && argc > 0 &&
        JSVAL_IS_OBJECT(argv[0]) &&
        &SFVec2f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
        (v2 = (VrmlSFVec2f *)JS_GetPrivate( cx, JSVAL_TO_OBJECT(argv[0]))) &&
        (((VrmlField*)v2)->fieldType() == VrmlField::SFVEC2F) &&
        (v3obj = JS_NewObject( cx, &SFVec2f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec2f(v1->subtract(*v2)));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}


// SFVec3f methods

static JSBool
vec3f_add(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    using JavaScript_::SFVec3f;

    const VrmlSFVec3f * const v1 =
            reinterpret_cast<VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
    const VrmlSFVec3f * v2 = 0;
    JSObject *v3obj = 0;

    // Verify that we have 2 SFVec3f's && create a result
    if (v1 && argc > 0 &&
        JSVAL_IS_OBJECT(argv[0]) &&
        &SFVec3f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
        (v2 = reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])))) &&
        (((VrmlField*)v2)->fieldType() == VrmlField::SFVEC3F) &&
        (v3obj = JS_NewObject( cx, &SFVec3f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->add(*v2)));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
vec3f_cross(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    using JavaScript_::SFVec3f;

    VrmlSFVec3f *v1 = (VrmlSFVec3f *)JS_GetPrivate( cx, obj );
    VrmlSFVec3f *v2 = 0;
    JSObject *v3obj = 0;

    // Verify that we have 2 SFVec3f's && create a result
    if (v1 && argc > 0 &&
        JSVAL_IS_OBJECT(argv[0]) &&
        &SFVec3f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
        (v2 = (VrmlSFVec3f *)JS_GetPrivate( cx, JSVAL_TO_OBJECT(argv[0]))) &&
        (((VrmlField*)v2)->fieldType() == VrmlField::SFVEC3F) &&
        (v3obj = JS_NewObject( cx, &SFVec3f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->cross(*v2)));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
vec3f_divide(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    using JavaScript_::SFVec3f;

    VrmlSFVec3f *v1 = (VrmlSFVec3f *)JS_GetPrivate( cx, obj );
    jsdouble d = 0.0;
    JSObject *v3obj = 0;

    // Verify that we have a number && create a result
    if (v1 && argc > 0 &&
        JS_ValueToNumber( cx, argv[0], &d ) &&
        (v3obj = JS_NewObject( cx, &SFVec3f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->divide(d)));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool vec3f_dot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                        jsval *rval) {
    using JavaScript_::SFVec3f;

    if (argc > 0 && JSVAL_IS_OBJECT(argv[0])
            && (&SFVec3f::jsclass == JS_GetClass(JSVAL_TO_OBJECT(argv[0])))) {
        
        const VrmlSFVec3f * const thisVec =
                reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
        assert(thisVec);
        
        const VrmlSFVec3f * const argVec =
                reinterpret_cast<const VrmlSFVec3f *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
        assert(argVec);
        
        *rval = DOUBLE_TO_JSVAL(thisVec->dot(*argVec));
        
        return JS_TRUE;
    }

    return JS_FALSE;
}

static JSBool
vec3f_length(JSContext *cx, JSObject *obj, uintN, jsval *, jsval *rval)
{
  VrmlSFVec3f *v = (VrmlSFVec3f *)JS_GetPrivate( cx, obj );
  if (v)
    {
      *rval = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->length()));
      return JS_TRUE;
    }
  return JS_FALSE;
}

static JSBool
vec3f_multiply(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    using JavaScript_::SFVec3f;

    const VrmlSFVec3f * const v1 =
            reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
    jsdouble d = 0.0;
    JSObject *v3obj = 0;

    // Verify that we have a number && create a result
    if (v1 && argc > 0 &&
        JS_ValueToNumber( cx, argv[0], &d ) &&
        (v3obj = JS_NewObject( cx, &SFVec3f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->multiply(d)));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
vec3f_negate(JSContext *cx, JSObject *obj, uintN, jsval *, jsval *rval)
{
    using JavaScript_::SFVec3f;

    const VrmlSFVec3f * const v1 =
            reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
    JSObject *v3obj = 0;

    if (v1 &&
        (v3obj = JS_NewObject( cx, &SFVec3f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate( cx, v3obj, new VrmlSFVec3f(v1->negate()));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
vec3f_normalize(JSContext *cx, JSObject *obj, uintN, jsval *, jsval *rval)
{
    using JavaScript_::SFVec3f;

    const VrmlSFVec3f * const v1 =
            reinterpret_cast<VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
    JSObject *v3obj = 0;

    if (v1 &&
        (v3obj = JS_NewObject(cx, &SFVec3f::jsclass, 0,
			      JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->normalize()));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

static JSBool
vec3f_subtract(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    using JavaScript_::SFVec3f;

    const VrmlSFVec3f * const v1 =
            reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
    const VrmlSFVec3f * v2 = 0;
    JSObject *v3obj = 0;

    // Verify that we have 2 SFVec3f's && create a result
    if (v1 && argc > 0 &&
        JSVAL_IS_OBJECT(argv[0]) &&
        &SFVec3f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
        (v2 = reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])))) &&
        (((VrmlField*)v2)->fieldType() == VrmlField::SFVEC3F) &&
        (v3obj = JS_NewObject( cx, &SFVec3f::jsclass, 0,
			       JS_GetParent( cx, obj ) )) != 0)
      {
        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->subtract(*v2)));
        *rval = OBJECT_TO_JSVAL(v3obj);
        return JS_TRUE;
      }

    return JS_FALSE;
}

// Create class objects for the non-scalar field types.
// Move toString to a prototype object?



// Convert a VrmlField value to a jsval, optionally protect from gc.

jsval ScriptJS::vrmlFieldToJSVal( VrmlField::VrmlFieldType type,
				  const VrmlField *f,
				  bool protect )
{
    using JavaScript_::SFColor;
    using JavaScript_::SFImage;
    using JavaScript_::SFNode;
    using JavaScript_::SFRotation;
    using JavaScript_::SFVec2f;
    using JavaScript_::SFVec3f;
    
  switch (type)
    {
    case VrmlField::SFBOOL:
      {
	return BOOLEAN_TO_JSVAL( f ? f->toSFBool()->get() : 0 );
      }

    case VrmlField::SFCOLOR:
      {
	JSObject *obj = JS_NewObject( d_cx, &SFColor::jsclass, 0, d_globalObj );
	if (! obj) return JSVAL_NULL;
	if (protect) JS_AddRoot( d_cx, obj );
	JS_SetPrivate( d_cx, obj, f ? f->clone() : new VrmlSFColor() );
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::SFFLOAT:
      {
	jsdouble *d = JS_NewDouble( d_cx, f ? f->toSFFloat()->get() : 0.0 );
	if (protect) JS_AddRoot( d_cx, d );
	return DOUBLE_TO_JSVAL( d );
      }

    case VrmlField::SFIMAGE:
      {
	JSObject *obj = JS_NewObject( d_cx, &SFImage::jsclass, 0, d_globalObj );
	if (! obj) return JSVAL_NULL;
	if (protect) JS_AddRoot( d_cx, obj );
	JS_SetPrivate( d_cx, obj, f ? f->clone() : new VrmlSFImage() );
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::SFINT32:
      {
	return INT_TO_JSVAL( f ? f->toSFInt32()->get() : 0 );
      }

    case VrmlField::SFNODE:
      {
	JSObject *obj = JS_NewObject( d_cx, &SFNode::jsclass, 0, d_globalObj );
	if (! obj) return JSVAL_NULL;
	if (protect) JS_AddRoot( d_cx, obj );
	JS_SetPrivate( d_cx, obj, f ? f->clone() : new VrmlSFNode() );
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::SFROTATION:
      {
	JSObject *obj = JS_NewObject( d_cx, &SFRotation::jsclass, 0, d_globalObj );
	if (! obj) return JSVAL_NULL;
	if (protect) JS_AddRoot( d_cx, obj );
	JS_SetPrivate( d_cx, obj, f ? f->clone() : new VrmlSFRotation() );
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::SFSTRING:
      {
	JSString *s = JS_NewStringCopyZ( d_cx, f ? f->toSFString()->get() : "" );
	if (protect) JS_AddRoot( d_cx, s );
	return STRING_TO_JSVAL( s );
      }

    case VrmlField::SFTIME:
      {
	jsdouble *d = JS_NewDouble( d_cx, f ? f->toSFTime()->get() : 0 );
	if (protect) JS_AddRoot( d_cx, d );
	return DOUBLE_TO_JSVAL( d );
      }

    case VrmlField::SFVEC2F:
      {
	JSObject *obj = JS_NewObject( d_cx, &SFVec2f::jsclass, 0, d_globalObj );
	if (! obj) return JSVAL_NULL;
	if (protect) JS_AddRoot( d_cx, obj );
	JS_SetPrivate( d_cx, obj, f ? f->clone() : new VrmlSFVec2f() );
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::SFVEC3F:
      {
	JSObject *obj = JS_NewObject( d_cx, &SFVec3f::jsclass, 0, d_globalObj );
	if (! obj) return JSVAL_NULL;
	if (protect) JS_AddRoot( d_cx, obj );
	JS_SetPrivate( d_cx, obj, f ? f->clone() : new VrmlSFVec3f() );
	return OBJECT_TO_JSVAL(obj);
      }

    // MF*
    case VrmlField::MFCOLOR:
      {
	assert(!f || dynamic_cast<const VrmlMFColor *>(f));
        const VrmlMFColor * const mf = static_cast<const VrmlMFColor *>(f);
	int i, n = mf ? mf->getLength() : 0;
	JSObject *obj = JS_NewArrayObject( d_cx, (jsint)n, 0 );
	if (! obj) return JSVAL_NULL;
	JS_AddRoot( d_cx, obj );
	const float * fn = mf ? mf->get() : 0;
	for (i=0; i<n; ++i, fn+=3)
	  {
	    JSObject *elt = JS_NewObject( d_cx, &SFColor::jsclass,
					  0, d_globalObj );
	    if (! elt) break;
	    JS_SetPrivate( d_cx, elt, new VrmlSFColor(fn[0], fn[1], fn[2]) );
	    JS_DefineElement( d_cx, obj, (jsint)i, OBJECT_TO_JSVAL(elt),
			      JS_PropertyStub, JS_PropertyStub,
			      JSPROP_ENUMERATE );
	  }
	if (! protect) JS_RemoveRoot( d_cx, obj );
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::MFFLOAT:
      {
	assert(!f || dynamic_cast<const VrmlMFFloat *>(f));
        const VrmlMFFloat * const mf = static_cast<const VrmlMFFloat *>(f);
	int i, n = mf ? mf->getLength() : 0;
	JSObject *obj = JS_NewArrayObject( d_cx, (jsint)n, 0 );
	if (! obj) return JSVAL_NULL;
	JS_AddRoot( d_cx, obj );
	const float * fn = mf ? mf->get() : 0;
	for (i=0; i<n; ++i, ++fn)
	  {
	    jsdouble *elt = JS_NewDouble( d_cx, *fn );
	    if (! elt) break;
	    JS_DefineElement( d_cx, obj, (jsint)i, DOUBLE_TO_JSVAL(elt),
			      JS_PropertyStub, JS_PropertyStub,
			      JSPROP_ENUMERATE );
	  }
	if (! protect) JS_RemoveRoot( d_cx, obj );
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::MFINT32:
      {
	assert(!f || dynamic_cast<const VrmlMFInt32 *>(f));
        const VrmlMFInt32 * const mf = static_cast<const VrmlMFInt32 *>(f);
	int i, n = mf ? mf->getLength() : 0;
	JSObject *obj = JS_NewArrayObject( d_cx, (jsint)n, 0 );
	if (! obj) return JSVAL_NULL;
	if (protect) JS_AddRoot( d_cx, obj );
	const long *fn = mf ? mf->get() : 0;
	for (i=0; i<n; ++i, ++fn)
	  {
	    JS_DefineElement( d_cx, obj, (jsint)i, INT_TO_JSVAL(*fn),
			      JS_PropertyStub, JS_PropertyStub,
			      JSPROP_ENUMERATE );
	  }
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::MFNODE:
      {
	assert(!f || dynamic_cast<const VrmlMFNode *>(f));
        const VrmlMFNode * const mf = static_cast<const VrmlMFNode *>(f);
	int i, n = mf ? mf->getLength() : 0;
	JSObject *obj = JS_NewArrayObject( d_cx, (jsint)n, 0 );
	if (! obj) return JSVAL_NULL;
	JS_AddRoot( d_cx, obj );
	for (i=0; i<n; ++i)
	  {
	    JSObject *elt = JS_NewObject( d_cx, &SFNode::jsclass,
					  0, d_globalObj );
	    if (! elt) break;
	    JS_SetPrivate(d_cx, elt, new VrmlSFNode(mf->getElement(i)));
	    JS_DefineElement( d_cx, obj, (jsint)i, OBJECT_TO_JSVAL(elt),
			      JS_PropertyStub, JS_PropertyStub,
			      JSPROP_ENUMERATE );
	  }
	if (! protect) JS_RemoveRoot( d_cx, obj );
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::MFROTATION:
      {
	assert(!f || dynamic_cast<const VrmlMFRotation *>(f));
        const VrmlMFRotation * const mf = static_cast<const VrmlMFRotation *>(f);
	int i, n = mf ? mf->getLength() : 0;
	JSObject *obj = JS_NewArrayObject( d_cx, (jsint)n, 0 );
	if (! obj) return JSVAL_NULL;
	JS_AddRoot( d_cx, obj );
	const float * fn = mf ? mf->get() : 0;
	for (i=0; i<n; ++i, fn+=4)
	  {
	    JSObject *elt = JS_NewObject( d_cx, &SFRotation::jsclass,
					  0, d_globalObj );
	    if (! elt) break;
	    JS_SetPrivate( d_cx, elt,
			   new VrmlSFRotation(fn[0], fn[1], fn[2], fn[3]) );
	    JS_DefineElement( d_cx, obj, (jsint)i, OBJECT_TO_JSVAL(elt),
			      JS_PropertyStub, JS_PropertyStub,
			      JSPROP_ENUMERATE );
	  }
	if (! protect) JS_RemoveRoot( d_cx, obj );
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::MFSTRING:
      {
	jsval *jsvec = 0;
	int i, n = 0;
	if (f)
	  {
            assert(dynamic_cast<const VrmlMFString *>(f));
	    const VrmlMFString * const mf = static_cast<const VrmlMFString *>(f);
	    n = mf ? mf->getLength() : 0;
	    if (n > 0) jsvec = new jsval[n];
	    for (i=0; i<n; ++i)
	      {
		JSString *s = JS_NewStringCopyZ( d_cx, mf->get(i) );
		jsvec[i] = STRING_TO_JSVAL( s );
	      }
	  }
	JSObject *obj = JS_NewArrayObject( d_cx, (jsint)n, jsvec );
	if (jsvec) delete [] jsvec;
	if (! obj) return JSVAL_NULL;
	if (protect) JS_AddRoot( d_cx, obj );
	return OBJECT_TO_JSVAL(obj);
      }
      
    case VrmlField::MFVEC2F:
      {
	assert(!f || dynamic_cast<const VrmlMFVec2f *>(f));
        const VrmlMFVec2f * const mf = static_cast<const VrmlMFVec2f *>(f);
	int i, n = mf ? mf->getLength() : 0;
	JSObject *obj = JS_NewArrayObject( d_cx, (jsint)n, 0 );
	if (! obj) return JSVAL_NULL;
	JS_AddRoot( d_cx, obj );
	const float * fn = mf ? mf->get() : 0;
	for (i=0; i<n; ++i, fn+=2)
	  {
	    JSObject *elt = JS_NewObject( d_cx, &SFVec2f::jsclass,
					  0, d_globalObj );
	    if (! elt) break;
	    JS_SetPrivate( d_cx, elt, new VrmlSFVec2f(fn[0], fn[1]) );
	    JS_DefineElement( d_cx, obj, (jsint)i, OBJECT_TO_JSVAL(elt),
			      JS_PropertyStub, JS_PropertyStub,
			      JSPROP_ENUMERATE );
	  }
	if (! protect) JS_RemoveRoot( d_cx, obj );
	return OBJECT_TO_JSVAL(obj);
      }

    case VrmlField::MFVEC3F:
      {
        assert(!f || dynamic_cast<const VrmlMFVec3f *>(f));
        const VrmlMFVec3f * const mf = static_cast<const VrmlMFVec3f *>(f);
	int i, n = mf ? mf->getLength() : 0;
	JSObject *obj = JS_NewArrayObject( d_cx, (jsint)n, 0 );
	if (! obj) return JSVAL_NULL;
	JS_AddRoot( d_cx, obj );
	const float *fn = mf ? mf->get() : 0;
	for (i=0; i<n; ++i, fn+=3)
	  {
	    JSObject *elt = JS_NewObject( d_cx, &SFVec3f::jsclass,
					  0, d_globalObj );
	    if (! elt) break;
	    JS_SetPrivate( d_cx, elt, new VrmlSFVec3f(fn[0], fn[1], fn[2]) );
	    JS_DefineElement( d_cx, obj, (jsint)i, OBJECT_TO_JSVAL(elt),
			      JS_PropertyStub, JS_PropertyStub,
			      JSPROP_ENUMERATE );
	  }
	if (! protect) JS_RemoveRoot( d_cx, obj );
	return OBJECT_TO_JSVAL(obj);
      }

    default:
      theSystem->error("vrmlFieldToJSVal: unhandled type (%d)\n",
		       (int) type);
      return BOOLEAN_TO_JSVAL( false );	// oops...
    }
}


// Must assign the proper type to eventOuts

static JSBool
eventOut_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *val)
{
  using JavaScript_::objToScript;
  using JavaScript_::createVrmlFieldFromJsval;

  JSString *str = JS_ValueToString(cx, id);
  if (! str) return JS_FALSE;
  const char *eventName = JS_GetStringBytes(str);

  // The ScriptJS object pointer is stored as a global property value:
  ScriptJS *script = objToScript( cx, obj );
  if (! script) return JS_FALSE;
  VrmlNodeScript & scriptNode = script->getScriptNode();

  // Validate the type
  VrmlField::VrmlFieldType t = scriptNode.hasEventOut(eventName);
  if (! t) return JS_FALSE;

  // Convert to a vrmlField and set the eventOut value
  VrmlField *f = createVrmlFieldFromJsval( cx, *val, t );
  if (! f)
    {
      theSystem->error("Error: invalid type in assignment to eventOut %s\n",
	      eventName );
      return JS_FALSE;
    }

  scriptNode.setEventOut(eventName, *f);
  if (f) delete f;

  // Don't overwrite the property value.
  if (JSVAL_IS_OBJECT(*val) &&
      JSVAL_TO_OBJECT(*val) != 0 &&
      ! JS_DefineProperty( cx, JSVAL_TO_OBJECT(*val), "_eventOut",
			   PRIVATE_TO_JSVAL((long int)eventName),
			   0, 0, JSPROP_READONLY | JSPROP_PERMANENT ))
    theSystem->error("JS_DefineProp _eventOut failed\n");

  return JS_TRUE;
}


// Define objects corresponding to fields/eventOuts

void ScriptJS::defineFields()
{
  VrmlNodeScript::FieldList::iterator i, end = this->scriptNode.fields().end();
  for (i = this->scriptNode.fields().begin(); i!=end; ++i)
    {
      jsval val = vrmlFieldToJSVal( (*i)->type, (*i)->value, false );
#ifdef SCRIPTJS_DEBUG
      if ( (*i)->value )
	cout << "field " << (*i)->name << " value "
	     << *((*i)->value) << endl;
      else
	cout << "field " << (*i)->name << " <no value>\n";
#endif
      if (! JS_DefineProperty( d_cx, d_globalObj, (*i)->name, val,
			       //getter, setter, ...
			       0, 0,
			       JSPROP_PERMANENT ))
	theSystem->error("JS_DefineProp %s failed\n", (*i)->name );
    }

  end = this->scriptNode.eventOuts().end();
  for (i = this->scriptNode.eventOuts().begin(); i!=end; ++i)
    {
      jsval val = vrmlFieldToJSVal( (*i)->type, (*i)->value, false );
      if (JSVAL_IS_OBJECT(val) &&
	  JSVAL_TO_OBJECT(val) != 0 &&
	  ! JS_DefineProperty( d_cx, JSVAL_TO_OBJECT(val), "_eventOut",
			       PRIVATE_TO_JSVAL((long int)((*i)->name)),
			       0, 0, JSPROP_READONLY | JSPROP_PERMANENT ))
	theSystem->error("JS_DefineProp _eventOut failed\n");

      if (! JS_DefineProperty( d_cx, d_globalObj, (*i)->name, val,
			       //getter, setter
			       0, eventOut_setProperty,
			       JSPROP_PERMANENT ))
	theSystem->error("JS_DefineProp %s failed\n", (*i)->name );
    }

}

// Global functions


// Browser functions



//

static void
ErrorReporter(JSContext *, const char *message, JSErrorReport *report)
{
    int i, j, k, n;

    theSystem->error("javascript: ");
    if (!report) {
	theSystem->error("%s\n", message);
	return;
    }

    if (report->filename)
	theSystem->error("%s, ", report->filename);
    if (report->lineno)
	theSystem->error("line %u: ", report->lineno);
    theSystem->error(message);
    if (!report->linebuf) {
	theSystem->error("\n");
	return;
    }

    theSystem->error(":\n%s\n", report->linebuf);
    n = report->tokenptr - report->linebuf;
    for (i = j = 0; i < n; i++) {
	if (report->linebuf[i] == '\t') {
	    for (k = (j + 8) & ~7; j < k; j++)
	      theSystem->error(".");
	    continue;
	}
	theSystem->error(".");
	j++;
    }
    theSystem->error("\n");
}

namespace {
    
    namespace JavaScript_ {

        void initVrmlClasses(JSContext * const cx, JSObject * const obj) {

            using namespace JavaScript_;

            if (!SFColor::initClass(cx, obj))
              theSystem->error("JS_InitClass SFColor failed\n");

            if (!SFImage::initClass(cx, obj))
              theSystem->error("JS_InitClass SFImage failed\n");

            if (!SFNode::initClass(cx, obj))
              theSystem->error("JS_InitClass SFNode failed\n");

            if (!SFRotation::initClass(cx, obj))
              theSystem->error("JS_InitClass SFRotation failed\n");

            if (!SFVec2f::initClass(cx, obj))
              theSystem->error("JS_InitClass SFVec2f failed\n");

            if (!SFVec3f::initClass(cx, obj))
              theSystem->error("JS_InitClass SFVec3f failed\n");


            // All the MF* classes are subclassed from Array
            JSObject *array_proto = js_InitArrayClass(cx, obj);

            // MFColor class
            if (! JS_InitClass( cx, obj, array_proto,
		                &MFColorClass,
		                MFColorCons, 0, // constructor function, min arg count
		                0, 0,
		                0, 0))	  // static properties and methods
              theSystem->error("JS_InitClass MFColor failed\n");

            // MFFloat class
            if (! JS_InitClass( cx, obj, array_proto,
		                &MFFloatClass,
		                MFFloatCons, 0, // constructor function, min arg count
		                0, 0,
		                0, 0))	  // static properties and methods
              theSystem->error("JS_InitClass MFFloat failed\n");

            // MFInt32 class
            if (! JS_InitClass( cx, obj, array_proto,
		                &MFInt32Class,
		                MFInt32Cons, 0, // constructor function, min arg count
		                0, 0,
		                0, 0))	  // static properties and methods
              theSystem->error("JS_InitClass MFInt32 failed\n");

            // MFNode class
            if (! JS_InitClass( cx, obj, array_proto,
		                &MFNodeClass,
		                MFNodeCons, 0, // constructor function, min arg count
		                0, 0,
		                0, 0))	  // static properties and methods
              theSystem->error("JS_InitClass MFNode failed\n");

            // MFRotation class
            if (! JS_InitClass( cx, obj, array_proto,
		                &MFRotationClass,
		                MFRotationCons, 0, // constructor function, min arg count
		                0, 0,
		                0, 0))	  // static properties and methods
              theSystem->error("JS_InitClass MFRotation failed\n");

            // MFString class
            if (! JS_InitClass( cx, obj, array_proto,
		                &MFStringClass,
		                MFStringCons, 0, // constructor function, min arg count
		                0, 0,
		                0, 0))	  // static properties and methods
              theSystem->error("JS_InitClass MFString failed\n");

            // MFVec2f class
            if (! JS_InitClass( cx, obj, array_proto,
		                &MFVec2fClass,
		                MFVec2fCons, 0, // constructor function, min arg count
		                0, 0,
		                0, 0))	  // static properties and methods
              theSystem->error("JS_InitClass MFVec2f failed\n");

            // MFVec3f class
            if (! JS_InitClass( cx, obj, array_proto,
		                &MFVec3fClass,
		                MFVec3fCons, 0, // constructor function, min arg count
		                0, 0,
		                0, 0))	  // static properties and methods
              theSystem->error("JS_InitClass MFVec3f failed\n");

        }
        
        JSBool floatsToJSArray(const size_t numFloats, const float * floats,
                               JSContext * const cx, jsval * const rval) {
            static const size_t MAX_FIXED = 20;
            jsval jsfixed[MAX_FIXED];
            jsval * jsvec = (numFloats > MAX_FIXED)
                            ? new jsval[numFloats]
                            : jsfixed;

            size_t i;
            for (i = 0; i < numFloats; ++i) {
                jsdouble * d = JS_NewDouble(cx, floats[i]);
                if (d && JS_AddRoot(cx, d)) {
                    jsvec[i] = DOUBLE_TO_JSVAL(d);
                } else {
                    break;
                }
            }

            if (i == numFloats) {
                JSObject * arr = JS_NewArrayObject(cx, numFloats, jsvec);
                if (arr) {
                    *rval = OBJECT_TO_JSVAL(arr);
                }
            }

            for (size_t j = 0; j < i; ++j) {
                JS_RemoveRoot(cx, JSVAL_TO_GCTHING(jsvec[j]));
            }
            if (numFloats > MAX_FIXED) {
                delete [] jsvec;
            }
            return (i == numFloats) ? JS_TRUE : JS_FALSE;
        }


        // Convert a jsval to a (new) VrmlField

        VrmlField * createVrmlFieldFromJsval(JSContext * cx, jsval v,
                                        VrmlField::VrmlFieldType expectType) {
            if (JSVAL_IS_NULL(v) || JSVAL_IS_VOID(v)) {
                return 0;
            } else if (JSVAL_IS_BOOLEAN(v) && expectType == VrmlField::SFBOOL) {
                return new VrmlSFBool(JSVAL_TO_BOOLEAN(v));
            } else if (JSVAL_IS_INT(v) && expectType == VrmlField::SFINT32) {
                return new VrmlSFInt32(JSVAL_TO_INT(v));
            } else if (JSVAL_IS_NUMBER(v) && expectType == VrmlField::SFFLOAT) {
                jsdouble d;
                JS_ValueToNumber(cx, v, &d);
                return new VrmlSFFloat(d);
            }

            else if (JSVAL_IS_NUMBER(v) && expectType == VrmlField::SFTIME)
              {
                jsdouble d;
                JS_ValueToNumber( cx, v, &d );
                return new VrmlSFTime( d );
              }

            else if (JSVAL_IS_STRING(v) && expectType == VrmlField::SFSTRING)
              {
                JSString *str = JSVAL_TO_STRING(v);
                return new VrmlSFString( str ? JS_GetStringBytes(str) : "" );
              }

            else if (JSVAL_IS_OBJECT(v) &&
	             ((expectType == VrmlField::SFCOLOR &&
	               &SFColor::jsclass == JS_GetClass( JSVAL_TO_OBJECT(v) )) ||
	              (expectType == VrmlField::SFIMAGE &&
	               &SFImage::jsclass == JS_GetClass( JSVAL_TO_OBJECT(v) )) ||
	              (expectType == VrmlField::SFNODE &&
	               &SFNode::jsclass == JS_GetClass( JSVAL_TO_OBJECT(v) )) ||
	              (expectType == VrmlField::SFROTATION &&
	               &SFRotation::jsclass == JS_GetClass( JSVAL_TO_OBJECT(v) )) ||
	              (expectType == VrmlField::SFVEC2F &&
	               &SFVec2f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(v) )) ||
	              (expectType == VrmlField::SFVEC3F &&
	               &SFVec3f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(v) )) ))
              {
                VrmlField *f = (VrmlField *)JS_GetPrivate( cx, JSVAL_TO_OBJECT(v) );
                if (! f) return 0;
                return f->clone();
              }

            // MFColor
            else if (JSVAL_IS_OBJECT(v) &&
	             JS_IsArrayObject( cx, JSVAL_TO_OBJECT(v) ) &&
	             expectType == VrmlField::MFCOLOR)
              {
                jsuint len;
                JS_GetArrayLength( cx, JSVAL_TO_OBJECT(v), &len );
                float *data = new float[ 3*len ];
                memset(data, 0, 3*len*sizeof(float));

                for (int i=0; i<(int)len; ++i)
	          {
	            jsval elt;
	            JS_GetElement( cx, JSVAL_TO_OBJECT(v), (jsint) i, &elt );
	            if (JSVAL_IS_OBJECT(elt)) {
	              JSObject *obj = JSVAL_TO_OBJECT(elt);
	              VrmlField *vf = obj ? (VrmlField *)JS_GetPrivate( cx, obj ) : 0;
	              VrmlSFColor *c = vf ? vf->toSFColor() : 0;
	              if (c) {
	                data[3*i] = c->get()[0];
	                data[3*i+1] = c->get()[1];
	                data[3*i+2] = c->get()[2];
	              }
	            }
	          }
                VrmlMFColor *f = new VrmlMFColor( (int)len, data );
                delete [] data;
                return f;
              }

            // MFFloat
            else if (JSVAL_IS_OBJECT(v) &&
	             JS_IsArrayObject( cx, JSVAL_TO_OBJECT(v) ) &&
	             expectType == VrmlField::MFFLOAT)
              {
                jsuint len;
                JS_GetArrayLength( cx, JSVAL_TO_OBJECT(v), &len );
                float *data = new float[ len ];
                memset(data, 0, len*sizeof(float));

                for (int i=0; i<(int)len; ++i)
	          {
	            jsval elt;
	            JS_GetElement( cx, JSVAL_TO_OBJECT(v), (jsint) i, &elt );
	            if (JSVAL_IS_NUMBER(elt)) {
	              jsdouble factor;
	              JS_ValueToNumber( cx, elt, &factor );
	              data[i] = (float) factor;
	            }
	          }
                VrmlMFFloat *f = new VrmlMFFloat( (int)len, data );
                delete [] data;
                return f;
              }

            // MFInt
            else if (JSVAL_IS_OBJECT(v) &&
	             JS_IsArrayObject( cx, JSVAL_TO_OBJECT(v) ) &&
	             expectType == VrmlField::MFINT32)
              {
                jsuint len;
                JS_GetArrayLength( cx, JSVAL_TO_OBJECT(v), &len );
                long *data = new long[ len ];
                memset(data, 0, len*sizeof(long));

                for (size_t i = 0; i < len; ++i)
	          {
	            jsval elt;
	            JS_GetElement( cx, JSVAL_TO_OBJECT(v), (jsint) i, &elt );
	            if (JSVAL_IS_NUMBER(elt)) {
	              jsdouble factor;
	              JS_ValueToNumber( cx, elt, &factor );
	              data[i] = static_cast<long>(factor);
	            }
	          }
                VrmlMFInt32 *f = new VrmlMFInt32(len, data);
                delete [] data;
                return f;
              }

            // MFNode
            else if (JSVAL_IS_OBJECT(v) &&
	             JS_IsArrayObject( cx, JSVAL_TO_OBJECT(v) ) &&
	             expectType == VrmlField::MFNODE)
              {
                jsuint len;
                JS_GetArrayLength( cx, JSVAL_TO_OBJECT(v), &len );
                VrmlMFNode *f = new VrmlMFNode(len);
                for (size_t i = 0; i < len; ++i)
	          {
	            jsval elt;
	            JS_GetElement(cx, JSVAL_TO_OBJECT(v), static_cast<jsint>(i), &elt);
	            VrmlSFNode *child = 0;
	            if (JSVAL_IS_OBJECT(elt) &&
	                &SFNode::jsclass == JS_GetClass( JSVAL_TO_OBJECT(elt) ))
	              child = static_cast<VrmlSFNode *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(elt)));
	            f->setElement(i, child ? child->get() : VrmlNodePtr(0));
	          }
                return f;
              }

            // MFRotation
            else if (JSVAL_IS_OBJECT(v) &&
	             JS_IsArrayObject( cx, JSVAL_TO_OBJECT(v) ) &&
	             expectType == VrmlField::MFROTATION)
              {
                jsuint len;
                JS_GetArrayLength( cx, JSVAL_TO_OBJECT(v), &len );
                float *data = new float[ 4*len ];
                memset(data, 0, 4*len*sizeof(float));

                for (int i=0; i<(int)len; ++i)
	          {
	            jsval elt;
	            JS_GetElement( cx, JSVAL_TO_OBJECT(v), (jsint) i, &elt );
	            if (JSVAL_IS_OBJECT(elt)) {
	              JSObject *obj = JSVAL_TO_OBJECT(elt);
	              VrmlField *vf = obj ? (VrmlField *)JS_GetPrivate( cx, obj ) : 0;
	              VrmlSFRotation *c = vf ? vf->toSFRotation() : 0;
	              if (c) {
	                data[3*i] = c->get()[0];
	                data[3*i+1] = c->get()[1];
	                data[3*i+2] = c->get()[2];
	                data[3*i+3] = c->get()[3];
	              }
	            }
	          }
                VrmlMFRotation *f = new VrmlMFRotation( (int)len, data );
                delete [] data;
                return f;
              }

            // MFString
            else if (JSVAL_IS_OBJECT(v) &&
	             JS_IsArrayObject( cx, JSVAL_TO_OBJECT(v) ) &&
	             expectType == VrmlField::MFSTRING)
              {
                jsuint len;
                JS_GetArrayLength( cx, JSVAL_TO_OBJECT(v), &len );
                char **s = new char*[ len ];
                for (int i=0; i<(int)len; ++i)
	          {
	            jsval elt;
	            JS_GetElement( cx, JSVAL_TO_OBJECT(v), (jsint) i, &elt );
	            JSString *str = 0;
	            if (JSVAL_IS_STRING(elt) &&
	                (str = JSVAL_TO_STRING(elt)) != 0)
	              s[i] = JS_GetStringBytes(str);
	            else
	              s[i] = 0;
	          }
                VrmlMFString *f = new VrmlMFString( (int)len, s );
                delete [] s;
                return f;
              }

            // MFVec2f
            else if (JSVAL_IS_OBJECT(v) &&
	             JS_IsArrayObject( cx, JSVAL_TO_OBJECT(v) ) &&
	             expectType == VrmlField::MFVEC2F)
              {
                jsuint len;
                JS_GetArrayLength( cx, JSVAL_TO_OBJECT(v), &len );
                float *data = new float[ 2*len ];
                memset(data, 0, 2*len*sizeof(float));

                for (int i=0; i<(int)len; ++i)
	          {
	            jsval elt;
	            JS_GetElement( cx, JSVAL_TO_OBJECT(v), (jsint) i, &elt );
	            if (JSVAL_IS_OBJECT(elt)) {
	              JSObject *obj = JSVAL_TO_OBJECT(elt);
	              VrmlField *vf = obj ? (VrmlField *)JS_GetPrivate( cx, obj ) : 0;
	              VrmlSFVec2f *c = vf ? vf->toSFVec2f() : 0;
	              if (c) {
	                data[3*i] = c->get()[0];
	                data[3*i+1] = c->get()[1];
	              }
	            }
	          }
                VrmlMFVec2f *f = new VrmlMFVec2f( (int)len, data );
                delete [] data;
                return f;
              }

            // MFVec3f
            else if (JSVAL_IS_OBJECT(v) &&
	             JS_IsArrayObject( cx, JSVAL_TO_OBJECT(v) ) &&
	             expectType == VrmlField::MFVEC3F)
              {
                jsuint len;
                JS_GetArrayLength( cx, JSVAL_TO_OBJECT(v), &len );
                float *data = new float[ 3*len ];
                memset(data, 0, 3*len*sizeof(float));

                for (int i=0; i<(int)len; ++i)
	          {
	            jsval elt;
	            JS_GetElement( cx, JSVAL_TO_OBJECT(v), (jsint) i, &elt );
	            if (JSVAL_IS_OBJECT(elt)) {
	              JSObject *obj = JSVAL_TO_OBJECT(elt);
	              VrmlField *vf = obj ? (VrmlField *)JS_GetPrivate( cx, obj ) : 0;
	              VrmlSFVec3f *c = vf ? vf->toSFVec3f() : 0;
	              if (c) {
	                data[3*i] = c->get()[0];
	                data[3*i+1] = c->get()[1];
	                data[3*i+2] = c->get()[2];
	              }
	            }
	          }
                VrmlMFVec3f *f = new VrmlMFVec3f( (int)len, data );
                delete [] data;
                return f;
              }

            else
              {
                theSystem->error("jsvalToVrmlField: unhandled type (expected %d)\n",
	                expectType);
                JSString *str = JS_ValueToString(cx, v);
                if (str)
	          theSystem->error("%s", JS_GetStringBytes(str));

                return 0;
              }
        }

        // Get a handle to the ScriptJS from the global object
        ScriptJS * objToScript(JSContext * cx, JSObject * obj) {
            jsval val;
            if (! JS_LookupProperty( cx, obj, "_script", &val ))
              theSystem->error("JS_LookupProperty _script failed\n");

            else if (! JSVAL_IS_INT(val)) // Actually JSVAL_IS_PRIVATE
              theSystem->error("JS_LookupProperty _script failed- NULL _script\n");

            return (ScriptJS*) JSVAL_TO_PRIVATE(val);
        }

        // Get a pointer to an eventOut from an object
        char * objToEventOut(JSContext * const cx, JSObject * const obj) {
            jsval val;
            if (JS_LookupProperty(cx, obj, "_eventOut", &val)
                    && JSVAL_IS_INT(val)) {
                return (char*) JSVAL_TO_PRIVATE(val);
            }
            return 0;
        }

        // Check whether we are modifying a prop on an eventOut object, and if so,
        // notify the script.
        void checkEventOut(JSContext * const cx, JSObject * const obj,
                           const VrmlField & val) {
            char * eventOut = 0;
            ScriptJS * script = 0;
            if ((eventOut = objToEventOut(cx, obj))
                    && (script = objToScript(cx, JS_GetParent(cx, obj)))) {
                VrmlNodeScript & scriptNode = script->getScriptNode();
                scriptNode.setEventOut(eventOut, val);
            }
        }

        namespace Global {

            JSBool print(JSContext * const cx, JSObject *,
                         const uintN argc, jsval * const argv, jsval *) {
                for (uintN i = 0; i < argc; i++) {
                    JSString * const str = JS_ValueToString(cx, argv[i]);
                    if (!str) {
                        return JS_FALSE;
                    }
                    cout << JS_GetStringBytes(str) << "\n";
                }
                cout << flush;
                return JS_TRUE;
            }
        }

        namespace Browser {

            JSBool getName(JSContext *cx, JSObject *b,
		           uintN , jsval *, jsval * rval) {
              jsval p;
              if (! JS_GetProperty( cx, b, "_script", &p ))
                return JS_FALSE;

              ScriptJS *s = (ScriptJS *)JSVAL_TO_PRIVATE(p);
              if (! s || ! s->getScriptNode().scene() )
                return JS_FALSE;

              const char *name = s->getScriptNode().scene()->getName();
              *rval = STRING_TO_JSVAL(JS_InternString( cx, name ));
              return JS_TRUE;
            }

            JSBool getVersion(JSContext *cx, JSObject *b,
		                  uintN , jsval *,
		                  jsval *rval)
            {
              jsval p;
              if (! JS_GetProperty( cx, b, "_script", &p ))
                return JS_FALSE;

              ScriptJS *s = (ScriptJS *)JSVAL_TO_PRIVATE(p);
              if (! s || ! s->getScriptNode().scene() )
                return JS_FALSE;

              const char *version = s->getScriptNode().scene()->getVersion();
              *rval = STRING_TO_JSVAL(JS_InternString( cx, version ));
              return JS_TRUE;
            }

            JSBool getCurrentSpeed(JSContext *cx, JSObject*,
			                  uintN, jsval *,
			                  jsval *rval)
            {
              *rval = DOUBLE_TO_JSVAL(JS_NewDouble( cx, 0.0 )); //...
              return JS_TRUE;
            }

            JSBool getCurrentFrameRate(JSContext *cx, JSObject* b,
				              uintN, jsval*, jsval *rval)
            {
              jsval p;
              if (! JS_GetProperty( cx, b, "_script", &p ))
                return JS_FALSE;

              ScriptJS *s = (ScriptJS *)JSVAL_TO_PRIVATE(p);
              if (! s || ! s->getScriptNode().scene())
                return JS_FALSE;

              *rval = DOUBLE_TO_JSVAL(JS_NewDouble(cx, s->getScriptNode().scene()->getFrameRate() ));
              return JS_TRUE;
            }

            JSBool getWorldURL(JSContext *cx, JSObject *b,
			              uintN, jsval *,
			              jsval *rval)
            {
              jsval p;
              if (! JS_GetProperty( cx, b, "_script", &p ))
                return JS_FALSE;

              ScriptJS *s = (ScriptJS *)JSVAL_TO_PRIVATE(p);
              if (! s || ! s->getScriptNode().scene())
                return JS_FALSE;

              const char *url = 0;
              if (s->getScriptNode().scene()->urlDoc())
                url = s->getScriptNode().scene()->urlDoc()->url();
              if (! url) url = "";
              *rval = STRING_TO_JSVAL(JS_InternString( cx, url ));
              return JS_TRUE;
            }


            // No events will be processed after loadURL.

            JSBool loadURL(JSContext* cx, JSObject* b,
		                  uintN argc, jsval *argv, jsval *)
            {
              if ((int)argc < 1)
                return JS_FALSE;

              jsval p;
              if (! JS_GetProperty( cx, b, "_script", &p ))
                return JS_FALSE;

              ScriptJS *s = (ScriptJS *)JSVAL_TO_PRIVATE(p);
              if (! s || ! s->getScriptNode().scene())
                return JS_FALSE;

              // Get 2 arguments MFString url, MFString parameters
              VrmlMFString *url, *parameters = 0;
              url = (VrmlMFString*) createVrmlFieldFromJsval( cx, argv[0], VrmlField::MFSTRING );
              if (! url) return JS_TRUE;	// empty URL

              if ((int)argc > 1)
                parameters = (VrmlMFString*) createVrmlFieldFromJsval( cx, argv[1],
						               VrmlField::MFSTRING );

              s->getScriptNode().scene()->queueLoadUrl( url, parameters );
              return JS_TRUE;
            }


            // This does return, but no events will be processed after it is called.

            JSBool replaceWorld(JSContext *cx, JSObject *b,
			               uintN argc, jsval *argv, jsval *)
            {
              if ((int)argc < 1)
                return JS_FALSE;

              jsval p;
              if (! JS_GetProperty( cx, b, "_script", &p ))
                return JS_FALSE;

              ScriptJS *s = (ScriptJS *)JSVAL_TO_PRIVATE(p);
              if (! s || ! s->getScriptNode().scene())
                return JS_FALSE;

              // Get 1 argument MFNode nodes
              VrmlMFNode *nodes = (VrmlMFNode*) createVrmlFieldFromJsval( cx,
						                  argv[0],
						                  VrmlField::MFNODE );
              if (nodes)
                {
                  VrmlNamespace *ns = new VrmlNamespace(); // should be stored with nodes...
                  s->getScriptNode().scene()->queueReplaceNodes( nodes, ns );
                }

              return JS_TRUE;
            }

            JSBool createVrmlFromString(JSContext *cx, JSObject* bobj,
				        uintN argc, jsval *argv, jsval *rval)
            {
              JSString *str;

              if (argc == 1 &&
                  JSVAL_IS_STRING(argv[0]) &&
                  (str = JSVAL_TO_STRING(argv[0])) != 0)
                {
                  char *vrmlString = JS_GetStringBytes(str);
                  VrmlNamespace ns;
                  VrmlMFNode kids(VrmlScene::readString(vrmlString, &ns));

                  if (kids.getLength() == 0)
	            return JS_FALSE;

                  // Put the children from g into an MFNode and return in rval.
                  // should store the namespace as well...
                  size_t i, n = kids.getLength();
                  jsval *jsvec = new jsval[n];

                  for (i=0; i<n; ++i)
	            {
	              JSObject *obj = JS_NewObject(cx, &SFNode::jsclass, 0, bobj);
	              if (! obj) return JS_FALSE;
	              //uwe JS_SetPrivate( cx, obj, k[i] ? k[i]->reference() : 0);
	              JS_SetPrivate(cx, obj, new VrmlSFNode(kids.getElement(i)));
	              jsvec[i] = OBJECT_TO_JSVAL(obj);
	            }
                  *rval = OBJECT_TO_JSVAL(JS_NewArrayObject(cx, (jsint)n, jsvec));
                  delete [] jsvec;
                  return JS_TRUE;
                }

              return JS_FALSE;
            }


            // createVrmlFromURL( MFString url, SFNode node, SFString event )

            JSBool createVrmlFromURL(JSContext* cx, JSObject* b,
				     uintN argc, jsval *argv, jsval *)
            {
              if ((int)argc != 3) return JS_FALSE;

              Doc2 * relative = 0;

              jsval p;
              ScriptJS *s = 0;		// egcs warning
              if (JS_GetProperty( cx, b, "_script", &p ) &&
                  (s = (ScriptJS *)JSVAL_TO_PRIVATE(p)) != 0 &&
                  s->getScriptNode().scene())
                relative = s->getScriptNode().scene()->urlDoc();

              VrmlMFString *url = 0;
              VrmlField *f;
              if (JSVAL_IS_STRING(argv[0]))
                {
                  JSString *str = JSVAL_TO_STRING(argv[0]);
                  const char * const strBytes = JS_GetStringBytes(str);
                  url = new VrmlMFString(1, &strBytes);
                }
              else if ((f = createVrmlFieldFromJsval( cx, argv[0], VrmlField::MFSTRING )) != 0)
                {
                  url = f->toMFString();
                }

              if (url)
                {
                  VrmlSFNode *node = 0;
                  JSString *str = 0;

                  f = createVrmlFieldFromJsval( cx, argv[1], VrmlField::SFNODE );
                  if ( f != 0 &&
	               (node = f->toSFNode()) != 0 &&
	               JSVAL_IS_STRING(argv[2]) &&
	               (str = JSVAL_TO_STRING(argv[2])) != 0)
	            {
	              char *event = JS_GetStringBytes(str);

	              VrmlNamespace ns;	// this is a problem...
	              VrmlMFNode *kids = VrmlScene::readWrl( url, relative, &ns );
	              VrmlNodePtr nn = node->get();

	              if ( ! kids )
	                {
	                  const char* sUrl = (url->get() && url->get(0)) ?
		            url->get(0) : "(null)";
	                  theSystem->error("createVrmlFromUrl \"%s\" failed.\n", sUrl);

	                  delete node;
	                  delete url;
	                  return JS_FALSE;
	                }

	              if (nn)
	                nn->eventIn( s_timeStamp, // fix me...
			             event,
			             *kids );
	              delete node;
	              delete kids;
	              delete url;
	              return JS_TRUE;
	            }

                  delete url;
                  delete f;
                }
              return JS_FALSE;
            }

            // addRoute(SFNode fromNode, String fromEventOut, SFNode toNode, String toEvent)

            JSBool addRoute(JSContext* cx, JSObject*,
		                   uintN argc, jsval *argv,
		                   jsval *)
            {
              if ((int)argc != 4 ||
                  ! JSVAL_IS_STRING(argv[1]) ||
                  ! JSVAL_IS_STRING(argv[3]))
                return JS_FALSE;
              JSString *str = JS_ValueToString(cx, argv[1]);
              if (! str) return JS_FALSE;
              char *eventOut = JS_GetStringBytes( str );
              str = JS_ValueToString(cx, argv[3]);
              if (! str) return JS_FALSE;
              char *eventIn = JS_GetStringBytes( str );

              if (! eventOut || ! eventIn) return JS_FALSE;
              VrmlField *f = createVrmlFieldFromJsval( cx, argv[0], VrmlField::SFNODE );
              VrmlSFNode *from = f ? f->toSFNode() : 0;
              f = createVrmlFieldFromJsval( cx, argv[2], VrmlField::SFNODE );
              VrmlSFNode *to = f ? f->toSFNode() : 0;
              if (! to || !to->get() || !from || !from->get()) return JS_FALSE;

              theSystem->debug("addRoute(%s::%s::%s, %s::%s::%s)\n",
		               from->get()->nodeType().getName(),
		               from->get()->name(),
		               eventOut,
		               to->get()->nodeType().getName(),
		               to->get()->name(),
		               eventIn);

              (from->get())->addRoute( eventOut, to->get(), eventIn );

              return JS_TRUE;
            }

            // deleteRoute(SFNode fromNode, String fromEventOut, SFNode toNode, String toEvent)

            JSBool deleteRoute(JSContext* cx, JSObject*,
			              uintN argc, jsval *argv, jsval *)
            {
              if ((int)argc != 4 ||
                  ! JSVAL_IS_STRING(argv[1]) ||
                  ! JSVAL_IS_STRING(argv[3]))
                return JS_FALSE;
              JSString *str = JS_ValueToString(cx, argv[1]);
              if (! str) return JS_FALSE;
              char *eventOut = JS_GetStringBytes( str );
              str = JS_ValueToString(cx, argv[3]);
              if (! str) return JS_FALSE;
              char *eventIn = JS_GetStringBytes( str );

              if (! eventOut || ! eventIn) return JS_FALSE;
              VrmlField *f = createVrmlFieldFromJsval( cx, argv[0], VrmlField::SFNODE );
              VrmlSFNode *from = f ? f->toSFNode() : 0;
              f = createVrmlFieldFromJsval( cx, argv[2], VrmlField::SFNODE );
              VrmlSFNode *to = f ? f->toSFNode() : 0;
              if (! to || !to->get() || !from || !from->get()) return JS_FALSE;

              (from->get())->deleteRoute( eventOut, to->get(), eventIn );

              return JS_TRUE;
            }

            JSBool setDescription(JSContext * cx, JSObject *,
			          const uintN argc, jsval * const argv, jsval *) {
                if (argc < 1) {
                    return JS_FALSE;
                }
                JSString * const str = JS_ValueToString(cx, argv[0]);
                if (!str) {
                    return JS_FALSE;
                }
                theSystem->inform("%s", JS_GetStringBytes(str));
                return JS_TRUE;
            }
        }

        namespace Field {
            // Generic VrmlField finalize

            void finalize(JSContext * const cx, JSObject * const obj) {
                delete reinterpret_cast<VrmlField *>(JS_GetPrivate(cx, obj));
                JS_SetPrivate(cx, obj, 0);
            }

            // Generic VrmlField toString

            JSBool toString(JSContext * const cx, JSObject * const obj,
                            uintN, jsval *, jsval * const rval) {
                const VrmlField * const fieldValue =
                        reinterpret_cast<const VrmlField *>(JS_GetPrivate(cx, obj));
                assert(fieldValue);

                ostrstream os;
                os << *fieldValue << '\0';
                const char * ss = os.str();
                JSString * s = JS_NewStringCopyZ(cx, ss);
                os.rdbuf()->freeze(0);
                if (!s) {
                    return JS_FALSE;
                }

                *rval = STRING_TO_JSVAL(s);
                return JS_TRUE;
            }
        }

        namespace SFColor {

            JSObject * initClass(JSContext * const cx, JSObject * const obj) {
                static JSPropertySpec properties[] =
                        { { "r", 0, JSPROP_ENUMERATE },
                          { "g", 1, JSPROP_ENUMERATE },
                          { "b", 2, JSPROP_ENUMERATE },
                          { 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                        // name, func ptr, argc
                        { { "setHSV", setHSV, 3 },
                          { "getHSV", getHSV, 0 },
                          { "toString", toString, 0 },
                          { 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    constructor, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
            }

            namespace {

                JSBool constructor(JSContext * const cx, JSObject * const obj,
	                           const uintN argc, jsval * const argv, jsval *) {
                    jsdouble r = 0.0, g = 0.0, b = 0.0;
                    for (uintN i = 0; i < argc; ++i) {
                        switch (i) {
                        case 0:
                            JS_ValueToNumber(cx, argv[i], &r);
                            break;
                        case 1:
                            JS_ValueToNumber(cx, argv[i], &g);
                            break;
                        case 2:
                            JS_ValueToNumber(cx, argv[i], &b);
                            break;
                        default:
                            // Too many args
                            break;
                        }
                    }

                    try {
                        VrmlSFColor * const sfcolor = new VrmlSFColor(r, g, b);
                        if (!JS_SetPrivate(cx, obj, sfcolor)) {
                            delete sfcolor;
                            return JS_FALSE;
                        }
                    } catch (std::bad_alloc & ex) {
    # ifndef NDEBUG
                        cerr << ex.what() << endl;
    # endif
                        return JS_FALSE;
                    }

                    return JS_TRUE;
                }


                JSBool getProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                    const VrmlSFColor * const sfcolor =
                            reinterpret_cast<const VrmlSFColor *>
                                    (JS_GetPrivate(cx, obj));
                    assert(sfcolor);

                    if (JSVAL_IS_INT(id)) {
                        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) {
                            return JS_FALSE;
                        }
                        *vp = DOUBLE_TO_JSVAL(
                                JS_NewDouble(cx, (*sfcolor)[JSVAL_TO_INT(id)]));
                    }

                    return JS_TRUE;
                }


                JSBool setProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                    VrmlSFColor * const sfcolor =
                            reinterpret_cast<VrmlSFColor *>(JS_GetPrivate(cx, obj));
                    assert(sfcolor);

                    if (!JSVAL_IS_INT(id)
                            || JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) {
                        return JS_FALSE;
                    }

                    jsdouble d;
                    if (!JS_ValueToNumber(cx, *vp, &d)) {
                        return JS_FALSE;
                    }

                    (*sfcolor)[JSVAL_TO_INT(id)] = d;
                    checkEventOut(cx, obj, *sfcolor);

                    return JS_TRUE;
                }

                JSBool setHSV(JSContext * const cx, JSObject * const obj,
                              const uintN argc, jsval * const argv, jsval *) {
                  jsdouble h, s, v;
                  VrmlSFColor *c = (VrmlSFColor *)JS_GetPrivate( cx, obj );
                  if ( c && argc == 3 &&
                       JS_ValueToNumber( cx, argv[0], &h ) &&
                       JS_ValueToNumber( cx, argv[1], &s ) &&
                       JS_ValueToNumber( cx, argv[2], &v ) )
                    {
                      c->setHSV( h, s, v );
                      return JS_TRUE;
                    }

                  return JS_FALSE;
                }

                JSBool getHSV(JSContext * const cx, JSObject * const obj,
                              uintN , jsval *, jsval *rval) {
                    VrmlSFColor *c = (VrmlSFColor *)JS_GetPrivate( cx, obj );
                    if ( c )
                      {
                        float hsv[3];
                        c->getHSV(hsv);
                        return floatsToJSArray( 3, hsv, cx, rval );
                      }
                    return JS_FALSE;
                }
            }
        }

        namespace SFImage {

            JSObject * initClass(JSContext * const cx, JSObject * const obj) {
                static JSPropertySpec properties[] =
                        { { "x", 0, JSPROP_ENUMERATE },
                          { "y", 1, JSPROP_ENUMERATE },
                          { "comp", 2, JSPROP_ENUMERATE },
                          { "array", 3, JSPROP_ENUMERATE },
                          { 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                        // name, func ptr, argc
                        { { "toString", toString, 0 },
                          { 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    constructor, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
            }
            // This is ugly because the spec requires an MFInt array for the pixel data
            // and because giving write access to the image size parameters can cause 
            // the library code to crash unless they are validated somehow...

            namespace {

                JSBool constructor(JSContext * cx, JSObject * obj,
	                           uintN argc, jsval * argv, jsval *)
                {
                  int w = 0, h = 0, nc = 0;
                  unsigned char *pixels = 0;
                  jsdouble x;

                  for (unsigned int i=0; i<argc; ++i)
                    switch (i) {
                    case 0: JS_ValueToNumber( cx, argv[i], &x ); w = (int) x; break;
                    case 1: JS_ValueToNumber( cx, argv[i], &x ); h = (int) x; break;
                    case 2: JS_ValueToNumber( cx, argv[i], &x ); nc = (int) x; break;
                    case 3: /* MFInt image data */
                      if ( JSVAL_IS_OBJECT(argv[i]) &&
	                   JS_IsArrayObject( cx, JSVAL_TO_OBJECT(argv[i])) )
	                {
	                  jsuint len;
	                  JS_GetArrayLength( cx, JSVAL_TO_OBJECT(argv[i]), &len );
	                  if ((int)len != w*h)
	                    return JS_FALSE;
	                  pixels = new unsigned char[ nc*w*h ];
	                  unsigned char *pp = pixels;
	                  for (int ii=0; ii<(int)len; ++ii, pp+=nc)
	                    {
	                      static int byteMask[] = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
	                      jsval elt;
	                      JS_GetElement( cx, JSVAL_TO_OBJECT(argv[i]), (jsint) ii, &elt );
	                      if (JSVAL_IS_NUMBER(elt)) {
		                jsdouble factor;
		                JS_ValueToNumber( cx, elt, &factor );
		                for (int j=0; j<nc; ++j)
		                  pp[j] = (((int) factor) & byteMask[j]) >> (8*j);
	                      }
	                    }
	                }
                      break;
                    default: /* Too many args */ break;
                    }

                  VrmlSFImage *c = new VrmlSFImage( w, h, nc, pixels );
                  JS_SetPrivate(cx, obj, c);
                  return JS_TRUE;
                }

                /**
                 * @brief JavaScript SFImage getter
                 *
                 * @todo Return the pixel array as an MFInt32.
                 */
                JSBool getProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                    const VrmlSFImage * const sfimage =
                            reinterpret_cast<const VrmlSFImage *>(JS_GetPrivate(cx, obj));
                    assert(sfimage);

                    if (JSVAL_IS_INT(id)) {
                        switch (JSVAL_TO_INT(id)) {
                            case 0:
                                *vp = INT_TO_JSVAL(sfimage->getWidth());
                                break;
                            case 1:
                                *vp = INT_TO_JSVAL(sfimage->getHeight());
                                break;
                            case 2:
                                *vp = INT_TO_JSVAL(sfimage->getComponents());
                                break;
                            case 3: // *vp = convert pixels to MFInt...
                            default: return JS_FALSE;
                        }
                    }

                    return JS_TRUE;
                }


                JSBool setProperty(JSContext *, JSObject *, jsval , jsval *)
                {
                  // ...
                  return JS_FALSE;
                }
            }
        }

        namespace SFNode {

            JSObject * initClass(JSContext * const cx, JSObject * const obj) {
                static JSFunctionSpec methods[] =
                        // name, func ptr, argc
                        { { "toString", toString, 0 },
                          { 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    constructor, 1, // constructor function, min arg count
                                    0, methods, // instance properties, methods
                                    0, 0); // static properties and methods
            }

            namespace {

                JSBool constructor(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv, jsval *) {
                    if (argc < 1) {
                        return JS_FALSE;
                    }

                    //
                    // If we have an argument, make sure it is a string.
                    //
                    if (!JSVAL_IS_STRING(argv[0])) {
                        return JS_FALSE;
                    }

                    JSString * const str = JSVAL_TO_STRING(argv[0]);
                    assert(str);

                    VrmlNamespace vrmlNamespace;
                    const VrmlMFNode nodes = VrmlScene::readString(JS_GetStringBytes(str),
                                                                   &vrmlNamespace);
                    //
                    // Fail if the string does not produce exactly one node.
                    //
                    if (nodes.getLength() != 1) {
                        return JS_FALSE;
                    }

                    try {
                        if (!JS_SetPrivate(cx, obj, new VrmlSFNode(nodes.getElement(0)))) {
                            return JS_FALSE;
                        }
                    } catch (std::bad_alloc & ex) {
# ifndef NDEBUG
                        cerr << ex.what() << endl;
# endif
                        return JS_FALSE;
                    }

                    return JS_TRUE;
                }

                // SFNode getProperty reads eventOut values, setProperty sends eventIns.

                JSBool getProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                    VrmlSFNode *n = (VrmlSFNode *)JS_GetPrivate( cx, obj );
                    JSString *str = 0;
                    ScriptJS *script = 0;

                    if (n && n->get() &&
                        JSVAL_IS_STRING(id) && ((str = JSVAL_TO_STRING(id)) != 0) &&
                        (script = objToScript( cx, JS_GetParent( cx, obj ))) != 0)
                      {
                        char *eventOut = JS_GetStringBytes(str);
                        const VrmlField *fieldVal = n->get()->getEventOut( eventOut );

                        // convert event out value to jsval...
                        if (fieldVal)
	                  {
	                    *vp = script->vrmlFieldToJSVal(fieldVal->fieldType(), fieldVal, false);
	                    //return JS_TRUE;
	                  }

                        // If JS_FALSE is returned, apparently the parent object is not searched for
                        // the property, so stuff like toString() will fail... don't know how to
                        // distinguish between a valid prop and an invalid eventOut...
                        // theSystem->error("unrecognized exposedField/eventOut:node.getProperty(%s)\n",
                        //                 JS_GetStringBytes(str));
                        return JS_TRUE;
                      }

                    return JS_FALSE;
                }

                JSBool setProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                    VrmlSFNode * const sfn =
                            reinterpret_cast<VrmlSFNode *>(JS_GetPrivate(cx, obj));
                    assert(sfn);
                    VrmlNodePtr n = sfn->get();

                    if (n && JSVAL_IS_STRING(id))
                      {
                        JSString *str = JSVAL_TO_STRING(id);
                        char *eventIn = str ? JS_GetStringBytes(str) : 0;
                        VrmlField::VrmlFieldType expect;
                        expect = n->nodeType().hasEventIn( eventIn );

                        if (expect && eventIn)	// convert vp to field, send eventIn to n
	                  {

	                    VrmlField *f = createVrmlFieldFromJsval( cx, *vp, expect );
                            if ( ! f ) return JS_FALSE;

                            // This should only happen if directOutput is set...
    # ifdef SCRIPTJS_DEBUG
                            cout << "ScriptJS::node_setProperty sending " << eventIn
                                 << " (" << (*f) << ") to "
                                 << n->nodeType().getName() << "::"
                                 << n->name() << endl;
    # endif

	                    // the timestamp should be stored as a global property and
	                    // looked up via obj somehow...
	                    ScriptJS *script = objToScript( cx, JS_GetParent( cx, obj ));
	                    if (script && script->getScriptNode().scene())
	                      script->getScriptNode().scene()->queueEvent( s_timeStamp, f, n, eventIn );
	                  }

                        checkEventOut(cx, obj, *sfn);
                      }
                    return JS_TRUE;
                }
            }
        }

        namespace SFRotation {

            JSObject * initClass(JSContext * const cx, JSObject * const obj) {
                static JSPropertySpec properties[] =
                        { { "x", 0, JSPROP_ENUMERATE },
                          { "y", 1, JSPROP_ENUMERATE },
                          { "z", 2, JSPROP_ENUMERATE },
                          { "angle", 3, JSPROP_ENUMERATE },
                          { 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                        // name, func ptr, argc
                        { { "getAxis", rot_getAxis, 0 },
                          { "inverse", rot_inverse, 0 },
                          { "multiply", rot_multiply, 1 },
                          { "multVec", rot_multVec, 1 },
                          { "setAxis", rot_setAxis, 1 },
                          { "slerp", rot_slerp, 2 },
                          { "toString", toString, 0 },
                          { 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    constructor, 0, // constructor function, min arg count
                                    properties, methods, // instance properties, methods
                                    0, 0); // static properties and methods
            }

            namespace {

                JSBool constructor(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv, jsval *) {
                    using JavaScript_::SFVec3f;
                    
                    jsdouble x = 0.0, y = 1.0, z = 0.0, angle = 0.0;

                    if (argc > 0)
                      {
                        if ( JSVAL_IS_NUMBER(argv[0]) )
	                  {
	                    JS_ValueToNumber( cx, argv[0], &x );
	                    if (argc > 1) JS_ValueToNumber( cx, argv[1], &y );
	                    if (argc > 2) JS_ValueToNumber( cx, argv[2], &z );
	                    if (argc > 3) JS_ValueToNumber( cx, argv[3], &angle );
	                  }
                        else if (argc == 2 &&
	                         JSVAL_IS_OBJECT(argv[0]) &&
	                         &SFVec3f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ))
	                  {
	                    JSObject *v1obj = JSVAL_TO_OBJECT(argv[0]);
	                    VrmlSFVec3f *v1 = (VrmlSFVec3f *)JS_GetPrivate( cx, v1obj );
	                    if (JSVAL_IS_NUMBER( argv[1] ))
	                      {
	                        x = v1->getX();	// axis/angle
	                        y = v1->getY();
	                        z = v1->getZ();
	                        JS_ValueToNumber( cx, argv[1], &angle );
	                      }
	                    else if ( JSVAL_IS_OBJECT(argv[1]) &&
		                      &SFVec3f::jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[1]) ))
	                      {
	                        JSObject *v2obj = JSVAL_TO_OBJECT(argv[1]);
	                        VrmlSFVec3f *v2 = (VrmlSFVec3f *)JS_GetPrivate( cx, v2obj );
	                        VrmlSFVec3f v3(*v1);  // find rotation that takes v1 to v2
	                        v3 = v3.cross(*v2);    
	                        x = v3.getX();
	                        y = v3.getY();
	                        z = v3.getZ();
	                        angle = acos( v1->dot(*v2) / (v1->length() * v2->length()) );
	                      }
	                    else
	                      return JS_FALSE;
	                  }
                        else
	                  return JS_FALSE;
                      }

                    VrmlSFRotation *v = new VrmlSFRotation( x, y, z, angle );
                    JS_SetPrivate(cx, obj, v);
                    return JS_TRUE;
                }

                JSBool getProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                  VrmlSFRotation *v = (VrmlSFRotation *)JS_GetPrivate( cx, obj );
                  if (v && JSVAL_IS_INT(id))
                    switch (JSVAL_TO_INT(id)) {
                    case 0: *vp = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->getX())); break;
                    case 1: *vp = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->getY())); break;
                    case 2: *vp = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->getZ())); break;
                    case 3: *vp = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->getAngle())); break;
                    default: return JS_FALSE;
                    }
                  return JS_TRUE;
                }

                JSBool setProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                  VrmlSFRotation *v = (VrmlSFRotation *)JS_GetPrivate( cx, obj );
                  jsdouble d;

                  if (v && JSVAL_IS_INT(id) &&
                      JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 4 &&
                      JS_ValueToNumber( cx, *vp, &d ))
                    {
                      (*v)[ JSVAL_TO_INT(id) ] = d;
                      checkEventOut(cx, obj, *v);
                      return JS_TRUE;
                    }

                  return JS_FALSE;
                }
            }
        }

        namespace SFVec2f {

            JSObject * initClass(JSContext * const cx, JSObject * const obj) {
                static JSPropertySpec properties[] =
                        { { "x", 0, JSPROP_ENUMERATE },
                          { "y", 1, JSPROP_ENUMERATE },
                          { 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                  /*    name          native          nargs    */
                        { { "add", vec2f_add, 1 },
                          { "divide", vec2f_divide, 1 },
                          { "dot", vec2f_dot, 1 },
                          { "length", vec2f_length, 0 },
                          { "multiply", vec2f_multiply, 1 },
                          { "negate", vec2f_negate, 0 },
                          { "normalize", vec2f_normalize, 0 },
                          { "subtract", vec2f_subtract, 1 },
                          { "toString", toString, 0 },
                          { 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    constructor, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
            }

            namespace {

                JSBool constructor(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv, jsval *) {
                  jsdouble x = 0.0, y = 0.0;
                  if (argc > 0) JS_ValueToNumber( cx, argv[0], &x );
                  if (argc > 1) JS_ValueToNumber( cx, argv[1], &y );
                  VrmlSFVec2f *v = new VrmlSFVec2f( x, y );
                  JS_SetPrivate(cx, obj, v);
                  return JS_TRUE;
                }


                JSBool getProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                  VrmlSFVec2f *v = (VrmlSFVec2f *)JS_GetPrivate( cx, obj );
                  if (v && JSVAL_IS_INT(id))
                    switch (JSVAL_TO_INT(id)) {
                    case 0: *vp = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->getX())); break;
                    case 1: *vp = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->getY())); break;
                    default: return JS_FALSE;
                    }
                  return JS_TRUE;
                }

                JSBool setProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                  VrmlSFVec2f * const sfVec2f =
                          reinterpret_cast<VrmlSFVec2f *>(JS_GetPrivate(cx, obj));
                  jsdouble d;

                  if (sfVec2f && JSVAL_IS_INT(id) &&
                      JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 2 &&
                      JS_ValueToNumber( cx, *vp, &d ))
                    {
                      (*sfVec2f)[JSVAL_TO_INT(id)] = d;
                      checkEventOut(cx, obj, *sfVec2f);
                      return JS_TRUE;
                    }

                  return JS_FALSE;
                }
            }
        }

        namespace SFVec3f {

            JSObject * initClass(JSContext * const cx, JSObject * const obj) {
                static JSPropertySpec properties[] =
                        { { "x", 0, JSPROP_ENUMERATE },
                          { "y", 1, JSPROP_ENUMERATE },
                          { "z", 2, JSPROP_ENUMERATE },
                          { 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                  /*    name          native          nargs    */
                        { { "add", vec3f_add, 1 },
                          { "cross", vec3f_cross, 1 },
                          { "divide", vec3f_divide, 1 },
                          { "dot", vec3f_dot, 1 },
                          { "length", vec3f_length, 0 },
                          { "multiply", vec3f_multiply, 1 },
                          { "negate", vec3f_negate, 0 },
                          { "normalize", vec3f_normalize, 0 },
                          { "subtract", vec3f_subtract, 1 },
                          { "toString", toString, 0 },
                          { 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    constructor, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
            }

            namespace {

                JSBool constructor(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv, jsval *) {
                  jsdouble x = 0.0, y = 0.0, z = 0.0;
                  if (argc > 0) JS_ValueToNumber( cx, argv[0], &x );
                  if (argc > 1) JS_ValueToNumber( cx, argv[1], &y );
                  if (argc > 2) JS_ValueToNumber( cx, argv[2], &z );
                  VrmlSFVec3f *v = new VrmlSFVec3f( x, y, z );
                  JS_SetPrivate(cx, obj, v);
                  return JS_TRUE;
                }

                JSBool getProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                  VrmlSFVec3f *v = (VrmlSFVec3f *)JS_GetPrivate( cx, obj );
                  if (v && JSVAL_IS_INT(id))
                    switch (JSVAL_TO_INT(id)) {
                    case 0: *vp = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->getX())); break;
                    case 1: *vp = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->getY())); break;
                    case 2: *vp = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->getZ())); break;
                    default: return JS_FALSE;
                    }
                  return JS_TRUE;
                }

                JSBool setProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
                  VrmlSFVec3f *v = (VrmlSFVec3f *)JS_GetPrivate( cx, obj );
                  jsdouble d;

                  if (v && JSVAL_IS_INT(id) &&
                      JSVAL_TO_INT(id) >= 0 && JSVAL_TO_INT(id) < 3 &&
                      JS_ValueToNumber( cx, *vp, &d ))
                    {
                      (*v)[JSVAL_TO_INT(id)] = d;
                      checkEventOut(cx, obj, *v);
                      return JS_TRUE;
                    }

                  return JS_FALSE;
                }
            }
        }
    }
}

# endif // ifndef OPENVRML_DISABLE_JAVASCRIPT
