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

#include "VrmlNodeILineSet.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"
#include "VrmlNodeColor.h"
#include "VrmlNodeCoordinate.h"
#include "VrmlNodeTextureCoordinate.h"
#include "Viewer.h"

static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeILineSet(s); }


// Define the built in VrmlNodeType:: "IndexedLineSet" fields

VrmlNodeType *VrmlNodeILineSet::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("IndexedLineSet", creator);
      t->reference();
    }

  VrmlNodeIndexedSet::defineType(t);	// Parent class

  return t;
}

VrmlNodeType & VrmlNodeILineSet::nodeType() const
{
    return *defineType(0);
}

VrmlNodeILineSet::VrmlNodeILineSet(VrmlScene *scene) :
  VrmlNodeIndexedSet(scene)
{
}

VrmlNodeILineSet::~VrmlNodeILineSet()
{
}

bool VrmlNodeILineSet::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeILineSet::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_color.get()) {
            this->d_color.get()->resetVisitedFlag();
        }
        if (this->d_coord.get()) {
            this->d_coord.get()->resetVisitedFlag();
        }
    }
}

// TO DO colors

Viewer::Object VrmlNodeILineSet::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;
  if (d_coord.get() && d_coordIndex.getLength() > 0)
    {
      VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();
      int nvert = coord.getLength();
      const float * color = 0;
      int nci = 0; const long * ci = 0;

      // check #colors is consistent with colorPerVtx, colorIndex...
      if (d_color.get())
	{
	  VrmlMFColor &c = d_color.get()->toColor()->color();
	  color = &c[0][0];
	  nci = d_colorIndex.getLength();
	  if (nci) ci = d_colorIndex.get();
	}

      obj =  viewer->insertLineSet(nvert,
				   &coord[0][0],
				   d_coordIndex.getLength(),
				   d_coordIndex.get(),
				   d_colorPerVertex.get(),
				   color,
				   nci, ci);

    }
  
  if (d_color.get()) d_color.get()->clearModified();
  if (d_coord.get()) d_coord.get()->clearModified();

  return obj;
}
