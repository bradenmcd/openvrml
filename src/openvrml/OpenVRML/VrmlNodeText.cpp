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

#include "VrmlNodeText.h"
#include "VrmlNodeType.h"
#include "VrmlNodeFontStyle.h"
#include "MathUtils.h"
#include "Viewer.h"

// Make a VrmlNodeText

static VrmlNode *creator( VrmlScene *s ) { return new VrmlNodeText(s); }


// Define the built in VrmlNodeType:: "Text" fields

VrmlNodeType *VrmlNodeText::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;	// Define type only once.
      t = st = new VrmlNodeType("Text", creator);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class
  t->addExposedField("string", VrmlField::MFSTRING);
  t->addExposedField("fontStyle", VrmlField::SFNODE);
  t->addExposedField("length", VrmlField::MFFLOAT);
  t->addExposedField("maxExtent", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodeText::nodeType() const
{
    return *defineType(0);
}


VrmlNodeText::VrmlNodeText(VrmlScene *scene) :
  VrmlNodeGeometry(scene)
{
}

VrmlNodeText::~VrmlNodeText()
{
}


VrmlNode *VrmlNodeText::cloneMe() const
{
  return new VrmlNodeText(*this);
}

void VrmlNodeText::cloneChildren(VrmlNamespace *ns)
{
  if (d_fontStyle.get())
    d_fontStyle.set(d_fontStyle.get()->clone(ns));
}

bool VrmlNodeText::isModified() const
{
  return (VrmlNode::isModified() ||
	  (d_fontStyle.get() && d_fontStyle.get()->isModified()));
}

void VrmlNodeText::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_fontStyle.get()) d_fontStyle.get()->updateModified(path);
  path.pop_front();
}


void VrmlNodeText::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_fontStyle.get()) d_fontStyle.get()->clearFlags();
}

void VrmlNodeText::addToScene( VrmlScene *s, const char *relUrl )
{
  d_scene = s;
  if (d_fontStyle.get()) d_fontStyle.get()->addToScene(s, relUrl);
}

void VrmlNodeText::copyRoutes( VrmlNamespace *ns ) const
{
  VrmlNode::copyRoutes(ns);
  if (d_fontStyle.get()) d_fontStyle.get()->copyRoutes(ns);
}

ostream& VrmlNodeText::printFields(ostream& os, int indent)
{
  if (d_string.getLength() > 0) PRINT_FIELD(string);
  if (d_fontStyle.get()) PRINT_FIELD(fontStyle);
  if (d_length.getLength() > 0) PRINT_FIELD(length);
  if (! FPZERO(d_maxExtent.get()) ) PRINT_FIELD(maxExtent);
  return os;
}


Viewer::Object VrmlNodeText::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
    char const * const * s = d_string.get();
    
    if (s) {
        int justify[2] = { 1, 1 };
        float size = 1.0;
        VrmlNodeFontStyle *f = 0;
        if (d_fontStyle.get()) {
            f = d_fontStyle.get()->toFontStyle();
        }
        
        if (f) {
            VrmlMFString & j = f->justify();
            
            for (size_t i=0; i<j.getLength(); ++i) {
                if (strcmp(j.get(i), "END") == 0) {
                    justify[i] = -1;
                } else if (strcmp(j.get(i), "MIDDLE") == 0) {
                    justify[i] = 0;
                }
            }
            size = f->size();
        }
        
        return viewer->insertText(justify, size, d_string.getLength(), s);
    }
    
    return 0;
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeText::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "string" ) == 0 )
    return &d_string;
  else if ( strcmp( fieldName, "fontStyle" ) == 0 )
    return &d_fontStyle;
  else if ( strcmp( fieldName, "length" ) == 0 )
    return &d_maxExtent;
  
  return VrmlNodeGeometry::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeText::setField(const char *fieldName,
			    const VrmlField &fieldValue)
{
  if TRY_FIELD(string, MFString)
  else if TRY_SFNODE_FIELD(fontStyle, FontStyle)
  else if TRY_FIELD(length, MFFloat)
  else if TRY_FIELD(maxExtent, SFFloat)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
}

/**
 * @brief Get the FontStyle associated with this Text node.
 *
 * @return fontStyle
 */
const VrmlSFNode & VrmlNodeText::getFontStyle() const {
    return this->d_fontStyle;
}

/**
 * @brief Set the FontStyle associated with this Text node.
 *
 * @param fontStyle
 */
void VrmlNodeText::setFontStyle(const VrmlSFNode & fontStyle) {
    this->d_fontStyle = fontStyle;
}
