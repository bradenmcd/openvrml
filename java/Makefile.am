SUBDIRS = vrml

if ENABLE_SCRIPT_NODE_JAVA

javapkgdatadir = $(pkgdatadir)/java
javapkgdata_DATA = script.jar

CLASSES = \
        vrml/InvalidEventInException.class \
        vrml/InvalidEventOutException.class \
        vrml/InvalidExposedFieldException.class \
        vrml/InvalidFieldChangeException.class \
        vrml/InvalidFieldException.class \
        vrml/InvalidRouteException.class \
        vrml/InvalidVRMLSyntaxException.class \
        vrml/Browser.class \
        vrml/Event.class \
        vrml/Field.class \
        vrml/ConstMField.class \
        vrml/MField.class \
        vrml/ConstField.class \
        vrml/BaseNode.class \
        vrml/field/SFBool.class \
        vrml/field/SFColor.class \
        vrml/field/SFFloat.class \
        vrml/field/SFImage.class \
        vrml/field/SFInt32.class \
        vrml/field/SFNode.class \
        vrml/field/SFRotation.class \
        vrml/field/SFString.class \
        vrml/field/SFTime.class \
        vrml/field/SFVec2f.class \
        vrml/field/SFVec3f.class \
        vrml/field/MFBool.class \
        vrml/field/MFColor.class \
        vrml/field/MFFloat.class \
        vrml/field/MFInt32.class \
        vrml/field/MFNode.class \
        vrml/field/MFRotation.class \
        vrml/field/MFString.class \
        vrml/field/MFTime.class \
        vrml/field/MFVec2f.class \
        vrml/field/MFVec3f.class \
        vrml/field/ConstSFBool.class \
        vrml/field/ConstSFColor.class \
        vrml/field/ConstSFFloat.class \
        vrml/field/ConstSFImage.class \
        vrml/field/ConstSFInt32.class \
        vrml/field/ConstSFNode.class \
        vrml/field/ConstSFRotation.class \
        vrml/field/ConstSFString.class \
        vrml/field/ConstSFTime.class \
        vrml/field/ConstSFVec2f.class \
        vrml/field/ConstSFVec3f.class \
        vrml/field/ConstMFBool.class \
        vrml/field/ConstMFColor.class \
        vrml/field/ConstMFFloat.class \
        vrml/field/ConstMFInt32.class \
        vrml/field/ConstMFNode.class \
        vrml/field/ConstMFRotation.class \
        vrml/field/ConstMFString.class \
        vrml/field/ConstMFTime.class \
        vrml/field/ConstMFVec2f.class \
        vrml/field/ConstMFVec3f.class \
        vrml/field/SFVec2d.class \
        vrml/field/MFVec2f.class \
        vrml/field/ConstSFVec2d.class \
        vrml/field/ConstMFVec2d.class \
        vrml/field/SFDouble.class \
        vrml/field/MFDouble.class \
        vrml/field/ConstSFDouble.class \
        vrml/field/ConstMFDouble.class \
        vrml/field/SFVec3d.class \
        vrml/field/MFVec3f.class \
        vrml/field/ConstSFVec3d.class \
        vrml/field/ConstMFVec3d.class \
        vrml/node/Node.class \
        vrml/node/Script.class

script.jar: $(CLASSES)
	$(JAR) cvf $@ $^

JNI_HFILES = \
        vrml_Browser.h \
        vrml_Event.h \
        vrml_Field.h \
        vrml_MField.h \
        vrml_ConstField.h \
        vrml_ConstMField.h \
        vrml_BaseNode.h \
        vrml_field_SFBool.h \
        vrml_field_SFColor.h \
        vrml_field_SFFloat.h \
        vrml_field_SFDouble.h \
        vrml_field_SFImage.h \
        vrml_field_SFInt32.h \
        vrml_field_SFNode.h \
        vrml_field_SFRotation.h \
        vrml_field_SFString.h \
        vrml_field_SFTime.h \
        vrml_field_SFVec2f.h \
        vrml_field_SFVec2d.h \
        vrml_field_SFVec3f.h \
        vrml_field_SFVec3d.h \
        vrml_field_MFBool.h \
        vrml_field_MFColor.h \
        vrml_field_MFFloat.h \
        vrml_field_MFDouble.h \
        vrml_field_MFInt32.h \
        vrml_field_MFNode.h \
        vrml_field_MFRotation.h \
        vrml_field_MFString.h \
        vrml_field_MFTime.h \
        vrml_field_MFVec2f.h \
        vrml_field_MFVec2d.h \
        vrml_field_MFVec3f.h \
        vrml_field_MFVec3d.h \
        vrml_field_ConstSFBool.h \
        vrml_field_ConstSFColor.h \
        vrml_field_ConstSFFloat.h \
        vrml_field_ConstSFDouble.h \
        vrml_field_ConstSFImage.h \
        vrml_field_ConstSFInt32.h \
        vrml_field_ConstSFNode.h \
        vrml_field_ConstSFRotation.h \
        vrml_field_ConstSFString.h \
        vrml_field_ConstSFTime.h \
        vrml_field_ConstSFVec2f.h \
        vrml_field_ConstSFVec2d.h \
        vrml_field_ConstSFVec3f.h \
        vrml_field_ConstSFVec3d.h \
        vrml_field_ConstMFBool.h \
        vrml_field_ConstMFColor.h \
        vrml_field_ConstMFFloat.h \
        vrml_field_ConstMFDouble.h \
        vrml_field_ConstMFInt32.h \
        vrml_field_ConstMFNode.h \
        vrml_field_ConstMFRotation.h \
        vrml_field_ConstMFString.h \
        vrml_field_ConstMFTime.h \
        vrml_field_ConstMFVec2f.h \
        vrml_field_ConstMFVec2d.h \
        vrml_field_ConstMFVec3f.h \
        vrml_field_ConstMFVec3d.h \
        vrml_node_Node.h \
        vrml_node_Script.h

