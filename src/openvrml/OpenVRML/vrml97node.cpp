//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 1999  Kumaran Santhanam
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
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <winconfig.h>
#endif

# include <errno.h>
# include "vrml97node.h"
# include "VrmlNamespace.h"
# include "nodetype.h"
# include "VrmlNodeVisitor.h"
# include "VrmlNodeProto.h"
# include "Doc.h"
# include "doc2.hpp"
# include "VrmlScene.h"
# include "Audio.h"
# include "sound.h"
# include "private.h"
# include "MathUtils.h"

using namespace OpenVRML_;

/**
 * @class VrmlNodeChild
 *
 * @brief Base class for all nodes that may be children of a Group node.
 */

/**
 * @brief Constructor.
 *
 * @param scene the VrmlScene to which this node belongs
 */
VrmlNodeChild::VrmlNodeChild(const NodeType & type,
                             VrmlScene * const scene):
        VrmlNode(type, scene) {}

/**
 * @brief Copy constructor.
 */
VrmlNodeChild::VrmlNodeChild(const VrmlNodeChild & node): VrmlNode(node) {}

/**
 * @brief Downcast method.
 */
VrmlNodeChild * VrmlNodeChild::toChild() const {
    return const_cast<VrmlNodeChild *>(this);
}


/**
 * @class VrmlNodeGeometry
 *
 * @brief Base class for all geometry nodes.
 */

/**
 * @brief Constructor.
 *
 * @param scene the VrmlScene to which this node belongs
 */
VrmlNodeGeometry::VrmlNodeGeometry(const NodeType & type,
                                   VrmlScene * const scene):
        VrmlNode(type, scene), d_viewerObject(0) {}

/**
 * @brief Destructor.
 */
VrmlNodeGeometry::~VrmlNodeGeometry()
{
  /* Need access to viewer to delete viewerObject...*/
}

/**
 * @brief Downcast method.
 *
 * @return a pointer to this object
 */
VrmlNodeGeometry * VrmlNodeGeometry::toGeometry() const {
    return const_cast<VrmlNodeGeometry *>(this);
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
 * @brief Constructor.
 *
 * @param scene the VrmlScene to which this node belongs.
 */
VrmlNodeLight::VrmlNodeLight(const NodeType & type,
                             VrmlScene * const scene):
        VrmlNodeChild(type, scene), d_ambientIntensity(0.0),
        d_color(1.0, 1.0, 1.0), d_intensity(1.0), d_on(true) {}

/**
 * @brief Destructor.
 */
VrmlNodeLight::~VrmlNodeLight() {}

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
  if (! fpzero(d_ambientIntensity.get()))
    PRINT_FIELD(ambientIntensity);
  if (! fpequal(d_color.getR(), 1.0) ||
      ! fpequal(d_color.getG(), 1.0) ||
      ! fpequal(d_color.getB(), 1.0) )
    PRINT_FIELD(color);
  if (! fpequal(d_intensity.get(), 1.0))
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
const VrmlField * VrmlNodeLight::getField(const std::string & fieldId) const {
    // exposedFields
    if (fieldId == "ambientIntensity") {
        return &d_ambientIntensity;
    } else if (fieldId == "color") {
        return &d_color;
    } else if (fieldId == "intensity") {
        return &d_intensity;
    } else if (fieldId == "on") {
        return &d_on;
    }

    return VrmlNode::getField(fieldId);
}

/**
 * @brief Set the value of one of the node fields.
 *
 * @param fieldName the name of the field to set.
 * @param fieldValue the new value
 */
void VrmlNodeLight::setField(const std::string & fieldId,
                 const VrmlField & fieldValue) {
  if TRY_FIELD(ambientIntensity, SFFloat)
  else if TRY_FIELD(color, SFColor)
  else if TRY_FIELD(intensity, SFFloat)
  else if TRY_FIELD(on, SFBool)
  else 
    VrmlNode::setField(fieldId, fieldValue);
}


namespace {
    const VrmlNodePtr createAnchor(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeAnchor(scene));
    }
}

/**
 * Define the built in NodeType:: "Anchor" fields.
 */
const NodeTypePtr VrmlNodeAnchor::defineType() {
    static NodeTypePtr st(0);
    if (st) {
        return st;
    }
    
    st.reset(new NodeType("Anchor", createAnchor));
    st->addEventIn("addChildren", VrmlField::MFNODE);
    st->addEventIn("removeChildren", VrmlField::MFNODE);
    st->addExposedField("children", VrmlField::MFNODE);
    st->addField("bboxCenter", VrmlField::SFVEC3F);
    st->addField("bboxSize", VrmlField::SFVEC3F);
    st->addExposedField("description", VrmlField::SFSTRING);
    st->addExposedField("parameter", VrmlField::MFSTRING);
    st->addExposedField("url", VrmlField::MFSTRING);

    return st;
}

VrmlNodeAnchor::VrmlNodeAnchor(VrmlScene * const scene):
        VrmlNodeGroup(*defineType(), scene) {
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
  if (d_description.get().length() > 0) PRINT_FIELD(description);
  if (d_parameter.getLength() > 0) PRINT_FIELD(parameter);
  if (d_url.getLength() > 0) PRINT_FIELD(url);

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
        std::string * tmp_url_array = new std::string[d_url.getLength()];

        for (size_t i = 0; i < d_url.getLength(); i++) {
            Doc2 relDoc(d_relative.get());
            tmp_url->seturl(d_url.getElement(i).c_str(), &relDoc);
            tmp_url_array[i] = tmp_url->url();
        }

        VrmlMFString urls(d_url.getLength(), tmp_url_array);
        if (!d_scene->loadUrl(urls, d_parameter)) {
            assert(this->d_url.getLength() > 0);
            theSystem->warn("Couldn't load URL %s\n", d_url.getElement(0).c_str());
        }

        delete [] tmp_url_array;
        delete tmp_url;
    }
}


// Get the value of a field or eventOut.

const VrmlField * VrmlNodeAnchor::getField(const std::string & fieldId) const {
    // exposedFields
    if (fieldId == "description") {
        return &d_description;
    } else if (fieldId == "parameter") {
        return &d_parameter;
    } else if (fieldId == "url") {
        return &d_url;
    }

    return VrmlNodeGroup::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.
// Need to delete current values ...

void VrmlNodeAnchor::setField(const std::string & fieldId,
                  const VrmlField & fieldValue)
{
  if TRY_FIELD(description, SFString)
  else if TRY_FIELD(parameter, MFString)
  else if TRY_FIELD(url, MFString)
  else
    VrmlNodeGroup::setField(fieldId, fieldValue);
  this->setBVolumeDirty(true);
}

const VrmlBVolume*
VrmlNodeAnchor::getBVolume() const
{
  const VrmlBVolume* bv = VrmlNodeGroup::getBVolume();
  return bv;
}


namespace {
    const VrmlNodePtr createAppearance(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeAppearance(scene));
    }
}

/**
 * Define the built in NodeType:: "Appearance" fields.
 */
const NodeTypePtr VrmlNodeAppearance::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("Appearance", createAppearance));
    st->addExposedField("material", VrmlField::SFNODE);
    st->addExposedField("texture", VrmlField::SFNODE);
    st->addExposedField("textureTransform", VrmlField::SFNODE);

    return st;
}

VrmlNodeAppearance::VrmlNodeAppearance(VrmlScene * const scene):
        VrmlNode(*defineType(), scene) {}

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

void VrmlNodeAppearance::addToScene(VrmlScene * scene,
                                    const std::string & rel) {
    this->d_scene = scene;
    if (d_material.get()) d_material.get()->addToScene(scene, rel);
    if (d_texture.get()) d_texture.get()->addToScene(scene, rel);
    if (d_textureTransform.get()) {
        d_textureTransform.get()->addToScene(scene, rel);
    }
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

const VrmlField * VrmlNodeAppearance::getField(const std::string & fieldId)
        const {
    // exposedFields
    if (fieldId == "material") {
        return &d_material;
    } else if (fieldId == "texture") {
        return &d_texture;
    } else if (fieldId == "textureTransform") {
        return &d_textureTransform;
    }

    return VrmlNode::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeAppearance::setField(const std::string & fieldId,
                  const VrmlField & fieldValue)
{
  if TRY_SFNODE_FIELD(material, Material)
  else if TRY_SFNODE_FIELD(texture, Texture)
  else if TRY_SFNODE_FIELD(textureTransform, TextureTransform)
  else
    VrmlNode::setField(fieldId, fieldValue);
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


namespace {
    const VrmlNodePtr createAudioClip(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeAudioClip(scene));
    }
}

/**
 * Define the built in NodeType:: "AudioClip" fields.
 */
const NodeTypePtr VrmlNodeAudioClip::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("AudioClip", createAudioClip));
    st->addExposedField("description", VrmlField::SFSTRING);
    st->addExposedField("loop", VrmlField::SFBOOL);
    st->addExposedField("pitch", VrmlField::SFFLOAT);
    st->addExposedField("startTime", VrmlField::SFTIME);
    st->addExposedField("stopTime", VrmlField::SFTIME);
    st->addExposedField("url", VrmlField::MFSTRING);

    st->addEventOut("duration_changed", VrmlField::SFTIME);
    st->addEventOut("isActive", VrmlField::SFBOOL);

    return st;
}

VrmlNodeAudioClip::VrmlNodeAudioClip(VrmlScene * const scene):
        VrmlNode(*defineType(), scene), d_pitch(1.0), d_isActive(false),
        d_audio(0), d_url_modified(false), _audio_index(0),
        _audio_intensity(1.0), _audio_fd(-1) {
    if (this->d_scene) {
        this->d_scene->addAudioClip(*this);
    }
}

/**
 * Define copy constructor so clones don't share d_audio (for now, anyway...).
 */
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
  d_audio(0),            // can these be shared?...
  d_url_modified(false),
  _audio_index(0),
  _audio_intensity(1.0),
  _audio_fd(-1)
{
    if (this->d_scene) {
        this->d_scene->addAudioClip(*this);
    }
}


VrmlNodeAudioClip::~VrmlNodeAudioClip()
{
    delete this->d_audio;
    if (this->d_scene) {
        this->d_scene->removeAudioClip(*this);
    }
}

bool VrmlNodeAudioClip::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeAudioClip::addToScene(VrmlScene * scene, const std::string & rel) {
    if (this->d_scene != scene && (this->d_scene = scene)) {
        this->d_scene->addAudioClip(*this);
    }
    this->d_relativeUrl.set(rel);
}

VrmlNodeAudioClip* VrmlNodeAudioClip::toAudioClip() const
{ return (VrmlNodeAudioClip*)this; }


ostream& VrmlNodeAudioClip::printFields(ostream& os, int indent)
{
  if (d_description.get().length() > 0) PRINT_FIELD(description);
  if (d_loop.get()) PRINT_FIELD(loop);
  if (! fpequal(d_pitch.get(),1.0)) PRINT_FIELD(pitch);
  if (! fpzero(d_startTime.get())) PRINT_FIELD(startTime);
  if (! fpzero(d_stopTime.get())) PRINT_FIELD(stopTime);
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
        std::string emptyUrl;
        d_audio = new Audio(emptyUrl);
        if (this->d_audio->tryURLs(d_url, &relDoc))
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

const VrmlField * VrmlNodeAudioClip::getField(const std::string & fieldId)
        const {
    // exposedFields
    if (fieldId == "description") {
        return &d_description;
    } else if (fieldId == "loop") {
        return &d_loop;
    } else if (fieldId == "pitch") {
        return &d_pitch;
    } else if (fieldId == "startTime") {
        return &d_startTime;
    } else if (fieldId == "stopTime") {
        return &d_stopTime;
    } else if (fieldId == "url") {
        return &d_url;
    }

    // eventOuts
    else if (fieldId == "duration") {
        return &d_duration;
    } else if (fieldId == "isActive") {
        return &d_isActive;
    }

    return VrmlNode::getField(fieldId); // Parent class
}


// Set the value of one of the node fields.

void VrmlNodeAudioClip::setField(const std::string & fieldId,
                 const VrmlField & fieldValue) {
    if TRY_FIELD(description, SFString)
    else if TRY_FIELD(loop, SFBool)
    else if TRY_FIELD(pitch, SFFloat)
    else if TRY_FIELD(startTime, SFTime)
    else if TRY_FIELD(stopTime, SFTime)
    else if TRY_FIELD(url, MFString)
    else
      VrmlNode::setField(fieldId, fieldValue);

    if (fieldId == "url") {
        d_url_modified = true;
        setModified();
    }
}


namespace {
    //  Background factory.
    const VrmlNodePtr createBackground(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeBackground(scene));
    }
}

/**
 * Define the built in NodeType:: "Background" fields.
 */
const NodeTypePtr VrmlNodeBackground::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Define the type once.
    }
    
    st.reset(new NodeType("Background", createBackground));
    st->addEventIn("set_bind", VrmlField::SFBOOL);

    st->addExposedField("groundAngle", VrmlField::MFFLOAT);
    st->addExposedField("groundColor", VrmlField::MFCOLOR);

    st->addExposedField("backUrl", VrmlField::MFSTRING);
    st->addExposedField("bottomUrl", VrmlField::MFSTRING);
    st->addExposedField("frontUrl", VrmlField::MFSTRING);
    st->addExposedField("leftUrl", VrmlField::MFSTRING);
    st->addExposedField("rightUrl", VrmlField::MFSTRING);
    st->addExposedField("topUrl", VrmlField::MFSTRING);

    st->addExposedField("skyAngle", VrmlField::MFFLOAT);
    st->addExposedField("skyColor", VrmlField::MFCOLOR);

    st->addEventOut("isBound", VrmlField::SFBOOL);

    return st;
}

VrmlNodeBackground::VrmlNodeBackground(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_isBound(false),
        d_viewerObject(0) {
    for (size_t i = 0; i < 6; ++i) { d_texPtr[i] = 0; }
    if (this->d_scene) { this->d_scene->addBackground(*this); }
}

VrmlNodeBackground::~VrmlNodeBackground() {
    if (this->d_scene) {
        this->d_scene->removeBackground(*this);
    }
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

void VrmlNodeBackground::addToScene(VrmlScene * scene,
                                    const std::string & rel) {
    if (this->d_scene != scene && (this->d_scene = scene)) {
        this->d_scene->addBackground(*this);
    }
    this->d_relativeUrl.set(rel);
}


ostream& VrmlNodeBackground::printFields(ostream& os, int indent)
{
  if (d_groundAngle.getLength()) PRINT_FIELD(groundAngle);
  if (d_groundColor.getLength()) PRINT_FIELD(groundColor);
  if (d_skyAngle.getLength()) PRINT_FIELD(skyAngle);
  if (d_skyColor.getLength()) PRINT_FIELD(skyColor);
  if (d_backUrl.getLength() > 0) PRINT_FIELD(backUrl);
  if (d_bottomUrl.getLength() > 0) PRINT_FIELD(bottomUrl);
  if (d_frontUrl.getLength() > 0) PRINT_FIELD(frontUrl);
  if (d_leftUrl.getLength() > 0) PRINT_FIELD(leftUrl);
  if (d_rightUrl.getLength() > 0) PRINT_FIELD(rightUrl);
  if (d_topUrl.getLength() > 0) PRINT_FIELD(topUrl);

  return os;
}


// Load and scale textures as needed.

static Image * getTexture(const VrmlMFString & urls, Doc2 * relative,
                          Image * tex, int thisIndex, Viewer *viewer) {
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
          if (urls.getElement(i) == currentTex
                    || urls.getElement(i) == (currentTex + relPathLen))
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
      Doc2 *rel = d_relativeUrl.get().length() > 0 ? &relDoc : d_scene->urlDoc();
      d_texPtr[0] = getTexture( d_backUrl, rel, d_tex, 0, viewer );
      d_texPtr[1] = getTexture( d_bottomUrl, rel, d_tex, 1, viewer );
      d_texPtr[2] = getTexture( d_frontUrl, rel, d_tex, 2, viewer );
      d_texPtr[3] = getTexture( d_leftUrl, rel, d_tex, 3, viewer );
      d_texPtr[4] = getTexture( d_rightUrl, rel, d_tex, 4, viewer );
      d_texPtr[5] = getTexture( d_topUrl, rel, d_tex, 5, viewer );
    }

      int i, whc[18];    // Width, height, and nComponents for 6 tex
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
                                 const std::string & eventName,
                                 const VrmlField & fieldValue) {
  if (eventName == "set_bind")
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

      
      if ( b->get() )        // set_bind TRUE
    {
      if (this != current)
        {
          if (current)
        current->eventOut( timeStamp, "isBound", VrmlSFBool(false));
          d_scene->bindablePush( this );
          eventOut( timeStamp, "isBound", VrmlSFBool(true) );
        }
    }
      else            // set_bind FALSE
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

const VrmlField * VrmlNodeBackground::getField(const std::string & fieldId)
        const {
    // exposedFields
    if (fieldId == "groundAngle") {
        return &d_groundAngle;
    } else if (fieldId == "groundColor") {
        return &d_groundColor;
    } else if (fieldId == "backUrl") {
        return &d_backUrl;
    } else if (fieldId == "bottomUrl") {
        return &d_bottomUrl;
    } else if (fieldId == "frontUrl") {
        return &d_frontUrl;
    } else if (fieldId == "leftUrl") {
        return &d_leftUrl;
    } else if (fieldId == "rightUrl") {
        return &d_rightUrl;
    } else if (fieldId == "topUrl") {
        return &d_topUrl;
    } else if (fieldId == "skyAngle") {
        return &d_skyAngle;
    } else if (fieldId == "skyColor") {
        return &d_skyColor;
    }

    // eventOuts
    else if (fieldId == "isBound") {
        VrmlSFBool* isBound = (VrmlSFBool*) &(this->d_isBound);
        isBound->set( d_scene->bindableBackgroundTop() == this );
        return isBound;
    }

    return VrmlNodeChild::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeBackground::setField(const std::string & fieldId,
                  const VrmlField & fieldValue) {
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
    VrmlNodeChild::setField(fieldId, fieldValue);
}


namespace {
    const VrmlNodePtr createBillboard(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeBillboard(scene));
    }
}

/**
 * Define the built in NodeType:: "Billboard" fields.
 */
const NodeTypePtr VrmlNodeBillboard::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("Billboard", createBillboard));
    st->addEventIn("addChildren", VrmlField::MFNODE);
    st->addEventIn("removeChildren", VrmlField::MFNODE);
    st->addExposedField("children", VrmlField::MFNODE);
    st->addField("bboxCenter", VrmlField::SFVEC3F);
    st->addField("bboxSize", VrmlField::SFVEC3F);
    st->addExposedField("axisOfRotation", VrmlField::SFVEC3F);

    return st;
}

VrmlNodeBillboard::VrmlNodeBillboard(VrmlScene * const scene):
        VrmlNodeGroup(*defineType(), scene), d_axisOfRotation(0.0, 1.0, 0.0),
	d_xformObject(0) {}

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
  if (!fpzero(d_axisOfRotation.getX()) ||
      !fpzero(d_axisOfRotation.getY()) ||
      !fpzero(d_axisOfRotation.getZ()) )
    PRINT_FIELD(axisOfRotation);

  VrmlNodeGroup::printFields(os, indent);
  return os;
}


