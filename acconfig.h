#ifndef CONFIG_H
#define CONFIG_H
/*
  Vrml 97 library
  Copyright (C) 1998 Chris Morley

  acconfig.h
*/

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

#undef OPENVRML_MAJOR_VERSION
#undef OPENVRML_MINOR_VERSION
#undef OPENVRML_MICRO_VERSION
#undef HAVE_LIBJPEG
#undef HAVE_LIBPNG
#undef HAVE_ZLIB
#undef XP_UNIX
#undef AIX4_3

@TOP@
@BOTTOM@

#endif	/* CONFIG_H */
