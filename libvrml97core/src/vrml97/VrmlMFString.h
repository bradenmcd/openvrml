//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
# ifndef VRMLMFSTRING_H
#   define VRMLMFSTRING_H

#   include <stddef.h>
#   include "VrmlField.h"


class VrmlMFString : public VrmlMField {
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


  size_t size() const;
  char const * const * get() const;
  char const * get(size_t) const;
  char const * operator[](size_t) const;

private:

  char ** d_v;
  size_t d_allocated;
  size_t d_size;

};

# endif
