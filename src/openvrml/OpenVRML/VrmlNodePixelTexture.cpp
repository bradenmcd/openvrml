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

#include "VrmlNodePixelTexture.h"
#include "VrmlNodeType.h"
#include "VrmlScene.h"

static VrmlNode *creator( VrmlScene *s ) {
  return new VrmlNodePixelTexture(s); 
}

VrmlNodePixelTexture* VrmlNodePixelTexture::toPixelTexture() const
{ return (VrmlNodePixelTexture*) this; }


// Define the built in VrmlNodeType:: "PixelTexture" fields

VrmlNodeType *VrmlNodePixelTexture::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("PixelTexture", creator);
      t->reference();
    }

  VrmlNodeTexture::defineType(t);	// Parent class

  t->addExposedField("image", VrmlField::SFIMAGE);
  t->addField("repeatS", VrmlField::SFBOOL);
  t->addField("repeatT", VrmlField::SFBOOL);
  return t;
}

VrmlNodeType & VrmlNodePixelTexture::nodeType() const
{
    return *defineType(0);
}


VrmlNodePixelTexture::VrmlNodePixelTexture(VrmlScene *scene) :
  VrmlNodeTexture(scene),
  d_repeatS(true),
  d_repeatT(true),
  d_texObject(0)
{
}

VrmlNodePixelTexture::~VrmlNodePixelTexture()
{
  // viewer->removeTextureObject( d_texObject ); ...
}


VrmlNode *VrmlNodePixelTexture::cloneMe() const
{
  return new VrmlNodePixelTexture(*this);
}


ostream& VrmlNodePixelTexture::printFields(ostream& os, int indent)
{
  if (! d_repeatS.get()) PRINT_FIELD(repeatS);
  if (! d_repeatT.get()) PRINT_FIELD(repeatT);
  if (d_image.getWidth() > 0 &&
      d_image.getHeight() > 0 &&
      d_image.getComponents() > 0 &&
      d_image.getPixels() != 0)
    PRINT_FIELD(image);
  return os;
}

      
void VrmlNodePixelTexture::render(Viewer *viewer, VrmlRenderContext rc) {
    if (isModified()) {
        if (d_texObject) {
            viewer->removeTextureObject(d_texObject);
            d_texObject = 0;
        }
    }
    
    if (this->d_image.getPixels()) {
        if (d_texObject) {
            viewer->insertTextureReference(d_texObject,
                                           d_image.getComponents());
        } else {
            // Ensure the image dimensions are powers of two
            const int sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
            const int nSizes = sizeof(sizes) / sizeof(int);
            int w = d_image.getWidth();
            int h = d_image.getHeight();
            int i, j;
            for (i=0; i<nSizes; ++i) {
                if (w < sizes[i]) break;
            }
            for (j=0; j<nSizes; ++j) {
                if (h < sizes[j]) break;
            }
            
            if (i > 0 && j > 0) {
                // Always scale images down in size and reuse the same pixel memory.
                
                // What if we had multiple renderers serving the same scene, and
                // the renderers had different requirements for rescaling the
                // image? I think it would be better to keep the rescaled image
                // in a cache in the renderer.
                //   -- Braden McDaniel <braden@endoframe.com>, 9 Dec, 2000
                if (w != sizes[i-1] || h != sizes[j-1]) {
                    const size_t numBytes =
                            this->d_image.getWidth() * this->d_image.getHeight()
                                * this->d_image.getComponents();
                    unsigned char * pixels = new unsigned char[numBytes];
                    std::copy(this->d_image.getPixels(),
                              this->d_image.getPixels() + numBytes,
                              pixels);
    
                    viewer->scaleTexture(w, h, sizes[i-1], sizes[j-1],
                                         d_image.getComponents(), pixels);
                    d_image.set(sizes[i-1], sizes[j-1], d_image.getComponents(),
                                pixels);
                    delete [] pixels;
		}
                
                d_texObject = viewer->insertTexture(d_image.getWidth(),
                                                    d_image.getHeight(),
                                                    d_image.getComponents(),
                                                    d_repeatS.get(),
                                                    d_repeatT.get(),
                                                    d_image.getPixels(),
                                                    true);
	    }
	}
    }
    
    clearModified();
}


size_t VrmlNodePixelTexture::nComponents()
{
  return d_image.getComponents();
}

size_t VrmlNodePixelTexture::width()
{
  return d_image.getWidth();
}

size_t VrmlNodePixelTexture::height()
{
  return d_image.getHeight();
}

size_t VrmlNodePixelTexture::nFrames()
{
  return 0;
}

const unsigned char * VrmlNodePixelTexture::pixels()
{
  return d_image.getPixels();
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodePixelTexture::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "image" ) == 0 )
    return &d_image;
  
  return VrmlNode::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodePixelTexture::setField(const char *fieldName,
				    const VrmlField &fieldValue)
{
  if TRY_FIELD(image, SFImage)
  else if TRY_FIELD(repeatS, SFBool)
  else if TRY_FIELD(repeatT, SFBool)
  else
    VrmlNode::setField(fieldName, fieldValue);

  //if ( strcmp(fieldName, "image") == 0 )
  //{
  //theSystem->debug("image w h c %dx%dx%d\n", d_image.width(), d_image.height(), d_image.nComponents());
  //unsigned char* p= d_image.pixels();
  //for (int y=0; y<d_image.height(); ++y) {
  //for (int y=0; y<1; ++y) {
  //for (int x=0; x<d_image.width(); ++x) {
  //for (int nc=0; nc<d_image.nComponents(); ++nc, ++p) {
  //if (*p)
  //cout << x << "," << y << "," << nc << ":" << (int)*p << endl;
  //}
  //}
  //}
  //theSystem->debug("pixel %d, %d byte %d: 0x%x",
  //x, y, nc, *p);
  //}
}

const VrmlSFImage & VrmlNodePixelTexture::getImage() const {
    return this->d_image;
}

const VrmlSFBool & VrmlNodePixelTexture::getRepeatS() const {
    return this->d_repeatS;
}

const VrmlSFBool & VrmlNodePixelTexture::getRepeatT() const {
    return this->d_repeatT;
}
