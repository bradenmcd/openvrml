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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Viewer.h"
#include "private.h"
#include "MathUtils.h"
#include "browser.h"

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
 * @var Browser & Viewer::browser
 *
 * @brief The Browser associated with the Viewer.
 */

/**
 * @brief Constructor.
 *
 * @param browser   a Browser.
 */
Viewer::Viewer(Browser & browser): browser(browser) {}

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
 * @fn void Viewer::getUserNavigation(VrmlMatrix & M)
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
 * @fn Viewer::Object Viewer::insertBackground(size_t nGroundAngles, const float * groundAngle, const float * groundColor, size_t nSkyAngles, const float * skyAngle, const float * skyColor, int * whc, unsigned char ** pixels)
 *
 * @brief Insert a background into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertBox(float, float, float)
 *
 * @brief Insert a box into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertCone(float, float, bool, bool)
 *
 * @brief Insert a cone into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertCylinder(float, float, bool, bool, bool)
 *
 * @brief Insert a cylinder into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertElevationGrid(unsigned int mask, size_t nx, size_t nz, const float * height, float dx, float dz, const float * tc, const float * normals, const float * colors)
 *
 * @brief Insert an elevation grid into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertExtrusion(unsigned int, size_t nOrientation, const float * orientation, size_t nScale, const float * scale, size_t nCrossSection, const float * crossSection, size_t nSpine, const float * spine)
 *
 * @brief Insert an extrusion into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertLineSet(size_t nCoords, const float * coord, size_t nCoordIndex, const long * coordIndex, bool colorPerVertex, const float * color, size_t nColorIndex, const long * colorIndex)
 *
 * @brief Insert a line set into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertPointSet(size_t nv, const float * v, const float * c)
 *
 * @brief Insert a point set into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertShell(unsigned int mask, size_t npoints, const float * points, size_t nfaces, const long * faces, const float * tc, size_t ntci, const long * tci, const float * normal, size_t nni, const long * ni, const float * color, size_t nci, const long * ci)
 *
 * @brief Insert a shell into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertSphere(float radius)
 *
 * @brief Insert a sphere into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertText(int*, float size, int n, char const * const *s)
 *
 * @brief Insert text into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertDirLight(float, float, const float [], const float [])
 *
 * @brief Insert a directional light into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertPointLight(float, const float [], const float [], float, const float [], float)
 *
 * @brief Insert a point light into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertSpotLight(float ambientIntensity, const float attenuation[], float beamWidth, const float color[], float cutOffAngle, const float direction[], float intensity, const float location[], float radius)
 *
 * @brief Insert a point light into a display list.
 */

/**
 * @fn Viewer::Object Viewer::insertReference(Obect existingObject)
 *
 * @brief Insert a reference to an existing object into a display list.
 */

/**
 * @fn void MatrixMultiply(const float M[4][4])
 *
 * @brief Multiply current ModelView Matrix with Given Matrix M
 *
 * @param M matrix in VrmlMatrix format (Same as OGL)
 */

/**
 * @brief Set the color.
 *
 * The default implementation is a nop.
 */
void Viewer::setColor(float, float, float, float) {}

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
 * @fn void Viewer::drawBSphere(const BSphere & bs, int flag)
 *
 * Draw a bounding sphere. Used for debugging view culling. Probably
 * should be drawBVolume and handle aaboxes as well.
 *
 * @param bs    a bounding sphere; if max, will not be drawn
 * @param flag  one of the bvolume intersection test constants, or 4
 *              to draw in unique way. (useful for debugging)
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
