//
// Copyright (C) 2000  Braden N. McDaniel
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

//
// This class is a temporary hack to supplant a temporary hack. Sigh. At some
// point (Real Soon Now) URI fetching needs to be rearchitected (or perhaps
// simply "architected" would be more accurate). In the meantime, the scanner
// used with the new ANTLR parser needs an istream rather than the C-style I/O
// provided by the Doc class.
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# ifdef _WIN32
#   include <winconfig.h>
# endif

# include "doc2.hpp"
# include "System.h"
# include <fstream.h>
# include <string.h>
# include <ctype.h>
# include <algorithm>
# include <zlib.h>

namespace {
    namespace z {
        
        typedef int level;
        level const no_compression      = Z_NO_COMPRESSION;
        level const best_speed          = Z_BEST_SPEED;
        level const best_compression    = Z_BEST_COMPRESSION;
        level const default_compression = Z_DEFAULT_COMPRESSION;
        
        enum strategy {
            default_strategy    = Z_DEFAULT_STRATEGY,
            filtered            = Z_FILTERED,
            huffman_only        = Z_HUFFMAN_ONLY
        };
        
        class filebuf : public ::streambuf {
            
            public:
                filebuf();
                ~filebuf();
                
                bool is_open() const;
                filebuf * open(char const * path, int mode,
                               level = default_compression,
                               strategy = default_strategy);
                filebuf * close();
                
            protected:
                virtual int underflow();
                virtual int overflow(int = EOF);
                
            private:
                enum { buffer_size = 10 };
                char buffer[buffer_size];
                gzFile file;
        };
        
        class ifstream : public ::istream {
            public:
                ifstream();
                explicit ifstream(char const * path, level = default_compression,
                                  strategy = default_strategy);
                ~ifstream();
                
                filebuf * rdbuf() const;
                bool is_open() const;
                void open(char const * path, level = default_compression,
                          strategy = default_strategy);
                void close();
                
            private:
                filebuf fbuf;
        };
    }
}

Doc2::Doc2(const char * url, Doc2 const * relative)
  : url_(0), tmpfile_(0), istm_(0), ostm_(0)
{
    if (url != 0) {
        this->seturl(url, relative);
    }
}

Doc2::Doc2(Doc2 const * doc)
  : url_(0), tmpfile_(0), istm_(0), ostm_(0)
{
    if (doc) {
        this->seturl(doc->url());
    }
}

Doc2::~Doc2()
{
    delete [] url_;
    delete istm_;
    delete ostm_;
    if (tmpfile_) {
        theSystem->removeFile(
# ifdef macintosh
                              convertCommonToMacPath(tmpfile_, sizeof(tmpfile_))
# else
                              tmpfile_
# endif
                              );
        delete [] tmpfile_;
    }
}

void Doc2::seturl(char const * url, Doc2 const * relative)
{
    delete [] this->url_;
    this->url_ = 0;
    
    if (url) {
        
        delete this->istm_;
        this->istm_ = 0;
        delete this->ostm_;
        this->ostm_ = 0;
        
        char const * path = "";
        
        if (relative && !isAbsolute(url)) {
	    path = relative->urlPath();
        }
        
        this->url_ = new char[strlen(path) + strlen(url) + 1];
        strcpy(this->url_, path);
        
        if (strlen(url) > 2 && url[0] == '.' && url[1] == SLASH) {
            strcat(this->url_, url + 2); // skip "./"
        } else {
            strcat(this->url_, url);
        }
    }
}

char const * Doc2::url() const
{
    return this->url_;
}

