AUTOMAKE_OPTIONS = 1.5 foreign

SUBDIRS = doc macros models lib @JAVA_SUBDIR@ src

ACLOCAL_AMFLAGS = -I macros

IDE_PROJECT_FILES = \
        ide-projects/Macintosh/CodeWarrior5/JavaScript.mcp \
        ide-projects/Macintosh/CodeWarrior5/JavaScript.mcp.xml \
        ide-projects/Macintosh/CodeWarrior5/JavaScript.prefix \
        ide-projects/Macintosh/CodeWarrior5/LibANTLR.mcp \
        ide-projects/Macintosh/CodeWarrior5/LibANTLR.mcp.xml \
        ide-projects/Macintosh/CodeWarrior5/LibANTLR.prefix \
        ide-projects/Macintosh/CodeWarrior5/Lookat.mcp \
        ide-projects/Macintosh/CodeWarrior5/Lookat.mcp.xml \
        ide-projects/Macintosh/CodeWarrior5/OpenVRMLcore.mcp \
        ide-projects/Macintosh/CodeWarrior5/OpenVRMLcore.mcp.xml \
        ide-projects/Macintosh/CodeWarrior5/OpenVRMLgl.mcp \
        ide-projects/Macintosh/CodeWarrior5/OpenVRMLgl.mcp.xml \
        ide-projects/Macintosh/CodeWarrior5/OpenVRML.mcp \
        ide-projects/Macintosh/CodeWarrior5/OpenVRML.mcp.xml \
        ide-projects/Macintosh/CodeWarrior5/OpenVRML.prefix \
        ide-projects/Macintosh/CodeWarrior5/OpenVRML.rsrc.hqx \
        ide-projects/Macintosh/CodeWarrior5/sys/types.h \
        ide-projects/Windows/VisualC6/Install \
        ide-projects/Windows/VisualC6/Makefile.vc6 \
        ide-projects/Windows/VisualC6/openvrml.dsp \
        ide-projects/Windows/VisualC6/openvrml.dsw \
        ide-projects/Windows/VisualC6/winconfig.h \
        ide-projects/Windows/VisualC6/lookat/lookat.dsp \
        ide-projects/Windows/VisualC6/lookat/lookat.dsw \
        ide-projects/Windows/VisualC6/lookat/Makefile.vc6 \
        ide-projects/Windows/VisualC6/openvrml-core/openvrmlcore.dsp \
        ide-projects/Windows/VisualC6/openvrml-core/openvrmlcore.dsw \
        ide-projects/Windows/VisualC6/openvrml-core/Makefile.vc6 \
        ide-projects/Windows/VisualC6/openvrml-gl/openvrmlgl.dsp \
        ide-projects/Windows/VisualC6/openvrml-gl/openvrmlgl.dsw \
        ide-projects/Windows/VisualC6/openvrml-gl/Makefile.vc6 \
        ide-projects/Windows/VisualC6/tplib/antlr/libantlr.dsp \
        ide-projects/Windows/VisualC6/tplib/antlr/libantlr.dsw \
        ide-projects/Windows/VisualC6/tplib/antlr/Makefile.vc6

EXTRA_DIST = Doxyfile.in openvrml.pc.in openvrml-gl.pc.in openvrml.spec.in \
lookat.xpm $(IDE_PROJECT_FILES) java

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = openvrml.pc @OPENVRML_GL_PC@

pixmapdatadir = $(datadir)/pixmaps
pixmapdata_DATA = lookat.xpm

dist-hook: openvrml.spec
	cp openvrml.spec $(distdir)
