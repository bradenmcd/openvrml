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

#ifndef  VRMLNODEIMAGETEXTURE_H
#define  VRMLNODEIMAGETEXTURE_H

#include "VrmlNodeTexture.h"
#include "VrmlMFString.h"
#include "VrmlSFBool.h"
#include "Viewer.h"

class Image;

class VrmlNodeImageTexture : public VrmlNodeTexture {

public:

  // Define the fields of ImageTexture nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeImageTexture(VrmlScene *);
  virtual ~VrmlNodeImageTexture();

  virtual VrmlNode *cloneMe() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual size_t nComponents();
  virtual size_t width();
  virtual size_t height();
  virtual size_t nFrames();
  virtual const unsigned char * pixels();

  virtual const VrmlMFString& getUrl() const;

  virtual VrmlNodeImageTexture*	toImageTexture() const;

  //
  // Aren't repeatS and repeatT shared by all texture nodes? Perhaps these
  // methods should be moved to the base class, VrmlNodeTexture. (Also, these
  // should probably return VrmlSFBool const references to be consistent with
  // the getUrl method.
  // -- Braden McDaniel, <braden@endoframe.com>, 11 Apr, 2000
  //
  virtual bool getRepeatS() const; // LarryD Feb18/99
  virtual bool getRepeatT() const; // LarryD Feb18/99

private:

  VrmlMFString d_url;
  VrmlSFBool d_repeatS;
  VrmlSFBool d_repeatT;

  Image *d_image;

  Viewer::TextureObject d_texObject;

};

#endif // VRMLNODEIMAGETEXTURE_H