void VrmlNodeBillboard::render(Viewer *viewer, VrmlRenderContext rc)
{
  VrmlMatrix LM;
  VrmlMatrix new_LM = rc.getMatrix();
  billboard_to_matrix(this,new_LM,LM);
  new_LM = new_LM.MMleft(LM);
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
      d_xformObject = viewer->beginObject(this->getId().c_str());

//      viewer->setBillboardTransform( d_axisOfRotation.get() );
      viewer->MatrixMultiply(LM.get());

      // Render children
      VrmlNodeGroup::render(viewer, rc);

//      viewer->unsetBillboardTransform( d_axisOfRotation.get() );
	  LM = LM.affine_inverse();
      viewer->MatrixMultiply(LM.get());

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


// Get the value of a field or eventOut.

const VrmlField * VrmlNodeBillboard::getField(const std::string & fieldId)
        const {
    // exposedFields
    if (fieldId == "axisOfRotation") {
        return &d_axisOfRotation;
    }

    return VrmlNodeGroup::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeBillboard::setField(const std::string & fieldId,
                 const VrmlField & fieldValue) {
  if TRY_FIELD(axisOfRotation, SFVec3f)
  else
    VrmlNodeGroup::setField(fieldId, fieldValue);
}

VrmlNodeBillboard * VrmlNodeBillboard::toBillboard() const {
    return const_cast<VrmlNodeBillboard *>(this);
}

void VrmlNodeBillboard::inverseTransform(VrmlMatrix & m)
{
// It is calling program's responsibility to pass m as an unit matrix. skb
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(m);
}        

/**
 * calculate bb transformation matrix. Store it into M
 * Here we are dealing with VrmlMatrix format (Matrices are stored 
 * in row-major order), 
 * @param M gets a copy of the resulting transform
 * @param MV is input ModelView transformation matrix
 */

void VrmlNodeBillboard::billboard_to_matrix(const VrmlNodeBillboard* t_arg,
											const VrmlMatrix & L_MV, VrmlMatrix& M)
{
  VrmlNodeBillboard* t = (VrmlNodeBillboard*)t_arg; // argh.
  VrmlMatrix MV = L_MV.affine_inverse();

// Viewer position in local coordinate system
  VrmlSFVec3f VP(MV[3][0], MV[3][1], MV[3][2]);
  VrmlSFVec3f NVP = VP.normalize();

// Viewer-alignment
  if((t_arg->d_axisOfRotation[0] == 0) &&
     (t_arg->d_axisOfRotation[1] == 0) &&
	 (t_arg->d_axisOfRotation[2] == 0))
  {

// Viewer's up vector
	  VrmlSFVec3f Y(MV[1][0],MV[1][1],MV[1][2]);
	  VrmlSFVec3f NY = Y.normalize();

// get x-vector from the cross product of Viewer's
// up vector and billboard-to-viewer vector.
      VrmlSFVec3f X = NY.cross(NVP);
      M[0][0] = X[0]; M[0][1] = X[1]; M[0][2] = X[2]; M[0][3] = 0.0;
      M[1][0] = NY[0]; M[1][1] = NY[1]; M[1][2] = NY[2]; M[1][3] = 0.0;
      M[2][0] = NVP[0]; M[2][1] = NVP[1]; M[2][2] = NVP[2]; M[2][3] = 0.0,
      M[3][0] = M[3][1] = M[3][2] = 0.0; M[3][3] = 1.0;
  }
// use axis of rotation
  else
  {
// axis of rotation will be the y-axis vector
	  VrmlSFVec3f Y(t_arg->d_axisOfRotation[0],t_arg->d_axisOfRotation[1],
		            t_arg->d_axisOfRotation[2]);

// Plane defined by the axisOfRotation and billboard-to-viewer vector
	  VrmlSFVec3f X = Y.cross(VP).normalize();

// Get Z axis vector from cross product of X and Y
	  VrmlSFVec3f Z = X.cross(Y);

// Transform Z axis vector of current coordinate system to new coordinate system.
     float nz[3];
     nz[0] = X[2]; nz[1] = Y[2]; nz[2] = Z[2];

// calculate the angle by which the Z axis vector of current coordinate system
// has to be rotated around the Y axis to new coordinate system.
     float angle = acos(nz[2]);
     if(nz[0] > 0) angle = -angle;
	 VrmlSFRotation Rot(Y,angle);
     M.setRotate(Rot);
  }
}

namespace {
    // Make a VrmlNodeBox
    const VrmlNodePtr createBox(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeBox(scene));
    }
}

/**
 * Define the built in NodeType:: "Box" fields.
 */
const NodeTypePtr VrmlNodeBox::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Define type only once.
    }
    
    st.reset(new NodeType("Box", createBox));
    st->addField("size", VrmlField::SFVEC3F);

    return st;
}

VrmlNodeBox::VrmlNodeBox(VrmlScene * const scene):
        VrmlNodeGeometry(*defineType(), scene), d_size(2.0, 2.0, 2.0) {
    this->setBVolumeDirty(true); // lazy calc of bvolume
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
  if (!fpequal(d_size.getX(), 2.0)
          || !fpequal(d_size.getY(), 2.0)
          || !fpequal(d_size.getZ(), 2.0))
    os << " size " << d_size;
  return os;
}

Viewer::Object VrmlNodeBox::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
  return viewer->insertBox(d_size.getX(), d_size.getY(), d_size.getZ());
}

// Set the value of one of the node fields.

void VrmlNodeBox::setField(const std::string & fieldId,
               const VrmlField & fieldValue)
{
  if TRY_FIELD(size, SFVec3f)
  else
    VrmlNodeGeometry::setField(fieldId, fieldValue);
  setBVolumeDirty(true);
}

VrmlNodeBox* VrmlNodeBox::toBox() const //LarryD Mar 08/99
{ return (VrmlNodeBox*) this; }

const VrmlSFVec3f& VrmlNodeBox::getSize() const   // LarryD Mar 08/99
{  return d_size; }

const VrmlBVolume*
VrmlNodeBox::getBVolume() const
{
  if (this->isBVolumeDirty()) {
    float corner[3] = { d_size.getX()/2.0f, d_size.getY()/2.0f, d_size.getZ()/2.0f };
    float r = Vlength(corner);
    ((VrmlNodeBox*)this)->d_bsphere.setRadius(r);
    ((VrmlNodeBox*)this)->setBVolumeDirty(false); // logical const
  }
  return &d_bsphere;
}


namespace {
    const VrmlNodePtr createCollision(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeCollision(scene));
    }
}

/**
 * Define the built in NodeType:: "Collision" fields.
 */
const NodeTypePtr VrmlNodeCollision::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("Collision", createCollision));
    st->addEventIn("addChildren", VrmlField::MFNODE);
    st->addEventIn("removeChildren", VrmlField::MFNODE);
    st->addExposedField("children", VrmlField::MFNODE);
    st->addField("bboxCenter", VrmlField::SFVEC3F);
    st->addField("bboxSize", VrmlField::SFVEC3F);
    st->addExposedField("collide", VrmlField::SFBOOL);
    st->addField("proxy", VrmlField::SFNODE);
    st->addEventOut("collideTime", VrmlField::SFTIME);

    return st;
}

VrmlNodeCollision::VrmlNodeCollision(VrmlScene * const scene):
        VrmlNodeGroup(*defineType(), scene), d_collide(true) {}

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

const VrmlMFNode VrmlNodeCollision::getChildren() const {
    VrmlMFNode children(this->d_children);
    children.setLength(children.getLength() + 1);
    children.setElement(children.getLength() - 1, this->d_proxy.get());
    return children;
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


void VrmlNodeCollision::addToScene(VrmlScene * scene, const std::string & rel) {
    VrmlNodeGroup::addToScene(scene, rel);
    if (this->d_proxy.get()) {
        this->d_proxy.get()->addToScene(scene, rel);
    }
}

ostream& VrmlNodeCollision::printFields(ostream& os, int indent)
{
  if (! d_collide.get()) PRINT_FIELD(collide);
  if (d_proxy.get()) PRINT_FIELD(proxy);

  VrmlNodeGroup::printFields(os, indent);
  return os;
}

// Get the value of a field or eventOut.

const VrmlField * VrmlNodeCollision::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "collide")
    return &d_collide;

  // eventOuts
  else if (fieldId == "collideTime")
    return &d_collideTime;

  return VrmlNodeGroup::getField(fieldId);
}


// Set the value of one of the node fields.

void VrmlNodeCollision::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
{
  if TRY_FIELD(collide, SFBool)
  else if TRY_SFNODE_FIELD(proxy, Child)
  else
    VrmlNodeGroup::setField(fieldId, fieldValue);
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

VrmlNodeCollision * VrmlNodeCollision::toCollision() const {
    return const_cast<VrmlNodeCollision *>(this);
}


namespace {
    const VrmlNodePtr createColor(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeColor(scene));
    }
}

/**
 * Define the built in NodeType:: "Color" fields.
 */
const NodeTypePtr VrmlNodeColor::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define type once.
    }

    st.reset(new NodeType("Color", createColor));
    st->addExposedField("color", VrmlField::MFCOLOR);

    return st;
}

VrmlNodeColor::VrmlNodeColor(VrmlScene * const scene):
        VrmlNode(*defineType(), scene) {}

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

const VrmlField * VrmlNodeColor::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "color")
    return &d_color;
  return VrmlNode::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeColor::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
{
  if TRY_FIELD(color, MFColor)
  else
    VrmlNode::setField(fieldId, fieldValue);
}


namespace {
    // ColorInterpolator factory.
    const VrmlNodePtr createColorInt(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeColorInt(scene));
    }
}

/**
 * Define the built in NodeType:: "ColorInterpolator" fields.
 */
const NodeTypePtr VrmlNodeColorInt::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }

    st.reset(new NodeType("ColorInterpolator", createColorInt));
    st->addEventIn("set_fraction", VrmlField::SFFLOAT);
    st->addExposedField("key", VrmlField::MFFLOAT);
    st->addExposedField("keyValue", VrmlField::MFCOLOR);
    st->addEventOut("value_changed", VrmlField::SFCOLOR);

    return st;
}

VrmlNodeColorInt::VrmlNodeColorInt(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene) {}

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


void VrmlNodeColorInt::eventIn(double timeStamp, const std::string & eventName,
                               const VrmlField & fieldValue) {
  if (eventName == "set_fraction")
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
    {
      theSystem->error
        ("Invalid type for %s eventIn %s (expected SFFloat).\n",
         this->type.getId().c_str(), eventName.c_str());
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

const VrmlField * VrmlNodeColorInt::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "key")
    return &d_key;
  else if (fieldId == "keyValue")
    return &d_keyValue;

  // eventOuts
  else if (fieldId == "value")
    return &d_value;

  return VrmlNodeChild::getField(fieldId);
}


// Set the value of one of the node fields.

void VrmlNodeColorInt::setField(const std::string & fieldId,
                const VrmlField & fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFColor)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}


namespace {
    //  Return a new VrmlNodeCone
    const VrmlNodePtr createCone(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeCone(scene));
    }
}

/**
 * Define the built in NodeType:: "Cone" fields.
 */
const NodeTypePtr VrmlNodeCone::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define type once.
    }
    st.reset(new NodeType("Cone", createCone));
    st->addField("bottom", VrmlField::SFBOOL);
    st->addField("bottomRadius", VrmlField::SFFLOAT);
    st->addField("height", VrmlField::SFFLOAT);
    st->addField("side", VrmlField::SFBOOL);

    return st;
}

VrmlNodeCone::VrmlNodeCone(VrmlScene * const scene):
        VrmlNodeGeometry(*defineType(), scene), d_bottom(true),
        d_bottomRadius(1.0), d_height(2.0), d_side(true) {}

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

void VrmlNodeCone::setField(const std::string & fieldId,
                const VrmlField & fieldValue)
{
  if TRY_FIELD(bottom, SFBool)
  else if TRY_FIELD(bottomRadius, SFFloat)
  else if TRY_FIELD(height, SFFloat)
  else if TRY_FIELD(side, SFBool)
  else
    VrmlNodeGeometry::setField(fieldId, fieldValue);
}

VrmlNodeCone* VrmlNodeCone::toCone() const //LarryD Mar 08/99
{ return (VrmlNodeCone*) this; }


namespace {
    const VrmlNodePtr createCoordinate(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeCoordinate(scene));
    }
}

/**
 * Define the built in NodeType:: "Coordinate" fields.
 */
const NodeTypePtr VrmlNodeCoordinate::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("Coordinate", createCoordinate));
    st->addExposedField("point", VrmlField::MFVEC3F);

    return st;
}

VrmlNodeCoordinate::VrmlNodeCoordinate(VrmlScene * const scene):
        VrmlNode(*defineType(), scene) {}

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

const VrmlField * VrmlNodeCoordinate::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "point")
    return &d_point;

  return VrmlNode::getField(fieldId);
}


// Set the value of one of the node fields.

void VrmlNodeCoordinate::setField(const std::string & fieldId,
                  const VrmlField & fieldValue)
{
  if TRY_FIELD(point, MFVec3f)
  else
    VrmlNode::setField(fieldId, fieldValue);
}


namespace {
    // CoordinateInt factory.
    const VrmlNodePtr createCoordinateInt(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeCoordinateInt(scene));
    }
}

// Define the built in NodeType:: "CoordinateInterpolator" fields

const NodeTypePtr VrmlNodeCoordinateInt::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("CoordinateInterpolator",
                              createCoordinateInt));
    st->addEventIn("set_fraction", VrmlField::SFFLOAT);
    st->addExposedField("key", VrmlField::MFFLOAT);
    st->addExposedField("keyValue", VrmlField::MFVEC3F);
    st->addEventOut("value_changed", VrmlField::MFVEC3F);

    return st;
}

VrmlNodeCoordinateInt::VrmlNodeCoordinateInt(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene) {}

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
                                    const std::string & eventName,
                                    const VrmlField & fieldValue) {
  if (eventName == "set_fraction")
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
    {
      theSystem->error
        ("Invalid type for %s eventIn %s (expected SFFloat).\n",
         this->type.getId().c_str(), eventName.c_str());
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

const VrmlField * VrmlNodeCoordinateInt::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "key")
    return &d_key;
  else if (fieldId == "keyValue")
    return &d_keyValue;

  // eventOuts
  else if (fieldId == "value")
    return &d_value;

  return VrmlNodeChild::getField(fieldId);
}


// Set the value of one of the node fields.

void VrmlNodeCoordinateInt::setField(const std::string & fieldId,
                     const VrmlField & fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFVec3f)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}


namespace {
    //  Return a new VrmlNodeCylinder
    const VrmlNodePtr createCylinder(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeCylinder(scene));
    }
}

/**
 * Define the built in NodeType:: "Cylinder" fields.
 */
const NodeTypePtr VrmlNodeCylinder::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }

    st.reset(new NodeType("Cylinder", createCylinder));
    st->addField("bottom", VrmlField::SFBOOL);
    st->addField("height", VrmlField::SFFLOAT);
    st->addField("radius", VrmlField::SFFLOAT);
    st->addField("side", VrmlField::SFBOOL);
    st->addField("top", VrmlField::SFBOOL);

    return st;
}

VrmlNodeCylinder::VrmlNodeCylinder(VrmlScene * const scene):
        VrmlNodeGeometry(*defineType(), scene), d_bottom(true), d_height(2.0),
        d_radius(1.0), d_side(true), d_top(true) {}

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

void VrmlNodeCylinder::setField(const std::string & fieldId,
                const VrmlField & fieldValue)
{
  if TRY_FIELD(bottom, SFBool)
  else if TRY_FIELD(height, SFFloat)
  else if TRY_FIELD(radius, SFFloat)
  else if TRY_FIELD(side, SFBool)
  else if TRY_FIELD(top, SFBool)
  else
    VrmlNodeGeometry::setField(fieldId, fieldValue);
}

VrmlNodeCylinder* VrmlNodeCylinder::toCylinder() const //LarryD Mar 08/99
{ return (VrmlNodeCylinder*) this; }


namespace {
    // CylinderSensor factory. 
    const VrmlNodePtr createCylinderSensor(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeCylinderSensor(scene));
    }
}

/**
 * Define the built in NodeType:: "CylinderSensor" fields.
 */
const NodeTypePtr VrmlNodeCylinderSensor::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }

    st.reset(new NodeType("CylinderSensor", createCylinderSensor));
    st->addExposedField("autoOffset", VrmlField::SFBOOL);
    st->addExposedField("diskAngle", VrmlField::SFFLOAT);
    st->addExposedField("enabled", VrmlField::SFBOOL);
    st->addExposedField("maxAngle", VrmlField::SFFLOAT);
    st->addExposedField("minAngle", VrmlField::SFFLOAT);
    st->addExposedField("offset", VrmlField::SFFLOAT);
    st->addEventOut("isActive", VrmlField::SFBOOL);
    st->addEventOut("rotation_changed", VrmlField::SFROTATION);
    st->addEventOut("trackPoint_changed", VrmlField::SFVEC3F);

    return st;
}

