//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  VrmlNodeImageTexture.h

#ifndef  _VRMLNODEIMAGETEXTURE_
#define  _VRMLNODEIMAGETEXTURE_

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

  virtual int nComponents();
  virtual int width();
  virtual int height();
  virtual int nFrames();
  virtual unsigned char* pixels();

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

#endif // _VRMLNODEIMAGETEXTURE_

