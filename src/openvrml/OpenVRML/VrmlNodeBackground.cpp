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
//  VrmlNodeBackground.cpp
//

#include "VrmlNodeBackground.h"

#include "doc2.hpp"
#include "Image.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"
#include "VrmlSFBool.h"
#include "VrmlScene.h"
#include "Viewer.h"


//  Background factory.

static VrmlNode *creator(VrmlScene *scene)
{
  return new VrmlNodeBackground(scene);
}


// Define the built in VrmlNodeType:: "Background" fields

VrmlNodeType *VrmlNodeBackground::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;	// Define the type once.
      t = st = new VrmlNodeType("Background", creator);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_bind", VrmlField::SFBOOL);

  t->addExposedField("groundAngle", VrmlField::MFFLOAT);
  t->addExposedField("groundColor", VrmlField::MFCOLOR);

  t->addExposedField("backUrl", VrmlField::MFSTRING);
  t->addExposedField("bottomUrl", VrmlField::MFSTRING);
  t->addExposedField("frontUrl", VrmlField::MFSTRING);
  t->addExposedField("leftUrl", VrmlField::MFSTRING);
  t->addExposedField("rightUrl", VrmlField::MFSTRING);
  t->addExposedField("topUrl", VrmlField::MFSTRING);

  t->addExposedField("skyAngle", VrmlField::MFFLOAT);
  t->addExposedField("skyColor", VrmlField::MFCOLOR);

  t->addEventOut("isBound", VrmlField::SFBOOL);

  return t;
}


VrmlNodeType & VrmlNodeBackground::nodeType() const
{
    return *defineType(0);
}


VrmlNodeBackground::VrmlNodeBackground(VrmlScene *scene)
  : VrmlNodeChild(scene),
    d_isBound(false),
    d_viewerObject(0)
{
  for (int i=0; i<6; ++i) d_texPtr[i] = 0;
  if (d_scene) d_scene->addBackground(this);
}

VrmlNodeBackground::~VrmlNodeBackground()
{
  if (d_scene) d_scene->removeBackground(this);
  // remove d_viewerObject...
}

bool VrmlNodeBackground::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeBackground* VrmlNodeBackground::toBackground() const
{ return (VrmlNodeBackground*) this; }

void VrmlNodeBackground::addToScene(VrmlScene *s, const char *rel)
{
  if (d_scene != s && (d_scene = s) != 0) d_scene->addBackground(this);
  d_relativeUrl.set(rel);
}


ostream& VrmlNodeBackground::printFields(ostream& os, int indent)
{
  if (d_groundAngle.getLength()) PRINT_FIELD(groundAngle);
  if (d_groundColor.getLength()) PRINT_FIELD(groundColor);
  if (d_skyAngle.getLength()) PRINT_FIELD(skyAngle);
  if (d_skyColor.getLength()) PRINT_FIELD(skyColor);
  if (d_backUrl.get()) PRINT_FIELD(backUrl);
  if (d_bottomUrl.get()) PRINT_FIELD(bottomUrl);
  if (d_frontUrl.get()) PRINT_FIELD(frontUrl);
  if (d_leftUrl.get()) PRINT_FIELD(leftUrl);
  if (d_rightUrl.get()) PRINT_FIELD(rightUrl);
  if (d_topUrl.get()) PRINT_FIELD(topUrl);

  return os;
}


// Load and scale textures as needed.

static Image* getTexture( VrmlMFString &urls,
			  Doc2 * relative,
			  Image *tex,
			  int thisIndex,
			  Viewer *viewer )
{
  // Check whether the url has already been loaded
  int n = urls.getLength();
  if (n > 0)
    {
      for (int index=thisIndex-1; index >= 0; --index)
	{
	  const char *currentTex = tex[index].url();
	  const char *relPath = relative ? relative->urlPath() : 0;
	  int currentLen = currentTex ? strlen(currentTex) : 0;
	  int relPathLen = relPath ? strlen(relPath) : 0;
	  if (relPathLen >= currentLen) relPathLen = 0;

	  if (currentTex)
	    for (int i=0; i<n; ++i)
	      if (strcmp(currentTex, urls.get(i)) == 0 ||
		  strcmp(currentTex+relPathLen, urls.get(i)) == 0 )
		return &tex[index];
	}

      // Have to load it
      if (!tex[thisIndex].tryURLs(urls, relative)) {
	cerr << "Error: couldn't read Background texture from URL "
	     << urls << endl;
      }

      // check whether it needs to be scaled
      else if ( tex[thisIndex].pixels() && tex[thisIndex].nc() )
	{
	  // Ensure the image dimensions are powers of two
	  int sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
	  int nSizes = sizeof(sizes) / sizeof(int);
	  int w = tex[thisIndex].w();
	  int h = tex[thisIndex].h();
	  int i, j;
	  for (i=0; i<nSizes; ++i)
	    if (w < sizes[i]) break;
	  for (j=0; j<nSizes; ++j)
	    if (h < sizes[j]) break;
	  
	  if (i > 0 && j > 0)
	    {
	      // Always scale images down in size and reuse the same pixel
	      // memory. This can cause some ugliness...
	      if (w != sizes[i-1] || h != sizes[j-1])
		{
		  viewer->scaleTexture( w, h, sizes[i-1], sizes[j-1],
					tex[thisIndex].nc(),
					tex[thisIndex].pixels());
		  tex[thisIndex].setSize( sizes[i-1], sizes[j-1] );
		}
	    }
	}
    }

  return &tex[thisIndex];
}


