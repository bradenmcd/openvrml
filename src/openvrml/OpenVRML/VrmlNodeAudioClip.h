//
// OpenVRML
//
// Copyright (C) 1999  Kumaran Santhanam
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

#ifndef VRMLNODEAUDIOCLIP_H
#define VRMLNODEAUDIOCLIP_H

#include "VrmlNode.h"
#include "VrmlMFString.h"
#include "VrmlSFBool.h"
#include "VrmlSFFloat.h"
#include "VrmlSFString.h"
#include "VrmlSFTime.h"

class Audio;

class VrmlNodeAudioClip : public VrmlNode {

public:

  // Define the fields of AudioClip nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeAudioClip(VrmlScene *);
  VrmlNodeAudioClip(const VrmlNodeAudioClip&);
  virtual ~VrmlNodeAudioClip();

  virtual bool accept(VrmlNodeVisitor & visitor);
  
  virtual void addToScene(VrmlScene *s, const char *relativeUrl);

  void update (VrmlSFTime &now);

  virtual VrmlNodeAudioClip* toAudioClip() const;

  virtual ostream& printFields(ostream& os, int indent);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

private:

  VrmlSFString d_description;
  VrmlSFBool d_loop;
  VrmlSFFloat d_pitch;
  VrmlSFTime d_startTime;
  VrmlSFTime d_stopTime;
  VrmlMFString d_url;

  VrmlSFString d_relativeUrl;

  VrmlSFTime d_duration;
  VrmlSFBool d_isActive;

  Audio *d_audio;
  bool   d_url_modified;
  int    _audio_index;
  double _audio_intensity;
  int    _audio_fd;
};

#endif
