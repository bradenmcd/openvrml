//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2001  Braden McDaniel
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

# include <errno.h>
# include "vrml97node.h"
# include "VrmlNamespace.h"
# include "VrmlNodeType.h"
# include "VrmlNodeVisitor.h"
# include "VrmlNodeProto.h"
# include "Doc.h"
# include "doc2.hpp"
# include "VrmlScene.h"
# include "Audio.h"
# include "sound.h"
# include "MathUtils.h"

/**
 * @class VrmlNodeChild
 *
 * @brief Base class for all nodes that may be children of a Group node.
 */

/**
 * @brief Define the fields of all built-in child nodes.
 * 
 * @param nodeType
 *
 * @return the type object
 */
VrmlNodeType * VrmlNodeChild::defineType(VrmlNodeType * nodeType) {
    return VrmlNode::defineType(nodeType);
}

/**
 * @brief Constructor.
 *
 * @param scene the VrmlScene to which this node belongs
 */
VrmlNodeChild::VrmlNodeChild(VrmlScene * scene): VrmlNode(scene) {}

/**
 * @brief Copy constructor.
 */
VrmlNodeChild::VrmlNodeChild(const VrmlNodeChild & node): VrmlNode(node) {}

/**
 * @brief Downcast method (const version).
 */
const VrmlNodeChild * VrmlNodeChild::toChild() const {
    return this;
}

/**
 * @brief Downcast method (non-const version).
 */
VrmlNodeChild * VrmlNodeChild::toChild() {
    return this;
}


/**
 * @class VrmlNodeGeometry
 *
 * @brief Base class for all geometry nodes.
 */

/**
 * @brief Define the fields of all built-in geometry nodes.
 * 
 * @param nodeType
 *
 * @return the type object
 */
VrmlNodeType *VrmlNodeGeometry::defineType(VrmlNodeType *t)
{
  return VrmlNode::defineType(t);
}

/**
 * @brief Constructor.
 *
 * @param scene the VrmlScene to which this node belongs
 */
VrmlNodeGeometry::VrmlNodeGeometry(VrmlScene * scene): VrmlNode(scene),
        d_viewerObject(0) {}

/**
 * @brief Destructor.
 */
VrmlNodeGeometry::~VrmlNodeGeometry()
{
  /* Need access to viewer to delete viewerObject...*/
}

/**
 * @brief Downcast method (const version).
 *
 * @return a pointer to this object
 */
const VrmlNodeGeometry * VrmlNodeGeometry::toGeometry() const {
    return this;
}

/**
 * @brief Downcast method (non-const version).
 *
 * @return a pointer to this object
 */
VrmlNodeGeometry * VrmlNodeGeometry::toGeometry() {
    return this;
}

/**
 * @brief Get the Color node associated with this geometry.
 *
 * @return a VrmlNodeColor
 */
VrmlNodeColor *VrmlNodeGeometry::color() { return 0; }

/**
 * @brief Render this node.
 *
 * Subclasses need only define insertGeometry(), not render().
 *
 * @param viewer a renderer
 * @param context the renderer context
 */
void VrmlNodeGeometry::render(Viewer * viewer, VrmlRenderContext context) 
{
  if ( d_viewerObject && isModified() )
    {
      viewer->removeObject(d_viewerObject);
      d_viewerObject = 0;
    }

  if (d_viewerObject)
    viewer->insertReference(d_viewerObject);
  else
    {
      d_viewerObject = insertGeometry(viewer, context);
      clearModified();
    }
}

/**
 * @class VrmlNodeLight
 *
 * @brief Base class for all light nodes.
 */

/**
 * @brief Define the built-in light node fields.
 *
 * @param nodeType the type object to use
 *
 * @return the defined type object
 */
VrmlNodeType * VrmlNodeLight::defineType(VrmlNodeType * nodeType) {
    VrmlNodeChild::defineType(nodeType);
    nodeType->addExposedField("ambientIntensity", VrmlField::SFFLOAT);
    nodeType->addExposedField("color", VrmlField::SFCOLOR);
    nodeType->addExposedField("intensity", VrmlField::SFFLOAT);
    nodeType->addExposedField("on", VrmlField::SFBOOL);

    return nodeType;
}

/**
 * @brief Constructor.
 *
 * @param scene the VrmlScene to which this node belongs.
 */
VrmlNodeLight::VrmlNodeLight(VrmlScene * scene): VrmlNodeChild(scene),
        d_ambientIntensity(0.0), d_color(1.0, 1.0, 1.0), d_intensity(1.0),
        d_on(true) {}

/**
 * @brief Get the type object for this node.
 * 
 * @return the type object
 */
VrmlNodeType & VrmlNodeLight::nodeType() const {
    return *defineType(0);
}

/**
 * @brief Downcast to a light node.
 *
 * @return a pointer to this object.
 */
VrmlNodeLight* VrmlNodeLight::toLight() const
{ return (VrmlNodeLight*) this; }

/**
 * @brief Render this node as scoped.
 *
 * The default version of this method does nothing.
 *
 * @param viewer a renderer.
 */
void VrmlNodeLight::renderScoped(Viewer * viewer) {}

/**
 * @brief Print the fields for this node.
 *
 * @param os a standard output stream
 * @param indent the number of spaces per indent level
 */
ostream& VrmlNodeLight::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_ambientIntensity.get()))
    PRINT_FIELD(ambientIntensity);
  if (! FPEQUAL(d_color.getR(), 1.0) ||
      ! FPEQUAL(d_color.getG(), 1.0) ||
      ! FPEQUAL(d_color.getB(), 1.0) )
    PRINT_FIELD(color);
  if (! FPEQUAL(d_intensity.get(), 1.0))
    PRINT_FIELD(intensity);
  if (! d_on.get() ) PRINT_FIELD(on);

  return os;
}

/**
 * @brief Get the value of a field or eventOut.
 *
 * @param fieldName the name of a field or eventOut
 *
 * @return a pointer to a VrmlField value, or a zero pointer if the node has
 *      no field or eventOut with the given name
 */
const VrmlField *VrmlNodeLight::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "ambientIntensity" ) == 0 )
    return &d_ambientIntensity;
  else if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  else if ( strcmp( fieldName, "intensity" ) == 0 )
    return &d_intensity;
  else if ( strcmp( fieldName, "on" ) == 0 )
    return &d_on;

  return VrmlNode::getField( fieldName );
}

/**
 * @brief Set the value of one of the node fields.
 *
 * @param fieldName the name of the field to set.
 * @param fieldValue the new value
 */
void VrmlNodeLight::setField(const char *fieldName,
			     const VrmlField &fieldValue)
{
  if TRY_FIELD(ambientIntensity, SFFloat)
  else if TRY_FIELD(color, SFColor)
  else if TRY_FIELD(intensity, SFFloat)
  else if TRY_FIELD(on, SFBool)
  else 
    VrmlNode::setField(fieldName, fieldValue);
}


static VrmlNode * createAnchor(VrmlScene * scene) {
    return new VrmlNodeAnchor(scene);
}


// Define the built in VrmlNodeType:: "Anchor" fields

VrmlNodeType *VrmlNodeAnchor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;
  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Anchor", createAnchor);
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
        tmp_url->seturl(d_url.getElement(i), &relDoc);
        tmp_url_array[i] = new char[ strlen(tmp_url->url()) + 1 ];
        strcpy(tmp_url_array[i], tmp_url->url());
      }
      
      VrmlMFString urls(d_url.getLength(), tmp_url_array);
      if (!d_scene->loadUrl(&urls, &d_parameter)) {
        theSystem->warn("Couldn't load URL %s\n", d_url.getElement(0));
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


static VrmlNode * createAppearance(VrmlScene * scene) {
    return new VrmlNodeAppearance(scene);
}


// Define the built in VrmlNodeType:: "Appearance" fields

VrmlNodeType *VrmlNodeAppearance::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("Appearance", createAppearance);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("material", VrmlField::SFNODE);
  t->addExposedField("texture", VrmlField::SFNODE);
  t->addExposedField("textureTransform", VrmlField::SFNODE);

  return t;
}

VrmlNodeType & VrmlNodeAppearance::nodeType() const
{
    return *defineType(0);
}


VrmlNodeAppearance::VrmlNodeAppearance(VrmlScene *scene) :
  VrmlNode(scene)
{
}

VrmlNodeAppearance::~VrmlNodeAppearance()
{
}

bool VrmlNodeAppearance::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeAppearance::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_material.get()) {
            this->d_material.get()->resetVisitedFlag();
        }
        if (this->d_texture.get()) {
            this->d_texture.get()->resetVisitedFlag();
        }
        if (this->d_textureTransform.get()) {
            this->d_textureTransform.get()->resetVisitedFlag();
        }
    }
}

VrmlNodeAppearance* VrmlNodeAppearance::toAppearance() const
{
  return (VrmlNodeAppearance*) this;
}


bool VrmlNodeAppearance::isModified() const
{
  return ( d_modified ||
	   (d_material.get() && d_material.get()->isModified()) ||
	   (d_texture.get() && d_texture.get()->isModified()) ||
	   (d_textureTransform.get() &&
	    d_textureTransform.get()->isModified()) );
}

void VrmlNodeAppearance::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_material.get()) d_material.get()->updateModified(path);
  if (d_texture.get()) d_texture.get()->updateModified(path);
  if (d_textureTransform.get()) d_textureTransform.get()->updateModified(path);
  path.pop_front();
}

void VrmlNodeAppearance::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_material.get()) d_material.get()->clearFlags();
  if (d_texture.get()) d_texture.get()->clearFlags();
  if (d_textureTransform.get()) d_textureTransform.get()->clearFlags();
}

void VrmlNodeAppearance::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  if (d_material.get()) d_material.get()->addToScene(s, rel);
  if (d_texture.get()) d_texture.get()->addToScene(s, rel);
  if (d_textureTransform.get())
    d_textureTransform.get()->addToScene(s,rel);
}

ostream& VrmlNodeAppearance::printFields(ostream& os, int indent)
{
  if (d_material.get()) PRINT_FIELD(material);
  if (d_texture.get()) PRINT_FIELD(texture);
  if (d_textureTransform.get()) PRINT_FIELD(textureTransform);

  return os;
}


void VrmlNodeAppearance::render(Viewer *viewer, VrmlRenderContext rc)
{
  VrmlNodeMaterial *m = d_material.get() ? d_material.get()->toMaterial() : 0;
  VrmlNodeTexture *t = d_texture.get() ? d_texture.get()->toTexture() : 0;

  if (m)
    {
      float trans = m->transparency();
      const float *diff = m->diffuseColor();
      float diffuse[3] = { diff[0], diff[1], diff[2] };
      int nTexComponents = t ? t->nComponents() : 0;
      if (nTexComponents == 2 || nTexComponents == 4)
	trans = 0.0;
      if (nTexComponents >= 3)
	diffuse[0] = diffuse[1] = diffuse[2] = 1.0;

      viewer->enableLighting(true);   // turn lighting on for this object
      viewer->setMaterial(m->ambientIntensity(),
			  diffuse,
			  m->emissiveColor(),
			  m->shininess(),
			  m->specularColor(),
			  trans);

      m->clearModified();
    }
  else
    {
      viewer->setColor(1.0, 1.0, 1.0); // default color
      viewer->enableLighting(false);   // turn lighting off for this object
    }

  if (t)
    {
      if (d_textureTransform.get())
	d_textureTransform.get()->render(viewer, rc);
      else
	viewer->setTextureTransform( 0, 0, 0, 0 );

      t->render(viewer, rc);
    }

  clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeAppearance::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "material" ) == 0 )
    return &d_material;
  else if ( strcmp( fieldName, "texture" ) == 0 )
    return &d_texture;
  else if ( strcmp( fieldName, "textureTransform" ) == 0 )
    return &d_textureTransform;
  
  return VrmlNode::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeAppearance::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_SFNODE_FIELD(material, Material)
  else if TRY_SFNODE_FIELD(texture, Texture)
  else if TRY_SFNODE_FIELD(textureTransform, TextureTransform)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

/**
 * @brief Get the Material node.
 *
 * @returns an SFNode object containing the Material node associated with
 *          this Appearance.
 */
const VrmlSFNode & VrmlNodeAppearance::getMaterial() const {
    return this->d_material;
}

/**
 * @brief Set the Material node.
 *
 * @param material an SFNode object containing a Material node
 */
void VrmlNodeAppearance::setMaterial(const VrmlSFNode & material) {
    assert(!material.get() || material.get()->toMaterial());
    this->d_material = material;
}

/**
 * @brief Get the texture node.
 *
 * @return an SFNode object containing the texture node associated with
 *         this Appearance.
 */
const VrmlSFNode & VrmlNodeAppearance::getTexture() const {
    return this->d_texture;
}

/**
 * @brief Set the texture node.
 *
 * @param texture an SFNode object containing a texture node.
 */
void VrmlNodeAppearance::setTexture(const VrmlSFNode & texture) {
    assert(!texture.get() || texture.get()->toTexture());
    this->d_texture = texture;
}

/**
 * @brief Get the TextureTransform node.
 *
 * @return an SFNode object containing the TextureTransform node
 *         associated with this Appearance.
 */
const VrmlSFNode & VrmlNodeAppearance::getTextureTransform() const {
    return this->d_textureTransform;
}

/**
 * @brief Set the TextureTransform node.
 *
 * @param textureTransform an SFNode object containing a
 *                         TextureTransform node.
 */
void VrmlNodeAppearance::setTextureTransform(const VrmlSFNode & textureTransform) {
    assert(!textureTransform.get()
            || textureTransform.get()->toTextureTransform());
    this->d_textureTransform = textureTransform;
}

static VrmlNode * createAudioClip(VrmlScene * scene) {
    return new VrmlNodeAudioClip(scene);
}


// Define the built in VrmlNodeType:: "AudioClip" fields

VrmlNodeType *VrmlNodeAudioClip::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("AudioClip", createAudioClip);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("description", VrmlField::SFSTRING);
  t->addExposedField("loop", VrmlField::SFBOOL);
  t->addExposedField("pitch", VrmlField::SFFLOAT);
  t->addExposedField("startTime", VrmlField::SFTIME);
  t->addExposedField("stopTime", VrmlField::SFTIME);
  t->addExposedField("url", VrmlField::MFSTRING);

  t->addEventOut("duration_changed", VrmlField::SFTIME);
  t->addEventOut("isActive", VrmlField::SFBOOL);

  return t;
}


VrmlNodeType & VrmlNodeAudioClip::nodeType() const
{
    return *defineType(0);
}


VrmlNodeAudioClip::VrmlNodeAudioClip(VrmlScene *scene) :
  VrmlNode(scene),
  d_pitch(1.0),
  d_isActive(false),
  d_audio(0),
  d_url_modified(false),
  _audio_index(0),
  _audio_intensity(1.0),
  _audio_fd(-1)
{
    if (d_scene) d_scene->addAudioClip(this);
}


// Define copy constructor so clones don't share d_audio (for now, anyway...)

VrmlNodeAudioClip::VrmlNodeAudioClip(const VrmlNodeAudioClip &n) :
  VrmlNode(n),
  d_description(n.d_description),
  d_loop(n.d_loop),
  d_pitch(n.d_pitch),
  d_startTime(n.d_startTime),
  d_stopTime(n.d_stopTime),
  d_url(n.d_url),
  d_duration(n.d_duration),
  d_isActive(false),
  d_audio(0),			// can these be shared?...
  d_url_modified(false),
  _audio_index(0),
  _audio_intensity(1.0),
  _audio_fd(-1)
{
    if (d_scene) d_scene->addAudioClip(this);
}


VrmlNodeAudioClip::~VrmlNodeAudioClip()
{
  delete d_audio;
  if (d_scene) d_scene->removeAudioClip(this);
}

bool VrmlNodeAudioClip::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeAudioClip::addToScene(VrmlScene *s, const char *rel)
{
  if (d_scene != s && (d_scene = s) != 0) d_scene->addAudioClip(this);
  d_relativeUrl.set(rel);
}

VrmlNodeAudioClip* VrmlNodeAudioClip::toAudioClip() const
{ return (VrmlNodeAudioClip*)this; }


ostream& VrmlNodeAudioClip::printFields(ostream& os, int indent)
{
  if (d_description.get()) PRINT_FIELD(description);
  if (d_loop.get()) PRINT_FIELD(loop);
  if (! FPEQUAL(d_pitch.get(),1.0)) PRINT_FIELD(pitch);
  if (! FPZERO(d_startTime.get())) PRINT_FIELD(startTime);
  if (! FPZERO(d_stopTime.get())) PRINT_FIELD(stopTime);
  if (d_url.getLength() > 0) PRINT_FIELD(url);
  return os;
}


void VrmlNodeAudioClip::update (VrmlSFTime &inTime)
{
    // If the URL has been modified, update the audio object
    if (d_url_modified)
    {
        Doc relDoc(d_relativeUrl.get(), static_cast<Doc const *>(0));
        delete d_audio;
        d_audio = new Audio(0);
        if (d_audio->tryURLs (d_url.getLength(),
                              d_url.get(),
			      &relDoc))
        {
            d_duration.set (d_audio->duration());
            eventOut (inTime.get(), "duration_changed", d_duration);
        }
        else
        {
#if HAVE_SOUND
            cerr << "Error: couldn't read AudioClip from URL "
                 << d_url << endl;
#endif
            delete d_audio;
            d_audio = 0;
        }

        d_url_modified = false;
    }

    // If there's no audio or START <= 0, we don't play anything
    if (d_audio == 0 || d_startTime.get() <= 0)
        return;

    // Determine if this clip should be playing right now
    bool audible = false;

    // If START < STOP  and  START <= NOW < STOP
    if (d_stopTime.get() > d_startTime.get())
        audible = (d_startTime.get() <= inTime.get() &&
                   inTime.get() < d_stopTime.get());

    // If STOP < START  and  START <= NOW
    else
        audible = (inTime.get() >= d_startTime.get());

    // If the clip is not looping, it's not audible after
    // its duration has expired.
    if (d_loop.get() == false)
        if (inTime.get()-d_startTime.get() > d_audio->duration())
            audible = false;

    // If the clip is audible, play it.  Otherwise, stop it.
    if (audible)
    {
        // If the sound device is not already open, open it.
        if (_audio_fd < 0)
        {
            _audio_fd = openSound (d_audio->channels(),
                                   d_audio->bitsPerSample(),
                                   d_audio->samplesPerSec());

            // If we can't get a sound device, silently return
            if (_audio_fd < 0)
                    return;

            _audio_index = d_audio->getByteIndex
                               (inTime.get() - d_startTime.get());

            d_isActive.set (true);
            eventOut (inTime.get(), "isActive", d_isActive);
        }

        // Send out a sound buffer
        _audio_index = outputSoundChunk (d_audio->numBytes(),
                                         d_audio->samples(),
                                         d_audio->bitsPerSample(),
                                         _audio_index, d_loop.get(),
                                         _audio_intensity,
                                         _audio_fd);
    }

    // Otherwise, close the sound device
    else
    {
        if (_audio_fd >= 0)
        {
            _audio_fd = closeSound (_audio_fd);
            d_isActive.set (false);
            eventOut (inTime.get(), "isActive", d_isActive);
        }
    }
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeAudioClip::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "description" ) == 0 )
    return &d_description;
  else if ( strcmp( fieldName, "loop" ) == 0 )
    return &d_loop;
  else if ( strcmp( fieldName, "pitch" ) == 0 )
    return &d_pitch;
  else if ( strcmp( fieldName, "startTime" ) == 0 )
    return &d_startTime;
  else if ( strcmp( fieldName, "stopTime" ) == 0 )
    return &d_stopTime;
  else if ( strcmp( fieldName, "url" ) == 0 )
    return &d_url;

  // eventOuts
  else if ( strcmp( fieldName, "duration" ) == 0 )
    return &d_duration;
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  
  return VrmlNode::getField(fieldName); // Parent class
}


// Set the value of one of the node fields.

void VrmlNodeAudioClip::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(description, SFString)
  else if TRY_FIELD(loop, SFBool)
  else if TRY_FIELD(pitch, SFFloat)
  else if TRY_FIELD(startTime, SFTime)
  else if TRY_FIELD(stopTime, SFTime)
  else if TRY_FIELD(url, MFString)
  else
    VrmlNode::setField(fieldName, fieldValue);

  if (strcmp ("url", fieldName) == 0)
  {
      d_url_modified = true;
      setModified();
  }
}


//  Background factory.

static VrmlNode * createBackground(VrmlScene *scene) {
    return new VrmlNodeBackground(scene);
}

// Define the built in VrmlNodeType:: "Background" fields

VrmlNodeType *VrmlNodeBackground::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;	// Define the type once.
      t = st = new VrmlNodeType("Background", createBackground);
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
	      if (strcmp(currentTex, urls.getElement(i)) == 0 ||
		  strcmp(currentTex+relPathLen, urls.getElement(i)) == 0 )
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
	  int sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
	  const int nSizes = sizeof(sizes) / sizeof(int);
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


static VrmlNode * createBillboard(VrmlScene * scene) {
    return new VrmlNodeBillboard(scene);
}


// Define the built in VrmlNodeType:: "Billboard" fields

VrmlNodeType *VrmlNodeBillboard::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Billboard", createBillboard);
      t->reference();
    }

  VrmlNodeGroup::defineType(t);	// Parent class
  t->addExposedField("axisOfRotation", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeBillboard::nodeType() const
{
    return *defineType(0);
}


VrmlNodeBillboard::VrmlNodeBillboard(VrmlScene *scene) :
  VrmlNodeGroup(scene),
  d_axisOfRotation(0.0, 1.0, 0.0),
  d_xformObject(0)
{
}

VrmlNodeBillboard::~VrmlNodeBillboard()
{
  // delete d_xformObject...
}

bool VrmlNodeBillboard::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeBillboard::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_axisOfRotation.getX()) ||
      ! FPZERO(d_axisOfRotation.getY()) ||
      ! FPZERO(d_axisOfRotation.getZ()) )
    PRINT_FIELD(axisOfRotation);

  VrmlNodeGroup::printFields(os, indent);
  return os;
}


void VrmlNodeBillboard::render(Viewer *viewer, VrmlRenderContext rc)
{
  if ( d_xformObject && isModified() )
    {
      viewer->removeObject(d_xformObject);
      d_xformObject = 0;
    }

  if (d_xformObject)
    viewer->insertReference(d_xformObject);

  else if (d_children.getLength() > 0)
    {
      d_xformObject = viewer->beginObject(name());

      viewer->setBillboardTransform( d_axisOfRotation.get() );

      // Render children
      VrmlNodeGroup::render(viewer, rc);

      viewer->unsetBillboardTransform( d_axisOfRotation.get() );

      viewer->endObject();
    }

  clearModified();
}

// Cache a pointer to (one of the) parent transforms for proper
// rendering of bindables.

void VrmlNodeBillboard::accumulateTransform(VrmlNode * parent) {
    d_parentTransform = parent;
    for (size_t i = 0; i < this->d_children.getLength(); ++i) {
        if (this->d_children.getElement(i)) {
            this->d_children.getElement(i)->accumulateTransform(this);
        }
    }
}

VrmlNode* VrmlNodeBillboard::getParentTransform() { return d_parentTransform; }

void VrmlNodeBillboard::inverseTransform(Viewer *viewer)
{
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(viewer);

  // Apply inverted bb transforms...
  //viewer->setBillboardTransform( d_axisOfRotation.get() );
}

void VrmlNodeBillboard::inverseTransform(double m[4][4])
{
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(m);
  else
    Midentity(m);

  // Invert bb transform...
  // ...
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeBillboard::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "axisOfRotation" ) == 0 )
    return &d_axisOfRotation;

  return VrmlNodeGroup::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeBillboard::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(axisOfRotation, SFVec3f)
  else
    VrmlNodeGroup::setField(fieldName, fieldValue);
}

const VrmlNodeBillboard * VrmlNodeBillboard::toBillboard() const {
    return this;
}

VrmlNodeBillboard * VrmlNodeBillboard::toBillboard() {
    return this;
}
// Make a VrmlNodeBox

static VrmlNode * createBox(VrmlScene * scene) {
    return new VrmlNodeBox(scene);
}


// Define the built in VrmlNodeType:: "Box" fields

VrmlNodeType *VrmlNodeBox::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;	// Define type only once.
      t = st = new VrmlNodeType("Box", createBox);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class
  t->addField("size", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeBox::nodeType() const
{
    return *defineType(0);
}


VrmlNodeBox::VrmlNodeBox(VrmlScene *scene) :
  VrmlNodeGeometry(scene),
  d_size(2.0, 2.0, 2.0)
{
  setBVolumeDirty(true); // lazy calc of bvolume
}

VrmlNodeBox::~VrmlNodeBox()
{
}

bool VrmlNodeBox::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeBox::printFields(ostream& os, int )
{
  if ( ! FPEQUAL(d_size.getX(), 2.0) ||
       ! FPEQUAL(d_size.getY(), 2.0) ||
       ! FPEQUAL(d_size.getZ(), 2.0) )
    os << " size " << d_size;
  return os;
}

Viewer::Object VrmlNodeBox::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  return viewer->insertBox(d_size.getX(), d_size.getY(), d_size.getZ());
}

// Set the value of one of the node fields.

void VrmlNodeBox::setField(const char *fieldName,
			   const VrmlField &fieldValue)
{
  if TRY_FIELD(size, SFVec3f)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
  setBVolumeDirty(true);
}

VrmlNodeBox* VrmlNodeBox::toBox() const //LarryD Mar 08/99
{ return (VrmlNodeBox*) this; }

const VrmlSFVec3f& VrmlNodeBox::getSize() const   // LarryD Mar 08/99
{  return d_size; }

const VrmlBVolume*
VrmlNodeBox::getBVolume() const
{
  //cout << "VrmlNodeBox::getBVolume()" << endl;
  //static VrmlBSphere* box_sphere = (VrmlBSphere*)0;
  //if (!box_sphere) 
  //box_sphere = new VrmlBSphere();
  if (this->isBVolumeDirty()) {
    float corner[3] = { d_size.getX()/2.0f, d_size.getY()/2.0f, d_size.getZ()/2.0f };
    float r = Vlength(corner);
    ((VrmlNodeBox*)this)->d_bsphere.setRadius(r);
    ((VrmlNodeBox*)this)->setBVolumeDirty(false); // logical const
  }
  //cout << "VrmlNodeBox::getBVolume():";
  //box_sphere->dump(cout) << endl;
  return &d_bsphere;
}

static VrmlNode * createCollision(VrmlScene * scene) {
    return new VrmlNodeCollision(scene);
}


// Define the built in VrmlNodeType:: "Collision" fields

VrmlNodeType *VrmlNodeCollision::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Collision", createCollision);
      t->reference();
    }

  VrmlNodeGroup::defineType(t);	// Parent class
  t->addExposedField("collide", VrmlField::SFBOOL);
  t->addField("proxy", VrmlField::SFNODE);
  t->addEventOut("collideTime", VrmlField::SFTIME);

  return t;
}

VrmlNodeType & VrmlNodeCollision::nodeType() const
{
    return *defineType(0);
}


VrmlNodeCollision::VrmlNodeCollision(VrmlScene *scene) :
  VrmlNodeGroup(scene),
  d_collide(true)
{
}

VrmlNodeCollision::~VrmlNodeCollision()
{
}

bool VrmlNodeCollision::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeCollision::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        for (size_t i = 0; i < this->d_children.getLength(); ++i) {
            if (this->d_children.getElement(i)) {
                this->d_children.getElement(i)->resetVisitedFlag();
            }
        }
        if (this->d_proxy.get()) {
            this->d_proxy.get()->resetVisitedFlag();
        }
    }
}

bool VrmlNodeCollision::isModified() const
{
  return ( (d_proxy.get() && d_proxy.get()->isModified()) ||
	   VrmlNodeGroup::isModified() );
}

void VrmlNodeCollision::clearFlags()
{
  VrmlNodeGroup::clearFlags();
  if (d_proxy.get()) d_proxy.get()->clearFlags();
}


void VrmlNodeCollision::addToScene( VrmlScene *s, const char *rel )
{
  VrmlNodeGroup::addToScene(s, rel);
  if (d_proxy.get()) d_proxy.get()->addToScene(s, rel);
}

ostream& VrmlNodeCollision::printFields(ostream& os, int indent)
{
  if (! d_collide.get()) PRINT_FIELD(collide);
  if (d_proxy.get()) PRINT_FIELD(proxy);

  VrmlNodeGroup::printFields(os, indent);
  return os;
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeCollision::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "collide" ) == 0 )
    return &d_collide;

  // eventOuts
  else if ( strcmp( fieldName, "collideTime" ) == 0 )
    return &d_collideTime;

  return VrmlNodeGroup::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeCollision::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(collide, SFBool)
  else if TRY_SFNODE_FIELD(proxy, Child)
  else
    VrmlNodeGroup::setField(fieldName, fieldValue);
}

/**
 * @brief Get the proxy geometry for this Collision node.
 *
 * @return proxy
 */
const VrmlSFNode & VrmlNodeCollision::getProxy() const {
    return this->d_proxy;
}

/**
 * @brief Set the proxy geometry for this Collision node.
 *
 * @param proxy
 */
void VrmlNodeCollision::setProxy(const VrmlSFNode & proxy) {
    this->d_proxy = proxy;
}

const VrmlNodeCollision * VrmlNodeCollision::toCollision() const {
    return this;
}

VrmlNodeCollision * VrmlNodeCollision::toCollision() {
    return this;
}

static VrmlNode *createColor(VrmlScene * scene) {
    return new VrmlNodeColor(scene);
}


// Define the built in VrmlNodeType:: "Color" fields

VrmlNodeType *VrmlNodeColor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;	// Only define type once.
      t = st = new VrmlNodeType("Color", createColor);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("color", VrmlField::MFCOLOR);

  return t;
}

VrmlNodeType & VrmlNodeColor::nodeType() const
{
    return *defineType(0);
}


VrmlNodeColor::VrmlNodeColor(VrmlScene *scene) : VrmlNode(scene)
{
}

VrmlNodeColor::~VrmlNodeColor()
{
}

bool VrmlNodeColor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeColor* VrmlNodeColor::toColor() const
{ return (VrmlNodeColor*) this; }


