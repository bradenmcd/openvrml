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

#include "VrmlNodeGeometry.h"
#include "VrmlNodeType.h"

// Define the fields of all built in geometry nodes
VrmlNodeType *VrmlNodeGeometry::defineType(VrmlNodeType *t)
{
  return VrmlNode::defineType(t);
}

VrmlNodeGeometry::VrmlNodeGeometry(VrmlScene *s) :
  VrmlNode(s),
  d_viewerObject(0) 
{
}

VrmlNodeGeometry::~VrmlNodeGeometry()
{
  /* Need access to viewer to delete viewerObject...*/
}


VrmlNodeGeometry* VrmlNodeGeometry::toGeometry() const
{ return (VrmlNodeGeometry*) this; }


VrmlNodeColor *VrmlNodeGeometry::color() { return 0; }



// Geometry nodes need only define insertGeometry(), not render().

void VrmlNodeGeometry::render(Viewer *v, VrmlRenderContext rc) 
{
  //cout << "VrmlModeGeometry::render()" << endl;
  if ( d_viewerObject && isModified() )
    {
      v->removeObject(d_viewerObject);
      d_viewerObject = 0;
    }

  if (d_viewerObject)
    v->insertReference(d_viewerObject);
  else
    {
      d_viewerObject = insertGeometry(v, rc);
      clearModified();
    }
}


