// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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
//  Viewer.cpp
//  Abstract base class for display of VRML models
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include "Viewer.h"
# include "private.h"

namespace OpenVRML {

/**
 * @class Viewer
 *
 * @brief Map the scene graph to the underlying graphics library.
 */

/**
 * @var Viewer::MASK_NONE
 */

/**
 * @var Viewer::MASK_CCW
 */

/**
 * @var Viewer::MASK_CONVEX
 */

/**
 * @var Viewer::MASK_SOLID
 */

/**
 * @var Viewer::MASK_BOTTOM
 */

/**
 * @var Viewer::MASK_TOP
 */

/**
 * @var Viewer::MASK_SIDE
 */

/**
 * @var Viewer::MASK_COLOR_PER_VERTEX
 */

/**
 * @var Viewer::MASK_NORMAL_PER_VERTEX
 */

/**
 * @enum Viewer::RenderMode
 *
 * @brief The rendering mode.
 */

/**
 * @var Viewer::RenderMode Viewer::RENDER_MODE_DRAW
 *
 * @brief Draw mode.
 */

/**
 * @var Viewer::RenderMode Viewer::RENDER_MODE_PICK
 *
 * @brief Pick mode.
 */

/**
 * @typedef Viewer::Object
 *
 * @brief An object handle.
 */

/**
 * @typedef Viewer::TextureObject
 *
 * @brief An texture object handle.
 */

/**
 * @var OpenVRML::browser & Viewer::browser
 *
 * @brief The browser associated with the Viewer.
 */

/**
 * @brief Constructor.
 *
 * @param browser   a browser.
 */
Viewer::Viewer(OpenVRML::browser & browser):
    browser(browser)
{}

/**
 * @brief Destructor.
 */
Viewer::~Viewer() {}

/**
 * @fn Viewer::RenderMode Viewer::getRenderMode()
 *
 * @brief Get the rendering mode.
 *
 * @return the rendering mode.
 */

/**
 * @fn double Viewer::getFrameRate()
 *
 * @brief Get the frame rate.
 *
 * @return the frame rate.
 */

/**
 * @fn void Viewer::resetUserNavigation()
 *
 * @brief Return view to the last bound Viewpoint.
 */

/**
 * @fn void Viewer::getUserNavigation(mat4f & M)
 *
 * @brief Get the user's navigation from the last bound viewpoint.
 *
 * @retval M    orthonormal navigation transformation.
 */

/**
 * @fn Viewer::Object Viewer::beginObject(const char *, bool)
 *
 * @brief Begin a display list.
 */

/**
 * @fn void Viewer::endObject()
 *
 * @brief End a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertBackground(const std::vector<float> & groundAngle, const std::vector<color> & groundColor, const std::vector<float> & skyAngle, const std::vector<color> & skyColor, int * whc, unsigned char ** pixels)
 *
 * @brief Insert a background into a display list.
 *
 * @param groundAngle   ground angles.
 * @param groundColor   ground colors.
 * @param skyAngle      sky angles.
 * @param skyColor      sky colors.
 * @param whc           texture width, height, and number of components.
 * @param pixels        texture pixel data.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertBox(const vec3f & size)
 *
 * @brief Insert a box into a display list.
 *
 * @param size  box dimensions.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertCone(float height, float radius, bool bottom, bool side)
 *
 * @brief Insert a cone into a display list.
 *
 * @param height    height.
 * @param radius    radius at base.
 * @param bottom    show the bottom.
 * @param side      show the side.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertCylinder(float height, float radius, bool bottom, bool side, bool top)
 *
 * @brief Insert a cylinder into a display list.
 *
 * @param height    height.
 * @param radius    radius.
 * @param bottom    show the bottom.
 * @param side      show the side.
 * @param top       show the top.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertElevationGrid(unsigned int mask, const std::vector<float> & height, int32 xDimension, int32 zDimension, float xSpacing, float zSpacing, const std::vector<color> & color, const std::vector<vec3f> & normal, const std::vector<vec2f> & texCoord)
 *
 * @brief Insert an elevation grid into a display list.
 *
 * @param mask
 * @param height        height field.
 * @param xDimension    vertices in the x direction.
 * @param zDimension    vertices in the z direction.
 * @param xSpacing      distance between vertices in the x direction.
 * @param zSpacing      distance between vertices in the z direction.
 * @param color         colors.
 * @param normal        normals.
 * @param texCoord      texture coordinates.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertExtrusion(unsigned int mask, const std::vector<vec3f> & spine, const std::vector<vec2f> & crossSection, const std::vector<rotation> & orientation, const std::vector<vec2f> & scale)
 *
 * @brief Insert an extrusion into a display list.
 *
 * @param mask
 * @param spine         spine points.
 * @param crossSection  cross-sections.
 * @param orientation   cross-section orientations.
 * @param scale         cross-section scales.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertLineSet(const std::vector<vec3f> & coord, const std::vector<int32> & coordIndex, bool colorPerVertex, const std::vector<color> & color, const std::vector<int32> & colorIndex)
 *
 * @brief Insert a line set into a display list.
 *
 * @param coord             coordinates.
 * @param coordIndex        coordinate indices.
 * @param colorPerVertex    whether colors are applied per-vertex or per-face.
 * @param color             colors.
 * @param colorIndex        color indices.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertPointSet(const std::vector<vec3f> & coord, const std::vector<color> & color)
 *
 * @brief Insert a point set into a display list.
 *
 * @param coord     points.
 * @param color     colors.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertShell(unsigned int mask, const std::vector<vec3f> & coord, const std::vector<int32> & coordIndex, const std::vector<color> & color, const std::vector<int32> & colorIndex, const std::vector<vec3f> & normal, const std::vector<int32> & normalIndex, const std::vector<vec2f> & texCoord, const std::vector<int32> & texCoordIndex)
 *
 * @brief Insert a shell into a display list.
 *
 * @param mask
 * @param coord         coordinates.
 * @param coordIndex    coordinate indices.
 * @param color         colors.
 * @param colorIndex    color indices.
 * @param normal        normals.
 * @param normalIndex   normal indices.
 * @param texCoord      texture coordinates.
 * @param texCoordIndex texture coordinate indices.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertSphere(float radius)
 *
 * @brief Insert a sphere into a display list.
 *
 * @param radius    sphere radius.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertDirLight(float ambientIntensity, float intensity , const color & color, const vec3f & direction)
 *
 * @brief Insert a directional light into a display list.
 *
 * @param ambientIntensity  ambient intensity.
 * @param intensity         intensity.
 * @param color             color.
 * @param direction         direction.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertPointLight(float ambientIntensity, const vec3f & attenuation, const color & color, float intensity, const vec3f & location, float radius)
 *
 * @brief Insert a point light into a display list.
 *
 * @param ambientIntensity  ambient intensity.
 * @param attenuation       attenuation.
 * @param color             color.
 * @param intensity         intensity.
 * @param location          location.
 * @param radius            radius.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertSpotLight(float ambientIntensity, const vec3f & attenuation, float beamWidth, const color & color, float cutOffAngle, const vec3f & direction, float intensity, const vec3f & location, float radius)
 *
 * @brief Insert a point light into a display list.
 *
 * @param ambientIntensity  ambient intensity.
 * @param attenuation       attenuation.
 * @param beamWidth         beam width.
 * @param color             color.
 * @param cutOffAngle       cut-off angle.
 * @param direction         direction.
 * @param intensity         intensity.
 * @param location          location.
 * @param radius            radius.
 *
 * @return display object identifier.
 */

/**
 * @fn Viewer::Object Viewer::insertReference(Object existingObject)
 *
 * @brief Insert a reference to an existing object into a display list.
 */

/**
 * @fn void Viewer::setFog(const color & color, float visibilityRange, const char * fogType)
 *
 * @brief Set the fog.
 *
 * @param color             fog color.
 * @param visibilityRange   the distance at which objects are fully obscured by
 *                          fog.
 * @param fogType           fog type.
 */

/**
 * @fn void Viewer::setColor(const color & rgb, float a)
 *
 * @brief Set the color.
 *
 * @param rgb   red, green, and blue components.
 * @param a     alpha (transparency) component.
 */

/**
 * @fn void Viewer::setMaterial(float ambientIntensity, const color & diffuseColor, const color & emissiveColor, float shininess, const color & specularColor, float transparency)
 *
 * @brief Set the material.
 *
 * @param ambientIntensity  ambient intensity.
 * @param diffuseColor      diffuse color.
 * @param emissiveColor     emissive color.
 * @param shininess         shininess.
 * @param specularColor     specular color.
 * @param transparency      transparency.
 */

/**
 * @fn void Viewer::setTextureTransform(const vec2f & center, float rotation, const vec2f & scale, const vec2f & translation)
 *
 * @brief Set the texture transform.
 *
 * @param center        center.
 * @param rotation      rotation.
 * @param scale         scale.
 * @param translation   translation.
 */

/**
 * @fn void Viewer::setViewpoint(const vec3f & position, const rotation & orientation, float fieldOfView, float avatarSize, float visLimit)
 *
 * @brief Set the viewpoint.
 *
 * @param position          position.
 * @param orienation        orientation.
 * @param fieldOfView       field of view.
 * @param avatarSize        avatar size.
 * @param visibilityLimit   visiblity limit.
 */

/**
 * @fn void Viewer::transform(const mat4f & mat)
 *
 * @brief Transform the modelview.
 *
 * Make the modelview matrix the result of multiplying @p mat by the current
 * modelview matrix.
 *
 * @param mat   transformation matrix.
 */

/**
 * Intersect the given bounding volume with the view volume. This
 * goes into the viewer to provide a hook for systems that use
 * non-standard view volumes. Most subclasses should be able to use
 * the default implementation provided here. If your view volume is
 * so strange that there's no way to cull to is, then reimplement to
 * always return BVolume::inside.
 *
 * @param bvolume   the bounding volume to intersect with the view volume.
 *
 * @return BVolume::inside, BVolume::outside, or BVolume::partial.
 */
BVolume::Intersection Viewer::intersectViewVolume(const BVolume & bvolume) const
{
    //
    // For normal VRML97 use, this won't need to be overridden, but for
    // systems with non-standard view volumes, this can be changed to
    // cull as appropriate. Note that culling can be disabled by setting
    // a flag in VrmlRenderContext. Since I don't have access to the
    // appropriate cave/boom/whichever api's, I can't be sure that this
    // is enough. If it isn't, please express any concerns to the
    // OpenVRML developer's list, and it can be fixed...
    //
    return bvolume.intersectFrustum(this->d_frust);
}

/**
 * @fn void Viewer::drawBSphere(const BSphere & bs, BVolume::Intersection intersection)
 *
 * Draw a bounding sphere. Used for debugging view culling. Probably
 * should be drawBVolume and handle aaboxes as well.
 *
 * @param bs            a bounding sphere; if max, will not be drawn
 * @param intersection  one of the bvolume intersection test constants, or 4
 *                      to draw in unique way. (useful for debugging)
 */

/**
 * @todo We're forcing everybody to carry around a frust
 * whether they want it or not. It shouldn't be used except
 * for debugging and stuff since it might not be valid in some
 * implementations
 */
const VrmlFrustum & Viewer::getFrustum() const { return this->d_frust; }

void Viewer::setFrustum(const VrmlFrustum & afrust) {
    this->d_frust = afrust; // copy
}

} // namespace OpenVRML
