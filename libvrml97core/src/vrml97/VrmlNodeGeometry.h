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

#ifndef  _VRMLNODEGEOMETRY_
#define  _VRMLNODEGEOMETRY_

#include "VrmlNode.h"
#include "Viewer.h"

class VrmlNodeColor;

class VrmlNodeGeometry : public VrmlNode {

public:

  // Define the fields of all built in geometry nodes
  static VrmlNodeType *defineType(VrmlNodeType *t);

  VrmlNodeGeometry(VrmlScene *);
  ~VrmlNodeGeometry();

  virtual VrmlNodeGeometry* toGeometry() const;

  // Specific geometry nodes need only define insertGeometry(), not render().
  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc) = 0;

  virtual VrmlNodeColor *color();

protected:

  Viewer::Object d_viewerObject; // move to VrmlNode? ...

};

#endif // _VRMLNODEGEOMETRY_

