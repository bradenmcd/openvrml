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
#ifndef SCRIPTJS_H
#define SCRIPTJS_H

#include "ScriptObject.h"
#include "field.h"

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

