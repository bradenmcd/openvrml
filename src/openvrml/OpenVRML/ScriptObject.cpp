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

//  ScriptObject.cpp
//  An abstract class to encapsulate the interface between the VRML97
//  scene graph and the supported scripting languages.
//



# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# if defined(_WIN32) && !defined(__CYGWIN__)
#   include <winconfig.h>
# endif

# include <math.h>
# include <string.h>
# if defined(_WIN32) && !defined(__CYGWIN__)
#   include <strstrea.h>
# else
#   include <strstream.h>
# endif
# include "ScriptObject.h"
# include "Doc.h"
# include "doc2.hpp"
# include "field.h"
# include "VrmlNamespace.h"
# include "VrmlNodeType.h"
# include "VrmlNodeScript.h"
# include "VrmlScene.h"

# ifdef HAVE_JAVASCRIPT
#   include <mozilla/jsapi.h>
namespace {

    class ScriptJS: public ScriptObject {
        static JSRuntime * rt;
        static size_t nInstances;

        double d_timeStamp;

        JSContext * d_cx;
        JSObject * d_globalObj;
        JSObject * d_browserObj;

    public:
        ScriptJS(VrmlNodeScript & scriptNode, const char * source);
        virtual ~ScriptJS();

        virtual void activate(double timeStamp, const char * fname,
                              size_t argc, const VrmlField * argv[]);

        VrmlNodeScript & getScriptNode() { return this->scriptNode; }

        jsval vrmlFieldToJSVal(const VrmlField & f, bool protect);

    private:
        void defineFields();
    };
}
# endif

ScriptObject::ScriptObject(VrmlNodeScript & scriptNode): scriptNode(scriptNode)
{}

ScriptObject::~ScriptObject() {}

ScriptObject * ScriptObject::create(VrmlNodeScript & scriptNode,
                                    const VrmlMFString & url) {
    // Try each url until we find one we like
    for (size_t i = 0; i < url.getLength(); ++i) {
        if (! url.getElement(i)) continue;

        // Get the protocol & mimetype...
# ifdef HAVE_JAVASCRIPT
        // Need to handle external .js files too...
        if (strncmp(url.getElement(i), "javascript:", 11) == 0 ||
	    strncmp(url.getElement(i), "vrmlscript:", 11) == 0 )
	{
	    return new ScriptJS(scriptNode, url.getElement(i) + 11);
	}
# endif

#if HAVE_JDK
        int slen = strlen(url[i]);
        if (slen > 6 &&
	    (strcmp(url[i]+slen-6,".class") == 0 ||
	     strcmp(url[i]+slen-6,".CLASS") == 0))
	{
	    Doc *relative = 0;
	    if ( node->scene() ) {
	        relative = node->scene()->url();
            }
	    Doc doc( url[i], relative );
	    if ( doc.localName() ) {
	        return new ScriptJDK( node, doc.urlBase(), doc.localPath() );
            }
	}
#endif
    }

  return 0;
}

namespace {

# ifdef HAVE_JAVASCRIPT
    namespace JavaScript_ {

        const long MAX_HEAP_BYTES = 4L * 1024L * 1024L;
        const long STACK_CHUNK_BYTES = 4024L;

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

            JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
            JSBool toJsval(const VrmlSFColor & sfcolor, bool protect,
                           JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();

            namespace {
                using namespace Field;
                JSBool construct(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval *) throw ();
                JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp) throw ();
                JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp) throw ();
                JSBool setHSV(JSContext * cx, JSObject * obj,
                                    uintN argc, jsval * argv,
                                    jsval * rval) throw ();
                JSBool getHSV(JSContext * cx, JSObject * obj,
                                    uintN argc, jsval * argv,
                                    jsval * rval) throw ();
            }

            JSClass jsclass = {
                "SFColor", JSCLASS_HAS_PRIVATE,
                JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }

        namespace SFImage {

            JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
            JSBool toJsval(const VrmlSFImage & sfcolor, bool protect,
                           JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();

            namespace {
                using namespace Field;
                JSBool construct(JSContext * cx, JSObject * obj,
	                           uintN argc, jsval * argv, jsval *) throw ();
                JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp) throw ();
                JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp) throw ();
            }

            JSClass jsclass = {
                "SFImage", JSCLASS_HAS_PRIVATE,
                JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }

        namespace SFNode {

            JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
            JSBool toJsval(const VrmlSFNode & sfnode, bool protect,
                           JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();

            namespace {
                using namespace Field;
                JSBool construct(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval *) throw ();
                JSBool initObject(JSContext * cx, JSObject * jsval,
                                  uintN argc, jsval * argv) throw ();
                JSBool getProperty(JSContext * cx, JSObject * obj,
                                   jsval id, jsval * vp) throw ();
                JSBool setProperty(JSContext * cx, JSObject * obj,
                                   jsval id, jsval * vp) throw ();
            }

            JSClass jsclass = {
                "SFNode", JSCLASS_HAS_PRIVATE,
                JS_PropertyStub,  JS_PropertyStub, getProperty, setProperty,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }

        namespace SFRotation {

            JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
            JSBool toJsval(const VrmlSFRotation & sfrotation, bool protect,
                           JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();

            namespace {
                using namespace Field;
                JSBool construct(JSContext * cx, JSObject * obj,
	                         uintN argc, jsval * argv, jsval *) throw ();
                JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp) throw ();
                JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp) throw ();
                JSBool getAxis(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval) throw ();
                JSBool inverse(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval);
                JSBool multiply(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval);
                JSBool multVec(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval);
                JSBool setAxis(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval);
                JSBool slerp(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv, jsval * rval);
            }

            JSClass jsclass = {
                "SFRotation", JSCLASS_HAS_PRIVATE,
                JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }

        namespace SFVec2f {

            JSObject * initClass(JSContext * cx, JSObject * obj);
            JSBool toJsval(const VrmlSFVec2f & sfvec2f, bool protect,
                           JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();

            namespace {
                using namespace Field;
                JSBool constructor(JSContext * cx, JSObject * obj,
	                           uintN argc, jsval * argv, jsval *);
                JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
                JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
                JSBool add(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval);
                JSBool divide(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval);
                JSBool dot(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval);
                JSBool length(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval);
                JSBool multiply(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval);
                JSBool negate(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval);
                JSBool normalize(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval * rval);
                JSBool subtract(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval);
            }

            JSClass jsclass = {
              "SFVec2f", JSCLASS_HAS_PRIVATE,
              JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
              JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }

        namespace SFVec3f {

            JSObject * initClass(JSContext * cx, JSObject * obj);
            JSBool toJsval(const VrmlSFVec3f & sfvec3f, bool protect,
                           JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();

            namespace {
                using namespace Field;
                JSBool constructor(JSContext * cx, JSObject * obj,
	                           uintN argc, jsval * argv, jsval *);
                JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
                JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                   jsval * vp);
                JSBool add(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval);
                JSBool cross(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv, jsval * rval);
                JSBool divide(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval);
                JSBool dot(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval);
                JSBool length(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval);
                JSBool multiply(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval);
                JSBool negate(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval);
                JSBool normalize(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval * rval);
                JSBool subtract(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval);
            }

            JSClass jsclass = {
              "SFVec3f", JSCLASS_HAS_PRIVATE,
              JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
              JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
            };
        }
        
        class MField {
            
            friend class ScriptJS;
            friend VrmlField * createVrmlFieldFromJsval(JSContext * cx,
                                jsval v, VrmlField::VrmlFieldType expectType);

        protected:
            typedef std::vector<jsval> JsvalArray;
        
            class MFData {
                JsvalArray * const array;
            public:
                MFData(JsvalArray * array);
                ~MFData();

                JsvalArray & getArray();

            private:
                // Not copyable.
                MFData(const MFData &);
                MFData & operator=(const MFData &);
            };
            
            static JSBool getElement(JSContext * cx, JSObject * obj,
                                     jsval id, jsval * vp);
            static JSBool getLength(JSContext * cx, JSObject * obj,
                                    jsval id, jsval * vp);

        private:
            MField();
        };

        /**
         * @class Base class template for SFColor, SFNode, SFRotation,
         *      SFVec2f, SFVec3f.
         */
        template <typename Subclass>
            class MFJSObject : public MField {
            public:
                static JSObject * initClass(JSContext * cx, JSObject * obj);
            
            protected:
                static JSBool construct(JSContext * cx, JSObject * obj,
	                                uintN argc, jsval * argv,
                                        jsval * vp);
                static JSBool setElement(JSContext * cx, JSObject * obj,
                                         jsval id, jsval * vp);
                static JSBool setLength(JSContext * cx, JSObject * obj,
                                        jsval id, jsval * vp);
                static JSBool toString(JSContext * cx, JSObject * obj,
                                       uintN argc, jsval * argv, jsval * rval);
                static void finalize(JSContext * cx, JSObject * obj);
            
            private:
                static JSBool initObject(JSContext * cx, JSObject * obj,
                                         uintN argc, jsval * argv);
            };

        /**
         * @class Base class template for MFFloat, MFTime.
         *
         * In JavaScript, all floating point values are treated as doubles.
         */
        template <typename Subclass>
            class MFJSDouble : public MField {
            public:
                static JSObject * initClass(JSContext * cx, JSObject * obj);
            
            protected:
                static JSBool construct(JSContext * cx, JSObject * obj,
	                                uintN argc, jsval * argv,
                                        jsval * vp);
                static JSBool setElement(JSContext * cx, JSObject * obj,
                                         jsval id, jsval * vp);
                static JSBool setLength(JSContext * cx, JSObject * obj,
                                        jsval id, jsval * vp);
                static JSBool toString(JSContext * cx, JSObject * obj,
                                       uintN argc, jsval * argv, jsval * rval);
                static void finalize(JSContext * cx, JSObject * obj);
            
            private:
                static JSBool initObject(JSContext * cx, JSObject * obj,
                                         uintN argc, jsval * argv);
            };
/*        
        class MFColor : public MFJSObject<MFColor> {
        public:
            static JSClass jsclass;
            static JSClass & sfjsclass;
            
            static JSBool toJsval(const VrmlMFColor & mfcolor, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
        };
*/
        class MFColor : public MField {
        public:
            static JSClass jsclass;
            
            static JSObject * initClass(JSContext * cx, JSObject * obj)
                    throw ();
            static JSBool toJsval(const VrmlMFColor & mfcolor, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();

        private:
            static JSBool construct(JSContext * cx, JSObject * obj,
	                            uintN argc, jsval * argv, jsval * vp)
                    throw ();
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv) throw ();
            static JSBool setElement(JSContext * cx, JSObject * obj,
                                     jsval id, jsval * vp) throw ();
            static JSBool setLength(JSContext * cx, JSObject * obj,
                                    jsval id, jsval * vp) throw ();
            static JSBool toString(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static void finalize(JSContext * cx, JSObject * obj) throw ();
        };
                
        class MFFloat : public MFJSDouble<MFFloat> {
        public:
            static JSClass jsclass;
            
            static JSBool toJsval(const VrmlMFFloat & mffloat, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
        };
        
        class MFInt32 : public MField {
        public:
            static JSClass jsclass;
            
            static JSObject * initClass(JSContext * cx, JSObject * obj);
            static JSBool toJsval(const VrmlMFInt32 & mfint32, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();

        private:
            static JSBool construct(JSContext * cx, JSObject * obj,
	                            uintN argc, jsval * argv,
                                    jsval * vp);
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv);
            static JSBool setElement(JSContext * cx, JSObject * obj,
                                     jsval id, jsval * vp);
            static JSBool setLength(JSContext * cx, JSObject * obj,
                                    jsval id, jsval * vp);
            static JSBool toString(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval);
            static void finalize(JSContext * cx, JSObject * obj);
        };
        
        class MFNode : public MFJSObject<MFNode> {
        public:
            static JSClass jsclass;
            static JSClass & sfjsclass;
            
            static JSBool toJsval(const VrmlMFNode & mfnode, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
        };
        
        class MFRotation : public MFJSObject<MFRotation> {
        public:
            static JSClass jsclass;
            static JSClass & sfjsclass;
            
            static JSBool toJsval(const VrmlMFRotation & mfrotation, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
        };
        
        class MFString : public MField {
        public:
            static JSClass jsclass;
            
            static JSObject * initClass(JSContext * cx, JSObject * obj)
                    throw ();
            static JSBool toJsval(const VrmlMFString & mfstring, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();

        private:
            static JSBool construct(JSContext * cx, JSObject * obj,
	                            uintN argc, jsval * argv,
                                    jsval * vp) throw ();
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv) throw ();
            static JSBool setElement(JSContext * cx, JSObject * obj,
                                     jsval id, jsval * vp) throw ();
            static JSBool setLength(JSContext * cx, JSObject * obj,
                                    jsval id, jsval * vp) throw ();
            static JSBool toString(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static void finalize(JSContext * cx, JSObject * obj) throw ();
        };
        
        class MFTime : public MFJSDouble<MFTime> {
        public:
            static JSClass jsclass;
            
            static JSBool toJsval(const VrmlMFTime & mftime, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
        };
        
        class MFVec2f : public MFJSObject<MFVec2f> {
        public:
            static JSClass jsclass;
            static JSClass & sfjsclass;
            
            static JSBool toJsval(const VrmlMFVec2f & mfvec2f, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
        };
        
        class MFVec3f : public MFJSObject<MFVec3f> {
        public:
            static JSClass jsclass;
            static JSClass & sfjsclass;
            
            static JSBool toJsval(const VrmlMFVec3f & mfvec3f, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
        };
    }
    
    JSRuntime * ScriptJS::rt = 0; // Javascript runtime singleton object
    size_t ScriptJS::nInstances = 0; // Number of distinct script objects
    
    void ErrorReporter(JSContext *, const char *, JSErrorReport *);


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
                jsargv[i] = argv[i] ? vrmlFieldToJSVal(*argv[i], true) : JSVAL_NULL;
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

        } catch (std::exception & ex) {
    # ifndef NDEBUG
            cerr << ex.what() << endl;
    # endif
        } catch (...) {
            assert(false);
        }
    }


    // Convert a VrmlField value to a jsval, optionally protect from gc.

    jsval ScriptJS::vrmlFieldToJSVal(const VrmlField & f, const bool protect) {
        using JavaScript_::SFColor;
        using JavaScript_::SFImage;
        using JavaScript_::SFNode;
        using JavaScript_::SFRotation;
        using JavaScript_::SFVec2f;
        using JavaScript_::SFVec3f;
        using JavaScript_::MFColor;
        using JavaScript_::MFFloat;
        using JavaScript_::MFInt32;
        using JavaScript_::MFNode;
        using JavaScript_::MFRotation;
        using JavaScript_::MFString;
        using JavaScript_::MFTime;
        using JavaScript_::MFVec2f;
        using JavaScript_::MFVec3f;

        jsval rval;

        switch (f.fieldType()) {
        case VrmlField::SFBOOL:
	    rval = BOOLEAN_TO_JSVAL(static_cast<const VrmlSFBool &>(f).get());
            break;

        case VrmlField::SFCOLOR:
            if (!SFColor::toJsval(static_cast<const VrmlSFColor &>(f), protect,
                                 this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::SFFLOAT:
            {
	        jsdouble * const d =
                        JS_NewDouble(d_cx, static_cast<const VrmlSFFloat &>(f).get());
	        if (protect) JS_AddRoot( d_cx, d );
	        rval = DOUBLE_TO_JSVAL( d );
                break;
            }

        case VrmlField::SFIMAGE:
            if (!SFImage::toJsval(static_cast<const VrmlSFImage &>(f), protect,
                                 this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::SFINT32:
	    return INT_TO_JSVAL(static_cast<const VrmlSFInt32 &>(f).get());

        case VrmlField::SFNODE:
            if (!SFNode::toJsval(static_cast<const VrmlSFNode &>(f), protect,
                                 this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::SFROTATION:
            if (!SFRotation::toJsval(static_cast<const VrmlSFRotation &>(f), protect,
                                     this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::SFSTRING:
            {
	        JSString * s = JS_NewStringCopyZ(d_cx,
                                        static_cast<const VrmlSFString &>(f).get());
	        if (protect) JS_AddRoot( d_cx, s );
	        rval = STRING_TO_JSVAL(s);
                break;
            }

        case VrmlField::SFTIME:
            {
	        jsdouble *d = JS_NewDouble(d_cx,
                                        static_cast<const VrmlSFTime &>(f).get());
	        if (protect) JS_AddRoot( d_cx, d );
	        rval = DOUBLE_TO_JSVAL(d);
                break;
            }

        case VrmlField::SFVEC2F:
            if (!SFVec2f::toJsval(static_cast<const VrmlSFVec2f &>(f), protect,
                                  this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::SFVEC3F:
            if (!SFVec3f::toJsval(static_cast<const VrmlSFVec3f &>(f), protect,
                                  this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::MFCOLOR:
            if (!MFColor::toJsval(static_cast<const VrmlMFColor &>(f), protect,
                                  this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::MFFLOAT:
            if (!MFFloat::toJsval(static_cast<const VrmlMFFloat &>(f), protect,
                                  this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::MFINT32:
            if (!MFInt32::toJsval(static_cast<const VrmlMFInt32 &>(f), protect,
                                  this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::MFNODE:
            if (!MFNode::toJsval(static_cast<const VrmlMFNode &>(f), protect,
                                  this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::MFROTATION:
            if (!MFRotation::toJsval(static_cast<const VrmlMFRotation &>(f), protect,
                                     this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::MFSTRING:
            if (!MFString::toJsval(static_cast<const VrmlMFString &>(f), protect,
                                   this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::MFTIME:
            if (!MFTime::toJsval(static_cast<const VrmlMFTime &>(f), protect,
                                 this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::MFVEC2F:
            if (!MFVec2f::toJsval(static_cast<const VrmlMFVec2f &>(f), protect,
                                  this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        case VrmlField::MFVEC3F:
            if (!MFVec3f::toJsval(static_cast<const VrmlMFVec3f &>(f), protect,
                                  this->d_cx, this->d_globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
            break;

        default:
            assert(false);
            rval = BOOLEAN_TO_JSVAL(false);
        }

        return rval;
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
          jsval val = vrmlFieldToJSVal(*(*i)->value, false);
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
          assert((*i)->value);
          jsval val = vrmlFieldToJSVal(*(*i)->value, false);
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


    void
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

            if (!MFColor::initClass(cx, obj))
              theSystem->error("JS_InitClass MFColor failed\n");

            if (!MFFloat::initClass(cx, obj))
              theSystem->error("JS_InitClass MFFloat failed\n");
            
            if (!MFInt32::initClass(cx, obj))
              theSystem->error("JS_InitClass MFInt32 failed\n");
            
            if (!MFNode::initClass(cx, obj))
              theSystem->error("JS_InitClass MFNode failed\n");

            if (!MFRotation::initClass(cx, obj))
              theSystem->error("JS_InitClass MFRotation failed\n");

            if (!MFString::initClass(cx, obj))
              theSystem->error("JS_InitClass MFString failed\n");

            if (!MFTime::initClass(cx, obj))
              theSystem->error("JS_InitClass MFTime failed\n");

            if (!MFVec2f::initClass(cx, obj))
              theSystem->error("JS_InitClass MFVec2f failed\n");

            if (!MFVec3f::initClass(cx, obj))
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

        /**
         * @brief Convert a jsval to a (new) VrmlField.
         */
        VrmlField * createVrmlFieldFromJsval(JSContext * cx, jsval v,
                                        VrmlField::VrmlFieldType expectType) {
            switch (expectType) {
            case VrmlField::SFBOOL:
                {
                    assert(JSVAL_IS_BOOLEAN(v));
                    return new VrmlSFBool(JSVAL_TO_BOOLEAN(v));
                }
                
            case VrmlField::SFCOLOR:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v), &SFColor::jsclass, 0));
                    const VrmlSFColor * const sfcolor =
                            reinterpret_cast<VrmlSFColor *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(sfcolor);
                    return sfcolor->clone();
                }
            
            case VrmlField::SFFLOAT:
                {
                    assert(JSVAL_IS_NUMBER(v));
                    jsdouble sffloatDouble;
                    JS_ValueToNumber(cx, v, &sffloatDouble);
                    return new VrmlSFFloat(sffloatDouble);
                }
            
            case VrmlField::SFINT32:
                {
                    assert(JSVAL_IS_INT(v));
                    return new VrmlSFInt32(JSVAL_TO_INT(v));
                }
            
            case VrmlField::SFIMAGE:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v), &SFImage::jsclass, 0));
                    const VrmlSFImage * const sfimage =
                            reinterpret_cast<VrmlSFImage *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(sfimage);
                    return sfimage->clone();
                }
            
            case VrmlField::SFNODE:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v), &SFNode::jsclass, 0));
                    const VrmlSFNode * const sfnode =
                            reinterpret_cast<VrmlSFNode *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(sfnode);
                    return sfnode->clone();
                }
            
            case VrmlField::SFROTATION:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v), &SFRotation::jsclass, 0));
                    const VrmlSFRotation * const sfrotation =
                            reinterpret_cast<VrmlSFRotation *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(sfrotation);
                    return sfrotation->clone();
                }
            
            case VrmlField::SFSTRING:
                {
                    assert(JSVAL_IS_STRING(v));
                    JSString * const jsstring = JSVAL_TO_STRING(v);
                    //
                    // Is the null check here really necessary? Perhaps this
                    // should be an assertion?
                    //
                    return new VrmlSFString(jsstring
                                            ? JS_GetStringBytes(jsstring)
                                            : "");
                }
            
            case VrmlField::SFTIME:
                {
                    assert(JSVAL_IS_NUMBER(v));
                    jsdouble sftimeDouble;
                    JS_ValueToNumber(cx, v, &sftimeDouble);
                    return new VrmlSFFloat(sftimeDouble);
                }
            
            case VrmlField::SFVEC2F:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v),
                                         &SFVec2f::jsclass, 0));
                    const VrmlSFVec2f * const sfvec2f =
                            reinterpret_cast<VrmlSFVec2f *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(sfvec2f);
                    return sfvec2f->clone();
                }
            
            case VrmlField::SFVEC3F:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v),
                                         &SFVec3f::jsclass, 0));
                    const VrmlSFVec3f * const sfvec3f =
                            reinterpret_cast<VrmlSFVec3f *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(sfvec3f);
                    return sfvec3f->clone();
                }
            
            case VrmlField::MFCOLOR:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v),
                                         &MFColor::jsclass, 0));
                    MField::MFData * const mfdata =
                            reinterpret_cast<MField::MFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(mfdata);
                    VrmlMFColor * mfcolor =
                            new VrmlMFColor(mfdata->getArray().size());
                    for (MField::JsvalArray::size_type i = 0;
                            i < mfdata->getArray().size(); ++i) {
                        assert(JSVAL_IS_OBJECT(mfdata->getArray()[i]));
                        assert(JS_InstanceOf(cx,
                                             JSVAL_TO_OBJECT(mfdata->getArray()[i]),
                                             &SFColor::jsclass, 0));
                        const VrmlSFColor * const sfcolor =
                                reinterpret_cast<VrmlSFColor *>
                                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->getArray()[i])));
                        assert(sfcolor);
                        mfcolor->setElement(i, sfcolor->get());
                    }

                    return mfcolor;
                }
                
            case VrmlField::MFFLOAT:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v),
                                         &MFFloat::jsclass, 0));
                    MField::MFData * const mfdata =
                            reinterpret_cast<MField::MFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(mfdata);
                    VrmlMFFloat * mffloat =
                            new VrmlMFFloat(mfdata->getArray().size());
                    for (MField::JsvalArray::size_type i = 0;
                            i < mfdata->getArray().size(); ++i) {
                        assert(JSVAL_IS_DOUBLE(mfdata->getArray()[i]));
                        mffloat->setElement(i,
                                        *JSVAL_TO_DOUBLE(mfdata->getArray()[i]));
                    }

                    return mffloat;
                }
            
            case VrmlField::MFINT32:
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
            
            case VrmlField::MFNODE:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v),
                                         &MFNode::jsclass, 0));
                    MField::MFData * const mfdata =
                            reinterpret_cast<MField::MFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(mfdata);
                    VrmlMFNode * mfnode =
                            new VrmlMFNode(mfdata->getArray().size());
                    for (MField::JsvalArray::size_type i = 0;
                            i < mfdata->getArray().size(); ++i) {
                        assert(JSVAL_IS_OBJECT(mfdata->getArray()[i]));
                        assert(JS_InstanceOf(cx,
                                             JSVAL_TO_OBJECT(mfdata->getArray()[i]),
                                             &SFNode::jsclass, 0));
                        const VrmlSFNode * const sfnode =
                                reinterpret_cast<VrmlSFNode *>
                                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->getArray()[i])));
                        assert(sfnode);
                        mfnode->setElement(i, sfnode->get());
                    }

