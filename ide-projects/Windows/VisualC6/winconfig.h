#ifndef WINCONFIG_H
#define WINCONFIG_H
/*
  OpenVrml library
  Copyright (C) 1998 Chris Morley
  See the file COPYING for license details.
  winconfig.h
*/

#define OPENVRML_MAJOR_VERSION	0
#define OPENVRML_MINOR_VERSION	11
#define OPENVRML_MICRO_VERSION	0



// Disable warning messages about truncation from const double to float
# pragma warning( disable : 4305 )  
// Disable warning messages about truncating names in debug info
# pragma warning( disable : 4786 )  
static const char SLASH = '/';


static const char COLON = ':';


#define HAVE_LIBJPEG
#define HAVE_LIBPNG
#define OPENVRML_HAVE_JS
/* #define OPENVRML_HAVE_JAVA */

/* #undef HAVE_LIBJPEG */
/* #undef HAVE_LIBPNG  */
/* #undef OPENVRML_HAVE_JS  */ 
#undef OPENVRML_HAVE_JAVA   



#endif	/* WINCONFIG_H */
