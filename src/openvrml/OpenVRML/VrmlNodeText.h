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

#ifndef  _VRMLNODETEXT_
#define  _VRMLNODETEXT_

#include "VrmlNodeGeometry.h"
#include "VrmlMFString.h"
#include "VrmlMFFloat.h"
#include "VrmlSFFloat.h"
#include "VrmlSFNode.h"

class VrmlScene;

class VrmlNodeText : public VrmlNodeGeometry {

public:

  // Define the fields of text nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeText(VrmlScene *);
  virtual ~VrmlNodeText();

  virtual bool accept(VrmlNodeVisitor & visitor);
  virtual void resetVisitedFlag();
  
  virtual bool isModified() const;
  virtual void updateModified(VrmlNodePath& path);
  virtual void clearFlags();

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual ostream& printFields(ostream& os, int indent);

  virtual Viewer::Object insertGeometry(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);
  
  const VrmlSFNode & getFontStyle() const;
  void setFontStyle(const VrmlSFNode & fontStyle);

protected:

  VrmlMFString d_string;
  VrmlSFNode d_fontStyle;
  VrmlMFFloat d_length;
  VrmlSFFloat d_maxExtent;

};

#endif _VRMLNODETEXT_