nodist_noinst_HEADERS = $(JNI_HFILES)
CLEANFILES = script.jar $(JNI_HFILES)

vrml_Browser.h: vrml/Browser.class
	$(JAVAH) -classpath . vrml.Browser

vrml_Event.h: vrml/Event.class
	$(JAVAH) -classpath . vrml.Event

vrml_Field.h: vrml/Field.class
	$(JAVAH) -classpath . vrml.Field

vrml_MField.h: vrml/MField.class
	$(JAVAH) -classpath . vrml.MField

vrml_ConstField.h: vrml/ConstField.class
	$(JAVAH) -classpath . vrml.ConstField

vrml_ConstMField.h: vrml/ConstMField.class
	$(JAVAH) -classpath . vrml.ConstMField

vrml_BaseNode.h: vrml/BaseNode.class
	$(JAVAH) -classpath . vrml.BaseNode

vrml_field_SFBool.h: vrml/field/SFBool.class
	$(JAVAH) -classpath . vrml.field.SFBool

vrml_field_SFColor.h: vrml/field/SFColor.class
	$(JAVAH) -classpath . vrml.field.SFColor

vrml_field_SFFloat.h: vrml/field/SFFloat.class
	$(JAVAH) -classpath . vrml.field.SFFloat

vrml_field_SFDouble.h: vrml/field/SFDouble.class
	$(JAVAH) -classpath . vrml.field.SFDouble

vrml_field_SFImage.h: vrml/field/SFImage.class
	$(JAVAH) -classpath . vrml.field.SFImage

vrml_field_SFInt32.h: vrml/field/SFInt32.class
	$(JAVAH) -classpath . vrml.field.SFInt32

vrml_field_SFNode.h: vrml/field/SFNode.class
	$(JAVAH) -classpath . vrml.field.SFNode

vrml_field_SFRotation.h: vrml/field/SFRotation.class
	$(JAVAH) -classpath . vrml.field.SFRotation

vrml_field_SFString.h: vrml/field/SFString.class
	$(JAVAH) -classpath . vrml.field.SFString

vrml_field_SFTime.h: vrml/field/SFTime.class
	$(JAVAH) -classpath . vrml.field.SFTime

vrml_field_SFVec2f.h: vrml/field/SFVec2f.class
	$(JAVAH) -classpath . vrml.field.SFVec2f

vrml_field_SFVec2d.h: vrml/field/SFVec2d.class
	$(JAVAH) -classpath . vrml.field.SFVec2d

vrml_field_SFVec3f.h: vrml/field/SFVec3f.class
	$(JAVAH) -classpath . vrml.field.SFVec3f

vrml_field_SFVec3d.h: vrml/field/SFVec3d.class
	$(JAVAH) -classpath . vrml.field.SFVec3d

vrml_field_MFBool.h: vrml/field/MFBool.class
	$(JAVAH) -classpath . vrml.field.MFBool

vrml_field_MFColor.h: vrml/field/MFColor.class
	$(JAVAH) -classpath . vrml.field.MFColor

vrml_field_MFFloat.h: vrml/field/MFFloat.class
	$(JAVAH) -classpath . vrml.field.MFFloat

vrml_field_MFDouble.h: vrml/field/MFDouble.class
	$(JAVAH) -classpath . vrml.field.MFDouble

vrml_field_MFInt32.h: vrml/field/MFInt32.class
	$(JAVAH) -classpath . vrml.field.MFInt32

vrml_field_MFNode.h: vrml/field/MFNode.class
	$(JAVAH) -classpath . vrml.field.MFNode

vrml_field_MFRotation.h: vrml/field/MFRotation.class
	$(JAVAH) -classpath . vrml.field.MFRotation

vrml_field_MFString.h: vrml/field/MFString.class
	$(JAVAH) -classpath . vrml.field.MFString

vrml_field_MFTime.h: vrml/field/MFTime.class
	$(JAVAH) -classpath . vrml.field.MFTime

