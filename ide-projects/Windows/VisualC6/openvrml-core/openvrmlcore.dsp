# Microsoft Developer Studio Project File - Name="openvrmlcore" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=openvrmlcore - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "openvrmlcore.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "openvrmlcore.mak" CFG="openvrmlcore - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "openvrmlcore - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "openvrmlcore - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

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

# Name "openvrmlcore - Win32 Release"
# Name "openvrmlcore - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\OpenVRML\Audio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Doc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\doc2.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\gifread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Image.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\jpgread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\MathUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\mpgread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\pngread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\ScriptJDK.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\ScriptJS.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\ScriptObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\sound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\System.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Viewer.h
# End Source File
# Begin Source File

SOURCE=.\Vrml97Parser.hpp
# End Source File
# Begin Source File

SOURCE=.\Vrml97ParserTokenTypes.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\vrml97scanner.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlField.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlFrustum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlMFColor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlMFFloat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlMFInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlMFRotation.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlMFString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlMFVec2f.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlMFVec3f.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNamespace.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeAnchor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeAppearance.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeAudioClip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeBackground.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeBillboard.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeBindable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeChild.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCollision.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeColor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeColorInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCone.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCoordinate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCoordinateInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCylinder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCylinderSensor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeDirLight.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeElevationGrid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeExtrusion.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeFog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeFontStyle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeGeometry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeGroup.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeIFaceSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeILineSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeImageTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeIndexedSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeInline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeLight.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeLOD.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeMaterial.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeMovieTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeNavigationInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeNormal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeNormalInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeOrientationInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodePixelTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodePlaneSensor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodePointLight.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodePointSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodePositionInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeProto.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeProximitySensor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeScalarInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeScript.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeShape.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeSound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeSphere.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeSphereSensor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeSpotLight.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeSwitch.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeText.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTextureCoordinate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTextureTransform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTimeSensor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTouchSensor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTransform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeViewpoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeVisibilitySensor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeWorldInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlParse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlRenderContext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlScene.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFBool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFColor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFFloat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFNode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFRotation.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFTime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFVec2f.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlSFVec3f.h
# End Source File
# End Group
# Begin Group "Source File"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\antlr.g

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

# Begin Custom Build
InputPath=.\antlr.g

"temp.out" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\src\openvrml\OpenVRML\Vrml97TokenTypes.txt vrml97tokentypes.txt 
	set classpath=c:\user\antlr-2.7.0 
	c:\user\jdk1.2.2\bin\java antlr.Tool ..\..\..\..\src\openvrml\OpenVRML\vrml97parser.g 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# Begin Custom Build
InputPath=.\antlr.g

"temp.out" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\..\..\..\src\openvrml\OpenVRML\Vrml97TokenTypes.txt  vrml97tokentypes.txt 
	set classpath = c:\user\antlr-2.7.0 
	c:\user\jdk1.2.2\bin\java antlr.Tool ..\..\..\..\src\openvrml\OpenVRML\vrml97parser.g 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Audio.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Doc.cpp
# ADD CPP /I "..\\"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\doc2.cpp
# ADD CPP /I "..\\"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\dummysound.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\gifread.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Image.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\jpgread.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\MathUtils.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\mpgread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\pngread.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\ScriptJS.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

# ADD CPP /I "..\..\..\..\lib" /D "XP_PC" /D js_InitArrayClass=JS_InitArrayClass

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "..\\" /I "..\..\..\..\lib" /D "XP_PC" /D js_InitArrayClass=JS_InitArrayClass

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\ScriptObject.cpp
# ADD CPP /I "..\\" /D "XP_PC"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\System.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\Viewer.cpp
# ADD CPP /I "..\\"
# End Source File
# Begin Source File

SOURCE=.\Vrml97Parser.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

# ADD CPP /GR /I "..\..\..\..\lib" /I "..\..\..\..\src\openvrml\OpenVRML"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "..\..\..\..\lib" /I "..\..\..\..\src\openvrml\OpenVRML"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\vrml97scanner.cpp
# ADD CPP /I "..\..\..\..\lib"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlAABox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlBSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlBVolume.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlField.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlFrustum.cpp
# ADD CPP /I "..\\"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNamespace.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNode.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeAnchor.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeAppearance.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeAudioClip.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeBackground.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeBillboard.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeBox.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCollision.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeColor.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeColorInt.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCone.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCoordinate.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCoordinateInt.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCylinder.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeCylinderSensor.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeDirLight.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeElevationGrid.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeExtrusion.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeFog.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeFontStyle.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeGeometry.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeGroup.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeIFaceSet.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeILineSet.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeImageTexture.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeIndexedSet.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeInline.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeLight.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeLOD.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeMaterial.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeMovieTexture.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeNavigationInfo.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeNormal.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeNormalInt.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeOrientationInt.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "..\\"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodePixelTexture.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodePlaneSensor.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodePointLight.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodePointSet.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodePositionInt.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeProto.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeProximitySensor.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeScalarInt.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeScript.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeShape.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeSound.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeSphere.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeSphereSensor.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeSpotLight.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeSwitch.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeText.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTextureCoordinate.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTextureTransform.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTimeSensor.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTouchSensor.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeTransform.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeType.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeViewpoint.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeVisibilitySensor.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlNodeWorldInfo.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlRenderContext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\openvrml\OpenVRML\VrmlScene.cpp

!IF  "$(CFG)" == "openvrmlcore - Win32 Release"

# ADD CPP /GR /I "..\..\..\..\lib"

!ELSEIF  "$(CFG)" == "openvrmlcore - Win32 Debug"

# ADD CPP /I "..\\" /I "..\..\..\..\lib"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
