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

#include "VrmlNodeLOD.h"
#include "VrmlNodeType.h"
#include "MathUtils.h"
#include "Viewer.h"

// Return a new VrmlNodeLOD
static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeLOD(s); }


// Define the built in VrmlNodeType:: "LOD" fields

VrmlNodeType *VrmlNodeLOD::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("LOD", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("level", VrmlField::MFNODE);
  t->addField("center", VrmlField::SFVEC3F);
  t->addField("range", VrmlField::MFFLOAT);

  return t;
}


VrmlNodeType & VrmlNodeLOD::nodeType() const
{
    return *defineType(0);
}


VrmlNodeLOD::VrmlNodeLOD(VrmlScene *scene) :
  VrmlNodeChild(scene)
{
  setBVolumeDirty(true); // lazy calc of bvolume
}

VrmlNodeLOD::~VrmlNodeLOD()
{
}


VrmlNode *VrmlNodeLOD::cloneMe() const
{
  return new VrmlNodeLOD(*this);
}

void VrmlNodeLOD::cloneChildren(VrmlNamespace *ns)
{
  int n = d_level.size();
  VrmlNode **kids = d_level.get();
  for (int i = 0; i<n; ++i)
    {
      if (! kids[i]) continue;
      VrmlNode *newKid = kids[i]->clone(ns)->reference();
      kids[i]->dereference();
      kids[i] = newKid;
    }
}

bool VrmlNodeLOD::isModified() const
{
  if (d_modified) return true;
  
  int n = d_level.size();

  // This should really check which range is being rendered...
  for (int i = 0; i<n; ++i)
    if (d_level[i]->isModified())
      return true;

  return false;
}

// what happens if one of the other children suddenly becomes the one
// selected? to be safe: check them all. this potentially means some
// extra work, but it's a lot easier to reason about.
//
void VrmlNodeLOD::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
 int n = d_level.size();
  for (int i = 0; i<n; ++i)
    d_level[i]->updateModified(path);
  path.pop_front();
}

void VrmlNodeLOD::clearFlags()
{
  VrmlNode::clearFlags();
  int n = d_level.size();
  for (int i = 0; i<n; ++i)
    d_level[i]->clearFlags();
}

void VrmlNodeLOD::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  
  int n = d_level.size();

  for (int i = 0; i<n; ++i)
    d_level[i]->addToScene(s, rel);
}

void VrmlNodeLOD::copyRoutes( VrmlNamespace *ns ) const
{
  VrmlNode::copyRoutes(ns);
  
  int n = d_level.size();
  for (int i = 0; i<n; ++i)
    d_level[i]->copyRoutes(ns);
}


ostream& VrmlNodeLOD::printFields(ostream& os, int indent)
{
  if (d_level.size() > 0) PRINT_FIELD(level);
  if (! FPZERO(d_center.x()) ||
      ! FPZERO(d_center.y()) ||
      ! FPZERO(d_center.z()) )
    PRINT_FIELD(center);
      
  if (d_range.size() > 0) PRINT_FIELD(range);

  return os;
}


// Render one of the children

void VrmlNodeLOD::render(Viewer *viewer, VrmlRenderContext rc)
{
  clearModified();
  if (d_level.size() <= 0) return;

  float x, y, z;
  viewer->getPosition( &x, &y, &z );

  float dx = x - d_center.x();
  float dy = y - d_center.y();
  float dz = z - d_center.z();
  float d2 = dx*dx + dy*dy + dz*dz;

  int i, n = d_range.size();
  for (i=0; i<n; ++i)
    if (d2 < d_range[i] * d_range[i])
      break;

  // Should choose an "optimal" level...
  if (d_range.size() == 0) i = d_level.size() - 1;

  // Not enough levels...
  if (i >= (int) d_level.size()) i = d_level.size() - 1;

  //printf("LOD d2 %g level %d\n", d2, i);

  d_level[i]->render(viewer, rc);

  // Don't re-render on their accounts
  n = d_level.size();
  for (i = 0; i<n; ++i)
    d_level[i]->clearModified();
}

// Get the value of one of the node fields.
const VrmlField *VrmlNodeLOD::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "level" ) == 0 )
    return &d_level;

  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.
void VrmlNodeLOD::setField(const char *fieldName,
			   const VrmlField &fieldValue)
{
  if TRY_FIELD(level, MFNode)
  else if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(range, MFFloat)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
  setBVolumeDirty(true); // lazy calc of bvolume
}

VrmlNodeLOD* VrmlNodeLOD::toLOD() const 
{ return (VrmlNodeLOD*) this; }

const VrmlMFFloat& VrmlNodeLOD::getRange() const  
{   return d_range; }

const VrmlSFVec3f& VrmlNodeLOD::getCenter() const   
{  return d_center; }


const VrmlBVolume* VrmlNodeLOD::getBVolume() const
{
  //cout << "VrmlNodeLOD[" << this << "]::getBVolume()" << endl;
  if (this->isBVolumeDirty())
    ((VrmlNodeLOD*)this)->recalcBSphere();
  return &d_bsphere;
}


void
VrmlNodeLOD::recalcBSphere()
{
  //cout << "VrmlNodeLOD[" << this << "]::recalcBSphere()" << endl;
  d_bsphere.reset();
  
  // let's say our bsphere is the union of the bspheres of all the
  // levels. we could have said it was just the bsphere of the current
  // level, but the current level depends on the viewer position, and
  // we'd like to make the calculation idependent of that. we could do
  // some sort of trick where we reset the bsphere during render, but
  // that seems like overkill unless this simpler method proves to be
  // a bottleneck.
  // 
  // hmm: just thought of a problem: one of the uses of the lod is to
  // switch in delayed-load inlines. this would necessarily switch
  // them in all at once. live with it for now.
  //
  for(int i=0; i<(int) d_level.size(); i++) {
    const VrmlBVolume* ci_bv = d_level[i]->getBVolume();
    d_bsphere.extend(*ci_bv);

  }
  this->setBVolumeDirty(false);
}