ostream& VrmlNodeColor::printFields(ostream& os, int indent)
{
  if (d_color.getLength() > 0) PRINT_FIELD(color);

  return os;
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeColor::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  return VrmlNode::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeColor::setField(const char *fieldName,
			     const VrmlField &fieldValue)
{
  if TRY_FIELD(color, MFColor)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

// ColorInterpolator factory.

static VrmlNode * createColorInt(VrmlScene * scene) {
    return new VrmlNodeColorInt(scene);
}


// Define the built in VrmlNodeType:: "ColorInterpolator" fields

VrmlNodeType *VrmlNodeColorInt::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("ColorInterpolator", createColorInt);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_fraction", VrmlField::SFFLOAT);
  t->addExposedField("key", VrmlField::MFFLOAT);
  t->addExposedField("keyValue", VrmlField::MFCOLOR);
  t->addEventOut("value_changed", VrmlField::SFCOLOR);

  return t;
}

VrmlNodeType & VrmlNodeColorInt::nodeType() const
{
    return *defineType(0);
}


VrmlNodeColorInt::VrmlNodeColorInt( VrmlScene *scene ) :
  VrmlNodeChild(scene)
{
}

VrmlNodeColorInt::~VrmlNodeColorInt()
{
}

bool VrmlNodeColorInt::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeColorInt::printFields(ostream& os, int indent)
{
  if (d_key.getLength() > 0) PRINT_FIELD(key);
  if (d_keyValue.getLength() > 0) PRINT_FIELD(keyValue);

  return os;
}


void VrmlNodeColorInt::eventIn(double timeStamp,
			       const char *eventName,
			       const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_fraction") == 0)
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
	{
	  theSystem->error
	    ("Invalid type for %s eventIn %s (expected SFFloat).\n",
		nodeType().getName(), eventName);
	  return;
	}
      float f = static_cast<const VrmlSFFloat &>(fieldValue).get();

      int n = d_key.getLength() - 1;
      if (f < d_key.getElement(0))
	d_value.set(d_keyValue.getElement(0));
      else if (f > d_key.getElement(n))
	d_value.set(d_keyValue.getElement(n));
      else
	{
	  //  convert to HSV for the interpolation...
	  for (int i=0; i<n; ++i)
	    if (d_key.getElement(i) <= f && f <= d_key.getElement(i + 1))
	      {
		const float *rgb1 = d_keyValue.getElement(i);
		const float *rgb2 = d_keyValue.getElement(i + 1);

		f = (f - d_key.getElement(i)) / (d_key.getElement(i + 1) - d_key.getElement(i));
		float hsv1[3], hsv2[3];
		VrmlSFColor::RGBtoHSV(rgb1, hsv1);
		VrmlSFColor::RGBtoHSV(rgb2, hsv2);

		// Interpolate angles via the shortest direction
		if (fabs(hsv2[0] - hsv1[0]) > 180.0)
		  {
		    if (hsv2[0] > hsv1[0]) hsv1[0] += 360.0;
		    else         hsv2[0] += 360.0;
		  }
                float hsv[3] = { hsv1[0] + f * (hsv2[0] - hsv1[0]),
                                 hsv1[1] + f * (hsv2[1] - hsv1[1]),
                                 hsv1[2] + f * (hsv2[2] - hsv1[2]) };
		if (hsv[0] >= 360.0) hsv[0] -= 360.0;
		else if (hsv[0] < 0.0) hsv[0] += 360.0;
		float rgb[3];
		VrmlSFColor::HSVtoRGB(hsv, rgb);
		d_value.set(rgb);
		break;
	      }
	}

      // Send the new value
      eventOut(timeStamp, "value_changed", d_value);
    }

  // Check exposedFields
  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);

      // This node is not renderable, so don't re-render on changes to it.
      clearModified();
    }
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeColorInt::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "key" ) == 0 )
    return &d_key;
  else if ( strcmp( fieldName, "keyValue" ) == 0 )
    return &d_keyValue;

  // eventOuts
  else if ( strcmp( fieldName, "value" ) == 0 )
    return &d_value;

  return VrmlNodeChild::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeColorInt::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFColor)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

//  Return a new VrmlNodeCone
static VrmlNode * createCone(VrmlScene * scene) {
    return new VrmlNodeCone(scene);
}


// Define the built in VrmlNodeType:: "Cone" fields

VrmlNodeType *VrmlNodeCone::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;	// Only define type once.
      t = st = new VrmlNodeType("Cone", createCone);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class
  t->addField("bottom", VrmlField::SFBOOL);
  t->addField("bottomRadius", VrmlField::SFFLOAT);
  t->addField("height", VrmlField::SFFLOAT);
  t->addField("side", VrmlField::SFBOOL);

  return t;
}

VrmlNodeType & VrmlNodeCone::nodeType() const
{
    return *defineType(0);
}


VrmlNodeCone::VrmlNodeCone(VrmlScene *scene) :
  VrmlNodeGeometry(scene),
  d_bottom(true),
  d_bottomRadius(1.0),
  d_height(2.0),
  d_side(true)
{
}

VrmlNodeCone::~VrmlNodeCone()
{
}

bool VrmlNodeCone::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeCone::printFields(ostream& os, int indent)
{
  if (! d_bottom.get()) PRINT_FIELD(bottom);
  if (d_bottomRadius.get() != 1.0) PRINT_FIELD(bottomRadius);
  if (d_height.get() != 2.0) PRINT_FIELD(height);
  if (! d_side.get()) PRINT_FIELD(side);

  return os;
}

Viewer::Object VrmlNodeCone::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  return viewer->insertCone(d_height.get(),
			    d_bottomRadius.get(),
			    d_bottom.get(),
			    d_side.get());
}

// Set the value of one of the node fields.

void VrmlNodeCone::setField(const char *fieldName,
			    const VrmlField &fieldValue)
{
  if TRY_FIELD(bottom, SFBool)
  else if TRY_FIELD(bottomRadius, SFFloat)
  else if TRY_FIELD(height, SFFloat)
  else if TRY_FIELD(side, SFBool)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
}

VrmlNodeCone* VrmlNodeCone::toCone() const //LarryD Mar 08/99
{ return (VrmlNodeCone*) this; }

static VrmlNode * createCoordinate(VrmlScene * scene) {
    return new VrmlNodeCoordinate(scene);
}


// Define the built in VrmlNodeType:: "Coordinate" fields

VrmlNodeType *VrmlNodeCoordinate::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Coordinate", createCoordinate);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("point", VrmlField::MFVEC3F);

  return t;
}


VrmlNodeType & VrmlNodeCoordinate::nodeType() const
{
    return *defineType(0);
}


VrmlNodeCoordinate::VrmlNodeCoordinate(VrmlScene *scene) : VrmlNode(scene)
{
}

VrmlNodeCoordinate::~VrmlNodeCoordinate()
{
}

bool VrmlNodeCoordinate::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeCoordinate* VrmlNodeCoordinate::toCoordinate() const
{ return (VrmlNodeCoordinate*)this; }


ostream& VrmlNodeCoordinate::printFields(ostream& os, int indent)
{
  if (d_point.getLength() > 0) PRINT_FIELD(point);

  return os;
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeCoordinate::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "point" ) == 0 )
    return &d_point;

  return VrmlNode::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeCoordinate::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_FIELD(point, MFVec3f)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

// CoordinateInt factory.

static VrmlNode * createCoordinateInt(VrmlScene * scene) {
    return new VrmlNodeCoordinateInt(scene);
}


// Define the built in VrmlNodeType:: "CoordinateInterpolator" fields

VrmlNodeType *VrmlNodeCoordinateInt::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("CoordinateInterpolator", createCoordinateInt);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_fraction", VrmlField::SFFLOAT);
  t->addExposedField("key", VrmlField::MFFLOAT);
  t->addExposedField("keyValue", VrmlField::MFVEC3F);
  t->addEventOut("value_changed", VrmlField::MFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeCoordinateInt::nodeType() const
{
    return *defineType(0);
}


VrmlNodeCoordinateInt::VrmlNodeCoordinateInt( VrmlScene *scene ) :
  VrmlNodeChild(scene)
{
}

VrmlNodeCoordinateInt::~VrmlNodeCoordinateInt()
{
}

bool VrmlNodeCoordinateInt::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeCoordinateInt::printFields(ostream& os, int indent)
{
  if (d_key.getLength() > 0) PRINT_FIELD(key);
  if (d_keyValue.getLength() > 0) PRINT_FIELD(keyValue);

  return os;
}

void VrmlNodeCoordinateInt::eventIn(double timeStamp,
				    const char *eventName,
				    const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_fraction") == 0)
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
	{
	  theSystem->error
	    ("Invalid type for %s eventIn %s (expected SFFloat).\n",
	     nodeType().getName(), eventName);
	  return;
	}
      float f = static_cast<const VrmlSFFloat &>(fieldValue).get();

      size_t nCoords = d_keyValue.getLength() / d_key.getLength();
      size_t n = d_key.getLength() - 1;

      if (f < d_key.getElement(0))
	{
	  d_value.set(nCoords, d_keyValue.getElement(0));
	}
      else if (f > d_key.getElement(n))
	{
	  d_value.set(nCoords, d_keyValue.getElement(n * nCoords));
	}
      else
	{
	  // Reserve enough space for the new value
	  d_value.set( nCoords, 0 );

	  for (int i=0; i<n; ++i)
	    if (d_key.getElement(i) <= f && f <= d_key.getElement(i + 1))
	      {
		const float * v1 = d_keyValue.getElement(i * nCoords);
		const float * v2 = d_keyValue.getElement((i + 1) * nCoords);

		f = (f - d_key.getElement(i)) / (d_key.getElement(i + 1) - d_key.getElement(i));

		for (size_t j = 0; j < nCoords; ++j)
		  {
		    const float vec[3] = { v1[0] + f * (v2[0] - v1[0]),
                                           v1[1] + f * (v2[1] - v1[1]),
                                           v1[2] + f * (v2[2] - v1[2]) };
                    this->d_value.setElement(j, vec);
		    v1 += 3;
		    v2 += 3;
		  }

		break;
	      }
	}

      // Send the new value
      eventOut(timeStamp, "value_changed", d_value);
    }

  // Check exposedFields
  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);

      // This node is not renderable, so don't re-render on changes to it.
      clearModified();
    }
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeCoordinateInt::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "key" ) == 0 )
    return &d_key;
  else if ( strcmp( fieldName, "keyValue" ) == 0 )
    return &d_keyValue;

  // eventOuts
  else if ( strcmp( fieldName, "value" ) == 0 )
    return &d_value;

  return VrmlNodeChild::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeCoordinateInt::setField(const char *fieldName,
				     const VrmlField &fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFVec3f)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

//  Return a new VrmlNodeCylinder
static VrmlNode *createCylinder(VrmlScene * s) {
    return new VrmlNodeCylinder(s);
}


// Define the built in VrmlNodeType:: "Cylinder" fields

VrmlNodeType * VrmlNodeCylinder::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Cylinder", createCylinder);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class
  t->addField("bottom", VrmlField::SFBOOL);
  t->addField("height", VrmlField::SFFLOAT);
  t->addField("radius", VrmlField::SFFLOAT);
  t->addField("side", VrmlField::SFBOOL);
  t->addField("top", VrmlField::SFBOOL);

  return t;
}


VrmlNodeType & VrmlNodeCylinder::nodeType() const
{
    return *defineType(0);
}


VrmlNodeCylinder::VrmlNodeCylinder(VrmlScene *scene) :
  VrmlNodeGeometry(scene),
  d_bottom(true),
  d_height(2.0),
  d_radius(1.0),
  d_side(true),
  d_top(true)
{
}

VrmlNodeCylinder::~VrmlNodeCylinder()
{
  // need access to viewer to remove d_viewerObject...
}

bool VrmlNodeCylinder::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeCylinder::printFields(ostream& os, int indent)
{
  if (! d_bottom.get()) PRINT_FIELD(bottom);
  if (! d_side.get()) PRINT_FIELD(side);
  if (! d_top.get()) PRINT_FIELD(top);
  if (d_height.get() != 2.0) PRINT_FIELD(height);
  if (d_radius.get() != 1.0) PRINT_FIELD(radius);

  return os;
}


Viewer::Object VrmlNodeCylinder::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  return viewer->insertCylinder(d_height.get(),
				d_radius.get(),
				d_bottom.get(),
				d_side.get(),
				d_top.get());
}

// Set the value of one of the node fields.

void VrmlNodeCylinder::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_FIELD(bottom, SFBool)
  else if TRY_FIELD(height, SFFloat)
  else if TRY_FIELD(radius, SFFloat)
  else if TRY_FIELD(side, SFBool)
  else if TRY_FIELD(top, SFBool)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
}

VrmlNodeCylinder* VrmlNodeCylinder::toCylinder() const //LarryD Mar 08/99
{ return (VrmlNodeCylinder*) this; }

// CylinderSensor factory. 

static VrmlNode *createCylinderSensor(VrmlScene * scene) {
    return new VrmlNodeCylinderSensor(scene);
}


// Define the built in VrmlNodeType:: "CylinderSensor" fields

VrmlNodeType *VrmlNodeCylinderSensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("CylinderSensor", createCylinderSensor);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("autoOffset", VrmlField::SFBOOL);
  t->addExposedField("diskAngle", VrmlField::SFFLOAT);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("maxAngle", VrmlField::SFFLOAT);
  t->addExposedField("minAngle", VrmlField::SFFLOAT);
  t->addExposedField("offset", VrmlField::SFFLOAT);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("rotation_changed", VrmlField::SFROTATION);
  t->addEventOut("trackPoint_changed", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeCylinderSensor::nodeType() const 
{
    return *defineType(0);
}


VrmlNodeCylinderSensor::VrmlNodeCylinderSensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_autoOffset(true),
  d_diskAngle(0.262),
  d_enabled(true),
  d_maxAngle(-1.0),
  d_minAngle(0.0),
  d_offset(0.0),
  d_isActive(false)
{
  setModified();
}


VrmlNodeCylinderSensor::~VrmlNodeCylinderSensor()
{
}

bool VrmlNodeCylinderSensor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeCylinderSensor* VrmlNodeCylinderSensor::toCylinderSensor() const	// mgiger 6/16/00
{
	return (VrmlNodeCylinderSensor*) this;
}

ostream& VrmlNodeCylinderSensor::printFields(ostream& os, int indent)
{
  if (! d_autoOffset.get()) PRINT_FIELD(autoOffset);
  if (! FPEQUAL(d_diskAngle.get(), 0.262)) PRINT_FIELD(diskAngle);
  if (! d_enabled.get())    PRINT_FIELD(enabled);
  if (! FPEQUAL(d_maxAngle.get(), -1.0)) PRINT_FIELD(maxAngle);
  if (! FPZERO(d_minAngle.get())) PRINT_FIELD(minAngle);
  if (! FPZERO(d_offset.get())) PRINT_FIELD(offset);

  return os;
}

const VrmlField *VrmlNodeCylinderSensor::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "autoOffset" ) == 0 )
    return &d_autoOffset;
  else if ( strcmp( fieldName, "diskAngle" ) == 0 )
    return &d_diskAngle;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "maxAngle" ) == 0 )
    return &d_maxAngle;
  else if ( strcmp( fieldName, "minAngle" ) == 0 )
    return &d_minAngle;
  else if ( strcmp( fieldName, "offset" ) == 0 )
    return &d_offset;

  // eventOuts
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  else if ( strcmp( fieldName, "rotation" ) == 0 )
    return &d_rotation;
  else if ( strcmp( fieldName, "trackPoint" ) == 0 )
    return &d_trackPoint;
  
  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeCylinderSensor::setField(const char *fieldName,
				      const VrmlField &fieldValue)
{
  if TRY_FIELD(autoOffset, SFBool)
  else if TRY_FIELD(diskAngle, SFFloat)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(maxAngle, SFFloat)
  else if TRY_FIELD(minAngle, SFFloat)
  else if TRY_FIELD(offset, SFFloat)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


void VrmlNodeCylinderSensor::activate( double timeStamp,
				    bool isActive,
				    double *p )
{
}

// Return a new VrmlNodeDirLight
static VrmlNode * createDirLight(VrmlScene * scene) {
    return new VrmlNodeDirLight(scene);
}


// Define the built in VrmlNodeType:: "DirLight" fields

VrmlNodeType *VrmlNodeDirLight::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("DirectionalLight", createDirLight);
      t->reference();
    }

  VrmlNodeLight::defineType(t);	// Parent class
  t->addExposedField("direction", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeDirLight::nodeType() const
{
    return *defineType(0);
}


VrmlNodeDirLight::VrmlNodeDirLight(VrmlScene *scene) :
  VrmlNodeLight(scene),
  d_direction(0.0, 0.0, -1.0)
{
}

VrmlNodeDirLight::~VrmlNodeDirLight()
{
}

bool VrmlNodeDirLight::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeDirLight::printFields(ostream& os, int indent)
{
  VrmlNodeLight::printFields(os, indent);
  if (! FPZERO(d_direction.getX()) ||
      ! FPZERO(d_direction.getY()) ||
      ! FPEQUAL(d_direction.getZ(), -1.0) )
    PRINT_FIELD(direction);

  return os;
}

// This should be called before rendering any sibling nodes.

void VrmlNodeDirLight::render(Viewer *viewer, VrmlRenderContext rc)
{
  if ( d_on.get() )
    viewer->insertDirLight( d_ambientIntensity.get(),
			    d_intensity.get(),
			    d_color.get(),
			    d_direction.get() );
  clearModified();
}

// Get the value of one of the exposedFields

const VrmlField *VrmlNodeDirLight::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "direction" ) == 0 )
    return &d_direction;
  return VrmlNodeLight::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeDirLight::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_FIELD(direction, SFVec3f)
  else 
    VrmlNodeLight::setField(fieldName, fieldValue);
}

const VrmlSFVec3f& VrmlNodeDirLight::getDirection() const   // LarryD Mar 04/99
{  return d_direction; }

VrmlNodeDirLight *VrmlNodeDirLight::toDirLight() const // LarryD Mar 04/99
{ return (VrmlNodeDirLight*) this; }

static VrmlNode *createElevationGrid(VrmlScene * scene) {
  return new VrmlNodeElevationGrid(scene);
}


// Define the built in VrmlNodeType:: "ElevationGrid" fields

VrmlNodeType *VrmlNodeElevationGrid::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("ElevationGrid", createElevationGrid);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class

  t->addEventIn("set_height", VrmlField::MFFLOAT);
  t->addExposedField("color", VrmlField::SFNODE);
  t->addExposedField("normal", VrmlField::SFNODE);
  t->addExposedField("texCoord", VrmlField::SFNODE);
  t->addField("ccw", VrmlField::SFBOOL);
  t->addField("colorPerVertex", VrmlField::SFBOOL);
  t->addField("creaseAngle", VrmlField::SFFLOAT);
  t->addField("height", VrmlField::MFFLOAT);
  t->addField("normalPerVertex", VrmlField::SFBOOL);
  t->addField("solid", VrmlField::SFBOOL);
  t->addField("xDimension", VrmlField::SFINT32);
  t->addField("xSpacing", VrmlField::SFFLOAT);
  t->addField("zDimension", VrmlField::SFINT32);
  t->addField("zSpacing", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodeElevationGrid::nodeType() const
{
    return *defineType(0);
}


VrmlNodeElevationGrid::VrmlNodeElevationGrid(VrmlScene *scene) :
  VrmlNodeGeometry(scene),
  d_ccw(true),
  d_colorPerVertex(true),
  d_normalPerVertex(true),
  d_solid(true),
  d_xDimension(0),
  d_xSpacing(1.0f),
  d_zDimension(0),
  d_zSpacing(1.0f)
{
}

VrmlNodeElevationGrid::~VrmlNodeElevationGrid()
{
}

bool VrmlNodeElevationGrid::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeElevationGrid::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_color.get()) {
            this->d_color.get()->resetVisitedFlag();
        }
        if (this->d_normal.get()) {
            this->d_normal.get()->resetVisitedFlag();
        }
        if (this->d_texCoord.get()) {
            this->d_texCoord.get()->resetVisitedFlag();
        }
    }
}

bool VrmlNodeElevationGrid::isModified() const
{
  return ( d_modified ||
	   (d_color.get() && d_color.get()->isModified()) ||
	   (d_normal.get() && d_normal.get()->isModified()) ||
	   (d_texCoord.get() && d_texCoord.get()->isModified()) );
}

void VrmlNodeElevationGrid::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_color.get()) d_color.get()->updateModified(path);
  if (d_normal.get()) d_normal.get()->updateModified(path);
  if (d_texCoord.get()) d_texCoord.get()->updateModified(path);
  path.pop_front();
}


void VrmlNodeElevationGrid::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_color.get()) d_color.get()->clearFlags();
  if (d_normal.get()) d_normal.get()->clearFlags();
  if (d_texCoord.get()) d_texCoord.get()->clearFlags();
}

void VrmlNodeElevationGrid::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  if (d_color.get()) d_color.get()->addToScene(s, rel);
  if (d_normal.get()) d_normal.get()->addToScene(s, rel);
  if (d_texCoord.get()) d_texCoord.get()->addToScene(s, rel);
}

ostream& VrmlNodeElevationGrid::printFields(ostream& os, int indent)
{
  if (d_color.get()) PRINT_FIELD(color);
  if (d_normal.get()) PRINT_FIELD(normal);
  if (d_texCoord.get()) PRINT_FIELD(texCoord);

  if (! d_ccw.get()) PRINT_FIELD(ccw);
  if (! d_colorPerVertex.get()) PRINT_FIELD(colorPerVertex);
  if (! d_normalPerVertex.get()) PRINT_FIELD(normalPerVertex);
  if (! d_solid.get()) PRINT_FIELD(solid);
  
  if (d_creaseAngle.get() != 0.0) PRINT_FIELD(creaseAngle);
  if (d_height.getLength() > 0) PRINT_FIELD(height);

  if (d_xDimension.get() != 0) PRINT_FIELD(xDimension);
  if (d_xSpacing.get() != 0) PRINT_FIELD(xSpacing);
  if (d_zDimension.get() != 0) PRINT_FIELD(zDimension);
  if (d_zSpacing.get() != 0) PRINT_FIELD(zSpacing);

  return os;
}

Viewer::Object VrmlNodeElevationGrid::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;

  if ( d_height.getLength() > 0 )
    {
      const float *tc = 0, *normals = 0, *colors = 0;

      if (d_texCoord.get())
	{
	  VrmlMFVec2f &texcoord =
	    d_texCoord.get()->toTextureCoordinate()->coordinate();
	  tc = &texcoord.getElement(0)[0];
	}

      if (d_normal.get())
	{
	  VrmlMFVec3f &n = d_normal.get()->toNormal()->normal();
	  normals = &n.getElement(0)[0];
	}

      if (d_color.get())
	{
	  VrmlMFColor &c = d_color.get()->toColor()->color();
	  colors = &c.getElement(0)[0];
	}

      // insert geometry
      unsigned int optMask = 0;
      if (d_ccw.get()) optMask |= Viewer::MASK_CCW;
      if (d_solid.get()) optMask |= Viewer::MASK_SOLID;
      if (d_colorPerVertex.get()) optMask |= Viewer::MASK_COLOR_PER_VERTEX;
      if (d_normalPerVertex.get()) optMask |= Viewer::MASK_NORMAL_PER_VERTEX;

      obj = viewer->insertElevationGrid(optMask,
					d_xDimension.get(),
					d_zDimension.get(),
					d_height.get(),
					d_xSpacing.get(),
					d_zSpacing.get(),
					tc,
					normals,
					colors);
    }

  if (d_color.get()) d_color.get()->clearModified();
  if (d_normal.get()) d_normal.get()->clearModified();
  if (d_texCoord.get()) d_texCoord.get()->clearModified();

  return obj;
}

// Get the value of one of the exposedFields or eventOuts.

const VrmlField *VrmlNodeElevationGrid::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  else if ( strcmp( fieldName, "normal" ) == 0 )
    return &d_normal;
  else if ( strcmp( fieldName, "texCoord" ) == 0 )
    return &d_texCoord;
  return VrmlNodeGeometry::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeElevationGrid::setField(const char *fieldName,
				     const VrmlField &fieldValue)
{
  if TRY_SFNODE_FIELD(color, Color)
  else if TRY_SFNODE_FIELD(normal, Normal)
  else if TRY_SFNODE_FIELD(texCoord, TextureCoordinate)
  else if TRY_FIELD(ccw, SFBool)
  else if TRY_FIELD(colorPerVertex, SFBool)
  else if TRY_FIELD(creaseAngle, SFFloat)
  else if TRY_FIELD(height, MFFloat)
  else if TRY_FIELD(normalPerVertex, SFBool)
  else if TRY_FIELD(solid, SFBool)
  else if TRY_FIELD(xDimension, SFInt32)
  else if TRY_FIELD(xSpacing, SFFloat)
  else if TRY_FIELD(zDimension, SFInt32)
  else if TRY_FIELD(zSpacing, SFFloat)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
}

// LarryD Mar 09/99
VrmlNodeElevationGrid* VrmlNodeElevationGrid::toElevationGrid() const 
{ return (VrmlNodeElevationGrid*) this; }

/**
 * @brief Get the Color node.
 *
 * @return an SFNode object containing the Color node associated with
 *         this ElevationGrid.
 */
const VrmlSFNode & VrmlNodeElevationGrid::getColor() const {
    return this->d_color;
}

/**
 * @brief Set the Color node.
 *
 * @param color an SFNode value containing a Color node.
 */
void VrmlNodeElevationGrid::setColor(const VrmlSFNode & color) {
    assert(!color.get() || color.get()->toColor());
    this->d_color = color;
}

/**
 * @brief Get the Normal node.
 *
 * @return an SFNode object containing the Normal node associated with
 *         this ElevationGrid.
 */
const VrmlSFNode & VrmlNodeElevationGrid::getNormal() const {
    return this->d_normal;
}

/**
 * @brief Set the Normal node.
 *
 * @param normal an SFNode value containing a Normal node.
 */
void VrmlNodeElevationGrid::setNormal(const VrmlSFNode & normal) {
    assert(!normal.get() || normal.get()->toNormal());
    this->d_normal = normal;
}

/**
 * @brief Get the TextureCoordinate node.
 *
 * @return an SFNode object containing the TextureCoordinate node
 *         associated with this ElevationGrid.
 */
const VrmlSFNode & VrmlNodeElevationGrid::getTexCoord() const {
    return this->d_texCoord;
}

/**
 * @brief Set the TextureCoordinate node.
 *
 * @param texCoord an SFNode value containing a TextureCoordinate node.
 */
void VrmlNodeElevationGrid::setTexCoord(const VrmlSFNode & texCoord) {
    assert(!texCoord.get() || texCoord.get()->toTextureCoordinate());
    this->d_texCoord = texCoord;
}

/**
 * @brief Get whether vertices are ordered counter-clockwise.
 *
 * @return an SFBool object.
 */
const VrmlSFBool & VrmlNodeElevationGrid::getCcw() const {
    return this->d_ccw;
}

/**
 * @brief Set whether vertices are ordered counter-clockwise.
 *
 * @param ccw an SFBool object.
 */
void VrmlNodeElevationGrid::setCcw(const VrmlSFBool & ccw) {
    this->d_ccw = ccw;
}

/**
 * @brief Get whether colors are interpreted per-vertex.
 *
 * @return an SFBool object.
 */
const VrmlSFBool & VrmlNodeElevationGrid::getColorPerVertex() const {
    return this->d_colorPerVertex;
}

/**
 * @brief Set whether colors are interpreted per-vertex.
 *
 * @param colorPerVertex an SFBool object.
 */
void VrmlNodeElevationGrid::setColorPerVertex(const VrmlSFBool & colorPerVertex)
{
    this->d_colorPerVertex = colorPerVertex;
}

/**
 * @brief Get the crease angle.
 *
 * @return the crease angle.
 */
const VrmlSFFloat & VrmlNodeElevationGrid::getCreaseAngle() const {
    return this->d_creaseAngle;
}

/**
 * @brief Set the crease angle.
 *
 * @param creaseAngle
 */
void VrmlNodeElevationGrid::setCreaseAngle(const VrmlSFFloat & creaseAngle) {
    this->d_creaseAngle = creaseAngle;
}

/**
 * @brief Get the height field.
 *
 * @return the height field
 */
const VrmlMFFloat & VrmlNodeElevationGrid::getHeight() const {
    return this->d_height;
}

/**
 * @brief Set the height field.
 *
 * @param height a height field
 */
void VrmlNodeElevationGrid::setHeight(const VrmlMFFloat & height) {
    this->d_height = height;
}

/**
 * @brief Get whether normals are interpreted per-vertex.
 *
 * @return an SFBool object.
 */
const VrmlSFBool & VrmlNodeElevationGrid::getNormalPerVertex() const {
    return this->d_normalPerVertex;
}

/**
 * @brief Set whether normals are interpreted per-vertex.
 *
 * @param normalPerVertex an SFBool object.
 */
void VrmlNodeElevationGrid::setNormalPerVertex(const VrmlSFBool &
                                                            normalPerVertex) {
    this->d_normalPerVertex = normalPerVertex;
}

/**
 * @brief Get whether the geometry should be interpreted as a solid.
 *
 * @return an SFBool object.
 */
const VrmlSFBool & VrmlNodeElevationGrid::getSolid() const {
    return this->d_solid;
}

/**
 * @brief Set whether the geometry should be interpreted as solid.
 *
 * @param solid
 */
void VrmlNodeElevationGrid::setSolid(const VrmlSFBool & solid) {
    this->d_solid = solid;
}

/**
 * @brief Get the number of vertices in the <var>x</var>-dimension.
 *
 * @return the xDimension
 */
const VrmlSFInt32 & VrmlNodeElevationGrid::getXDimension() const {
    return this->d_xDimension;
}

/**
 * @brief Set the number of vertices in the <var>x</var>-dimension.
 *
 * @param xDimension
 */
void VrmlNodeElevationGrid::setXDimension(const VrmlSFInt32 & xDimension) {
    this->d_xDimension = xDimension;
}

/**
 * @brief Get the distance between vertices in the
 *        <var>x</var>-dimension.
 *
 * @return the xSpacing
 */
const VrmlSFFloat & VrmlNodeElevationGrid::getXSpacing() const {
    return this->d_xSpacing;
}

/**
 * @brief Set the distance between vertices in the
 *        <var>x</var>-dimension.
 *
 * @param xSpacing
 */
void VrmlNodeElevationGrid::setXSpacing(const VrmlSFFloat & xSpacing) {
    this->d_xSpacing = xSpacing;
}

/**
 * @brief Get the number of vertices in the <var>z</var>-dimension.
 *
 * @return the zDimension
 */
const VrmlSFInt32 & VrmlNodeElevationGrid::getZDimension() const {
    return this->d_zDimension;
}

/**
 * @brief Set the number of vertices in the <var>z</var>-dimension.
 *
 * @param zDimension
 */
void VrmlNodeElevationGrid::setZDimension(const VrmlSFInt32 & zDimension) {
    this->d_zDimension = zDimension;
}

/**
 * @brief Get the distance between vertices in the
 *        <var>z</var>-dimension.
 *
 * @return the zSpacing
 */
const VrmlSFFloat & VrmlNodeElevationGrid::getZSpacing() const {
    return this->d_zSpacing;
}

/**
 * @brief Set the distance between vertices in the
 *        <var>z</var>-dimension.
 *
 * @param zSpacing
 */
void VrmlNodeElevationGrid::setZSpacing(const VrmlSFFloat & zSpacing) {
    this->d_zSpacing = zSpacing;
}

static VrmlNode * createExtrusion(VrmlScene * scene) {
    return new VrmlNodeExtrusion(scene);
}


// Define the built in VrmlNodeType:: "Extrusion" fields

VrmlNodeType *VrmlNodeExtrusion::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Extrusion", createExtrusion);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class

  t->addEventIn("set_crossSection", VrmlField::MFVEC2F);
  t->addEventIn("set_orientation", VrmlField::MFROTATION);
  t->addEventIn("set_scale", VrmlField::MFVEC2F);
  t->addEventIn("set_spine", VrmlField::MFVEC3F);

  t->addField("beginCap", VrmlField::SFBOOL);
  t->addField("ccw", VrmlField::SFBOOL);
  t->addField("convex", VrmlField::SFBOOL);
  t->addField("creaseAngle", VrmlField::SFFLOAT);
  t->addField("crossSection", VrmlField::MFVEC2F);
  t->addField("endCap", VrmlField::SFBOOL);
  t->addField("orientation", VrmlField::MFROTATION);
  t->addField("scale", VrmlField::MFVEC2F);
  t->addField("solid", VrmlField::SFBOOL);
  t->addField("spine", VrmlField::MFVEC3F);

  return t;
}


