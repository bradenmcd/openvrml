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

class VrmlMFInt32 : public VrmlField {
private:

  class IData {			// reference counted int data
  public:
    IData(long n=0) : d_refs(1), d_n(n), d_v(n > 0 ? new long[n] : 0) {}
    ~IData() { delete [] d_v; }

    IData *ref() { ++d_refs; return this; }
    void deref() { if (--d_refs == 0) delete this; }

    long d_refs;    // number of MFInt objects using this data
    size_t d_n;     // size (in ints) of d_v
    long * d_v;     // data vector
  };

  IData *d_data;

public:

  VrmlMFInt32();
  VrmlMFInt32(long);
  VrmlMFInt32(size_t n, const long * v);
  VrmlMFInt32(const VrmlMFInt32 & src);

  ~VrmlMFInt32();

  virtual ostream& print(ostream& os) const;

  void set(size_t, const long *);
  VrmlMFInt32 & operator=(const VrmlMFInt32 & rhs);

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlMFInt32 * toMFInt32() const;
  virtual VrmlMFInt32 * toMFInt32();

  size_t size() const;
  const long * get() const;
  const long & operator[](size_t) const;

};

#endif // VRMLMFINT_H
