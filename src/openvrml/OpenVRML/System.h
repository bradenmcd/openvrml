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

# ifndef SYSTEM_H
#   define SYSTEM_H
//
//  System dependent utilities class
//

#   include <stddef.h>
#   include "common.h"

OPENVRML_SCOPE class System {

public:

  virtual double time();

  virtual void error(const char *, ...);

  virtual void warn(const char *, ...);

  virtual void inform(const char *, ...);

  virtual void debug(const char *, ...);

#ifdef macintosh
  virtual bool loadUrl(char *url, int np, char **parameters );
#else
  virtual bool loadUrl(const char *url, size_t np, char const * const * parameters );
#endif

  virtual int connectSocket( const char *host, int port );

  virtual const char *httpHost(const char *url, int *port);
  virtual const char *httpFetch( const char *url );

  virtual void removeFile( const char *fn );

};

extern System *theSystem;

#endif // SYSTEM_H
