LIBRARY_VERSION = 2:0:0
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

SUBDIRS = antlr javascript

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

#
# JS doesn't include config.h, so we have to do this here.
#
JS_CPPFLAGS = -DXP_UNIX

AM_CPPFLAGS = $(DEBUG_CPPFLAGS) $(NO_STD_NAMESPACE_DEF) $(JS_CPPFLAGS)
AM_CXXFLAGS = -Wall @JPEG_CFLAGS@ @PNG_CFLAGS@

INCLUDES = -I./javascript

libvrml97_la_LDFLAGS = -version-info $(LIBRARY_VERSION) @JPEG_LIBS@ @PNG_LIBS@

lib_LTLIBRARIES = libvrml97.la 

BUILT_SOURCES = Vrml97Parser.cpp Vrml97Parser.hpp Vrml97ParserTokenTypes.hpp \
Vrml97ParserTokenTypes.txt

Vrml97Parser.cpp Vrml97Parser.hpp Vrml97ParserTokenTypes.hpp \
Vrml97ParserTokenTypes.txt: vrml97parser.g Vrml97TokenTypes.txt
	$(JAVA) antlr.Tool $<

vrml97scanner.o vrml97scanner.lo: vrml97scanner.cpp vrml97scanner.hpp
	$(LTCXXCOMPILE) -c $<

Vrml97Parser.o Vrml97Parser.lo: Vrml97Parser.cpp Vrml97Parser.hpp VrmlField.h \
doc2.hpp VrmlNamespace.h VrmlNodeType.h VrmlMFNode.h VrmlSFString.h VrmlNode.h \
System.h VrmlNodeScript.h VrmlNodeChild.h VrmlMFString.h VrmlSFBool.h \
VrmlSFColor.h VrmlSFFloat.h VrmlSFImage.h VrmlSFInt.h VrmlSFNode.h \
VrmlSFRotation.h VrmlSFTime.h VrmlSFVec2f.h VrmlSFVec3f.h VrmlMFColor.h \
VrmlMFFloat.h VrmlMFInt.h VrmlMFNode.h VrmlMFRotation.h VrmlMFString.h \
VrmlMFTime.h VrmlMFVec2f.h VrmlMFVec3f.h
	$(LTCXXCOMPILE) -c $<

ScriptJS.o ScriptJS.lo: ScriptJS.cpp ScriptJS.h
	$(LTCXXCOMPILE) -c $<

libvrml97_la_SOURCES = \
        Doc.cpp \
	doc2.cpp \
	MathUtils.cpp \
	gifread.cpp \
	jpgread.cpp \
	mpgread.cpp \
	pngread.cpp \
        dummysound.cpp \
	VrmlScene.cpp VrmlNodeType.cpp VrmlNamespace.cpp \
	Image.cpp Audio.cpp System.cpp \
	ScriptObject.cpp \
	Viewer.cpp \
	VrmlField.cpp \
	VrmlNode.cpp \
	VrmlNodeAnchor.cpp \
	VrmlNodeAppearance.cpp \
	VrmlNodeAudioClip.cpp \
	VrmlNodeBackground.cpp \
	VrmlNodeBillboard.cpp \
	VrmlNodeBox.cpp \
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
	VrmlNodeProto.cpp \
	VrmlBVolume.cpp \
	VrmlBSphere.cpp \
	VrmlAABox.cpp \
	VrmlFrustum.cpp \
	VrmlRenderContext.cpp


antlr-prelinked.lo: vrml97scanner.lo Vrml97Parser.lo antlr/libantlr.la
	$(CXXLINK) $^

scriptjs-prelinked.lo: ScriptJS.lo javascript/libjs.la
	$(CXXLINK) $^

libvrml97_la_LIBADD = antlr-prelinked.lo scriptjs-prelinked.lo

vrml97includedir = $(includedir)/vrml97
vrml97include_HEADERS = \
	doc2.hpp \
        vrml97scanner.hpp \
        Vrml97Parser.hpp \
        Vrml97ParserTokenTypes.hpp \
	Audio.h \
	Doc.h \
	Image.h \
	System.h \
	ScriptObject.h \
	ScriptJS.h \
	Viewer.h \
	VrmlEvent.h \
        VrmlNamespace.h \
        VrmlScene.h \
	VrmlField.h \
	VrmlSFBool.h \
        VrmlSFColor.h \
        VrmlSFFloat.h \
        VrmlSFImage.h \
        VrmlSFInt.h \
        VrmlSFNode.h \
	VrmlSFRotation.h \
        VrmlSFString.h \
        VrmlSFTime.h \
        VrmlSFVec2f.h \
        VrmlSFVec3f.h \
	VrmlMFColor.h \
        VrmlMFFloat.h \
        VrmlMFInt.h \
        VrmlMFNode.h \
        VrmlMFRotation.h \
	VrmlMFString.h \
        VrmlMFTime.h \
        VrmlMFVec2f.h \
        VrmlMFVec3f.h \
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
	VrmlNodeWorldInfo.h \
	VrmlNodeProto.h \
	VrmlBVolume.h \
	VrmlBSphere.h \
	VrmlAABox.h \
	VrmlFrustum.h \
	VrmlRenderContext.h

noinst_HEADERS = MathUtils.h gifread.h jpgread.h mpgread.h pngread.h sound.h

EXTRA_DIST = vrml97scanner.cpp Vrml97TokenTypes.txt.in Vrml97Parser.cpp \
Vrml97Parser.hpp Vrml97ParserTokenTypes.txt vrml97parser.g ScriptJS.cpp
