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

#ifndef VRMLNODEMOVIETEXTURE_H
#define VRMLNODEMOVIETEXTURE_H

#include "VrmlNodeTexture.h"
#include "VrmlMFString.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"
#include "VrmlSFTime.h"
#include "Viewer.h"

class Image;

class VrmlNodeMovieTexture : public VrmlNodeTexture {

public:

  // Define the fields of MovieTexture nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeMovieTexture(VrmlScene *);
  virtual ~VrmlNodeMovieTexture();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual VrmlNodeMovieTexture* toMovieTexture() const;

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual ostream& printFields(ostream& os, int indent);

  void update( VrmlSFTime &now );

  virtual void render(Viewer *, VrmlRenderContext rc);

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField & fieldValue);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  virtual size_t nComponents();
  virtual size_t width();
  virtual size_t height();
  virtual size_t nFrames();
  virtual const unsigned char * pixels();

private:

  VrmlSFBool d_loop;
  VrmlSFFloat d_speed;
  VrmlSFTime d_startTime;
  VrmlSFTime d_stopTime;

  VrmlMFString d_url;
  VrmlSFBool d_repeatS;
  VrmlSFBool d_repeatT;

  VrmlSFTime d_duration;
  VrmlSFBool d_isActive;

  Image *d_image;
  int d_frame, d_lastFrame;
  double d_lastFrameTime;

  Viewer::TextureObject d_texObject;

};

#endif // VRMLNODEMOVIETEXTURE_H