vrml_field_MFVec2f.h: vrml/field/MFVec2f.class
	$(JAVAH) -classpath . vrml.field.MFVec2f

vrml_field_MFVec2d.h: vrml/field/MFVec2d.class
	$(JAVAH) -classpath . vrml.field.MFVec2d

vrml_field_MFVec3f.h: vrml/field/MFVec3f.class
	$(JAVAH) -classpath . vrml.field.MFVec3f

vrml_field_MFVec3d.h: vrml/field/MFVec3d.class
	$(JAVAH) -classpath . vrml.field.MFVec3d

vrml_field_ConstSFBool.h: vrml/field/ConstSFBool.class
	$(JAVAH) -classpath . vrml.field.ConstSFBool

vrml_field_ConstSFColor.h: vrml/field/ConstSFColor.class
	$(JAVAH) -classpath . vrml.field.ConstSFColor

vrml_field_ConstSFFloat.h: vrml/field/ConstSFFloat.class
	$(JAVAH) -classpath . vrml.field.ConstSFFloat

vrml_field_ConstSFDouble.h: vrml/field/ConstSFDouble.class
	$(JAVAH) -classpath . vrml.field.ConstSFDouble

vrml_field_ConstSFImage.h: vrml/field/ConstSFImage.class
	$(JAVAH) -classpath . vrml.field.ConstSFImage

vrml_field_ConstSFInt32.h: vrml/field/ConstSFInt32.class
	$(JAVAH) -classpath . vrml.field.ConstSFInt32

vrml_field_ConstSFNode.h: vrml/field/ConstSFNode.class
	$(JAVAH) -classpath . vrml.field.ConstSFNode

vrml_field_ConstSFRotation.h: vrml/field/ConstSFRotation.class
	$(JAVAH) -classpath . vrml.field.ConstSFRotation

vrml_field_ConstSFString.h: vrml/field/ConstSFString.class
	$(JAVAH) -classpath . vrml.field.ConstSFString

vrml_field_ConstSFTime.h: vrml/field/ConstSFTime.class
	$(JAVAH) -classpath . vrml.field.ConstSFTime

vrml_field_ConstSFVec2f.h: vrml/field/ConstSFVec2f.class
	$(JAVAH) -classpath . vrml.field.ConstSFVec2f

vrml_field_ConstSFVec2d.h: vrml/field/ConstSFVec2d.class
	$(JAVAH) -classpath . vrml.field.ConstSFVec2d

vrml_field_ConstSFVec3f.h: vrml/field/ConstSFVec3f.class
	$(JAVAH) -classpath . vrml.field.ConstSFVec3f

vrml_field_ConstSFVec3d.h: vrml/field/ConstSFVec3d.class
	$(JAVAH) -classpath . vrml.field.ConstSFVec3d

vrml_field_ConstMFBool.h: vrml/field/ConstMFBool.class
	$(JAVAH) -classpath . vrml.field.ConstMFBool

vrml_field_ConstMFColor.h: vrml/field/ConstMFColor.class
	$(JAVAH) -classpath . vrml.field.ConstMFColor

vrml_field_ConstMFFloat.h: vrml/field/ConstMFFloat.class
	$(JAVAH) -classpath . vrml.field.ConstMFFloat

vrml_field_ConstMFDouble.h: vrml/field/ConstMFDouble.class
	$(JAVAH) -classpath . vrml.field.ConstMFDouble

vrml_field_ConstMFInt32.h: vrml/field/ConstMFInt32.class
	$(JAVAH) -classpath . vrml.field.ConstMFInt32

vrml_field_ConstMFNode.h: vrml/field/ConstMFNode.class
	$(JAVAH) -classpath . vrml.field.ConstMFNode

vrml_field_ConstMFRotation.h: vrml/field/ConstMFRotation.class
	$(JAVAH) -classpath . vrml.field.ConstMFRotation

vrml_field_ConstMFString.h: vrml/field/ConstMFString.class
	$(JAVAH) -classpath . vrml.field.ConstMFString

vrml_field_ConstMFTime.h: vrml/field/ConstMFTime.class
	$(JAVAH) -classpath . vrml.field.ConstMFTime

vrml_field_ConstMFVec2f.h: vrml/field/ConstMFVec2f.class
	$(JAVAH) -classpath . vrml.field.ConstMFVec2f

vrml_field_ConstMFVec2d.h: vrml/field/ConstMFVec2d.class
	$(JAVAH) -classpath . vrml.field.ConstMFVec2d

vrml_field_ConstMFVec3f.h: vrml/field/ConstMFVec3f.class
	$(JAVAH) -classpath . vrml.field.ConstMFVec3f

vrml_field_ConstMFVec3d.h: vrml/field/ConstMFVec3d.class
	$(JAVAH) -classpath . vrml.field.ConstMFVec3d

vrml_node_Node.h: vrml/node/Node.class
	$(JAVAH) -classpath . vrml.node.Node

vrml_node_Script.h: vrml/node/Script.class
	$(JAVAH) -classpath . vrml.node.Script

endif