VrmlNodeCylinderSensor::VrmlNodeCylinderSensor(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_autoOffset(true),
        d_diskAngle(0.262), d_enabled(true), d_maxAngle(-1.0), d_minAngle(0.0),
        d_offset(0.0), d_isActive(false) {
    this->setModified();
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

VrmlNodeCylinderSensor* VrmlNodeCylinderSensor::toCylinderSensor() const    // mgiger 6/16/00
{
    return (VrmlNodeCylinderSensor*) this;
}

ostream& VrmlNodeCylinderSensor::printFields(ostream& os, int indent)
{
  if (! d_autoOffset.get()) PRINT_FIELD(autoOffset);
  if (!fpequal(d_diskAngle.get(), 0.262)) PRINT_FIELD(diskAngle);
  if (! d_enabled.get())    PRINT_FIELD(enabled);
  if (!fpequal(d_maxAngle.get(), -1.0)) PRINT_FIELD(maxAngle);
  if (!fpzero(d_minAngle.get())) PRINT_FIELD(minAngle);
  if (!fpzero(d_offset.get())) PRINT_FIELD(offset);

  return os;
}

const VrmlField * VrmlNodeCylinderSensor::getField(const std::string & fieldId) const
{
  if (fieldId == "autoOffset")
    return &d_autoOffset;
  else if (fieldId == "diskAngle")
    return &d_diskAngle;
  else if (fieldId == "enabled")
    return &d_enabled;
  else if (fieldId == "maxAngle")
    return &d_maxAngle;
  else if (fieldId == "minAngle")
    return &d_minAngle;
  else if (fieldId == "offset")
    return &d_offset;

  // eventOuts
  else if (fieldId == "isActive")
    return &d_isActive;
  else if (fieldId == "rotation")
    return &d_rotation;
  else if (fieldId == "trackPoint")
    return &d_trackPoint;
  
  return VrmlNodeChild::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeCylinderSensor::setField(const std::string & fieldId,
                      const VrmlField & fieldValue)
{
  if TRY_FIELD(autoOffset, SFBool)
  else if TRY_FIELD(diskAngle, SFFloat)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(maxAngle, SFFloat)
  else if TRY_FIELD(minAngle, SFFloat)
  else if TRY_FIELD(offset, SFFloat)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}

// Store the ModelView matrix which is calculated at the time of rendering
// in render-context. This matrix will be in use at the time of activation

void VrmlNodeCylinderSensor::render(Viewer* v, VrmlRenderContext rc)
{
	setMVMatrix(rc.getMatrix());
}

void VrmlNodeCylinderSensor::activate( double timeStamp,
                    bool isActive,
                    double *p )
{
}

/**
 * Get the modelview matrix (M). 
 *
 * @param return modelview matrix in VrmlMatrix format. 
*/
const VrmlMatrix & VrmlNodeCylinderSensor::getMVMatrix() const
{
return this->M;
}

/**
 * Sets the modelview matrix (M). 
 * @param M_in a modelview matrix in VrmlMatrix format. 
*/
void VrmlNodeCylinderSensor::setMVMatrix(const VrmlMatrix & M_in)
{
this->M = M_in;
}

namespace {
    // Return a new VrmlNodeDirLight
    const VrmlNodePtr createDirLight(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeDirLight(scene));
    }
}

/**
 * Define the built in NodeType:: "DirLight" fields.
 */
const NodeTypePtr VrmlNodeDirLight::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("DirectionalLight", createDirLight));
    st->addExposedField("ambientIntensity", VrmlField::SFFLOAT);
    st->addExposedField("color", VrmlField::SFCOLOR);
    st->addExposedField("intensity", VrmlField::SFFLOAT);
    st->addExposedField("on", VrmlField::SFBOOL);
    st->addExposedField("direction", VrmlField::SFVEC3F);

    return st;
}

VrmlNodeDirLight::VrmlNodeDirLight(VrmlScene * const scene):
        VrmlNodeLight(*defineType(), scene), d_direction(0.0, 0.0, -1.0) {}

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
  if (! fpzero(d_direction.getX()) ||
      ! fpzero(d_direction.getY()) ||
      ! fpequal(d_direction.getZ(), -1.0) )
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

const VrmlField * VrmlNodeDirLight::getField(const std::string & fieldId) const
{
  if (fieldId == "direction")
    return &d_direction;
  return VrmlNodeLight::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeDirLight::setField(const std::string & fieldId,
                const VrmlField & fieldValue)
{
  if TRY_FIELD(direction, SFVec3f)
  else 
    VrmlNodeLight::setField(fieldId, fieldValue);
}

const VrmlSFVec3f& VrmlNodeDirLight::getDirection() const   // LarryD Mar 04/99
{  return d_direction; }

VrmlNodeDirLight *VrmlNodeDirLight::toDirLight() const // LarryD Mar 04/99
{ return (VrmlNodeDirLight*) this; }


namespace {
    const VrmlNodePtr createElevationGrid(VrmlScene * const scene) {
      return VrmlNodePtr(new VrmlNodeElevationGrid(scene));
    }
}

/**
 * Define the built in NodeType:: "ElevationGrid" fields.
 */
const NodeTypePtr VrmlNodeElevationGrid::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("ElevationGrid", createElevationGrid));
    st->addEventIn("set_height", VrmlField::MFFLOAT);
    st->addExposedField("color", VrmlField::SFNODE);
    st->addExposedField("normal", VrmlField::SFNODE);
    st->addExposedField("texCoord", VrmlField::SFNODE);
    st->addField("ccw", VrmlField::SFBOOL);
    st->addField("colorPerVertex", VrmlField::SFBOOL);
    st->addField("creaseAngle", VrmlField::SFFLOAT);
    st->addField("height", VrmlField::MFFLOAT);
    st->addField("normalPerVertex", VrmlField::SFBOOL);
    st->addField("solid", VrmlField::SFBOOL);
    st->addField("xDimension", VrmlField::SFINT32);
    st->addField("xSpacing", VrmlField::SFFLOAT);
    st->addField("zDimension", VrmlField::SFINT32);
    st->addField("zSpacing", VrmlField::SFFLOAT);

    return st;
}

VrmlNodeElevationGrid::VrmlNodeElevationGrid(VrmlScene * const scene):
        VrmlNodeGeometry(*defineType(), scene), d_ccw(true),
        d_colorPerVertex(true), d_normalPerVertex(true), d_solid(true),
        d_xDimension(0), d_xSpacing(1.0f), d_zDimension(0), d_zSpacing(1.0f) {}

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

const VrmlMFNode VrmlNodeElevationGrid::getChildren() const {
    VrmlNodePtr children[] = { this->d_color.get(), this->d_normal.get(),
                               this->d_texCoord.get() };
    return VrmlMFNode(3, children);
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

void VrmlNodeElevationGrid::addToScene(VrmlScene * scene,
                                       const std::string & rel) {
    this->d_scene = scene;
    if (this->d_color.get()) {
        this->d_color.get()->addToScene(scene, rel);
    }
    if (this->d_normal.get()) {
        this->d_normal.get()->addToScene(scene, rel);
    }
    if (this->d_texCoord.get()) {
        this->d_texCoord.get()->addToScene(scene, rel);
    }
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

const VrmlField * VrmlNodeElevationGrid::getField(const std::string & fieldId) const
{
  if (fieldId == "color")
    return &d_color;
  else if (fieldId == "normal")
    return &d_normal;
  else if (fieldId == "texCoord")
    return &d_texCoord;
  return VrmlNodeGeometry::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeElevationGrid::setField(const std::string & fieldId,
                     const VrmlField & fieldValue)
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
    VrmlNodeGeometry::setField(fieldId, fieldValue);
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


namespace {
    const VrmlNodePtr createExtrusion(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeExtrusion(scene));
    }
}

/**
 * Define the built in NodeType:: "Extrusion" fields.
 */
const NodeTypePtr VrmlNodeExtrusion::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }

    st.reset(new NodeType("Extrusion", createExtrusion));
    st->addEventIn("set_crossSection", VrmlField::MFVEC2F);
    st->addEventIn("set_orientation", VrmlField::MFROTATION);
    st->addEventIn("set_scale", VrmlField::MFVEC2F);
    st->addEventIn("set_spine", VrmlField::MFVEC3F);

    st->addField("beginCap", VrmlField::SFBOOL);
    st->addField("ccw", VrmlField::SFBOOL);
    st->addField("convex", VrmlField::SFBOOL);
    st->addField("creaseAngle", VrmlField::SFFLOAT);
    st->addField("crossSection", VrmlField::MFVEC2F);
    st->addField("endCap", VrmlField::SFBOOL);
    st->addField("orientation", VrmlField::MFROTATION);
    st->addField("scale", VrmlField::MFVEC2F);
    st->addField("solid", VrmlField::SFBOOL);
    st->addField("spine", VrmlField::MFVEC3F);

    return st;
}

namespace {
    const float extrusionDefaultCrossSection_[] =
            { 1.0, 1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0 };
    const float extrusionDefaultScale_[] =
            { 1.0, 1.0 };
    const float extrusionDefaultSpine_[] =
            { 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 };
}

VrmlNodeExtrusion::VrmlNodeExtrusion(VrmlScene * const scene):
        VrmlNodeGeometry(*defineType(), scene), d_beginCap(true), d_ccw(true),
        d_convex(true), d_creaseAngle(0),
        d_crossSection(5, extrusionDefaultCrossSection_),
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

void VrmlNodeExtrusion::setField(const std::string & fieldId,
                                 const VrmlField & fieldValue)
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
    VrmlNodeGeometry::setField(fieldId, fieldValue);
}

VrmlNodeExtrusion* VrmlNodeExtrusion::toExtrusion() const
{ return (VrmlNodeExtrusion*) this; }


namespace {
    //  Fog factory.
    //  Since Fog is a bindable child node, the first one created needs
    //  to notify its containing scene.
    const VrmlNodePtr createFog(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeFog(scene));
    }
}

/**
 * Define the built in NodeType:: "Fog" fields.
 */
const NodeTypePtr VrmlNodeFog::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }

    st.reset(new NodeType("Fog", createFog));
    st->addEventIn("set_bind", VrmlField::SFBOOL);
    st->addExposedField("color", VrmlField::SFCOLOR);
    st->addExposedField("fogType", VrmlField::SFSTRING);
    st->addExposedField("visibilityRange", VrmlField::SFFLOAT);
    st->addEventOut("isBound", VrmlField::SFBOOL);

    return st;
}

VrmlNodeFog::VrmlNodeFog(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_color(1.0, 1.0, 1.0),
        d_fogType("LINEAR"), d_visibilityRange(0.0), d_isBound(false) {
    if (this->d_scene) {
        this->d_scene->addFog(*this);
    }
}

VrmlNodeFog::~VrmlNodeFog()
{
    if (this->d_scene) {
        this->d_scene->removeFog(*this);
    }
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


void VrmlNodeFog::addToScene(VrmlScene * scene, const std::string &) {
    if (this->d_scene != scene && (this->d_scene = scene)) {
        this->d_scene->addFog(*this);
    }
}


ostream& VrmlNodeFog::printFields(ostream& os, int indent)
{
  if (d_color.getR() != 1.0 ||
      d_color.getG() != 1.0 ||
      d_color.getB() != 1.0 )
    PRINT_FIELD(color);
  if (d_fogType.get() == "LINEAR")
    PRINT_FIELD(fogType);
  if (d_visibilityRange.get() != 0.0)
    PRINT_FIELD(visibilityRange);

  return os;
}

// Note that this method is not maintaining isBound.

void VrmlNodeFog::eventIn(double timeStamp, const std::string & eventName,
                          const VrmlField & fieldValue) {
  if (eventName == "set_bind")
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

      if ( b->get() )        // set_bind TRUE
    {
      if (this != current)
        {
          if (current)
        current->eventOut( timeStamp, "isBound", VrmlSFBool(false));
          d_scene->bindablePush( this );
          eventOut( timeStamp, "isBound", VrmlSFBool(true) );
        }
    }
      else            // set_bind FALSE
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

const VrmlField * VrmlNodeFog::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "color")
    return &d_color;
  else if (fieldId == "fogType")
    return &d_fogType;
  else if (fieldId == "visibilityRange")
    return &d_visibilityRange;
  
  // eventOuts
  else if (fieldId == "isBound")
    {
      VrmlSFBool* isBound = (VrmlSFBool*) &(this->d_isBound);
      isBound->set( d_scene->bindableFogTop() == this );
      return isBound;
    }

  return VrmlNodeChild::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeFog::setField(const std::string & fieldId,
               const VrmlField & fieldValue)
{
  if TRY_FIELD(color, SFColor)
  else if TRY_FIELD(fogType, SFString)
  else if TRY_FIELD(visibilityRange, SFFloat)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}


namespace {
    const VrmlNodePtr createFontStyle(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeFontStyle(scene));
    }
}

/**
 * Define the built in NodeType:: "FontStyle" fields.
 */
const NodeTypePtr VrmlNodeFontStyle::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }

    st.reset(new NodeType("FontStyle", createFontStyle));
    st->addField("family", VrmlField::MFSTRING);
    st->addField("horizontal", VrmlField::SFBOOL);
    st->addField("justify", VrmlField::MFSTRING);
    st->addField("language", VrmlField::SFSTRING);
    st->addField("leftToRight", VrmlField::SFBOOL);
    st->addField("size", VrmlField::SFFLOAT);
    st->addField("spacing", VrmlField::SFFLOAT);
    st->addField("style", VrmlField::SFSTRING);
    st->addField("topToBottom", VrmlField::SFBOOL);

    return st;
}

namespace {
    const std::string fontStyleInitFamily_[] = { "SERIF" };
    const std::string fontStyleInitJustify_[] = { "BEGIN" };
}

VrmlNodeFontStyle::VrmlNodeFontStyle(VrmlScene * const scene):
        VrmlNode(*defineType(), scene), d_family(1, fontStyleInitFamily_),
        d_horizontal(true), d_justify(1, fontStyleInitJustify_),
        d_leftToRight(true), d_size(1.0), d_spacing(1.0), d_style("PLAIN"),
        d_topToBottom(true) {}

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
  if (d_family.getLength() > 1
        || (d_family.getLength() == 1 && d_family.getElement(0) != "SERIF"))
    PRINT_FIELD(family);
  if (! d_horizontal.get()) PRINT_FIELD(horizontal);
  if (d_justify.getLength() > 1
        || (d_justify.getLength() == 1 && d_justify.getElement(0) != "BEGIN"))
    PRINT_FIELD(justify);
  if (d_language.get().length() > 0)
    PRINT_FIELD(language);
  if (! d_leftToRight.get()) PRINT_FIELD(leftToRight);
  if (! fpequal(d_size.get(), 1.0)) PRINT_FIELD(size);
  if (! fpequal(d_spacing.get(), 1.0)) PRINT_FIELD(spacing);
  if (d_style.get().length() > 0 && d_style.get() != "PLAIN")
    PRINT_FIELD(style);
  if (! d_topToBottom.get()) PRINT_FIELD(topToBottom);

  return os;
}


// Set the value of one of the node fields.

void VrmlNodeFontStyle::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
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
    VrmlNode::setField(fieldId, fieldValue);
}


namespace {
    // Return a new VrmlNodeGroup
    const VrmlNodePtr createGroup(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeGroup(scene));
    }
}

/**
 * Define the built in NodeType:: "Group" fields.
 */
const NodeTypePtr VrmlNodeGroup::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("Group", createGroup));
    st->addEventIn("addChildren", VrmlField::MFNODE);
    st->addEventIn("removeChildren", VrmlField::MFNODE);
    st->addExposedField("children", VrmlField::MFNODE);
    st->addField("bboxCenter", VrmlField::SFVEC3F);
    st->addField("bboxSize", VrmlField::SFVEC3F);

    return st;
}