VrmlNodeType & VrmlNodeExtrusion::nodeType() const
{
    return *defineType(0);
}

namespace {
    const float extrusionDefaultCrossSection_[] =
            { 1.0, 1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0 };
    const float extrusionDefaultScale_[] =
            { 1.0, 1.0 };
    const float extrusionDefaultSpine_[] =
            { 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 };
}

VrmlNodeExtrusion::VrmlNodeExtrusion(VrmlScene * scene):
        VrmlNodeGeometry(scene), d_beginCap(true), d_ccw(true), d_convex(true),
        d_creaseAngle(0), d_crossSection(5, extrusionDefaultCrossSection_),
        d_endCap(true), d_orientation(1), d_scale(1, extrusionDefaultScale_),
        d_solid(true), d_spine(2, extrusionDefaultSpine_) {}

VrmlNodeExtrusion::~VrmlNodeExtrusion() {}

bool VrmlNodeExtrusion::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeExtrusion::printFields(ostream& os, int indent)
{
  if (! d_beginCap.get()) PRINT_FIELD(beginCap);
  if (! d_endCap.get()) PRINT_FIELD(endCap);
  if (! d_ccw.get()) PRINT_FIELD(ccw);
  if (! d_convex.get()) PRINT_FIELD(convex);
  if (! d_solid.get()) PRINT_FIELD(solid);

  if (d_creaseAngle.get() != 0.0) PRINT_FIELD(creaseAngle);
  PRINT_FIELD(crossSection);
  PRINT_FIELD(orientation);
  PRINT_FIELD(scale);
  PRINT_FIELD(spine);

  return os;
}


Viewer::Object VrmlNodeExtrusion::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;
  if ( d_crossSection.getLength() > 0 &&
       d_spine.getLength() > 1 )
    {
      unsigned int optMask = 0;
      if (d_ccw.get()) optMask |= Viewer::MASK_CCW;
      if (d_convex.get()) optMask |= Viewer::MASK_CONVEX;
      if (d_solid.get()) optMask |= Viewer::MASK_SOLID;
      if (d_beginCap.get()) optMask |= Viewer::MASK_BOTTOM;
      if (d_endCap.get()) optMask |= Viewer::MASK_TOP;

      obj = viewer->insertExtrusion(optMask,
				    d_orientation.getLength(),
				    d_orientation.get(),
				    d_scale.getLength(),
				    d_scale.get(),
				    d_crossSection.getLength(),
				    d_crossSection.get(),
				    d_spine.getLength(),
				    d_spine.get());
    }

  return obj;
}


// Set the value of one of the node fields.

void VrmlNodeExtrusion::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(beginCap, SFBool)
  else if TRY_FIELD(ccw, SFBool)
  else if TRY_FIELD(convex, SFBool)
  else if TRY_FIELD(creaseAngle, SFFloat)
  else if TRY_FIELD(crossSection, MFVec2f)
  else if TRY_FIELD(endCap, SFBool)
  else if TRY_FIELD(orientation, MFRotation)
  else if TRY_FIELD(scale, MFVec2f)
  else if TRY_FIELD(solid, SFBool)
  else if TRY_FIELD(spine, MFVec3f)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
}

VrmlNodeExtrusion* VrmlNodeExtrusion::toExtrusion() const
{ return (VrmlNodeExtrusion*) this; }

//  Fog factory.
//  Since Fog is a bindable child node, the first one created needs
//  to notify its containing scene.

static VrmlNode * createFog(VrmlScene * scene) {
    return new VrmlNodeFog(scene);
}


// Define the built in VrmlNodeType:: "Fog" fields

VrmlNodeType *VrmlNodeFog::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Fog", createFog);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_bind", VrmlField::SFBOOL);
  t->addExposedField("color", VrmlField::SFCOLOR);
  t->addExposedField("fogType", VrmlField::SFSTRING);
  t->addExposedField("visibilityRange", VrmlField::SFFLOAT);
  t->addEventOut("isBound", VrmlField::SFBOOL);

  return t;
}


VrmlNodeType & VrmlNodeFog::nodeType() const
{
    return *defineType(0);
}


VrmlNodeFog::VrmlNodeFog(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_color(1.0, 1.0, 1.0),
  d_fogType("LINEAR"),
  d_visibilityRange(0.0),
  d_isBound(false)
{
  if (d_scene) d_scene->addFog(this);
}

VrmlNodeFog::~VrmlNodeFog()
{
  if (d_scene) d_scene->removeFog(this);
}

bool VrmlNodeFog::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeFog* VrmlNodeFog::toFog() const
{ return (VrmlNodeFog*) this; }


void VrmlNodeFog::addToScene(VrmlScene *s, const char *)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addFog(this); }


ostream& VrmlNodeFog::printFields(ostream& os, int indent)
{
  if (d_color.getR() != 1.0 ||
      d_color.getG() != 1.0 ||
      d_color.getB() != 1.0 )
    PRINT_FIELD(color);
  if (strcmp(d_fogType.get(),"LINEAR") != 0)
    PRINT_FIELD(fogType);
  if (d_visibilityRange.get() != 0.0)
    PRINT_FIELD(visibilityRange);

  return os;
}

// Note that this method is not maintaining isBound.

void VrmlNodeFog::eventIn(double timeStamp,
			  const char *eventName,
			  const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_bind") == 0)
    {
      VrmlNodeFog *current = d_scene->bindableFogTop();
      const VrmlSFBool * const b =
                dynamic_cast<const VrmlSFBool *>(&fieldValue);
      
      if (! b)
	{
	  cerr << "Error: invalid value for Fog::set_bind eventIn "
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
	      current = d_scene->bindableFogTop();
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

const VrmlField *VrmlNodeFog::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  else if ( strcmp( fieldName, "fogType" ) == 0 )
    return &d_fogType;
  else if ( strcmp( fieldName, "visibilityRange" ) == 0 )
    return &d_visibilityRange;
  
  // eventOuts
  else if ( strcmp( fieldName, "isBound" ) == 0 )
    {
      VrmlSFBool* isBound = (VrmlSFBool*) &(this->d_isBound);
      isBound->set( d_scene->bindableFogTop() == this );
      return isBound;
    }

  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeFog::setField(const char *fieldName,
			   const VrmlField &fieldValue)
{
  if TRY_FIELD(color, SFColor)
  else if TRY_FIELD(fogType, SFString)
  else if TRY_FIELD(visibilityRange, SFFloat)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


static VrmlNode * createFontStyle(VrmlScene * scene) {
    return new VrmlNodeFontStyle(scene);
}


// Define the built in VrmlNodeType:: "FontStyle" fields

VrmlNodeType *VrmlNodeFontStyle::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("FontStyle", createFontStyle);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addField("family", VrmlField::MFSTRING);
  t->addField("horizontal", VrmlField::SFBOOL);
  t->addField("justify", VrmlField::MFSTRING);
  t->addField("language", VrmlField::SFSTRING);
  t->addField("leftToRight", VrmlField::SFBOOL);
  t->addField("size", VrmlField::SFFLOAT);
  t->addField("spacing", VrmlField::SFFLOAT);
  t->addField("style", VrmlField::SFSTRING);
  t->addField("topToBottom", VrmlField::SFBOOL);

  return t;
}


VrmlNodeType & VrmlNodeFontStyle::nodeType() const
{
    return *defineType(0);
}

namespace {
    const char * const fontStyleInitFamily_[] = { "SERIF" };
    const char * const fontStyleInitJustify_[] = { "BEGIN" };
}

VrmlNodeFontStyle::VrmlNodeFontStyle(VrmlScene *scene): VrmlNode(scene),
        d_family(1, fontStyleInitFamily_), d_horizontal(true),
        d_justify(1, fontStyleInitJustify_), d_leftToRight(true), d_size(1.0),
        d_spacing(1.0), d_style("PLAIN"), d_topToBottom(true) {}

VrmlNodeFontStyle::~VrmlNodeFontStyle()
{
}

bool VrmlNodeFontStyle::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeFontStyle* VrmlNodeFontStyle::toFontStyle() const
{ return (VrmlNodeFontStyle*) this; }


ostream& VrmlNodeFontStyle::printFields(ostream& os, int indent)
{
  if (d_family.getLength() > 1 ||
      (d_family.getLength() == 1 && strcmp(d_family.getElement(0),"SERIF")) )
    PRINT_FIELD(family);
  if (! d_horizontal.get()) PRINT_FIELD(horizontal);
  if (d_justify.getLength() > 1 ||
      (d_justify.getLength() == 1 && strcmp(d_justify.getElement(0),"BEGIN")) )
    PRINT_FIELD(justify);
  if (d_language.get() && strcmp(d_language.get(), ""))
    PRINT_FIELD(language);
  if (! d_leftToRight.get()) PRINT_FIELD(leftToRight);
  if (! FPEQUAL(d_size.get(), 1.0)) PRINT_FIELD(size);
  if (! FPEQUAL(d_spacing.get(), 1.0)) PRINT_FIELD(spacing);
  if (d_style.get() && strcmp(d_style.get(), "PLAIN"))
    PRINT_FIELD(style);
  if (! d_topToBottom.get()) PRINT_FIELD(topToBottom);

  return os;
}


// Set the value of one of the node fields.

void VrmlNodeFontStyle::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(family, MFString)
  else if TRY_FIELD(horizontal, SFBool)
  else if TRY_FIELD(justify, MFString)		
  else if TRY_FIELD(language, SFString)		
  else if TRY_FIELD(leftToRight, SFBool)
  else if TRY_FIELD(size, SFFloat)
  else if TRY_FIELD(spacing, SFFloat)
  else if TRY_FIELD(style, SFString)		
  else if TRY_FIELD(topToBottom, SFBool)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

// Return a new VrmlNodeGroup
static VrmlNode * createGroup(VrmlScene * scene) {
    return new VrmlNodeGroup(scene);
}


// Define the built in VrmlNodeType:: "Group" fields

VrmlNodeType *VrmlNodeGroup::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Group", createGroup);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("addChildren", VrmlField::MFNODE);
  t->addEventIn("removeChildren", VrmlField::MFNODE);
  t->addExposedField("children", VrmlField::MFNODE);
  t->addField("bboxCenter", VrmlField::SFVEC3F);
  t->addField("bboxSize", VrmlField::SFVEC3F);

  return t;
}


VrmlNodeType & VrmlNodeGroup::nodeType() const
{
    return *defineType(0);
}


VrmlNodeGroup::VrmlNodeGroup(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_bboxSize(-1.0, -1.0, -1.0),
  d_parentTransform(0),
  d_viewerObject(0)
{
  this->setBVolumeDirty(true);
}

VrmlNodeGroup::~VrmlNodeGroup()
{
  // delete d_viewerObject...
}

bool VrmlNodeGroup::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeGroup::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        for (size_t i = 0; i < this->d_children.getLength(); ++i) {
            if (this->d_children.getElement(i)) {
                this->d_children.getElement(i)->resetVisitedFlag();
            }
        }
    }
}

VrmlNodeGroup* VrmlNodeGroup::toGroup() const
{ return (VrmlNodeGroup*) this; }


bool VrmlNodeGroup::isModified() const
{
  if (d_modified) return true;
  
  int n = d_children.getLength();

  for (int i = 0; i<n; ++i)
    if (d_children.getElement(i)->isModified())
      return true;

  return false;
}


void
VrmlNodeGroup::updateModified(VrmlNodePath& path, int flags)
{
  // if the mark_modifed short circuit doesn't
  // pan out, we should be a little smarter here...
  //cout << "VrmlNodeGroup[" << this << "]::updateModified()" << endl;
  if (this->isModified()) markPathModified(path, true, flags);
  path.push_front(this);
  int n = d_children.getLength();
  //cout << "VrmlNodeGroup[" << this << "]::updateModified():n=" << n << endl;
  for (int i=0; i<n; ++i) {
    //cout << "VrmlNodeGroup[" << this << "]::updateModified():" << d_children[i] << endl;
    d_children.getElement(i)->updateModified(path, flags);
  }
  path.pop_front();
}



void VrmlNodeGroup::clearFlags()
{
  VrmlNode::clearFlags();
  int n = d_children.getLength();
  for (int i=0; i<n; ++i)
    d_children.getElement(i)->clearFlags();
}


void VrmlNodeGroup::addToScene(VrmlScene *s, const char *relativeUrl)
{
  d_scene = s;

  //theSystem->debug("VrmlNodeGroup::addToScene( %s )\n",
  //relativeUrl ? relativeUrl : "<null>");

  const char *currentRel = d_relative.get();
  if ( ! currentRel || ! relativeUrl || strcmp(currentRel, relativeUrl) != 0)
    d_relative.set( relativeUrl );
  
  int n = d_children.getLength();

  for (int i = 0; i<n; ++i)
    d_children.getElement(i)->addToScene(s, d_relative.get());
}


VrmlNode* VrmlNodeGroup::getParentTransform() { return d_parentTransform; }



ostream& VrmlNodeGroup::printFields(ostream& os, int indent)
{
  if (d_bboxCenter.getX() != 0.0 ||
      d_bboxCenter.getZ() != 0.0 ||
      d_bboxCenter.getY() != 0.0)
    PRINT_FIELD(bboxCenter);
  if (d_bboxSize.getX() != -1.0 ||
      d_bboxSize.getZ() != -1.0 ||
      d_bboxSize.getY() != -1.0)
    PRINT_FIELD(bboxSize);
  if (d_children.getLength() > 0)
    PRINT_FIELD(children);

  return os;
}


// Render each of the children

void VrmlNodeGroup::render(Viewer *viewer, VrmlRenderContext rc)
{
  //cout << "VrmlNodeGroup::render()" << endl;

  if (rc.getCullFlag() != VrmlBVolume::BV_INSIDE) {

    const VrmlBSphere* bs = (const VrmlBSphere*)this->getBVolume();
    VrmlBSphere bv_copy(*bs);
    bv_copy.transform(rc.getMatrix());
    int r = viewer->isectViewVolume(bv_copy);
    if (rc.getDrawBSpheres())
      viewer->drawBSphere(*bs, r);
    //bs->dump(cout);

    if (r == VrmlBVolume::BV_OUTSIDE)
      return;
    if (r == VrmlBVolume::BV_INSIDE)
      rc.setCullFlag(VrmlBVolume::BV_INSIDE);

    //rc.setCullFlag(VrmlBVolume::BV_PARTIAL);
  }

  renderNoCull(viewer, rc);
}


// because children will already have done the culling, we don't need
// to repeat it here.
//
void VrmlNodeGroup::renderNoCull(Viewer *viewer, VrmlRenderContext rc)
{
  //cout << "VrmlNodeGroup::renderNoCull()" << endl;

  if ( d_viewerObject && isModified() )
    {
      viewer->removeObject(d_viewerObject);
      d_viewerObject = 0;
    }

  if (d_viewerObject)
    viewer->insertReference(d_viewerObject);

  else if (d_children.getLength() > 0)
    {
      int i, n = d_children.getLength();
      int nSensors = 0;

      d_viewerObject = viewer->beginObject(name());

      // Draw nodes that impact their siblings (DirectionalLights,
      // TouchSensors, any others? ...)
      for (i = 0; i<n; ++i)
	{
	  const VrmlNodePtr & kid(d_children.getElement(i));

	  if ( kid->toLight() && ! (kid->toPointLight() || kid->toSpotLight()) )
	    kid->render(viewer, rc);
	  else if (( kid->toTouchSensor() && kid->toTouchSensor()->isEnabled() ) ||
		   ( kid->toPlaneSensor() && kid->toPlaneSensor()->isEnabled() ) ||
		   ( kid->toCylinderSensor() && kid->toCylinderSensor()->isEnabled() ) ||
		    ( kid->toSphereSensor() && kid->toSphereSensor()->isEnabled() ))
	    {
	      if (++nSensors == 1)
		viewer->setSensitive( this );
	    }
	}	      

      // Do the rest of the children (except the scene-level lights)
      for (i = 0; i<n; ++i) {
        const VrmlNodePtr & child(this->d_children.getElement(i));
	if (! (child->toLight() ||
	       child->toPlaneSensor() ||
	       child->toCylinderSensor() ||
	       child->toSphereSensor() ||
	       child->toTouchSensor()) )
	  child->render(viewer, rc);
      }

      // Turn off sensitivity
      if (nSensors > 0)
	viewer->setSensitive( 0 );

      viewer->endObject();

    }

  clearModified();
}


// Accumulate transforms
// Cache a pointer to (one of the) parent transforms for proper
// rendering of bindables.

void VrmlNodeGroup::accumulateTransform(VrmlNode * parent) {
    d_parentTransform = parent;
    for (size_t i = 0; i < this->d_children.getLength(); ++i) {
        if (this->d_children.getElement(i)) {
            this->d_children.getElement(i)->accumulateTransform(parent);
        }
    }
}


// Pass on to enabled touchsensor child.

void VrmlNodeGroup::activate( double time,
			      bool isOver, bool isActive,
			      double *p )
{
  int i, n = d_children.getLength();

  for (i = 0; i<n; ++i)
    {
      const VrmlNodePtr & kid(d_children.getElement(i));

      if ( kid->toTouchSensor() && kid->toTouchSensor()->isEnabled() )
	{
	  kid->toTouchSensor()->activate( time, isOver, isActive, p );
	  break;
	}
      else if ( kid->toPlaneSensor() && kid->toPlaneSensor()->isEnabled() )
	{
	  kid->toPlaneSensor()->activate( time, isActive, p );
	  break;
	}
      else if ( kid->toCylinderSensor() && kid->toCylinderSensor()->isEnabled() )
	{
	  kid->toCylinderSensor()->activate( time, isActive, p );
	  break;
	}
      else if ( kid->toSphereSensor() && kid->toSphereSensor()->isEnabled() )
	{
	  kid->toSphereSensor()->activate( time, isActive, p );
	  break;
	}
    }	      

}

/**
 * @brief Get the Group's children.
 *
 * @return the Group's children
 */
const VrmlMFNode & VrmlNodeGroup::getChildren() const {
    return this->d_children;
}

/**
 * @brief Set the Group's children.
 *
 * @param children the new children for the node
 *
 * @todo We should throw an exception if any of the nodes in
 *       <var>children</var> are not child nodes.
 */
void VrmlNodeGroup::setChildren(const VrmlMFNode & children) {
    const size_t currentLength = this->d_children.getLength();
    
    for (size_t i = 0; i < children.getLength(); ++i) {
        VrmlNodeProto * p = 0;
        const VrmlNodePtr & child(children.getElement(i));
        if (child && (child->toChild() ||
	        ((p = child->toProto()) != 0 && p->getImplNodes().getLength() == 0))) {
	    child->addToScene(d_scene, d_relative.get());
	    child->accumulateTransform(d_parentTransform);
	} else {
	    theSystem->error(
                "Error: Attempt to add a %s node as a child of a %s node.\n",
	        child->nodeType().getName(), nodeType().getName());
        }
    }
    
    this->d_children = children;
    
    if (currentLength != d_children.getLength()) {
        //??eventOut( d_scene->timeNow(), "children_changed", d_children );
        setModified();
        this->setBVolumeDirty(true);
    }
}

/**
 * @brief Add children from another MFNode.
 *
 * Add legal children and un-instantiated EXTERNPROTOs. Children only
 * get added if they do not already exist in this Group. NULLs in the
 * argument MFNode are <strong>not</strong> added.
 *
 * @param children a MFNode containing the nodes to add to this Group
 */
void VrmlNodeGroup::addChildren(const VrmlMFNode & children) {
    size_t nNow = d_children.getLength();
    size_t n = children.getLength();
    
    for (size_t i = 0; i < n; ++i) {
        const VrmlNodePtr & child(children.getElement(i));
        VrmlNodeProto *p = 0;
        
        if (child && (child->toChild() ||
	        ((p = child->toProto()) != 0 && p->getImplNodes().getLength() == 0))) {
	    d_children.addNode(*child);
	    child->addToScene( d_scene, d_relative.get() );
	    child->accumulateTransform( d_parentTransform );
	} else {
	    theSystem->error(
                "Error: Attempt to add a %s node as a child of a %s node.\n",
	        child->nodeType().getName(), nodeType().getName());
        }
    }
    
    if (nNow != d_children.getLength()) {
        //??eventOut( d_scene->timeNow(), "children_changed", d_children );
        setModified();
        this->setBVolumeDirty(true);
    }
}

void VrmlNodeGroup::removeChildren( const VrmlMFNode &children )
{
    const size_t oldLength = d_children.getLength();
    
    for (size_t i = 0; i < children.getLength(); ++i) {
        if (children.getElement(i)) {
            this->d_children.removeNode(*children.getElement(i));
        }
    }
    
    if (oldLength != this->d_children.getLength()) {
        //??eventOut( d_scene->timeNow(), "children_changed", d_children );
        setModified();
        this->setBVolumeDirty(true);
    }
}

/**
 * @todo Remove this method in favor of passing an empty VrmlMFNode to
 *       setChildren()?
 */
void VrmlNodeGroup::removeChildren() {
    for (size_t i = this->d_children.getLength(); i > 0; --i) {
        if (this->d_children.getElement(i - 1)) {
            d_children.removeNode(*d_children.getElement(i - 1));
        }
    }
    
    setModified();
    this->setBVolumeDirty(true);
}


void VrmlNodeGroup::eventIn(double timeStamp,
			    const char *eventName,
			    const VrmlField & fieldValue)
{
  if (strcmp(eventName, "addChildren") == 0)
    {
      if (dynamic_cast<const VrmlMFNode *>(&fieldValue))      // check that fieldValue is MFNode
	addChildren(static_cast<const VrmlMFNode &>(fieldValue));
      else
	theSystem->error("VrmlNodeGroup.%s %s eventIn invalid field type.\n",
		      name(), eventName);
    }

  else if (strcmp(eventName, "removeChildren") == 0)
    {
      if (dynamic_cast<const VrmlMFNode *>(&fieldValue))      // check that fieldValue is MFNode
	removeChildren(static_cast<const VrmlMFNode &>(fieldValue));
      else
	theSystem->error("VrmlNodeGroup.%s %s eventIn invalid field type.\n",
		      name(), eventName);
    }

  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);
    }
}

// Get the value of one of the exposedFields or eventOuts.

const VrmlField *VrmlNodeGroup::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "children" ) == 0 )
    return &d_children;

  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.
void VrmlNodeGroup::setField(const char *fieldName,
			     const VrmlField &fieldValue)
{
  if TRY_FIELD(bboxCenter, SFVec3f)
  else if TRY_FIELD(bboxSize, SFVec3f)
  else if TRY_FIELD(children, MFNode)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true); // overly conservative?
}

const VrmlBVolume* VrmlNodeGroup::getBVolume() const
{
  //cout << "VrmlNodeGroup[" << this << "]::getBVolume() {" << endl;
  if (this->isBVolumeDirty())
    ((VrmlNodeGroup*)this)->recalcBSphere();
  //cout << "}:";
  //d_bsphere.dump(cout) << endl;
  return &d_bsphere;
}

void VrmlNodeGroup::recalcBSphere() {
    d_bsphere.reset();
    for (size_t i = 0; i< d_children.getLength(); ++i) {
        if (this->d_children.getElement(i)) {
            const VrmlBVolume * const ci_bv = d_children.getElement(i)->getBVolume();
            if (ci_bv) {
                d_bsphere.extend(*ci_bv);
            }
        }
    }
    this->setBVolumeDirty(false);
}

static VrmlNode * createIFaceSet(VrmlScene * scene) {
    return new VrmlNodeIFaceSet(scene);
}

// Define the built in VrmlNodeType:: "IndexedFaceSet" fields

VrmlNodeType *VrmlNodeIFaceSet::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("IndexedFaceSet", createIFaceSet);
      t->reference();
    }

  VrmlNodeIndexedSet::defineType(t);	// Parent class

  t->addEventIn("set_normalIndex", VrmlField::MFINT32);
  t->addEventIn("set_texCoordIndex", VrmlField::MFINT32);
  t->addExposedField("normal", VrmlField::SFNODE);
  t->addExposedField("texCoord", VrmlField::SFNODE);
  t->addField("ccw", VrmlField::SFBOOL);
  t->addField("convex", VrmlField::SFBOOL);
  t->addField("creaseAngle", VrmlField::SFFLOAT);
  t->addField("normalIndex", VrmlField::MFINT32);
  t->addField("normalPerVertex", VrmlField::SFBOOL);
  t->addField("solid", VrmlField::SFBOOL);
  t->addField("texCoordIndex", VrmlField::MFINT32);

  return t;
}


VrmlNodeType & VrmlNodeIFaceSet::nodeType() const
{
    return *defineType(0);
}


VrmlNodeIFaceSet::VrmlNodeIFaceSet(VrmlScene *scene) :
  VrmlNodeIndexedSet(scene),
  d_ccw(true),
  d_convex(true),
  d_creaseAngle(0.0),
  d_normalPerVertex(true),
  d_solid(true)
{
  this->setBVolumeDirty(true);
}

VrmlNodeIFaceSet::~VrmlNodeIFaceSet()
{
}

bool VrmlNodeIFaceSet::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeIFaceSet::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_color.get()) {
            this->d_color.get()->resetVisitedFlag();
        }
        if (this->d_coord.get()) {
            this->d_coord.get()->resetVisitedFlag();
        }
        if (this->d_normal.get()) {
            this->d_normal.get()->resetVisitedFlag();
        }
        if (this->d_texCoord.get()) {
            this->d_texCoord.get()->resetVisitedFlag();
        }
    }
}

bool VrmlNodeIFaceSet::isModified() const
{
  return ( d_modified ||
	   (d_color.get() && d_color.get()->isModified()) ||
	   (d_coord.get() && d_coord.get()->isModified()) ||
	   (d_normal.get() && d_normal.get()->isModified()) ||
	   (d_texCoord.get() && d_texCoord.get()->isModified()) );
}

void VrmlNodeIFaceSet::updateModified(VrmlNodePath& path, int flags)
{
  //cout << "VrmlNodeIFaceSet::updateModified()" << endl;
  if (this->isModified()) markPathModified(path, true, flags);
  path.push_front(this);
  if (d_color.get()) d_color.get()->updateModified(path, flags);
  if (d_coord.get()) d_coord.get()->updateModified(path, flags);
  if (d_normal.get()) d_normal.get()->updateModified(path, flags);
  if (d_texCoord.get()) d_texCoord.get()->updateModified(path, flags);
  path.pop_front();
}

void VrmlNodeIFaceSet::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_color.get()) d_color.get()->clearFlags();
  if (d_coord.get()) d_coord.get()->clearFlags();
  if (d_normal.get()) d_normal.get()->clearFlags();
  if (d_texCoord.get()) d_texCoord.get()->clearFlags();
}

void VrmlNodeIFaceSet::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  if (d_color.get()) d_color.get()->addToScene(s, rel);
  if (d_coord.get()) d_coord.get()->addToScene(s, rel);
  if (d_normal.get()) d_normal.get()->addToScene(s, rel);
  if (d_texCoord.get()) d_texCoord.get()->addToScene(s, rel);
}

ostream& VrmlNodeIFaceSet::printFields(ostream& os, int indent)
{
  if (! d_ccw.get()) PRINT_FIELD(ccw);
  if (! d_convex.get()) PRINT_FIELD(convex);
  if (! d_normalPerVertex.get()) PRINT_FIELD(normalPerVertex);
  if (! d_solid.get()) PRINT_FIELD(solid);

  if (d_creaseAngle.get() != 0.0) PRINT_FIELD(creaseAngle);
  if (d_normal.get()) PRINT_FIELD(normal);
  if (d_normalIndex.getLength() > 0) PRINT_FIELD(normalIndex);
  if (d_texCoord.get()) PRINT_FIELD(texCoord);
  if (d_texCoordIndex.getLength() > 0) PRINT_FIELD(texCoordIndex);

  VrmlNodeIndexedSet::printFields(os, indent);

  return os;
}
  

// TO DO: stripify, crease angle, generate normals ...

Viewer::Object VrmlNodeIFaceSet::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;

  if (rc.getDrawBSpheres()) {
    const VrmlBSphere* bs = (VrmlBSphere*)this->getBVolume();
    viewer->drawBSphere(*bs, 4);
  }

  if (d_coord.get() && d_coordIndex.getLength() > 0)
    {
      VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();
      int nvert = coord.getLength();
      const float *tc = 0, *color = 0, *normal = 0;
      int ntc = 0;
      size_t ntci = 0; const long * tci = 0;	// texture coordinate indices
      int nci = 0; const long * ci = 0;	// color indices
      int nni = 0; const long * ni = 0;	// normal indices

      // Get texture coordinates and texCoordIndex
      if (d_texCoord.get())
	{
	  VrmlMFVec2f &texcoord =
	    d_texCoord.get()->toTextureCoordinate()->coordinate();
	  tc = &texcoord.getElement(0)[0];
	  ntc = texcoord.getLength();
	  ntci = d_texCoordIndex.getLength();
	  if (ntci) tci = d_texCoordIndex.get();
	}

      // check #tc is consistent with #coords/max texCoordIndex...
      if (tci && ntci < d_coordIndex.getLength())
	{
	  theSystem->error("IndexedFaceSet: not enough texCoordIndex values (there should be at least as many as coordIndex values).\n");
	  theSystem->error("IndexedFaceSet: #coord %d, #coordIndex %d, #texCoord %d, #texCoordIndex %d\n", nvert, d_coordIndex.getLength(), ntc, ntci);
	  tci = 0;
	  ntci = 0;
	}

      // check #colors is consistent with colorPerVtx, colorIndex...
      if (d_color.get())
	{
	  VrmlMFColor &c = d_color.get()->toColor()->color();
	  color = &c.getElement(0)[0];
	  nci = d_colorIndex.getLength();
	  if (nci) ci = d_colorIndex.get();
	}

      // check #normals is consistent with normalPerVtx, normalIndex...
      if (d_normal.get())
	{
	  VrmlMFVec3f &n = d_normal.get()->toNormal()->normal();
	  normal = &n.getElement(0)[0];
	  nni = d_normalIndex.getLength();
	  if (nni) ni = d_normalIndex.get();
	}

      unsigned int optMask = 0;
      if (d_ccw.get()) optMask |= Viewer::MASK_CCW;
      if (d_convex.get()) optMask |= Viewer::MASK_CONVEX;
      if (d_solid.get()) optMask |= Viewer::MASK_SOLID;
      if (d_colorPerVertex.get()) optMask |= Viewer::MASK_COLOR_PER_VERTEX;
      if (d_normalPerVertex.get()) optMask |= Viewer::MASK_NORMAL_PER_VERTEX;

      obj = viewer->insertShell(optMask,
                                nvert, &coord.getElement(0)[0],
				d_coordIndex.getLength(), d_coordIndex.get(),
				tc, ntci, tci,
				normal, nni, ni,
				color, nci, ci);
    }

  if (d_color.get()) d_color.get()->clearModified();
  if (d_coord.get()) d_coord.get()->clearModified();
  if (d_normal.get()) d_normal.get()->clearModified();
  if (d_texCoord.get()) d_texCoord.get()->clearModified();

  return obj;
}

