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

#if defined(__FreeBSD__)
#include <floatingpoint.h>
#endif

#include <stdio.h>
#include <GL/glut.h>
#include <vrml97/doc2.hpp>
#include <vrml97/System.h>
#include <vrml97/VrmlScene.h>

#include "ViewerGlut.h"

static void worldChangedCB(int);
static void buildViewpointMenu();

VrmlScene *vrmlScene = 0;
ViewerGlut   *viewer = 0;

static bool setTitleUrl = true;


int
main(int argc, char **argv)
{
#if defined(__FreeBSD__)
  fpsetmask(0);
#endif;  
  
  glutInitWindowSize(400, 320);
  glutInit( &argc, argv);

  char *inputUrl = 0;
  char *inputName = 0;
  char *outputName = 0;
  char *title = 0;

  char usage[] = " file.wrl [outputfile]\n";

  for (int i = 1; i < argc; ++i)
    {
      if (*argv[i] == '-')
	{
	  if (strcmp(argv[i], "-url") == 0)
	    inputUrl = argv[++i];
	  else if (strcmp(argv[i], "-notitle") == 0)
	    setTitleUrl = false;
	  else if (strcmp(argv[i], "-title") == 0)
	    {
	      setTitleUrl = false;
	      title = argv[++i];
	    }
	  else
	    {
	      cerr << "Error: unrecognized option " << argv[i] << '\n';
	      cerr << "Usage: " << argv[0] << usage << endl;
	      exit(1);
	    }
	}
      else if (! inputName)
	inputName = argv[i];
      else if (! outputName)
	outputName = argv[i];
      else
	{
	  cerr << "Usage: " << argv[0] << usage << endl;
	  exit(1);
	}
    }

  if (! inputName)
    {
      if (inputUrl)
	inputName = inputUrl;
      else
	inputName = inputUrl = "-";		// Read stdin
    }
  else
    cout << "Loading " << inputName << " ...";

  if (! inputUrl) inputUrl = inputName;

  vrmlScene = new VrmlScene( inputUrl, inputName );

  if (outputName)
    {
      cout << "  Saving scene to " << outputName << endl;
      if (! vrmlScene->save(outputName))
	cout << "\nError: couldn't write to " << outputName << endl;
    }

  viewer = new ViewerGlut( vrmlScene );
  if (! viewer)
    {
      cout << "\nError: couldn't create GLUT viewer.\n";
      exit(1);
    }

  if (*inputName != '-')
    cout << " done.\n";

  if (title && *title) glutSetWindowTitle(title);

  vrmlScene->addWorldChangedCallback( worldChangedCB );
  worldChangedCB( VrmlScene::REPLACE_WORLD );
  viewer->update();

  glutMainLoop();
  return 0;
}


static void worldChangedCB(int reason)
{
  switch (reason)
    {
    case VrmlScene::DESTROY_WORLD:
      delete viewer;
      delete vrmlScene;
      exit(0);
      break;

    case VrmlScene::REPLACE_WORLD:
      if ( setTitleUrl ) {
        Doc2 * urlDoc = vrmlScene->urlDoc();
        const char * title = urlDoc ? urlDoc->urlBase() : 0;
        if (title && *title) glutSetWindowTitle(title);
      }
      buildViewpointMenu();
      break;
    }
}


static void lookatViewpointMenu(int item) {
  if ( item == 0 )
    viewer->resetUserNavigation(); // ...
  else
    vrmlScene->setViewpoint(item-1);
}


static void buildViewpointMenu() {

  static int  topmenu     = 0;
  static int  vpmenu     = 0;
  static int  nvp     = 0;
  int numberOfViewpoints = 0;
  
  if ( vpmenu )
    {
      glutSetMenu( vpmenu );
      for (int i=nvp; i>0; --i)
	glutRemoveMenuItem( i );
    }
  else
    {
      topmenu = glutCreateMenu(0);
      vpmenu = glutCreateMenu(lookatViewpointMenu);
      glutSetMenu(topmenu);

      glutAddSubMenu("Viewpoints", vpmenu);
      glutAttachMenu(GLUT_RIGHT_BUTTON);
    }

  glutSetMenu(vpmenu);
  //glutAddMenuEntry( "Reset", 0 );
  
  numberOfViewpoints = vrmlScene->nViewpoints();
  nvp = numberOfViewpoints;

  if (numberOfViewpoints > 0 )
    {
      for (int i = 0; i < numberOfViewpoints; i++) {
	const char *name, *description;
	vrmlScene->getViewpoint(i, &name, &description);
	if ( description && *description )
	  glutAddMenuEntry(description, i+1);
	else if ( name && *name )
	  glutAddMenuEntry(name, i+1);
	else
	  {
	    char buf[25];
	    sprintf(buf,"Viewpoint %d", i+1);
	    glutAddMenuEntry(buf, i+1);
	  }
      }
    }

  //glutAttachMenuName(GLUT_RIGHT_BUTTON, "Viewpoints");

}
