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
  DEBUG_CXXFLAGS = -g
else
  DEBUG_CPPFLAGS = -DNDEBUG
endif

INCLUDES = -I$(top_srcdir)/libvrml97js/src -I../../../libvrml97js/src/vrml97js
AM_CPPFLAGS = $(DEBUG_CPPFLAGS) $(NO_STD_NAMESPACE_DEF)
AM_CXXFLAGS = -Wall $(DEBUG_CXXFLAGS) @JPEG_CFLAGS@ @PNG_CFLAGS@
AM_LFLAGS = -Ls
AM_YFLAGS = -d
LDFLAGS = -version-info @LIBVRML97_SO_VERSION@ @JPEG_LIBS@ @PNG_LIBS@

vrml97includedir = $(includedir)/vrml97

lib_LTLIBRARIES = libvrml97.la 

vrml97include_HEADERS = \
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
	VrmlNodeProto.h

noinst_HEADERS = \
	MathUtils.h \
	gifread.h \
	jpgread.h \
	pngread.h \
	parser.h \
	sound.h

BUILT_SOURCES = lexer.cpp parser.cpp
EXTRA_DIST = lexer.lpp parser.ypp

LEX_OUTPUT_ROOT=lex.yy

lexer.cpp : lexer.lpp
	$(LEX) $(AM_LFLAGS) $(LFLAGS) $< && mv $(LEX_OUTPUT_ROOT).c $@
parser.cpp : parser.ypp
	$(YACC) $(AM_YFLAGS) $(YFLAGS) $< && mv y.tab.c $*.cpp
	if test -f y.tab.h; then \
	if cmp -s y.tab.h $*.h; then rm -f y.tab.h; else mv y.tab.h $*.h; fi; \
	else :; fi

libvrml97_la_SOURCES = \
	parser.cpp lexer.cpp \
	MathUtils.cpp \
	gifread.cpp \
	jpgread.cpp \
	pngread.cpp \
        dummysound.cpp \
	VrmlScene.cpp VrmlNodeType.cpp VrmlNamespace.cpp \
	Doc.cpp Image.cpp Audio.cpp System.cpp \
	ScriptObject.cpp \
	ScriptJS.cpp \
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
	VrmlNodeProto.cpp
