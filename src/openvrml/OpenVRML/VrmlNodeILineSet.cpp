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


VrmlNode *VrmlNodeILineSet::cloneMe() const
{
  return new VrmlNodeILineSet(*this);
}

void VrmlNodeILineSet::cloneChildren(VrmlNamespace* ns)
{
  if (d_color.get())
    d_color.set(d_color.get()->clone(ns));
  if (d_coord.get())
    d_coord.set(d_coord.get()->clone(ns));
}


// TO DO colors

Viewer::Object VrmlNodeILineSet::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;
  if (d_coord.get() && d_coordIndex.size() > 0)
    {
      VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();
      int nvert = coord.size();
      float *color = 0;
      int nci = 0; const long * ci = 0;

      // check #colors is consistent with colorPerVtx, colorIndex...
      if (d_color.get())
	{
	  VrmlMFColor &c = d_color.get()->toColor()->color();
	  color = &c[0][0];
	  nci = d_colorIndex.size();
	  if (nci) ci = d_colorIndex.get();
	}

      obj =  viewer->insertLineSet(nvert,
				   &coord[0][0],
				   d_coordIndex.size(),
				   &d_coordIndex[0],
				   d_colorPerVertex.get(),
				   color,
				   nci, ci);

    }
  
  if (d_color.get()) d_color.get()->clearModified();
  if (d_coord.get()) d_coord.get()->clearModified();

  return obj;
}