VrmlNodeGroup::VrmlNodeGroup(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_bboxSize(-1.0, -1.0, -1.0),
        d_parentTransform(0), d_viewerObject(0) {
    this->setBVolumeDirty(true);
}

VrmlNodeGroup::VrmlNodeGroup(const NodeType & type,
                             VrmlScene * const scene):
        VrmlNodeChild(type, scene), d_bboxSize(-1.0, -1.0, -1.0),
        d_parentTransform(0), d_viewerObject(0) {
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
  if (this->isModified()) markPathModified(path, true, flags);
  path.push_front(this);
  int n = d_children.getLength();
  for (int i=0; i<n; ++i) {
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


void VrmlNodeGroup::addToScene(VrmlScene * scene,
                               const std::string & relativeUrl) {
    this->d_scene = scene;

    const std::string & currentRel = this->d_relative.get();
    if (currentRel.length() == 0 || relativeUrl.length() == 0
            || currentRel != relativeUrl) {
        this->d_relative.set(relativeUrl);
    }

    for (size_t i = 0; i < this->d_children.getLength(); ++i) {
        this->d_children.getElement(i)->addToScene(scene,
                                                   this->d_relative.get());
    }
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
  }

  renderNoCull(viewer, rc);
}


// because children will already have done the culling, we don't need
// to repeat it here.
//
void VrmlNodeGroup::renderNoCull(Viewer *viewer, VrmlRenderContext rc)
{
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

      d_viewerObject = viewer->beginObject(this->getId().c_str());

      // Draw nodes that impact their siblings (DirectionalLights,
      // TouchSensors, any others? ...)
      for (i = 0; i<n; ++i)
    {
      const VrmlNodePtr & kid = d_children.getElement(i);

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
        const VrmlNodePtr & child = this->d_children.getElement(i);
    if (! (child->toLight() ||
//           child->toPlaneSensor() ||
//           child->toCylinderSensor() ||
//           child->toSphereSensor() ||
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
      const VrmlNodePtr & kid = d_children.getElement(i);

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
const VrmlMFNode VrmlNodeGroup::getChildren() const {
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
        const VrmlNodePtr & child = children.getElement(i);
        if (child && (child->toChild()
                || ((p = dynamic_cast<VrmlNodeProto *>(child.get()))
                    && p->getImplNodes().getLength() == 0))) {
            child->addToScene(d_scene, d_relative.get());
            child->accumulateTransform(d_parentTransform);
        } else {
            theSystem ->error(
                "Error: Attempt to add a %s node as a child of a %s node.\n",
                child->type.getId().c_str(), this->type.getId().c_str());
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
        const VrmlNodePtr & child = children.getElement(i);
        VrmlNodeProto *p = 0;
        
        if (child && (child->toChild()
                || ((p = dynamic_cast<VrmlNodeProto *>(child.get()))
                    && p->getImplNodes().getLength() == 0))) {
            d_children.addNode(*child);
            child->addToScene( d_scene, d_relative.get() );
            child->accumulateTransform( d_parentTransform );
        } else {
            theSystem->error(
                "Error: Attempt to add a %s node as a child of a %s node.\n",
                child->type.getId().c_str(), this->type.getId().c_str());
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


void VrmlNodeGroup::eventIn(double timeStamp, const std::string & eventName,
                            const VrmlField & fieldValue) {
  if (eventName == "addChildren")
    {
      if (dynamic_cast<const VrmlMFNode *>(&fieldValue))      // check that fieldValue is MFNode
    addChildren(static_cast<const VrmlMFNode &>(fieldValue));
      else
    theSystem->error("VrmlNodeGroup.%s %s eventIn invalid field type.\n",
                 this->getId().c_str(), eventName.c_str());
    }

  else if (eventName == "removeChildren")
    {
      if (dynamic_cast<const VrmlMFNode *>(&fieldValue))      // check that fieldValue is MFNode
    removeChildren(static_cast<const VrmlMFNode &>(fieldValue));
      else
    theSystem->error("VrmlNodeGroup.%s %s eventIn invalid field type.\n",
                 this->getId().c_str(), eventName.c_str());
    }

  else
    {
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);
    }
}

// Get the value of one of the exposedFields or eventOuts.

const VrmlField * VrmlNodeGroup::getField(const std::string & fieldId) const
{
  if (fieldId == "children")
    return &d_children;

  return VrmlNodeChild::getField(fieldId);
}

// Set the value of one of the node fields.
void VrmlNodeGroup::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
{
  if TRY_FIELD(bboxCenter, SFVec3f)
  else if TRY_FIELD(bboxSize, SFVec3f)
  else if TRY_FIELD(children, MFNode)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
  this->setBVolumeDirty(true); // overly conservative?
}

const VrmlBVolume* VrmlNodeGroup::getBVolume() const
{
  if (this->isBVolumeDirty())
    ((VrmlNodeGroup*)this)->recalcBSphere();
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


namespace {
    const VrmlNodePtr createIFaceSet(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeIFaceSet(scene));
    }
}

/**
 * Define the built in NodeType:: "IndexedFaceSet" fields.
 */
const NodeTypePtr VrmlNodeIFaceSet::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("IndexedFaceSet", createIFaceSet));
    st->addEventIn("set_colorIndex", VrmlField::MFINT32);
    st->addEventIn("set_coordIndex", VrmlField::MFINT32);
    st->addExposedField("color", VrmlField::SFNODE);
    st->addExposedField("coord", VrmlField::SFNODE);
    st->addField("colorIndex", VrmlField::MFINT32);
    st->addField("colorPerVertex", VrmlField::SFBOOL);
    st->addField("coordIndex", VrmlField::MFINT32);
    st->addEventIn("set_normalIndex", VrmlField::MFINT32);
    st->addEventIn("set_texCoordIndex", VrmlField::MFINT32);
    st->addExposedField("normal", VrmlField::SFNODE);
    st->addExposedField("texCoord", VrmlField::SFNODE);
    st->addField("ccw", VrmlField::SFBOOL);
    st->addField("convex", VrmlField::SFBOOL);
    st->addField("creaseAngle", VrmlField::SFFLOAT);
    st->addField("normalIndex", VrmlField::MFINT32);
    st->addField("normalPerVertex", VrmlField::SFBOOL);
    st->addField("solid", VrmlField::SFBOOL);
    st->addField("texCoordIndex", VrmlField::MFINT32);

    return st;
}

VrmlNodeIFaceSet::VrmlNodeIFaceSet(VrmlScene * const scene):
        VrmlNodeIndexedSet(*defineType(), scene), d_ccw(true), d_convex(true),
        d_creaseAngle(0.0), d_normalPerVertex(true), d_solid(true) {
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

const VrmlMFNode VrmlNodeIFaceSet::getChildren() const {
    VrmlNodePtr children[] = { this->d_color.get(), this->d_coord.get(),
                               this->d_normal.get(), this->d_texCoord.get() };
    return VrmlMFNode(4, children);
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

void VrmlNodeIFaceSet::addToScene(VrmlScene * scene, const std::string & rel) {
    this->d_scene = scene;
    if (this->d_color.get()) {
        d_color.get()->addToScene(scene, rel);
    }
    if (this->d_coord.get()) {
        this->d_coord.get()->addToScene(scene, rel);
    }
    if (this->d_normal.get()) {
        this->d_normal.get()->addToScene(scene, rel);
    }
    if (this->d_texCoord.get()) {
        this->d_texCoord.get()->addToScene(scene, rel);
    }
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
      size_t ntci = 0; const long * tci = 0;    // texture coordinate indices
      int nci = 0; const long * ci = 0;    // color indices
      int nni = 0; const long * ni = 0;    // normal indices

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

const VrmlField * VrmlNodeIFaceSet::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "normal")
    return &d_normal;
  else if (fieldId == "texCoord")
    return &d_texCoord;
  
  return VrmlNodeIndexedSet::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeIFaceSet::setField(const std::string & fieldId,
                const VrmlField & fieldValue)
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
    VrmlNodeIndexedSet::setField(fieldId, fieldValue);

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
  this->setBVolumeDirty(false);
}


const VrmlBVolume* VrmlNodeIFaceSet::getBVolume() const
{
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


namespace {
    const VrmlNodePtr createILineSet(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeILineSet(scene));
    }
}

/**
 * Define the built in NodeType:: "IndexedLineSet" fields.
 */
const NodeTypePtr VrmlNodeILineSet::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }

    st.reset(new NodeType("IndexedLineSet", createILineSet));
    st->addEventIn("set_colorIndex", VrmlField::MFINT32);
    st->addEventIn("set_coordIndex", VrmlField::MFINT32);
    st->addExposedField("color", VrmlField::SFNODE);
    st->addExposedField("coord", VrmlField::SFNODE);
    st->addField("colorIndex", VrmlField::MFINT32);
    st->addField("colorPerVertex", VrmlField::SFBOOL);
    st->addField("coordIndex", VrmlField::MFINT32);

    return st;
}

VrmlNodeILineSet::VrmlNodeILineSet(VrmlScene *const scene):
        VrmlNodeIndexedSet(*defineType(), scene) {}

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


namespace {
    const VrmlNodePtr createImageTexture(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeImageTexture(scene));
    }
}

const VrmlMFString& VrmlNodeImageTexture::getUrl() const 
{   return d_url; }

VrmlNodeImageTexture* VrmlNodeImageTexture::toImageTexture() const
{ return (VrmlNodeImageTexture*) this; }

/**
 * Define the built in NodeType:: "ImageTexture" fields.
 */
const NodeTypePtr VrmlNodeImageTexture::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }

    st.reset(new NodeType("ImageTexture", createImageTexture));
    st->addExposedField("url", VrmlField::MFSTRING);
    st->addField("repeatS", VrmlField::SFBOOL);
    st->addField("repeatT", VrmlField::SFBOOL);

    return st;
}

VrmlNodeImageTexture::VrmlNodeImageTexture(VrmlScene * const scene) :
        VrmlNodeTexture(*defineType(), scene), d_repeatS(true), d_repeatT(true),
        d_image(0), d_texObject(0) {}

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
  if (d_url.getLength() > 0) PRINT_FIELD(url);
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
      delete d_image;        // URL is the only modifiable bit
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
      const std::string relUrl = d_relativeUrl.get().length() > 0
                                    ? d_relativeUrl.get()
                                    : d_scene->urlDoc()->url();
      Doc relDoc(relUrl, static_cast<Doc const *>(0));
      d_image = new Image;
      if (!d_image->tryURLs(d_url, &relDoc))
          theSystem->error("Couldn't read ImageTexture from URL %s\n", d_url.getElement(0).c_str());
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

const VrmlField * VrmlNodeImageTexture::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "url")
    return &d_url;
  
  return VrmlNode::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeImageTexture::setField(const std::string & fieldId,
                    const VrmlField & fieldValue) {
  if (fieldId == "url")
    {
      delete d_image;
      d_image = 0;
    }

  if TRY_FIELD(url, MFString)
  else if TRY_FIELD(repeatS, SFBool)
  else if TRY_FIELD(repeatT, SFBool)
  else
    VrmlNode::setField(fieldId, fieldValue);
}

bool VrmlNodeImageTexture::getRepeatS() const
{
    return d_repeatS.get();
}

bool VrmlNodeImageTexture::getRepeatT() const
{
    return d_repeatT.get();
}

VrmlNodeIndexedSet::VrmlNodeIndexedSet(const NodeType & type,
                                       VrmlScene * const scene):
        VrmlNodeGeometry(type, scene), d_colorPerVertex(true) {}

VrmlNodeIndexedSet::~VrmlNodeIndexedSet()
{
}

const VrmlMFNode VrmlNodeIndexedSet::getChildren() const {
    VrmlNodePtr children[] = { this->d_color.get(), this->d_coord.get() };
    return VrmlMFNode(2, children);
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

void VrmlNodeIndexedSet::addToScene(VrmlScene * scene,
                                    const std::string & rel) {
    this->d_scene = scene;
    if (this->d_color.get()) {
        this->d_color.get()->addToScene(scene, rel);
    }
    if (this->d_coord.get()) {
        this->d_coord.get()->addToScene(scene, rel);
    }
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

const VrmlField * VrmlNodeIndexedSet::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "color")
    return &d_color;
  else if (fieldId == "coord")
    return &d_coord;
  
  return VrmlNodeGeometry::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeIndexedSet::setField(const std::string & fieldId,
                  const VrmlField & fieldValue)
{
  if TRY_SFNODE_FIELD(color, Color)
  else if TRY_FIELD(colorIndex, MFInt32)
  else if TRY_FIELD(colorPerVertex, SFBool)
  else if TRY_SFNODE_FIELD(coord, Coordinate)
  else if TRY_FIELD(coordIndex, MFInt32)
  else
    VrmlNodeGeometry::setField(fieldId, fieldValue);
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


namespace {
    const VrmlNodePtr createInline(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeInline(scene));
    }
}

/**
 * Define the built in NodeType:: "Inline" fields.
 */
const NodeTypePtr VrmlNodeInline::defineType() {
    static NodeTypePtr st(0);
    if (st) {
        return st;
    }

    st.reset(new NodeType("Inline", createInline));
    // Having Inline a subclass of Group is not right since
    // Groups have an exposedField "children" and eventIns
    // addChildren/deleteChildren that Inlines don't support...
    st->addField("bboxCenter", VrmlField::SFVEC3F);
    st->addField("bboxSize", VrmlField::SFVEC3F);
    st->addExposedField("url", VrmlField::MFSTRING);

    return st;
}

VrmlNodeInline::VrmlNodeInline(VrmlScene * const scene):
        VrmlNodeGroup(*defineType(), scene), d_namespace(0),
        d_hasLoaded(false) {
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

void VrmlNodeInline::addToScene(VrmlScene * scene,
                                const std::string & relativeUrl) {
    this->d_scene = scene;
    this->load(relativeUrl);
    VrmlNodeGroup::addToScene(scene, relativeUrl);
}


ostream& VrmlNodeInline::printFields(ostream& os, int indent)
{
  if ( !fpzero(d_bboxCenter.getX()) ||
       !fpzero(d_bboxCenter.getY()) ||
       !fpzero(d_bboxCenter.getZ()) )
    PRINT_FIELD(bboxCenter);

  if ( !fpequal(d_bboxSize.getX(), -1) ||
       !fpequal(d_bboxSize.getY(), -1) ||
       !fpequal(d_bboxSize.getZ(), -1) )
    PRINT_FIELD(bboxCenter);

  if (d_url.getLength() > 0) PRINT_FIELD(url);

  return os;
}

// Set the value of one of the exposed fields.

const VrmlField * VrmlNodeInline::getField(const std::string & fieldId) const
{
  if (fieldId == "url")
    return &d_url;

  return VrmlNodeGroup::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeInline::setField(const std::string & fieldId,
                  const VrmlField & fieldValue)
{
  if TRY_FIELD(url, MFString)
  else
    VrmlNodeGroup::setField(fieldId, fieldValue);
}

//  Load the children from the URL

void VrmlNodeInline::load(const std::string & relativeUrl)
{
  // Already loaded? Need to check whether Url has been modified...
  if (this->d_hasLoaded) {
      return;
  }

  this->d_hasLoaded = true; // although perhaps not successfully
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
                           d_url.getElement(i).c_str(),
                           d_relative.get().length() > 0
                                ? d_relative.get().c_str() : "<null>");
          url.seturl(d_url.getElement(i).c_str(), &relDoc);

      kids = VrmlScene::readWrl( &url, ns );
      if ( kids )
        break;
      else if (i < n-1 && strncmp(d_url.getElement(i).c_str(), "urn:", 4))
        theSystem->warn("Couldn't read url '%s': %s\n",
                d_url.getElement(i).c_str(), strerror( errno));
    }

      if ( kids )
    {
      delete d_namespace;
      d_namespace = ns;
      d_relative.set(url.url()); // children will be relative to this url

      removeChildren();
      addChildren( *kids );     // check for nested Inlines
      delete kids;
    }
      else
    {
      theSystem->warn("couldn't load Inline %s (relative %s)\n",
              d_url.getElement(0).c_str(),
              d_relative.get().length() > 0 ? d_relative.get().c_str() : "<null>");
      delete ns;
    }
    }
}


namespace {
    // Return a new VrmlNodeLOD
    const VrmlNodePtr createLOD(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeLOD(scene));
    }
}

/**
 * Define the built in NodeType:: "LOD" fields.
 */
const NodeTypePtr VrmlNodeLOD::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("LOD", createLOD));
    st->addExposedField("level", VrmlField::MFNODE);
    st->addField("center", VrmlField::SFVEC3F);
    st->addField("range", VrmlField::MFFLOAT);

    return st;
}

VrmlNodeLOD::VrmlNodeLOD(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene) {
    this->setBVolumeDirty(true); // lazy calc of bvolume
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

const VrmlMFNode VrmlNodeLOD::getChildren() const {
    return this->d_level;
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

void VrmlNodeLOD::addToScene(VrmlScene * scene, const std::string & rel) {
    this->d_scene = scene;
    for (size_t i = 0; i < this->d_level.getLength(); ++i) {
        this->d_level.getElement(i)->addToScene(scene, rel);
    }
}

ostream& VrmlNodeLOD::printFields(ostream& os, int indent)
{
  if (d_level.getLength() > 0) PRINT_FIELD(level);
  if (! fpzero(d_center.getX()) ||
      ! fpzero(d_center.getY()) ||
      ! fpzero(d_center.getZ()) )
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
const VrmlField * VrmlNodeLOD::getField(const std::string & fieldId) const
{
  if (fieldId == "level")
    return &d_level;

  return VrmlNodeChild::getField(fieldId);
}

// Set the value of one of the node fields.
void VrmlNodeLOD::setField(const std::string & fieldId,
               const VrmlField & fieldValue)
{
  if TRY_FIELD(level, MFNode)
  else if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(range, MFFloat)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
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
  if (this->isBVolumeDirty())
    ((VrmlNodeLOD*)this)->recalcBSphere();
  return &d_bsphere;
}


void
VrmlNodeLOD::recalcBSphere()
{
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


namespace {
    const VrmlNodePtr createMaterial(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeMaterial(scene));
    }
}

/**
 * Define the built in NodeType:: "Material" fields.
 */
const NodeTypePtr VrmlNodeMaterial::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("Material", createMaterial));
    st->addExposedField("ambientIntensity", VrmlField::SFFLOAT);
    st->addExposedField("diffuseColor", VrmlField::SFCOLOR);
    st->addExposedField("emissiveColor", VrmlField::SFCOLOR);
    st->addExposedField("shininess", VrmlField::SFFLOAT);
    st->addExposedField("specularColor", VrmlField::SFCOLOR);
    st->addExposedField("transparency", VrmlField::SFFLOAT);

    return st;
}

VrmlNodeMaterial::VrmlNodeMaterial(VrmlScene * const scene):
        VrmlNode(*defineType(), scene), d_ambientIntensity(0.2),
        d_diffuseColor(0.8, 0.8, 0.8), d_emissiveColor(0.0, 0.0, 0.0),
        d_shininess(0.2), d_specularColor(0.0, 0.0, 0.0), d_transparency(0.0) {}

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
  if (! fpequal(d_ambientIntensity.get(), 0.2))
    PRINT_FIELD(ambientIntensity);

  if (! fpequal(d_diffuseColor.getR(), 0.8) ||
      ! fpequal(d_diffuseColor.getG(), 0.8) ||
      ! fpequal(d_diffuseColor.getB(), 0.8) )
    PRINT_FIELD(diffuseColor);

  if (! fpzero(d_emissiveColor.getR()) ||
      ! fpzero(d_emissiveColor.getG()) ||
      ! fpzero(d_emissiveColor.getB()) )
    PRINT_FIELD(emissiveColor);

  if (! fpequal(d_shininess.get(), 0.2))
    PRINT_FIELD(shininess);

  if (! fpzero(d_specularColor.getR()) ||
      ! fpzero(d_specularColor.getG()) ||
      ! fpzero(d_specularColor.getB()) )
    PRINT_FIELD(specularColor);

  if (! fpzero(d_transparency.get()) )
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

const VrmlField * VrmlNodeMaterial::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "ambientIntensity")
    return &d_ambientIntensity;
  else if (fieldId == "diffuseColor")
    return &d_diffuseColor;
  else if (fieldId == "emissiveColor")
    return &d_emissiveColor;
  else if (fieldId == "shininess")
    return &d_shininess;
  else if (fieldId == "specularColor")
    return &d_specularColor;
  else if (fieldId == "transparency")
    return &d_transparency;
  
  return VrmlNode::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeMaterial::setField(const std::string & fieldId,
                const VrmlField & fieldValue)
{
  if TRY_FIELD(ambientIntensity, SFFloat)
  else if TRY_FIELD(diffuseColor, SFColor)
  else if TRY_FIELD(emissiveColor, SFColor)
  else if TRY_FIELD(shininess, SFFloat)
  else if TRY_FIELD(specularColor, SFColor)
  else if TRY_FIELD(transparency, SFFloat)
  else
    VrmlNode::setField(fieldId, fieldValue);
}


namespace {
    const VrmlNodePtr createMovieTexture(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeMovieTexture(scene));
    }
}

/**
 * Define the built in NodeType:: "MovieTexture" fields.
 */
const NodeTypePtr VrmlNodeMovieTexture::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("MovieTexture", createMovieTexture));
    st->addExposedField("loop", VrmlField::SFBOOL);
    st->addExposedField("speed", VrmlField::SFFLOAT);
    st->addExposedField("startTime", VrmlField::SFTIME);
    st->addExposedField("stopTime", VrmlField::SFTIME);
    st->addExposedField("url", VrmlField::MFSTRING);
    st->addField("repeatS", VrmlField::SFBOOL);
    st->addField("repeatT", VrmlField::SFBOOL);
    st->addEventOut("duration_changed", VrmlField::SFTIME);
    st->addEventOut("isActive", VrmlField::SFBOOL);

    return st;
}

VrmlNodeMovieTexture::VrmlNodeMovieTexture(VrmlScene * const scene) :
        VrmlNodeTexture(*defineType(), scene), d_loop(false), d_speed(1.0),
        d_repeatS(true), d_repeatT(true), d_image(0), d_frame(0),
        d_lastFrame(-1), d_lastFrameTime(-1.0), d_texObject(0) {
    if (this->d_scene) {
        this->d_scene->addMovie(*this);
    }
}