// Get the value of one of the exposedFields

const VrmlField *VrmlNodeIFaceSet::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "normal" ) == 0 )
    return &d_normal;
  else if ( strcmp( fieldName, "texCoord" ) == 0 )
    return &d_texCoord;
  
  return VrmlNodeIndexedSet::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeIFaceSet::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_FIELD(ccw, SFBool)
  else if TRY_FIELD(convex, SFBool)
  else if TRY_FIELD(creaseAngle, SFFloat)
  else if TRY_SFNODE_FIELD(normal, Normal)
  else if TRY_FIELD(normalIndex, MFInt32)
  else if TRY_FIELD(normalPerVertex, SFBool)
  else if TRY_FIELD(solid, SFBool)
  else if TRY_FIELD(normalIndex, MFInt32)
  else if TRY_SFNODE_FIELD(texCoord, TextureCoordinate)
  else if TRY_FIELD(texCoordIndex, MFInt32)
  else
    VrmlNodeIndexedSet::setField(fieldName, fieldValue);

  // overly conservative: changing the creaseAngle doesn't really
  // change the bvolume, but the TRY_FIELD macro makes it hard to
  // case things out. sigh. "clarity over optimization", but it
  // still rankles...
  //
  this->setBVolumeDirty(true);
}


const VrmlMFInt32 & VrmlNodeIFaceSet::getNormalIndex() const
{   return d_normalIndex; }

const VrmlMFInt32 & VrmlNodeIFaceSet::getTexCoordIndex() const
{   return d_texCoordIndex; }

VrmlNodeIFaceSet* VrmlNodeIFaceSet::toIFaceSet() const
{ return (VrmlNodeIFaceSet*) this; }



void VrmlNodeIFaceSet::recalcBSphere()
{
  //cout << "VrmlNodeIFaceSet::recalcBSphere()" << endl;

  // take the bvolume of all the points. technically, we should figure
  // out just which points are used by the index and just use those,
  // but for a first pass this is fine (also: if we do it this way
  // then we don't have to update the bvolume when the index
  // changes). motto: always do it the simple way first...
  //
  VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();
  const float* p = coord.get();
  int n = coord.getLength();
  d_bsphere.reset();
  d_bsphere.enclose(p, n);
  //int nvert = coord.size();
  //for(int i=0; i<nvert; i++) {
  //float* vi = coord[i]; // vi[3]
  //d_bsphere.extend(vi);
  //}
  //d_bsphere.dump(cout);
  this->setBVolumeDirty(false);
}


const VrmlBVolume* VrmlNodeIFaceSet::getBVolume() const
{
  //cout << "VrmlNodeIFaceSet::getBVolume():" << this->isBVolumeDirty() << endl;
  if (this->isBVolumeDirty())
    ((VrmlNodeIFaceSet*)this)->recalcBSphere();
  return &d_bsphere; // hmmm, const?
}

/**
 * @brief Get the Normal node associated with this IndexedFaceSet.
 *
 * @return normal
 */
const VrmlSFNode & VrmlNodeIFaceSet::getNormal() const {
    return this->d_normal;
}

/**
 * @brief Set the Normal node associated with this IndexedFaceSet.
 *
 * @param normal
 */
void VrmlNodeIFaceSet::setNormal(const VrmlSFNode & normal) {
    this->d_normal = normal;
}

/**
 * @brief Get the TextureCoordinate node associated with this
 *        IndexedFaceSet.
 *
 * @return the SFNode value of texCoord
 */
const VrmlSFNode & VrmlNodeIFaceSet::getTexCoord() const {
    return this->d_texCoord;
}

/**
 * @brief Set the TextureCoordinate node associated with this
 *        IndexedFaceSet.
 *
 * @param textCoord
 */
void VrmlNodeIFaceSet::setTexCoord(const VrmlSFNode & texCoord) {
    this->d_texCoord = texCoord;
}

static VrmlNode * createILineSet(VrmlScene * scene) {
    return new VrmlNodeILineSet(scene);
}


// Define the built in VrmlNodeType:: "IndexedLineSet" fields

VrmlNodeType *VrmlNodeILineSet::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("IndexedLineSet", createILineSet);
      t->reference();
    }

  VrmlNodeIndexedSet::defineType(t);	// Parent class

  return t;
}

VrmlNodeType & VrmlNodeILineSet::nodeType() const
{
    return *defineType(0);
}

VrmlNodeILineSet::VrmlNodeILineSet(VrmlScene *scene) :
  VrmlNodeIndexedSet(scene)
{
}

VrmlNodeILineSet::~VrmlNodeILineSet()
{
}

bool VrmlNodeILineSet::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeILineSet::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_color.get()) {
            this->d_color.get()->resetVisitedFlag();
        }
        if (this->d_coord.get()) {
            this->d_coord.get()->resetVisitedFlag();
        }
    }
}

// TO DO colors

Viewer::Object VrmlNodeILineSet::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;
  if (d_coord.get() && d_coordIndex.getLength() > 0)
    {
      VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();
      int nvert = coord.getLength();
      const float * color = 0;
      int nci = 0; const long * ci = 0;

      // check #colors is consistent with colorPerVtx, colorIndex...
      if (d_color.get())
	{
	  VrmlMFColor &c = d_color.get()->toColor()->color();
	  color = &c.getElement(0)[0];
	  nci = d_colorIndex.getLength();
	  if (nci) ci = d_colorIndex.get();
	}

      obj =  viewer->insertLineSet(nvert,
                                   &coord.getElement(0)[0],
				   d_coordIndex.getLength(),
				   d_coordIndex.get(),
				   d_colorPerVertex.get(),
				   color,
				   nci, ci);

    }
  
  if (d_color.get()) d_color.get()->clearModified();
  if (d_coord.get()) d_coord.get()->clearModified();

  return obj;
}

static VrmlNode * createImageTexture(VrmlScene * scene) {
    return new VrmlNodeImageTexture(scene);
}

const VrmlMFString& VrmlNodeImageTexture::getUrl() const 
{   return d_url; }

VrmlNodeImageTexture* VrmlNodeImageTexture::toImageTexture() const
{ return (VrmlNodeImageTexture*) this; }

// Define the built in VrmlNodeType:: "ImageTexture" fields

VrmlNodeType *VrmlNodeImageTexture::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("ImageTexture", createImageTexture);
      t->reference();
    }

  VrmlNodeTexture::defineType(t);	// Parent class

  t->addExposedField("url", VrmlField::MFSTRING);
  t->addField("repeatS", VrmlField::SFBOOL);
  t->addField("repeatT", VrmlField::SFBOOL);

  return t;
}

VrmlNodeImageTexture::VrmlNodeImageTexture(VrmlScene *scene) :
  VrmlNodeTexture(scene),
  d_repeatS(true),
  d_repeatT(true),
  d_image(0),
  d_texObject(0)
{
}

VrmlNodeType & VrmlNodeImageTexture::nodeType() const
{
    return *defineType(0);
}

VrmlNodeImageTexture::~VrmlNodeImageTexture()
{
  delete d_image;
  // delete d_texObject...
}

bool VrmlNodeImageTexture::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeImageTexture::printFields(ostream& os, int indent)
{
  if (d_url.get()) PRINT_FIELD(url);
  if (! d_repeatS.get()) PRINT_FIELD(repeatS);
  if (! d_repeatT.get()) PRINT_FIELD(repeatT);
  return os;
}


void VrmlNodeImageTexture::render(Viewer *viewer, VrmlRenderContext rc)
{
  if ( isModified() )
    {
      if (d_image)
	{
	  delete d_image;		// URL is the only modifiable bit
	  d_image = 0;
	}
      if (d_texObject)
	{
	  viewer->removeTextureObject(d_texObject);
	  d_texObject = 0;
	}
    }

  // should probably read the image during addToScene...
  // should cache on url so multiple references to the same file are
  // loaded just once... of course world authors should just DEF/USE
  // them...
  if (! d_image && d_url.getLength() > 0)
    {
      const char *relUrl = d_relativeUrl.get() ? d_relativeUrl.get() :
	d_scene->urlDoc()->url();
      Doc relDoc(relUrl, static_cast<Doc const *>(0));
      d_image = new Image;
      if ( ! d_image->tryURLs( d_url.getLength(), d_url.get(), &relDoc ) )
	      theSystem->error("Couldn't read ImageTexture from URL %s\n", (char*)d_url.getElement(0));
    }

  // Check texture cache
  if (d_texObject && d_image)
    {
      viewer->insertTextureReference(d_texObject, d_image->nc());
    }
  else
    {
      unsigned char *pix;

      if (d_image && (pix = d_image->pixels()))
	{
	  // Ensure the image dimensions are powers of two
	  int sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
	  int nSizes = sizeof(sizes) / sizeof(int);
	  int w = d_image->w();
	  int h = d_image->h();
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
					d_image->nc(), pix );
		  d_image->setSize( sizes[i-1], sizes[j-1] );
		}

	      d_texObject = viewer->insertTexture(d_image->w(),
						  d_image->h(),
						  d_image->nc(),
						  d_repeatS.get(),
						  d_repeatT.get(),
						  pix,
						  true);
	    }
	}
    }

  clearModified();
}


size_t VrmlNodeImageTexture::nComponents()
{
  return d_image ? d_image->nc() : 0;
}

size_t VrmlNodeImageTexture::width()
{
  return d_image ? d_image->w() : 0;
}

size_t VrmlNodeImageTexture::height()
{
  return d_image ? d_image->h() : 0;
}

size_t VrmlNodeImageTexture::nFrames()
{
  return 0;
}

const unsigned char * VrmlNodeImageTexture::pixels()
{
  return d_image ? d_image->pixels() : 0;
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeImageTexture::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "url" ) == 0 )
    return &d_url;
  
  return VrmlNode::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeImageTexture::setField(const char *fieldName,
				    const VrmlField &fieldValue)
{
  if (strcmp(fieldName,"url") == 0)
    {
      delete d_image;
      d_image = 0;
    }

  if TRY_FIELD(url, MFString)
  else if TRY_FIELD(repeatS, SFBool)
  else if TRY_FIELD(repeatT, SFBool)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

bool VrmlNodeImageTexture::getRepeatS() const
{
    return d_repeatS.get();
}

bool VrmlNodeImageTexture::getRepeatT() const
{
    return d_repeatT.get();
}

// Define the built in VrmlNodeType:: "Indexed*Set" fields

VrmlNodeType *VrmlNodeIndexedSet::defineType(VrmlNodeType *t)
{
  VrmlNodeGeometry::defineType(t);	// Parent class

  t->addEventIn("set_colorIndex", VrmlField::MFINT32);
  t->addEventIn("set_coordIndex", VrmlField::MFINT32);
  t->addExposedField("color", VrmlField::SFNODE);
  t->addExposedField("coord", VrmlField::SFNODE);
  t->addField("colorIndex", VrmlField::MFINT32);
  t->addField("colorPerVertex", VrmlField::SFBOOL);
  t->addField("coordIndex", VrmlField::MFINT32);

  return t;
}

VrmlNodeIndexedSet::VrmlNodeIndexedSet(VrmlScene *scene) :
  VrmlNodeGeometry(scene),
  d_colorPerVertex(true)
{
}

VrmlNodeIndexedSet::~VrmlNodeIndexedSet()
{
}


bool VrmlNodeIndexedSet::isModified() const
{
  return ( d_modified ||
	   (d_color.get() && d_color.get()->isModified()) ||
	   (d_coord.get() && d_coord.get()->isModified()) );
}

void VrmlNodeIndexedSet::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_color.get()) d_color.get()->updateModified(path);
  if (d_coord.get()) d_coord.get()->updateModified(path);
  path.pop_front();
}

void VrmlNodeIndexedSet::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_color.get()) d_color.get()->clearFlags();
  if (d_coord.get()) d_coord.get()->clearFlags();
}

void VrmlNodeIndexedSet::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  if (d_color.get()) d_color.get()->addToScene(s, rel);
  if (d_coord.get()) d_coord.get()->addToScene(s, rel);
}

ostream& VrmlNodeIndexedSet::printFields(ostream& os, int indent)
{
  if (d_color.get()) PRINT_FIELD(color);
  if (d_colorIndex.getLength() > 0) PRINT_FIELD(colorIndex);
  if (! d_colorPerVertex.get()) PRINT_FIELD(colorPerVertex);
  if (d_coord.get()) PRINT_FIELD(coord);
  if (d_coordIndex.getLength() > 0) PRINT_FIELD(coordIndex);
  return os;
}

VrmlNodeColor *VrmlNodeIndexedSet::color()
{
  return d_color.get() ? d_color.get()->toColor() : 0;
}

const VrmlMFInt32 & VrmlNodeIndexedSet::getCoordIndex() const
{   return d_coordIndex; }

const VrmlMFInt32 & VrmlNodeIndexedSet::getColorIndex() const   // LarryD Feb 18/99
{   return d_colorIndex; }

// Get the value of one of the exposedFields

const VrmlField *VrmlNodeIndexedSet::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  else if ( strcmp( fieldName, "coord" ) == 0 )
    return &d_coord;
  
  return VrmlNodeGeometry::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeIndexedSet::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_SFNODE_FIELD(color, Color)
  else if TRY_FIELD(colorIndex, MFInt32)
  else if TRY_FIELD(colorPerVertex, SFBool)
  else if TRY_SFNODE_FIELD(coord, Coordinate)
  else if TRY_FIELD(coordIndex, MFInt32)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
}

/**
 * @brief Get the Color node associated with this Indexed*Set.
 *
 * @return color
 */
const VrmlSFNode & VrmlNodeIndexedSet::getColor() const {
    return this->d_color;
}

/**
 * @brief Set the Color node associated with this Indexed*Set.
 *
 * @param color
 */
void VrmlNodeIndexedSet::setColor(const VrmlSFNode & color) {
    this->d_color = color;
}

/**
 * @brief Get the Coordinate node associated with this Indexed*Set.
 *
 * @return coord
 */
const VrmlSFNode & VrmlNodeIndexedSet::getCoord() const {
    return this->d_coord;
}

/**
 * @brief Set the Coordinate node associated with this Indexed*Set.
 *
 * @param coord
 */
void VrmlNodeIndexedSet::setCoord(const VrmlSFNode & coord) {
    this->d_coord = coord;
}

static VrmlNode * createInline(VrmlScene * scene) {
    return new VrmlNodeInline(scene);
}


// Define the built in VrmlNodeType:: "Inline" fields

VrmlNodeType *VrmlNodeInline::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;
  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Inline", createInline);
      t->reference();
    }

  // Having Inline a subclass of Group is not right since
  // Groups have an exposedField "children" and eventIns
  // addChildren/deleteChildren that Inlines don't support...
  VrmlNodeGroup::defineType(t);	// Parent class
  t->addExposedField("url", VrmlField::MFSTRING);

  return t;
}

VrmlNodeType & VrmlNodeInline::nodeType() const
{
    return *defineType(0);
}


VrmlNodeInline::VrmlNodeInline(VrmlScene *scene) :
  VrmlNodeGroup(scene),
  d_namespace(0),
  d_hasLoaded(false)
{
  this->setBVolumeDirty(true);
}

VrmlNodeInline::~VrmlNodeInline()
{
  delete d_namespace;
}

bool VrmlNodeInline::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeInline* VrmlNodeInline::toInline() const
{ return (VrmlNodeInline*) this; }


// Inlines are loaded during addToScene traversal

void VrmlNodeInline::addToScene(VrmlScene *s, const char *relativeUrl)
{
  d_scene = s;
  load(relativeUrl);
  VrmlNodeGroup::addToScene(s, relativeUrl);
}


ostream& VrmlNodeInline::printFields(ostream& os, int indent)
{
  if ( !FPZERO(d_bboxCenter.getX()) ||
       !FPZERO(d_bboxCenter.getY()) ||
       !FPZERO(d_bboxCenter.getZ()) )
    PRINT_FIELD(bboxCenter);

  if ( !FPEQUAL(d_bboxSize.getX(), -1) ||
       !FPEQUAL(d_bboxSize.getY(), -1) ||
       !FPEQUAL(d_bboxSize.getZ(), -1) )
    PRINT_FIELD(bboxCenter);

  if (d_url.get()) PRINT_FIELD(url);

  return os;
}

// Set the value of one of the exposed fields.

const VrmlField *VrmlNodeInline::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "url" ) == 0 )
    return &d_url;

  return VrmlNodeGroup::getField(fieldName);
}

// Set the value of one of the node fields.

void VrmlNodeInline::setField(const char *fieldName,
			      const VrmlField &fieldValue)
{
  if TRY_FIELD(url, MFString)
  else
    VrmlNodeGroup::setField(fieldName, fieldValue);
}

//  Load the children from the URL

void VrmlNodeInline::load(const char *relativeUrl)
{
  // Already loaded? Need to check whether Url has been modified...
  if (d_hasLoaded) return;

  d_hasLoaded = true;		// although perhaps not successfully
  this->setBVolumeDirty(true);

  if (d_url.getLength() > 0)
    {
      VrmlNamespace *ns = new VrmlNamespace();
      VrmlMFNode *kids = 0;
      Doc2 url;
      int i, n = d_url.getLength();
      for (i=0; i<n; ++i)
	{
	  Doc2 relDoc(relativeUrl);
	  theSystem->debug("Trying to read url '%s' (relative %s)\n",
			   d_url.getElement(i), d_relative.get() ? d_relative.get() : "<null>");
	  url.seturl(d_url.getElement(i), &relDoc);

	  kids = VrmlScene::readWrl( &url, ns );
	  if ( kids )
	    break;
	  else if (i < n-1 && strncmp(d_url.getElement(i),"urn:",4))
	    theSystem->warn("Couldn't read url '%s': %s\n",
			    d_url.getElement(i), strerror( errno));
	}

      if ( kids )
	{
	  delete d_namespace;
	  d_namespace = ns;
	  d_relative.set(url.url()); // children will be relative to this url

	  removeChildren();
	  addChildren( *kids ); 	// check for nested Inlines
	  delete kids;
	}
      else
	{
	  theSystem->warn("couldn't load Inline %s (relative %s)\n",
			  d_url.getElement(0),
			  d_relative.get() ? d_relative.get() : "<null>");
	  delete ns;
	}
    }
}


// Return a new VrmlNodeLOD
static VrmlNode * createLOD(VrmlScene * scene) {
    return new VrmlNodeLOD(scene);
}


// Define the built in VrmlNodeType:: "LOD" fields

VrmlNodeType *VrmlNodeLOD::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("LOD", createLOD);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("level", VrmlField::MFNODE);
  t->addField("center", VrmlField::SFVEC3F);
  t->addField("range", VrmlField::MFFLOAT);

  return t;
}


VrmlNodeType & VrmlNodeLOD::nodeType() const
{
    return *defineType(0);
}


VrmlNodeLOD::VrmlNodeLOD(VrmlScene *scene) :
  VrmlNodeChild(scene)
{
  setBVolumeDirty(true); // lazy calc of bvolume
}

VrmlNodeLOD::~VrmlNodeLOD()
{
}

bool VrmlNodeLOD::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeLOD::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        for (size_t i = 0; i < this->d_level.getLength(); ++i) {
            if (this->d_level.getElement(i)) {
                this->d_level.getElement(i)->resetVisitedFlag();
            }
        }
    }
}

bool VrmlNodeLOD::isModified() const
{
  if (d_modified) return true;
  
  int n = d_level.getLength();

  // This should really check which range is being rendered...
  for (int i = 0; i<n; ++i)
    if (d_level.getElement(i)->isModified())
      return true;

  return false;
}

// what happens if one of the other children suddenly becomes the one
// selected? to be safe: check them all. this potentially means some
// extra work, but it's a lot easier to reason about.
//
void VrmlNodeLOD::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
 int n = d_level.getLength();
  for (int i = 0; i<n; ++i)
    d_level.getElement(i)->updateModified(path);
  path.pop_front();
}

void VrmlNodeLOD::clearFlags()
{
  VrmlNode::clearFlags();
  int n = d_level.getLength();
  for (int i = 0; i<n; ++i)
    d_level.getElement(i)->clearFlags();
}

void VrmlNodeLOD::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  
  int n = d_level.getLength();

  for (int i = 0; i<n; ++i)
    d_level.getElement(i)->addToScene(s, rel);
}

ostream& VrmlNodeLOD::printFields(ostream& os, int indent)
{
  if (d_level.getLength() > 0) PRINT_FIELD(level);
  if (! FPZERO(d_center.getX()) ||
      ! FPZERO(d_center.getY()) ||
      ! FPZERO(d_center.getZ()) )
    PRINT_FIELD(center);
      
  if (d_range.getLength() > 0) PRINT_FIELD(range);

  return os;
}


// Render one of the children

void VrmlNodeLOD::render(Viewer *viewer, VrmlRenderContext rc)
{
  clearModified();
  if (d_level.getLength() <= 0) return;

  float x, y, z;
  viewer->getPosition( &x, &y, &z );

  float dx = x - d_center.getX();
  float dy = y - d_center.getY();
  float dz = z - d_center.getZ();
  float d2 = dx*dx + dy*dy + dz*dz;

  int i, n = d_range.getLength();
  for (i=0; i<n; ++i)
    if (d2 < d_range.getElement(i) * d_range.getElement(i))
      break;

  // Should choose an "optimal" level...
  if (d_range.getLength() == 0) i = d_level.getLength() - 1;

  // Not enough levels...
  if (i >= (int) d_level.getLength()) i = d_level.getLength() - 1;

  //printf("LOD d2 %g level %d\n", d2, i);

  d_level.getElement(i)->render(viewer, rc);

  // Don't re-render on their accounts
  n = d_level.getLength();
  for (i = 0; i<n; ++i)
    d_level.getElement(i)->clearModified();
}

// Get the value of one of the node fields.
const VrmlField *VrmlNodeLOD::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "level" ) == 0 )
    return &d_level;

  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.
void VrmlNodeLOD::setField(const char *fieldName,
			   const VrmlField &fieldValue)
{
  if TRY_FIELD(level, MFNode)
  else if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(range, MFFloat)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
  setBVolumeDirty(true); // lazy calc of bvolume
}

VrmlNodeLOD* VrmlNodeLOD::toLOD() const 
{ return (VrmlNodeLOD*) this; }

const VrmlMFFloat& VrmlNodeLOD::getRange() const  
{   return d_range; }

const VrmlSFVec3f& VrmlNodeLOD::getCenter() const   
{  return d_center; }


const VrmlBVolume* VrmlNodeLOD::getBVolume() const
{
  //cout << "VrmlNodeLOD[" << this << "]::getBVolume()" << endl;
  if (this->isBVolumeDirty())
    ((VrmlNodeLOD*)this)->recalcBSphere();
  return &d_bsphere;
}


void
VrmlNodeLOD::recalcBSphere()
{
  //cout << "VrmlNodeLOD[" << this << "]::recalcBSphere()" << endl;
  d_bsphere.reset();
  
  // let's say our bsphere is the union of the bspheres of all the
  // levels. we could have said it was just the bsphere of the current
  // level, but the current level depends on the viewer position, and
  // we'd like to make the calculation idependent of that. we could do
  // some sort of trick where we reset the bsphere during render, but
  // that seems like overkill unless this simpler method proves to be
  // a bottleneck.
  // 
  // hmm: just thought of a problem: one of the uses of the lod is to
  // switch in delayed-load inlines. this would necessarily switch
  // them in all at once. live with it for now.
  //
  for(int i=0; i<(int) d_level.getLength(); i++) {
    const VrmlBVolume* ci_bv = d_level.getElement(i)->getBVolume();
    d_bsphere.extend(*ci_bv);

  }
  this->setBVolumeDirty(false);
}

/**
 * @brief Get the detail levels.
 *
 * @return level
 */
const VrmlMFNode & VrmlNodeLOD::getLevel() const {
    return this->d_level;
}

/**
 * @brief Set the detail levels.
 *
 * @param level
 */
void VrmlNodeLOD::setLevel(const VrmlMFNode & level) {
    this->d_level = level;
    this->setBVolumeDirty(true);
}

static VrmlNode *createMaterial(VrmlScene * scene) {
    return new VrmlNodeMaterial(scene);
}

// Define the built in VrmlNodeType:: "Material" fields

VrmlNodeType *VrmlNodeMaterial::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Material", createMaterial);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("ambientIntensity", VrmlField::SFFLOAT);
  t->addExposedField("diffuseColor", VrmlField::SFCOLOR);
  t->addExposedField("emissiveColor", VrmlField::SFCOLOR);
  t->addExposedField("shininess", VrmlField::SFFLOAT);
  t->addExposedField("specularColor", VrmlField::SFCOLOR);
  t->addExposedField("transparency", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodeMaterial::nodeType() const
{
    return *defineType(0);
}

VrmlNodeMaterial::VrmlNodeMaterial(VrmlScene *scene) :
  VrmlNode(scene),
  d_ambientIntensity(0.2),
  d_diffuseColor(0.8, 0.8, 0.8),
  d_emissiveColor(0.0, 0.0, 0.0),
  d_shininess(0.2),
  d_specularColor(0.0, 0.0, 0.0),
  d_transparency(0.0)
{
}

VrmlNodeMaterial::~VrmlNodeMaterial()
{
}

bool VrmlNodeMaterial::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeMaterial* VrmlNodeMaterial::toMaterial() const
{ return (VrmlNodeMaterial*) this; }


ostream& VrmlNodeMaterial::printFields(ostream& os, int indent)
{
  if (! FPEQUAL(d_ambientIntensity.get(), 0.2))
    PRINT_FIELD(ambientIntensity);

  if (! FPEQUAL(d_diffuseColor.getR(), 0.8) ||
      ! FPEQUAL(d_diffuseColor.getG(), 0.8) ||
      ! FPEQUAL(d_diffuseColor.getB(), 0.8) )
    PRINT_FIELD(diffuseColor);

  if (! FPZERO(d_emissiveColor.getR()) ||
      ! FPZERO(d_emissiveColor.getG()) ||
      ! FPZERO(d_emissiveColor.getB()) )
    PRINT_FIELD(emissiveColor);

  if (! FPEQUAL(d_shininess.get(), 0.2))
    PRINT_FIELD(shininess);

  if (! FPZERO(d_specularColor.getR()) ||
      ! FPZERO(d_specularColor.getG()) ||
      ! FPZERO(d_specularColor.getB()) )
    PRINT_FIELD(specularColor);

  if (! FPZERO(d_transparency.get()) )
      PRINT_FIELD(transparency);

  return os;
}

// This currently isn't used - see VrmlNodeAppearance.cpp.

void VrmlNodeMaterial::render(Viewer *viewer, VrmlRenderContext rc)
{
  viewer->setMaterial(d_ambientIntensity.get(),
		      d_diffuseColor.get(),
		      d_emissiveColor.get(),
		      d_shininess.get(),
		      d_specularColor.get(),
		      d_transparency.get());
  clearModified();
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeMaterial::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "ambientIntensity" ) == 0 )
    return &d_ambientIntensity;
  else if ( strcmp( fieldName, "diffuseColor" ) == 0 )
    return &d_diffuseColor;
  else if ( strcmp( fieldName, "emissiveColor" ) == 0 )
    return &d_emissiveColor;
  else if ( strcmp( fieldName, "shininess" ) == 0 )
    return &d_shininess;
  else if ( strcmp( fieldName, "specularColor" ) == 0 )
    return &d_specularColor;
  else if ( strcmp( fieldName, "transparency" ) == 0 )
    return &d_transparency;
  
  return VrmlNode::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeMaterial::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_FIELD(ambientIntensity, SFFloat)
  else if TRY_FIELD(diffuseColor, SFColor)
  else if TRY_FIELD(emissiveColor, SFColor)
  else if TRY_FIELD(shininess, SFFloat)
  else if TRY_FIELD(specularColor, SFColor)
  else if TRY_FIELD(transparency, SFFloat)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

static VrmlNode * createMovieTexture(VrmlScene * scene) {
    return new VrmlNodeMovieTexture(scene);
}


// Define the built in VrmlNodeType:: "MovieTexture" fields

VrmlNodeType *VrmlNodeMovieTexture::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("MovieTexture", createMovieTexture);
      t->reference();
    }

  VrmlNodeTexture::defineType(t);	// Parent class

  t->addExposedField("loop", VrmlField::SFBOOL);
  t->addExposedField("speed", VrmlField::SFFLOAT);
  t->addExposedField("startTime", VrmlField::SFTIME);
  t->addExposedField("stopTime", VrmlField::SFTIME);
  t->addExposedField("url", VrmlField::MFSTRING);
  t->addField("repeatS", VrmlField::SFBOOL);
  t->addField("repeatT", VrmlField::SFBOOL);
  t->addEventOut("duration_changed", VrmlField::SFTIME);
  t->addEventOut("isActive", VrmlField::SFBOOL);

  return t;
}


VrmlNodeType & VrmlNodeMovieTexture::nodeType() const
{
    return *defineType(0);
}


VrmlNodeMovieTexture::VrmlNodeMovieTexture(VrmlScene *scene) :
  VrmlNodeTexture(scene),
  d_loop(false),
  d_speed(1.0),
  d_repeatS(true),
  d_repeatT(true),
  d_image(0),
  d_frame(0),
  d_lastFrame(-1),
  d_lastFrameTime(-1.0),
  d_texObject(0)
{
  if (d_scene) d_scene->addMovie( this );
}


VrmlNodeMovieTexture::~VrmlNodeMovieTexture()
{
  //if (d_texObject) d_viewer->removeTextureObject(d_texObject);
  if (d_scene) d_scene->removeMovie( this );
  delete d_image;
}

bool VrmlNodeMovieTexture::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeMovieTexture* VrmlNodeMovieTexture::toMovieTexture() const
{ return (VrmlNodeMovieTexture*) this; }


void VrmlNodeMovieTexture::addToScene(VrmlScene *s, const char *rel)
{
  if (d_scene != s && (d_scene = s) != 0) d_scene->addMovie(this);
  VrmlNodeTexture::addToScene(s, rel);
}


ostream& VrmlNodeMovieTexture::printFields(ostream& os, int indent)
{
  if (d_loop.get()) PRINT_FIELD(loop);
  if (!FPEQUAL(d_speed.get(), 1.0)) PRINT_FIELD(speed);
  if (!FPZERO(d_startTime.get())) PRINT_FIELD(startTime);
  if (!FPZERO(d_stopTime.get())) PRINT_FIELD(stopTime);
  if (d_url.get()) PRINT_FIELD(url);
  if (! d_repeatS.get()) PRINT_FIELD(repeatS);
  if (! d_repeatT.get()) PRINT_FIELD(repeatT);
  return os;
}


