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
#ifndef VRMLMFCOLOR_H
#define VRMLMFCOLOR_H

#include "VrmlField.h"

//
// It would be nice to somehow incorporate the reference counting
// into a base class (VrmlMField) or make a VrmlMField template...
// There is no support for copy-on-write, so if you modify an element
// of the data vector, all objects that share that data will see the
// change.
//

class VrmlMFColor : public VrmlField {
private:

  class FData {			// reference counted float data
  public:
    FData(int n=0) : d_refs(1), d_n(n), d_v(n > 0 ? new float[n] : 0) {}
    ~FData() { delete [] d_v; }

    FData *ref() { ++d_refs; return this; }
    void deref() { if (--d_refs == 0) delete this; }

    int d_refs;			// number of objects using this data
    int d_n;			// size (in floats) of d_v
    float *d_v;			// data vector
  };

  FData *d_data;		// Color data in RGB triples

public:

  VrmlMFColor();
  VrmlMFColor(float r, float g, float b);
  VrmlMFColor(int n, float const *values);
  VrmlMFColor(const VrmlMFColor &source);

  ~VrmlMFColor();

  virtual ostream& print(ostream& os) const;

  // Assignment.
  void set(size_t n, const float * v);
  VrmlMFColor& operator=(const VrmlMFColor& rhs);

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlMFColor* toMFColor() const;
  virtual VrmlMFColor* toMFColor();

  size_t getLength() const		{ return d_data->d_n/3; } // Number of colors
  const float * get() const		{ return d_data->d_v; }
  const float * operator[](size_t i) const	{ return &d_data->d_v[3*i]; }

};

#endif // VRMLMFCOLOR_H