VrmlNodeMovieTexture::~VrmlNodeMovieTexture() {
    if (this->d_scene) {
        this->d_scene->removeMovie(*this);
    }
    delete this->d_image;
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


void VrmlNodeMovieTexture::addToScene(VrmlScene * scene,
                                      const std::string & rel) {
    if (this->d_scene != scene && (this->d_scene = scene)) {
        this->d_scene->addMovie(*this);
    }
    VrmlNodeTexture::addToScene(scene, rel);
}


ostream& VrmlNodeMovieTexture::printFields(ostream& os, int indent)
{
  if (d_loop.get()) PRINT_FIELD(loop);
  if (!fpequal(d_speed.get(), 1.0)) PRINT_FIELD(speed);
  if (!fpzero(d_startTime.get())) PRINT_FIELD(startTime);
  if (!fpzero(d_stopTime.get())) PRINT_FIELD(stopTime);
  if (d_url.getLength() > 0) PRINT_FIELD(url);
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
          size_t len = d_url.getElement(i).length();
          
          if (d_url.getElement(i) == imageUrl
                    || (imageLen > len
                        && d_url.getElement(i) == (imageUrl + imageLen - len)))
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
      Doc2 * rel = d_relativeUrl.get().length() > 0 ? &relDoc : d_scene->urlDoc();
      d_image = new Image;
      if (!d_image->tryURLs(d_url, rel))
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
                                   const std::string & eventName,
                                   const VrmlField & fieldValue) {
    static const char * eventInPrefix = "set_";
    std::string basicEventName;
    if (std::equal(eventInPrefix, eventInPrefix + 4, eventName.begin())) {
        basicEventName = eventName.substr(4);
    } else {
        basicEventName = eventName;
    }
    
    // Ignore set_speed when active
    if (basicEventName == "speed") {
        if (! d_isActive.get()) {
            this->setField(basicEventName, fieldValue);
            this->eventOut(timeStamp, "speed_changed", fieldValue);
            this->setModified();
        }
    } else {
        // Let the generic code handle the rest.
        VrmlNode::eventIn(timeStamp, eventName, fieldValue);
    }
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

const VrmlField * VrmlNodeMovieTexture::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "loop")
    return &d_loop;
  else if (fieldId == "speed")
    return &d_speed;
  else if (fieldId == "startTime")
    return &d_startTime;
  else if (fieldId == "stopTime")
    return &d_stopTime;
  else if (fieldId == "url")
    return &d_url;

  // eventOuts
  else if (fieldId == "duration")
    return &d_duration;
  else if (fieldId == "isActive")
    return &d_isActive;
  
  return VrmlNode::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeMovieTexture::setField(const std::string & fieldId,
                    const VrmlField & fieldValue)
{
  if TRY_FIELD(loop, SFBool)
  else if TRY_FIELD(speed, SFFloat)
  else if TRY_FIELD(startTime, SFTime)
  else if TRY_FIELD(stopTime, SFTime)
  else if TRY_FIELD(url, MFString)
  else if TRY_FIELD(repeatS, SFBool)
  else if TRY_FIELD(repeatT, SFBool)
  else
    VrmlNode::setField(fieldId, fieldValue);
}


namespace {
    //  NavigationInfo factory.
    //  Since NavInfo is a bindable child node, the first one created needs
    //  to notify its containing scene.
    const VrmlNodePtr createNavigationInfo(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeNavigationInfo(scene));
    }
}

/**
 * Define the built in NodeType:: "NavigationInfo" fields.
 */
const NodeTypePtr VrmlNodeNavigationInfo::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("NavigationInfo", createNavigationInfo));
    st->addEventIn("set_bind", VrmlField::SFBOOL);
    st->addExposedField("avatarSize", VrmlField::MFFLOAT);
    st->addExposedField("headlight", VrmlField::SFBOOL);
    st->addExposedField("speed", VrmlField::SFFLOAT);
    st->addExposedField("type", VrmlField::MFSTRING);
    st->addExposedField("visibilityLimit", VrmlField::SFFLOAT);
    st->addEventOut("isBound", VrmlField::SFBOOL);

    return st;
}

VrmlNodeNavigationInfo::VrmlNodeNavigationInfo(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_headlight(true), d_speed( 1.0 ),
        d_visibilityLimit(0.0), d_isBound(false) {
  float avatarSize[] = { 0.25, 1.6, 0.75 };
  const std::string type[] = { "WALK", "ANY" };

  d_avatarSize.set(3, avatarSize);
  d_type = VrmlMFString(2, type);
  if (d_scene) d_scene->addNavigationInfo(*this);
}

VrmlNodeNavigationInfo::~VrmlNodeNavigationInfo() {
    if (this->d_scene) {
        this->d_scene->removeNavigationInfo(*this);
    }
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


void VrmlNodeNavigationInfo::addToScene(VrmlScene * scene,
                                        const std::string &) {
    if (this->d_scene != scene && (this->d_scene = scene)) {
        this->d_scene->addNavigationInfo(*this);
    }
}


ostream& VrmlNodeNavigationInfo::printFields(ostream& os, int indent)
{
  if (d_avatarSize.getLength() != 3 ||
      ! fpequal(d_avatarSize.getElement(0), 0.25) ||
      ! fpequal(d_avatarSize.getElement(1), 1.6) ||
      ! fpequal(d_avatarSize.getElement(2), 0.75) )
    PRINT_FIELD(avatarSize);
  if (! d_headlight.get()) PRINT_FIELD(headlight);
  if (! fpequal(d_speed.get(), 1.0)) PRINT_FIELD(speed);
  if (d_type.getLength() != 2
        || d_type.getElement(0) != "WALK"
        || d_type.getElement(1) != "ANY")
    PRINT_FIELD(type);
  if (! fpzero(d_visibilityLimit.get())) PRINT_FIELD(visibilityLimit);

  return os;
}

// Note that this method is not maintaining isBound.
// I can't find a good way to ensure that the isBound
// var of the current node gets updated at the right time...

void VrmlNodeNavigationInfo::eventIn(double timeStamp,
                                     const std::string & eventName,
                                     const VrmlField & fieldValue) {
  if (eventName == "set_bind")
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

      if ( b->get() )        // set_bind TRUE
    {
      if (this != current)
        {
          if (current)
        current->eventOut( timeStamp, "isBound", VrmlSFBool(false));
          d_scene->bindablePush( this );
          eventOut( timeStamp, "isBound", VrmlSFBool(true) );
        }
    }
      else            // set_bind FALSE
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

const VrmlField * VrmlNodeNavigationInfo::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "avatarSize")
    return &d_avatarSize;
  else if (fieldId == "headlight")
    return &d_headlight;
  else if (fieldId == "speed")
    return &d_speed;
  else if (fieldId == "type")
    return &d_type;
  else if (fieldId == "visibilityLimit")
    return &d_visibilityLimit;
  
  // eventOuts
  else if (fieldId == "isBound")
    {
      VrmlSFBool* isBound = (VrmlSFBool*) &(this->d_isBound);
      isBound->set( d_scene->bindableNavigationInfoTop() == this );
      return isBound;
    }

  return VrmlNodeChild::getField(fieldId); // Parent class
}


// Set the value of one of the node fields.

void VrmlNodeNavigationInfo::setField(const std::string & fieldId,
                      const VrmlField & fieldValue)
{
  if TRY_FIELD(avatarSize, MFFloat)
  else if TRY_FIELD(headlight, SFBool)
  else if TRY_FIELD(speed, SFFloat)
  else if TRY_FIELD(type, MFString)
  else if TRY_FIELD(visibilityLimit, SFFloat)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}


namespace {
    const VrmlNodePtr createNormal(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeNormal(scene));
    }
}

/**
 * Define the built in NodeType:: "Normal" fields.
 */
const NodeTypePtr VrmlNodeNormal::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("Normal", createNormal));
    st->addExposedField("vector", VrmlField::MFVEC3F);

    return st;
}

VrmlNodeNormal::VrmlNodeNormal(VrmlScene * const scene):
        VrmlNode(*defineType(), scene) {}

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

const VrmlField * VrmlNodeNormal::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "vector")
    return &d_vector;

  return VrmlNode::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeNormal::setField(const std::string & fieldId,
                  const VrmlField & fieldValue)
{
  if TRY_FIELD(vector, MFVec3f)
  else
    VrmlNode::setField(fieldId, fieldValue);
}


namespace {
    // NormalInt factory.
    const VrmlNodePtr createNormalInt(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeNormalInt(scene));
    }
}

/**
 * Define the built in NodeType:: "NormalInterpolator" fields.
 */
const NodeTypePtr VrmlNodeNormalInt::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }

    st.reset(new NodeType("NormalInterpolator", createNormalInt));
    st->addEventIn("set_fraction", VrmlField::SFFLOAT);
    st->addExposedField("key", VrmlField::MFFLOAT);
    st->addExposedField("keyValue", VrmlField::MFVEC3F);
    st->addEventOut("value_changed", VrmlField::MFVEC3F);

    return st;
}

VrmlNodeNormalInt::VrmlNodeNormalInt(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene) {}

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


void VrmlNodeNormalInt::eventIn(double timeStamp, const std::string & eventName,
                                const VrmlField & fieldValue) {
  if (eventName == "set_fraction")
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
    {
      theSystem->error
        ("Invalid type for %s eventIn %s (expected SFFloat).\n",
             this->type.getId().c_str(), eventName.c_str());
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

const VrmlField * VrmlNodeNormalInt::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "key")
    return &d_key;
  else if (fieldId == "keyValue")
    return &d_keyValue;

  // eventOuts
  else if (fieldId == "value")
    return &d_value;

  return VrmlNodeChild::getField(fieldId);
}


// Set the value of one of the node fields.

void VrmlNodeNormalInt::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFVec3f)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}


namespace {
    // OrientationInt factory.
    const VrmlNodePtr createOrientationInt(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeOrientationInt(scene));
    }
}

/**
 * Define the built in NodeType:: "OrientationInt" fields.
 */
const NodeTypePtr VrmlNodeOrientationInt::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("OrientationInterpolator", createOrientationInt));
    st->addEventIn("set_fraction", VrmlField::SFFLOAT);
    st->addExposedField("key", VrmlField::MFFLOAT);
    st->addExposedField("keyValue", VrmlField::MFROTATION);
    st->addEventOut("value_changed", VrmlField::SFROTATION);

    return st;
}

VrmlNodeOrientationInt::VrmlNodeOrientationInt(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene) {}

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
                                     const std::string & eventName,
                                     const VrmlField & fieldValue) {
  if (eventName == "set_fraction")
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
    {
      theSystem->error(
                "Invalid type for %s eventIn %s (expected SFFloat).\n",
                this->type.getId().c_str(), eventName.c_str());
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
        if (fabs(r2 - r1) > PI)
          {
            if (r2 > r1) r1 += 2.0 * PI;
            else         r2 += 2.0 * PI;
          }
        float angle = r1 + f * (r2 - r1);
        if (angle >= 2.0 * PI) angle -= 2.0 * PI;
        else if (angle < 0.0)    angle += 2.0 * PI;
        
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

const VrmlField * VrmlNodeOrientationInt::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "key")
    return &d_key;
  else if (fieldId == "keyValue")
    return &d_keyValue;

  // eventOuts
  else if (fieldId == "value")
    return &d_value;

  return VrmlNodeChild::getField(fieldId);
}


// Set the value of one of the node fields.

void VrmlNodeOrientationInt::setField(const std::string & fieldId,
                      const VrmlField & fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFRotation)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}

VrmlNodeOrientationInt* VrmlNodeOrientationInt::toOrientationInt() const 
{ return (VrmlNodeOrientationInt*) this; }

const VrmlMFFloat& VrmlNodeOrientationInt::getKey() const  
{   return d_key; }

const VrmlMFRotation& VrmlNodeOrientationInt::getKeyValue() const  
{ return d_keyValue; }


namespace {
    const VrmlNodePtr createPixelTexture(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodePixelTexture(scene));
    }
}

VrmlNodePixelTexture* VrmlNodePixelTexture::toPixelTexture() const
{ return (VrmlNodePixelTexture*) this; }

/**
 * Define the built in NodeType:: "PixelTexture" fields.
 */
const NodeTypePtr VrmlNodePixelTexture::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("PixelTexture", createPixelTexture));
    st->addExposedField("image", VrmlField::SFIMAGE);
    st->addField("repeatS", VrmlField::SFBOOL);
    st->addField("repeatT", VrmlField::SFBOOL);
    
    return st;
}

VrmlNodePixelTexture::VrmlNodePixelTexture(VrmlScene * const scene):
        VrmlNodeTexture(*defineType(), scene), d_repeatS(true), d_repeatT(true),
        d_texObject(0) {}

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

const VrmlField * VrmlNodePixelTexture::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "image")
    return &d_image;
  
  return VrmlNode::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodePixelTexture::setField(const std::string & fieldId,
                    const VrmlField & fieldValue)
{
  if TRY_FIELD(image, SFImage)
  else if TRY_FIELD(repeatS, SFBool)
  else if TRY_FIELD(repeatT, SFBool)
  else
    VrmlNode::setField(fieldId, fieldValue);

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


namespace {
    // PlaneSensor factory. 
    const VrmlNodePtr createPlaneSensor(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodePlaneSensor(scene));
    }
}

/**
 * Define the built in NodeType:: "PlaneSensor" fields.
 */
const NodeTypePtr VrmlNodePlaneSensor::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("PlaneSensor", createPlaneSensor));
    st->addExposedField("autoOffset", VrmlField::SFBOOL);
    st->addExposedField("enabled", VrmlField::SFBOOL);
    st->addExposedField("maxPosition", VrmlField::SFVEC2F);
    st->addExposedField("minPosition", VrmlField::SFVEC2F);
    st->addExposedField("offset", VrmlField::SFVEC3F);
    st->addEventOut("isActive", VrmlField::SFBOOL);
    st->addEventOut("translation_changed", VrmlField::SFVEC3F);
    st->addEventOut("trackPoint_changed", VrmlField::SFVEC3F);

    return st;
}

VrmlNodePlaneSensor::VrmlNodePlaneSensor(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_autoOffset(true),
        d_enabled(true), d_maxPosition(-1.0, -1.0), d_isActive(false),
        d_minPosition(0.0,0.0), d_offset(0.0,0.0,0.0), 
        d_parentTransform(0) {
    this->setModified();
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
  if (! fpequal(d_maxPosition.getX(), -1.0) ||
      ! fpequal(d_maxPosition.getY(), -1.0))
    PRINT_FIELD(maxPosition);
  if (! fpequal(d_minPosition.getX(), -1.0) ||
      ! fpequal(d_minPosition.getY(), -1.0))
    PRINT_FIELD(minPosition);

  if (! fpzero(d_offset.getX()) ||
      ! fpzero(d_offset.getY()) ||
      ! fpzero(d_offset.getZ()) )
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

// Store the ModelView matrix which is calculated at the time of rendering
// in render-context. This matrix will be in use at the time of activation

void VrmlNodePlaneSensor::render(Viewer* v, VrmlRenderContext rc)
{
	setMVMatrix(rc.getMatrix());
}

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
	  VrmlMatrix M = getMVMatrix().affine_inverse();
	  M.VecXMatrix(V,V);
      d_activationPoint.set(V);
      eventOut( timeStamp, "isActive", d_isActive );
    }

  // Become inactive
  else if ( ! isActive && d_isActive.get() )
    {
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
  	  VrmlMatrix M = getMVMatrix().affine_inverse();
	  M.VecXMatrix(V,V);
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

const VrmlField * VrmlNodePlaneSensor::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "autoOffset")
    return &d_autoOffset;
  else if (fieldId == "enabled")
    return &d_enabled;
  else if (fieldId == "enabled")
    return &d_enabled;
  else if (fieldId == "enabled")
    return &d_enabled;
  else if (fieldId == "enabled")
    return &d_enabled;
  
  // eventOuts
  else if (fieldId == "isActive")
    return &d_isActive;
  else if (fieldId == "translation")
    return &d_translation;
  else if (fieldId == "trackPoint")
    return &d_trackPoint;
  
  return VrmlNodeChild::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodePlaneSensor::setField(const std::string & fieldId,
                   const VrmlField & fieldValue)
{
  if TRY_FIELD(autoOffset, SFBool)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(maxPosition, SFVec2f)
  else if TRY_FIELD(minPosition, SFVec2f)
  else if TRY_FIELD(offset, SFVec3f)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}


/**
 * Get the modelview matrix (M). 
 *
 * @param return modelview matrix in VrmlMatrix format. 
*/
const VrmlMatrix & VrmlNodePlaneSensor::getMVMatrix() const
{
return this->M;
}

/**
 * Sets the modelview matrix (M). 
 * @param M_in a modelview matrix in VrmlMatrix format. 
*/
void VrmlNodePlaneSensor::setMVMatrix(const VrmlMatrix & M_in)
{
this->M = M_in;
}


namespace {
    // Return a new VrmlNodePointLight
    const VrmlNodePtr createPointLight(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodePointLight(scene));
    }
}

/**
 * Define the built in NodeType:: "PointLight" fields.
 */
const NodeTypePtr VrmlNodePointLight::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("PointLight", createPointLight));
    st->addExposedField("ambientIntensity", VrmlField::SFFLOAT);
    st->addExposedField("color", VrmlField::SFCOLOR);
    st->addExposedField("intensity", VrmlField::SFFLOAT);
    st->addExposedField("on", VrmlField::SFBOOL);
    st->addExposedField("attenuation", VrmlField::SFVEC3F);
    st->addExposedField("location", VrmlField::SFVEC3F);
    st->addExposedField("radius", VrmlField::SFFLOAT);

    return st;
}

VrmlNodePointLight::VrmlNodePointLight(VrmlScene * const scene):
        VrmlNodeLight(*defineType(), scene), d_attenuation(1.0, 0.0, 0.0),
        d_location(0.0, 0.0, 0.0), d_radius(100) {
    if (this->d_scene) {
        this->d_scene->addScopedLight(*this);
    }
}

VrmlNodePointLight::~VrmlNodePointLight() {
    if (this->d_scene) {
        this->d_scene->removeScopedLight(*this);
    }
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

void VrmlNodePointLight::addToScene(VrmlScene * scene, const std::string &) {
    if (this->d_scene != scene && (this->d_scene = scene)) {
        this->d_scene->addScopedLight(*this);
    }
}


ostream& VrmlNodePointLight::printFields(ostream& os, int indent)
{
  VrmlNodeLight::printFields(os, indent);
  if (! fpequal(d_attenuation.getX(), 1.0) ||
      ! fpzero(d_attenuation.getY()) ||
      ! fpzero(d_attenuation.getZ()) )
    PRINT_FIELD(attenuation);
  if (! fpzero(d_location.getX()) ||
      ! fpzero(d_location.getY()) ||
      ! fpzero(d_location.getZ()) )
    PRINT_FIELD(location);
  if (! fpequal(d_radius.get(), 100.0))
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

const VrmlField * VrmlNodePointLight::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "attenuation")
    return &d_attenuation;
  else if (fieldId == "location")
    return &d_location;
  else if (fieldId == "radius")
    return &d_radius;

  return VrmlNodeLight::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodePointLight::setField(const std::string & fieldId,
                  const VrmlField & fieldValue)
{
  if TRY_FIELD(attenuation, SFVec3f)
  else if TRY_FIELD(location, SFVec3f)
  else if TRY_FIELD(radius, SFFloat)
  else
    VrmlNodeLight::setField(fieldId, fieldValue);
}

const VrmlSFVec3f& VrmlNodePointLight::getAttenuation() const   // LarryD Mar 04/99
{  return d_attenuation; }

const VrmlSFVec3f& VrmlNodePointLight::getLocation() const   // LarryD Mar 04/99
{  return d_location; }


namespace {
    const VrmlNodePtr createPointSet(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodePointSet(scene));
    }
}

/**
 * Define the built in NodeType:: "PointSet" fields.
 */
const NodeTypePtr VrmlNodePointSet::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("PointSet", createPointSet));
    st->addExposedField("color", VrmlField::SFNODE);
    st->addExposedField("coord", VrmlField::SFNODE);

    return st;
}

