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

#include "VrmlNodeElevationGrid.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"
#include "VrmlNodeColor.h"
#include "VrmlNodeNormal.h"
#include "VrmlNodeTextureCoordinate.h"
#include "Viewer.h"


static VrmlNode *creator( VrmlScene *s ) {
  return new VrmlNodeElevationGrid(s);
}


// Define the built in VrmlNodeType:: "ElevationGrid" fields

VrmlNodeType *VrmlNodeElevationGrid::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("ElevationGrid", creator);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class

  t->addEventIn("set_height", VrmlField::MFFLOAT);
  t->addExposedField("color", VrmlField::SFNODE);
  t->addExposedField("normal", VrmlField::SFNODE);
  t->addExposedField("texCoord", VrmlField::SFNODE);
  t->addField("ccw", VrmlField::SFBOOL);
  t->addField("colorPerVertex", VrmlField::SFBOOL);
  t->addField("creaseAngle", VrmlField::SFFLOAT);
  t->addField("height", VrmlField::MFFLOAT);
  t->addField("normalPerVertex", VrmlField::SFBOOL);
  t->addField("solid", VrmlField::SFBOOL);
  t->addField("xDimension", VrmlField::SFINT32);
  t->addField("xSpacing", VrmlField::SFFLOAT);
  t->addField("zDimension", VrmlField::SFINT32);
  t->addField("zSpacing", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodeElevationGrid::nodeType() const
{
    return *defineType(0);
}


VrmlNodeElevationGrid::VrmlNodeElevationGrid(VrmlScene *scene) :
  VrmlNodeGeometry(scene),
  d_ccw(true),
  d_colorPerVertex(true),
  d_normalPerVertex(true),
  d_solid(true),
  d_xDimension(0),
  d_xSpacing(1.0f),
  d_zDimension(0),
  d_zSpacing(1.0f)
{
}

VrmlNodeElevationGrid::~VrmlNodeElevationGrid()
{
}

bool VrmlNodeElevationGrid::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeElevationGrid::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_color.get()) {
            this->d_color.get()->resetVisitedFlag();
        }
        if (this->d_normal.get()) {
            this->d_normal.get()->resetVisitedFlag();
        }
        if (this->d_texCoord.get()) {
            this->d_texCoord.get()->resetVisitedFlag();
        }
    }
}

bool VrmlNodeElevationGrid::isModified() const
{
  return ( d_modified ||
	   (d_color.get() && d_color.get()->isModified()) ||
	   (d_normal.get() && d_normal.get()->isModified()) ||
	   (d_texCoord.get() && d_texCoord.get()->isModified()) );
}

void VrmlNodeElevationGrid::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_color.get()) d_color.get()->updateModified(path);
  if (d_normal.get()) d_normal.get()->updateModified(path);
  if (d_texCoord.get()) d_texCoord.get()->updateModified(path);
  path.pop_front();
}


void VrmlNodeElevationGrid::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_color.get()) d_color.get()->clearFlags();
  if (d_normal.get()) d_normal.get()->clearFlags();
  if (d_texCoord.get()) d_texCoord.get()->clearFlags();
}

void VrmlNodeElevationGrid::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  if (d_color.get()) d_color.get()->addToScene(s, rel);
  if (d_normal.get()) d_normal.get()->addToScene(s, rel);
  if (d_texCoord.get()) d_texCoord.get()->addToScene(s, rel);
}

ostream& VrmlNodeElevationGrid::printFields(ostream& os, int indent)
{
  if (d_color.get()) PRINT_FIELD(color);
  if (d_normal.get()) PRINT_FIELD(normal);
  if (d_texCoord.get()) PRINT_FIELD(texCoord);

  if (! d_ccw.get()) PRINT_FIELD(ccw);
  if (! d_colorPerVertex.get()) PRINT_FIELD(colorPerVertex);
  if (! d_normalPerVertex.get()) PRINT_FIELD(normalPerVertex);
  if (! d_solid.get()) PRINT_FIELD(solid);
  
  if (d_creaseAngle.get() != 0.0) PRINT_FIELD(creaseAngle);
  if (d_height.getLength() > 0) PRINT_FIELD(height);

  if (d_xDimension.get() != 0) PRINT_FIELD(xDimension);
  if (d_xSpacing.get() != 0) PRINT_FIELD(xSpacing);
  if (d_zDimension.get() != 0) PRINT_FIELD(zDimension);
  if (d_zSpacing.get() != 0) PRINT_FIELD(zSpacing);

  return os;
}

