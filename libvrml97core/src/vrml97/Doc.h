//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
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

class Doc2;
typedef void * gzFile;

class Doc {

public:

  Doc(char const * url = 0, Doc const *relative = 0);
  Doc(char const * url, Doc2 const * relative);
  Doc(Doc const *);
  Doc(Doc2 const *);
  ~Doc();

  void seturl(char const * url, Doc const * relative = 0);
  void seturl(char const * url, Doc2 const * relative = 0);

  const char *url() const;          // "http://www.foo.com/dir/file.xyz#Viewpoint"
  const char *urlBase() const;      // "file" or ""
  const char *urlExt() const;       // "xyz" or ""
  const char *urlPath() const;      // "http://www.foo.com/dir/" or ""
  const char *urlProtocol() const;  // "http"
  const char *urlModifier() const;  // "#Viewpoint" or ""

  const char *localName();    // "/tmp/file.xyz" or NULL
  const char *localPath();    // "/tmp/" or NULL


  FILE *fopen(const char *mode);
  void fclose();

  // For (optionally) compressed files
  gzFile gzopen(const char *mode);
  void gzclose();

  ostream &outputStream();

protected:

  static const char *stripProtocol(const char *url);
  static bool isAbsolute(const char *url);
  bool filename( char *fn, int nfn );
#ifdef macintosh
  char* convertCommonToMacPath( char *fn, int nfn );
#endif

  char *d_url;
  ostream *d_ostream;
  FILE *d_fp;
  gzFile d_gz;
  char *d_tmpfile;		// Local copy of http: files
  
private:
    //
    // Non-copyable; copy ctor and operator= are declared private and not
    // defined.
    //
    Doc(Doc const &);
    Doc & operator=(Doc const &);

};
#endif // DOC_H