void VrmlNodeMovieTexture::update( VrmlSFTime &timeNow )
{
  if ( isModified() )
    {
      if (d_image)
	{
	  const char *imageUrl = d_image->url();
	  int imageLen = strlen(imageUrl);
	  int i, nUrls = d_url.getLength();
	  for (i=0; i<nUrls; ++i)
	    {
	      int len = strlen(d_url.getElement(i));
	      
	      if ((strcmp(imageUrl, d_url.getElement(i)) == 0) ||
		  (imageLen > len &&
		   strcmp(imageUrl+imageLen-len, d_url.getElement(i)) == 0))
		break;
	    }

	  // if (d_image->url() not in d_url list) ...
	  if (i == nUrls)
	    {
	      delete d_image;
	      d_image = 0;
	    }
	}
    }

  // Load the movie if needed (should check startTime...)
  if (! d_image && d_url.getLength() > 0)
    {
      Doc2 relDoc(d_relativeUrl.get());
      Doc2 * rel = d_relativeUrl.get() ? &relDoc : d_scene->urlDoc();
      d_image = new Image;
      if ( ! d_image->tryURLs( d_url.getLength(), d_url.get(), rel ) )
	cerr << "Error: couldn't read MovieTexture from URL " << d_url << endl;


      int nFrames = d_image->nFrames();
      d_duration = VrmlSFTime((nFrames >= 0) ? double(nFrames) : double(-1));
      eventOut( timeNow.get(), "duration_changed", d_duration );
      d_frame = (d_speed.get() >= 0) ? 0 : nFrames-1;

      //theSystem->debug("MovieTexture.%s loaded %d frames\n", name(), nFrames);
    }

  // No pictures to show
  if (! d_image || d_image->nFrames() == 0) return;

  // Become active at the first tick at or after startTime if either
  // the valid stopTime hasn't passed or we are looping.
  if (! d_isActive.get() &&
      d_startTime.get() <= timeNow.get() &&
      d_startTime.get() >= d_lastFrameTime &&
      ( (d_stopTime.get() < d_startTime.get() || // valid stopTime
	 d_stopTime.get() > timeNow.get()) ||    // hasn't passed
	d_loop.get() ))
    {
      //theSystem->debug("MovieTexture.%s::isActive TRUE\n", name());
      d_isActive.set(true);
      eventOut( timeNow.get(), "isActive", d_isActive );
      d_lastFrameTime = timeNow.get();
      d_frame = (d_speed.get() >= 0) ? 0 : d_image->nFrames() - 1;
      setModified();
    }

  // Check whether stopTime has passed
  else if ( d_isActive.get() &&
	    (( d_stopTime.get() > d_startTime.get() &&
	       d_stopTime.get() <= timeNow.get() ) ||
	     d_frame < 0))
    {
      //theSystem->debug("MovieTexture.%s::isActive FALSE\n", name());
      d_isActive.set(false);
      eventOut( timeNow.get(), "isActive", d_isActive );
      setModified();
    }

  // Check whether the frame should be advanced
  else if ( d_isActive.get() &&
	    d_lastFrameTime + fabs(1/d_speed.get()) <= timeNow.get() )
    {
      if (d_speed.get() < 0.0)
	--d_frame;
      else
	++d_frame;
      //theSystem->debug("MovieTexture.%s::frame %d\n", name(), d_frame);
      d_lastFrameTime = timeNow.get();
      setModified();
    }

  // Tell the scene when the next update is needed.
  if (d_isActive.get())
    {
      double d = d_lastFrameTime + fabs(1/d_speed.get()) - timeNow.get();
      d_scene->setDelta( 0.9 * d );
    }

}

// Ignore set_speed when active.

void VrmlNodeMovieTexture::eventIn(double timeStamp,
				   const char *eventName,
				   const VrmlField & fieldValue)
{
  const char *origEventName = eventName;
  if ( strncmp(eventName, "set_", 4) == 0 )
    eventName += 4;

  // Ignore set_speed when active
  if ( strcmp(eventName,"speed") == 0 )
    {
      if (! d_isActive.get())
	{
	  setField(eventName, fieldValue);
	  eventOut(timeStamp, "speed_changed", fieldValue);
	  setModified();
	}
    }

  // Let the generic code handle the rest.
  else
    VrmlNode::eventIn( timeStamp, origEventName, fieldValue );
}


// Render a frame if there is one available.

void VrmlNodeMovieTexture::render(Viewer *viewer, VrmlRenderContext rc)
{
  //theSystem->debug("MovieTexture.%s::render frame %d\n", name(), d_frame);

  if ( ! d_image || d_frame < 0 ) return;

  unsigned char *pix = d_image->pixels( d_frame );

  if ( d_frame != d_lastFrame && d_texObject )
    {
      viewer->removeTextureObject( d_texObject );
      d_texObject = 0;
    }

  if ( ! pix )
    {
      d_frame = -1;
    }
  else if ( d_texObject )
    {
      viewer->insertTextureReference( d_texObject, d_image->nc() );
    }
  else
    {
      // Ensure image dimensions are powers of 2 (move to VrmlNodeTexture...)
      int sizes[] = { 2, 4, 8, 16, 32, 64, 128, 256 };
      int nSizes = sizeof(sizes) / sizeof(int);
      int w = d_image->w();
      int h = d_image->h();
      int i, j;
      for (i=0; i<nSizes; ++i)
	if (w < sizes[i]) break;
      for (j=0; j<nSizes; ++j)
	if (h < sizes[j]) break;

      if (i > 0 && j > 0)
	{
	  // Always scale images down in size and reuse the same pixel memory.
	  if (w != sizes[i-1] || h != sizes[j-1])
	    {
	      viewer->scaleTexture( w, h, sizes[i-1], sizes[j-1],
				    d_image->nc(), pix );
	      d_image->setSize( sizes[i-1], sizes[j-1] );
	    }

	  d_texObject = viewer->insertTexture(d_image->w(),
					      d_image->h(),
					      d_image->nc(),
					      d_repeatS.get(),
					      d_repeatT.get(),
					      pix,
					      ! d_isActive.get() );
	}
    }

  d_lastFrame = d_frame;
  clearModified();
}


size_t VrmlNodeMovieTexture::nComponents()
{
  return d_image ? d_image->nc() : 0;
}

size_t VrmlNodeMovieTexture::width()
{
  return d_image ? d_image->w() : 0;
}

size_t VrmlNodeMovieTexture::height()
{
  return d_image ? d_image->h() : 0;
}

size_t VrmlNodeMovieTexture::nFrames()
{
  return d_image ? d_image->nFrames() : 0;
}

const unsigned char * VrmlNodeMovieTexture::pixels()
{
  return d_image ? d_image->pixels() : 0;
}



// Get the value of a field or eventOut.

const VrmlField *VrmlNodeMovieTexture::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "loop" ) == 0 )
    return &d_loop;
  else if ( strcmp( fieldName, "speed" ) == 0 )
    return &d_speed;
  else if ( strcmp( fieldName, "startTime" ) == 0 )
    return &d_startTime;
  else if ( strcmp( fieldName, "stopTime" ) == 0 )
    return &d_stopTime;
  else if ( strcmp( fieldName, "url" ) == 0 )
    return &d_url;

  // eventOuts
  else if ( strcmp( fieldName, "duration" ) == 0 )
    return &d_duration;
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  
  return VrmlNode::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeMovieTexture::setField(const char *fieldName,
				    const VrmlField &fieldValue)
{
  if TRY_FIELD(loop, SFBool)
  else if TRY_FIELD(speed, SFFloat)
  else if TRY_FIELD(startTime, SFTime)
  else if TRY_FIELD(stopTime, SFTime)
  else if TRY_FIELD(url, MFString)
  else if TRY_FIELD(repeatS, SFBool)
  else if TRY_FIELD(repeatT, SFBool)
  else
    VrmlNode::setField(fieldName, fieldValue);
}


//  NavigationInfo factory.
//  Since NavInfo is a bindable child node, the first one created needs
//  to notify its containing scene.

static VrmlNode * createNavigationInfo(VrmlScene * scene) {
    return new VrmlNodeNavigationInfo(scene);
}


// Define the built in VrmlNodeType:: "NavigationInfo" fields

VrmlNodeType *VrmlNodeNavigationInfo::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("NavigationInfo", createNavigationInfo);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_bind", VrmlField::SFBOOL);
  t->addExposedField("avatarSize", VrmlField::MFFLOAT);
  t->addExposedField("headlight", VrmlField::SFBOOL);
  t->addExposedField("speed", VrmlField::SFFLOAT);
  t->addExposedField("type", VrmlField::MFSTRING);
  t->addExposedField("visibilityLimit", VrmlField::SFFLOAT);
  t->addEventOut("isBound", VrmlField::SFBOOL);

  return t;
}


VrmlNodeType & VrmlNodeNavigationInfo::nodeType() const
{
    return *defineType(0);
}


VrmlNodeNavigationInfo::VrmlNodeNavigationInfo(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_headlight( true ),
  d_speed( 1.0 ),
  d_visibilityLimit( 0.0 ),
  d_isBound(false)
{
  float avatarSize[] = { 0.25, 1.6, 0.75 };
  char *type[] = { "WALK", "ANY" };

  d_avatarSize.set( 3, avatarSize );
  d_type.set( 2, type );
  if (d_scene) d_scene->addNavigationInfo(this);
}

VrmlNodeNavigationInfo::~VrmlNodeNavigationInfo()
{
  if (d_scene) d_scene->removeNavigationInfo(this);
}

bool VrmlNodeNavigationInfo::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeNavigationInfo* VrmlNodeNavigationInfo::toNavigationInfo() const
{ return (VrmlNodeNavigationInfo*) this; }


void VrmlNodeNavigationInfo::addToScene(VrmlScene *s, const char *)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addNavigationInfo(this); }


ostream& VrmlNodeNavigationInfo::printFields(ostream& os, int indent)
{
  if (d_avatarSize.getLength() != 3 ||
      ! FPEQUAL(d_avatarSize.getElement(0), 0.25) ||
      ! FPEQUAL(d_avatarSize.getElement(1), 1.6) ||
      ! FPEQUAL(d_avatarSize.getElement(2), 0.75) )
    PRINT_FIELD(avatarSize);
  if (! d_headlight.get()) PRINT_FIELD(headlight);
  if (! FPEQUAL(d_speed.get(), 1.0)) PRINT_FIELD(speed);
  if (d_type.getLength() != 2 ||
      strcmp(d_type.getElement(0), "WALK") != 0 ||
      strcmp(d_type.getElement(1), "ANY") != 0 )
    PRINT_FIELD(type);
  if (! FPZERO(d_visibilityLimit.get())) PRINT_FIELD(visibilityLimit);

  return os;
}

// Note that this method is not maintaining isBound.
// I can't find a good way to ensure that the isBound
// var of the current node gets updated at the right time...

void VrmlNodeNavigationInfo::eventIn(double timeStamp,
				     const char *eventName,
				     const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_bind") == 0)
    {
      VrmlNodeNavigationInfo *current = d_scene->bindableNavigationInfoTop();
      const VrmlSFBool * const b =
                dynamic_cast<const VrmlSFBool *>(&fieldValue);
      if (! b)
	{
	  cerr << "Error: invalid value for NavigationInfo::set_bind eventIn "
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
	      current = d_scene->bindableNavigationInfoTop();
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

const VrmlField *VrmlNodeNavigationInfo::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "avatarSize" ) == 0 )
    return &d_avatarSize;
  else if ( strcmp( fieldName, "headlight" ) == 0 )
    return &d_headlight;
  else if ( strcmp( fieldName, "speed" ) == 0 )
    return &d_speed;
  else if ( strcmp( fieldName, "type" ) == 0 )
    return &d_type;
  else if ( strcmp( fieldName, "visibilityLimit" ) == 0 )
    return &d_visibilityLimit;
  
  // eventOuts
  else if ( strcmp( fieldName, "isBound" ) == 0 )
    {
      VrmlSFBool* isBound = (VrmlSFBool*) &(this->d_isBound);
      isBound->set( d_scene->bindableNavigationInfoTop() == this );
      return isBound;
    }

  return VrmlNodeChild::getField(fieldName); // Parent class
}


// Set the value of one of the node fields.

void VrmlNodeNavigationInfo::setField(const char *fieldName,
				      const VrmlField &fieldValue)
{
  if TRY_FIELD(avatarSize, MFFloat)
  else if TRY_FIELD(headlight, SFBool)
  else if TRY_FIELD(speed, SFFloat)
  else if TRY_FIELD(type, MFString)
  else if TRY_FIELD(visibilityLimit, SFFloat)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


static VrmlNode * createNormal(VrmlScene * scene) {
    return new VrmlNodeNormal(scene);
}


// Define the built in VrmlNodeType:: "Normal" fields

VrmlNodeType *VrmlNodeNormal::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Normal", createNormal);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("vector", VrmlField::MFVEC3F);

  return t;
}


VrmlNodeType & VrmlNodeNormal::nodeType() const
{
    return *defineType(0);
}


VrmlNodeNormal::VrmlNodeNormal(VrmlScene *scene) : VrmlNode(scene)
{
}

VrmlNodeNormal::~VrmlNodeNormal()
{
}

bool VrmlNodeNormal::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeNormal* VrmlNodeNormal::toNormal() const
{ return (VrmlNodeNormal*) this; }


ostream& VrmlNodeNormal::printFields(ostream& os, int indent)
{
  if (d_vector.getLength() > 0) PRINT_FIELD(vector);
  return os;
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeNormal::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "vector" ) == 0 )
    return &d_vector;

  return VrmlNode::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeNormal::setField(const char *fieldName,
			      const VrmlField &fieldValue)
{
  if TRY_FIELD(vector, MFVec3f)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

// NormalInt factory.

static VrmlNode * createNormalInt(VrmlScene * scene) {
    return new VrmlNodeNormalInt(scene);
}


// Define the built in VrmlNodeType:: "NormalInterpolator" fields

VrmlNodeType *VrmlNodeNormalInt::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("NormalInterpolator", createNormalInt);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_fraction", VrmlField::SFFLOAT);
  t->addExposedField("key", VrmlField::MFFLOAT);
  t->addExposedField("keyValue", VrmlField::MFVEC3F);
  t->addEventOut("value_changed", VrmlField::MFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeNormalInt::nodeType() const
{
    return *defineType(0);
}


VrmlNodeNormalInt::VrmlNodeNormalInt( VrmlScene *scene ) :
  VrmlNodeChild(scene)
{
}

VrmlNodeNormalInt::~VrmlNodeNormalInt()
{
}

bool VrmlNodeNormalInt::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeNormalInt::printFields(ostream& os, int indent)
{
  if (d_key.getLength() > 0) PRINT_FIELD(key);
  if (d_keyValue.getLength() > 0) PRINT_FIELD(keyValue);

  return os;
}


void VrmlNodeNormalInt::eventIn(double timeStamp,
				const char *eventName,
				const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_fraction") == 0)
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
	{
	  theSystem->error
	    ("Invalid type for %s eventIn %s (expected SFFloat).\n",
		nodeType().getName(), eventName);
	  return;
	}
      float f = static_cast<const VrmlSFFloat &>(fieldValue).get();

      int nNormals = d_keyValue.getLength() / d_key.getLength();
      int n = d_key.getLength() - 1;

      if (f < d_key.getElement(0))
	{
          d_value.set(nNormals, d_keyValue.getElement(0));
	}
      else if (f > d_key.getElement(n))
	{
	  d_value.set(nNormals, d_keyValue.getElement(n * nNormals));
	}
      else
	{
	  // Reserve enough space for the new value
	  d_value.set( nNormals, 0 );

	  for (int i=0; i<n; ++i)
	    if (d_key.getElement(i) <= f && f <= d_key.getElement(i + 1))
	      {
		const float * v1 = d_keyValue.getElement(i * nNormals);
		const float * v2 = d_keyValue.getElement((i + 1) * nNormals);

		f = (f - d_key.getElement(i)) / (d_key.getElement(i + 1) - d_key.getElement(i));

		// Interpolate on the surface of unit sphere.
		// Contributed by S. K. Bose. (bose@garuda.barc.ernet.in)
		for (int j=0; j<nNormals; ++j)
		  {
                    float alpha, beta;
                    float dotval = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
                    if ((dotval+1.0) > FPTOLERANCE) // Vectors are opposite
		      {
			if ((1.0-dotval) > FPTOLERANCE)  // Vectors coincide
			  {
			    float omega = acos(dotval);
			    float sinomega = sin(omega);
			    alpha = sin((1.0 - f) * omega)/sinomega;
			    beta = sin(f*omega)/sinomega;
			  }
			else
			  {
			    // Do linear interpolation...
			    alpha = 1.0 - f;
			    beta = f;
			  }
		      }
                    else
		      {                    // Do linear interpolation...
			alpha = 1.0 -f;
			beta = f;
		      }
                    const float vec[3] = { alpha * v1[0] + beta * v2[0],
                                           alpha * v1[1] + beta * v2[1],
                                           alpha * v1[2] + beta * v2[2] };
                    this->d_value.setElement(j, vec);

		    v1 += 3;
		    v2 += 3;
		  }

		break;
	      }
	}

      // Send the new value
      eventOut(timeStamp, "value_changed", d_value);
    }

  // Check exposedFields
  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);

      // This node is not renderable, so don't re-render on changes to it.
      clearModified();
    }
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeNormalInt::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "key" ) == 0 )
    return &d_key;
  else if ( strcmp( fieldName, "keyValue" ) == 0 )
    return &d_keyValue;

  // eventOuts
  else if ( strcmp( fieldName, "value" ) == 0 )
    return &d_value;

  return VrmlNodeChild::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeNormalInt::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFVec3f)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

// OrientationInt factory.

static VrmlNode * createOrientationInt(VrmlScene * scene) {
    return new VrmlNodeOrientationInt(scene);
}


// Define the built in VrmlNodeType:: "OrientationInt" fields

VrmlNodeType *VrmlNodeOrientationInt::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("OrientationInterpolator", createOrientationInt);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_fraction", VrmlField::SFFLOAT);
  t->addExposedField("key", VrmlField::MFFLOAT);
  t->addExposedField("keyValue", VrmlField::MFROTATION);
  t->addEventOut("value_changed", VrmlField::SFROTATION);

  return t;
}

VrmlNodeType & VrmlNodeOrientationInt::nodeType() const
{
    return *defineType(0);
}


VrmlNodeOrientationInt::VrmlNodeOrientationInt( VrmlScene *scene ) :
  VrmlNodeChild(scene)
{
}

VrmlNodeOrientationInt::~VrmlNodeOrientationInt()
{
}

bool VrmlNodeOrientationInt::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeOrientationInt::printFields(ostream& os, int indent)
{
  if (d_key.getLength() > 0) PRINT_FIELD(key);
  if (d_keyValue.getLength() > 0) PRINT_FIELD(keyValue);

  return os;
}


void VrmlNodeOrientationInt::eventIn(double timeStamp,
				     const char *eventName,
				     const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_fraction") == 0)
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
	{
	  theSystem->error
	    ("Invalid type for %s eventIn %s (expected SFFloat).\n",
		nodeType().getName(), eventName);
	  return;
	}
      float f = static_cast<const VrmlSFFloat &>(fieldValue).get();

      //printf("OI.set_fraction %g ", f);

      int n = d_key.getLength() - 1;
      if (f < d_key.getElement(0))
	{
	  const float * v0 = d_keyValue.getElement(0);
	  //printf(" 0 [%g %g %g %g]\n", v0[0], v0[1], v0[2], v0[3] );
	  d_value.set(v0);
	}
      else if (f > d_key.getElement(n))
	{
	  const float * vn = d_keyValue.getElement(n);
	  //printf(" n [%g %g %g %g]\n", vn[0], vn[1], vn[2], vn[3] );
	  d_value.set(vn);
	}
      else
	{
	  for (int i=0; i<n; ++i)
	    if (d_key.getElement(i) <= f && f <= d_key.getElement(i + 1))
	      {
		const float * v1 = d_keyValue.getElement(i);
		const float * v2 = d_keyValue.getElement(i + 1);

		// Interpolation factor
		f = (f - d_key.getElement(i)) / (d_key.getElement(i + 1) - d_key.getElement(i));

		float x, y, z, r1, r2;
		r1 = v1[3];

		// Make sure the vectors are not pointing opposite ways
		if (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] < 0.0)
		  {
		    x = v1[0] + f * (-v2[0] - v1[0]);
		    y = v1[1] + f * (-v2[1] - v1[1]);
		    z = v1[2] + f * (-v2[2] - v1[2]);
		    r2 = -v2[3];
		  }
		else
		  {
		    x = v1[0] + f * (v2[0] - v1[0]);
		    y = v1[1] + f * (v2[1] - v1[1]);
		    z = v1[2] + f * (v2[2] - v1[2]);
		    r2 = v2[3];
		  }

		// Interpolate angles via the shortest direction
		if (fabs(r2 - r1) > M_PI)
		  {
		    if (r2 > r1) r1 += 2.0 * M_PI;
		    else         r2 += 2.0 * M_PI;
		  }
		float angle = r1 + f * (r2 - r1);
		if (angle >= 2.0 * M_PI) angle -= 2.0 * M_PI;
		else if (angle < 0.0)    angle += 2.0 * M_PI;
		
		//printf(" %g between (%d,%d) [%g %g %g %g]\n", f, i, i+1,
		//x, y, z, r);
                const float rotation[4] = { x, y, z, angle };
		d_value.set(rotation);
		break;
	      }
	}

      // Send the new value
      eventOut(timeStamp, "value_changed", d_value);
    }

  // Check exposedFields
  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);

      // This node is not renderable, so don't re-render on changes to it.
      clearModified();
    }
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeOrientationInt::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "key" ) == 0 )
    return &d_key;
  else if ( strcmp( fieldName, "keyValue" ) == 0 )
    return &d_keyValue;

  // eventOuts
  else if ( strcmp( fieldName, "value" ) == 0 )
    return &d_value;

  return VrmlNodeChild::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeOrientationInt::setField(const char *fieldName,
				      const VrmlField &fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFRotation)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

VrmlNodeOrientationInt* VrmlNodeOrientationInt::toOrientationInt() const 
{ return (VrmlNodeOrientationInt*) this; }

const VrmlMFFloat& VrmlNodeOrientationInt::getKey() const  
{   return d_key; }

const VrmlMFRotation& VrmlNodeOrientationInt::getKeyValue() const  
{ return d_keyValue; }


static VrmlNode * createPixelTexture(VrmlScene * scene) {
    return new VrmlNodePixelTexture(scene); 
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
      t = st = new VrmlNodeType("PixelTexture", createPixelTexture);
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

bool VrmlNodePixelTexture::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
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

// PlaneSensor factory. 

static VrmlNode * createPlaneSensor(VrmlScene * scene) {
    return new VrmlNodePlaneSensor(scene);
}


// Define the built in VrmlNodeType:: "PlaneSensor" fields

VrmlNodeType *VrmlNodePlaneSensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("PlaneSensor", createPlaneSensor);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("autoOffset", VrmlField::SFBOOL);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("maxPosition", VrmlField::SFVEC2F);
  t->addExposedField("minPosition", VrmlField::SFVEC2F);
  t->addExposedField("offset", VrmlField::SFVEC3F);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("translation_changed", VrmlField::SFVEC3F);
  t->addEventOut("trackPoint_changed", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodePlaneSensor::nodeType() const 
{
    return *defineType(0);
}


VrmlNodePlaneSensor::VrmlNodePlaneSensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_autoOffset(true),
  d_enabled(true),
  d_maxPosition(-1.0, -1.0),
  d_isActive(false),
  d_parentTransform(0)
{
  setModified();
  Midentity(d_activationMatrix);
}

// need copy constructor for d_parentTransform ...

VrmlNodePlaneSensor::~VrmlNodePlaneSensor()
{
}

bool VrmlNodePlaneSensor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodePlaneSensor* VrmlNodePlaneSensor::toPlaneSensor() const
{ return (VrmlNodePlaneSensor*) this; }


ostream& VrmlNodePlaneSensor::printFields(ostream& os, int indent)
{
  if (! d_autoOffset.get()) PRINT_FIELD(autoOffset);
  if (! d_enabled.get())    PRINT_FIELD(enabled);
  if (! FPEQUAL(d_maxPosition.getX(), -1.0) ||
      ! FPEQUAL(d_maxPosition.getY(), -1.0))
    PRINT_FIELD(maxPosition);
  if (! FPEQUAL(d_minPosition.getX(), -1.0) ||
      ! FPEQUAL(d_minPosition.getY(), -1.0))
    PRINT_FIELD(minPosition);

  if (! FPZERO(d_offset.getX()) ||
      ! FPZERO(d_offset.getY()) ||
      ! FPZERO(d_offset.getZ()) )
    PRINT_FIELD(offset);

  return os;
}

// Cache a pointer to (one of the) parent transforms for converting
// hits into local coords.

void VrmlNodePlaneSensor::accumulateTransform( VrmlNode *parent )
{
  d_parentTransform = parent;
}

VrmlNode* VrmlNodePlaneSensor::getParentTransform() { return d_parentTransform; }


// This is not correct. The local coords are computed for one instance,
// need to convert p to local coords for each instance (DEF/USE) of the 
// sensor...

void VrmlNodePlaneSensor::activate( double timeStamp,
				    bool isActive,
				    double *p )
{
  // Become active
  if ( isActive && ! d_isActive.get() )
    {
      d_isActive.set(isActive);

      float V[3] = { p[0], p[1], p[2] };
      //double M[4][4];
      inverseTransform( d_activationMatrix );
      VM( V, d_activationMatrix, V );
      d_activationPoint.set(V);
#if 0
      theSystem->warn(" planesensor: activate at (%g %g %g)\n",
		      p[0],p[1],p[2]);
      theSystem->warn(" planesensor: local coord (%g %g %g)\n",
		      V[0],V[1],V[2]);
#endif
      eventOut( timeStamp, "isActive", d_isActive );
    }

  // Become inactive
  else if ( ! isActive && d_isActive.get() )
    {
#if 0
      theSystem->warn(" planesensor: deactivate\n");
#endif
      d_isActive.set(isActive);
      eventOut( timeStamp, "isActive", d_isActive );

      // auto offset
      if ( d_autoOffset.get() )
	{
	  d_offset = d_translation;
	  eventOut( timeStamp, "offset_changed", d_offset );
	}
    }

  // Tracking
  else if ( isActive )
    {
      float V[3] = { p[0], p[1], p[2] };
      //double M[4][4];
      //inverseTransform( M );
      VM( V, d_activationMatrix, V );
      d_trackPoint.set(V);
      eventOut( timeStamp, "trackPoint_changed", d_trackPoint );

      float t[3];
      t[0] = V[0] - d_activationPoint.getX() + d_offset.getX();
      t[1] = V[1] - d_activationPoint.getY() + d_offset.getY();
      t[2] = 0.0;

      if ( d_minPosition.getX() == d_maxPosition.getX() )
	t[0] = d_minPosition.getX();
      else if ( d_minPosition.getX() < d_maxPosition.getX() )
	{
	  if (t[0] < d_minPosition.getX())
	    t[0] = d_minPosition.getX();
	  else if (t[0] > d_maxPosition.getX())
	    t[0] = d_maxPosition.getX();
	}

      if ( d_minPosition.getY() == d_maxPosition.getY() )
	t[1] = d_minPosition.getY();
      else if ( d_minPosition.getY() < d_maxPosition.getY() )
	{
	  if (t[1] < d_minPosition.getY())
	    t[1] = d_minPosition.getY();
	  else if (t[1] > d_maxPosition.getY())
	    t[1] = d_maxPosition.getY();
	}

      d_translation.set(t);
      eventOut( timeStamp, "translation_changed", d_translation );
    }
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodePlaneSensor::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "autoOffset" ) == 0 )
    return &d_autoOffset;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  
  // eventOuts
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  else if ( strcmp( fieldName, "translation" ) == 0 )
    return &d_translation;
  else if ( strcmp( fieldName, "trackPoint" ) == 0 )
    return &d_trackPoint;
  
  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodePlaneSensor::setField(const char *fieldName,
				   const VrmlField &fieldValue)
{
  if TRY_FIELD(autoOffset, SFBool)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(maxPosition, SFVec2f)
  else if TRY_FIELD(minPosition, SFVec2f)
  else if TRY_FIELD(offset, SFVec3f)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


// Return a new VrmlNodePointLight
static VrmlNode * createPointLight(VrmlScene * scene) {
    return new VrmlNodePointLight(scene);
}


// Define the built in VrmlNodeType:: "PointLight" fields

VrmlNodeType *VrmlNodePointLight::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("PointLight", createPointLight);
      t->reference();
    }

  VrmlNodeLight::defineType(t);	// Parent class
  t->addExposedField("attenuation", VrmlField::SFVEC3F);
  t->addExposedField("location", VrmlField::SFVEC3F);
  t->addExposedField("radius", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodePointLight::nodeType() const
{
    return *defineType(0);
}


VrmlNodePointLight::VrmlNodePointLight(VrmlScene *scene) :
  VrmlNodeLight(scene),
  d_attenuation(1.0, 0.0, 0.0),
  d_location(0.0, 0.0, 0.0),
  d_radius(100)
{
  if (d_scene) d_scene->addScopedLight(this);
}

VrmlNodePointLight::~VrmlNodePointLight()
{
  if (d_scene) d_scene->removeScopedLight(this);
}

bool VrmlNodePointLight::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodePointLight* VrmlNodePointLight::toPointLight() const
{ return (VrmlNodePointLight*) this; }

void VrmlNodePointLight::addToScene(VrmlScene *s, const char *)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addScopedLight(this); }


ostream& VrmlNodePointLight::printFields(ostream& os, int indent)
{
  VrmlNodeLight::printFields(os, indent);
  if (! FPEQUAL(d_attenuation.getX(), 1.0) ||
      ! FPZERO(d_attenuation.getY()) ||
      ! FPZERO(d_attenuation.getZ()) )
    PRINT_FIELD(attenuation);
  if (! FPZERO(d_location.getX()) ||
      ! FPZERO(d_location.getY()) ||
      ! FPZERO(d_location.getZ()) )
    PRINT_FIELD(location);
  if (! FPEQUAL(d_radius.get(), 100.0))
    PRINT_FIELD(radius);

  return os;
}

// This should be called before rendering any geometry nodes in the scene.
// Since this is called from Scene::render() before traversing the
// scene graph, the proper transformation matrix hasn't been set up.
// Somehow it needs to figure out the accumulated xforms of its
// parents and apply them before rendering. This is not easy with
// DEF/USEd nodes...

void VrmlNodePointLight::renderScoped(Viewer *viewer)
{
  if ( d_on.get() && d_radius.get() > 0.0 )
    viewer->insertPointLight( d_ambientIntensity.get(),
			      d_attenuation.get(),
			      d_color.get(),
			      d_intensity.get(),
			      d_location.get(),
			      d_radius.get() );
  clearModified();
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodePointLight::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "attenuation" ) == 0 )
    return &d_attenuation;
  else if ( strcmp( fieldName, "location" ) == 0 )
    return &d_location;
  else if ( strcmp( fieldName, "radius" ) == 0 )
    return &d_radius;

  return VrmlNodeLight::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodePointLight::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_FIELD(attenuation, SFVec3f)
  else if TRY_FIELD(location, SFVec3f)
  else if TRY_FIELD(radius, SFFloat)
  else
    VrmlNodeLight::setField(fieldName, fieldValue);
}

const VrmlSFVec3f& VrmlNodePointLight::getAttenuation() const   // LarryD Mar 04/99
{  return d_attenuation; }

const VrmlSFVec3f& VrmlNodePointLight::getLocation() const   // LarryD Mar 04/99
{  return d_location; }

static VrmlNode * createPointSet(VrmlScene * scene) {
    return new VrmlNodePointSet(scene);
}


// Define the built in VrmlNodeType:: "PointSet" fields

VrmlNodeType *VrmlNodePointSet::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("PointSet", createPointSet);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class
  t->addExposedField("color", VrmlField::SFNODE);
  t->addExposedField("coord", VrmlField::SFNODE);

  return t;
}

