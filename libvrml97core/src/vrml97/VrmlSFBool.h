//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
#ifndef  VRMLSFBOOL_H
#define  VRMLSFBOOL_H

#include "VrmlField.h"


class VrmlSFBool : public VrmlSField {
public:

  VrmlSFBool(bool value = false);

  virtual ostream& print(ostream& os) const;

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlSFBool* toSFBool() const;
  virtual VrmlSFBool* toSFBool();

  bool get(void) const		{ return d_value; }
  void set(bool value)		{ d_value = value; }

private:
  bool d_value;

};

#endif
