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
#ifndef VRMLSFINT_H
#define VRMLSFINT_H

#include "VrmlField.h"

class VrmlSFInt : public VrmlField {
public:

  VrmlSFInt(int value = 0);

  virtual ostream& print(ostream& os) const;

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlSFInt* toSFInt() const;
  virtual VrmlSFInt* toSFInt();

  int  get(void) const		{ return d_value; }
  void set(int value)		{ d_value = value; }

private:
  int d_value;

};

#endif
