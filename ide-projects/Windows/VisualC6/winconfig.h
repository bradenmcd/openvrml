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

/* Make sure that PI and friends are defined.  This is needed under 
   platforms that are not directly BSD derived (even under GNU libc
   this is not included by default).
*/
#ifndef M_PI
#define M_PI            3.14159265358979323846  /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2          1.57079632679489661923  /* pi/2 */
#endif

#ifndef M_PI_4
#define M_PI_4          0.78539816339744830962  /* pi/4 */
#endif

#ifndef M_1_PI
#define M_1_PI          0.31830988618379067154  /* 1/pi */
#endif


#define HAVE_LIBJPEG
#define HAVE_LIBPNG
#define HAVE_MOZJS 
/* #define OPENVRML_HAVE_JAVA */

/* #undef HAVE_LIBJPEG */
/* #undef HAVE_LIBPNG  */
/* #undef HAVE_MOZJS   */ 
#undef OPENVRML_HAVE_JAVA   



#endif	/* WINCONFIG_H */
