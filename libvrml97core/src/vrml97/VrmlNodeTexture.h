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

#ifndef VRMLNODETEXTURE_H
#define VRMLNODETEXTURE_H

#include "VrmlNode.h"
#include "VrmlSFString.h"

class VrmlNodePixelTexture;
class VrmlNodeImageTexture;

class VrmlNodeTexture : public VrmlNode {

public:

  // Define the fields of Texture nodes
  static VrmlNodeType * defineType(VrmlNodeType *t);

  VrmlNodeTexture(VrmlScene *s);
  virtual ~VrmlNodeTexture();

  virtual VrmlNodeTexture* toTexture() const;

  virtual int nComponents() = 0;
  virtual int width() = 0;
  virtual int height() = 0;
  virtual int nFrames() = 0;
  virtual unsigned char* pixels() = 0;

  //similar to the VrmlNode calls, but placed here, cause they make more sense here.
  virtual VrmlNodePixelTexture*	toPixelTexture() const { return NULL; }
  virtual VrmlNodeImageTexture*	toImageTexture() const { return NULL; }

  void addToScene( VrmlScene *scene, const char *relativeUrl )
    { d_scene = scene; d_relativeUrl.set( relativeUrl ); }

protected:

  VrmlSFString d_relativeUrl;

};

#endif
