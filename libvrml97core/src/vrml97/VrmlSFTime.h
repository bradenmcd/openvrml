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
#ifndef VRMLSFTIME
#define VRMLSFTIME

#include "VrmlField.h"


class VrmlSFTime : public VrmlSField {
public:

  VrmlSFTime(double value = 0.0);
  VrmlSFTime(const VrmlSFTime& );

  virtual ostream& print(ostream& os) const;

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlSFTime* toSFTime() const;
  virtual VrmlSFTime* toSFTime();

  // Assignment.
  VrmlSFTime& operator=(const VrmlSFTime& rhs);
  VrmlSFTime& operator=(double rhs);

  double get(void) const	{ return d_value; }
  void set(double value)	{ d_value = value; }

private:
  double d_value;

};

#endif
