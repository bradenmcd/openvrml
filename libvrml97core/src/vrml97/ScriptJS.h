//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
#ifndef SCRIPTJS_H
#define SCRIPTJS_H

//
//  Javascript Script class
//
//#include "javascript/jsapi.h"

#include "ScriptObject.h"
#include "VrmlField.h"

class VrmlNodeScript;
class VrmlScene;

typedef long jsval;

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

  static struct JSRuntime *rt;
  static int nInstances;

  void defineAPI();
  void defineFields();

  VrmlNodeScript *d_node;

  double d_timeStamp;

  struct JSContext *d_cx;
  struct JSObject *d_globalObj;
  struct JSObject *d_browserObj;

};

#endif // SCRIPTJS_H

