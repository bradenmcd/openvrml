# Microsoft Developer Studio Project File - Name="libvrml97core" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libvrml97core - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libvrml97core.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libvrml97core.mak" CFG="libvrml97core - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libvrml97core - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libvrml97core - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libvrml97core - Win32 Release"
# Name "libvrml97core - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\vrml97\Audio.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\Doc.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\doc2.hpp
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\gifread.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\Image.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\jpgread.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\MathUtils.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\mpgread.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\pngread.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\ScriptJDK.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\ScriptJS.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\ScriptObject.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\sound.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\System.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\Viewer.h
# End Source File
# Begin Source File

SOURCE=.\Vrml97Parser.hpp
# End Source File
# Begin Source File

SOURCE=.\Vrml97ParserTokenTypes.hpp
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\vrml97scanner.hpp
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlEvent.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlField.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlFrustum.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlMFColor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlMFFloat.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlMFInt.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlMFRotation.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlMFString.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlMFVec2f.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlMFVec3f.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNamespace.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNode.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeAnchor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeAppearance.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeAudioClip.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeBackground.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeBillboard.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeBindable.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeBox.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeChild.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCollision.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeColor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeColorInt.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCone.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCoordinate.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCoordinateInt.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCylinder.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCylinderSensor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeDirLight.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeElevationGrid.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeExtrusion.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeFog.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeFontStyle.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeGeometry.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeGroup.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeIFaceSet.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeILineSet.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeImageTexture.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeIndexedSet.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeInline.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeLight.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeLOD.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeMaterial.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeMovieTexture.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeNavigationInfo.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeNormal.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeNormalInt.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeOrientationInt.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodePixelTexture.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodePlaneSensor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodePointLight.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodePointSet.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodePositionInt.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeProto.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeProximitySensor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeScalarInt.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeScript.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeShape.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeSound.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeSphere.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeSphereSensor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeSpotLight.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeSwitch.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeText.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTexture.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTextureCoordinate.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTextureTransform.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTimeSensor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTouchSensor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTransform.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeType.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeViewpoint.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeVisibilitySensor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeWorldInfo.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlParse.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlRenderContext.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlScene.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFBool.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFColor.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFFloat.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFImage.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFInt.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFNode.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFRotation.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFString.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFTime.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFVec2f.h
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlSFVec3f.h
# End Source File
# End Group
# Begin Group "Source File"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\antlr.g

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

# Begin Custom Build
InputPath=.\antlr.g

"temp.out" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\src\vrml97\Vrml97TokenTypes.txt vrml97tokentypes.txt 
	set classpath=c:\user\antlr-2.7.0 
	c:\user\jdk1.2.2\bin\java antlr.Tool ..\src\vrml97\vrml97parser.g 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# Begin Custom Build
InputPath=.\antlr.g

"temp.out" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\src\vrml97\Vrml97TokenTypes.txt  vrml97tokentypes.txt 
	set classpath = c:\user\antlr-2.7.0 
	c:\user\jdk1.2.2\bin\java antlr.Tool ..\src\vrml97\vrml97parser.g 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\Audio.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\Doc.cpp
# ADD CPP /I "..\..\win32"
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\doc2.cpp
# ADD CPP /I "..\..\win32"
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\dummysound.cpp
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\gifread.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\Image.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\jpgread.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\MathUtils.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\mpgread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\pngread.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\ScriptJS.cpp
# ADD CPP /I "..\..\win32" /D "XP_PC" /D js_InitArrayClass=JS_InitArrayClass
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\ScriptObject.cpp
# ADD CPP /I "..\..\win32" /D "XP_PC"
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\System.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\Viewer.cpp
# ADD CPP /I "..\..\win32"
# End Source File
# Begin Source File

SOURCE=.\Vrml97Parser.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

# ADD CPP /GR /I "..\src\vrml97"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "..\src\vrml97"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\vrml97scanner.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

# ADD CPP /I "..\..\win32"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlAABox.cpp
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlBSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlBVolume.cpp
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlField.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlFrustum.cpp
# ADD CPP /I "..\..\win32"
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNamespace.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNode.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeAnchor.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeAppearance.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeAudioClip.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeBackground.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeBillboard.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeBox.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCollision.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeColor.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeColorInt.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCone.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCoordinate.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCoordinateInt.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCylinder.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeCylinderSensor.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeDirLight.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeElevationGrid.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeExtrusion.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeFog.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeFontStyle.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeGeometry.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeGroup.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeIFaceSet.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeILineSet.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeImageTexture.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeIndexedSet.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeInline.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeLight.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeLOD.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeMaterial.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeMovieTexture.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeNavigationInfo.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeNormal.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeNormalInt.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeOrientationInt.cpp
# ADD CPP /I "..\..\win32"
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodePixelTexture.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodePlaneSensor.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodePointLight.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodePointSet.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodePositionInt.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeProto.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeProximitySensor.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeScalarInt.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeScript.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeShape.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeSound.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeSphere.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeSphereSensor.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeSpotLight.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeSwitch.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeText.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTextureCoordinate.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTextureTransform.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTimeSensor.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTouchSensor.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeTransform.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeType.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeViewpoint.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeVisibilitySensor.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlNodeWorldInfo.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlRenderContext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\vrml97\VrmlScene.cpp

!IF  "$(CFG)" == "libvrml97core - Win32 Release"

# ADD CPP /GR /I "..\..\win32" /I ".\\"

!ELSEIF  "$(CFG)" == "libvrml97core - Win32 Debug"

# ADD CPP /I "..\..\win32" /I ".\\"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
