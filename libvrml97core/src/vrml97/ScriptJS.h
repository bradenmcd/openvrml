//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
#ifndef SCRIPTJS_H
#define SCRIPTJS_H

//
//  Javascript Script class
//
#include "libjs/jsapi.h"

#include "ScriptObject.h"
#include "VrmlField.h"

class VrmlNodeScript;
class VrmlScene;

class ScriptJS: public ScriptObject {

public:

  ScriptJS( VrmlNodeScript *, const char *source );
  ~ScriptJS();

  virtual void activate( double timeStamp,
			 const char *fname,
			 int argc,
			 const VrmlField *argv[]);

  VrmlScene *browser();
  VrmlNodeScript *scriptNode() { return d_node; }

  jsval vrmlFieldToJSVal( VrmlField::VrmlFieldType,
			  const VrmlField *f, bool protect );


protected:

  static JSRuntime *rt;
  static int nInstances;

  void defineAPI();
  void defineFields();

  VrmlNodeScript *d_node;

  double d_timeStamp;

  JSContext *d_cx;
  JSObject *d_globalObj;
  JSObject *d_browserObj;

};

#endif // SCRIPTJS_H