Viewer::Object VrmlNodeElevationGrid::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;

  if ( d_height.getLength() > 0 )
    {
      const float *tc = 0, *normals = 0, *colors = 0;

      if (d_texCoord.get())
	{
	  VrmlMFVec2f &texcoord =
	    d_texCoord.get()->toTextureCoordinate()->coordinate();
	  tc = &texcoord[0][0];
	}

      if (d_normal.get())
	{
	  VrmlMFVec3f &n = d_normal.get()->toNormal()->normal();
	  normals = &n[0][0];
	}

      if (d_color.get())
	{
	  VrmlMFColor &c = d_color.get()->toColor()->color();
	  colors = &c[0][0];
	}

      // insert geometry
      unsigned int optMask = 0;
      if (d_ccw.get()) optMask |= Viewer::MASK_CCW;
      if (d_solid.get()) optMask |= Viewer::MASK_SOLID;
      if (d_colorPerVertex.get()) optMask |= Viewer::MASK_COLOR_PER_VERTEX;
      if (d_normalPerVertex.get()) optMask |= Viewer::MASK_NORMAL_PER_VERTEX;

      obj = viewer->insertElevationGrid(optMask,
					d_xDimension.get(),
					d_zDimension.get(),
					d_height.get(),
					d_xSpacing.get(),
					d_zSpacing.get(),
					tc,
					normals,
					colors);
    }

  if (d_color.get()) d_color.get()->clearModified();
  if (d_normal.get()) d_normal.get()->clearModified();
  if (d_texCoord.get()) d_texCoord.get()->clearModified();

  return obj;
}

// Get the value of one of the exposedFields or eventOuts.

const VrmlField *VrmlNodeElevationGrid::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  else if ( strcmp( fieldName, "normal" ) == 0 )
    return &d_normal;
  else if ( strcmp( fieldName, "texCoord" ) == 0 )
    return &d_texCoord;
  return VrmlNodeGeometry::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeElevationGrid::setField(const char *fieldName,
				     const VrmlField &fieldValue)
{
  if TRY_SFNODE_FIELD(color, Color)
  else if TRY_SFNODE_FIELD(normal, Normal)
  else if TRY_SFNODE_FIELD(texCoord, TextureCoordinate)
  else if TRY_FIELD(ccw, SFBool)
  else if TRY_FIELD(colorPerVertex, SFBool)
  else if TRY_FIELD(creaseAngle, SFFloat)
  else if TRY_FIELD(height, MFFloat)
  else if TRY_FIELD(normalPerVertex, SFBool)
  else if TRY_FIELD(solid, SFBool)
  else if TRY_FIELD(xDimension, SFInt32)
  else if TRY_FIELD(xSpacing, SFFloat)
  else if TRY_FIELD(zDimension, SFInt32)
  else if TRY_FIELD(zSpacing, SFFloat)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
}

// LarryD Mar 09/99
VrmlNodeElevationGrid* VrmlNodeElevationGrid::toElevationGrid() const 
{ return (VrmlNodeElevationGrid*) this; }

/**
 * @brief Get the Color node.
 *
 * @return an SFNode object containing the Color node associated with
 *         this ElevationGrid.
 */
const VrmlSFNode & VrmlNodeElevationGrid::getColor() const {
    return this->d_color;
}

/**
 * @brief Set the Color node.
 *
 * @param color an SFNode value containing a Color node.
 */
void VrmlNodeElevationGrid::setColor(const VrmlSFNode & color) {
    assert(!color.get() || dynamic_cast<VrmlNodeColor *>(color.get()));
    this->d_color = color;
}

/**
 * @brief Get the Normal node.
 *
 * @return an SFNode object containing the Normal node associated with
 *         this ElevationGrid.
 */
const VrmlSFNode & VrmlNodeElevationGrid::getNormal() const {
    return this->d_normal;
}

/**
 * @brief Set the Normal node.
 *
 * @param normal an SFNode value containing a Normal node.
 */
void VrmlNodeElevationGrid::setNormal(const VrmlSFNode & normal) {
    assert(!normal.get() || dynamic_cast<VrmlNodeNormal *>(normal.get()));
    this->d_normal = normal;
}

/**
 * @brief Get the TextureCoordinate node.
 *
 * @return an SFNode object containing the TextureCoordinate node
 *         associated with this ElevationGrid.
 */
const VrmlSFNode & VrmlNodeElevationGrid::getTexCoord() const {
    return this->d_texCoord;
}

/**
 * @brief Set the TextureCoordinate node.
 *
 * @param texCoord an SFNode value containing a TextureCoordinate node.
 */
void VrmlNodeElevationGrid::setTexCoord(const VrmlSFNode & texCoord) {
    assert(!texCoord.get()
            || dynamic_cast<VrmlNodeTextureCoordinate *>(texCoord.get()));
    this->d_texCoord = texCoord;
}

/**
 * @brief Get whether vertices are ordered counter-clockwise.
 *
 * @return an SFBool object.
 */
const VrmlSFBool & VrmlNodeElevationGrid::getCcw() const {
    return this->d_ccw;
}

