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
#ifndef VRMLMFINT_H
#define VRMLMFINT_H

#include "VrmlField.h"

//
// It would be nice to somehow incorporate the reference counting
// into a base class (VrmlMField) or make a VrmlMField template...
// There is no support for copy-on-write, so if you modify an element
// of the data vector, all objects that share that data will see the
// change.
//

class VrmlMFInt : public VrmlField {
private:

  class IData {			// reference counted int data
  public:
    IData(int n=0) : d_refs(1), d_n(n), d_v(n > 0 ? new int[n] : 0) {}
    ~IData() { delete [] d_v; }

    IData *ref() { ++d_refs; return this; }
    void deref() { if (--d_refs == 0) delete this; }

    int d_refs;			// number of MFInt objects using this data
    int d_n;			// size (in ints) of d_v
    int *d_v;			// data vector
  };

  IData *d_data;

public:

  VrmlMFInt();
  VrmlMFInt(int value);
  VrmlMFInt(int n, int const *v);
  VrmlMFInt(const VrmlMFInt &src);

  ~VrmlMFInt();

  virtual ostream& print(ostream& os) const;

  void set(int n, int *v);
  VrmlMFInt& operator=(const VrmlMFInt& rhs);

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlMFInt* toMFInt() const;
  virtual VrmlMFInt* toMFInt();

  int size() const			{ return d_data->d_n; }
  int *get() const			{ return d_data->d_v; }
  int &operator[](int i) const		{ return d_data->d_v[i]; }

};

#endif // VRMLMFINT_H