VrmlNodePointSet::VrmlNodePointSet(VrmlScene * const scene):
        VrmlNodeGeometry(*defineType(), scene) {
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

const VrmlMFNode VrmlNodePointSet::getChildren() const {
    VrmlNodePtr children[] = { this->d_color.get(), this->d_coord.get() };
    return VrmlMFNode(2, children);
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

void VrmlNodePointSet::addToScene(VrmlScene * scene, const std::string & rel) {
    this->d_scene = scene;
    if (this->d_color.get()) {
        this->d_color.get()->addToScene(scene, rel);
    }
    if (this->d_coord.get()) {
        this->d_coord.get()->addToScene(scene, rel);
    }
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

const VrmlField * VrmlNodePointSet::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "color")
    return &d_color;
  else if (fieldId == "coord")
    return &d_coord;

  return VrmlNodeGeometry::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodePointSet::setField(const std::string & fieldId,
                const VrmlField & fieldValue)
{
  if TRY_SFNODE_FIELD(color, Color)
  else if TRY_SFNODE_FIELD(coord, Coordinate)
  else
    VrmlNodeGeometry::setField(fieldId, fieldValue);
  this->setBVolumeDirty(true);
}

void VrmlNodePointSet::recalcBSphere()
{
  d_bsphere.reset();
  VrmlMFVec3f &coord = d_coord.get()->toCoordinate()->coordinate();
  int nvert = coord.getLength();
  for(int i=0; i<nvert; i++) {
    const float * vi = coord.getElement(i); // vi[3]
    d_bsphere.extend(vi);
  }
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


namespace {
    // PositionInt factory.
    const VrmlNodePtr createPositionInt(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodePositionInt(scene));
    }
}

/**
 * Define the built in NodeType:: "PositionInt" fields.
 */
const NodeTypePtr VrmlNodePositionInt::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }

    st.reset(new NodeType("PositionInterpolator", createPositionInt));
    st->addEventIn("set_fraction", VrmlField::SFFLOAT);
    st->addExposedField("key", VrmlField::MFFLOAT);
    st->addExposedField("keyValue", VrmlField::MFVEC3F);
    st->addEventOut("value_changed", VrmlField::SFVEC3F);

    return st;
}

VrmlNodePositionInt::VrmlNodePositionInt(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene) {}

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
                                  const std::string & eventName,
                                  const VrmlField & fieldValue) {
  if (eventName == "set_fraction")
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
    {
      theSystem->error(
                  "Invalid type for %s eventIn %s (expected SFFloat).\n",
                  this->type.getId().c_str(), eventName.c_str());
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

const VrmlField * VrmlNodePositionInt::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "key")
    return &d_key;
  else if (fieldId == "keyValue")
    return &d_keyValue;

  // eventOuts
  else if (fieldId == "value")
    return &d_value;

  return VrmlNodeChild::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodePositionInt::setField(const std::string & fieldId,
                   const VrmlField & fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFVec3f)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}

VrmlNodePositionInt* VrmlNodePositionInt::toPositionInt() const 
{ return (VrmlNodePositionInt*) this; }

const VrmlMFFloat& VrmlNodePositionInt::getKey() const  
{   return d_key; }

const VrmlMFVec3f& VrmlNodePositionInt::getKeyValue() const  
{ return d_keyValue; }


namespace {
    // ProximitySensor factory. 
    const VrmlNodePtr createProximitySensor(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeProximitySensor(scene));
    }
}

/**
 * Define the built in NodeType:: "ProximitySensor" fields.
 */
const NodeTypePtr VrmlNodeProximitySensor::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }

    st.reset(new NodeType("ProximitySensor", createProximitySensor));
    st->addExposedField("center", VrmlField::SFVEC3F);
    st->addExposedField("enabled", VrmlField::SFBOOL);
    st->addExposedField("size", VrmlField::SFVEC3F);
    st->addEventOut("enterTime", VrmlField::SFTIME);
    st->addEventOut("exitTime", VrmlField::SFTIME);
    st->addEventOut("isActive", VrmlField::SFBOOL);
    st->addEventOut("orientation_changed", VrmlField::SFROTATION);
    st->addEventOut("position_changed", VrmlField::SFVEC3F);

    return st;
}

VrmlNodeProximitySensor::VrmlNodeProximitySensor(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_center(0.0, 0.0, 0.0),
        d_enabled(true), d_size(0.0, 0.0, 0.0), d_isActive(false),
        d_position(0.0, 0.0, 0.0), d_enterTime(0.0), d_exitTime(0.0) {
    this->setModified();
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
  if (! fpzero(d_center.getX()) ||
      ! fpzero(d_center.getY()) ||
      ! fpzero(d_center.getZ()) )
    PRINT_FIELD(center);
  if (! d_enabled.get())
    PRINT_FIELD(enabled);
  if (! fpzero(d_size.getX()) ||
      ! fpzero(d_size.getY()) ||
      ! fpzero(d_size.getZ()) )
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
      theSystem->debug("PS.%s::render ENTER %g %g %g\n",
                           this->getId().c_str(), x, y, z);

      d_isActive.set(true);
      eventOut(timeNow.get(), "isActive", d_isActive);

      d_enterTime = timeNow;
      eventOut(timeNow.get(), "enterTime", d_enterTime);
    }

      // Check if viewer has left the box
      else if (wasIn && ! inside)
    {
      theSystem->debug("PS.%s::render EXIT %g %g %g\n",
                           this->getId().c_str(), x, y, z);

      d_isActive.set(false);
      eventOut(timeNow.get(), "isActive", d_isActive );

      d_exitTime = timeNow;
      eventOut(timeNow.get(), "exitTime", d_exitTime);
    }

      // Check for movement within the box
      if (wasIn || inside)
    {
      if ( ! fpequal(d_position.getX(), x) ||
           ! fpequal(d_position.getY(), y) ||
           ! fpequal(d_position.getZ(), z) )
        {
          const float positionVec[3] = { x, y, z };
              d_position.set(positionVec);
          eventOut(timeNow.get(), "position_changed", d_position);
        }

      float xyzr[4];
      viewer->getOrientation( xyzr );
      if ( ! fpequal(d_orientation.getX(), xyzr[0]) ||
           ! fpequal(d_orientation.getY(), xyzr[1]) ||
           ! fpequal(d_orientation.getZ(), xyzr[2]) ||
           ! fpequal(d_orientation.getAngle(), xyzr[3]) )
        {
          d_orientation.set(xyzr);
          eventOut(timeNow.get(), "orientation_changed", d_orientation);
        }
    }
    }

  else
    clearModified();
}


const VrmlField * VrmlNodeProximitySensor::getField(const std::string & fieldId) const
{
  if (fieldId == "center")
    return &d_center;
  else if (fieldId == "enabled")
    return &d_enabled;
  else if (fieldId == "size")
    return &d_size;

  // eventOuts
  else if (fieldId == "isActive")
    return &d_isActive;
  else if (fieldId == "position")
    return &d_position;
  else if (fieldId == "orientation")
    return &d_orientation;
  else if (fieldId == "enterTime")
    return &d_enterTime;
  else if (fieldId == "exitTime")
    return &d_exitTime;
  
  return VrmlNodeChild::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeProximitySensor::setField(const std::string & fieldId,
                       const VrmlField & fieldValue)
{
  if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(size, SFVec3f)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}


namespace {
    // ScalarInt factory.
    const VrmlNodePtr createScalarInt(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeScalarInt(scene));
    }
}

/**
 * Define the built in NodeType:: "ScalarInterpolator" fields.
 */
const NodeTypePtr VrmlNodeScalarInt::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("ScalarInterpolator", createScalarInt));
    st->addEventIn("set_fraction", VrmlField::SFFLOAT);
    st->addExposedField("key", VrmlField::MFFLOAT);
    st->addExposedField("keyValue", VrmlField::MFFLOAT);
    st->addEventOut("value_changed", VrmlField::SFFLOAT);

    return st;
}

VrmlNodeScalarInt::VrmlNodeScalarInt(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene) {}

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

void VrmlNodeScalarInt::eventIn(double timeStamp, const std::string & eventName,
                                const VrmlField & fieldValue) {
  if (eventName == "set_fraction")
    {
      if (!dynamic_cast<const VrmlSFFloat *>(&fieldValue))
    {
      theSystem->error(
                "Invalid type for %s eventIn %s (expected SFFloat).\n",
                this->type.getId().c_str(), eventName.c_str());
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

const VrmlField * VrmlNodeScalarInt::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "key")
    return &d_key;
  else if (fieldId == "keyValue")
    return &d_keyValue;

  // eventOuts
  else if (fieldId == "value")
    return &d_value;

  return VrmlNodeChild::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeScalarInt::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
{
  if TRY_FIELD(key, MFFloat)
  else if TRY_FIELD(keyValue, MFFloat)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}

VrmlNodeScalarInt* VrmlNodeScalarInt::toScalarInt() const
{ return (VrmlNodeScalarInt*) this; }

const VrmlMFFloat& VrmlNodeScalarInt::getKey() const 
{   return d_key; }

const VrmlMFFloat& VrmlNodeScalarInt::getKeyValue() const 
{ return d_keyValue; }


namespace {
    const VrmlNodePtr createShape(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeShape(scene));
    }
}

/**
 * Define the built in NodeType:: "Shape" fields.
 */
const NodeTypePtr VrmlNodeShape::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("Shape", createShape));
    st->addExposedField("appearance", VrmlField::SFNODE);
    st->addExposedField("geometry", VrmlField::SFNODE);

    return st;
}

VrmlNodeShape::VrmlNodeShape(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_viewerObject(0) {}

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

const VrmlMFNode VrmlNodeShape::getChildren() const {
    VrmlNodePtr children[] = { this->d_appearance.get(),
                               this->d_geometry.get() };
    return VrmlMFNode(2, children);
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

void VrmlNodeShape::addToScene(VrmlScene * scene, const std::string & relUrl) {
    this->d_scene = scene;
    if (this->d_appearance.get()) {
        this->d_appearance.get()->addToScene(scene, relUrl);
    }
    if (this->d_geometry.get()) {
        this->d_geometry.get()->addToScene(scene, relUrl);
    }
}

ostream& VrmlNodeShape::printFields(ostream& os, int indent)
{
  if (d_appearance.get()) PRINT_FIELD(appearance);
  if (d_geometry.get()) PRINT_FIELD(geometry);

  return os;
}

VrmlNodeShape*    VrmlNodeShape::toShape() const
{ return (VrmlNodeShape*) this; }


void VrmlNodeShape::render(Viewer *viewer, VrmlRenderContext rc)
{
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
      d_viewerObject = viewer->beginObject(this->getId().c_str());

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

const VrmlField * VrmlNodeShape::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "appearance")
    return &d_appearance;
  else if (fieldId == "geometry")
    return &d_geometry;
  
  return VrmlNodeChild::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeShape::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
{
  if TRY_SFNODE_FIELD(appearance, Appearance)
  else if TRY_SFNODE_FIELD(geometry, Geometry)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
  this->setBVolumeDirty(true); // case out: just if geom set!
}


// just pass off to the geometry's getbvolume() method
//
const VrmlBVolume* VrmlNodeShape::getBVolume() const
{
  //cout << "VrmlNodeShape::getBVolume() {" << endl;
  const VrmlBVolume* r = (VrmlBVolume*)0;
  const VrmlNodePtr & geom = d_geometry.get();
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


namespace {
    // Sound factory. Add each Sound to the scene for fast access.
    const VrmlNodePtr createSound(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeSound(scene));
    }
}

/**
 * Define the built in NodeType:: "Sound" fields.
 */
const NodeTypePtr VrmlNodeSound::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("Sound", createSound));
    st->addExposedField("direction", VrmlField::SFVEC3F);
    st->addExposedField("intensity", VrmlField::SFFLOAT);
    st->addExposedField("location", VrmlField::SFVEC3F);
    st->addExposedField("maxBack", VrmlField::SFFLOAT);
    st->addExposedField("maxFront", VrmlField::SFFLOAT);
    st->addExposedField("minBack", VrmlField::SFFLOAT);
    st->addExposedField("minFront", VrmlField::SFFLOAT);
    st->addExposedField("priority", VrmlField::SFFLOAT);
    st->addExposedField("source", VrmlField::SFNODE);
    st->addField("spatialize", VrmlField::SFBOOL);

    return st;
}

VrmlNodeSound::VrmlNodeSound(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_direction(0, 0, 1),
        d_intensity(1), d_maxBack(10), d_maxFront(10), d_minBack(1),
        d_minFront(1), d_spatialize(true) {}


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

const VrmlMFNode VrmlNodeSound::getChildren() const {
    return VrmlMFNode(1, &this->d_source.get());
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

void VrmlNodeSound::addToScene(VrmlScene * scene, const std::string & relUrl) {
    this->d_scene = scene;
    if (this->d_source.get()) {
        this->d_source.get()->addToScene(scene, relUrl);
    }
}

ostream& VrmlNodeSound::printFields(ostream& os, int indent)
{
  if (! fpzero(d_direction.getX()) ||
      ! fpzero(d_direction.getY()) ||
      ! fpequal(d_direction.getZ(), 1.0) )
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

const VrmlField * VrmlNodeSound::getField(const std::string & fieldId) const {
  // exposedFields
  if (fieldId == "direction")
    return &d_direction;
  else if (fieldId == "intensity")
    return &d_intensity;
  else if (fieldId == "location")
    return &d_location;
  else if (fieldId == "maxBack")
    return &d_maxBack;
  else if (fieldId == "maxFront")
    return &d_maxFront;
  else if (fieldId == "minBack")
    return &d_minBack;
  else if (fieldId == "minFront")
    return &d_minFront;
  else if (fieldId == "priority")
    return &d_priority;
  else if (fieldId == "source")
    return &d_source;
  else if (fieldId == "spatialize")
    return &d_spatialize;
  
  return VrmlNodeChild::getField(fieldId); // Parent class
}



// Set the value of one of the node fields/events.
// setField is public so the parser can access it.

void VrmlNodeSound::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
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
    VrmlNodeChild::setField(fieldId, fieldValue);
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


namespace {
    const VrmlNodePtr createSphere(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeSphere(scene));
    }
}    

/**
 * Define the built in NodeType:: "Sphere" fields.
 */
const NodeTypePtr VrmlNodeSphere::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }

    st.reset(new NodeType("Sphere", createSphere));
    st->addField("radius", VrmlField::SFFLOAT);

    return st;
}

VrmlNodeSphere::VrmlNodeSphere(VrmlScene * const scene):
        VrmlNodeGeometry(*defineType(), scene), d_radius(1.0) {
    this->setBVolumeDirty(true); // lazy calc of bvolumes
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

void VrmlNodeSphere::setField(const std::string & fieldId,
                  const VrmlField & fieldValue)
{
  if TRY_FIELD(radius, SFFloat)
  else
    VrmlNodeGeometry::setField(fieldId, fieldValue);
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


namespace {
    // SphereSensor factory. 
    const VrmlNodePtr createSphereSensor(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeSphereSensor(scene));
    }
}

/**
 * Define the built in NodeType:: "SphereSensor" fields.
 */
const NodeTypePtr VrmlNodeSphereSensor::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("SphereSensor", createSphereSensor));
    st->addExposedField("autoOffset", VrmlField::SFBOOL);
    st->addExposedField("enabled", VrmlField::SFBOOL);
    st->addExposedField("offset", VrmlField::SFROTATION);
    st->addEventOut("isActive", VrmlField::SFBOOL);
    st->addEventOut("rotation_changed", VrmlField::SFROTATION);
    st->addEventOut("trackPoint_changed", VrmlField::SFVEC3F);

    return st;
}

VrmlNodeSphereSensor::VrmlNodeSphereSensor(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_autoOffset(true),
        d_enabled(true), d_offset(0,1,0,0), d_isActive(false) {
    this->setModified();
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

VrmlNodeSphereSensor* VrmlNodeSphereSensor::toSphereSensor() const    // mgiger 6/16/00
{
    return (VrmlNodeSphereSensor*) this;
}

ostream& VrmlNodeSphereSensor::printFields(ostream& os, int indent)
{
  if (! d_autoOffset.get()) PRINT_FIELD(autoOffset);
  if (! d_enabled.get())    PRINT_FIELD(enabled);

  if (! fpzero(d_offset.getX()) ||
      ! fpequal(d_offset.getY(), 1.0) ||
      ! fpzero(d_offset.getZ()) ||
      ! fpzero(d_offset.getAngle()) )
    PRINT_FIELD(offset);

  return os;
}

const VrmlField * VrmlNodeSphereSensor::getField(const std::string & fieldId) const
{
  if (fieldId == "autoOffset")
    return &d_autoOffset;
  else if (fieldId == "enabled")
    return &d_enabled;
  else if (fieldId == "offset")
    return &d_offset;

  // eventOuts
  else if (fieldId == "isActive")
    return &d_isActive;
  else if (fieldId == "rotation")
    return &d_rotation;
  else if (fieldId == "trackPoint")
    return &d_trackPoint;
  
  return VrmlNodeChild::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeSphereSensor::setField(const std::string & fieldId,
                   const VrmlField & fieldValue)
{
  if TRY_FIELD(autoOffset, SFBool)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(offset, SFRotation)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}

// Store the ModelView matrix which is calculated at the time of rendering
// in render-context. This matrix will be in use at the time of activation

void VrmlNodeSphereSensor::render(Viewer* v, VrmlRenderContext rc)
{
	setMVMatrix(rc.getMatrix());
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
        VrmlMatrix M = getMVMatrix().affine_inverse();
		M.VecXMatrix( V , V );
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
        VrmlMatrix M = getMVMatrix().affine_inverse();
		M.VecXMatrix( V , V );
        d_trackPoint.set(V);
        eventOut( timeStamp, "trackPoint_changed", d_trackPoint );
        
        float V2[3] = { p[0], p[1], p[2] };
                float tempv[3];
        Vdiff(tempv, V2, d_centerPoint.get());
                VrmlSFVec3f dir1(tempv);
        double dist = dir1.length();                // get the length of the pre-normalized vector
        dir1 = dir1.normalize();
        Vdiff(tempv, d_activationPoint.get(), d_centerPoint.get());
                VrmlSFVec3f dir2(tempv);
        dir2 = dir2.normalize();
        
        Vcross(tempv, dir1.get(), dir2.get());
                VrmlSFVec3f cx(tempv);
        cx = cx.normalize();     

        VrmlSFRotation newRot(cx, dist * acos(dir1.dot(dir2)));
        if ( d_autoOffset.get() )
            newRot = newRot.multiply(d_offset);
        d_rotation = newRot;
        
        eventOut( timeStamp, "rotation_changed", d_rotation );
    }
}

/**
 * Get the modelview matrix (M). 
 *
 * @param return modelview matrix in VrmlMatrix format. 
*/
const VrmlMatrix & VrmlNodeSphereSensor::getMVMatrix() const
{
return this->M;
}

/**
 * Sets the modelview matrix (M). 
 * @param M_in a modelview matrix in VrmlMatrix format. 
*/
void VrmlNodeSphereSensor::setMVMatrix(const VrmlMatrix & M_in)
{
this->M = M_in;
}

namespace {
    // Return a new VrmlNodeSpotLight
    const VrmlNodePtr createSpotLight(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeSpotLight(scene));
    }
}
    
/**
 * Define the built in NodeType:: "SpotLight" fields.
 */
const NodeTypePtr VrmlNodeSpotLight::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("SpotLight", createSpotLight));
    st->addExposedField("ambientIntensity", VrmlField::SFFLOAT);
    st->addExposedField("color", VrmlField::SFCOLOR);
    st->addExposedField("intensity", VrmlField::SFFLOAT);
    st->addExposedField("on", VrmlField::SFBOOL);
    st->addExposedField("attenuation", VrmlField::SFVEC3F);
    st->addExposedField("beamWidth", VrmlField::SFFLOAT);
    st->addExposedField("cutOffAngle", VrmlField::SFFLOAT);
    st->addExposedField("direction", VrmlField::SFVEC3F);
    st->addExposedField("location", VrmlField::SFVEC3F);
    st->addExposedField("radius", VrmlField::SFFLOAT);

    return st;
}