/**
 * @brief Set whether vertices are ordered counter-clockwise.
 *
 * @param ccw an SFBool object.
 */
void VrmlNodeElevationGrid::setCcw(const VrmlSFBool & ccw) {
    this->d_ccw = ccw;
}

/**
 * @brief Get whether colors are interpreted per-vertex.
 *
 * @return an SFBool object.
 */
const VrmlSFBool & VrmlNodeElevationGrid::getColorPerVertex() const {
    return this->d_colorPerVertex;
}

/**
 * @brief Set whether colors are interpreted per-vertex.
 *
 * @param colorPerVertex an SFBool object.
 */
void VrmlNodeElevationGrid::setColorPerVertex(const VrmlSFBool & colorPerVertex)
{
    this->d_colorPerVertex = colorPerVertex;
}

/**
 * @brief Get the crease angle.
 *
 * @return the crease angle.
 */
const VrmlSFFloat & VrmlNodeElevationGrid::getCreaseAngle() const {
    return this->d_creaseAngle;
}

/**
 * @brief Set the crease angle.
 *
 * @param creaseAngle
 */
void VrmlNodeElevationGrid::setCreaseAngle(const VrmlSFFloat & creaseAngle) {
    this->d_creaseAngle = creaseAngle;
}

/**
 * @brief Get the height field.
 *
 * @return the height field
 */
const VrmlMFFloat & VrmlNodeElevationGrid::getHeight() const {
    return this->d_height;
}

/**
 * @brief Set the height field.
 *
 * @param height a height field
 */
void VrmlNodeElevationGrid::setHeight(const VrmlMFFloat & height) {
    this->d_height = height;
}

/**
 * @brief Get whether normals are interpreted per-vertex.
 *
 * @return an SFBool object.
 */
const VrmlSFBool & VrmlNodeElevationGrid::getNormalPerVertex() const {
    return this->d_normalPerVertex;
}

/**
 * @brief Set whether normals are interpreted per-vertex.
 *
 * @param normalPerVertex an SFBool object.
 */
void VrmlNodeElevationGrid::setNormalPerVertex(const VrmlSFBool &
                                                            normalPerVertex) {
    this->d_normalPerVertex = normalPerVertex;
}

/**
 * @brief Get whether the geometry should be interpreted as a solid.
 *
 * @return an SFBool object.
 */
const VrmlSFBool & VrmlNodeElevationGrid::getSolid() const {
    return this->d_solid;
}

/**
 * @brief Set whether the geometry should be interpreted as solid.
 *
 * @param solid
 */
void VrmlNodeElevationGrid::setSolid(const VrmlSFBool & solid) {
    this->d_solid = solid;
}

/**
 * @brief Get the number of vertices in the <var>x</var>-dimension.
 *
 * @return the xDimension
 */
const VrmlSFInt32 & VrmlNodeElevationGrid::getXDimension() const {
    return this->d_xDimension;
}

/**
 * @brief Set the number of vertices in the <var>x</var>-dimension.
 *
 * @param xDimension
 */
void VrmlNodeElevationGrid::setXDimension(const VrmlSFInt32 & xDimension) {
    this->d_xDimension = xDimension;
}

/**
 * @brief Get the distance between vertices in the
 *        <var>x</var>-dimension.
 *
 * @return the xSpacing
 */
const VrmlSFFloat & VrmlNodeElevationGrid::getXSpacing() const {
    return this->d_xSpacing;
}

/**
 * @brief Set the distance between vertices in the
 *        <var>x</var>-dimension.
 *
 * @param xSpacing
 */
void VrmlNodeElevationGrid::setXSpacing(const VrmlSFFloat & xSpacing) {
    this->d_xSpacing = xSpacing;
}

/**
 * @brief Get the number of vertices in the <var>z</var>-dimension.
 *
 * @return the zDimension
 */
const VrmlSFInt32 & VrmlNodeElevationGrid::getZDimension() const {
    return this->d_zDimension;
}

/**
 * @brief Set the number of vertices in the <var>z</var>-dimension.
 *
 * @param zDimension
 */
void VrmlNodeElevationGrid::setZDimension(const VrmlSFInt32 & zDimension) {
    this->d_zDimension = zDimension;
}

/**
 * @brief Get the distance between vertices in the
 *        <var>z</var>-dimension.
 *
 * @return the zSpacing
 */
const VrmlSFFloat & VrmlNodeElevationGrid::getZSpacing() const {
    return this->d_zSpacing;
}

/**
 * @brief Set the distance between vertices in the
 *        <var>z</var>-dimension.
 *
 * @param zSpacing
 */
void VrmlNodeElevationGrid::setZSpacing(const VrmlSFFloat & zSpacing) {
    this->d_zSpacing = zSpacing;
}