char const * Doc2::urlBase() const
{
    if (!url_) {
        return "";
    }
    
    static char path[1024];
    char * p, * s = path;
    strncpy(path, url_, sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0';
    if ((p = strrchr(s, SLASH)) != 0) {
        s = p + 1;
    } else if ((p = strchr(s, COLON)) != 0) {
        s = p + 1;
    }
    
    if ((p = strrchr(s, '.')) != 0) {
        *p = '\0';
    }
    
    return s;
}

char const * Doc2::urlExt() const
{
    if (!url_) {
        return "";
    }
    
    static char ext[20];
    char * p;
    
    if ((p = strrchr(url_, '.')) != 0) {
        strncpy(ext, p + 1, sizeof(ext) - 1);
        ext[sizeof(ext)-1] = '\0';
    } else {
        ext[0] = '\0';
    }
    
    return &ext[0];
}

char const * Doc2::urlPath() const
{
    if (!url_) return "";
    
    static char path[1024];
    
    strcpy(path, url_);
    char * slash;
    if ((slash = strrchr(path, SLASH)) != 0) {
        *(slash+1) = '\0';
    } else {
        path[0] = '\0';
    }
    
    return &path[0]; 
}

char const * Doc2::urlProtocol() const
{
    if (url_) {
        static char protocol[12];
        char const * s = url_;
        
# ifdef _WIN32
        if (strncmp(s+1,":\\",2) == 0) {
            return "file";
        }
# endif
        for (unsigned int i = 0; i < sizeof(protocol); ++i, ++s) {
	    if (*s == 0 || !isalpha(*s)) {
	        protocol[i] = '\0';
	        break;
	    }
	    protocol[i] = tolower(*s);
	}
        protocol[sizeof(protocol)-1] = '\0';
        if (*s == COLON) {
            return protocol;
        }
    }
    
    return "file";
}

char const * Doc2::urlModifier() const
{
    char const * mod = url_ ? strrchr(url_, '#') : 0;
    return mod ? mod : "";
}

char const * Doc2::localName()
{
    static char buf[1024];
    
    if (filename(buf, sizeof(buf))) {
        return buf;
    }
    
    return 0;
}

char const * Doc2::localPath()
{
    static char buf[1024];
    
    if (filename(buf, sizeof(buf))) {
        
        char * s = strrchr(buf, SLASH);
        if (s) {
            *(s+1) = '\0';
        }
        
        return buf;
    }
    
    return 0;
}

::istream & Doc2::inputStream()
{
    if (!this->istm_) {
        
        char fn[256];
        
        filename(fn, sizeof(fn));
        if (strcmp(fn, "-") == 0) {
            this->istm_ = &cin;
        } else {
            this->istm_ = new z::ifstream(
# ifdef macintosh
                                          convertCommonToMacPath(fn, sizeof(fn))
# else
                                          fn
# endif
                                          );
        }
    }
    
    return *this->istm_;
}

::ostream & Doc2::outputStream()
{
    if (!ostm_) {
        ostm_ = new ::ofstream(stripProtocol(url_), ios::out);
    }
    
    return *this->ostm_;
}

char const * Doc2::stripProtocol(char const * url)
{
    char const * s = url;
    
# ifdef _WIN32
    if (strncmp(s+1,":\\",2) == 0) {
        return url;
    }
# endif
    
    // strip off protocol if any
    while (*s && isalpha(*s)) {
        ++s;
    }
    
    if (*s == COLON) {
        return s + 1;
    }
    
    return url;
}

bool Doc2::isAbsolute(const char *url)
{
  const char *s = stripProtocol(url);
  return ( *s == SLASH || *(s+1) == ':' );
}

// Converts a url into a local filename

bool Doc2::filename(char * fn, int nfn)
{
    fn[0] = '\0';
    
    char * e = 0;
    char const * s = stripProtocol(url_);
    
    if ((e = strrchr(s,'#')) != 0) {
        *e = '\0';
    }
    
    char const * protocol = urlProtocol();
    
    // Get a local copy of http files
    if (strcmp(protocol, "http") == 0) {
        if (tmpfile_) {    // Already fetched it
            s = tmpfile_;
        } else if ((s = (char *) theSystem->httpFetch(url_))) {
            tmpfile_ = new char[strlen(s)+1];
            strcpy(tmpfile_, s);
            free(const_cast<char *>(s));        // assumes tempnam or equiv...
            s = tmpfile_;
        }
    }
    // Unrecognized protocol (need ftp here...)
    else if (strcmp(protocol, "file") != 0) {
        s = 0;
    }
    
    if (s) {
        strncpy( fn, s, nfn-1 );
        fn[nfn-1] = '\0';
    }
    
    if (e) {
        *e = '#';
    }
    
    return s && *s;
}

# ifdef macintosh

namespace {
    
    inline char convertHex(char c)
    {
        static char converted;
        if (c>='0' && c<='9')
            converted = c-'0';
        else
            if (c>='A' && c<='F')
                converted = c-'A'+10;
            else 
                converted = c-'a'+10;
        return converted;
    }
    
    char* decodePath(const char* path)
    {
        static char converted[256];
        strcpy (converted, path);
    
        char * a = converted;
        char * b = converted;
        
        while(*a) {
            if (*a == '%') {
                a++;
                if (*a) {
                    *b = convertHex(*a++) * 16;
                }
                if (*a) {
                    *b = *b+convertHex(*a);
                }
                a++, b++;
            } else {
                *b++ = *a++; 
            }
        }
        *b++ = 0;
        
        return &converted[0];
    }
}

char* Doc2::convertCommonToMacPath( char *fn, int nfn )
{
  /* Note that only full paths can be use on the Mac to
     retrieve files correctly, so this function assumes
     that the viewer, e.g. Lookat, has provided VrmlScene with
     a file path in the form of a URL (optionally without the protocol
     if it is a local path) */
  
  static char macfn[256];
  
  // We start at index 3 in order to skip the /// at the start
  // of a legal Mac file protocol URL
    
  if ( !((nfn > 3) && (fn[0] == '/') && (fn[1] == '/') && (fn[2] == '/')) ) {
    return fn; // its either a tmp file from a URL transfer or its an incorrect path
  }

  int macfnpos = 0;
  for ( int i = 3; i < nfn; i++ ) {
    if ( fn[i] == SLASH ) {
      macfn[macfnpos] = COLON;
      macfnpos++;
    }
    else {
      if ( fn[i] == '.' ) {
         if ( (i+2 < nfn) && (fn[i+1] == '.') && (fn[i+2] == '/') ) {
           // replace "../" with an extra :
           macfn[macfnpos] = COLON;
           macfnpos++;
           i=i+2;
        }
        else {
          macfn[macfnpos] = fn[i];
          macfnpos++;
        }
      } 
      else {
        macfn[macfnpos] = fn[i];
        macfnpos++;
      }
    }
  }
  return decodePath(macfn);
}

# endif /* macintosh */

namespace {
    
    ////////////////////////////////////////////////////////////////////////////
    // filebuf
    //
    
    int const lookback(4);
    
    z::filebuf::filebuf()
      : file(0)
    {
        setg(this->buffer + lookback,     // beginning of putback area
             this->buffer + lookback,     // read position
             this->buffer + lookback);    // end position
    }
    
    z::filebuf::~filebuf()
    {
        this->close();
    }
    
    bool z::filebuf::is_open() const
    {
        return this->file;
    }
    
    z::filebuf * z::filebuf::open(const char * path, int mode,
                                  level comp_level, strategy comp_strategy)
    {
        if (this->file) {
            return 0;
        }
        
        //
        // zlib only supports the "rb" and "wb" modes, so we bail on anything
        // else.
        //
        static char const * const read_mode_string = "rb";
        static char const * const write_mode_string = "wb";
        char const * mode_string = 0;
        if (mode == (ios::binary | ios::in)) {
            mode_string = read_mode_string;
        } else if (   (mode == (ios::binary | ios::out))
                   || (mode == (ios::binary | ios::out | ios::trunc))) {
            mode_string = write_mode_string;
        } else {
            return 0;
        }
        
        this->file = gzopen(path, mode_string);
        if (!this->file) {
            return 0;
        }
        
        gzsetparams(this->file, comp_level, comp_strategy);
        
        return this;
    }
    
    z::filebuf * z::filebuf::close()
    {
        if (!this->file) {
            return 0;
        }
        
        gzclose(this->file);
        this->file = 0;
        
        return this;
    }
    
    int z::filebuf::underflow()
    {
        if (gptr() < egptr()) {
            return *gptr();
        }
        
        //
        // Process the size of the putback area; use the number of characters read,
        // but at most four.
        //
        int num_putback = gptr() - eback();
        if (num_putback > lookback) {
            num_putback = lookback;
        }
        
        std::copy(gptr() - num_putback, gptr(),
                  this->buffer + (lookback - num_putback));
    
        //
        // Read new characters.
        //
        int num = gzread(this->file,
                         this->buffer + lookback,
                         filebuf::buffer_size - lookback);
        if (num <= 0) {
            //
            // Error condition or end of file.
            //
            return EOF;
        }
        
        //
        // Reset the buffer pointers.
        //
        setg(buffer + (lookback - num_putback), // Beginning of putback area.
             buffer + lookback,                 // Read position.
             buffer + lookback + num);          // End of buffer.
        
        //
        // Return the next character.
        //
        return *gptr();
    }
    
    int z::filebuf::overflow(int c)
    {
        //
        // This probably ought to be buffered, but this will do for now.
        //
        if (c != EOF) {
            if (gzputc(file, c) == -1) {
                return EOF;
            }
        }
        
        return c;
    }
    
    
    ////////////////////////////////////////////////////////////////////////////
    // ifstream
    //
    
    z::ifstream::ifstream()
      : istream(&fbuf)
    {}
    
    z::ifstream::ifstream(char const * path, level lev, strategy strat)
      : istream(&fbuf)
    {
        this->open(path, lev, strat);
    }
    
    z::ifstream::~ifstream() {}
    
    z::filebuf * z::ifstream::rdbuf() const
    {
        return const_cast<z::filebuf *>(&this->fbuf);
    }

    bool z::ifstream::is_open() const
    {
        return this->fbuf.is_open();
    }
    
    void z::ifstream::open(char const * path, level lev, strategy strat)
    {
        if (!this->fbuf.open(path, ios::binary | ios::in, lev, strat)) {
# ifdef _WIN32
            this->clear(failbit);
# else
            this->setstate(failbit);
# endif
        }
    }
    
    void z::ifstream::close()
    {
        this->fbuf.close();
    }
    
}
