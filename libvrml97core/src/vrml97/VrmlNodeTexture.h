//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  %W% %G%
//  VrmlNodeTexture.h

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