VrmlNodeSpotLight::VrmlNodeSpotLight(VrmlScene * const scene):
        VrmlNodeLight(*defineType(), scene), d_attenuation(1.0, 0.0, 0.0),
        d_beamWidth(1.570796), d_cutOffAngle(0.785398),
        d_direction(0.0, 0.0, -1.0), d_location(0.0, 0.0, 0.0), d_radius(100) {
    if (this->d_scene) {
        this->d_scene->addScopedLight(*this);
    }
}

VrmlNodeSpotLight::~VrmlNodeSpotLight() {
    if (this->d_scene) {
        this->d_scene->removeScopedLight(*this);
    }
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


void VrmlNodeSpotLight::addToScene(VrmlScene * scene, const std::string &) {
    if (this->d_scene != scene && (this->d_scene = scene)) {
        this->d_scene->addScopedLight(*this);
    }
}


ostream& VrmlNodeSpotLight::printFields(ostream& os, int indent)
{
  VrmlNodeLight::printFields(os, indent);
  if (! fpequal(d_attenuation.getX(), 1.0) ||
      ! fpzero(d_attenuation.getY()) ||
      ! fpzero(d_attenuation.getZ()) )
    PRINT_FIELD(attenuation);
  if (! fpequal(d_beamWidth.get(), 1.570796))
    PRINT_FIELD(beamWidth);

  if (! fpequal(d_cutOffAngle.get(), 1.570796))
    PRINT_FIELD(cutOffAngle);
  if (! fpzero(d_direction.getX()) ||
      ! fpzero(d_direction.getY()) ||
      ! fpequal(d_direction.getZ(), -1.0) )
    PRINT_FIELD(direction);

  if (! fpzero(d_location.getX()) ||
      ! fpzero(d_location.getY()) ||
      ! fpzero(d_location.getZ()) )
    PRINT_FIELD(location);

  if (! fpequal(d_radius.get(), 100.0))
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

const VrmlField * VrmlNodeSpotLight::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "attenuation")
    return &d_attenuation;
  else if (fieldId == "beamWidth")
    return &d_beamWidth;
  else if (fieldId == "cutOffAngle")
    return &d_cutOffAngle;
  else if (fieldId == "direction")
    return &d_direction;
  else if (fieldId == "location")
    return &d_location;
  else if (fieldId == "radius")
    return &d_radius;

  return VrmlNodeLight::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeSpotLight::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
{
  if TRY_FIELD(attenuation, SFVec3f)
  else if TRY_FIELD(beamWidth, SFFloat)
  else if TRY_FIELD(cutOffAngle, SFFloat)
  else if TRY_FIELD(direction, SFVec3f)
  else if TRY_FIELD(location, SFVec3f)
  else if TRY_FIELD(radius, SFFloat)
  else 
    VrmlNodeLight::setField(fieldId, fieldValue);
}

const VrmlSFVec3f& VrmlNodeSpotLight::getAttenuation() const   // LarryD Mar 04/99
{  return d_attenuation; }

const VrmlSFVec3f& VrmlNodeSpotLight::getDirection() const   // LarryD Mar 04/99
{  return d_direction; }

const VrmlSFVec3f& VrmlNodeSpotLight::getLocation() const   // LarryD Mar 04/99
{  return d_location; }


namespace {
    // Return a new VrmlNodeSwitch
    const VrmlNodePtr createSwitch(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeSwitch(scene));
    }
}

/**
 * Define the built in NodeType:: "Switch" fields.
 */
const NodeTypePtr VrmlNodeSwitch::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("Switch", createSwitch));
    st->addExposedField("choice", VrmlField::MFNODE);
    st->addExposedField("whichChoice", VrmlField::SFINT32);

    return st;
}

VrmlNodeSwitch::VrmlNodeSwitch(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_whichChoice(-1) {
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

const VrmlMFNode VrmlNodeSwitch::getChildren() const {
    return this->d_choice;
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

void VrmlNodeSwitch::addToScene(VrmlScene * scene, const std::string & rel) {
    this->d_scene = scene;
    for (size_t i = 0; i < d_choice.getLength(); ++i) {
        this->d_choice.getElement(i)->addToScene(scene, rel);
    }
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
const VrmlField * VrmlNodeSwitch::getField(const std::string & fieldId) const
{
  if (fieldId == "choice")
    return &d_choice;
  else if (fieldId == "whichChoice")
    return &d_whichChoice;

  return VrmlNodeChild::getField(fieldId);
}

// Set the value of one of the node fields.
void VrmlNodeSwitch::setField(const std::string & fieldId,
                  const VrmlField & fieldValue)
{
  if TRY_FIELD(choice, MFNode)
  else if TRY_FIELD(whichChoice, SFInt32)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
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


namespace {
    // Make a VrmlNodeText
    const VrmlNodePtr createText(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeText(scene));
    }
}

/**
 * Define the built in NodeType:: "Text" fields.
 */
const NodeTypePtr VrmlNodeText::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Define type only once.
    }
    
    st.reset(new NodeType("Text", createText));
    st->addExposedField("string", VrmlField::MFSTRING);
    st->addExposedField("fontStyle", VrmlField::SFNODE);
    st->addExposedField("length", VrmlField::MFFLOAT);
    st->addExposedField("maxExtent", VrmlField::SFFLOAT);

    return st;
}

VrmlNodeText::VrmlNodeText(VrmlScene * const scene):
        VrmlNodeGeometry(*defineType(), scene) {}

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

const VrmlMFNode VrmlNodeText::getChildren() const {
    return VrmlMFNode(1, &this->d_fontStyle.get());
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

void VrmlNodeText::addToScene(VrmlScene * scene, const std::string & relUrl) {
    this->d_scene = scene;
    if (this->d_fontStyle.get()) {
        this->d_fontStyle.get()->addToScene(scene, relUrl);
    }
}

ostream& VrmlNodeText::printFields(ostream& os, int indent)
{
  if (d_string.getLength() > 0) PRINT_FIELD(string);
  if (d_fontStyle.get()) PRINT_FIELD(fontStyle);
  if (d_length.getLength() > 0) PRINT_FIELD(length);
  if (! fpzero(d_maxExtent.get()) ) PRINT_FIELD(maxExtent);
  return os;
}


Viewer::Object VrmlNodeText::insertGeometry(Viewer *viewer, VrmlRenderContext rc)
{
    char * * strs = new char *[this->d_string.getLength()];
    size_t i;
    for (i = 0; i < this->d_string.getLength(); ++i) {
        const std::string & currentString = this->d_string.getElement(i);
        strs[i] = new char[currentString.length() + 1];
        std::copy(currentString.begin(), currentString.end(), strs[i]);
        strs[i][currentString.length()] = '\0';
    }
    
    Viewer::Object retval(0);
    
    int justify[2] = { 1, 1 };
    float size = 1.0;
    VrmlNodeFontStyle *f = 0;
    if (d_fontStyle.get()) {
        f = d_fontStyle.get()->toFontStyle();
    }

    if (f) {
        const VrmlMFString & j = f->justify();

        for (size_t i=0; i < j.getLength(); ++i) {
            if (j.getElement(i) == "END") {
                justify[i] = -1;
            } else if (j.getElement(i) == "MIDDLE") {
                justify[i] = 0;
            }
        }
        size = f->size();
    }
    
    retval = viewer->insertText(justify, size, d_string.getLength(), strs);
    
    for (i = 0; i < this->d_string.getLength(); i++) {
        delete [] strs[i];
    }
    delete [] strs;
    
    return retval;
}

// Get the value of a field or eventOut.

const VrmlField * VrmlNodeText::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "string")
    return &d_string;
  else if (fieldId == "fontStyle")
    return &d_fontStyle;
  else if (fieldId == "length")
    return &d_maxExtent;
  
  return VrmlNodeGeometry::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeText::setField(const std::string & fieldId,
                const VrmlField & fieldValue)
{
  if TRY_FIELD(string, MFString)
  else if TRY_SFNODE_FIELD(fontStyle, FontStyle)
  else if TRY_FIELD(length, MFFloat)
  else if TRY_FIELD(maxExtent, SFFloat)
  else
    VrmlNodeGeometry::setField(fieldId, fieldValue);
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


namespace {
    const VrmlNodePtr createTextureCoordinate(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeTextureCoordinate(scene));
    }
}

/**
 * Define the built in NodeType:: "TextureCoordinate" fields.
 */
const NodeTypePtr VrmlNodeTextureCoordinate::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }

    st.reset(new NodeType("TextureCoordinate", createTextureCoordinate));
    st->addExposedField("point", VrmlField::MFVEC2F);

    return st;
}

VrmlNodeTextureCoordinate::VrmlNodeTextureCoordinate(VrmlScene * const scene):
        VrmlNode(*defineType(), scene) {}

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

const VrmlField * VrmlNodeTextureCoordinate::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "point")
    return &d_point;
  return VrmlNode::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTextureCoordinate::setField(const std::string & fieldId,
                     const VrmlField & fieldValue)
{
  if TRY_FIELD(point, MFVec2f)
  else
    VrmlNode::setField(fieldId, fieldValue);
}


namespace {
    const VrmlNodePtr createTextureTransform(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeTextureTransform(scene)); 
    }
}

/**
 * Define the built in NodeType:: "TextureTransform" fields.
 */
const NodeTypePtr VrmlNodeTextureTransform::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("TextureTransform", createTextureTransform));
    st->addExposedField("center", VrmlField::SFVEC2F);
    st->addExposedField("rotation", VrmlField::SFFLOAT);
    st->addExposedField("scale", VrmlField::SFVEC2F);
    st->addExposedField("translation", VrmlField::SFVEC2F);

    return st;
}

VrmlNodeTextureTransform::VrmlNodeTextureTransform(VrmlScene * const scene):
        VrmlNode(*defineType(), scene), d_center(0.0, 0.0), d_rotation(0.0),
        d_scale(1.0, 1.0), d_translation(0.0, 0.0) {}

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
  if (! fpzero(d_center.getX()) ||
      ! fpzero(d_center.getY()))
    PRINT_FIELD(center);

  if (! fpzero(d_rotation.get()))
    PRINT_FIELD(rotation);

  if (! fpequal(d_scale.getX(), 1.0) ||
      ! fpequal(d_scale.getY(), 1.0))
    PRINT_FIELD(scale);
  if (! fpzero(d_translation.getX()) ||
      ! fpzero(d_translation.getY()))
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

const VrmlField * VrmlNodeTextureTransform::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "center")
    return &d_center;
  else if (fieldId == "rotation")
    return &d_rotation;
  else if (fieldId == "scale")
    return &d_scale;
  else if (fieldId == "translation")
    return &d_translation;
  
  return VrmlNode::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTextureTransform::setField(const std::string & fieldId,
                    const VrmlField & fieldValue)
{
  if TRY_FIELD(center, SFVec2f)
  else if TRY_FIELD(rotation, SFFloat)
  else if TRY_FIELD(scale, SFVec2f)
  else if TRY_FIELD(translation, SFVec2f)
  else
    VrmlNode::setField(fieldId, fieldValue);
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


namespace {
    // TimeSensor factory. Add each TimeSensor to the scene for fast access.
    const VrmlNodePtr createTimeSensor(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeTimeSensor(scene));
    }
}

/**
 * Define the built in NodeType:: "TimeSensor" fields.
 */
const NodeTypePtr VrmlNodeTimeSensor::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("TimeSensor", createTimeSensor));
    st->addExposedField("cycleInterval", VrmlField::SFTIME);
    st->addExposedField("enabled", VrmlField::SFBOOL);
    st->addExposedField("loop", VrmlField::SFBOOL);
    st->addExposedField("startTime", VrmlField::SFTIME);
    st->addExposedField("stopTime", VrmlField::SFTIME);
    st->addEventOut("cycleTime", VrmlField::SFTIME);
    st->addEventOut("fraction_changed", VrmlField::SFFLOAT);
    st->addEventOut("isActive", VrmlField::SFBOOL);
    st->addEventOut("time", VrmlField::SFTIME);

    return st;
}

VrmlNodeTimeSensor::VrmlNodeTimeSensor(VrmlScene * const scene) :
        VrmlNodeChild(*defineType(), scene), d_cycleInterval(1.0),
        d_enabled(true), d_loop(false), d_startTime(0.0), d_stopTime(0.0),
        d_isActive(false), d_lastTime(-1.0) {
    if (this->d_scene) {
        this->d_scene->addTimeSensor(*this);
    }
}

VrmlNodeTimeSensor::~VrmlNodeTimeSensor() {
    if (this->d_scene) {
        this->d_scene->removeTimeSensor(*this);
    }
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

void VrmlNodeTimeSensor::addToScene(VrmlScene * scene, const std::string &) {
    if (this->d_scene != scene && (this->d_scene = scene)) {
        this->d_scene->addTimeSensor(*this);
    }
}


ostream& VrmlNodeTimeSensor::printFields(ostream& os, int indent)
{
  if (! fpequal(d_cycleInterval.get(), 1.0))
    PRINT_FIELD(cycleInterval);
  if (! d_enabled.get()) PRINT_FIELD(enabled);
  if (d_loop.get()) PRINT_FIELD(loop);
  if (! fpzero(d_startTime.get())) PRINT_FIELD(startTime);
  if (! fpzero(d_stopTime.get())) PRINT_FIELD(stopTime);

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
      theSystem->debug("TimeSensor.%s isActive TRUE\n",
                           this->getId().c_str());

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
      VrmlSFFloat fraction_changed( fpzero(f) ? 1.0 : (f / cycleInt) );
      eventOut( timeNow.get(), "fraction_changed", fraction_changed );

      // Current time message
      eventOut( timeNow.get(), "time", timeNow );

      // End of cycle message (this may miss cycles...)
      if ( fpequal(fraction_changed.get(), 1.0) )
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
                                 const std::string & eventName,
                                 const VrmlField & fieldValue) {
    static const char * eventInPrefix = "set_";
    std::string basicEventName;
    if (std::equal(eventInPrefix, eventInPrefix + 4, eventName.begin())) {
        basicEventName = eventName.substr(4);
    } else {
        basicEventName = eventName;
    }
    
    //theSystem->debug("TimeSensor.%s eventIn %s\n", name(), origEventName);
    
    // Ignore set_cycleInterval & set_startTime when active
    if (basicEventName == "cycleInterval" || eventName == "startTime") {
        if (!d_isActive.get()) {
            d_lastTime = timeStamp;
            this->setField(basicEventName, fieldValue);
            this->eventOut(timeStamp, basicEventName + "_changed", fieldValue);
    }
    } else if (basicEventName == "enabled") {
        // Shutdown if set_enabled FALSE is received when active
        this->setField(basicEventName, fieldValue);
        if (d_isActive.get() && !d_enabled.get()) {
            d_isActive.set(false);

            // Send relevant eventOuts (continuous ones first)
            VrmlSFTime timeNow(timeStamp);
            eventOut(timeStamp, "time", timeNow);
            
        double f;
            double cycleInt = d_cycleInterval.get();
            if (cycleInt > 0.0) {
                f = fmod( timeNow.get() - d_startTime.get(), cycleInt );
            } else {
                f = 0.0;
            }

        // Fraction of cycle message
        VrmlSFFloat fraction_changed(fpzero(f) ? 1.0 : (f / cycleInt));
            
        eventOut(timeStamp, "fraction_changed", fraction_changed);
        eventOut(timeStamp, "isActive", d_isActive);
    }
        
        eventOut(timeStamp, "enabled_changed", fieldValue);
    } else {
        // Let the generic code handle the rest.
        VrmlNode::eventIn(timeStamp, eventName, fieldValue);
    }

    // TimeSensors shouldn't generate redraws.
    clearModified();
}

// Get the value of a field or eventOut.

const VrmlField * VrmlNodeTimeSensor::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "cycleInterval")
    return &d_cycleInterval;
  else if (fieldId == "enabled")
    return &d_enabled;
  else if (fieldId == "loop")
    return &d_loop;
  else if (fieldId == "startTime")
    return &d_startTime;
  else if (fieldId == "stopTime")
    return &d_stopTime;

  // eventOuts
  else if (fieldId == "isActive")
    return &d_isActive;
#if 0                // not implemented yet...
  else if (fieldId == "cycleTime")
    return &d_cycleTime;
  else if (fieldId == "fraction")
    return &d_fraction;
  else if (fieldId == "time")
    return &d_time;
#endif

  return VrmlNodeChild::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTimeSensor::setField(const std::string & fieldId,
                  const VrmlField & fieldValue)
{
  if TRY_FIELD(cycleInterval, SFTime)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(loop, SFBool)
  else if TRY_FIELD(startTime, SFTime)
  else if TRY_FIELD(stopTime, SFTime)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
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


namespace {
    // TouchSensor factory. 
    const VrmlNodePtr createTouchSensor(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeTouchSensor(scene));
    }
}

/**
 * Define the built in NodeType:: "TouchSensor" fields.
 */
const NodeTypePtr VrmlNodeTouchSensor::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("TouchSensor", createTouchSensor));
    st->addExposedField("enabled", VrmlField::SFBOOL);
    st->addEventOut("hitNormal_changed", VrmlField::SFVEC3F);
    st->addEventOut("hitPoint_changed", VrmlField::SFVEC3F);
    st->addEventOut("hitTexCoord_changed", VrmlField::SFVEC2F);
    st->addEventOut("isActive", VrmlField::SFBOOL);
    st->addEventOut("isOver", VrmlField::SFBOOL);
    st->addEventOut("touchTime", VrmlField::SFTIME);

    return st;
}

