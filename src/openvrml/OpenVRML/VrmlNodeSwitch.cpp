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

#include "VrmlNodeSwitch.h"
#include "VrmlNodeType.h"
#include "Viewer.h"
#include "VrmlBSphere.h"


// Return a new VrmlNodeSwitch
static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeSwitch(s); }


// Define the built in VrmlNodeType:: "Switch" fields

VrmlNodeType *VrmlNodeSwitch::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Switch", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("choice", VrmlField::MFNODE);
  t->addExposedField("whichChoice", VrmlField::SFINT32);

  return t;
}

VrmlNodeType & VrmlNodeSwitch::nodeType() const
{
    return *defineType(0);
}


VrmlNodeSwitch::VrmlNodeSwitch(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_whichChoice(-1)
{
  this->setBVolumeDirty(true);
}

VrmlNodeSwitch::~VrmlNodeSwitch()
{
}


VrmlNode *VrmlNodeSwitch::cloneMe() const
{
  return new VrmlNodeSwitch(*this);
}

void VrmlNodeSwitch::cloneChildren(VrmlNamespace * ns) {
    for (size_t i = 0; i < this->d_choice.getLength(); ++i) {
        if (! this->d_choice[i]) {
            continue;
        }
        VrmlNode * const newKid = this->d_choice[i]->clone(ns)->reference();
        this->d_choice[i]->dereference();
        this->d_choice[i] = newKid;
    }
}


bool VrmlNodeSwitch::isModified() const
{
  if (d_modified) return true;

  int w = d_whichChoice.get();

  return (w >= 0 && w < (int) d_choice.getLength() && d_choice[w]->isModified());
}


// ok: again we get this issue of whether to check _all_ the children
// or just the current choice (ref LOD). again, chooise to test them
// all. note that the original isModified() just tested the current
// one. keep that in mind, and change it back when confirmed safe.
//
void VrmlNodeSwitch::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  int n = d_choice.getLength();
  for (int i=0; i<n; ++i)
    d_choice[i]->updateModified(path);
  path.pop_front();
}


void VrmlNodeSwitch::clearFlags()
{
  VrmlNode::clearFlags();
  
  int n = d_choice.getLength();
  for (int i = 0; i<n; ++i)
    d_choice[i]->clearFlags();
}

void VrmlNodeSwitch::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  
  int n = d_choice.getLength();

  for (int i = 0; i<n; ++i)
    d_choice[i]->addToScene(s, rel);
}

void VrmlNodeSwitch::copyRoutes( VrmlNamespace *ns ) const
{
  VrmlNode::copyRoutes(ns);
  
  int n = d_choice.getLength();
  for (int i = 0; i<n; ++i)
    d_choice[i]->copyRoutes(ns);
}


ostream& VrmlNodeSwitch::printFields(ostream& os, int indent)
{
  if (d_choice.getLength() > 0) PRINT_FIELD(choice);
  if (d_whichChoice.get() != -1) PRINT_FIELD(whichChoice);
  return os;
}


// Render the selected child

void VrmlNodeSwitch::render(Viewer *viewer, VrmlRenderContext rc)
{
  int w = d_whichChoice.get();

  if (w >= 0 && w < (int) d_choice.getLength())
    d_choice[w]->render(viewer, rc);

  clearModified();
}


// Get the value of one of the node fields.
const VrmlField *VrmlNodeSwitch::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "choice" ) == 0 )
    return &d_choice;
  else if ( strcmp( fieldName, "whichChoice" ) == 0 )
    return &d_whichChoice;

  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.
void VrmlNodeSwitch::setField(const char *fieldName,
			      const VrmlField &fieldValue)
{
  if TRY_FIELD(choice, MFNode)
  else if TRY_FIELD(whichChoice, SFInt32)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true);
}

VrmlNodeSwitch* VrmlNodeSwitch::toSwitch() const //LarryD
{ return (VrmlNodeSwitch*) this; }


const VrmlBVolume* VrmlNodeSwitch::getBVolume() const
{
  //cout << "VrmlNodeGroup[" << this << "]::getBVolume()" << endl;
  if (this->isBVolumeDirty())
    ((VrmlNodeSwitch*)this)->recalcBSphere();
  return &d_bsphere;
}

void
VrmlNodeSwitch::recalcBSphere()
{
  //cout << "VrmlNodeSwitch[" << this << "]::recalcBSphere()" << endl;
  d_bsphere.reset();
  int w = d_whichChoice.get();
  if (w >= 0 && w < (int) d_choice.getLength()) {
    const VrmlBVolume* ci_bv = d_choice[w]->getBVolume();
    if (ci_bv)
      d_bsphere.extend(*ci_bv);
  }
  this->setBVolumeDirty(false);
}

/**
 * @brief Get the nodes that may be chosen.
 *
 * @return choice
 */
const VrmlMFNode & VrmlNodeSwitch::getChoice() const {
    return this->d_choice;
}

/**
 * @brief Set the nodes that may be chosen.
 *
 * @param choice
 */
void VrmlNodeSwitch::setChoice(const VrmlMFNode & choice) {
    this->d_choice = choice;
    this->setBVolumeDirty(true);
}

/**
 * @brief Get the index of the chosen node.
 *
 * @return whichChoice
 */
const VrmlSFInt32 & VrmlNodeSwitch::getWhichChoice() const {
    return this->d_whichChoice;
}

/**
 * @brief Set the index of the chosen node.
 *
 * @param whichChoice
 */
void VrmlNodeSwitch::setWhichChoice(const VrmlSFInt32 & whichChoice) {
    this->d_whichChoice = whichChoice;
    this->setBVolumeDirty(true);
}
