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
//  Program to exercise the VrmlScene and ViewerGlut classes.
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# ifdef __FreeBSD__
#   include <floatingpoint.h>
# endif

# ifdef macintosh
#   include "console.h"
#   include "unix.h"
#   include "SIOUX.h"
# endif

# include <stdio.h>
# include OPENVRML_GLUT_H
# include <OpenVRML/doc2.hpp>
# include <OpenVRML/VrmlScene.h>

# include "ViewerGlut.h"

using OpenVRML::VrmlScene;
using OpenVRML::Doc2;

extern "C" {
    typedef void (*GlutMenu)(int);
}

namespace {
    VrmlScene * vrmlScene = 0;
    ViewerGlut * viewer = 0;
    
    bool setTitleUrl = true;
    
    void onExit();
    void worldChangedCB(VrmlScene::CBReason);
    void buildViewpointMenu();
}

int main(int argc, char * argv[]) {
    using std::cerr;
    using std::cout;
    using std::endl;
    
    atexit(onExit);
    
#if defined(__FreeBSD__)
    fpsetmask(0);
# endif
 
# ifdef macintosh
    SIOUXSettings.asktosaveonclose = 0;
    argc = ccommand(&argv);
    _fcreator = 'ttxt';
    _ftype = 'TEXT';
# endif

    glutInitWindowSize(400, 320);
    glutInit(&argc, argv);

    const char * inputUrl = 0;
    const char * inputName = 0;
    const char * outputName = 0;
    const char * title = 0;

    const char * const usage = " file.wrl [outputfile]\n";

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
        } else if (!inputName) {
            inputName = argv[i];
        } else if (!outputName) {
            outputName = argv[i];
        } else {
            cerr << "Usage: " << argv[0] << usage << endl;
            exit(EXIT_FAILURE);
        }
    }

    if (!inputName) {
        if (inputUrl) {
            inputName = inputUrl;
        } else {
            inputName = inputUrl = "-"; // Read stdin
        }
    }

    if (!inputUrl) { inputUrl = inputName; }

    vrmlScene = new VrmlScene(inputUrl);

    if (outputName) {
        cout << "  Saving scene to " << outputName << endl;
        if (! vrmlScene->save(outputName)) {
            cout << "\nError: couldn't write to " << outputName << endl;
        }
    }

    viewer = new ViewerGlut(*vrmlScene);
    if (!viewer) {
        cerr << "\nError: couldn't create GLUT viewer.\n";
        exit(EXIT_FAILURE);
    }

    if (title && *title) { glutSetWindowTitle(title); }

    vrmlScene->addWorldChangedCallback( worldChangedCB );
    worldChangedCB( VrmlScene::REPLACE_WORLD );
    viewer->update();

    glutMainLoop();
}

namespace {

    void onExit() {
        delete vrmlScene;
        delete viewer;
    }

    void worldChangedCB(const VrmlScene::CBReason reason) {
        switch (reason) {
        case VrmlScene::DESTROY_WORLD:
            exit(0);
            break;

        case VrmlScene::REPLACE_WORLD:
            if (setTitleUrl) {
                Doc2 * urlDoc = vrmlScene->urlDoc();
                const char * title = urlDoc ? urlDoc->urlBase() : 0;
                if (title && *title) glutSetWindowTitle(title);
            }
            buildViewpointMenu();
            break;
        }
    }

    void lookatViewpointMenu(int item) {
        if (item == 0) {
            viewer->resetUserNavigation(); // ...
        } else {
            vrmlScene->setViewpoint(item - 1);
        }
    }

    void buildViewpointMenu() {
        static int topmenu = 0;
        static int vpmenu = 0;
        static int nvp = 0;
        int numberOfViewpoints = 0;

        if (vpmenu) {
            glutSetMenu(vpmenu);
            for (int i = nvp; i > 0; --i) { glutRemoveMenuItem(i); }
        } else {
            topmenu = glutCreateMenu(0);
            vpmenu = glutCreateMenu(reinterpret_cast<GlutMenu>(lookatViewpointMenu));
            glutSetMenu(topmenu);

            glutAddSubMenu("Viewpoints", vpmenu);
            glutAttachMenu(GLUT_RIGHT_BUTTON);
        }

        glutSetMenu(vpmenu);
        //glutAddMenuEntry( "Reset", 0 );

        numberOfViewpoints = vrmlScene->nViewpoints();
        nvp = numberOfViewpoints;

        if (numberOfViewpoints > 0) {
            for (int i = 0; i < numberOfViewpoints; i++) {
                std::string name, description;
                vrmlScene->getViewpoint(i, name, description);
                if (description.length() > 0) {
                    glutAddMenuEntry(description.c_str(), i + 1);
                } else if (name.length() > 0) {
                    glutAddMenuEntry(name.c_str(), i + 1);
                } else {
                    char buf[25];
                    sprintf(buf,"Viewpoint %d", i + 1);
                    glutAddMenuEntry(buf, i + 1);
                }
            }
        }

        //glutAttachMenuName(GLUT_RIGHT_BUTTON, "Viewpoints");
    }
}