                    return mfnode;
                }

            case VrmlField::MFROTATION:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v),
                                         &MFRotation::jsclass, 0));
                    MField::MFData * const mfdata =
                            reinterpret_cast<MField::MFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(mfdata);
                    VrmlMFRotation * mfrotation =
                            new VrmlMFRotation(mfdata->getArray().size());
                    for (MField::JsvalArray::size_type i = 0;
                            i < mfdata->getArray().size(); ++i) {
                        assert(JSVAL_IS_OBJECT(mfdata->getArray()[i]));
                        assert(JS_InstanceOf(cx,
                                             JSVAL_TO_OBJECT(mfdata->getArray()[i]),
                                             &SFRotation::jsclass, 0));
                        const VrmlSFRotation * const sfrotation =
                                reinterpret_cast<VrmlSFRotation *>
                                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->getArray()[i])));
                        assert(sfrotation);
                        mfrotation->setElement(i, sfrotation->get());
                    }

                    return mfrotation;
                }

            case VrmlField::MFSTRING:
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
            
            case VrmlField::MFTIME:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v),
                                         &MFTime::jsclass, 0));
                    MField::MFData * const mfdata =
                            reinterpret_cast<MField::MFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(mfdata);
                    VrmlMFTime * mftime =
                            new VrmlMFTime(mfdata->getArray().size());
                    for (MField::JsvalArray::size_type i = 0;
                            i < mfdata->getArray().size(); ++i) {
                        assert(JSVAL_IS_DOUBLE(mfdata->getArray()[i]));
                        mftime->setElement(i,
                                        *JSVAL_TO_DOUBLE(mfdata->getArray()[i]));
                    }

                    return mftime;
                }
            
            case VrmlField::MFVEC2F:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v),
                                         &MFVec2f::jsclass, 0));
                    MField::MFData * const mfdata =
                            reinterpret_cast<MField::MFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(mfdata);
                    VrmlMFVec2f * mfvec2f =
                            new VrmlMFVec2f(mfdata->getArray().size());
                    for (MField::JsvalArray::size_type i = 0;
                            i < mfdata->getArray().size(); ++i) {
                        assert(JSVAL_IS_OBJECT(mfdata->getArray()[i]));
                        assert(JS_InstanceOf(cx,
                                             JSVAL_TO_OBJECT(mfdata->getArray()[i]),
                                             &SFVec2f::jsclass, 0));
                        const VrmlSFVec2f * const sfvec2f =
                                reinterpret_cast<VrmlSFVec2f *>
                                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->getArray()[i])));
                        assert(sfvec2f);
                        mfvec2f->setElement(i, sfvec2f->get());
                    }

                    return mfvec2f;
                }

            case VrmlField::MFVEC3F:
                {
                    assert(JSVAL_IS_OBJECT(v));
                    assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(v),
                                         &MFVec3f::jsclass, 0));
                    MField::MFData * const mfdata =
                            reinterpret_cast<MField::MFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(v)));
                    assert(mfdata);
                    VrmlMFVec3f * mfvec3f =
                            new VrmlMFVec3f(mfdata->getArray().size());
                    for (MField::JsvalArray::size_type i = 0;
                            i < mfdata->getArray().size(); ++i) {
                        assert(JSVAL_IS_OBJECT(mfdata->getArray()[i]));
                        assert(JS_InstanceOf(cx,
                                             JSVAL_TO_OBJECT(mfdata->getArray()[i]),
                                             &SFVec3f::jsclass, 0));
                        const VrmlSFVec3f * const sfvec3f =
                                reinterpret_cast<VrmlSFVec3f *>
                                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->getArray()[i])));
                        assert(sfvec3f);
                        mfvec3f->setElement(i, sfvec3f->get());
                    }

                    return mfvec3f;
                }

            default:
                assert(false);
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
	                  const char* sUrl = (url->get() && url->getElement(0))
                                  ? url->getElement(0) : "(null)";
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

            JSObject * initClass(JSContext * const cx, JSObject * const obj)
                    throw () {
                static JSPropertySpec properties[] =
                        { { "r", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "g", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "b", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                        // name, func ptr, argc
                        { { "setHSV", setHSV, 3, 0, 0 },
                          { "getHSV", getHSV, 0, 0, 0 },
                          { "toString", toString, 0, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    construct, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
            }

            JSBool toJsval(const VrmlSFColor & sfcolor, const bool protect,
                           JSContext * const cx, JSObject * const obj,
                           jsval * const rval) throw () {
                JSObject * const sfcolorObj =
                        JS_ConstructObject(cx, &jsclass, 0, obj);
                if (!sfcolorObj) {
                    return JS_FALSE;
                }
                if (protect) {
                    if (!JS_AddRoot(cx, sfcolorObj)) {
                        return JS_FALSE;
                    }
                }
                
                VrmlSFColor * const privateSfcolor =
                        reinterpret_cast<VrmlSFColor *>
                            (JS_GetPrivate(cx, sfcolorObj));
                assert(privateSfcolor);
                *privateSfcolor = sfcolor; // could throw? nah...
                
                *rval = OBJECT_TO_JSVAL(sfcolorObj);
                return JS_TRUE;
            }
            
            namespace {

                JSBool construct(JSContext * const cx, JSObject * const obj,
	                         const uintN argc, jsval * const argv, jsval *)
                        throw () {
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

                    VrmlSFColor * sfcolor = 0;
                    try {
                        sfcolor = new VrmlSFColor(r, g, b);
                    } catch (std::exception & ex) {
# ifndef NDEBUG
                        cerr << ex.what() << endl;
# endif
                        return JS_FALSE;
                    } catch (...) {
                        assert(false);
                        return JS_FALSE;
                    }

                    if (!JS_SetPrivate(cx, obj, sfcolor)) {
                        delete sfcolor; // cleanup
                        return JS_FALSE;
                    }
                    
                    return JS_TRUE;
                }


                JSBool getProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) throw () {
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
                                   const jsval id, jsval * const vp) throw () {
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
                              const uintN argc, jsval * const argv, jsval *)
                        throw () {
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
                              uintN, jsval *, jsval * rval) throw () {
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

            JSObject * initClass(JSContext * const cx, JSObject * const obj)
                    throw () {
                static JSPropertySpec properties[] =
                        { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "comp", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "array", 3, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                        // name, func ptr, argc
                        { { "toString", toString, 0, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    construct, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
            }
            
            JSBool toJsval(const VrmlSFImage & sfimage, const bool protect,
                           JSContext * const cx, JSObject * const obj,
                           jsval * rval) throw () {
	        JSObject * const sfimageObj =
                        JS_ConstructObject(cx, &jsclass, 0, obj);
	        if (!sfimageObj) {
                    return JS_FALSE;
                }
	        if (protect) {
                    if (!JS_AddRoot(cx, sfimageObj)) {
                        return JS_FALSE;
                    }
                }
                
                VrmlSFImage * const privateSfimage =
                        reinterpret_cast<VrmlSFImage *>
                            (JS_GetPrivate(cx, sfimageObj));
                assert(privateSfimage);
                *privateSfimage = sfimage; // could throw? nah...
                
                *rval = OBJECT_TO_JSVAL(sfimageObj);
                return JS_TRUE;
            }
            
            // This is ugly because the spec requires an MFInt array for the pixel data
            // and because giving write access to the image size parameters can cause 
            // the library code to crash unless they are validated somehow...

            namespace {

                JSBool construct(JSContext * const cx, JSObject * const obj,
	                         const uintN argc, jsval * const argv, jsval *)
                        throw () {
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
                                   const jsval id, jsval * const vp) throw () {
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
                        throw () {
                  // ...
                  return JS_FALSE;
                }
            }
        }

        namespace SFNode {

            JSObject * initClass(JSContext * const cx, JSObject * const obj)
                    throw () {
                static JSFunctionSpec methods[] =
                        // name, func ptr, argc
                        { { "toString", toString, 0, 0, 0 },
                          { 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    construct, 1, // constructor function, min arg count
                                    0, methods, // instance properties, methods
                                    0, 0); // static properties and methods
            }
            
            JSBool toJsval(const VrmlSFNode & sfnode, const bool protect,
                           JSContext * const cx, JSObject * const obj,
                           jsval * const rval) throw () {
                //
                // The SFNode constructor requires one arg (a vrmlstring),
                // so we can't use JS_ConstructObject here.
                //
                JSObject * const sfnodeObj =
                        JS_NewObject(cx, &jsclass, 0, obj);
	        if (!sfnodeObj) {
                    return JS_FALSE;
                }
                
                VrmlSFNode * sfnodeClone = 0;
                try {
                    sfnodeClone = static_cast<VrmlSFNode *>(sfnode.clone());
                } catch (std::exception & ex) {
#ifndef NDEBUG
                    cerr << ex.what() << endl;
# endif
                    return JS_FALSE;
                } catch (...) {
                    assert(false);
                    return JS_FALSE;
                }
                
                if (!JS_SetPrivate(cx, sfnodeObj, sfnodeClone)) {
                    delete sfnodeClone;
                    return JS_FALSE;
                }
                
	        if (protect) {
                    if (!JS_AddRoot(cx, sfnodeObj)) {
                        delete sfnodeClone;
                        return JS_FALSE;
                    }
                }
                
                *rval = OBJECT_TO_JSVAL(sfnodeObj);
                return JS_TRUE;
            }

            namespace {

                JSBool construct(JSContext * const cx, JSObject * const obj,
                                 const uintN argc, jsval * const argv, jsval *)
                        throw () {
                    return initObject(cx, obj, argc, argv);
                }

                JSBool initObject(JSContext * const cx, JSObject * const obj,
                                  const uintN argc, jsval * const argv)
                        throw () {
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

                    VrmlSFNode * sfnode = 0;
                    try {
                        sfnode = new VrmlSFNode(nodes.getElement(0));
                    } catch (std::exception & ex) {
# ifndef NDEBUG
                        cerr << ex.what() << endl;
# endif
                        return JS_FALSE;
                    }
                    
                    if (!JS_SetPrivate(cx, obj, sfnode)) {
                        delete sfnode; // cleanup
                        return JS_FALSE;
                    }

                    return JS_TRUE;
                }

                // SFNode getProperty reads eventOut values, setProperty sends eventIns.

                JSBool getProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) throw () {
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
	                    *vp = script->vrmlFieldToJSVal(*fieldVal, false);
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
                                   const jsval id, jsval * const vp) throw () {
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

            JSObject * initClass(JSContext * const cx, JSObject * const obj)
                    throw () {
                static JSPropertySpec properties[] =
                        { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "z", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "angle", 3, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                        // name, func ptr, argc
                        { { "getAxis", getAxis, 0, 0, 0 },
                          { "inverse", inverse, 0, 0, 0 },
                          { "multiply", multiply, 1, 0, 0 },
                          { "multVec", multVec, 1, 0, 0 },
                          { "setAxis", setAxis, 1, 0, 0 },
                          { "slerp", slerp, 2, 0, 0 },
                          { "toString", toString, 0, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    construct, 0, // constructor function, min arg count
                                    properties, methods, // instance properties, methods
                                    0, 0); // static properties and methods
            }

            JSBool toJsval(const VrmlSFRotation & sfrotation, const bool protect,
                           JSContext * const cx, JSObject * const obj,
                           jsval * const rval) throw () {
	        JSObject * const sfrotationObj =
                        JS_ConstructObject(cx, &jsclass, 0, obj);
	        if (!sfrotationObj) {
                    return JS_FALSE;
                }
	        if (protect) {
                    if (!JS_AddRoot(cx, sfrotationObj)) {
                        return JS_FALSE;
                    }
                }
                
                VrmlSFRotation * const privateSfrotation =
                        reinterpret_cast<VrmlSFRotation *>
                            (JS_GetPrivate(cx, sfrotationObj));
                assert(privateSfrotation);
                *privateSfrotation = sfrotation; // could throw? nah...
                
                *rval = OBJECT_TO_JSVAL(sfrotationObj);
                return JS_TRUE;
            }
            
            namespace {

                JSBool construct(JSContext * const cx, JSObject * const obj,
                                 const uintN argc, jsval * const argv, jsval *)
                        throw () {
                    using JavaScript_::SFVec3f;
                    
                    jsdouble x = 0.0, y = 1.0, z = 0.0, angle = 0.0;

                    if (argc > 0) {
                        if (JSVAL_IS_NUMBER(argv[0])) {
	                    if (!JS_ValueToNumber(cx, argv[0], &x)) {
                                return JS_FALSE;
                            }
	                    if (argc > 1) {
                                if (!JSVAL_IS_NUMBER(argv[1])
                                        || !JS_ValueToNumber(cx, argv[1], &y)) {
                                    return JS_FALSE;
                                }
                                
	                        if (argc > 2) {
                                    if (!JSVAL_IS_NUMBER(argv[2])
                                            || !JS_ValueToNumber(cx,
                                                                 argv[2], &z)) {
                                        return JS_FALSE;
                                    }
                                    if (argc > 3) {
                                        if (!JSVAL_IS_NUMBER(argv[3])
                                                || !JS_ValueToNumber(cx,
                                                            argv[3], &angle)) {
                                            return JS_FALSE;
                                        }
                                    }
                                }
                            }
                        } else if (JSVAL_IS_OBJECT(argv[0])) {
                            if (!JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                               &SFVec3f::jsclass, 0)) {
                                return JS_FALSE;
                            }
                            VrmlSFVec3f * const argVec1 =
                                    reinterpret_cast<VrmlSFVec3f *>
                                        (JS_GetPrivate(cx,
                                                    JSVAL_TO_OBJECT(argv[0])));
                            assert(argVec1);
                            
                            if (argc > 1) {
                                if (JSVAL_IS_OBJECT(argv[1])) {
                                    if (!JS_InstanceOf(cx,
                                                       JSVAL_TO_OBJECT(argv[1]),
                                                       &SFVec3f::jsclass, 0)) {
                                        return JS_FALSE;
                                    }
                                    
                                    VrmlSFVec3f * const argVec2 =
                                            reinterpret_cast<VrmlSFVec3f *>
                                                (JS_GetPrivate(cx,
                                                    JSVAL_TO_OBJECT(argv[1])));
                                    
                                    VrmlSFVec3f axisVec(argVec1->cross(*argVec2));
                                    x = axisVec.getX();
                                    y = axisVec.getY();
                                    z = axisVec.getZ();
                                    angle = acos(argVec1->dot(*argVec2) /
                                        (argVec1->length() * argVec2->length()));
                                    
                                } else if (JSVAL_IS_NUMBER(argv[1])) {
                                    if (!JS_ValueToNumber(cx, argv[1],
                                                          &angle)) {
                                        return JS_FALSE;
                                    }
                                    x = argVec1->getX();
                                    y = argVec1->getY();
                                    z = argVec1->getZ();

                                } else {
                                    return JS_FALSE;
                                }
                            }
                        } else {
                            return JS_FALSE;
                        }
                    }

                    VrmlSFRotation * sfrotation = 0;
                    try {
                        sfrotation = new VrmlSFRotation(x, y, z, angle);
                    } catch (std::exception & ex) {
# ifndef NDEBUG
                        cerr << ex.what() << endl;
# endif
                        return JS_FALSE;
                    } catch (...) {
                        assert(false);
                        return JS_FALSE;
                    }
                    
                    if (!JS_SetPrivate(cx, obj, sfrotation)) {
                        delete sfrotation;
                        return JS_FALSE;
                    }
                    
                    return JS_TRUE;
                }

                JSBool getProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) throw () {
                    const VrmlSFRotation * const thisRot =
                            reinterpret_cast<VrmlSFRotation *>
                                (JS_GetPrivate(cx, obj));
                    assert(thisRot);
                    
                    if (JSVAL_IS_INT(id)) {
                        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) {
                            return JS_FALSE;
                        }
                        
                        jsdouble * d = JS_NewDouble(cx, (*thisRot)[JSVAL_TO_INT(id)]);
                        if (!d) {
                            return JS_FALSE;
                        }
                        *vp = DOUBLE_TO_JSVAL(d);
                    }
                    
                    return JS_TRUE;
                }

                JSBool setProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) throw () {
                    VrmlSFRotation * const thisRot =
                            reinterpret_cast<VrmlSFRotation *>
                                (JS_GetPrivate(cx, obj));
                    assert(thisRot);
                            
                    if (JSVAL_IS_INT(id)) {
                        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) {
                            return JS_FALSE;
                        }
                        
                        jsdouble d;
                        if (!JS_ValueToNumber(cx, *vp, &d)) {
                            return JS_FALSE;
                        }
                        (*thisRot)[JSVAL_TO_INT(id)] = d;
                        checkEventOut(cx, obj, *thisRot);
                        return JS_TRUE;
                    }

                    return JS_FALSE;
                }
                
                JSBool getAxis(JSContext * const cx, JSObject * const obj,
                               uintN, jsval *, jsval * const rval) throw () {
                    const VrmlSFRotation * const thisRot =
                            reinterpret_cast<const VrmlSFRotation *>
                                (JS_GetPrivate(cx, obj));
                    assert(thisRot);
                    
                    JSObject * const axisObj =
                            JS_ConstructObject(cx, &SFVec3f::jsclass, 0, axisObj);
                    if (!axisObj) {
                        return JS_FALSE;
                    }
                    
                    VrmlSFVec3f * const axisVec =
                            reinterpret_cast<VrmlSFVec3f *>
                                (JS_GetPrivate(cx, axisObj));
                    assert(axisVec);
                    
                    *axisVec = thisRot->getAxis(); // could this throw? nah...
                    
                    *rval = OBJECT_TO_JSVAL(axisObj);
                    return JS_TRUE;
                }

                JSBool inverse(JSContext * const cx, JSObject * const obj,
                               uintN, jsval *, jsval * const rval) {
                    const VrmlSFRotation * const thisRot =
                            reinterpret_cast<const VrmlSFRotation *>
                                (JS_GetPrivate(cx, obj));
                    assert(thisRot);
                    
                    JSObject *robj = 0;

                    if ((robj = JS_NewObject( cx, &SFRotation::jsclass, 0,
			                      JS_GetParent( cx, obj ) )) != 0)
                      {
                        JS_SetPrivate(cx, robj, new VrmlSFRotation(thisRot->inverse()));
                        *rval = OBJECT_TO_JSVAL(robj);
                        return JS_TRUE;
                      }

                    return JS_FALSE;
                }

                JSBool multiply(JSContext * const cx, JSObject * const obj,
                                const uintN argc, jsval * const argv,
                                jsval * const rval) {
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

                JSBool multVec(JSContext * const cx, JSObject * const obj,
                               const uintN argc, jsval * const argv,
	                       jsval * const rval) {
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

                JSBool setAxis(JSContext * const cx, JSObject * const obj,
                               const uintN argc, jsval * const argv,
                               jsval * const rval) {
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

                JSBool slerp(JSContext * const cx, JSObject * const obj,
                             const uintN argc, jsval * const argv,
                             jsval * const rval) {
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
            }
        }

        namespace SFVec2f {

            JSObject * initClass(JSContext * const cx, JSObject * const obj) {
                static JSPropertySpec properties[] =
                        { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                  /*    name          native          nargs    */
                        { { "add", add, 1, 0, 0 },
                          { "divide", divide, 1, 0, 0 },
                          { "dot", dot, 1, 0, 0 },
                          { "length", length, 0, 0, 0 },
                          { "multiply", multiply, 1, 0, 0 },
                          { "negate", negate, 0, 0, 0 },
                          { "normalize", normalize, 0, 0, 0 },
                          { "subtract", subtract, 1, 0, 0 },
                          { "toString", toString, 0, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    constructor, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
            }

            JSBool toJsval(const VrmlSFVec2f & sfvec2f, const bool protect,
                           JSContext * const cx, JSObject * const obj,
                           jsval * const rval) throw () {
	        JSObject * const sfvec2fObj =
                        JS_ConstructObject(cx, &jsclass, 0, obj);
	        if (!sfvec2fObj) {
                    return JS_FALSE;
                }
	        if (protect) {
                    if (!JS_AddRoot(cx, sfvec2fObj)) {
                        return JS_FALSE;
                    }
                }
                
                VrmlSFVec2f * const privateSFVec2f =
                        reinterpret_cast<VrmlSFVec2f *>
                            (JS_GetPrivate(cx, sfvec2fObj));
                assert(privateSFVec2f);
                *privateSFVec2f = sfvec2f; // could throw? nah...
                
                *rval = OBJECT_TO_JSVAL(sfvec2fObj);
                return JS_TRUE;
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
                
                JSBool add(JSContext * const cx, JSObject * const obj,
                           const uintN argc, jsval * const argv,
                           jsval * const rval) {
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

                JSBool divide(JSContext * const cx, JSObject * const obj,
                              const uintN argc, jsval * const argv,
                              jsval * const rval) {
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

                JSBool dot(JSContext * const cx, JSObject * const obj,
                           const uintN argc, jsval * const argv,
                           jsval * const rval) {
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

                JSBool length(JSContext * const cx, JSObject * const obj,
                              uintN, jsval *, jsval * const rval) {
                    VrmlSFVec2f *v = (VrmlSFVec2f *)JS_GetPrivate( cx, obj );
                    if (v)
                      {
                        *rval = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->length()));
                        return JS_TRUE;
                      }
                    return JS_FALSE;
                }

                JSBool multiply(JSContext * const cx, JSObject * const obj,
                                const uintN argc, jsval * const argv,
                                jsval * const rval) {
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

                JSBool negate(JSContext * const cx, JSObject * const obj,
                              uintN, jsval *, jsval * const rval) {
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

                JSBool normalize(JSContext * const cx, JSObject * const obj,
                                 uintN, jsval *, jsval * const rval) {
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

                JSBool subtract(JSContext * const cx, JSObject * const obj,
                                const uintN argc, jsval * const argv,
                                jsval * const rval) {
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
            }
        }

        namespace SFVec3f {

            JSObject * initClass(JSContext * const cx, JSObject * const obj) {
                static JSPropertySpec properties[] =
                        { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { "z", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                  /*    name          native          nargs    */
                        { { "add", add, 1, 0, 0 },
                          { "cross", cross, 1, 0, 0 },
                          { "divide", divide, 1, 0, 0 },
                          { "dot", dot, 1, 0, 0 },
                          { "length", length, 0, 0, 0 },
                          { "multiply", multiply, 1, 0, 0 },
                          { "negate", negate, 0, 0, 0 },
                          { "normalize", normalize, 0, 0, 0 },
                          { "subtract", subtract, 1, 0, 0 },
                          { "toString", toString, 0, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                return JS_InitClass(cx, obj, 0, &jsclass,
                                    constructor, 0, // constructor function, min arg count
                                    properties, methods,
                                    0, 0); // static properties and methods
            }

            JSBool toJsval(const VrmlSFVec3f & sfvec3f, const bool protect,
                           JSContext * const cx, JSObject * const obj,
                           jsval * const rval) throw () {
	        JSObject * const sfvec3fObj =
                        JS_ConstructObject(cx, &jsclass, 0, obj);
	        if (!sfvec3fObj) {
                    return JS_FALSE;
                }
	        if (protect) {
                    if (!JS_AddRoot(cx, sfvec3fObj)) {
                        return JS_FALSE;
                    }
                }
                
                VrmlSFVec3f * const privateSFVec3f =
                        reinterpret_cast<VrmlSFVec3f *>
                            (JS_GetPrivate(cx, sfvec3fObj));
                assert(privateSFVec3f);
                *privateSFVec3f = sfvec3f; // could throw? nah...
                
                *rval = OBJECT_TO_JSVAL(sfvec3fObj);
                return JS_TRUE;
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
                
                JSBool add(JSContext * const cx, JSObject * const obj,
                           const uintN argc, jsval * const argv,
                           jsval * const rval) {
                    const VrmlSFVec3f * const v1 =
                            reinterpret_cast<VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
                    const VrmlSFVec3f * v2 = 0;
                    JSObject *v3obj = 0;

                    // Verify that we have 2 SFVec3f's && create a result
                    if (v1 && argc > 0 &&
                        JSVAL_IS_OBJECT(argv[0]) &&
                        &jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
                        (v2 = reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])))) &&
                        (((VrmlField*)v2)->fieldType() == VrmlField::SFVEC3F) &&
                        (v3obj = JS_NewObject( cx, &jsclass, 0,
			                       JS_GetParent( cx, obj ) )) != 0)
                      {
                        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->add(*v2)));
                        *rval = OBJECT_TO_JSVAL(v3obj);
                        return JS_TRUE;
                      }

                    return JS_FALSE;
                }

                JSBool cross(JSContext * const cx, JSObject * const obj,
                             const uintN argc, jsval * const argv,
                             jsval * const rval) {
                    VrmlSFVec3f *v1 = (VrmlSFVec3f *)JS_GetPrivate( cx, obj );
                    VrmlSFVec3f *v2 = 0;
                    JSObject *v3obj = 0;

                    // Verify that we have 2 SFVec3f's && create a result
                    if (v1 && argc > 0 &&
                        JSVAL_IS_OBJECT(argv[0]) &&
                        &jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
                        (v2 = (VrmlSFVec3f *)JS_GetPrivate( cx, JSVAL_TO_OBJECT(argv[0]))) &&
                        (((VrmlField*)v2)->fieldType() == VrmlField::SFVEC3F) &&
                        (v3obj = JS_NewObject( cx, &jsclass, 0,
			                       JS_GetParent( cx, obj ) )) != 0)
                      {
                        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->cross(*v2)));
                        *rval = OBJECT_TO_JSVAL(v3obj);
                        return JS_TRUE;
                      }

                    return JS_FALSE;
                }

                JSBool divide(JSContext * const cx, JSObject * const obj,
                              const uintN argc, jsval * const argv,
                              jsval * const rval) {
                    VrmlSFVec3f *v1 = (VrmlSFVec3f *)JS_GetPrivate( cx, obj );
                    jsdouble d = 0.0;
                    JSObject *v3obj = 0;

                    // Verify that we have a number && create a result
                    if (v1 && argc > 0 &&
                        JS_ValueToNumber( cx, argv[0], &d ) &&
                        (v3obj = JS_NewObject( cx, &jsclass, 0,
			                       JS_GetParent( cx, obj ) )) != 0)
                      {
                        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->divide(d)));
                        *rval = OBJECT_TO_JSVAL(v3obj);
                        return JS_TRUE;
                      }

                    return JS_FALSE;
                }

                JSBool dot(JSContext * const cx, JSObject * const obj,
                           const uintN argc, jsval * const argv,
                           jsval * const rval) {
                    if (argc > 0 && JSVAL_IS_OBJECT(argv[0])
                            && (&jsclass == JS_GetClass(JSVAL_TO_OBJECT(argv[0])))) {

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

                JSBool length(JSContext * const cx, JSObject * const obj,
                              uintN, jsval *, jsval * const rval) {
                  VrmlSFVec3f *v = (VrmlSFVec3f *)JS_GetPrivate( cx, obj );
                  if (v)
                    {
                      *rval = DOUBLE_TO_JSVAL(JS_NewDouble( cx, v->length()));
                      return JS_TRUE;
                    }
                  return JS_FALSE;
                }

                JSBool multiply(JSContext * const cx, JSObject * const obj,
                                const uintN argc, jsval * const argv,
                                jsval * const rval) {
                    const VrmlSFVec3f * const v1 =
                            reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
                    jsdouble d = 0.0;
                    JSObject *v3obj = 0;

                    // Verify that we have a number && create a result
                    if (v1 && argc > 0 &&
                        JS_ValueToNumber( cx, argv[0], &d ) &&
                        (v3obj = JS_NewObject(cx, &jsclass, 0,
			                       JS_GetParent( cx, obj ) )) != 0)
                      {
                        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->multiply(d)));
                        *rval = OBJECT_TO_JSVAL(v3obj);
                        return JS_TRUE;
                      }

                    return JS_FALSE;
                }

                JSBool negate(JSContext * const cx, JSObject * const obj,
                              uintN, jsval *, jsval * const rval) {
                    const VrmlSFVec3f * const v1 =
                            reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
                    JSObject *v3obj = 0;

                    if (v1 &&
                        (v3obj = JS_NewObject(cx, &jsclass, 0,
			                       JS_GetParent( cx, obj ) )) != 0)
                      {
                        JS_SetPrivate( cx, v3obj, new VrmlSFVec3f(v1->negate()));
                        *rval = OBJECT_TO_JSVAL(v3obj);
                        return JS_TRUE;
                      }

                    return JS_FALSE;
                }

                JSBool normalize(JSContext * const cx, JSObject * const obj,
                                 uintN, jsval *, jsval * const rval) {
                    const VrmlSFVec3f * const v1 =
                            reinterpret_cast<VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
                    JSObject *v3obj = 0;

                    if (v1 &&
                        (v3obj = JS_NewObject(cx, &jsclass, 0,
			                      JS_GetParent( cx, obj ) )) != 0)
                      {
                        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->normalize()));
                        *rval = OBJECT_TO_JSVAL(v3obj);
                        return JS_TRUE;
                      }

                    return JS_FALSE;
                }

                JSBool subtract(JSContext * const cx, JSObject * const obj,
                                const uintN argc, jsval * const argv,
                                jsval * const rval) {
                    const VrmlSFVec3f * const v1 =
                            reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, obj));
                    const VrmlSFVec3f * v2 = 0;
                    JSObject *v3obj = 0;

                    // Verify that we have 2 SFVec3f's && create a result
                    if (v1 && argc > 0 &&
                        JSVAL_IS_OBJECT(argv[0]) &&
                        &jsclass == JS_GetClass( JSVAL_TO_OBJECT(argv[0]) ) &&
                        (v2 = reinterpret_cast<const VrmlSFVec3f *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])))) &&
                        (((VrmlField*)v2)->fieldType() == VrmlField::SFVEC3F) &&
                        (v3obj = JS_NewObject( cx, &jsclass, 0,
			                       JS_GetParent( cx, obj ) )) != 0)
                      {
                        JS_SetPrivate(cx, v3obj, new VrmlSFVec3f(v1->subtract(*v2)));
                        *rval = OBJECT_TO_JSVAL(v3obj);
                        return JS_TRUE;
                      }

                    return JS_FALSE;
                }
            }
        }
        
        
        MField::MFData::MFData(JsvalArray * const array): array(array) {}

        MField::MFData::~MFData() {
            delete array;
        }

        MField::JsvalArray & MField::MFData::getArray() {
            return *this->array;
        }


        JSBool MField::getElement(JSContext * const cx, JSObject * const obj,
                                  const jsval id, jsval * const vp) {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            if (JSVAL_IS_INT(id)) {
                if (JSVAL_TO_INT(id) < 0
                        || JSVAL_TO_INT(id) >= mfdata->getArray().size()) {
                    return JS_FALSE;
                }

                *vp = mfdata->getArray()[JSVAL_TO_INT(id)];
            }

            return JS_TRUE;
        }

        JSBool MField::getLength(JSContext * const cx, JSObject * const obj,
                                 const jsval id, jsval * const vp) {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            *vp = INT_TO_JSVAL(mfdata->getArray().size());
            return JS_TRUE;
        }

        
        template <typename Subclass>
            JSObject * MFJSObject<Subclass>::initClass(JSContext * const cx,
                                                       JSObject * const obj) {
                static JSPropertySpec properties[] =
                        { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
                          { 0, 0, 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                        { { "toString", toString, 0, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                JSObject * proto = JS_InitClass(cx, obj, 0, &Subclass::jsclass,
                                                construct, 0, // constructor function, min arg count
                                                properties, methods,
                                                0, 0); // static properties and methods
                if (!proto || !initObject(cx, proto, 0, 0)) {
                    return 0;
                }

                return proto;
            }

        template <typename Subclass>
            JSBool MFJSObject<Subclass>::construct(JSContext * const cx,
                                                   JSObject * obj,
	                                           const uintN argc,
                                                   jsval * const argv,
                                                   jsval * const rval) {
                //
                // If called without new, replace obj with a new object.
                //
                if (!JS_IsConstructing(cx)) {
                    obj = JS_NewObject(cx, &Subclass::jsclass, 0, 0);
                    if (!obj) {
                        return JS_FALSE;
                    }
                    *rval = OBJECT_TO_JSVAL(obj);
                }
                
                return initObject(cx, obj, argc, argv);
            }
        
        template <typename Subclass>
            JSBool MFJSObject<Subclass>::initObject(JSContext * const cx,
                                                    JSObject * const obj,
                                                    const uintN argc,
                                                    jsval * const argv) {
                try {
                    MFData * const mfdata =
                            new MFData(new JsvalArray(argc));
                    if (!JS_SetPrivate(cx, obj, mfdata)) {
                        return JS_FALSE;
                    }


                    for (uintN i = 0; i < argc; ++i) {
                        if (!JSVAL_IS_OBJECT(argv[i])
                                || !JS_InstanceOf(cx,
                                                  JSVAL_TO_OBJECT(argv[i]),
                                                  &Subclass::sfjsclass, 0)) {
                            return JS_FALSE;
                        }

                        assert(JSVAL_IS_GCTHING(argv[i]));
                        JSBool ok =
                                JS_AddRoot(cx, JSVAL_TO_GCTHING(argv[i]));
                        assert(ok);

                        mfdata->getArray()[i] = argv[i];
                    }

                } catch (std::exception & ex) {
# ifndef NDEBUG
                    cerr << ex.what() << endl;
# endif
                    return JS_FALSE;
                } catch (...) {
                    assert(false);
                    return JS_FALSE;
                }

                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSObject<Subclass>::setElement(JSContext * const cx,
                                                    JSObject * const obj,
                                                    const jsval id,
                                                    jsval * const vp) {
                MFData * const mfdata = reinterpret_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                //
                // Make sure new value is of the correct type.
                //
                if (!JSVAL_IS_OBJECT(*vp)
                        || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(*vp),
                                          &Subclass::sfjsclass, 0)) {
                    return JS_FALSE;
                }

                if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) {
                    return JS_FALSE;
                }

                //
                // Grow array if necessary.
                //
                if (JSVAL_TO_INT(id) >= mfdata->getArray().size()) {
                    jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
                    if (!setLength(cx, obj, 0, &newLength)) {
                        return JS_FALSE;
                    }
                }

                //
                // Protect the new element from garbage collection.
                //
                if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(*vp))) {
                    return JS_FALSE;
                }

                //
                // Allow the old element to be garbage collected.
                //
                JSBool ok = JS_RemoveRoot(cx,
                            JSVAL_TO_GCTHING(mfdata->getArray()[JSVAL_TO_INT(id)]));
                assert(ok);

                //
                // Put the new element in the array.
                //
                mfdata->getArray()[JSVAL_TO_INT(id)] = *vp;

                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSObject<Subclass>::setLength(JSContext * const cx,
                                                   JSObject * const obj,
                                                   const jsval id,
                                                   jsval * const vp) {
                MFData * const mfdata = reinterpret_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) {
                    return JS_FALSE;
                }

                try {
                    if (JSVAL_TO_INT(*vp) > mfdata->getArray().size()) {
                        //
                        // Grow the array; create new elements and protect
                        // them from garbage collection.
                        //
                        const JsvalArray::size_type oldSize =
                                mfdata->getArray().size();
                        mfdata->getArray().resize(JSVAL_TO_INT(*vp));
                        for (JsvalArray::size_type i = oldSize;
                                i < mfdata->getArray().size(); ++i) {
                            JSObject * const element =
                                JS_ConstructObject(cx, &Subclass::sfjsclass,
                                                   0, 0);
                            if (!element) {
                                throw std::bad_alloc();
                            }
                            jsval elementVal = OBJECT_TO_JSVAL(element);
                            if (!JS_AddRoot(cx,
                                    JSVAL_TO_GCTHING(elementVal))) {
                                throw std::bad_alloc();
                            }
                            mfdata->getArray()[i] = elementVal;
                        }
                    } else if (JSVAL_TO_INT(*vp) < mfdata->getArray().size()) {
                        //
                        // Shrink the array; allow the truncated elements to
                        // be garbage collected.
                        //
                        for (JsvalArray::size_type i = JSVAL_TO_INT(*vp);
                                i < mfdata->getArray().size(); ++i) {
                            JSBool ok = JS_RemoveRoot(cx,
                                JSVAL_TO_GCTHING(mfdata->getArray()[i]));
                            assert(ok);
                        }
                        mfdata->getArray().resize(JSVAL_TO_INT(*vp));
                    }
                } catch (std::exception & ex) {
# ifndef NDEBUG
                    cerr << ex.what() << endl;
# endif
                    return JS_FALSE;
                } catch (...) {
                    assert(false);
                    return JS_FALSE;
                }

                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSObject<Subclass>::toString(JSContext * const cx,
                                                  JSObject * const obj,
                                                  uintN, jsval *,
                                                  jsval * const rval) {
                MFData * const mfdata = reinterpret_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
                assert(mfdata);

                ostrstream os;
                os << "[";
                for (JsvalArray::size_type i = 0; i < mfdata->getArray().size();
                        ++i) {
                    assert(JSVAL_IS_OBJECT(mfdata->getArray()[i]));
                    const VrmlField * fieldValue =
                            reinterpret_cast<VrmlField *>(JS_GetPrivate(cx,
                                JSVAL_TO_OBJECT(mfdata->getArray()[i])));
                    assert(fieldValue);
                    os << *fieldValue;
                    if ((i + 1) < mfdata->getArray().size()) {
                        os << ", ";
                    }
                }
                os << "]";

                JSString * jsstr = JS_NewStringCopyZ(cx, os.str());
                
                //
                // Return ownership of the buffer to the ostrstream.
                //
                os.rdbuf()->freeze(0);
                
                if (!jsstr) {
                    return JS_FALSE;
                }

                *rval = STRING_TO_JSVAL(jsstr);
                return JS_TRUE;
            }

        template <typename Subclass>
            void MFJSObject<Subclass>::finalize(JSContext * const cx,
                                                JSObject * const obj) {
                MFData * const mfdata = reinterpret_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                for (JsvalArray::const_iterator i(mfdata->getArray().begin());
                        i != mfdata->getArray().end(); ++i) {
                    JSBool ok = JS_RemoveRoot(cx, JSVAL_TO_GCTHING(*i));
                    assert(ok);
                }
                delete mfdata;
            }


        template <typename Subclass>
            JSObject * MFJSDouble<Subclass>::initClass(JSContext * const cx,
                                                       JSObject * const obj) {
                static JSPropertySpec properties[] =
                        { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
                          { 0, 0, 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                        { { "toString", toString, 0, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                JSObject * proto = JS_InitClass(cx, obj, 0, &Subclass::jsclass,
                                                construct, 0, // constructor function, min arg count
                                                properties, methods,
                                                0, 0); // static properties and methods
                if (!proto || !initObject(cx, proto, 0, 0)) {
                    return 0;
                }

                return proto;
            }

        template <typename Subclass>
            JSBool MFJSDouble<Subclass>::construct(JSContext * const cx,
                                                   JSObject * obj,
	                                           const uintN argc,
                                                   jsval * const argv,
                                                   jsval * rval) {
                //
                // If called without new, replace obj with a new object.
                //
                if (!JS_IsConstructing(cx)) {
                    obj = JS_NewObject(cx, &Subclass::jsclass, 0, 0);
                    if (!obj) {
                        return JS_FALSE;
                    }
                    *rval = OBJECT_TO_JSVAL(obj);
                }
                
                return initObject(cx, obj, argc, argv);
            }
        
        template <typename Subclass>
            JSBool MFJSDouble<Subclass>::initObject(JSContext * const cx,
                                                    JSObject * const obj,
                                                    const uintN argc,
                                                    jsval * const argv) {
                try {
                    MFData * const mfdata =
                            new MFData(new JsvalArray(argc));
                    if (!JS_SetPrivate(cx, obj, mfdata)) {
                        return JS_FALSE;
                    }


                    for (uintN i = 0; i < argc; ++i) {
                        if (!JSVAL_IS_NUMBER(argv[i])) {
                            return JS_FALSE;
                        }

                        //
                        // If the value is an int, convert it to a double.
                        //
                        jsval doubleVal;
                        if (JSVAL_IS_INT(argv[i])) {
                            if (!JS_NewDoubleValue(cx, JSVAL_TO_INT(argv[i]),
                                                   &doubleVal)) {
                                return JS_FALSE;
                            }
                        } else {
                            assert(JSVAL_IS_DOUBLE(argv[i]));
                            doubleVal = argv[i];
                        }
                        
                        //
                        // Protect from garbage collection.
                        //
                        assert(JSVAL_IS_GCTHING(doubleVal));
                        JSBool ok =
                                JS_AddRoot(cx, JSVAL_TO_GCTHING(doubleVal));
                        assert(ok);

                        mfdata->getArray()[i] = doubleVal;
                    }

                } catch (std::exception & ex) {
# ifndef NDEBUG
                    cerr << ex.what() << endl;
# endif
                    return JS_FALSE;
                } catch (...) {
                    assert(false);
                    return JS_FALSE;
                }
                
                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSDouble<Subclass>::setElement(JSContext * const cx,
                                                    JSObject * const obj,
                                                    const jsval id,
                                                    jsval * const vp) {
                MFData * const mfdata = reinterpret_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                //
                // Make sure new value is a number.
                //
                if (!JSVAL_IS_NUMBER(*vp)) {
                    return JS_FALSE;
                }

                //
                // If the new value is an int, convert it to a double.
                //
                jsval doubleVal;
                if (JSVAL_IS_INT(*vp)) {
                    if (!JS_NewDoubleValue(cx, JSVAL_TO_INT(*vp),
                                           &doubleVal)) {
                        return JS_FALSE;
                    }
                } else {
                    assert(JSVAL_IS_DOUBLE(*vp));
                    doubleVal = *vp;
                }
                
                if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) {
                    return JS_FALSE;
                }

                //
                // Grow array if necessary.
                //
                if (JSVAL_TO_INT(id) >= mfdata->getArray().size()) {
                    jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
                    if (!setLength(cx, obj, 0, &newLength)) {
                        return JS_FALSE;
                    }
                }

                //
                // Protect the new element from garbage collection.
                //
                if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(doubleVal))) {
                    return JS_FALSE;
                }

                //
                // Allow the old element to be garbage collected.
                //
                JSBool ok = JS_RemoveRoot(cx,
                        JSVAL_TO_GCTHING(mfdata->getArray()[JSVAL_TO_INT(id)]));
                assert(ok);

                //
                // Put the new element in the array.
                //
                mfdata->getArray()[JSVAL_TO_INT(id)] = doubleVal;

                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSDouble<Subclass>::setLength(JSContext * const cx,
                                                   JSObject * const obj,
                                                   const jsval id,
                                                   jsval * const vp) {
                MFData * const mfdata = reinterpret_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) {
                    return JS_FALSE;
                }

                try {
                    if (JSVAL_TO_INT(*vp) > mfdata->getArray().size()) {
                        //
                        // Grow the array; create new elements and protect
                        // them from garbage collection.
                        //
                        const JsvalArray::size_type oldSize =
                                mfdata->getArray().size();
                        mfdata->getArray().resize(JSVAL_TO_INT(*vp));
                        for (JsvalArray::size_type i = oldSize;
                                i < mfdata->getArray().size(); ++i) {
                            jsval doubleVal;
                            if (!JS_NewDoubleValue(cx, 0.0, &doubleVal)) {
                                throw std::bad_alloc();
                            }
                            if (!JS_AddRoot(cx,
                                    JSVAL_TO_GCTHING(doubleVal))) {
                                throw std::bad_alloc();
                            }
                            mfdata->getArray()[i] = doubleVal;
                        }
                    } else if (JSVAL_TO_INT(*vp) < mfdata->getArray().size()) {
                        //
                        // Shrink the array; allow the truncated elements to
                        // be garbage collected.
                        //
                        for (JsvalArray::size_type i = JSVAL_TO_INT(*vp);
                                i < mfdata->getArray().size(); ++i) {
                            JSBool ok = JS_RemoveRoot(cx,
                                JSVAL_TO_GCTHING(mfdata->getArray()[i]));
                            assert(ok);
                        }
                        mfdata->getArray().resize(JSVAL_TO_INT(*vp));
                    }
                } catch (std::exception & ex) {
# ifndef NDEBUG
                    cerr << ex.what() << endl;
# endif
                    return JS_FALSE;
                }

                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSDouble<Subclass>::toString(JSContext * const cx,
                                                  JSObject * const obj,
                                                  uintN, jsval *,
                                                  jsval * const rval) {
                MFData * const mfdata = reinterpret_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
                assert(mfdata);

                ostrstream os;
                os << "[";
                for (JsvalArray::size_type i = 0; i < mfdata->getArray().size();
                        ++i) {
                    assert(JSVAL_IS_DOUBLE(mfdata->getArray()[i]));
                    os << *JSVAL_TO_DOUBLE(mfdata->getArray()[i]);
                    if ((i + 1) < mfdata->getArray().size()) {
                        os << ", ";
                    }
                }
                os << "]";

                JSString * jsstr = JS_NewStringCopyZ(cx, os.str());
                
                //
                // Return ownership of the buffer to the ostrstream.
                //
                os.rdbuf()->freeze(0);
                
                if (!jsstr) {
                    return JS_FALSE;
                }

                *rval = STRING_TO_JSVAL(jsstr);
                return JS_TRUE;
            }

        template <typename Subclass>
            void MFJSDouble<Subclass>::finalize(JSContext * const cx,
                                                JSObject * const obj) {
                MFData * const mfdata = reinterpret_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                for (JsvalArray::const_iterator i(mfdata->getArray().begin());
                        i != mfdata->getArray().end(); ++i) {
                    JSBool ok = JS_RemoveRoot(cx, JSVAL_TO_GCTHING(*i));
                    assert(ok);
                }
                delete mfdata;
            }

        JSClass MFColor::jsclass =
                { "MFColor", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSObject * MFColor::initClass(JSContext * const cx,
                                      JSObject * const obj) throw () {
            static JSPropertySpec properties[] =
                    { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
                    { { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };
            
            JSObject * proto = JS_InitClass(cx, obj, 0, &jsclass,
                                            construct, 0, // constructor function, min arg count
                                            properties, methods,
                                            0, 0); // static properties and methods
            if (!proto || !initObject(cx, proto, 0, 0)) {
                return 0;
            }

            return proto;
        }

        JSBool MFColor::construct(JSContext * const cx, JSObject * obj,
	                          const uintN argc, jsval * const argv,
                                  jsval * const rval) throw () {
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }

        JSBool MFColor::initObject(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv)
                throw () {
            try {
                MFData * const mfdata = new MFData(new JsvalArray(argc));
                if (!JS_SetPrivate(cx, obj, mfdata)) {
                    return JS_FALSE;
                }


                for (uintN i = 0; i < argc; ++i) {
                    if (!JSVAL_IS_OBJECT(argv[i])
                            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[i]),
                                              &SFColor::jsclass, 0)) {
                        return JS_FALSE;
                    }

                    assert(JSVAL_IS_GCTHING(argv[i]));
                    JSBool ok =
                            JS_AddRoot(cx, JSVAL_TO_GCTHING(argv[i]));
                    assert(ok);

                    mfdata->getArray()[i] = argv[i];
                }

            } catch (std::exception & ex) {
# ifndef NDEBUG
                cerr << ex.what() << endl;
# endif
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool MFColor::setElement(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) throw () {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Make sure new value is of the correct type.
            //
            if (!JSVAL_IS_OBJECT(*vp)
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(*vp),
                                      &SFColor::jsclass, 0)) {
                return JS_FALSE;
            }

            if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) {
                return JS_FALSE;
            }

            //
            // Grow array if necessary.
            //
            if (JSVAL_TO_INT(id) >= mfdata->getArray().size()) {
                jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
                if (!setLength(cx, obj, 0, &newLength)) {
                    return JS_FALSE;
                }
            }

            //
            // Protect the new element from garbage collection.
            //
            if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(*vp))) {
                return JS_FALSE;
            }

            //
            // Allow the old element to be garbage collected.
            //
            JSBool ok = JS_RemoveRoot(cx,
                        JSVAL_TO_GCTHING(mfdata->getArray()[JSVAL_TO_INT(id)]));
            assert(ok);

            //
            // Put the new element in the array.
            //
            mfdata->getArray()[JSVAL_TO_INT(id)] = *vp;

            return JS_TRUE;
        }

        JSBool MFColor::setLength(JSContext * const cx, JSObject * const obj,
                                  const jsval id, jsval * const vp) throw () {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) {
                return JS_FALSE;
            }

            try {
                if (JSVAL_TO_INT(*vp) > mfdata->getArray().size()) {
                    //
                    // Grow the array; create new elements and protect
                    // them from garbage collection.
                    //
                    const JsvalArray::size_type oldSize =
                            mfdata->getArray().size();
                    mfdata->getArray().resize(JSVAL_TO_INT(*vp));
                    for (JsvalArray::size_type i = oldSize;
                            i < mfdata->getArray().size(); ++i) {
                        JSObject * const element =
                            JS_ConstructObject(cx, &SFColor::jsclass, 0, 0);
                        if (!element) {
                            throw std::bad_alloc();
                        }
                        jsval elementVal = OBJECT_TO_JSVAL(element);
                        if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(elementVal))) {
                            throw std::bad_alloc();
                        }
                        mfdata->getArray()[i] = elementVal;
                    }
                } else if (JSVAL_TO_INT(*vp) < mfdata->getArray().size()) {
                    //
                    // Shrink the array; allow the truncated elements to
                    // be garbage collected.
                    //
                    for (JsvalArray::size_type i = JSVAL_TO_INT(*vp);
                            i < mfdata->getArray().size(); ++i) {
                        JSBool ok = JS_RemoveRoot(cx,
                            JSVAL_TO_GCTHING(mfdata->getArray()[i]));
                        assert(ok);
                    }
                    mfdata->getArray().resize(JSVAL_TO_INT(*vp));
                }
            } catch (std::exception & ex) {
# ifndef NDEBUG
                cerr << ex.what() << endl;
# endif
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool MFColor::toString(JSContext * const cx, JSObject * const obj,
                                 uintN, jsval *, jsval * const rval) throw () {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                        (JS_GetPrivate(cx, obj));
            assert(mfdata);

            ostrstream os;
            os << "[";
            for (JsvalArray::size_type i = 0; i < mfdata->getArray().size();
                    ++i) {
                assert(JSVAL_IS_OBJECT(mfdata->getArray()[i]));
                const VrmlSFColor * const sfcolor =
                        reinterpret_cast<VrmlSFColor *>(JS_GetPrivate(cx,
                            JSVAL_TO_OBJECT(mfdata->getArray()[i])));
                assert(sfcolor);
                os << *sfcolor;
                if ((i + 1) < mfdata->getArray().size()) {
                    os << ", ";
                }
            }
            os << "]";

            JSString * jsstr = JS_NewStringCopyZ(cx, os.str());

            //
            // Return ownership of the buffer to the ostrstream.
            //
            os.rdbuf()->freeze(0);

            if (!jsstr) {
                return JS_FALSE;
            }

            *rval = STRING_TO_JSVAL(jsstr);
            return JS_TRUE;
        }

        void MFColor::finalize(JSContext * const cx, JSObject * const obj)
                throw () {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            for (JsvalArray::const_iterator i(mfdata->getArray().begin());
                    i != mfdata->getArray().end(); ++i) {
                JSBool ok = JS_RemoveRoot(cx, JSVAL_TO_GCTHING(*i));
                assert(ok);
            }
            delete mfdata;
        }


        JSBool MFColor::toJsval(const VrmlMFColor & mfcolor, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            JSObject * const mfcolorObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfcolorObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = reinterpret_cast<MFData *>
                                    (JS_GetPrivate(cx, mfcolorObj));

            mfdata->getArray().resize(mfcolor.getLength());

            for (size_t i = 0; i < mfcolor.getLength(); ++i) {
                const VrmlSFColor sfcolor(mfcolor.getElement(i));
                if (!SFColor::toJsval(sfcolor, true, cx, obj,
                                      &mfdata->getArray()[i])) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfcolorObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfcolorObj);
            return JS_TRUE;
        }

        JSClass MFFloat::jsclass =
                { "MFFloat", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSBool MFFloat::toJsval(const VrmlMFFloat & mffloat, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            JSObject * const mffloatObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mffloatObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = reinterpret_cast<MFData *>
                                    (JS_GetPrivate(cx, mffloatObj));

            mfdata->getArray().resize(mffloat.getLength());

            for (size_t i = 0; i < mffloat.getLength(); ++i) {
                if (!JS_NewDoubleValue(cx, mffloat.getElement(i),
                                       &mfdata->getArray()[i])) {
                    return JS_FALSE;
                }
                if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(mfdata->getArray()[i]))) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mffloatObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mffloatObj);
            return JS_TRUE;
        }

        JSClass MFInt32::jsclass =
                { "MFFloat", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSObject * MFInt32::initClass(JSContext * const cx,
                                      JSObject * const obj) {
            static JSPropertySpec properties[] =
                    { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
                    { { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            JSObject * proto = JS_InitClass(cx, obj, 0, &jsclass,
                                            construct, 0, // constructor function, min arg count
                                            properties, methods,
                                            0, 0); // static properties and methods
            if (!proto || !initObject(cx, proto, 0, 0)) {
                return 0;
            }

            return proto;
        }

        JSBool MFInt32::toJsval(const VrmlMFInt32 & mfint32, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            JSObject * const mfint32Obj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfint32Obj) {
                return JS_FALSE;
            }

            MFData * const mfdata = reinterpret_cast<MFData *>
                                    (JS_GetPrivate(cx, mfint32Obj));

            mfdata->getArray().resize(mfint32.getLength());

            for (size_t i = 0; i < mfint32.getLength(); ++i) {
                mfdata->getArray()[i] = INT_TO_JSVAL(mfint32.getElement(i));
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfint32Obj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfint32Obj);
            return JS_TRUE;
        }

        JSBool MFInt32::construct(JSContext * const cx, JSObject * obj,
	                          const uintN argc, jsval * const argv,
                                  jsval * rval) {
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }

        JSBool MFInt32::initObject(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv) {
            try {
                MFData * const mfdata =
                        new MFData(new JsvalArray(argc));
                if (!JS_SetPrivate(cx, obj, mfdata)) {
                    return JS_FALSE;
                }

                for (uintN i = 0; i < argc; ++i) {
                    if (!JSVAL_IS_INT(argv[i])) {
                        return JS_FALSE;
                    }

                    mfdata->getArray()[i] = JSVAL_TO_INT(argv[i]);
                }

            } catch (std::exception & ex) {
# ifndef NDEBUG
                cerr << ex.what() << endl;
# endif
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool MFInt32::setElement(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
            MFData * const mfdata =
                    reinterpret_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Make sure new value is an integer.
            //
            if (!JSVAL_IS_INT(*vp)) {
                return JS_FALSE;
            }

            if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) {
                return JS_FALSE;
            }

            //
            // Grow array if necessary.
            //
            if (JSVAL_TO_INT(id) >= mfdata->getArray().size()) {
                jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
                if (!setLength(cx, obj, 0, &newLength)) {
                    return JS_FALSE;
                }
            }

            //
            // Put the new element in the array.
            //
            mfdata->getArray()[JSVAL_TO_INT(id)] = JSVAL_TO_INT(*vp);

            return JS_TRUE;
        }

        JSBool MFInt32::setLength(JSContext * const cx, JSObject * const obj,
                                  const jsval id, jsval * const vp) {
            MFData * const mfdata =
                    reinterpret_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);

            if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) {
                return JS_FALSE;
            }

            try {
                if (JSVAL_TO_INT(*vp) != mfdata->getArray().size()) {
                    mfdata->getArray().resize(JSVAL_TO_INT(*vp));
                }
            } catch (std::exception & ex) {
# ifndef NDEBUG
                cerr << ex.what() << endl;
# endif
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool MFInt32::toString(JSContext * const cx, JSObject * const obj,
                                 uintN, jsval *, jsval * const rval) {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                        (JS_GetPrivate(cx, obj));
            assert(mfdata);

            ostrstream os;
            os << "[";
            for (JsvalArray::size_type i = 0; i < mfdata->getArray().size();
                    ++i) {
                os << JSVAL_TO_INT(mfdata->getArray()[i]);
                if ((i + 1) < mfdata->getArray().size()) {
                    os << ", ";
                }
            }
            os << "]";

            JSString * jsstr = JS_NewStringCopyZ(cx, os.str());

            //
            // Return ownership of the buffer to the ostrstream.
            //
            os.rdbuf()->freeze(0);

            if (!jsstr) {
                return JS_FALSE;
            }

            *rval = STRING_TO_JSVAL(jsstr);
            return JS_TRUE;
        }

        void MFInt32::finalize(JSContext * const cx, JSObject * const obj) {
            delete reinterpret_cast<MFData *>(JS_GetPrivate(cx, obj));
        }

        JSClass MFNode::jsclass =
                { "MFNode", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSClass & MFNode::sfjsclass = SFNode::jsclass;

        JSBool MFNode::toJsval(const VrmlMFNode & mfnode, const bool protect,
                               JSContext * const cx, JSObject * const obj,
                               jsval * const rval) throw () {
            JSObject * const mfnodeObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfnodeObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = reinterpret_cast<MFData *>
                                    (JS_GetPrivate(cx, mfnodeObj));

            mfdata->getArray().resize(mfnode.getLength());

            for (size_t i = 0; i < mfnode.getLength(); ++i) {
                const VrmlSFNode sfnode(mfnode.getElement(i));
                if (!SFNode::toJsval(sfnode, true, cx, obj,
                                     &mfdata->getArray()[i])) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfnodeObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfnodeObj);
            return JS_TRUE;
        }

        JSClass MFRotation::jsclass =
                { "MFRotation", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSClass & MFRotation::sfjsclass = SFRotation::jsclass;

        JSBool MFRotation::toJsval(const VrmlMFRotation & mfrotation,
                                   const bool protect,
                                   JSContext * const cx, JSObject * const obj,
                                   jsval * const rval) throw () {
            JSObject * const mfrotationObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfrotationObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = reinterpret_cast<MFData *>
                                    (JS_GetPrivate(cx, mfrotationObj));

            mfdata->getArray().resize(mfrotation.getLength());

            for (size_t i = 0; i < mfrotation.getLength(); ++i) {
                const VrmlSFRotation sfrotation(mfrotation.getElement(i));
                if (!SFRotation::toJsval(sfrotation, true, cx, obj,
                                         &mfdata->getArray()[i])) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfrotationObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfrotationObj);
            return JS_TRUE;
        }

        JSClass MFString::jsclass =
                { "MFString", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSObject * MFString::initClass(JSContext * const cx,
                                       JSObject * const obj) throw () {
            static JSPropertySpec properties[] =
                    { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
                    { { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            JSObject * proto = JS_InitClass(cx, obj, 0, &jsclass,
                                            construct, 0, // constructor function, min arg count
                                            properties, methods,
                                            0, 0); // static properties and methods
            if (!proto || !initObject(cx, proto, 0, 0)) {
                return 0;
            }

            return proto;
        }

        JSBool MFString::toJsval(const VrmlMFString & mfstring,
                                 const bool protect,
                                 JSContext * const cx, JSObject * const obj,
                                 jsval * const rval) throw () {
            JSObject * const mfstringObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfstringObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = reinterpret_cast<MFData *>
                                    (JS_GetPrivate(cx, mfstringObj));

            mfdata->getArray().resize(mfstring.getLength());

            for (size_t i = 0; i < mfstring.getLength(); ++i) {
                JSString * jsstring =
                        JS_NewStringCopyZ(cx, mfstring.getElement(i));
                if (!jsstring) {
                    return JS_FALSE;
                }
                if (!JS_AddRoot(cx, jsstring)) {
                    return JS_FALSE;
                }
                mfdata->getArray()[i] = STRING_TO_JSVAL(jsstring);
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfstringObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfstringObj);
            return JS_TRUE;
        }

        JSBool MFString::construct(JSContext * const cx, JSObject * obj,
	                           const uintN argc, jsval * const argv,
                                   jsval * rval) throw () {
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }

        JSBool MFString::initObject(JSContext * const cx, JSObject * const obj,
                                    const uintN argc, jsval * const argv)
                throw () {
            try {
                MFData * const mfdata =
                        new MFData(new JsvalArray(argc));
                if (!JS_SetPrivate(cx, obj, mfdata)) {
                    return JS_FALSE;
                }

                for (uintN i = 0; i < argc; ++i) {
                    if (!JSVAL_IS_STRING(argv[i])) {
                        return JS_FALSE;
                    }

                    //
                    // Protect from garbage collection.
                    //
                    assert(JSVAL_IS_GCTHING(argv[i]));
                    JSBool ok = JS_AddRoot(cx, JSVAL_TO_GCTHING(argv[i]));
                    assert(ok);

                    mfdata->getArray()[i] = argv[i];
                }

            } catch (std::exception & ex) {
# ifndef NDEBUG
                cerr << ex.what() << endl;
# endif
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool MFString::setElement(JSContext * const cx, JSObject * const obj,
                                    const jsval id, jsval * const vp) throw () {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Make sure new value is a string.
            //
            if (!JSVAL_IS_STRING(*vp)) {
                return JS_FALSE;
            }

            if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) {
                return JS_FALSE;
            }

            //
            // Grow array if necessary.
            //
            if (JSVAL_TO_INT(id) >= mfdata->getArray().size()) {
                jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
                if (!setLength(cx, obj, 0, &newLength)) {
                    return JS_FALSE;
                }
            }

            //
            // Protect the new element from garbage collection.
            //
            if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(*vp))) {
                return JS_FALSE;
            }

            //
            // Allow the old element to be garbage collected.
            //
            JSBool ok = JS_RemoveRoot(cx,
                    JSVAL_TO_GCTHING(mfdata->getArray()[JSVAL_TO_INT(id)]));
            assert(ok);

            //
            // Put the new element in the array.
            //
            mfdata->getArray()[JSVAL_TO_INT(id)] = *vp;

            return JS_TRUE;
        }

        JSBool MFString::setLength(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) throw () {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) {
                return JS_FALSE;
            }

            try {
                if (JSVAL_TO_INT(*vp) > mfdata->getArray().size()) {
                    //
                    // Grow the array; create new elements and protect
                    // them from garbage collection.
                    //
                    const JsvalArray::size_type oldSize =
                            mfdata->getArray().size();
                    mfdata->getArray().resize(JSVAL_TO_INT(*vp));
                    for (JsvalArray::size_type i = oldSize;
                            i < mfdata->getArray().size(); ++i) {
                        JSString * jsstring = JS_NewStringCopyZ(cx, "");
                        if (!jsstring) {
                            throw std::bad_alloc();
                        }
                        if (!JS_AddRoot(cx, jsstring)) {
                            throw std::bad_alloc();
                        }
                        mfdata->getArray()[i] = STRING_TO_JSVAL(jsstring);
                    }
                } else if (JSVAL_TO_INT(*vp) < mfdata->getArray().size()) {
                    //
                    // Shrink the array; allow the truncated elements to
                    // be garbage collected.
                    //
                    for (JsvalArray::size_type i = JSVAL_TO_INT(*vp);
                            i < mfdata->getArray().size(); ++i) {
                        JSBool ok = JS_RemoveRoot(cx,
                            JSVAL_TO_GCTHING(mfdata->getArray()[i]));
                        assert(ok);
                    }
                    mfdata->getArray().resize(JSVAL_TO_INT(*vp));
                }
            } catch (std::exception & ex) {
# ifndef NDEBUG
                cerr << ex.what() << endl;
# endif
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool MFString::toString(JSContext * const cx, JSObject * const obj,
                                  uintN, jsval *, jsval * const rval) throw () {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                        (JS_GetPrivate(cx, obj));
            assert(mfdata);

            ostrstream os;
            os << "[";
            for (JsvalArray::size_type i = 0; i < mfdata->getArray().size();
                    ++i) {
                assert(JSVAL_IS_STRING(mfdata->getArray()[i]));
                os << "\""
                   << JS_GetStringBytes(JSVAL_TO_STRING(mfdata->getArray()[i]))
                   << "\"";
                if ((i + 1) < mfdata->getArray().size()) {
                    os << ", ";
                }
            }
            os << "]";

            JSString * jsstr = JS_NewStringCopyZ(cx, os.str());

            //
            // Return ownership of the buffer to the ostrstream.
            //
            os.rdbuf()->freeze(0);

            if (!jsstr) {
                return JS_FALSE;
            }

            *rval = STRING_TO_JSVAL(jsstr);
            return JS_TRUE;
        }

        void MFString::finalize(JSContext * const cx, JSObject * const obj)
                throw () {
            MFData * const mfdata = reinterpret_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            for (JsvalArray::const_iterator i(mfdata->getArray().begin());
                    i != mfdata->getArray().end(); ++i) {
                JSBool ok = JS_RemoveRoot(cx, JSVAL_TO_GCTHING(*i));
                assert(ok);
            }
            delete mfdata;
        }

        JSClass MFTime::jsclass =
                { "MFTime", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSBool MFTime::toJsval(const VrmlMFTime & mftime, const bool protect,
                               JSContext * const cx, JSObject * const obj,
                               jsval * const rval) throw () {
            JSObject * const mftimeObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mftimeObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = reinterpret_cast<MFData *>
                                    (JS_GetPrivate(cx, mftimeObj));

            mfdata->getArray().resize(mftime.getLength());

            for (size_t i = 0; i < mftime.getLength(); ++i) {
                if (!JS_NewDoubleValue(cx, mftime.getElement(i),
                                       &mfdata->getArray()[i])) {
                    return JS_FALSE;
                }
                if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(mfdata->getArray()[i]))) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mftimeObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mftimeObj);
            return JS_TRUE;
        }

        JSClass MFVec2f::jsclass =
                { "MFVec2f", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSClass & MFVec2f::sfjsclass = SFVec2f::jsclass;

        JSBool MFVec2f::toJsval(const VrmlMFVec2f & mfvec2f, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            JSObject * const mfvec2fObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfvec2fObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = reinterpret_cast<MFData *>
                                    (JS_GetPrivate(cx, mfvec2fObj));

            mfdata->getArray().resize(mfvec2f.getLength());

            for (size_t i = 0; i < mfvec2f.getLength(); ++i) {
                const VrmlSFVec2f sfvec2f(mfvec2f.getElement(i));
                if (!SFVec2f::toJsval(sfvec2f, true, cx, obj,
                                      &mfdata->getArray()[i])) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfvec2fObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfvec2fObj);
            return JS_TRUE;
        }

        JSClass MFVec3f::jsclass =
                { "MFVec3f", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSClass & MFVec3f::sfjsclass = SFVec3f::jsclass;

        JSBool MFVec3f::toJsval(const VrmlMFVec3f & mfvec3f, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            JSObject * const mfvec3fObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfvec3fObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = reinterpret_cast<MFData *>
                                    (JS_GetPrivate(cx, mfvec3fObj));

            mfdata->getArray().resize(mfvec3f.getLength());

            for (size_t i = 0; i < mfvec3f.getLength(); ++i) {
                const VrmlSFVec3f sfvec3f(mfvec3f.getElement(i));
                if (!SFVec3f::toJsval(sfvec3f, true, cx, obj,
                                      &mfdata->getArray()[i])) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfvec3fObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfvec3fObj);
            return JS_TRUE;
        }
    }
# endif // HAVE_JAVASCRIPT
}