VrmlNodeType & VrmlNodePointSet::nodeType() const
{
    return *defineType(0);
}


VrmlNodePointSet::VrmlNodePointSet(VrmlScene *scene) :
  VrmlNodeGeometry(scene)
{
  this->setBVolumeDirty(true);
}

VrmlNodePointSet::~VrmlNodePointSet()
{
}

bool VrmlNodePointSet::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodePointSet::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_color.get()) {
            this->d_color.get()->resetVisitedFlag();
        }
        if (this->d_coord.get()) {
            this->d_coord.get()->resetVisitedFlag();
        }
    }
}

bool VrmlNodePointSet::isModified() const
{
  return ( d_modified ||
	   (d_color.get() && d_color.get()->isModified()) ||
	   (d_coord.get() && d_coord.get()->isModified()) );
}

void VrmlNodePointSet::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_color.get()) d_color.get()->updateModified(path);
  if (d_coord.get()) d_coord.get()->updateModified(path);
  path.pop_front();
}



void VrmlNodePointSet::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_color.get()) d_color.get()->clearFlags();
  if (d_coord.get()) d_coord.get()->clearFlags();
}

void VrmlNodePointSet::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  if (d_color.get()) d_color.get()->addToScene(s, rel);
  if (d_coord.get()) d_coord.get()->addToScene(s, rel);
}

ostream& VrmlNodePointSet::printFields(ostream& os, int indent)
{
  if (d_color.get()) PRINT_FIELD(color);
  if (d_coord.get()) PRINT_FIELD(coord);

  return os;
}


Viewer::Object VrmlNodePointSet::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  Viewer::Object obj = 0;

  if (rc.getDrawBSpheres()) {
    const VrmlBSphere* bs = (const VrmlBSphere*)this->getBVolume();
    viewer->drawBSphere(*bs, 4);
  }

  if ( d_coord.get() )
    {
      const float * color = 0;
      if ( d_color.get() )
	{
	  VrmlMFColor &c = d_color.get()->toColor()->color();
	  color = &c.getElement(0)[0];
	}

      VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();

      obj = viewer->insertPointSet(coord.getLength(), &coord.getElement(0)[0], color);
    }

  if (d_color.get()) d_color.get()->clearModified();
  if (d_coord.get()) d_coord.get()->clearModified();

  return obj;
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodePointSet::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "color" ) == 0 )
    return &d_color;
  else if ( strcmp( fieldName, "coord" ) == 0 )
    return &d_coord;

  return VrmlNodeGeometry::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodePointSet::setField(const char *fieldName,
				const VrmlField &fieldValue)
{
  if TRY_SFNODE_FIELD(color, Color)
  else if TRY_SFNODE_FIELD(coord, Coordinate)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true);
}

void VrmlNodePointSet::recalcBSphere()
{
  //cout << "VrmlNodePointSet::recalcBSphere()" << endl;
  d_bsphere.reset();
  VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();
  int nvert = coord.getLength();
  for(int i=0; i<nvert; i++) {
    const float * vi = coord.getElement(i); // vi[3]
    //cout << vi[0] << "," << vi[1] << "," << vi[2] << ")" << endl;
    d_bsphere.extend(vi);
    //d_bsphere.dump(cout);
  }
  //d_bsphere.dump(cout);
  this->setBVolumeDirty(false);
}


const VrmlBVolume* VrmlNodePointSet::getBVolume() const
{
  if (this->isBVolumeDirty())
    ((VrmlNodePointSet*)this)->recalcBSphere();
  return &d_bsphere;
}

/**
 * @brief Get the Color node associated with this PointSet.
 *
 * @return color
 */
const VrmlSFNode & VrmlNodePointSet::getColor() const {
    return this->d_color;
}

/**
 * @brief Set the Color node associated with this PointSet.
 *
 * @param color
 */
void VrmlNodePointSet::setColor(const VrmlSFNode & color) {
    this->d_color = color;
}

/**
 * @brief Get the Coordinate node associated with this PointSet.
 *
 * @return coord
 */
const VrmlSFNode & VrmlNodePointSet::getCoord() const {
    return this->d_coord;
}

/**
 * @brief Set the Coordinate node associated with this PointSet.
 *
 * @param coord
 */
void VrmlNodePointSet::setCoord(const VrmlSFNode & coord) {
    this->d_coord = coord;
}

// PositionInt factory.

static VrmlNode * createPositionInt(VrmlScene * scene) {
    return new VrmlNodePositionInt(scene);
}


// Define the built in VrmlNodeType:: "PositionInt" fields

VrmlNodeType *VrmlNodePositionInt::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("PositionInterpolator", createPositionInt);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_fraction", VrmlField::SFFLOAT);
  t->addExposedField("key", VrmlField::MFFLOAT);
  t->addExposedField("keyValue", VrmlField::MFVEC3F);
  t->addEventOut("value_changed", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodePositionInt::nodeType() const
{
    return *defineType(0);
}


VrmlNodePositionInt::VrmlNodePositionInt( VrmlScene *scene ) :
  VrmlNodeChild(scene)
{
}

VrmlNodePositionInt::~VrmlNodePositionInt()
{
}

bool VrmlNodePositionInt::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodePositionInt::printFields(ostream& os, int indent)
{
  if (d_key.getLength() > 0) PRINT_FIELD(key);
  if (d_keyValue.getLength() > 0) PRINT_FIELD(keyValue);

  return os;
}

void VrmlNodePositionInt::eventIn(double timeStamp,
				  const char *eventName,
				  const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_fraction") == 0)
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
	{
	  theSystem->error
	    ("Invalid type for %s eventIn %s (expected SFFloat).\n",
		nodeType().getName(), eventName);
	  return;
	}
      float f = static_cast<const VrmlSFFloat &>(fieldValue).get();

      int n = d_key.getLength() - 1;
      if (f < d_key.getElement(0))
	d_value.set(d_keyValue.getElement(0));
      else if (f > d_key.getElement(n))
	d_value.set(d_keyValue.getElement(n));
      else
	{
	  // should cache the last index used...
	  for (int i=0; i<n; ++i)
	    if (d_key.getElement(i) <= f && f <= d_key.getElement(i + 1))
	      {
                const float * v1 = d_keyValue.getElement(i);
                const float * v2 = d_keyValue.getElement(i + 1);

		f = (f - d_key.getElement(i)) / (d_key.getElement(i + 1) - d_key.getElement(i));
                const float valueVec[3] = { v1[0] + f * (v2[0] - v1[0]),
                                            v1[1] + f * (v2[1] - v1[1]),
                                            v1[2] + f * (v2[2] - v1[2]) };
		d_value.set(valueVec);
		break;
	      }
	}

      // Send the new value
      eventOut(timeStamp, "value_changed", d_value);
    }

  // Check exposedFields
  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);

      // This node is not renderable, so don't re-render on changes to it.
      clearModified();
    }
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodePositionInt::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "key" ) == 0 )
    return &d_key;
  else if ( strcmp( fieldName, "keyValue" ) == 0 )
    return &d_keyValue;

  // eventOuts
  else if ( strcmp( fieldName, "value" ) == 0 )
    return &d_value;

  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodePositionInt::setField(const char *fieldName,
				   const VrmlField &fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFVec3f)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

VrmlNodePositionInt* VrmlNodePositionInt::toPositionInt() const 
{ return (VrmlNodePositionInt*) this; }

const VrmlMFFloat& VrmlNodePositionInt::getKey() const  
{   return d_key; }

const VrmlMFVec3f& VrmlNodePositionInt::getKeyValue() const  
{ return d_keyValue; }

// ProximitySensor factory. 

static VrmlNode * createProximitySensor(VrmlScene * scene) {
    return new VrmlNodeProximitySensor(scene);
}


// Define the built in VrmlNodeType:: "ProximitySensor" fields

VrmlNodeType *VrmlNodeProximitySensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("ProximitySensor", createProximitySensor);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("center", VrmlField::SFVEC3F);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("size", VrmlField::SFVEC3F);
  t->addEventOut("enterTime", VrmlField::SFTIME);
  t->addEventOut("exitTime", VrmlField::SFTIME);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("orientation_changed", VrmlField::SFROTATION);
  t->addEventOut("position_changed", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeProximitySensor::nodeType() const
{
    return *defineType(0);
}


VrmlNodeProximitySensor::VrmlNodeProximitySensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_center(0.0, 0.0, 0.0),
  d_enabled(true),
  d_size(0.0, 0.0, 0.0),
  d_isActive(false),
  d_position(0.0, 0.0, 0.0),
  d_enterTime(0.0),
  d_exitTime(0.0)
{
  setModified();
}

VrmlNodeProximitySensor::~VrmlNodeProximitySensor()
{
}

bool VrmlNodeProximitySensor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeProximitySensor::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_center.getX()) ||
      ! FPZERO(d_center.getY()) ||
      ! FPZERO(d_center.getZ()) )
    PRINT_FIELD(center);
  if (! d_enabled.get())
    PRINT_FIELD(enabled);
  if (! FPZERO(d_size.getX()) ||
      ! FPZERO(d_size.getY()) ||
      ! FPZERO(d_size.getZ()) )
    PRINT_FIELD(size);
      
  return os;
}

//
// Generate proximity events. If necessary, events prior to the current
// time are generated due to interpolation of enterTimes and exitTimes. 
// The timestamp should never be increased.
//
// This is in a render() method since the it needs the viewer position
// with respect to the local coordinate system.
// Could do this with VrmlNode::inverseTransform(double [4][4]) now...
//
// The positions and times are not interpolated to report the exact
// place and time of entries and exits from the sensor regions as
// required by the spec, since that would require the last viewer position
// to be stored in the node, which is problematic in the presence of
// DEF/USEd nodes...
// I suppose the scene could keep the last viewer position in the global
// coordinate system and it could be transformed all the way down the
// scenegraph, but that sounds painful.

void VrmlNodeProximitySensor::render(Viewer *viewer, VrmlRenderContext rc)
{
  if (d_enabled.get() &&
      d_size.getX() > 0.0 &&
      d_size.getY() > 0.0 &&
      d_size.getZ() > 0.0 &&
      viewer->getRenderMode() == Viewer::RENDER_MODE_DRAW)
    {
      VrmlSFTime timeNow( theSystem->time() );
      float x, y, z;

      // Is viewer inside the box?
      viewer->getPosition( &x, &y, &z );
      bool inside = (fabs(x - d_center.getX()) <= 0.5 * d_size.getX() &&
		     fabs(y - d_center.getY()) <= 0.5 * d_size.getY() &&
		     fabs(z - d_center.getZ()) <= 0.5 * d_size.getZ());
      bool wasIn = d_isActive.get();

      // Check if viewer has entered the box
      if (inside && ! wasIn)
	{
	  theSystem->debug("PS.%s::render ENTER %g %g %g\n", name(), x, y, z);

	  d_isActive.set(true);
	  eventOut(timeNow.get(), "isActive", d_isActive);

	  d_enterTime = timeNow;
	  eventOut(timeNow.get(), "enterTime", d_enterTime);
	}

      // Check if viewer has left the box
      else if (wasIn && ! inside)
	{
	  theSystem->debug("PS.%s::render EXIT %g %g %g\n", name(), x, y, z);

	  d_isActive.set(false);
	  eventOut(timeNow.get(), "isActive", d_isActive );

	  d_exitTime = timeNow;
	  eventOut(timeNow.get(), "exitTime", d_exitTime);
	}

      // Check for movement within the box
      if (wasIn || inside)
	{
	  if ( ! FPEQUAL(d_position.getX(), x) ||
	       ! FPEQUAL(d_position.getY(), y) ||
	       ! FPEQUAL(d_position.getZ(), z) )
	    {
	      const float positionVec[3] = { x, y, z };
              d_position.set(positionVec);
	      eventOut(timeNow.get(), "position_changed", d_position);
	    }

	  float xyzr[4];
	  viewer->getOrientation( xyzr );
	  if ( ! FPEQUAL(d_orientation.getX(), xyzr[0]) ||
	       ! FPEQUAL(d_orientation.getY(), xyzr[1]) ||
	       ! FPEQUAL(d_orientation.getZ(), xyzr[2]) ||
	       ! FPEQUAL(d_orientation.getAngle(), xyzr[3]) )
	    {
	      d_orientation.set(xyzr);
	      eventOut(timeNow.get(), "orientation_changed", d_orientation);
	    }
	}
    }

  else
    clearModified();
}


const VrmlField *VrmlNodeProximitySensor::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "center" ) == 0 )
    return &d_center;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "size" ) == 0 )
    return &d_size;

  // eventOuts
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  else if ( strcmp( fieldName, "position" ) == 0 )
    return &d_position;
  else if ( strcmp( fieldName, "orientation" ) == 0 )
    return &d_orientation;
  else if ( strcmp( fieldName, "enterTime" ) == 0 )
    return &d_enterTime;
  else if ( strcmp( fieldName, "exitTime" ) == 0 )
    return &d_exitTime;
  
  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeProximitySensor::setField(const char *fieldName,
				       const VrmlField &fieldValue)
{
  if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(size, SFVec3f)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


// ScalarInt factory.

static VrmlNode * createScalarInt(VrmlScene * scene) {
    return new VrmlNodeScalarInt(scene);
}


// Define the built in VrmlNodeType:: "ScalarInterpolator" fields

VrmlNodeType *VrmlNodeScalarInt::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("ScalarInterpolator", createScalarInt);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_fraction", VrmlField::SFFLOAT);
  t->addExposedField("key", VrmlField::MFFLOAT);
  t->addExposedField("keyValue", VrmlField::MFFLOAT);
  t->addEventOut("value_changed", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodeScalarInt::nodeType() const
{
    return *defineType(0);
}


VrmlNodeScalarInt::VrmlNodeScalarInt( VrmlScene *scene ) :
  VrmlNodeChild(scene)
{
}

VrmlNodeScalarInt::~VrmlNodeScalarInt()
{
}

bool VrmlNodeScalarInt::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeScalarInt::printFields(ostream& os, int indent)
{
  if (d_key.getLength() > 0) PRINT_FIELD(key);
  if (d_keyValue.getLength() > 0) PRINT_FIELD(keyValue);

  return os;
}

void VrmlNodeScalarInt::eventIn(double timeStamp,
				const char *eventName,
				const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_fraction") == 0)
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
	{
	  theSystem->error
	    ("Invalid type for %s eventIn %s (expected SFFloat).\n",
		nodeType().getName(), eventName);
	  return;
	}
      float f = static_cast<const VrmlSFFloat &>(fieldValue).get();

      int n = d_key.getLength() - 1;
      if (f < d_key.getElement(0))
	d_value.set(d_keyValue.getElement(0));
      else if (f > d_key.getElement(n))
	d_value.set(d_keyValue.getElement(n));
      else
	{
	  for (int i=0; i<n; ++i)
	    if (d_key.getElement(i) <= f && f <= d_key.getElement(i + 1))
	      {
		float v1 = d_keyValue.getElement(i);
		float v2 = d_keyValue.getElement(i + 1);

		f = (f - d_key.getElement(i)) / (d_key.getElement(i + 1) - d_key.getElement(i));
		d_value.set( v1 + f * (v2 - v1) );
		break;
	      }
	}

      // Send the new value
      eventOut(timeStamp, "value_changed", d_value);
    }

  // Check exposedFields
  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);

      // This node is not renderable, so don't re-render on changes to it.
      clearModified();
    }
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeScalarInt::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "key" ) == 0 )
    return &d_key;
  else if ( strcmp( fieldName, "keyValue" ) == 0 )
    return &d_keyValue;

  // eventOuts
  else if ( strcmp( fieldName, "value" ) == 0 )
    return &d_value;

  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeScalarInt::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFFloat)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

VrmlNodeScalarInt* VrmlNodeScalarInt::toScalarInt() const
{ return (VrmlNodeScalarInt*) this; }

const VrmlMFFloat& VrmlNodeScalarInt::getKey() const 
{   return d_key; }

const VrmlMFFloat& VrmlNodeScalarInt::getKeyValue() const 
{ return d_keyValue; }

static VrmlNode * createShape(VrmlScene * scene) {
    return new VrmlNodeShape(scene);
}


// Define the built in VrmlNodeType:: "Shape" fields

VrmlNodeType *VrmlNodeShape::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Shape", createShape);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("appearance", VrmlField::SFNODE);
  t->addExposedField("geometry", VrmlField::SFNODE);

  return t;
}


VrmlNodeType & VrmlNodeShape::nodeType() const
{
    return *defineType(0);
}


VrmlNodeShape::VrmlNodeShape(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_viewerObject(0)
{
}

VrmlNodeShape::~VrmlNodeShape()
{
  // need viewer to free d_viewerObject ...
}

bool VrmlNodeShape::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeShape::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_appearance.get()) {
            this->d_appearance.get()->resetVisitedFlag();
        }
        if (this->d_geometry.get()) {
            this->d_geometry.get()->resetVisitedFlag();
        }
    }
}

bool VrmlNodeShape::isModified() const
{
  return ( d_modified ||
	   (d_geometry.get() && d_geometry.get()->isModified()) ||
	   (d_appearance.get() && d_appearance.get()->isModified()) );
}

void VrmlNodeShape::updateModified(VrmlNodePath& path, int flags)
{
  //cout << "VrmlNodeShape[" << this << "]::updateModified()" << endl;
  if (this->isModified()) markPathModified(path, true, flags);
  path.push_front(this);
  if (d_appearance.get()) d_appearance.get()->updateModified(path, flags);
  if (d_geometry.get()) d_geometry.get()->updateModified(path, flags);
  path.pop_front();
}

void VrmlNodeShape::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_appearance.get()) d_appearance.get()->clearFlags();
  if (d_geometry.get()) d_geometry.get()->clearFlags();
}

void VrmlNodeShape::addToScene(VrmlScene *s, const char *relUrl)
{
  d_scene = s;
  if (d_appearance.get()) d_appearance.get()->addToScene(s,relUrl);
  if (d_geometry.get()) d_geometry.get()->addToScene(s,relUrl);
}

ostream& VrmlNodeShape::printFields(ostream& os, int indent)
{
  if (d_appearance.get()) PRINT_FIELD(appearance);
  if (d_geometry.get()) PRINT_FIELD(geometry);

  return os;
}

VrmlNodeShape*	VrmlNodeShape::toShape() const
{ return (VrmlNodeShape*) this; }


void VrmlNodeShape::render(Viewer *viewer, VrmlRenderContext rc)
{
  //cout << "VrmlNodeShape::render()" << endl;

  if ( d_viewerObject && isModified() )
    {
      viewer->removeObject(d_viewerObject);
      d_viewerObject = 0;
    }

  VrmlNodeGeometry *g = d_geometry.get() ? d_geometry.get()->toGeometry() : 0;

  if (d_viewerObject)
    viewer->insertReference(d_viewerObject);

  else if ( g )
    {
      d_viewerObject = viewer->beginObject( name() );

      // Don't care what color it is if we are picking
      bool picking = (Viewer::RENDER_MODE_PICK == viewer->getRenderMode());
      if (! picking )
	{
	  int nTexComponents = 0;

	  if (! picking && d_appearance.get() &&
	      d_appearance.get()->toAppearance() )
	    {
	      VrmlNodeAppearance *a = d_appearance.get()->toAppearance();
	      a->render(viewer, rc);

	      if (a->getTexture().get() && a->getTexture().get()->toTexture())
		nTexComponents = a->getTexture().get()->toTexture()->nComponents();
	    }
	  else
	    {
	      viewer->setColor(1.0, 1.0, 1.0); // default object color
	      viewer->enableLighting(false);  // turn lighting off
	    }

	  // hack for opengl material mode
	  viewer->setMaterialMode( nTexComponents, g->color() != 0 );
	}

      g->render(viewer, rc);

      viewer->endObject();
    }

  else if (d_appearance.get())
    d_appearance.get()->clearModified();

  clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeShape::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "appearance" ) == 0 )
    return &d_appearance;
  else if ( strcmp( fieldName, "geometry" ) == 0 )
    return &d_geometry;
  
  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeShape::setField(const char *fieldName,
			     const VrmlField &fieldValue)
{
  if TRY_SFNODE_FIELD(appearance, Appearance)
  else if TRY_SFNODE_FIELD(geometry, Geometry)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true); // case out: just if geom set!
}


// just pass off to the geometry's getbvolume() method
//
const VrmlBVolume* VrmlNodeShape::getBVolume() const
{
  //cout << "VrmlNodeShape::getBVolume() {" << endl;
  const VrmlBVolume* r = (VrmlBVolume*)0;
  const VrmlNodePtr & geom(d_geometry.get());
  if (geom) {
    r = geom->getBVolume();
  }
  //cout << "}:" << r << ":";
  //if (r) r->dump(cout);
  //cout << endl;
  ((VrmlNodeShape*)this)->setBVolumeDirty(false);
  return r;
}

/**
 * @brief Get the Appearance node associated with this Shape.
 *
 * @return appearance
 */
const VrmlSFNode & VrmlNodeShape::getAppearance() const {
    return this->d_appearance;
}

/**
 * @brief Set the Appearance node associated with this Shape.
 *
 * @param appearance
 */
void VrmlNodeShape::setAppearance(const VrmlSFNode & appearance) {
    assert(!appearance.get() || appearance.get()->toAppearance());
    this->d_appearance = appearance;
}

/**
 * @brief Get the geometry node associated with this Shape.
 *
 * @return geometry
 */
const VrmlSFNode & VrmlNodeShape::getGeometry() const {
    return this->d_geometry;
}

/**
 * @brief Set the geometry node associated with this Shape.
 *
 * @param geometry
 */
void VrmlNodeShape::setGeometry(const VrmlSFNode & geometry) {
    assert(!geometry.get() || geometry.get()->toGeometry());
    this->d_geometry = geometry;
}

// Sound factory. Add each Sound to the scene for fast access.

static VrmlNode *createSound(VrmlScene * scene) {
    return new VrmlNodeSound(scene);
}


// Define the built in VrmlNodeType:: "Sound" fields

VrmlNodeType *VrmlNodeSound::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("Sound", createSound);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("direction", VrmlField::SFVEC3F);
  t->addExposedField("intensity", VrmlField::SFFLOAT);
  t->addExposedField("location", VrmlField::SFVEC3F);
  t->addExposedField("maxBack", VrmlField::SFFLOAT);
  t->addExposedField("maxFront", VrmlField::SFFLOAT);
  t->addExposedField("minBack", VrmlField::SFFLOAT);
  t->addExposedField("minFront", VrmlField::SFFLOAT);
  t->addExposedField("priority", VrmlField::SFFLOAT);
  t->addExposedField("source", VrmlField::SFNODE);
  t->addField("spatialize", VrmlField::SFBOOL);

  return t;
}

// Should subclass NodeType and have each Sound maintain its own type...

VrmlNodeType & VrmlNodeSound::nodeType() const
{
    return *defineType(0);
}


VrmlNodeSound::VrmlNodeSound( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_direction(0, 0, 1),
  d_intensity(1),
  d_maxBack(10),
  d_maxFront(10),
  d_minBack(1),
  d_minFront(1),
  d_spatialize(true)
{
}


VrmlNodeSound::~VrmlNodeSound()
{
}

bool VrmlNodeSound::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeSound::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_source.get()) {
            this->d_source.get()->resetVisitedFlag();
        }
    }
}

VrmlNodeSound* VrmlNodeSound::toSound() const
{ return (VrmlNodeSound*) this; }

void VrmlNodeSound::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_source.get()) d_source.get()->updateModified(path);
  path.pop_front();
}

void VrmlNodeSound::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_source.get()) d_source.get()->clearFlags();
}

void VrmlNodeSound::addToScene(VrmlScene *s, const char *relUrl)
{
  d_scene = s;
  if (d_source.get()) d_source.get()->addToScene(s, relUrl);
}

ostream& VrmlNodeSound::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_direction.getX()) ||
      ! FPZERO(d_direction.getY()) ||
      ! FPEQUAL(d_direction.getZ(), 1.0) )
    PRINT_FIELD(direction);

  // ...

  return os;
}


void VrmlNodeSound::render(Viewer *viewer, VrmlRenderContext rc)
{
    // If this clip has been modified, update the internal data
    if (d_source.get() && d_source.get()->isModified())
        d_source.get()->render (viewer, rc);
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeSound::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "direction" ) == 0 )
    return &d_direction;
  else if ( strcmp( fieldName, "intensity" ) == 0 )
    return &d_intensity;
  else if ( strcmp( fieldName, "location" ) == 0 )
    return &d_location;
  else if ( strcmp( fieldName, "maxBack" ) == 0 )
    return &d_maxBack;
  else if ( strcmp( fieldName, "maxFront" ) == 0 )
    return &d_maxFront;
  else if ( strcmp( fieldName, "minBack" ) == 0 )
    return &d_minBack;
  else if ( strcmp( fieldName, "minFront" ) == 0 )
    return &d_minFront;
  else if ( strcmp( fieldName, "priority" ) == 0 )
    return &d_priority;
  else if ( strcmp( fieldName, "source" ) == 0 )
    return &d_source;
  else if ( strcmp( fieldName, "spatialize" ) == 0 )
    return &d_spatialize;
  
  return VrmlNodeChild::getField(fieldName); // Parent class
}



// Set the value of one of the node fields/events.
// setField is public so the parser can access it.

void VrmlNodeSound::setField(const char *fieldName,
			     const VrmlField &fieldValue)
{
  if TRY_FIELD(direction, SFVec3f)
  else if TRY_FIELD(intensity, SFFloat)
  else if TRY_FIELD(location, SFVec3f)		     
  else if TRY_FIELD(maxBack, SFFloat)
  else if TRY_FIELD(maxFront, SFFloat)
  else if TRY_FIELD(minBack, SFFloat)
  else if TRY_FIELD(minFront, SFFloat)
  else if TRY_FIELD(priority, SFFloat)
  else if TRY_SFNODE_FIELD2(source, AudioClip, MovieTexture)
  else if TRY_FIELD(spatialize, SFBool)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

/**
 * @brief Get the sound source associated with this Sound.
 *
 * @return source
 */
const VrmlSFNode & VrmlNodeSound::getSource() const {
    return this->d_source;
}

/**
 * @brief Set the sound source associated with this Sound.
 *
 * @param source
 */
void VrmlNodeSound::setSource(const VrmlSFNode & source) {
    this->d_source = source;
}

static VrmlNode * createSphere(VrmlScene * scene) {
    return new VrmlNodeSphere(scene);
}


// Define the built in VrmlNodeType:: "Sphere" fields

VrmlNodeType *VrmlNodeSphere::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Sphere", createSphere);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class
  t->addField("radius", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodeSphere::nodeType() const
{
    return *defineType(0);
}


VrmlNodeSphere::VrmlNodeSphere(VrmlScene *scene) :
  VrmlNodeGeometry(scene),
  d_radius(1.0)
{
  setBVolumeDirty(true); // lazy calc of bvolume
}

VrmlNodeSphere::~VrmlNodeSphere()
{
}

bool VrmlNodeSphere::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeSphere::printFields(ostream& os, int )
{
  if (d_radius.get() != 1.0)
    os << "radius " << d_radius;
  return os;
}

Viewer::Object VrmlNodeSphere::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  return viewer->insertSphere(d_radius.get());
}

// Set the value of one of the node fields.

void VrmlNodeSphere::setField(const char *fieldName,
			      const VrmlField &fieldValue)
{
  if TRY_FIELD(radius, SFFloat)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
  setBVolumeDirty(true);
}

VrmlNodeSphere* VrmlNodeSphere::toSphere() const //LarryD Mar 08/99
{ return (VrmlNodeSphere*) this; }


const VrmlBVolume*
VrmlNodeSphere::getBVolume() const
{
  //cout << "VrmlNodeSphere::getBVolume()" << endl;
  //static VrmlBSphere* sphere_sphere = new VrmlBSphere();
  if (this->isBVolumeDirty()) {
    ((VrmlNodeSphere*)this)->d_bsphere.setRadius(d_radius.get());
    ((VrmlNode*)this)->setBVolumeDirty(false); // logical const
    //cout << "VrmlNodeSphere::getBVolume():recalc:";
  }
  //else 
  //cout << "VrmlNodeSphere::getBVolume():clean :";
  //b_bsphere->dump(cout) << endl;
  return &d_bsphere;
}


// SphereSensor factory. 

static VrmlNode * createSphereSensor(VrmlScene * scene) {
    return new VrmlNodeSphereSensor(scene);
}


// Define the built in VrmlNodeType:: "SphereSensor" fields

VrmlNodeType *VrmlNodeSphereSensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("SphereSensor", createSphereSensor);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("autoOffset", VrmlField::SFBOOL);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("offset", VrmlField::SFROTATION);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("rotation_changed", VrmlField::SFROTATION);
  t->addEventOut("trackPoint_changed", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeSphereSensor::nodeType() const 
{
    return *defineType(0);
}


VrmlNodeSphereSensor::VrmlNodeSphereSensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_autoOffset(true),
  d_enabled(true),
  d_offset(0,1,0,0),
  d_isActive(false)
{
  setModified();
}


VrmlNodeSphereSensor::~VrmlNodeSphereSensor()
{
}

bool VrmlNodeSphereSensor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeSphereSensor* VrmlNodeSphereSensor::toSphereSensor() const	// mgiger 6/16/00
{
	return (VrmlNodeSphereSensor*) this;
}

ostream& VrmlNodeSphereSensor::printFields(ostream& os, int indent)
{
  if (! d_autoOffset.get()) PRINT_FIELD(autoOffset);
  if (! d_enabled.get())    PRINT_FIELD(enabled);

  if (! FPZERO(d_offset.getX()) ||
      ! FPEQUAL(d_offset.getY(), 1.0) ||
      ! FPZERO(d_offset.getZ()) ||
      ! FPZERO(d_offset.getAngle()) )
    PRINT_FIELD(offset);

  return os;
}

const VrmlField *VrmlNodeSphereSensor::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "autoOffset" ) == 0 )
    return &d_autoOffset;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "offset" ) == 0 )
    return &d_offset;

  // eventOuts
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  else if ( strcmp( fieldName, "rotation" ) == 0 )
    return &d_rotation;
  else if ( strcmp( fieldName, "trackPoint" ) == 0 )
    return &d_trackPoint;
  
  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeSphereSensor::setField(const char *fieldName,
				   const VrmlField &fieldValue)
{
  if TRY_FIELD(autoOffset, SFBool)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(offset, SFRotation)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}

