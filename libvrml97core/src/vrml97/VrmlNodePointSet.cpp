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

#include "VrmlNodePointSet.h"
#include "VrmlNodeCoordinate.h"
#include "VrmlNodeColor.h"
#include "VrmlNodeType.h"
#include "Viewer.h"


static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodePointSet(s); }


// Define the built in VrmlNodeType:: "PointSet" fields

VrmlNodeType *VrmlNodePointSet::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("PointSet", creator);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class
  t->addExposedField("color", VrmlField::SFNODE);
  t->addExposedField("coord", VrmlField::SFNODE);

  return t;
}

VrmlNodeType & VrmlNodePointSet::nodeType() const
{
    return *defineType(0);
}


VrmlNodePointSet::VrmlNodePointSet(VrmlScene *scene) :
  VrmlNodeGeometry(scene)
{
  this->setBVolumeDirty(true);
}

VrmlNodePointSet::~VrmlNodePointSet()
{
}


VrmlNode *VrmlNodePointSet::cloneMe() const
{
  return new VrmlNodePointSet(*this);
}

void VrmlNodePointSet::cloneChildren(VrmlNamespace *ns)
{
  if (d_color.get())
    d_color.set(d_color.get()->clone(ns));
  if (d_coord.get())
    d_coord.set(d_coord.get()->clone(ns));
}


bool VrmlNodePointSet::isModified() const
{
  return ( d_modified ||
	   (d_color.get() && d_color.get()->isModified()) ||
	   (d_coord.get() && d_coord.get()->isModified()) );
}

void VrmlNodePointSet::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_color.get()) d_color.get()->updateModified(path);
  if (d_coord.get()) d_coord.get()->updateModified(path);
  path.pop_front();
}



void VrmlNodePointSet::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_color.get()) d_color.get()->clearFlags();
  if (d_coord.get()) d_coord.get()->clearFlags();
}

void VrmlNodePointSet::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  if (d_color.get()) d_color.get()->addToScene(s, rel);
  if (d_coord.get()) d_coord.get()->addToScene(s, rel);
}

void VrmlNodePointSet::copyRoutes( VrmlNamespace *ns ) const
{
  VrmlNode::copyRoutes(ns);
  if (d_color.get()) d_color.get()->copyRoutes(ns);
  if (d_coord.get()) d_coord.get()->copyRoutes(ns);
}

ostream& VrmlNodePointSet::printFields(ostream& os, int indent)
{
  if (d_color.get()) PRINT_FIELD(color);
  if (d_coord.get()) PRINT_FIELD(coord);

  return os;
}


Viewer::Object VrmlNodePointSet::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;

  if (rc.getDrawBSpheres()) {
    const VrmlBSphere* bs = (const VrmlBSphere*)this->getBVolume();
    viewer->drawBSphere(*bs, 4);
  }

  if ( d_coord.get() )
    {
      float *color = 0;
      if ( d_color.get() )
	{
	  VrmlMFColor &c = d_color.get()->toColor()->color();
	  color = &c[0][0];
	}

      VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();

      obj = viewer->insertPointSet(coord.size(), &coord[0][0], color);
    }

  if (d_color.get()) d_color.get()->clearModified();
  if (d_coord.get()) d_coord.get()->clearModified();

  return obj;
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodePointSet::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  else if ( strcmp( fieldName, "coord" ) == 0 )
    return &d_coord;

  return VrmlNodeGeometry::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodePointSet::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_SFNODE_FIELD(color, Color)
  else if TRY_SFNODE_FIELD(coord, Coordinate)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true);
}

void VrmlNodePointSet::recalcBSphere()
{
  //cout << "VrmlNodePointSet::recalcBSphere()" << endl;
  d_bsphere.reset();
  VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();
  int nvert = coord.size();
  for(int i=0; i<nvert; i++) {
    float* vi = coord[i]; // vi[3]
    //cout << vi[0] << "," << vi[1] << "," << vi[2] << ")" << endl;
    d_bsphere.extend(vi);
    //d_bsphere.dump(cout);
  }
  //d_bsphere.dump(cout);
  this->setBVolumeDirty(false);
}


const VrmlBVolume* VrmlNodePointSet::getBVolume() const
{
  if (this->isBVolumeDirty())
    ((VrmlNodePointSet*)this)->recalcBSphere();
  return &d_bsphere;
}