VrmlNodeTouchSensor::VrmlNodeTouchSensor(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_enabled(true), d_isActive(false),
        d_isOver(false), d_touchTime(0.0) {
    this->setModified();
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

const VrmlField * VrmlNodeTouchSensor::getField(const std::string & fieldId)
        const {
  // exposedFields
  if (fieldId == "enabled")
    return &d_enabled;

  // eventOuts
  else if (fieldId == "hitNormal")
    return &d_hitNormal;
  else if (fieldId == "hitPoint")
    return &d_hitPoint;
  else if (fieldId == "hitTexCoord")
    return &d_hitTexCoord;
  else if (fieldId == "isActive")
    return &d_isActive;
  else if (fieldId == "isOver")
    return &d_isOver;
  else if (fieldId == "touchTime")
    return &d_touchTime;

  return VrmlNodeChild::getField(fieldId); // Parent class
}

// Set the value of one of the node fields.

void VrmlNodeTouchSensor::setField(const std::string & fieldId,
                   const VrmlField & fieldValue)
{
  if TRY_FIELD(enabled, SFBool)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}


/**
 * @class VrmlNodeTransform
 */

/**
 * @var VrmlNodeTransform::M
 *
 * Cached copy (in VrmlMatrix format) of this node's transformation.
 * Currently this is used only by the culling code, but eventually
 * all the matrix manipulation needs to be moved from the Viewer
 * side over into core.
 */

/**
 * @var VrmlNodeTransform::M_dirty
 *
 * If true, we need to recalculate M. Is this the same as
 * VrmlNode::d_modified? No, since it's entirely a core-side issue,
 * and has nothing to do with the viewer being out of date wrt the
 * core scene graph.
 */

namespace {
    const VrmlNodePtr createTransform(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeTransform(scene));
    }
}

/**
 * Define the built in NodeType:: "Transform" fields.
 */
const NodeTypePtr VrmlNodeTransform::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("Transform", createTransform));
    st->addEventIn("addChildren", VrmlField::MFNODE);
    st->addEventIn("removeChildren", VrmlField::MFNODE);
    st->addExposedField("children", VrmlField::MFNODE);
    st->addField("bboxCenter", VrmlField::SFVEC3F);
    st->addField("bboxSize", VrmlField::SFVEC3F);
    st->addExposedField("center", VrmlField::SFVEC3F);
    st->addExposedField("rotation", VrmlField::SFROTATION);
    st->addExposedField("scale", VrmlField::SFVEC3F);
    st->addExposedField("scaleOrientation", VrmlField::SFROTATION);
    st->addExposedField("translation", VrmlField::SFVEC3F);

    return st;
}

VrmlNodeTransform::VrmlNodeTransform(VrmlScene * const scene):
        VrmlNodeGroup(*defineType(), scene), d_center(0.0, 0.0, 0.0),
        d_rotation(0.0, 0.0, 1.0, 0.0), d_scale(1.0, 1.0, 1.0),
        d_scaleOrientation(0.0, 0.0, 1.0, 0.0), d_translation(0.0, 0.0, 0.0),
        d_xformObject(0) {
    this->M_dirty = true;
    this->synch_cached_matrix();
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
  if (! fpzero(d_center.getX()) ||
      ! fpzero(d_center.getY()) ||
      ! fpzero(d_center.getZ()))
    PRINT_FIELD(center);
  if (! fpzero(d_rotation.getX()) ||
      ! fpzero(d_rotation.getY()) ||
      ! fpequal(d_rotation.getZ(), 1.0) ||
      ! fpzero(d_rotation.getAngle()))
    PRINT_FIELD(rotation);
  if (! fpequal(d_scale.getX(), 1.0) ||
      ! fpequal(d_scale.getY(), 1.0) ||
      ! fpequal(d_scale.getZ(), 1.0))
    PRINT_FIELD(scale);
  if (! fpzero(d_scaleOrientation.getX()) ||
      ! fpzero(d_scaleOrientation.getY()) ||
      ! fpequal(d_scaleOrientation.getZ(), 1.0) ||
      ! fpzero(d_scaleOrientation.getAngle()))
    PRINT_FIELD(scaleOrientation);
  if (! fpzero(d_translation.getX()) ||
      ! fpzero(d_translation.getY()) ||
      ! fpzero(d_translation.getZ()))
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

  VrmlMatrix LM;
  synch_cached_matrix();
  this->getMatrix(LM);
  VrmlMatrix new_LM = rc.getMatrix();
  new_LM = new_LM.MMleft(LM);
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
      d_xformObject = viewer->beginObject(this->getId().c_str());

      // Apply transforms
// Why to do again matrix computation in GL side? when this matrix is available
// here. 
//      viewer->setTransform(d_center.get(),
//               d_rotation.get(),
//               d_scale.get(),
//               d_scaleOrientation.get(),
//               d_translation.get());
      VrmlMatrix M;
      this->getMatrix(M);
      viewer->MatrixMultiply(M.get());
      // Render children
      VrmlNodeGroup::renderNoCull(viewer, rc);

      // Reverse transforms (for immediate mode/no matrix stack renderer)
//      viewer->unsetTransform(d_center.get(),
//                 d_rotation.get(),
//                 d_scale.get(),
//                 d_scaleOrientation.get(),
//                 d_translation.get());
      M = M.affine_inverse();
      viewer->MatrixMultiply(M.get());
      viewer->endObject();
    }

  clearModified();
}


// Get the value of a field or eventOut.

const VrmlField * VrmlNodeTransform::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "center")
    return &d_center;
  else if (fieldId == "rotation")
    return &d_rotation;
  else if (fieldId == "scale")
    return &d_scale;
  else if (fieldId == "scaleOrientation")
    return &d_scaleOrientation;
  else if (fieldId == "translation")
    return &d_translation;

  return VrmlNodeGroup::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeTransform::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
{
  if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(rotation, SFRotation)
  else if TRY_FIELD(scale, SFVec3f)
  else if TRY_FIELD(scaleOrientation, SFRotation)
  else if TRY_FIELD(translation, SFVec3f)
  else
    VrmlNodeGroup::setField(fieldId, fieldValue);
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
   viewer->unsetTransform(d_center.get(),
                             d_rotation.get(),
                             d_scale.get(),
                             d_scaleOrientation.get(),
                             d_translation.get());
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(viewer);
} 

void VrmlNodeTransform::inverseTransform(VrmlMatrix & m)
{
// It is calling program's responsibility to pass m as an unit matrix. skb
  VrmlMatrix M;
  synch_cached_matrix();
  this->getMatrix(M);
  M = M.affine_inverse();
  m = m.MMleft(M);
  VrmlNode *parentTransform = getParentTransform();
  if (parentTransform)
    parentTransform->inverseTransform(m);
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

/**
 * calculate transformation matrix. Store it into M
 * Here we are dealing with VrmlMatrix format (Matrices are stored 
 * in row-major order), hence
 * we are doing premultiplication.
 * @param M gets a copy of the resulting transform
 */

void VrmlNodeTransform::transform_to_matrix(const VrmlNodeTransform * t,
                                            VrmlMatrix & M) {
    VrmlMatrix temp;
    M.makeIdentity();
    if(!((t->d_translation[0] == 0.0)
            && (t->d_translation[1] == 0.0)
            && (t->d_translation[2] == 0.0))) {
        temp.setTranslate(t->d_translation);
        M = M.MMleft(temp);            // M = T * M   = T
    }
    if(!((t->d_center[0] == 0.0)
            && (t->d_center[1] == 0.0)
            && (t->d_center[2] == 0.0))) {
        temp.setTranslate(t->d_center);
        M = M.MMleft(temp);            // M = C * M   = C * T   
    }
    if(!((t->d_rotation.getX() == 0.0)
            && (t->d_rotation.getY() == 0.0)
            && (t->d_rotation.getZ() == 1.0)
            && (t->d_rotation.getAngle() == 0.0))) {
        temp.setRotate(t->d_rotation);
        M = M.MMleft(temp);            // M = R * M    = R * C * T
    }
    if(!((t->d_scale[0] == 1.0)
            && (t->d_scale[1] == 1.0)
            && (t->d_scale[2] == 1.0))) {
        if(!((t->d_scaleOrientation.getX() == 0.0)
                && (t->d_scaleOrientation.getY() == 0.0)
                && (t->d_scaleOrientation.getZ() == 1.0)
                && (t->d_scaleOrientation.getAngle() == 0.0))) {
            temp.setRotate(t->d_scaleOrientation);
            M = M.MMleft(temp);            // M = SR * M    = SR * R * C * T 
        }
        temp.setScale(t->d_scale);
        M = M.MMleft(temp);            // M = S * M     = S * SR * R * C * T
        if(!((t->d_scaleOrientation.getX() == 0.0)
                && (t->d_scaleOrientation.getY() == 0.0)
                && (t->d_scaleOrientation.getZ() == 1.0)
                && (t->d_scaleOrientation.getAngle() == 0.0))) {
            temp.setRotate(t->d_scaleOrientation.inverse());
            M = M.MMleft(temp);            // M = -SR * M   = -SR * S * SR * R * C * T
        }
    }
    if(!((t->d_center[0] == 0.0)
            && (t->d_center[1] == 0.0)
            && (t->d_center[2] == 0.0))) {
       temp.setTranslate(t->d_center.negate());
       M = M.MMleft(temp);            // M = -C * M    =  -C * -SR * S * SR * R * C * T
    }
}


// P' = T × C × R × SR × S × -SR × -C × P
//
/**
 * Resynchronize the cached matrix <code>M</code> with the node
 * fields, but only if M_dirty is true. Think logical const.
 */
void
VrmlNodeTransform::synch_cached_matrix()
{
  if (M_dirty) {
    transform_to_matrix(this,M);
    //Mdump(cout, M);
    M_dirty = false;
  }
}

/**
 * Get a matrix representation (in VrmlMatrix format, same as OGL) of the
 * transformation stored in the node fields.
 *
 * @return a copy of the cached transformation matrix
 */
void
VrmlNodeTransform::getMatrix(VrmlMatrix & M_out) const
{
  ((VrmlNodeTransform*)this)->synch_cached_matrix();
  M_out = M;
}


namespace {
    //  Viewpoint factory.
    const VrmlNodePtr createViewpoint(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeViewpoint(scene));
    }
}

/**
 * Define the built in NodeType:: "Viewpoint" fields.
 */
const NodeTypePtr VrmlNodeViewpoint::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    st.reset(new NodeType("Viewpoint", createViewpoint));

    st->addEventIn("set_bind", VrmlField::SFBOOL);
    st->addExposedField("fieldOfView", VrmlField::SFFLOAT);
    st->addExposedField("jump", VrmlField::SFBOOL);
    st->addExposedField("orientation", VrmlField::SFROTATION);
    st->addExposedField("position", VrmlField::SFVEC3F);
    st->addField("description", VrmlField::SFSTRING);
    st->addEventOut("bindTime", VrmlField::SFTIME);
    st->addEventOut("isBound", VrmlField::SFBOOL);

    return st;
}

static const float DEFAULT_FIELD_OF_VIEW = 0.785398;

VrmlNodeViewpoint::VrmlNodeViewpoint(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene),
        d_fieldOfView(DEFAULT_FIELD_OF_VIEW), d_jump(true),
        d_orientation(0.0, 0.0, 1.0, 0.0), d_position(0.0, 0.0, 10.0),
        d_isBound(false), d_bindTime(0), d_parentTransform(0) {
    if (this->d_scene) {
        this->d_scene->addViewpoint(*this);
    }
}

// need copy constructor for d_parentTransform ...

VrmlNodeViewpoint::~VrmlNodeViewpoint() {
    if (this->d_scene) {
        this->d_scene->removeViewpoint(*this);
    }
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

void VrmlNodeViewpoint::addToScene(VrmlScene * scene, const std::string &) {
    if (this->d_scene != scene && (this->d_scene = scene)) {
        this->d_scene->addViewpoint(*this);
    }
}


ostream& VrmlNodeViewpoint::printFields(ostream& os, int indent)
{
  if (! fpequal( d_fieldOfView.get(), DEFAULT_FIELD_OF_VIEW))
    PRINT_FIELD(fieldOfView);
  if (! d_jump.get()) PRINT_FIELD(jump);
  if (! fpzero(d_orientation.getX()) ||
      ! fpzero(d_orientation.getY()) ||
      ! fpequal(d_orientation.getZ(), 1.0) ||
      ! fpzero(d_orientation.getAngle()) )
    PRINT_FIELD(orientation);
  if (! fpzero(d_position.getX()) ||
      ! fpzero(d_position.getY()) ||
      ! fpequal(d_position.getZ(), 10.0) )
    PRINT_FIELD(position);
  if (d_description.get().length() > 0) PRINT_FIELD(description);

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

void VrmlNodeViewpoint::eventIn(double timeStamp, const std::string & eventName,
                                const VrmlField & fieldValue) {
  if (eventName == "set_bind")
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

      if ( b->get() )        // set_bind TRUE
    {
      if (this != current)
        {
          if (current)
        current->eventOut( timeStamp, "isBound", VrmlSFBool(false));
          d_scene->bindablePush( this );
          eventOut( timeStamp, "isBound", VrmlSFBool(true) );
          const std::string & n = this->getId();
          const std::string & d = d_description.get();
          if (n.length() > 0 && d.length() > 0)
        theSystem->inform("%s: %s", n.c_str(), d.c_str());
          else if (d.length() > 0)
        theSystem->inform("%s", d.c_str());
          else if (n.length() > 0)
        theSystem->inform("%s", n.c_str());
        }
    }
      else            // set_bind FALSE
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

const VrmlField * VrmlNodeViewpoint::getField(const std::string & fieldId) const
{
  // exposedFields
  if (fieldId == "fieldOfView")
    return &d_fieldOfView;
  else if (fieldId == "jump")
    return &d_jump;
  else if (fieldId == "orientation")
    return &d_orientation;
  else if (fieldId == "position")
    return &d_position;

  // eventOuts
  else if (fieldId == "bindTime")
    return &d_bindTime;  
  else if (fieldId == "isBound")
    {
      VrmlSFBool* isBound = (VrmlSFBool*) &(this->d_isBound);
      isBound->set( d_scene->bindableViewpointTop() == this );
      return isBound;
    }

  return VrmlNodeChild::getField(fieldId);
}

// Set the value of one of the node fields.

void VrmlNodeViewpoint::setField(const std::string & fieldId,
                 const VrmlField & fieldValue)
{
  if TRY_FIELD(fieldOfView, SFFloat)
  else if TRY_FIELD(jump, SFBool)
  else if TRY_FIELD(orientation, SFRotation)
  else if TRY_FIELD(position, SFVec3f)
  else if TRY_FIELD(description, SFString)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}


/**
 * Determine the inverse of the transform represented by the
 * viewpoint's position and orientation fields. Return the matrix in
 * VrmlMatrix format (same as OGL). Note that this method deals only with the
 * viewpoint node's transform, not with any ancestor transforms.
 *
 * @param IM inverse of the position/orientation transform
 *
 */
void VrmlNodeViewpoint::getInverseMatrix(VrmlMatrix & M) const
{
  VrmlMatrix tmp;
  float rot_aa[4];
  rot_aa[0] =  d_orientation.getX();
  rot_aa[1] =  d_orientation.getY();
  rot_aa[2] =  d_orientation.getZ();
  rot_aa[3] = -d_orientation.getAngle();
  tmp.setRotate(rot_aa);
  float pos_vec[3];
  pos_vec[0] = -d_position.getX();
  pos_vec[1] = -d_position.getY();
  pos_vec[2] = -d_position.getZ();
  M.setTranslate(pos_vec);
  M = M.MMright(tmp);

}


/**
 * @todo Implement me!
 */
void VrmlNodeViewpoint::getFrustum(VrmlFrustum& frust) const
{
    // XXX Implement me!;
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


namespace {
    // VisibilitySensor factory. 
    const VrmlNodePtr createVisibilitySensor(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeVisibilitySensor(scene));
    }
}

/**
 * Define the built in NodeType:: "VisibilitySensor" fields.
 */
const NodeTypePtr VrmlNodeVisibilitySensor::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st; // Only define the type once.
    }
    
    st.reset(new NodeType("VisibilitySensor", createVisibilitySensor));
    st->addExposedField("center", VrmlField::SFVEC3F);
    st->addExposedField("enabled", VrmlField::SFBOOL);
    st->addExposedField("size", VrmlField::SFVEC3F);
    st->addEventOut("enterTime", VrmlField::SFTIME);
    st->addEventOut("exitTime", VrmlField::SFTIME);
    st->addEventOut("isActive", VrmlField::SFBOOL);

    return st;
}

VrmlNodeVisibilitySensor::VrmlNodeVisibilitySensor(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_center(0.0, 0.0, 0.0),
        d_enabled(true), d_size(0.0, 0.0, 0.0), d_isActive(false),
        d_enterTime(0.0), d_exitTime(0.0) {
    this->setModified();
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
  if (! fpzero(d_center.getX()) ||
      ! fpzero(d_center.getY()) ||
      ! fpzero(d_center.getZ()) )
    PRINT_FIELD(center);
  if (! d_enabled.get())
    PRINT_FIELD(enabled);
  if (! fpzero(d_size.getX()) ||
      ! fpzero(d_size.getY()) ||
      ! fpzero(d_size.getZ()) )
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
      if (! fpzero(r) ) r = sqrt( r );

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
          ! fpzero(ni->visibilityLimit()) &&
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

const VrmlField * VrmlNodeVisibilitySensor::getField(const std::string & fieldId)
        const {
  // exposedFields
  if (fieldId == "center")
    return &d_center;
  else if (fieldId == "enabled")
    return &d_enabled;
  else if (fieldId == "size")
    return &d_size;

  // eventOuts
  else if (fieldId == "isActive")
    return &d_isActive;  
  else if (fieldId == "enterTime")
    return &d_enterTime;  
  else if (fieldId == "exitTime")
    return &d_exitTime;  

  return VrmlNodeChild::getField(fieldId);
}


// Set the value of one of the node fields.

void VrmlNodeVisibilitySensor::setField(const std::string & fieldId,
                    const VrmlField & fieldValue)
{
  if TRY_FIELD(center, SFVec3f)
  else if TRY_FIELD(enabled, SFBool)
  else if TRY_FIELD(size, SFVec3f)
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}


namespace {
    //  WorldInfo factory.
    const VrmlNodePtr createWorldInfo(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeWorldInfo(scene));
    }
}

/**
 * Define the built in NodeType:: "WorldInfo" fields.
 */
const NodeTypePtr VrmlNodeWorldInfo::defineType() {
    static NodeTypePtr st(0);

    if (st) {
        return st;
    }
    
    st.reset(new NodeType("WorldInfo", createWorldInfo));
    st->addField("info", VrmlField::MFSTRING);
    st->addField("title", VrmlField::SFSTRING);

    return st;
}

VrmlNodeWorldInfo::VrmlNodeWorldInfo(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene) {}

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
  if (d_title.get().length() > 0) PRINT_FIELD(title);
  if (d_info.getLength() > 0) PRINT_FIELD(info);

  return os;
}

// Set the value of one of the node fields.

void VrmlNodeWorldInfo::setField(const std::string & fieldId,
                                 const VrmlField & fieldValue)
{
  if TRY_FIELD(info, MFString)
  else if TRY_FIELD(title, SFString)
  else
    VrmlNode::setField(fieldId, fieldValue);
}

VrmlNodeTexture::VrmlNodeTexture(const NodeType & type,
                                 VrmlScene * const scene):
        VrmlNode(type, scene) {}

VrmlNodeTexture::~VrmlNodeTexture() {}

VrmlNodeTexture* VrmlNodeTexture::toTexture() const
{ return (VrmlNodeTexture*) this; }

