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

#include "VrmlNodeAppearance.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"

#include "Viewer.h"
#include "VrmlNodeMaterial.h"
#include "VrmlNodeTexture.h"
#include "VrmlNodeTextureTransform.h"

static VrmlNode *creator( VrmlScene *scene ) 
{ return new VrmlNodeAppearance(scene); }


// Define the built in VrmlNodeType:: "Appearance" fields

VrmlNodeType *VrmlNodeAppearance::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("Appearance", creator);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("material", VrmlField::SFNODE);
  t->addExposedField("texture", VrmlField::SFNODE);
  t->addExposedField("textureTransform", VrmlField::SFNODE);

  return t;
}

VrmlNodeType & VrmlNodeAppearance::nodeType() const
{
    return *defineType(0);
}


VrmlNodeAppearance::VrmlNodeAppearance(VrmlScene *scene) :
  VrmlNode(scene)
{
}

VrmlNodeAppearance::~VrmlNodeAppearance()
{
}

bool VrmlNodeAppearance::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeAppearance::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_material.get()) {
            this->d_material.get()->resetVisitedFlag();
        }
        if (this->d_texture.get()) {
            this->d_texture.get()->resetVisitedFlag();
        }
        if (this->d_textureTransform.get()) {
            this->d_textureTransform.get()->resetVisitedFlag();
        }
    }
}

VrmlNodeAppearance* VrmlNodeAppearance::toAppearance() const
{
  return (VrmlNodeAppearance*) this;
}


bool VrmlNodeAppearance::isModified() const
{
  return ( d_modified ||
	   (d_material.get() && d_material.get()->isModified()) ||
	   (d_texture.get() && d_texture.get()->isModified()) ||
	   (d_textureTransform.get() &&
	    d_textureTransform.get()->isModified()) );
}

void VrmlNodeAppearance::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_material.get()) d_material.get()->updateModified(path);
  if (d_texture.get()) d_texture.get()->updateModified(path);
  if (d_textureTransform.get()) d_textureTransform.get()->updateModified(path);
  path.pop_front();
}

void VrmlNodeAppearance::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_material.get()) d_material.get()->clearFlags();
  if (d_texture.get()) d_texture.get()->clearFlags();
  if (d_textureTransform.get()) d_textureTransform.get()->clearFlags();
}

void VrmlNodeAppearance::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  if (d_material.get()) d_material.get()->addToScene(s, rel);
  if (d_texture.get()) d_texture.get()->addToScene(s, rel);
  if (d_textureTransform.get())
    d_textureTransform.get()->addToScene(s,rel);
}

ostream& VrmlNodeAppearance::printFields(ostream& os, int indent)
{
  if (d_material.get()) PRINT_FIELD(material);
  if (d_texture.get()) PRINT_FIELD(texture);
  if (d_textureTransform.get()) PRINT_FIELD(textureTransform);

  return os;
}


void VrmlNodeAppearance::render(Viewer *viewer, VrmlRenderContext rc)
{
  VrmlNodeMaterial *m = d_material.get() ? d_material.get()->toMaterial() : 0;
  VrmlNodeTexture *t = d_texture.get() ? d_texture.get()->toTexture() : 0;

  if (m)
    {
      float trans = m->transparency();
      const float *diff = m->diffuseColor();
      float diffuse[3] = { diff[0], diff[1], diff[2] };
      int nTexComponents = t ? t->nComponents() : 0;
      if (nTexComponents == 2 || nTexComponents == 4)
	trans = 0.0;
      if (nTexComponents >= 3)
	diffuse[0] = diffuse[1] = diffuse[2] = 1.0;

      viewer->enableLighting(true);   // turn lighting on for this object
      viewer->setMaterial(m->ambientIntensity(),
			  diffuse,
			  m->emissiveColor(),
			  m->shininess(),
			  m->specularColor(),
			  trans);

      m->clearModified();
    }
  else
    {
      viewer->setColor(1.0, 1.0, 1.0); // default color
      viewer->enableLighting(false);   // turn lighting off for this object
    }

  if (t)
    {
      if (d_textureTransform.get())
	d_textureTransform.get()->render(viewer, rc);
      else
	viewer->setTextureTransform( 0, 0, 0, 0 );

      t->render(viewer, rc);
    }

  clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeAppearance::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "material" ) == 0 )
    return &d_material;
  else if ( strcmp( fieldName, "texture" ) == 0 )
    return &d_texture;
  else if ( strcmp( fieldName, "textureTransform" ) == 0 )
    return &d_textureTransform;
  
  return VrmlNode::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeAppearance::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_SFNODE_FIELD(material, Material)
  else if TRY_SFNODE_FIELD(texture, Texture)
  else if TRY_SFNODE_FIELD(textureTransform, TextureTransform)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

/**
 * @brief Get the Material node.
 *
 * @returns an SFNode object containing the Material node associated with
 *          this Appearance.
 */
const VrmlSFNode & VrmlNodeAppearance::getMaterial() const {
    return this->d_material;
}

/**
 * @brief Set the Material node.
 *
 * @param material an SFNode object containing a Material node
 */
void VrmlNodeAppearance::setMaterial(const VrmlSFNode & material) {
    assert(!material.get() || material.get()->toMaterial());
    this->d_material = material;
}

/**
 * @brief Get the texture node.
 *
 * @return an SFNode object containing the texture node associated with
 *         this Appearance.
 */
const VrmlSFNode & VrmlNodeAppearance::getTexture() const {
    return this->d_texture;
}

/**
 * @brief Set the texture node.
 *
 * @param texture an SFNode object containing a texture node.
 */
void VrmlNodeAppearance::setTexture(const VrmlSFNode & texture) {
    assert(!texture.get() || texture.get()->toTexture());
    this->d_texture = texture;
}

/**
 * @brief Get the TextureTransform node.
 *
 * @return an SFNode object containing the TextureTransform node
 *         associated with this Appearance.
 */
const VrmlSFNode & VrmlNodeAppearance::getTextureTransform() const {
    return this->d_textureTransform;
}

/**
 * @brief Set the TextureTransform node.
 *
 * @param textureTransform an SFNode object containing a
 *                         TextureTransform node.
 */
void VrmlNodeAppearance::setTextureTransform(const VrmlSFNode & textureTransform) {
    assert(!textureTransform.get()
            || textureTransform.get()->toTextureTransform());
    this->d_textureTransform = textureTransform;
}
