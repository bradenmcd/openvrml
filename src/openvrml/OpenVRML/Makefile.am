LIBRARY_VERSION = 0:0:0
#                 | | |
#          +------+ | +---+
#          |        |     |
#       current:revision:age
#          |        |     |
#          |        |     +- Increment if interfaces have been added
#          |        |        Set to zero if interfaces have been removed or
#          |        |        changed
#          |        +- Increment if source code has changed
#          |           Set to zero if current is incremented
#          +- Increment if interfaces have been added, removed or changed

#
# Blow off "std" if the compiler doesn't support namespaces
#
if NO_CXX_NAMESPACES
  NO_STD_NAMESPACE_DEF = -Dstd=""
endif

#
# Add any debug flags
#
if DEBUG
else
  DEBUG_CPPFLAGS = -DNDEBUG
endif

if DISABLE_JAVASCRIPT
else
#
# JS doesn't include config.h, so we have to do this here.
#
JS_CPPFLAGS = -DXP_UNIX
JAVASCRIPT_SUPPORT_LIBOBJ = scriptjs-prelinked.lo
JAVASCRIPT_INCLUDES = -I$(top_builddir)/lib/js
ScriptJS.o ScriptJS.lo: ScriptJS.cpp ScriptJS.h ScriptObject.h field.h \
doc2.hpp MathUtils.h System.h VrmlNodeScript.h VrmlNodeChild.h VrmlNode.h \
VrmlRenderContext.h VrmlNamespace.h VrmlNodeType.h VrmlScene.h VrmlNodeGroup.h \
VrmlBSphere.h VrmlBVolume.h Viewer.h VrmlFrustum.h
	$(LTCXXCOMPILE) -c $<

$(JAVASCRIPT_SUPPORT_LIBOBJ): ScriptJS.lo $(top_builddir)/lib/js/libjs.la
	$(CXXLINK) $^
endif

INCLUDES = -I$(top_srcdir)/lib $(JAVASCRIPT_INCLUDES)
AM_CPPFLAGS = $(DEBUG_CPPFLAGS) $(NO_STD_NAMESPACE_DEF) $(JS_CPPFLAGS)
AM_CXXFLAGS = @JPEG_CFLAGS@ @PNG_CFLAGS@

lib_LTLIBRARIES = libopenvrml.la 
libopenvrml_la_LDFLAGS = -version-info $(LIBRARY_VERSION) @JPEG_LIBS@ @PNG_LIBS@

Vrml97Scanner.o Vrml97Scanner.lo: Vrml97Scanner.cpp Vrml97Scanner.h
	$(LTCXXCOMPILE) -c $<

Vrml97Parser.o Vrml97Parser.lo: Vrml97Parser.cpp Vrml97Parser.hpp field.h \
doc2.hpp VrmlNamespace.h VrmlNodeType.h VrmlNode.h System.h VrmlNodeScript.h \
VrmlNodeChild.h
	$(LTCXXCOMPILE) -c $<

libopenvrml_la_SOURCES = \
        Doc.cpp \
	doc2.cpp \
	MathUtils.cpp \
        dummysound.cpp \
	Image.cpp Audio.cpp System.cpp \
	Viewer.cpp \
	VrmlBVolume.cpp \
	VrmlBSphere.cpp \
	VrmlAABox.cpp \
	VrmlFrustum.cpp \
	VrmlRenderContext.cpp \
	field.cpp \
        VrmlNamespace.cpp \
	VrmlNodeType.cpp \
        VrmlNodePtr.cpp \
        Route.cpp \
        VrmlNodeVisitor.cpp \
	VrmlNode.cpp \
	VrmlNodeProto.cpp \
	VrmlNodeAnchor.cpp \
	VrmlNodeAppearance.cpp \
	VrmlNodeAudioClip.cpp \
	VrmlNodeBackground.cpp \
	VrmlNodeBillboard.cpp \
	VrmlNodeBox.cpp \
        VrmlNodeChild.cpp \
	VrmlNodeCollision.cpp \
	VrmlNodeColor.cpp \
	VrmlNodeColorInt.cpp \
	VrmlNodeCone.cpp \
	VrmlNodeCoordinate.cpp \
	VrmlNodeCoordinateInt.cpp \
	VrmlNodeCylinder.cpp \
	VrmlNodeCylinderSensor.cpp \
	VrmlNodeDirLight.cpp \
	VrmlNodeElevationGrid.cpp \
	VrmlNodeExtrusion.cpp \
	VrmlNodeFog.cpp \
	VrmlNodeFontStyle.cpp \
	VrmlNodeGeometry.cpp \
	VrmlNodeGroup.cpp \
	VrmlNodeIFaceSet.cpp \
	VrmlNodeILineSet.cpp \
	VrmlNodeImageTexture.cpp \
	VrmlNodeInline.cpp \
	VrmlNodeIndexedSet.cpp \
	VrmlNodeLOD.cpp \
	VrmlNodeLight.cpp \
	VrmlNodeMaterial.cpp \
	VrmlNodeMovieTexture.cpp \
	VrmlNodeNavigationInfo.cpp \
	VrmlNodeNormal.cpp \
	VrmlNodeNormalInt.cpp \
	VrmlNodeOrientationInt.cpp \
	VrmlNodePixelTexture.cpp \
	VrmlNodePlaneSensor.cpp \
	VrmlNodePointLight.cpp \
	VrmlNodePointSet.cpp \
	VrmlNodePositionInt.cpp \
	VrmlNodeProximitySensor.cpp \
	VrmlNodeScalarInt.cpp \
	ScriptObject.cpp \
	VrmlNodeScript.cpp \
	VrmlNodeShape.cpp \
	VrmlNodeSound.cpp \
	VrmlNodeSphere.cpp \
	VrmlNodeSphereSensor.cpp \
	VrmlNodeSpotLight.cpp \
	VrmlNodeSwitch.cpp \
	VrmlNodeText.cpp \
	VrmlNodeTextureCoordinate.cpp \
	VrmlNodeTextureTransform.cpp \
	VrmlNodeTimeSensor.cpp \
	VrmlNodeTouchSensor.cpp \
	VrmlNodeTransform.cpp \
	VrmlNodeViewpoint.cpp \
	VrmlNodeVisibilitySensor.cpp \
	VrmlNodeWorldInfo.cpp \
        VrmlScene.cpp


