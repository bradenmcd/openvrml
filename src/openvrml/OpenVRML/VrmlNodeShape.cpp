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

#include "VrmlNodeShape.h"
#include "VrmlNodeAppearance.h"
#include "VrmlNodeGeometry.h"
#include "VrmlNodeTexture.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"
#include "Viewer.h"
#include "VrmlBVolume.h"


static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeShape(s); }


// Define the built in VrmlNodeType:: "Shape" fields

VrmlNodeType *VrmlNodeShape::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Shape", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("appearance", VrmlField::SFNODE);
  t->addExposedField("geometry", VrmlField::SFNODE);

  return t;
}


VrmlNodeType & VrmlNodeShape::nodeType() const
{
    return *defineType(0);
}


VrmlNodeShape::VrmlNodeShape(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_viewerObject(0)
{
}

VrmlNodeShape::~VrmlNodeShape()
{
  // need viewer to free d_viewerObject ...
}

bool VrmlNodeShape::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeShape::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_appearance.get()) {
            this->d_appearance.get()->resetVisitedFlag();
        }
        if (this->d_geometry.get()) {
            this->d_geometry.get()->resetVisitedFlag();
        }
    }
}

bool VrmlNodeShape::isModified() const
{
  return ( d_modified ||
	   (d_geometry.get() && d_geometry.get()->isModified()) ||
	   (d_appearance.get() && d_appearance.get()->isModified()) );
}

void VrmlNodeShape::updateModified(VrmlNodePath& path, int flags)
{
  //cout << "VrmlNodeShape[" << this << "]::updateModified()" << endl;
  if (this->isModified()) markPathModified(path, true, flags);
  path.push_front(this);
  if (d_appearance.get()) d_appearance.get()->updateModified(path, flags);
  if (d_geometry.get()) d_geometry.get()->updateModified(path, flags);
  path.pop_front();
}

void VrmlNodeShape::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_appearance.get()) d_appearance.get()->clearFlags();
  if (d_geometry.get()) d_geometry.get()->clearFlags();
}

void VrmlNodeShape::addToScene(VrmlScene *s, const char *relUrl)
{
  d_scene = s;
  if (d_appearance.get()) d_appearance.get()->addToScene(s,relUrl);
  if (d_geometry.get()) d_geometry.get()->addToScene(s,relUrl);
}

ostream& VrmlNodeShape::printFields(ostream& os, int indent)
{
  if (d_appearance.get()) PRINT_FIELD(appearance);
  if (d_geometry.get()) PRINT_FIELD(geometry);

  return os;
}

VrmlNodeShape*	VrmlNodeShape::toShape() const
{ return (VrmlNodeShape*) this; }


void VrmlNodeShape::render(Viewer *viewer, VrmlRenderContext rc)
{
  //cout << "VrmlNodeShape::render()" << endl;

  if ( d_viewerObject && isModified() )
    {
      viewer->removeObject(d_viewerObject);
      d_viewerObject = 0;
    }

  VrmlNodeGeometry *g = d_geometry.get() ? d_geometry.get()->toGeometry() : 0;

  if (d_viewerObject)
    viewer->insertReference(d_viewerObject);

  else if ( g )
    {
      d_viewerObject = viewer->beginObject( name() );

      // Don't care what color it is if we are picking
      bool picking = (Viewer::RENDER_MODE_PICK == viewer->getRenderMode());
      if (! picking )
	{
	  int nTexComponents = 0;

	  if (! picking && d_appearance.get() &&
	      d_appearance.get()->toAppearance() )
	    {
	      VrmlNodeAppearance *a = d_appearance.get()->toAppearance();
	      a->render(viewer, rc);

	      if (a->getTexture().get() && a->getTexture().get()->toTexture())
		nTexComponents = a->getTexture().get()->toTexture()->nComponents();
	    }
	  else
	    {
	      viewer->setColor(1.0, 1.0, 1.0); // default object color
	      viewer->enableLighting(false);  // turn lighting off
	    }

	  // hack for opengl material mode
	  viewer->setMaterialMode( nTexComponents, g->color() != 0 );
	}

      g->render(viewer, rc);

      viewer->endObject();
    }

  else if (d_appearance.get())
    d_appearance.get()->clearModified();

  clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeShape::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "appearance" ) == 0 )
    return &d_appearance;
  else if ( strcmp( fieldName, "geometry" ) == 0 )
    return &d_geometry;
  
  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeShape::setField(const char *fieldName,
			     const VrmlField &fieldValue)
{
  if TRY_SFNODE_FIELD(appearance, Appearance)
  else if TRY_SFNODE_FIELD(geometry, Geometry)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true); // case out: just if geom set!
}


// just pass off to the geometry's getbvolume() method
//
const VrmlBVolume* VrmlNodeShape::getBVolume() const
{
  //cout << "VrmlNodeShape::getBVolume() {" << endl;
  const VrmlBVolume* r = (VrmlBVolume*)0;
  VrmlNode* geom = d_geometry.get();
  if (geom) {
    r = geom->getBVolume();
  }
  //cout << "}:" << r << ":";
  //if (r) r->dump(cout);
  //cout << endl;
  ((VrmlNodeShape*)this)->setBVolumeDirty(false);
  return r;
}

/**
 * @brief Get the Appearance node associated with this Shape.
 *
 * @return appearance
 */
const VrmlSFNode & VrmlNodeShape::getAppearance() const {
    return this->d_appearance;
}

/**
 * @brief Set the Appearance node associated with this Shape.
 *
 * @param appearance
 */
void VrmlNodeShape::setAppearance(const VrmlSFNode & appearance) {
    assert(!appearance.get()
            || dynamic_cast<VrmlNodeAppearance *>(appearance.get()));
    this->d_appearance = appearance;
}

/**
 * @brief Get the geometry node associated with this Shape.
 *
 * @return geometry
 */
const VrmlSFNode & VrmlNodeShape::getGeometry() const {
    return this->d_geometry;
}

/**
 * @brief Set the geometry node associated with this Shape.
 *
 * @param geometry
 */
void VrmlNodeShape::setGeometry(const VrmlSFNode & geometry) {
    assert(!geometry.get()
            || dynamic_cast<VrmlNodeGeometry *>(geometry.get()));
    this->d_geometry = geometry;
}
