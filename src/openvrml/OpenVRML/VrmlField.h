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

#ifndef VRMLFIELD_H
#define VRMLFIELD_H

#include <iostream.h>

class VrmlSFBool;
class VrmlSFColor;
class VrmlSFFloat;
class VrmlSFImage;
class VrmlSFInt32;
class VrmlSFNode;
class VrmlSFRotation;
class VrmlSFString;
class VrmlSFTime;
class VrmlSFVec2f;
class VrmlSFVec3f;

class VrmlMFColor;
class VrmlMFFloat;
class VrmlMFInt32;
class VrmlMFNode;
class VrmlMFRotation;
class VrmlMFString;
class VrmlMFVec2f;
class VrmlMFVec3f;

// Global output operator for fields

class VrmlField;
ostream& operator<<(ostream& os, const VrmlField& f);


// Abstract base class for field values

class VrmlField {
  friend ostream& operator<<(ostream& os, const VrmlField& f);

public:

  // Field type identifiers
  enum VrmlFieldType {
    NO_FIELD,
    SFBOOL,
    SFCOLOR,
    SFFLOAT,
    SFIMAGE,
    SFINT32,
    SFNODE,
    SFROTATION,
    SFSTRING,
    SFTIME,
    SFVEC2F,
    SFVEC3F,
    MFCOLOR,
    MFFLOAT,
    MFINT32,
    MFNODE,
    MFROTATION,
    MFSTRING,
    MFTIME,
    MFVEC2F,
    MFVEC3F
  };

  virtual ~VrmlField() = 0;

  // Copy self
  virtual VrmlField *clone() const = 0;

  // Write self
  virtual ostream& print(ostream& os) const = 0;

  // Field type
  virtual VrmlFieldType fieldType() const = 0;

  // Field type name
  const char *fieldTypeName() const;

  // Name to field type
  static VrmlFieldType fieldType(const char *fieldTypeName);

  // safe downcasts, const and non-const versions.
  // These avoid casts of VrmlField* but are ugly in that this class
  // must know of the existence of all of its subclasses...

  virtual const VrmlSFBool *toSFBool() const;
  virtual const VrmlSFColor *toSFColor() const;
  virtual const VrmlSFFloat *toSFFloat() const;
  virtual const VrmlSFImage *toSFImage() const;
  virtual const VrmlSFInt32 *toSFInt32() const;
  virtual const VrmlSFNode *toSFNode() const;
  virtual const VrmlSFRotation *toSFRotation() const;
  virtual const VrmlSFString *toSFString() const;
  virtual const VrmlSFTime *toSFTime() const;
  virtual const VrmlSFVec2f *toSFVec2f() const;
  virtual const VrmlSFVec3f *toSFVec3f() const;
  
  virtual const VrmlMFColor *toMFColor() const;
  virtual const VrmlMFFloat *toMFFloat() const;
  virtual const VrmlMFInt32 *toMFInt32() const;
  virtual const VrmlMFNode *toMFNode() const;
  virtual const VrmlMFRotation *toMFRotation() const;
  virtual const VrmlMFString *toMFString() const;
  virtual const VrmlMFVec2f *toMFVec2f() const;
  virtual const VrmlMFVec3f *toMFVec3f() const;

  virtual VrmlSFBool *toSFBool();
  virtual VrmlSFColor *toSFColor();
  virtual VrmlSFFloat *toSFFloat();
  virtual VrmlSFImage *toSFImage();
  virtual VrmlSFInt32 *toSFInt32();
  virtual VrmlSFNode *toSFNode();
  virtual VrmlSFRotation *toSFRotation();
  virtual VrmlSFString *toSFString();
  virtual VrmlSFTime *toSFTime();
  virtual VrmlSFVec2f *toSFVec2f();
  virtual VrmlSFVec3f *toSFVec3f();
  
  virtual VrmlMFColor *toMFColor();
  virtual VrmlMFFloat *toMFFloat();
  virtual VrmlMFInt32 *toMFInt32();
  virtual VrmlMFNode *toMFNode();
  virtual VrmlMFRotation *toMFRotation();
  virtual VrmlMFString *toMFString();
  virtual VrmlMFVec2f *toMFVec2f();
  virtual VrmlMFVec3f *toMFVec3f();

};

#endif
