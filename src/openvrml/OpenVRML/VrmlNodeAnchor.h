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
//  VrmlNodeAnchor.h

#ifndef VRMLNODEANCHOR_H
#define VRMLNODEANCHOR_H

#include "VrmlNodeGroup.h"
#include "VrmlMFString.h"
#include "VrmlSFString.h"

class VrmlNodeType;
class VrmlScene;
class VrmlRenderContext;

class VrmlNodeAnchor : public VrmlNodeGroup {

public:

  // Define the built in VrmlNodeType:: "Anchor"
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeAnchor(VrmlScene*);
  VrmlNodeAnchor(const VrmlNodeAnchor&);
  virtual ~VrmlNodeAnchor();

  // Copy the node.
  virtual VrmlNode *cloneMe() const;

  virtual VrmlNodeAnchor *toAnchor() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  void activate();

  virtual const VrmlField *getField(const char *fieldName) const;

  virtual void setField(const char *fieldName,
			const VrmlField &fieldValue);

  const char *description() { return d_description.get(); }
  const char *url() { return d_url.getLength() > 0 ? d_url[0] : 0; }

  const VrmlBVolume* getBVolume() const;

protected:

  VrmlSFString d_description;
  VrmlMFString d_parameter;
  VrmlMFString d_url;
  
};

#endif // VRMLNODEANCHOR_H

