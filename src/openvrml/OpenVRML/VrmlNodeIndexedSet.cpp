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

#include "VrmlNodeIndexedSet.h"
#include "VrmlNodeType.h"


// Define the built in VrmlNodeType:: "Indexed*Set" fields

VrmlNodeType *VrmlNodeIndexedSet::defineType(VrmlNodeType *t)
{
  VrmlNodeGeometry::defineType(t);	// Parent class

  t->addEventIn("set_colorIndex", VrmlField::MFINT32);
  t->addEventIn("set_coordIndex", VrmlField::MFINT32);
  t->addExposedField("color", VrmlField::SFNODE);
  t->addExposedField("coord", VrmlField::SFNODE);
  t->addField("colorIndex", VrmlField::MFINT32);
  t->addField("colorPerVertex", VrmlField::SFBOOL);
  t->addField("coordIndex", VrmlField::MFINT32);

  return t;
}

VrmlNodeIndexedSet::VrmlNodeIndexedSet(VrmlScene *scene) :
  VrmlNodeGeometry(scene),
  d_colorPerVertex(true)
{
}

VrmlNodeIndexedSet::~VrmlNodeIndexedSet()
{
}


bool VrmlNodeIndexedSet::isModified() const
{
  return ( d_modified ||
	   (d_color.get() && d_color.get()->isModified()) ||
	   (d_coord.get() && d_coord.get()->isModified()) );
}

void VrmlNodeIndexedSet::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_color.get()) d_color.get()->updateModified(path);
  if (d_coord.get()) d_coord.get()->updateModified(path);
  path.pop_front();
}

void VrmlNodeIndexedSet::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_color.get()) d_color.get()->clearFlags();
  if (d_coord.get()) d_coord.get()->clearFlags();
}

void VrmlNodeIndexedSet::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  if (d_color.get()) d_color.get()->addToScene(s, rel);
  if (d_coord.get()) d_coord.get()->addToScene(s, rel);
}

void VrmlNodeIndexedSet::copyRoutes( VrmlNamespace *ns ) const
{
  VrmlNode::copyRoutes(ns);
  if (d_color.get()) d_color.get()->copyRoutes(ns);
  if (d_coord.get()) d_coord.get()->copyRoutes(ns);
}

ostream& VrmlNodeIndexedSet::printFields(ostream& os, int indent)
{
  if (d_color.get()) PRINT_FIELD(color);
  if (d_colorIndex.getLength() > 0) PRINT_FIELD(colorIndex);
  if (! d_colorPerVertex.get()) PRINT_FIELD(colorPerVertex);
  if (d_coord.get()) PRINT_FIELD(coord);
  if (d_coordIndex.getLength() > 0) PRINT_FIELD(coordIndex);
  return os;
}

VrmlNodeColor *VrmlNodeIndexedSet::color()
{
  return d_color.get() ? d_color.get()->toColor() : 0;
}

const VrmlMFInt32 & VrmlNodeIndexedSet::getCoordIndex() const
{   return d_coordIndex; }

const VrmlMFInt32 & VrmlNodeIndexedSet::getColorIndex() const   // LarryD Feb 18/99
{   return d_colorIndex; }

// Get the value of one of the exposedFields

const VrmlField *VrmlNodeIndexedSet::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  else if ( strcmp( fieldName, "coord" ) == 0 )
    return &d_coord;
  
  return VrmlNodeGeometry::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeIndexedSet::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_SFNODE_FIELD(color, Color)
  else if TRY_FIELD(colorIndex, MFInt32)
  else if TRY_FIELD(colorPerVertex, SFBool)
  else if TRY_SFNODE_FIELD(coord, Coordinate)
  else if TRY_FIELD(coordIndex, MFInt32)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
}

/**
 * @brief Get the Color node associated with this Indexed*Set.
 *
 * @return color
 */
const VrmlSFNode & VrmlNodeIndexedSet::getColor() const {
    return this->d_color;
}

/**
 * @brief Set the Color node associated with this Indexed*Set.
 *
 * @param color
 */
void VrmlNodeIndexedSet::setColor(const VrmlSFNode & color) {
    this->d_color = color;
}

/**
 * @brief Get the Coordinate node associated with this Indexed*Set.
 *
 * @return coord
 */
const VrmlSFNode & VrmlNodeIndexedSet::getCoord() const {
    return this->d_coord;
}

/**
 * @brief Set the Coordinate node associated with this Indexed*Set.
 *
 * @param coord
 */
void VrmlNodeIndexedSet::setCoord(const VrmlSFNode & coord) {
    this->d_coord = coord;
}
