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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# ifdef __FreeBSD__
#   include <floatingpoint.h>
# endif

# include <iostream>
# if HAVE_APPLE_OPENGL_FRAMEWORK
#   include <GLUT/glut.h>
# else
#   include <GL/glut.h>
# endif
# include <openvrml/browser.h>

# include "ViewerGlut.h"

extern "C" {
    typedef void (*GlutMenu)(int);
}

namespace {
    openvrml::browser * browser = 0;
    ViewerGlut * viewer = 0;

    bool setTitleUrl = true;

    void onExit();
    void worldChangedCB(openvrml::browser::cb_reason);
    void buildViewpointMenu();
}

int main(int argc, char * argv[]) {
    using std::cerr;
    using std::cout;
    using std::endl;
    using std::string;
    using std::vector;

    atexit(onExit);

#if defined(__FreeBSD__)
    fpsetmask(0);
# endif

    glutInitWindowSize(400, 320);
    glutInit(&argc, argv);

    string inputUrl;
    string inputName;
    const char * title = 0;

    const char * const usage = " file.wrl\n";

    for (int i = 1; i < argc; ++i) {
        if (*argv[i] == '-') {
            if (strcmp(argv[i], "-url") == 0) {
                inputUrl = argv[++i];
            } else if (strcmp(argv[i], "-notitle") == 0) {
                setTitleUrl = false;
            } else if (strcmp(argv[i], "-title") == 0) {
                setTitleUrl = false;
                title = argv[++i];
            } else {
                cerr << "Error: unrecognized option " << argv[i] << '\n';
                cerr << "Usage: " << argv[0] << usage << endl;
                exit(EXIT_FAILURE);
            }
        } else if (inputName.empty()) {
            inputName = argv[i];
        } else {
            cerr << "Usage: " << argv[0] << usage << endl;
            exit(EXIT_FAILURE);
        }
    }

    if (inputName.empty()) {
        if (inputUrl.empty()) {
            inputName = inputUrl;
        } else {
            inputName = inputUrl = "-"; // Read stdin
        }
    }

    if (inputUrl.empty()) { inputUrl = inputName; }

    browser = new openvrml::browser(cout, cerr);

    vector<string> uri(1, inputUrl);
    vector<string> parameter;
    browser->load_url(uri, parameter);

    viewer = new ViewerGlut(*::browser);
    if (!viewer) {
        cerr << "\nError: couldn't create GLUT viewer.\n";
        exit(EXIT_FAILURE);
    }

    if (title && *title) { glutSetWindowTitle(title); }

    browser->add_world_changed_callback(worldChangedCB);
    worldChangedCB(openvrml::browser::replace_world_id);
    viewer->update();

    glutMainLoop();
}

namespace {

    void onExit() {
        delete browser;
        delete viewer;
    }

    void worldChangedCB(const openvrml::browser::cb_reason reason) {
        switch (reason) {
        case openvrml::browser::destroy_world_id:
            exit(0);
            break;

        case openvrml::browser::replace_world_id:
            if (setTitleUrl) {
                glutSetWindowTitle(browser->world_url().c_str());
            }
            buildViewpointMenu();
            break;
        }
    }

    void lookatViewpointMenu(int item) {
        typedef std::list<openvrml::viewpoint_node *> viewpoint_list_t;
        const viewpoint_list_t & viewpoints = browser->viewpoints();
        viewpoint_list_t::const_iterator viewpoint = viewpoints.begin();
        while(std::distance(viewpoints.begin(), viewpoint) != item
                    && viewpoint != viewpoints.end()) {
            ++viewpoint;
        }
        (*viewpoint)->process_event("set_bind", openvrml::sfbool(true),
                                    openvrml::browser::current_time());
    }

    void buildViewpointMenu() {
        static int topmenu = 0;
        static int vpmenu = 0;
        static size_t nvp = 0;
        size_t numberOfViewpoints = 0;

        if (vpmenu) {
            glutSetMenu(vpmenu);
            for (int i = int(nvp); i > 0; --i) { glutRemoveMenuItem(i); }
        } else {
            topmenu = glutCreateMenu(0);
            vpmenu = glutCreateMenu(reinterpret_cast<GlutMenu>
                                    (lookatViewpointMenu));
            glutSetMenu(topmenu);

            glutAddSubMenu("Viewpoints", vpmenu);
            glutAttachMenu(GLUT_RIGHT_BUTTON);
        }

        glutSetMenu(vpmenu);
        //glutAddMenuEntry( "Reset", 0 );

        numberOfViewpoints = browser->viewpoints().size();
        nvp = numberOfViewpoints;

        typedef std::list<openvrml::viewpoint_node *> viewpoint_list_t;
        const viewpoint_list_t & viewpoints = browser->viewpoints();
        for (viewpoint_list_t::const_iterator viewpoint(viewpoints.begin());
                viewpoint != viewpoints.end(); ++viewpoint) {
            const std::string & description = (*viewpoint)->description();
            if (!description.empty()) {
                glutAddMenuEntry(description.c_str(),
                                 std::distance(viewpoints.begin(), viewpoint));
            }
        }

        //glutAttachMenuName(GLUT_RIGHT_BUTTON, "Viewpoints");
    }
}