// Backgrounds are rendered once per scene at the beginning, not
// when they are traversed by the standard render() method.

void VrmlNodeBackground::renderBindable(Viewer *viewer)
{
  // Background isn't selectable, so don't waste the time.
  if (viewer->getRenderMode() == Viewer::RENDER_MODE_PICK)
    return;

  if (d_viewerObject && isModified())
    {
      viewer->removeObject(d_viewerObject);
      d_viewerObject = 0;
    }

  if (d_viewerObject)
    viewer->insertReference(d_viewerObject);
  else
    {
      if (isModified() || d_texPtr[0] == 0)
	{
	  Doc2 relDoc( d_relativeUrl.get() );
	  Doc2 *rel = d_relativeUrl.get() ? &relDoc : d_scene->urlDoc();
	  d_texPtr[0] = getTexture( d_backUrl, rel, d_tex, 0, viewer );
	  d_texPtr[1] = getTexture( d_bottomUrl, rel, d_tex, 1, viewer );
	  d_texPtr[2] = getTexture( d_frontUrl, rel, d_tex, 2, viewer );
	  d_texPtr[3] = getTexture( d_leftUrl, rel, d_tex, 3, viewer );
	  d_texPtr[4] = getTexture( d_rightUrl, rel, d_tex, 4, viewer );
	  d_texPtr[5] = getTexture( d_topUrl, rel, d_tex, 5, viewer );
	}

      int i, whc[18];	// Width, height, and nComponents for 6 tex
      unsigned char *pixels[6];
      int nPix = 0;

      for (i=0; i<6; ++i)
	{
	  whc[3*i+0] = d_texPtr[i]->w();
	  whc[3*i+1] = d_texPtr[i]->h();
	  whc[3*i+2] = d_texPtr[i]->nc();
	  pixels[i] = d_texPtr[i]->pixels();
	  if (whc[3*i+0] > 0 && whc[3*i+1] > 0 && whc[3*i+2] > 0 &&
	      pixels[i])
	    ++nPix;
	}
	  
      d_viewerObject =
	viewer->insertBackground(nGroundAngles(),
				 groundAngle(),
				 groundColor(),
				 nSkyAngles(),
				 skyAngle(),
				 skyColor(),
				 whc,
				 (nPix > 0) ? pixels : 0);

      clearModified();
    }
}


// Note that this method is not maintaining isBound.

void VrmlNodeBackground::eventIn(double timeStamp,
				 const char *eventName,
				 const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_bind") == 0)
    {
      VrmlNodeBackground *current = d_scene->bindableBackgroundTop();
      const VrmlSFBool * const b =
                dynamic_cast<const VrmlSFBool *>(&fieldValue);
      if (! b)
	{
	  cerr << "Error: invalid value for Background::set_bind eventIn "
	       << fieldValue << endl;
	  return;
	}

      
      if ( b->get() )		// set_bind TRUE
	{
	  if (this != current)
	    {
	      if (current)
		current->eventOut( timeStamp, "isBound", VrmlSFBool(false));
	      d_scene->bindablePush( this );
	      eventOut( timeStamp, "isBound", VrmlSFBool(true) );
	    }
	}
      else			// set_bind FALSE
	{
	  d_scene->bindableRemove( this );
	  if (this == current)
	    {
	      eventOut( timeStamp, "isBound", VrmlSFBool(false));
	      current = d_scene->bindableBackgroundTop();
	      if (current)
		current->eventOut( timeStamp, "isBound", VrmlSFBool(true) );
	    }
	}
    }

  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);
    }
}

// Get the value of a field or eventOut.
// The isBound eventOut is only set when queried,
// don't rely on it's value to be valid. This hoses
// the const-ness of the method, of course :(

const VrmlField *VrmlNodeBackground::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "groundAngle" ) == 0 )
    return &d_groundAngle;
  else if ( strcmp( fieldName, "groundColor" ) == 0 )
    return &d_groundColor;
  else if ( strcmp( fieldName, "backUrl" ) == 0 )
    return &d_backUrl;
  else if ( strcmp( fieldName, "bottomUrl" ) == 0 )
    return &d_bottomUrl;
  else if ( strcmp( fieldName, "frontUrl" ) == 0 )
    return &d_frontUrl;
  else if ( strcmp( fieldName, "leftUrl" ) == 0 )
    return &d_leftUrl;
  else if ( strcmp( fieldName, "rightUrl" ) == 0 )
    return &d_rightUrl;
  else if ( strcmp( fieldName, "topUrl" ) == 0 )
    return &d_topUrl;
  else if ( strcmp( fieldName, "skyAngle" ) == 0 )
    return &d_skyAngle;
  else if ( strcmp( fieldName, "skyColor" ) == 0 )
    return &d_skyColor;

  // eventOuts
  else if ( strcmp( fieldName, "isBound" ) == 0 )
    {
      VrmlSFBool* isBound = (VrmlSFBool*) &(this->d_isBound);
      isBound->set( d_scene->bindableBackgroundTop() == this );
      return isBound;
    }

  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeBackground::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_FIELD(groundAngle, MFFloat)
  else if TRY_FIELD(groundColor, MFColor)
  else if TRY_FIELD(backUrl, MFString)
  else if TRY_FIELD(bottomUrl, MFString)
  else if TRY_FIELD(frontUrl, MFString)
  else if TRY_FIELD(leftUrl, MFString)
  else if TRY_FIELD(rightUrl, MFString)
  else if TRY_FIELD(topUrl, MFString)
  else if TRY_FIELD(skyAngle, MFFloat)
  else if TRY_FIELD(skyColor, MFColor)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

