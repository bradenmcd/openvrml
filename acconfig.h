#ifndef ACCONFIG_H
#define ACCONFIG_H
/*
  Vrml 97 library
  Copyright (C) 1998 Chris Morley

  acconfig.h
*/

#define LIBVRML_MAJOR_VERSION	0
#define LIBVRML_MINOR_VERSION	8
#define LIBVRML_MICRO_VERSION	0

#ifdef _WIN32
// Disable warning messages about truncation from const double to float
# pragma warning( disable : 4305 )  
// Disable warning messages about truncating names in debug info
# pragma warning( disable : 4786 )  
static const char SLASH = '\\';
#else
static const char SLASH = '/';
#endif

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

#undef LIBVRML_VERSION
#undef HAVE_LIBJPEG
#undef HAVE_LIBPNG
#undef HAVE_ZLIB

@TOP@
@BOTTOM@

#endif	/* ACCONFIG_H */