void VrmlNodeSphereSensor::activate( double timeStamp,
				    bool isActive,
				    double *p )
{
		// Become active
	if ( isActive && ! d_isActive.get() ) {
		d_isActive.set(isActive);
		
			// set activation point in world coords
                const float floatVec[3] = { p[0], p[1], p[2] };
		d_activationPoint.set(floatVec);
		
		if(d_autoOffset.get())
			d_rotation = d_offset;
		
			// calculate the center of the object in world coords
		float V[3] = { 0.0, 0.0, 0.0 };
		double M[4][4];
		inverseTransform( M );
		VM( V, M, V );
		d_centerPoint.set(V);
		
			// send message
		eventOut( timeStamp, "isActive", d_isActive );
	}

		// Become inactive
	else if ( ! isActive && d_isActive.get() ) {
		d_isActive.set(isActive);
		eventOut( timeStamp, "isActive", d_isActive );

			// save auto offset of rotation
		if ( d_autoOffset.get() ) {
			d_offset = d_rotation;
			eventOut( timeStamp, "offset_changed", d_offset );
		}
	}

		// Tracking
	else if ( isActive ) {

		// get local coord for touch point
		float V[3] = { p[0], p[1], p[2] };
		double M[4][4];
		inverseTransform( M );
		VM( V, M, V );
		d_trackPoint.set(V);
		eventOut( timeStamp, "trackPoint_changed", d_trackPoint );
		
		float V2[3] = { p[0], p[1], p[2] };
                float tempv[3];
		Vdiff(tempv, V2, d_centerPoint.get());
                VrmlSFVec3f dir1(tempv);
		double dist = dir1.length();				// get the length of the pre-normalized vector
		dir1 = dir1.normalize();
		Vdiff(tempv, d_activationPoint.get(), d_centerPoint.get());
                VrmlSFVec3f dir2(tempv);
		dir2 = dir2.normalize();
		
		Vcross(tempv, dir1.get(), dir2.get());
                VrmlSFVec3f cx(tempv);

		VrmlSFRotation newRot(cx, dist * acos(dir1.dot(dir2)));
		if ( d_autoOffset.get() )
			newRot = newRot.multiply(d_offset);
		d_rotation = newRot;
		
		eventOut( timeStamp, "rotation_changed", d_rotation );
	}
}

// Return a new VrmlNodeSpotLight
static VrmlNode * createSpotLight(VrmlScene * scene) {
    return new VrmlNodeSpotLight(scene);
}


// Define the built in VrmlNodeType:: "SpotLight" fields

VrmlNodeType *VrmlNodeSpotLight::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("SpotLight", createSpotLight);
      t->reference();
    }

  VrmlNodeLight::defineType(t);	// Parent class
  t->addExposedField("attenuation", VrmlField::SFVEC3F);
  t->addExposedField("beamWidth", VrmlField::SFFLOAT);
  t->addExposedField("cutOffAngle", VrmlField::SFFLOAT);
  t->addExposedField("direction", VrmlField::SFVEC3F);
  t->addExposedField("location", VrmlField::SFVEC3F);
  t->addExposedField("radius", VrmlField::SFFLOAT);

  return t;
}


VrmlNodeType & VrmlNodeSpotLight::nodeType() const
{
    return *defineType(0);
}


VrmlNodeSpotLight::VrmlNodeSpotLight(VrmlScene *scene) :
  VrmlNodeLight(scene),
  d_attenuation(1.0, 0.0, 0.0),
  d_beamWidth(1.570796),
  d_cutOffAngle(0.785398),
  d_direction(0.0, 0.0, -1.0),
  d_location(0.0, 0.0, 0.0),
  d_radius(100)
{
  if (d_scene) d_scene->addScopedLight(this);
}

VrmlNodeSpotLight::~VrmlNodeSpotLight()
{
  if (d_scene) d_scene->removeScopedLight(this);
}

bool VrmlNodeSpotLight::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeSpotLight* VrmlNodeSpotLight::toSpotLight() const
{ return (VrmlNodeSpotLight*) this; }


void VrmlNodeSpotLight::addToScene(VrmlScene *s, const char *)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addScopedLight(this); }


ostream& VrmlNodeSpotLight::printFields(ostream& os, int indent)
{
  VrmlNodeLight::printFields(os, indent);
  if (! FPEQUAL(d_attenuation.getX(), 1.0) ||
      ! FPZERO(d_attenuation.getY()) ||
      ! FPZERO(d_attenuation.getZ()) )
    PRINT_FIELD(attenuation);
  if (! FPEQUAL(d_beamWidth.get(), 1.570796))
    PRINT_FIELD(beamWidth);

  if (! FPEQUAL(d_cutOffAngle.get(), 1.570796))
    PRINT_FIELD(cutOffAngle);
  if (! FPZERO(d_direction.getX()) ||
      ! FPZERO(d_direction.getY()) ||
      ! FPEQUAL(d_direction.getZ(), -1.0) )
    PRINT_FIELD(direction);

  if (! FPZERO(d_location.getX()) ||
      ! FPZERO(d_location.getY()) ||
      ! FPZERO(d_location.getZ()) )
    PRINT_FIELD(location);

  if (! FPEQUAL(d_radius.get(), 100.0))
    PRINT_FIELD(radius);

  return os;
}


// This should be called before rendering any geometry in the scene.
// Since this is called from Scene::render() before traversing the
// scene graph, the proper transformation matrix hasn't been set up.
// Somehow it needs to figure out the accumulated xforms of its
// parents and apply them before rendering. This is not easy with
// DEF/USEd nodes...
void VrmlNodeSpotLight::renderScoped(Viewer *viewer)
{
  if ( d_on.get() && d_radius.get() > 0.0 )
    viewer->insertSpotLight( d_ambientIntensity.get(),
			     d_attenuation.get(),
			     d_beamWidth.get(),
			     d_color.get(),
			     d_cutOffAngle.get(),
			     d_direction.get(),
			     d_intensity.get(),
			     d_location.get(),
			     d_radius.get() );
  clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeSpotLight::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "attenuation" ) == 0 )
    return &d_attenuation;
  else if ( strcmp( fieldName, "beamWidth" ) == 0 )
    return &d_beamWidth;
  else if ( strcmp( fieldName, "cutOffAngle" ) == 0 )
    return &d_cutOffAngle;
  else if ( strcmp( fieldName, "direction" ) == 0 )
    return &d_direction;
  else if ( strcmp( fieldName, "location" ) == 0 )
    return &d_location;
  else if ( strcmp( fieldName, "radius" ) == 0 )
    return &d_radius;

  return VrmlNodeLight::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeSpotLight::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(attenuation, SFVec3f)
  else if TRY_FIELD(beamWidth, SFFloat)
  else if TRY_FIELD(cutOffAngle, SFFloat)
  else if TRY_FIELD(direction, SFVec3f)
  else if TRY_FIELD(location, SFVec3f)
  else if TRY_FIELD(radius, SFFloat)
  else 
    VrmlNodeLight::setField(fieldName, fieldValue);
}

const VrmlSFVec3f& VrmlNodeSpotLight::getAttenuation() const   // LarryD Mar 04/99
{  return d_attenuation; }

const VrmlSFVec3f& VrmlNodeSpotLight::getDirection() const   // LarryD Mar 04/99
{  return d_direction; }

const VrmlSFVec3f& VrmlNodeSpotLight::getLocation() const   // LarryD Mar 04/99
{  return d_location; }

// Return a new VrmlNodeSwitch
static VrmlNode *createSwitch(VrmlScene * scene) {
    return new VrmlNodeSwitch(scene);
}


// Define the built in VrmlNodeType:: "Switch" fields

VrmlNodeType *VrmlNodeSwitch::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Switch", createSwitch);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("choice", VrmlField::MFNODE);
  t->addExposedField("whichChoice", VrmlField::SFINT32);

  return t;
}

VrmlNodeType & VrmlNodeSwitch::nodeType() const
{
    return *defineType(0);
}


VrmlNodeSwitch::VrmlNodeSwitch(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_whichChoice(-1)
{
  this->setBVolumeDirty(true);
}

VrmlNodeSwitch::~VrmlNodeSwitch()
{
}

bool VrmlNodeSwitch::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeSwitch::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        for (size_t i = 0; i < this->d_choice.getLength(); ++i) {
            if (this->d_choice.getElement(i)) {
                this->d_choice.getElement(i)->resetVisitedFlag();
            }
        }
    }
}

bool VrmlNodeSwitch::isModified() const
{
  if (d_modified) return true;

  int w = d_whichChoice.get();

  return (w >= 0 && w < (int) d_choice.getLength() && d_choice.getElement(w)->isModified());
}


// ok: again we get this issue of whether to check _all_ the children
// or just the current choice (ref LOD). again, chooise to test them
// all. note that the original isModified() just tested the current
// one. keep that in mind, and change it back when confirmed safe.
//
void VrmlNodeSwitch::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  int n = d_choice.getLength();
  for (int i=0; i<n; ++i)
    d_choice.getElement(i)->updateModified(path);
  path.pop_front();
}


void VrmlNodeSwitch::clearFlags()
{
  VrmlNode::clearFlags();
  
  int n = d_choice.getLength();
  for (int i = 0; i<n; ++i)
    d_choice.getElement(i)->clearFlags();
}

void VrmlNodeSwitch::addToScene( VrmlScene *s, const char *rel )
{
  d_scene = s;
  
  int n = d_choice.getLength();

  for (int i = 0; i<n; ++i)
    d_choice.getElement(i)->addToScene(s, rel);
}

ostream& VrmlNodeSwitch::printFields(ostream& os, int indent)
{
  if (d_choice.getLength() > 0) PRINT_FIELD(choice);
  if (d_whichChoice.get() != -1) PRINT_FIELD(whichChoice);
  return os;
}


// Render the selected child

void VrmlNodeSwitch::render(Viewer *viewer, VrmlRenderContext rc)
{
  int w = d_whichChoice.get();

  if (w >= 0 && w < (int) d_choice.getLength())
    d_choice.getElement(w)->render(viewer, rc);

  clearModified();
}


// Get the value of one of the node fields.
const VrmlField *VrmlNodeSwitch::getField(const char *fieldName) const
{
  if ( strcmp( fieldName, "choice" ) == 0 )
    return &d_choice;
  else if ( strcmp( fieldName, "whichChoice" ) == 0 )
    return &d_whichChoice;

  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.
void VrmlNodeSwitch::setField(const char *fieldName,
			      const VrmlField &fieldValue)
{
  if TRY_FIELD(choice, MFNode)
  else if TRY_FIELD(whichChoice, SFInt32)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true);
}

VrmlNodeSwitch* VrmlNodeSwitch::toSwitch() const //LarryD
{ return (VrmlNodeSwitch*) this; }


const VrmlBVolume* VrmlNodeSwitch::getBVolume() const
{
  //cout << "VrmlNodeGroup[" << this << "]::getBVolume()" << endl;
  if (this->isBVolumeDirty())
    ((VrmlNodeSwitch*)this)->recalcBSphere();
  return &d_bsphere;
}

void
VrmlNodeSwitch::recalcBSphere()
{
  //cout << "VrmlNodeSwitch[" << this << "]::recalcBSphere()" << endl;
  d_bsphere.reset();
  int w = d_whichChoice.get();
  if (w >= 0 && w < (int) d_choice.getLength()) {
    const VrmlBVolume* ci_bv = d_choice.getElement(w)->getBVolume();
    if (ci_bv)
      d_bsphere.extend(*ci_bv);
  }
  this->setBVolumeDirty(false);
}

/**
 * @brief Get the nodes that may be chosen.
 *
 * @return choice
 */
const VrmlMFNode & VrmlNodeSwitch::getChoice() const {
    return this->d_choice;
}

/**
 * @brief Set the nodes that may be chosen.
 *
 * @param choice
 */
void VrmlNodeSwitch::setChoice(const VrmlMFNode & choice) {
    this->d_choice = choice;
    this->setBVolumeDirty(true);
}

/**
 * @brief Get the index of the chosen node.
 *
 * @return whichChoice
 */
const VrmlSFInt32 & VrmlNodeSwitch::getWhichChoice() const {
    return this->d_whichChoice;
}

/**
 * @brief Set the index of the chosen node.
 *
 * @param whichChoice
 */
void VrmlNodeSwitch::setWhichChoice(const VrmlSFInt32 & whichChoice) {
    this->d_whichChoice = whichChoice;
    this->setBVolumeDirty(true);
}

// Make a VrmlNodeText

static VrmlNode * createText(VrmlScene * scene) {
    return new VrmlNodeText(scene);
}


// Define the built in VrmlNodeType:: "Text" fields

VrmlNodeType *VrmlNodeText::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;	// Define type only once.
      t = st = new VrmlNodeType("Text", createText);
      t->reference();
    }

  VrmlNodeGeometry::defineType(t);	// Parent class
  t->addExposedField("string", VrmlField::MFSTRING);
  t->addExposedField("fontStyle", VrmlField::SFNODE);
  t->addExposedField("length", VrmlField::MFFLOAT);
  t->addExposedField("maxExtent", VrmlField::SFFLOAT);

  return t;
}

VrmlNodeType & VrmlNodeText::nodeType() const
{
    return *defineType(0);
}


VrmlNodeText::VrmlNodeText(VrmlScene *scene) :
  VrmlNodeGeometry(scene)
{
}

VrmlNodeText::~VrmlNodeText()
{
}

bool VrmlNodeText::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeText::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        if (this->d_fontStyle.get()) {
            this->d_fontStyle.get()->resetVisitedFlag();
        }
    }
}

bool VrmlNodeText::isModified() const
{
  return (VrmlNode::isModified() ||
	  (d_fontStyle.get() && d_fontStyle.get()->isModified()));
}

void VrmlNodeText::updateModified(VrmlNodePath& path)
{
  if (this->isModified()) markPathModified(path, true);
  path.push_front(this);
  if (d_fontStyle.get()) d_fontStyle.get()->updateModified(path);
  path.pop_front();
}


void VrmlNodeText::clearFlags()
{
  VrmlNode::clearFlags();
  if (d_fontStyle.get()) d_fontStyle.get()->clearFlags();
}

void VrmlNodeText::addToScene( VrmlScene *s, const char *relUrl )
{
  d_scene = s;
  if (d_fontStyle.get()) d_fontStyle.get()->addToScene(s, relUrl);
}

ostream& VrmlNodeText::printFields(ostream& os, int indent)
{
  if (d_string.getLength() > 0) PRINT_FIELD(string);
  if (d_fontStyle.get()) PRINT_FIELD(fontStyle);
  if (d_length.getLength() > 0) PRINT_FIELD(length);
  if (! FPZERO(d_maxExtent.get()) ) PRINT_FIELD(maxExtent);
  return os;
}


Viewer::Object VrmlNodeText::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
    char const * const * s = d_string.get();
    
    if (s) {
        int justify[2] = { 1, 1 };
        float size = 1.0;
        VrmlNodeFontStyle *f = 0;
        if (d_fontStyle.get()) {
            f = d_fontStyle.get()->toFontStyle();
        }
        
        if (f) {
            VrmlMFString & j = f->justify();
            
            for (size_t i=0; i<j.getLength(); ++i) {
                if (strcmp(j.getElement(i), "END") == 0) {
                    justify[i] = -1;
                } else if (strcmp(j.getElement(i), "MIDDLE") == 0) {
                    justify[i] = 0;
                }
            }
            size = f->size();
        }
        
        return viewer->insertText(justify, size, d_string.getLength(), s);
    }
    
    return 0;
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeText::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "string" ) == 0 )
    return &d_string;
  else if ( strcmp( fieldName, "fontStyle" ) == 0 )
    return &d_fontStyle;
  else if ( strcmp( fieldName, "length" ) == 0 )
    return &d_maxExtent;
  
  return VrmlNodeGeometry::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeText::setField(const char *fieldName,
			    const VrmlField &fieldValue)
{
  if TRY_FIELD(string, MFString)
  else if TRY_SFNODE_FIELD(fontStyle, FontStyle)
  else if TRY_FIELD(length, MFFloat)
  else if TRY_FIELD(maxExtent, SFFloat)
  else
    VrmlNodeGeometry::setField(fieldName, fieldValue);
}

/**
 * @brief Get the FontStyle associated with this Text node.
 *
 * @return fontStyle
 */
const VrmlSFNode & VrmlNodeText::getFontStyle() const {
    return this->d_fontStyle;
}

/**
 * @brief Set the FontStyle associated with this Text node.
 *
 * @param fontStyle
 */
void VrmlNodeText::setFontStyle(const VrmlSFNode & fontStyle) {
    this->d_fontStyle = fontStyle;
}

static VrmlNode * createTextureCoordinate(VrmlScene * scene) {
    return new VrmlNodeTextureCoordinate(scene);
}


// Define the built in VrmlNodeType:: "TextureCoordinate" fields

VrmlNodeType *VrmlNodeTextureCoordinate::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("TextureCoordinate", createTextureCoordinate);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("point", VrmlField::MFVEC2F);

  return t;
}

VrmlNodeType & VrmlNodeTextureCoordinate::nodeType() const 
{
    return *defineType(0);
}


VrmlNodeTextureCoordinate::VrmlNodeTextureCoordinate(VrmlScene *scene) :
  VrmlNode(scene)
{
}

VrmlNodeTextureCoordinate::~VrmlNodeTextureCoordinate()
{
}

bool VrmlNodeTextureCoordinate::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeTextureCoordinate* VrmlNodeTextureCoordinate::toTextureCoordinate() const
{ return (VrmlNodeTextureCoordinate*) this; }


ostream& VrmlNodeTextureCoordinate::printFields(ostream& os, int indent)
{
  if (d_point.getLength() > 0) PRINT_FIELD(point);
  return os;
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeTextureCoordinate::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "point" ) == 0 )
    return &d_point;
  return VrmlNode::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTextureCoordinate::setField(const char *fieldName,
					 const VrmlField &fieldValue)
{
  if TRY_FIELD(point, MFVec2f)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

static VrmlNode * createTextureTransform(VrmlScene * scene) {
  return new VrmlNodeTextureTransform(scene); 
}


// Define the built in VrmlNodeType:: "TextureTransform" fields

VrmlNodeType *VrmlNodeTextureTransform::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("TextureTransform", createTextureTransform);
      t->reference();
    }

  VrmlNode::defineType(t);	// Parent class
  t->addExposedField("center", VrmlField::SFVEC2F);
  t->addExposedField("rotation", VrmlField::SFFLOAT);
  t->addExposedField("scale", VrmlField::SFVEC2F);
  t->addExposedField("translation", VrmlField::SFVEC2F);

  return t;
}

VrmlNodeType & VrmlNodeTextureTransform::nodeType() const 
{
    return *defineType(0);
}


VrmlNodeTextureTransform::VrmlNodeTextureTransform(VrmlScene *scene) :
  VrmlNode(scene),
  d_center(0.0, 0.0),
  d_rotation(0.0),
  d_scale(1.0, 1.0),
  d_translation(0.0, 0.0)
{
}

VrmlNodeTextureTransform::~VrmlNodeTextureTransform()
{
}

bool VrmlNodeTextureTransform::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeTextureTransform* VrmlNodeTextureTransform::toTextureTransform() const 
{ return (VrmlNodeTextureTransform*) this; }


ostream& VrmlNodeTextureTransform::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_center.getX()) ||
      ! FPZERO(d_center.getY()))
    PRINT_FIELD(center);

  if (! FPZERO(d_rotation.get()))
    PRINT_FIELD(rotation);

  if (! FPEQUAL(d_scale.getX(), 1.0) ||
      ! FPEQUAL(d_scale.getY(), 1.0))
    PRINT_FIELD(scale);
  if (! FPZERO(d_translation.getX()) ||
      ! FPZERO(d_translation.getY()))
    PRINT_FIELD(translation);
      
  return os;
}


void VrmlNodeTextureTransform::render(Viewer *viewer, VrmlRenderContext rc)
{
  viewer->setTextureTransform( d_center.get(),
			       d_rotation.get(),
			       d_scale.get(),
			       d_translation.get() );
  clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeTextureTransform::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "center" ) == 0 )
    return &d_center;
  else if ( strcmp( fieldName, "rotation" ) == 0 )
    return &d_rotation;
  else if ( strcmp( fieldName, "scale" ) == 0 )
    return &d_scale;
  else if ( strcmp( fieldName, "translation" ) == 0 )
    return &d_translation;
  
  return VrmlNode::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTextureTransform::setField(const char *fieldName,
					const VrmlField &fieldValue)
{
  if TRY_FIELD(center, SFVec2f)
  else if TRY_FIELD(rotation, SFFloat)
  else if TRY_FIELD(scale, SFVec2f)
  else if TRY_FIELD(translation, SFVec2f)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

const VrmlSFVec2f & VrmlNodeTextureTransform::getCenter() const {
    return this->d_center;
}

const VrmlSFFloat & VrmlNodeTextureTransform::getRotation() const {
    return this->d_rotation;
}

const VrmlSFVec2f & VrmlNodeTextureTransform::getScale() const {
    return this->d_scale;
}

const VrmlSFVec2f & VrmlNodeTextureTransform::getTranslation() const {
    return this->d_translation;
}

// TimeSensor factory. Add each TimeSensor to the scene for fast access.

static VrmlNode * createTimeSensor(VrmlScene * scene) {
    return new VrmlNodeTimeSensor(scene);
}


// Define the built in VrmlNodeType:: "TimeSensor" fields

VrmlNodeType *VrmlNodeTimeSensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("TimeSensor", createTimeSensor);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("cycleInterval", VrmlField::SFTIME);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("loop", VrmlField::SFBOOL);
  t->addExposedField("startTime", VrmlField::SFTIME);
  t->addExposedField("stopTime", VrmlField::SFTIME);
  t->addEventOut("cycleTime", VrmlField::SFTIME);
  t->addEventOut("fraction_changed", VrmlField::SFFLOAT);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("time", VrmlField::SFTIME);

  return t;
}


VrmlNodeType & VrmlNodeTimeSensor::nodeType() const
{
    return *defineType(0);
}


VrmlNodeTimeSensor::VrmlNodeTimeSensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_cycleInterval(1.0),
  d_enabled(true),
  d_loop(false),
  d_startTime(0.0),
  d_stopTime(0.0),
  d_isActive(false),
  d_lastTime(-1.0)
{
  if (d_scene) d_scene->addTimeSensor(this);
}

VrmlNodeTimeSensor::~VrmlNodeTimeSensor()
{
  if (d_scene) d_scene->removeTimeSensor(this);
}

bool VrmlNodeTimeSensor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeTimeSensor* VrmlNodeTimeSensor::toTimeSensor() const
{
  return (VrmlNodeTimeSensor*) this;
}

void VrmlNodeTimeSensor::addToScene(VrmlScene *s, const char*)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addTimeSensor(this); }


ostream& VrmlNodeTimeSensor::printFields(ostream& os, int indent)
{
  if (! FPEQUAL(d_cycleInterval.get(), 1.0))
    PRINT_FIELD(cycleInterval);
  if (! d_enabled.get()) PRINT_FIELD(enabled);
  if (d_loop.get()) PRINT_FIELD(loop);
  if (! FPZERO(d_startTime.get())) PRINT_FIELD(startTime);
  if (! FPZERO(d_stopTime.get())) PRINT_FIELD(stopTime);

  return os;
}


//
// Generate timer events. If necessary, events prior to the timestamp (inTime)
// are generated to respect stopTimes and cycleIntervals. The timestamp
// should never be increased. This assumes the event loop delivers pending
// events in order (ascending time stamps). Should inTime be modified?
// Should ensure continuous events are delivered before discrete ones
// (such as cycleTime, isActive).

void VrmlNodeTimeSensor::update( VrmlSFTime &inTime )
{
  VrmlSFTime timeNow( inTime );

  //printf("VrmlNodeTimeSensor %s ::update( enabled %d, active %d, loop %d )\n",
  //name(), d_enabled.get(), d_isActive.get(), d_loop.get() );

  if (d_enabled.get())
    {
      if ( d_lastTime > inTime.get() )
	d_lastTime = inTime.get();

      // print the results of the tests for activation
      //printf(" startTime <= timeNow: %d\n", d_startTime.get() <= timeNow.get() );
      //printf(" startTime >= d_lastTime: %d\n", d_startTime.get() >= d_lastTime );
      //printf(" stopTime < startTime: %d\n", d_stopTime.get() < d_startTime.get() );
      //printf(" stopTime > timeNow: %d\n", d_stopTime.get() > timeNow.get() );

      // Become active at startTime if either the valid stopTime hasn't
      // passed or we are looping.
      if (! d_isActive.get() &&
	  d_startTime.get() <= timeNow.get() &&
	  d_startTime.get() >= d_lastTime &&
	  ( (d_stopTime.get() < d_startTime.get() ||
	     d_stopTime.get() > timeNow.get()) ||
	    d_loop.get() ))
	{
	  d_isActive.set(true);
	  theSystem->debug("TimeSensor.%s isActive TRUE\n", name());

	  // Start at first tick >= startTime
	  eventOut( timeNow.get(), "isActive", d_isActive );

	  eventOut( timeNow.get(), "time", timeNow );
	  eventOut( timeNow.get(), "fraction_changed", VrmlSFFloat(0.0) );
	  eventOut( timeNow.get(), "cycleTime", timeNow );

	}

      // Running (active and enabled)
      else if ( d_isActive.get() )
	{
	  double f, cycleInt = d_cycleInterval.get();
	  bool deactivate = false;

	  // Are we done? Choose min of stopTime or start + single cycle.
	  if ( (d_stopTime.get() > d_startTime.get() &&
		d_stopTime.get() <= timeNow.get()) ||
	       ( (! d_loop.get()) &&
		 d_startTime.get() + cycleInt <= timeNow.get()) )
	    {
	      d_isActive.set(false);

	      // Must respect stopTime/cycleInterval exactly
	      if (d_startTime.get() + cycleInt < d_stopTime.get())
		timeNow = VrmlSFTime(d_startTime.get() + cycleInt);
	      else
		timeNow = d_stopTime;

	      deactivate = true;
	    }

	  if (cycleInt > 0.0 && timeNow.get() > d_startTime.get())
	    f = fmod( timeNow.get() - d_startTime.get(), cycleInt );
	  else
	    f = 0.0;

	  // Fraction of cycle message
	  VrmlSFFloat fraction_changed( FPZERO(f) ? 1.0 : (f / cycleInt) );
	  eventOut( timeNow.get(), "fraction_changed", fraction_changed );

	  // Current time message
	  eventOut( timeNow.get(), "time", timeNow );

	  // End of cycle message (this may miss cycles...)
	  if ( FPEQUAL(fraction_changed.get(), 1.0) )
	    eventOut( timeNow.get(), "cycleTime", timeNow );

	  if (deactivate)
	    eventOut( timeNow.get(), "isActive", d_isActive );
	}

      // Tell the scene this node needs quick updates while it is active.
      // Should check whether time, fraction_changed eventOuts are
      // being used, and set delta to cycleTime if not...
#ifdef macintosh
      if (d_isActive.get()) d_scene->setDelta( 0.001 ); //0.0 is too fast(!)
#else
      if (d_isActive.get()) d_scene->setDelta( 0.0 );
#endif
      d_lastTime = inTime.get();

    }
}

// Ignore set_cycleInterval & set_startTime when active, deactivate
// if set_enabled FALSE is received when active.

void VrmlNodeTimeSensor::eventIn(double timeStamp,
				 const char *eventName,
				 const VrmlField & fieldValue)
{
  const char *origEventName = eventName;
  if ( strncmp(eventName, "set_", 4) == 0 )
    eventName += 4;

  //theSystem->debug("TimeSensor.%s eventIn %s\n", name(), origEventName);

  // Ignore set_cycleInterval & set_startTime when active
  if ( strcmp(eventName,"cycleInterval") == 0 ||
       strcmp(eventName,"startTime") == 0 )
    {
      if (! d_isActive.get())
	{
	  d_lastTime = timeStamp;
	  setField(eventName, fieldValue);
	  char eventOutName[256];
	  strcpy(eventOutName, eventName);
	  strcat(eventOutName, "_changed");
	  eventOut(timeStamp, eventOutName, fieldValue);
	}
    }

  // Shutdown if set_enabled FALSE is received when active
  else if ( strcmp(eventName, "enabled") == 0 )
    {
      setField(eventName, fieldValue);
      if ( d_isActive.get() && ! d_enabled.get() )
	{
	  d_isActive.set(false);

	  // Send relevant eventOuts (continuous ones first)
	  VrmlSFTime timeNow( timeStamp );
	  eventOut(timeStamp, "time", timeNow);

	  double f, cycleInt = d_cycleInterval.get();
	  if (cycleInt > 0.0)
	    f = fmod( timeNow.get() - d_startTime.get(), cycleInt );
	  else
	    f = 0.0;

	  // Fraction of cycle message
	  VrmlSFFloat fraction_changed( FPZERO(f) ? 1.0 : (f / cycleInt) );

	  eventOut(timeStamp, "fraction_changed", fraction_changed);
	  eventOut(timeStamp, "isActive", d_isActive);
	}

      eventOut(timeStamp, "enabled_changed", fieldValue);

    }

  // Let the generic code handle the rest.
  else
    VrmlNode::eventIn( timeStamp, origEventName, fieldValue );

  // TimeSensors shouldn't generate redraws.
  clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeTimeSensor::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "cycleInterval" ) == 0 )
    return &d_cycleInterval;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "loop" ) == 0 )
    return &d_loop;
  else if ( strcmp( fieldName, "startTime" ) == 0 )
    return &d_startTime;
  else if ( strcmp( fieldName, "stopTime" ) == 0 )
    return &d_stopTime;

  // eventOuts
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
#if 0				// not implemented yet...
  else if ( strcmp( fieldName, "cycleTime" ) == 0 )
    return &d_cycleTime;
  else if ( strcmp( fieldName, "fraction" ) == 0 )
    return &d_fraction;
  else if ( strcmp( fieldName, "time" ) == 0 )
    return &d_time;
#endif

  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTimeSensor::setField(const char *fieldName,
				  const VrmlField &fieldValue)
{
  if TRY_FIELD(cycleInterval, SFTime)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(loop, SFBool)
  else if TRY_FIELD(startTime, SFTime)
  else if TRY_FIELD(stopTime, SFTime)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


const VrmlBVolume* VrmlNodeTimeSensor::getBVolume() const
{
  //cout << "VrmlNodeTimeSensor::getBVolume():unset" << endl;
  static VrmlBSphere* inf_bsphere = (VrmlBSphere*)0;
  if (!inf_bsphere) {
    inf_bsphere = new VrmlBSphere();
  }
  return inf_bsphere;
}

// TouchSensor factory. 

static VrmlNode * createTouchSensor(VrmlScene * scene) {
    return new VrmlNodeTouchSensor(scene);
}


// Define the built in VrmlNodeType:: "TouchSensor" fields

VrmlNodeType *VrmlNodeTouchSensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("TouchSensor", createTouchSensor);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addEventOut("hitNormal_changed", VrmlField::SFVEC3F);
  t->addEventOut("hitPoint_changed", VrmlField::SFVEC3F);
  t->addEventOut("hitTexCoord_changed", VrmlField::SFVEC2F);
  t->addEventOut("isActive", VrmlField::SFBOOL);
  t->addEventOut("isOver", VrmlField::SFBOOL);
  t->addEventOut("touchTime", VrmlField::SFTIME);

  return t;
}

VrmlNodeType & VrmlNodeTouchSensor::nodeType() const
{
    return *defineType(0);
}


VrmlNodeTouchSensor::VrmlNodeTouchSensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_enabled(true),
  d_isActive(false),
  d_isOver(false),
  d_touchTime(0.0)
{
  setModified();
}


VrmlNodeTouchSensor::~VrmlNodeTouchSensor()
{
}

bool VrmlNodeTouchSensor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeTouchSensor* VrmlNodeTouchSensor::toTouchSensor() const
{ return (VrmlNodeTouchSensor*) this; }



ostream& VrmlNodeTouchSensor::printFields(ostream& os, int indent)
{
  if (! d_enabled.get())
    PRINT_FIELD(enabled);
  return os;
}

// Doesn't compute the xxx_changed eventOuts yet...

