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
# ifndef VRMLMFSTRING_H
#   define VRMLMFSTRING_H

#   include <stddef.h>
#   include "VrmlField.h"


class VrmlMFString : public VrmlField {
public:

  VrmlMFString();
  VrmlMFString(char const * s);
  VrmlMFString(size_t n, char const * const * values = 0);
  VrmlMFString(const VrmlMFString&);

  ~VrmlMFString();

  // Assignment. Just reallocate for now...
  void set(size_t n, char const * const v[]);
  VrmlMFString& operator=(const VrmlMFString& rhs);

  virtual VrmlField *clone() const;

  virtual VrmlFieldType fieldType() const;
  virtual const VrmlMFString* toMFString() const;
  virtual VrmlMFString* toMFString();

  virtual ostream& print(ostream& os) const;


  size_t getLength() const;
  char const * const * get() const;
  char const * get(size_t) const;
  char const * operator[](size_t) const;

private:

  char ** d_v;
  size_t d_allocated;
  size_t d_size;

};

# endif
