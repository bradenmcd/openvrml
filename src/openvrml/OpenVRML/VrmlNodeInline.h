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

#ifndef  _VRMLNODEINLINE_
#define  _VRMLNODEINLINE_

#include "VrmlNodeGroup.h"
#include "VrmlMFString.h"

class VrmlNamespace;
class VrmlNodeType;
class VrmlScene;

class VrmlNodeInline : public VrmlNodeGroup {

public:

  // Define the built in VrmlNodeType:: "Inline"
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeInline(VrmlScene*);
  virtual ~VrmlNodeInline();

  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeInline* toInline() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void addToScene( VrmlScene *s, const char* relativeUrl );

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName,
			const VrmlField &fieldValue);

  void load(const char *relativeUrl);

protected:

  VrmlMFString d_url;

  VrmlNamespace *d_namespace;

  bool d_hasLoaded;

};

#endif // _VRMLNODEINLINE_