void VrmlNodeTouchSensor::activate( double timeStamp,
				    bool isOver, bool isActive,
				    double * )
{
  if (isOver && !isActive && d_isActive.get())
    {
      d_touchTime.set(timeStamp);
      eventOut( timeStamp, "touchTime", d_touchTime);
      //theSystem->debug("TouchSensor.%s touchTime\n", name());
    }

  if (isOver != d_isOver.get())
    {
      d_isOver.set(isOver);
      eventOut( timeStamp, "isOver", d_isOver );
    }

  if (isActive != d_isActive.get())
    {
      d_isActive.set(isActive);
      eventOut( timeStamp, "isActive", d_isActive );
    }

  // if (isOver && any routes from eventOuts)
  //   generate xxx_changed eventOuts...

}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeTouchSensor::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;

  // eventOuts
  else if ( strcmp( fieldName, "hitNormal" ) == 0 )
    return &d_hitNormal;
  else if ( strcmp( fieldName, "hitPoint" ) == 0 )
    return &d_hitPoint;
  else if ( strcmp( fieldName, "hitTexCoord" ) == 0 )
    return &d_hitTexCoord;
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;
  else if ( strcmp( fieldName, "isOver" ) == 0 )
    return &d_isOver;
  else if ( strcmp( fieldName, "touchTime" ) == 0 )
    return &d_touchTime;

  return VrmlNodeChild::getField(fieldName); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTouchSensor::setField(const char *fieldName,
				   const VrmlField &fieldValue)
{
  if TRY_FIELD(enabled, SFBool)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


static VrmlNode * createTransform(VrmlScene * scene) {
    return new VrmlNodeTransform(scene);
}


// Define the built in VrmlNodeType:: "Transform" fields

VrmlNodeType *VrmlNodeTransform::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Transform", createTransform);
      t->reference();
    }

  VrmlNodeGroup::defineType(t);	// Parent class
  t->addExposedField("center", VrmlField::SFVEC3F);
  t->addExposedField("rotation", VrmlField::SFROTATION);
  t->addExposedField("scale", VrmlField::SFVEC3F);
  t->addExposedField("scaleOrientation", VrmlField::SFROTATION);
  t->addExposedField("translation", VrmlField::SFVEC3F);

  return t;
}

VrmlNodeType & VrmlNodeTransform::nodeType() const
{
    return *defineType(0);
}


VrmlNodeTransform::VrmlNodeTransform(VrmlScene *scene) :
  VrmlNodeGroup(scene),
  d_center(0.0, 0.0, 0.0),
  d_rotation(0.0, 0.0, 1.0, 0.0),
  d_scale(1.0, 1.0, 1.0),
  d_scaleOrientation(0.0, 0.0, 1.0, 0.0),
  d_translation(0.0, 0.0, 0.0),
  d_xformObject(0)
{
  M_dirty = true;
  synch_cached_matrix();
  this->setBVolumeDirty(true);
}

VrmlNodeTransform::~VrmlNodeTransform()
{
  // delete d_xformObject...
}

bool VrmlNodeTransform::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeTransform::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        for (size_t i = 0; i < this->d_children.getLength(); ++i) {
            if (this->d_children.getElement(i)) {
                this->d_children.getElement(i)->resetVisitedFlag();
            }
        }
    }
}

VrmlNodeTransform* VrmlNodeTransform::toTransform() const //LarryD Feb24/99
{ return (VrmlNodeTransform*) this; }

const VrmlSFVec3f& VrmlNodeTransform::getCenter() const   // LarryD Feb 18/99
{  return d_center; }

const VrmlSFRotation& VrmlNodeTransform::getRotation() const  //LarryD Feb 24/99
{ return d_rotation; }

const VrmlSFVec3f& VrmlNodeTransform::getScale() const //LarryD Feb 24/99
{ return d_scale ;}

const VrmlSFRotation& VrmlNodeTransform::getScaleOrientation() const //LarryD Feb 24/99
{ return d_scaleOrientation; }

const VrmlSFVec3f& VrmlNodeTransform::getTranslation() const  //LarryD Feb 24/99
{ return d_translation; }

ostream& VrmlNodeTransform::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_center.getX()) ||
      ! FPZERO(d_center.getY()) ||
      ! FPZERO(d_center.getZ()))
    PRINT_FIELD(center);
  if (! FPZERO(d_rotation.getX()) ||
      ! FPZERO(d_rotation.getY()) ||
      ! FPEQUAL(d_rotation.getZ(), 1.0) ||
      ! FPZERO(d_rotation.getAngle()))
    PRINT_FIELD(rotation);
  if (! FPEQUAL(d_scale.getX(), 1.0) ||
      ! FPEQUAL(d_scale.getY(), 1.0) ||
      ! FPEQUAL(d_scale.getZ(), 1.0))
    PRINT_FIELD(scale);
  if (! FPZERO(d_scaleOrientation.getX()) ||
      ! FPZERO(d_scaleOrientation.getY()) ||
      ! FPEQUAL(d_scaleOrientation.getZ(), 1.0) ||
      ! FPZERO(d_scaleOrientation.getAngle()))
    PRINT_FIELD(scaleOrientation);
  if (! FPZERO(d_translation.getX()) ||
      ! FPZERO(d_translation.getY()) ||
      ! FPZERO(d_translation.getZ()))
    PRINT_FIELD(translation);

  VrmlNodeGroup::printFields(os, indent);
  return os;
}


void VrmlNodeTransform::render(Viewer *viewer, VrmlRenderContext rc)
{
  //cout << "VrmlNodeTransform::render()" << endl;
  
  if (rc.getCullFlag() != VrmlBVolume::BV_INSIDE) {

    const VrmlBSphere* bs = (VrmlBSphere*)this->getBVolume();
    VrmlBSphere bv_copy(*bs);
    bv_copy.transform(rc.getMatrix());
    int r = viewer->isectViewVolume(bv_copy);
    if (rc.getDrawBSpheres())
      viewer->drawBSphere(*bs, r);

    if (r == VrmlBVolume::BV_OUTSIDE)
      return;
    if (r == VrmlBVolume::BV_INSIDE)
      rc.setCullFlag(VrmlBVolume::BV_INSIDE);

    //rc.setCullFlag(VrmlBVolume::BV_PARTIAL);
  }

  double LM[4][4];
  synch_cached_matrix();
  this->getMatrix(LM);
  double new_LM[4][4];
  Mcopy(new_LM, rc.getMatrix());
  MM(new_LM, LM);
  rc.setMatrix(new_LM);

  if ( d_xformObject && isModified() )
    {
      viewer->removeObject(d_xformObject);
      d_xformObject = 0;
    }

  if (d_xformObject)
    viewer->insertReference(d_xformObject);

  else if (d_children.getLength() > 0)
    {
      d_xformObject = viewer->beginObject(name());

      // Apply transforms
      viewer->setTransform(d_center.get(),
			   d_rotation.get(),
			   d_scale.get(),
			   d_scaleOrientation.get(),
			   d_translation.get());

      // Render children
      VrmlNodeGroup::renderNoCull(viewer, rc);

      // Reverse transforms (for immediate mode/no matrix stack renderer)
      viewer->unsetTransform(d_center.get(),
			     d_rotation.get(),
			     d_scale.get(),
			     d_scaleOrientation.get(),
			     d_translation.get());
      viewer->endObject();
    }

  clearModified();
}


// Get the value of a field or eventOut.

const VrmlField *VrmlNodeTransform::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "center" ) == 0 )
    return &d_center;
  else if ( strcmp( fieldName, "rotation" ) == 0 )
    return &d_rotation;
  else if ( strcmp( fieldName, "scale" ) == 0 )
    return &d_scale;
  else if ( strcmp( fieldName, "scaleOrientation" ) == 0 )
    return &d_scaleOrientation;
  else if ( strcmp( fieldName, "translation" ) == 0 )
    return &d_translation;

  return VrmlNodeGroup::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeTransform::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(rotation, SFRotation)
  else if TRY_FIELD(scale, SFVec3f)
  else if TRY_FIELD(scaleOrientation, SFRotation)
  else if TRY_FIELD(translation, SFVec3f)
  else
    VrmlNodeGroup::setField(fieldName, fieldValue);
  this->setBVolumeDirty(true);
  M_dirty = true; // d_modified? see M_dirty comments in .h
}


// Cache a pointer to (one of the) parent transforms for proper
// rendering of bindables.

void VrmlNodeTransform::accumulateTransform(VrmlNode * parent) {
    d_parentTransform = parent;
    for (size_t i = 0; i < this->d_children.getLength(); ++i) {
        if (this->d_children.getElement(i)) {
            this->d_children.getElement(i)->accumulateTransform(this);
        }
    }
}


void VrmlNodeTransform::inverseTransform(Viewer *viewer)
{
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(viewer);

  // Build the inverse
  float trans[3] = { - d_translation.getX(),
		     - d_translation.getY(),
		     - d_translation.getZ() };
  float rot[4] = { d_rotation.getX(),
		   d_rotation.getY(),
		   d_rotation.getZ(),
		   -  d_rotation.getAngle() };

  // Invert scale (1/x)
  float scale[3] = { d_scale.getX(), d_scale.getY(), d_scale.getZ() };
  if (! FPZERO(scale[0])) scale[0] = 1.0 / scale[0];
  if (! FPZERO(scale[1])) scale[1] = 1.0 / scale[1];
  if (! FPZERO(scale[2])) scale[2] = 1.0 / scale[2];

  // Apply transforms (this may need to be broken into separate
  // calls to perform the ops in reverse order...)
  viewer->setTransform( d_center.get(),
			rot,
			scale,
			d_scaleOrientation.get(),
			trans );
}

void VrmlNodeTransform::inverseTransform(double m[4][4])
{
  //cout << "VrmlNodeTransform::inverseTransform()" << endl;
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(m);
  else
    Midentity(m);

  //// Invert this transform
  //float rot[4] = { d_rotation.x(),
  //d_rotation.y(),
  //d_rotation.z(),
  //- d_rotation.r() };
  //double M[4][4];
  //Mrotation( M, rot );
  //MM( m, M );
  //// Invert scale (1/x)
  //float scale[3] = { d_scale.x(), d_scale.y(), d_scale.z() };
  //if (! FPZERO(scale[0])) scale[0] = 1.0 / scale[0];
  //if (! FPZERO(scale[1])) scale[1] = 1.0 / scale[1];
  //if (! FPZERO(scale[2])) scale[2] = 1.0 / scale[2];
  //Mscale( M, scale );

  double IM[4][4];
  synch_cached_matrix();
  transform_to_matrix(this, 1, IM);
  MM( m, IM );
}


const VrmlBVolume*
VrmlNodeTransform::getBVolume() const
{
  //cout << "VrmlNodeTransform[" << this << "]::getBVolume() {" << endl;
  if (this->isBVolumeDirty())
    ((VrmlNodeTransform*)this)->recalcBSphere();
  //cout << "}:";
  //d_bsphere.dump(cout) << endl;
  return &d_bsphere;
}



void
VrmlNodeTransform::recalcBSphere()
{
  //cout << "VrmlNodeTransform[" << this << "]::recalcBSphere()" << endl;
  d_bsphere.reset();
  for (size_t i=0; i<d_children.getLength(); ++i) {
    const VrmlBVolume* ci_bv = d_children.getElement(i)->getBVolume();
    if (ci_bv)
      d_bsphere.extend(*ci_bv);
  }
  synch_cached_matrix();

  //d_bsphere.orthoTransform(M);
  d_bsphere.transform(M);

  this->setBVolumeDirty(false);
}



#if 0
void
VrmlNodeTransform::recalcBSphere()
{
  cout << "VrmlNodeTransform[" << this << "]::recalcBSphere()" << endl;
  synch_cached_matrix();
  d_bsphere.reset();
  for (int i = 0; i<d_children.size(); ++i) {
    VrmlNode* ci = d_children[i];
    const VrmlBVolume* ci_bv = ci->getBVolume();
    if (ci_bv) { // shouldn't happen...
      VrmlBSphere* bs = (VrmlBSphere*)ci_bv;
      VrmlBSphere tmp(*bs);
      tmp.transform(M);
      d_bsphere.extend(tmp);
    }
  }
  this->setBVolumeDirty(false);
}
#endif


// flag==0 is normal matrix
// flag==1 is inverse matrix
//
void
VrmlNodeTransform::transform_to_matrix(const VrmlNodeTransform* t_arg, int flag, double M[4][4])
{
  // lots and lots of fat here, but let's face it: this probably isn't
  // a bottleneck, and until it is, there's no sense in obfuscating
  // the code. so: if you feel tempted to make this "more efficient",
  // do us all a favor and run some performance tests before you commit
  // your changes.
  //
  // judicious reuse of temporaries is a goal to aim for. plus, we
  // could get tricky and reuse the inverses since they have a simple
  // relationship to the original. but see above.
  //
  Midentity(M);     // M = I

  VrmlNodeTransform* t = (VrmlNodeTransform*)t_arg; // argh.

  double NC[4][4];
  float nc[3];
  Vset(nc, t->d_center.get());
  Vscale(nc, -1);
  Mtranslation(NC, nc);
  //Mdump(cout, NC) << endl;

  double NSR[4][4];
  float nsr[4]; // just reverse the rotation, right?
  nsr[0] = t->d_scaleOrientation.getX();
  nsr[1] = t->d_scaleOrientation.getY();
  nsr[2] = t->d_scaleOrientation.getZ();
  nsr[3] = -t->d_scaleOrientation.getAngle();
  Mrotation(NSR, nsr);
  //Mdump(cout, NSR) << endl;

  double SR[4][4];
  Mrotation(SR, t->d_scaleOrientation.get());
  //Mdump(cout, SR) << endl;

  double C[4][4];
  Mtranslation(C, t->d_center.get());
  //Mdump(cout, C) << endl;


  if (flag == 0) {
    //
    // normal matrix
    //

    double S[4][4];
    Mscale(S, t->d_scale.get());
    //Mdump(cout, S) << endl;

    double R[4][4];
    Mrotation(R, t->d_rotation.get());
    //Mdump(cout, R) << endl;

    double C[4][4];
    Mtranslation(C, t->d_center.get());
    //Mdump(cout, C) << endl;

    double T[4][4];
    Mtranslation(T, t->d_translation.get());
    //Mdump(cout, T) << endl;

    MM(M, T);         // M = M * T   = T
    MM(M, C);         // M = M * C   = T * C
    MM(M, R);         // M = M * R   = T * C * R
    MM(M, SR);        // M = M * SR  = T * C * R * SR
    MM(M, S);         // M = M * S   = T * C * R * SR * S
    MM(M, NSR);       // M = M * -SR = T * C * R * SR * S * -SR
    MM(M, NC);        // M = M * -C  = T * C * R * SR * S * -SR * -C

  } else {

    //
    // inverse matrix
    //

    double NS[4][4];
    float ns[3];
    Vset(ns, t->d_scale.get());
    ns[0] = 1.0f/ns[0];
    ns[1] = 1.0f/ns[1];
    ns[2] = 1.0f/ns[2];
    Mscale(NS, ns);
    //Mdump(cout, S) << endl;

    double NR[4][4];
    float nr[4];
    nr[0] = t->d_rotation.getX();
    nr[1] = t->d_rotation.getY();
    nr[2] = t->d_rotation.getZ();
    nr[3] = -t->d_rotation.getAngle();
    Mrotation(NR, nr);
    
    double NT[4][4];
    float nt[3];
    Vset(nt, t->d_translation.get());
    Vscale(nt, -1);
    Mtranslation(NT, nt);

    MM(M, C);
    MM(M, SR);
    MM(M, NS);
    MM(M, NSR);
    MM(M, NR);
    MM(M, NC);
    MM(M, NT);
  }

}


// P' = T × C × R × SR × S × -SR × -C × P
//
void
VrmlNodeTransform::synch_cached_matrix()
{
  if (M_dirty) {
    transform_to_matrix(this, 0, M);
    //Mdump(cout, M);
    M_dirty = false;
  }
}


void
VrmlNodeTransform::getMatrix(double M_out[4][4]) const
{
  // we're logically, but not physically const
  ((VrmlNodeTransform*)this)->synch_cached_matrix();
  Mcopy(M_out, M);
}

//  Viewpoint factory.

static VrmlNode * createViewpoint(VrmlScene * scene) {
    return new VrmlNodeViewpoint(scene);
}


// Define the built in VrmlNodeType:: "Viewpoint" fields

VrmlNodeType *VrmlNodeViewpoint::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("Viewpoint", createViewpoint);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addEventIn("set_bind", VrmlField::SFBOOL);
  t->addExposedField("fieldOfView", VrmlField::SFFLOAT);
  t->addExposedField("jump", VrmlField::SFBOOL);
  t->addExposedField("orientation", VrmlField::SFROTATION);
  t->addExposedField("position", VrmlField::SFVEC3F);
  t->addField("description", VrmlField::SFSTRING);
  t->addEventOut("bindTime", VrmlField::SFTIME);
  t->addEventOut("isBound", VrmlField::SFBOOL);

  return t;
}

VrmlNodeType & VrmlNodeViewpoint::nodeType() const
{
    return *defineType(0);
}


static const float DEFAULT_FIELD_OF_VIEW = 0.785398;

VrmlNodeViewpoint::VrmlNodeViewpoint(VrmlScene *scene) :
  VrmlNodeChild(scene),
  d_fieldOfView(DEFAULT_FIELD_OF_VIEW),
  d_jump(true),
  d_orientation(0.0, 0.0, 1.0, 0.0),
  d_position(0.0, 0.0, 10.0),
  d_isBound(false),
  d_bindTime(0),
  d_parentTransform(0)
{
  if (d_scene) d_scene->addViewpoint(this);
}

// need copy constructor for d_parentTransform ...

VrmlNodeViewpoint::~VrmlNodeViewpoint()
{
  if (d_scene) d_scene->removeViewpoint(this);
}

bool VrmlNodeViewpoint::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

VrmlNodeViewpoint* VrmlNodeViewpoint::toViewpoint() const
{ return (VrmlNodeViewpoint*) this; }

void VrmlNodeViewpoint::addToScene(VrmlScene *s, const char *)
{ if (d_scene != s && (d_scene = s) != 0) d_scene->addViewpoint(this); }


ostream& VrmlNodeViewpoint::printFields(ostream& os, int indent)
{
  if (! FPEQUAL( d_fieldOfView.get(), DEFAULT_FIELD_OF_VIEW))
    PRINT_FIELD(fieldOfView);
  if (! d_jump.get()) PRINT_FIELD(jump);
  if (! FPZERO(d_orientation.getX()) ||
      ! FPZERO(d_orientation.getY()) ||
      ! FPEQUAL(d_orientation.getZ(), 1.0) ||
      ! FPZERO(d_orientation.getAngle()) )
    PRINT_FIELD(orientation);
  if (! FPZERO(d_position.getX()) ||
      ! FPZERO(d_position.getY()) ||
      ! FPEQUAL(d_position.getZ(), 10.0) )
    PRINT_FIELD(position);
  if (d_description.get()) PRINT_FIELD(description);

  return os;
}

// Cache a pointer to (one of the) parent transforms for proper
// rendering of bindables.

void VrmlNodeViewpoint::accumulateTransform( VrmlNode *parent )
{
  d_parentTransform = parent;
}

VrmlNode* VrmlNodeViewpoint::getParentTransform() { return d_parentTransform; }

// Note that this method is not maintaining isBound.

void VrmlNodeViewpoint::eventIn(double timeStamp,
				const char *eventName,
				const VrmlField & fieldValue)
{
  if (strcmp(eventName, "set_bind") == 0)
    {
      VrmlNodeViewpoint *current = d_scene->bindableViewpointTop();
      const VrmlSFBool * const b =
                dynamic_cast<const VrmlSFBool *>(&fieldValue);
      
      if (! b)
	{
	  cerr << "Error: invalid value for Viewpoint::set_bind eventIn "
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
	      const char *n = name();
	      const char *d = d_description.get();
	      if ( *n && d && *d )
		theSystem->inform("%s: %s", n, d);
	      else if ( d && *d )
		theSystem->inform("%s", d);
	      else if ( *n )
		theSystem->inform("%s", n);
	    }
	}
      else			// set_bind FALSE
	{
	  d_scene->bindableRemove( this );
	  if (this == current)
	    {
	      eventOut( timeStamp, "isBound", VrmlSFBool(false));
	      current = d_scene->bindableViewpointTop();
	      if (current)
		current->eventOut( timeStamp, "isBound", VrmlSFBool(true) );
	    }
	}

      d_bindTime.set( timeStamp );
      eventOut( timeStamp, "bindTime", d_bindTime );
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

const VrmlField *VrmlNodeViewpoint::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "fieldOfView" ) == 0 )
    return &d_fieldOfView;
  else if ( strcmp( fieldName, "jump" ) == 0 )
    return &d_jump;
  else if ( strcmp( fieldName, "orientation" ) == 0 )
    return &d_orientation;
  else if ( strcmp( fieldName, "position" ) == 0 )
    return &d_position;

  // eventOuts
  else if ( strcmp( fieldName, "bindTime" ) == 0 )
    return &d_bindTime;  
  else if ( strcmp( fieldName, "isBound" ) == 0 )
    {
      VrmlSFBool* isBound = (VrmlSFBool*) &(this->d_isBound);
      isBound->set( d_scene->bindableViewpointTop() == this );
      return isBound;
    }

  return VrmlNodeChild::getField( fieldName );
}

// Set the value of one of the node fields.

void VrmlNodeViewpoint::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(fieldOfView, SFFloat)
  else if TRY_FIELD(jump, SFBool)
  else if TRY_FIELD(orientation, SFRotation)
  else if TRY_FIELD(position, SFVec3f)
  else if TRY_FIELD(description, SFString)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


void VrmlNodeViewpoint::getInverseMatrix(double IM[4][4]) const
{
  //cout << "VrmlNodeViewpoint::getInverseMatrix()" << endl;

  double rot_mat[4][4];
  float rot_aa[4];
  rot_aa[0] =  d_orientation.getX();
  rot_aa[1] =  d_orientation.getY();
  rot_aa[2] =  d_orientation.getZ();
  rot_aa[3] = -d_orientation.getAngle();
  Mrotation(rot_mat, rot_aa);

  double pos_mat[4][4];
  float pos_vec[3];
  pos_vec[0] = -d_position.getX();
  pos_vec[1] = -d_position.getY();
  pos_vec[2] = -d_position.getZ();
  Mtranslation(pos_mat, pos_vec);

  //MM(IM, pos_mat, rot_mat);
  MM(IM, rot_mat, pos_mat);
}

void VrmlNodeViewpoint::getFrustum(VrmlFrustum& frust) const
{
  cout << "VrmlNodeViewpoint::getFrustum():WARNING:not implemented" << endl;
}


const VrmlBVolume* VrmlNodeViewpoint::getBVolume() const
{
  //cout << "VrmlNodeViewpoint::getBVolume():unset" << endl;
  static VrmlBSphere* inf_bsphere = (VrmlBSphere*)0;
  if (!inf_bsphere) {
    inf_bsphere = new VrmlBSphere();
  }
  return inf_bsphere;
}

const VrmlSFFloat & VrmlNodeViewpoint::getFieldOfView() const {
    return this->d_fieldOfView;
}

const VrmlSFRotation & VrmlNodeViewpoint::getOrientation() const {
    return this->d_orientation;
}

const VrmlSFVec3f & VrmlNodeViewpoint::getPosition() const {
    return this->d_position;
}

const VrmlSFString & VrmlNodeViewpoint::getDescription() const {
    return this->d_description;
}

// VisibilitySensor factory. 

static VrmlNode * createVisibilitySensor(VrmlScene * scene) {
    return new VrmlNodeVisibilitySensor(scene);
}


// Define the built in VrmlNodeType:: "VisibilitySensor" fields

VrmlNodeType *VrmlNodeVisibilitySensor::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;		// Only define the type once.
      t = st = new VrmlNodeType("VisibilitySensor", createVisibilitySensor);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addExposedField("center", VrmlField::SFVEC3F);
  t->addExposedField("enabled", VrmlField::SFBOOL);
  t->addExposedField("size", VrmlField::SFVEC3F);
  t->addEventOut("enterTime", VrmlField::SFTIME);
  t->addEventOut("exitTime", VrmlField::SFTIME);
  t->addEventOut("isActive", VrmlField::SFBOOL);

  return t;
}

VrmlNodeType & VrmlNodeVisibilitySensor::nodeType() const
{
    return *defineType(0);
}


VrmlNodeVisibilitySensor::VrmlNodeVisibilitySensor( VrmlScene *scene ) :
  VrmlNodeChild(scene),
  d_center(0.0, 0.0, 0.0),
  d_enabled(true),
  d_size(0.0, 0.0, 0.0),
  d_isActive(false),
  d_enterTime(0.0),
  d_exitTime(0.0)
{
  setModified();
}

VrmlNodeVisibilitySensor::~VrmlNodeVisibilitySensor()
{
}

bool VrmlNodeVisibilitySensor::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeVisibilitySensor::printFields(ostream& os, int indent)
{
  if (! FPZERO(d_center.getX()) ||
      ! FPZERO(d_center.getY()) ||
      ! FPZERO(d_center.getZ()) )
    PRINT_FIELD(center);
  if (! d_enabled.get())
    PRINT_FIELD(enabled);
  if (! FPZERO(d_size.getX()) ||
      ! FPZERO(d_size.getY()) ||
      ! FPZERO(d_size.getZ()) )
    PRINT_FIELD(size);
      
  return os;
}

//
// Generate visibility events.
//
// This is in a render() method since the it needs to be computed
// with respect to the accumulated transformations above it in the
// scene graph. Move to update() when xforms are accumulated in Groups...
//

void VrmlNodeVisibilitySensor::render(Viewer *viewer, VrmlRenderContext rc)
{

  if (d_enabled.get())
    {
      VrmlSFTime timeNow( theSystem->time() );
      float xyz[2][3];

      // hack: enclose box in a sphere...
      xyz[0][0] = d_center.getX();
      xyz[0][1] = d_center.getY();
      xyz[0][2] = d_center.getZ();
      xyz[1][0] = d_center.getX() + d_size.getX();
      xyz[1][1] = d_center.getY() + d_size.getY();
      xyz[1][2] = d_center.getZ() + d_size.getZ();
      viewer->transformPoints( 2, &xyz[0][0] );
      float dx = xyz[1][0]-xyz[0][0];
      float dy = xyz[1][1]-xyz[0][1];
      float dz = xyz[1][2]-xyz[0][2];
      float r  = dx*dx + dy*dy + dz*dz;
      if (! FPZERO(r) ) r = sqrt( r );

      // Was the sphere visible last time through? How does this work
      // for USE'd nodes? I need a way for each USE to store whether
      // it was active.
      bool wasIn = d_isActive.get();

      // Is the sphere visible? ...
      bool inside = xyz[0][2] < 0.0; // && z > - scene->visLimit()
      if (inside)
	{
	  VrmlNodeNavigationInfo *ni = d_scene->bindableNavigationInfoTop();
	  if (ni &&
	      ! FPZERO(ni->visibilityLimit()) &&
	      xyz[0][2] < - ni->visibilityLimit())
	    inside = false;
	}

      // This bit assumes 90degree fieldOfView to get rid of trig calls...
      if (inside)
	inside = ( fabs(xyz[0][0]) < -0.5 * xyz[0][2] + r &&
		   fabs(xyz[0][1]) < -0.5 * xyz[0][2] + r );

      // Just became visible
      if (inside && ! wasIn)
	{
	  theSystem->debug("VS enter %g, %g, %g\n",
			xyz[0][0], xyz[0][1], xyz[0][2]);

	  d_isActive.set(true);
	  eventOut(timeNow.get(), "isActive", d_isActive);

	  d_enterTime = timeNow;
	  eventOut(timeNow.get(), "enterTime", d_enterTime);
	}

      // Check if viewer has left the box
      else if (wasIn && ! inside)
	{
	  theSystem->debug("VS exit %g, %g, %g\n",
			xyz[0][0], xyz[0][1], xyz[0][2]);

	  d_isActive.set(false);
	  eventOut(timeNow.get(), "isActive", d_isActive );

	  d_exitTime = timeNow;
	  eventOut(timeNow.get(), "exitTime", d_exitTime);
	}
    }

  else
    clearModified();
}

// Get the value of a field or eventOut.

const VrmlField *VrmlNodeVisibilitySensor::getField(const char *fieldName) const
{
  // exposedFields
  if ( strcmp( fieldName, "center" ) == 0 )
    return &d_center;
  else if ( strcmp( fieldName, "enabled" ) == 0 )
    return &d_enabled;
  else if ( strcmp( fieldName, "size" ) == 0 )
    return &d_size;

  // eventOuts
  else if ( strcmp( fieldName, "isActive" ) == 0 )
    return &d_isActive;  
  else if ( strcmp( fieldName, "enterTime" ) == 0 )
    return &d_enterTime;  
  else if ( strcmp( fieldName, "exitTime" ) == 0 )
    return &d_exitTime;  

  return VrmlNodeChild::getField( fieldName );
}


// Set the value of one of the node fields.

void VrmlNodeVisibilitySensor::setField(const char *fieldName,
					const VrmlField &fieldValue)
{
  if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(size, SFVec3f)
  else
    VrmlNodeChild::setField(fieldName, fieldValue);
}


//  WorldInfo factory.

static VrmlNode * createWorldInfo(VrmlScene * scene) {
    return new VrmlNodeWorldInfo(scene);
}

// Define the built in VrmlNodeType:: "WorldInfo" fields

VrmlNodeType *VrmlNodeWorldInfo::defineType(VrmlNodeType *t)
{
  static VrmlNodeType *st = 0;

  if (! t)
    {
      if (st) return st;
      t = st = new VrmlNodeType("WorldInfo", createWorldInfo);
      t->reference();
    }

  VrmlNodeChild::defineType(t);	// Parent class
  t->addField("info", VrmlField::MFSTRING);
  t->addField("title", VrmlField::SFSTRING);

  return t;
}

VrmlNodeType & VrmlNodeWorldInfo::nodeType() const
{
    return *defineType(0);
}


VrmlNodeWorldInfo::VrmlNodeWorldInfo(VrmlScene *scene) :
  VrmlNodeChild(scene)
{
}

VrmlNodeWorldInfo::~VrmlNodeWorldInfo()
{
}

bool VrmlNodeWorldInfo::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

ostream& VrmlNodeWorldInfo::printFields(ostream& os, int indent)
{
  if (d_title.get()) PRINT_FIELD(title);
  if (d_info.getLength() > 0) PRINT_FIELD(info);

  return os;
}

// Set the value of one of the node fields.

void VrmlNodeWorldInfo::setField(const char *fieldName,
				 const VrmlField &fieldValue)
{
  if TRY_FIELD(info, MFString)
  else if TRY_FIELD(title, SFString)
  else
    VrmlNode::setField(fieldName, fieldValue);
}

VrmlNodeType *VrmlNodeTexture::defineType(VrmlNodeType *t)
{ return VrmlNode::defineType(t); }

VrmlNodeTexture::VrmlNodeTexture(VrmlScene *s) : VrmlNode(s) {}

VrmlNodeTexture::~VrmlNodeTexture() {}

VrmlNodeTexture* VrmlNodeTexture::toTexture() const
{ return (VrmlNodeTexture*) this; }

