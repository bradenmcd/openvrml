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

#ifndef VRMLNODEWORLDINFO_H
#define VRMLNODEWORLDINFO_H

#include "VrmlNodeChild.h"
#include "VrmlMFString.h"
#include "VrmlSFString.h"


class VrmlNodeWorldInfo : public VrmlNodeChild {

public:

  // Define the built in VrmlNodeType:: "WorldInfo"
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);

  virtual VrmlNodeType & nodeType() const;

  VrmlNodeWorldInfo(VrmlScene *);
  virtual ~VrmlNodeWorldInfo();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual ostream& printFields(ostream& os, int indent);

  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  const char* title() { return d_title.get(); }

  const char * const * info() { return d_info.get(); }

  size_t size() { return d_info.getLength(); }


private:

  VrmlMFString d_info;

  VrmlSFString d_title;

};



#endif