antlr-prelinked.lo: Vrml97Scanner.lo Vrml97Parser.lo \
$(top_builddir)/lib/antlr/libantlr.la
	$(CXXLINK) $^

libopenvrml_la_LIBADD = antlr-prelinked.lo $(JAVASCRIPT_SUPPORT_LIBOBJ)

OpenVRMLincludedir = $(includedir)/OpenVRML
OpenVRMLinclude_HEADERS = \
	Audio.h \
	Doc.h \
	Image.h \
        MathUtils.h \
        Route.h \
	ScriptObject.h \
	System.h \
	Viewer.h \
	VrmlAABox.h \
	VrmlBVolume.h \
	VrmlBSphere.h \
	VrmlEvent.h \
	VrmlFrustum.h \
	field.h \
        VrmlNamespace.h \
        VrmlNodePtr.h \
	VrmlNode.h \
	VrmlNodeType.h \
	VrmlNodeAnchor.h \
	VrmlNodeAppearance.h \
	VrmlNodeAudioClip.h \
	VrmlNodeBackground.h \
	VrmlNodeBillboard.h \
	VrmlNodeBox.h \
	VrmlNodeChild.h \
	VrmlNodeCollision.h \
	VrmlNodeColor.h \
	VrmlNodeColorInt.h \
	VrmlNodeCone.h \
	VrmlNodeCoordinate.h \
	VrmlNodeCoordinateInt.h \
	VrmlNodeCylinder.h \
	VrmlNodeCylinderSensor.h \
	VrmlNodeDirLight.h \
	VrmlNodeElevationGrid.h \
	VrmlNodeExtrusion.h \
	VrmlNodeFog.h \
	VrmlNodeFontStyle.h \
	VrmlNodeGeometry.h \
	VrmlNodeGroup.h \
	VrmlNodeIFaceSet.h \
	VrmlNodeILineSet.h \
	VrmlNodeImageTexture.h \
	VrmlNodeInline.h \
	VrmlNodeIndexedSet.h \
	VrmlNodeLOD.h \
	VrmlNodeLight.h \
	VrmlNodeMaterial.h \
	VrmlNodeMovieTexture.h \
	VrmlNodeNavigationInfo.h \
	VrmlNodeNormal.h \
	VrmlNodeNormalInt.h \
	VrmlNodeOrientationInt.h \
	VrmlNodePixelTexture.h \
	VrmlNodePlaneSensor.h \
	VrmlNodePointLight.h \
	VrmlNodePointSet.h \
	VrmlNodePositionInt.h \
	VrmlNodeProto.h \
	VrmlNodeProximitySensor.h \
	VrmlNodeScalarInt.h \
	VrmlNodeScript.h \
	VrmlNodeShape.h \
	VrmlNodeSound.h \
	VrmlNodeSphere.h \
	VrmlNodeSphereSensor.h \
	VrmlNodeSpotLight.h \
	VrmlNodeSwitch.h \
	VrmlNodeText.h \
	VrmlNodeTexture.h \
	VrmlNodeTextureCoordinate.h \
	VrmlNodeTextureTransform.h \
	VrmlNodeTimeSensor.h \
	VrmlNodeTouchSensor.h \
	VrmlNodeTransform.h \
	VrmlNodeViewpoint.h \
	VrmlNodeVisibilitySensor.h \
        VrmlNodeVisitor.h \
	VrmlNodeWorldInfo.h \
	VrmlRenderContext.h \
        VrmlScene.h \
	doc2.hpp

noinst_HEADERS = Vrml97Parser.hpp Vrml97ParserTokenTypes.hpp ScriptJS.h \
sound.h Vrml97Scanner.h

EXTRA_DIST = Vrml97Scanner.cpp Vrml97TokenTypes.txt Vrml97Parser.cpp \
Vrml97Parser.hpp Vrml97ParserTokenTypes.txt Vrml97Parser.g ScriptJS.cpp
