SUBDIRS = field node

JAVAROOT = $(top_builddir)/java

JAVA_FILES = \
        Browser.java \
        Event.java \
        Field.java \
        MField.java \
        ConstField.java \
        ConstMField.java \
        BaseNode.java \
        InvalidEventInException.java \
        InvalidEventOutException.java \
        InvalidExposedFieldException.java \
        InvalidFieldChangeException.java \
        InvalidFieldException.java \
        InvalidRouteException.java \
        InvalidVRMLSyntaxException.java

if ENABLE_SCRIPT_NODE_JAVA
javadir = $(datadir)/java/vrml
noinst_JAVA = $(JAVA_FILES)
endif

EXTRA_DIST = $(JAVA_FILES)
