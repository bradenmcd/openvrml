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
//  VrmlNodeAnchor.cpp
//


#include "VrmlNodeAnchor.h"
#include "VrmlNodeType.h"
#include "VrmlNodeVisitor.h"
#include "VrmlScene.h"

#include "doc2.hpp"


static VrmlNode *creator( VrmlScene *scene ) 
{
  return new VrmlNodeAnchor(scene);
}


// Define the built in VrmlNodeType:: "Anchor" fields

VrmlNodeType *VrmlNodeAnchor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;
  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Anchor", creator);
      t->reference();
    }

  VrmlNodeGroup::defineType(t);	// Parent class
  t->addExposedField("description", VrmlField::SFSTRING);
  t->addExposedField("parameter", VrmlField::MFSTRING);
  t->addExposedField("url", VrmlField::MFSTRING);

  return t;
}

VrmlNodeType & VrmlNodeAnchor::nodeType() const
{
    return *VrmlNodeAnchor::defineType(0);
}


VrmlNodeAnchor::VrmlNodeAnchor(VrmlScene *scene) : VrmlNodeGroup(scene)
{
  this->setBVolumeDirty(true);
}

VrmlNodeAnchor::VrmlNodeAnchor(const VrmlNodeAnchor &n) : VrmlNodeGroup(n)
{
  d_description = n.d_description;
  d_parameter = n.d_parameter;
  d_url = n.d_url;
  this->setBVolumeDirty(true);
}

VrmlNodeAnchor::~VrmlNodeAnchor()
{
}

bool VrmlNodeAnchor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeAnchor* VrmlNodeAnchor::toAnchor() const
{ return (VrmlNodeAnchor*)this; }


ostream& VrmlNodeAnchor::printFields(ostream& os, int indent)
{
  VrmlNodeGroup::printFields(os, indent);
  if (d_description.get()) PRINT_FIELD(description);
  if (d_parameter.get()) PRINT_FIELD(parameter);
  if (d_url.get()) PRINT_FIELD(url);

  return os;
}


void VrmlNodeAnchor::render(Viewer *viewer, VrmlRenderContext rc)
{
  viewer->setSensitive( this );

  // Render children
  VrmlNodeGroup::render(viewer, rc);

  viewer->setSensitive( 0 );
}


// Handle a click by loading the url

void VrmlNodeAnchor::activate()
{
  if (d_scene && d_url.getLength() > 0) {
      Doc2*  tmp_url = new Doc2();
      char** tmp_url_array = new char *[d_url.getLength()];
      
      for (size_t i = 0; i < d_url.getLength(); i++) {
        Doc2 relDoc(d_relative.get());
        tmp_url->seturl( d_url.get(i), &relDoc);
        tmp_url_array[i] = new char[ strlen(tmp_url->url()) + 1 ];
        strcpy(tmp_url_array[i], tmp_url->url());
      }
      
      VrmlMFString urls(d_url.getLength(), tmp_url_array);
      if (!d_scene->loadUrl(&urls, &d_parameter)) {
        theSystem->warn("Couldn't load URL %s\n", d_url.get(0));
      }

      for (size_t j = 0; j < d_url.getLength(); j++)
        delete [] tmp_url_array[j];
      delete [] tmp_url_array;
      delete tmp_url;
    }
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeAnchor::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "description" ) == 0 )
    return &d_description;
  else if ( strcmp( fieldName, "parameter" ) == 0 )
    return &d_parameter;
  else if ( strcmp( fieldName, "url" ) == 0 )
    return &d_url;
  
  return VrmlNodeGroup::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.
// Need to delete current values ...

void VrmlNodeAnchor::setField(const char *fieldName,
			      const VrmlField &fieldValue)
{
  if TRY_FIELD(description, SFString)
  else if TRY_FIELD(parameter, MFString)
  else if TRY_FIELD(url, MFString)
  else
    VrmlNodeGroup::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true);
}

const VrmlBVolume*
VrmlNodeAnchor::getBVolume() const
{
  //cout << "VrmlNodeAnchor::getBVolume() {" << endl;
  const VrmlBVolume* bv = VrmlNodeGroup::getBVolume();
  //cout << "}:";
  //bv->dump(cout) << endl;
  return bv;
}
