//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  VrmlNodeFog.h

#ifndef VRMLNODEFOG_H
#define VRMLNODEFOG_H

#include "VrmlNodeChild.h"
#include "VrmlSFBool.h"
#include "VrmlSFColor.h"
#include "VrmlSFFloat.h"
#include "VrmlSFString.h"

class VrmlScene;

class VrmlNodeFog : public VrmlNodeChild {

public:

  // Define the fields of Fog nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeFog(VrmlScene *);
  virtual ~VrmlNodeFog();

  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeFog* toFog() const;

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual ostream& printFields(ostream& os, int indent);

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField *fieldValue);

  // Get a field or eventOut by name.
  virtual const VrmlField *getField(const char *fieldName) const;
  
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  float *color()		{ return d_color.get(); }
  const char *fogType()		{ return d_fogType.get(); }
  float visibilityRange()	{ return d_visibilityRange.get(); }

private:

  VrmlSFColor d_color;
  VrmlSFString d_fogType;
  VrmlSFFloat d_visibilityRange;

  VrmlSFBool d_isBound;		// eventOut

};

#endif
