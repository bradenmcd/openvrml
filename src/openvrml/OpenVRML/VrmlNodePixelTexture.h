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

#ifndef VRMLNODEPIXELTEXTURE_H
#define VRMLNODEPIXELTEXTURE_H

#include "VrmlNodeTexture.h"
#include "Viewer.h"

class VrmlNodePixelTexture : public VrmlNodeTexture {

public:

  // Define the fields of PixelTexture nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodePixelTexture(VrmlScene *);
  virtual ~VrmlNodePixelTexture();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual size_t nComponents();
  virtual size_t width();
  virtual size_t height();
  virtual size_t nFrames();
  virtual const unsigned char * pixels();

  virtual VrmlNodePixelTexture*	toPixelTexture() const;
  
  const VrmlSFImage & getImage() const;
  const VrmlSFBool & getRepeatS() const;
  const VrmlSFBool & getRepeatT() const;

private:

  VrmlSFImage d_image;
  VrmlSFBool d_repeatS;
  VrmlSFBool d_repeatT;

  Viewer::TextureObject d_texObject;
};

#endif // VRMLNODEPIXELTEXTURE_H

