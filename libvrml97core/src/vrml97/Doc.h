//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
#ifndef DOC_H
#define DOC_H
//
//  Document class
//

// Need to get rid of stdio (used by flex)...
//struct FILE; gcc doesn't like this so #include <stdio.h> is necessary
#include <stdio.h>
#include <iostream.h>
#include <zlib.h>

class Doc {

public:

  Doc(const char *url = 0, Doc *relative = 0);
  Doc(Doc *);
  ~Doc();

  void seturl(const char *url, Doc *relative = 0);

  const char *url();		// "http://www.foo.com/dir/file.xyz#Viewpoint"
  const char *urlBase();	// "file" or ""
  const char *urlExt();		// "xyz" or ""
  const char *urlPath();	// "http://www.foo.com/dir/" or ""
  const char *urlProtocol();	// "http"
  const char *urlModifier();	// "#Viewpoint" or ""

  const char *localName();	// "/tmp/file.xyz" or NULL
  const char *localPath();	// "/tmp/" or NULL


  FILE *fopen(const char *mode);
  void fclose();

  // For (optionally) compressed files
  gzFile gzopen(const char *mode);
  void gzclose();

  ostream &outputStream();

protected:

  static const char *stripProtocol(const char *url);
  static bool isAbsolute(const char *url);
#ifdef macintosh
  bool filename( char *fn, int nfn );
  char* convertCommonToMacPath( char *fn, int nfn );
#else
  bool Doc::filename( char *fn, int nfn );
#endif

  char *d_url;
  ostream *d_ostream;
  FILE *d_fp;
  gzFile d_gz;
  char *d_tmpfile;		// Local copy of http: files

};
#endif // DOC_H
