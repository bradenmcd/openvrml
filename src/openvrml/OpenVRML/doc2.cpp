//
// OpenVRML
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

# if defined(_WIN32) && !defined(__CYGWIN__)
#   include <winconfig.h>
# endif

# include "doc2.hpp"
# include "System.h"
# include <string.h>
# include <ctype.h>
# include <algorithm>
# include <fstream>

# ifdef OPENVRML_HAVE_ZLIB
#   include <zlib.h>

namespace {
    namespace z {
        
        typedef int level;
        const level no_compression      = Z_NO_COMPRESSION;
        const level best_speed          = Z_BEST_SPEED;
        const level best_compression    = Z_BEST_COMPRESSION;
        const level default_compression = Z_DEFAULT_COMPRESSION;
        
        enum strategy {
            default_strategy    = Z_DEFAULT_STRATEGY,
            filtered            = Z_FILTERED,
            huffman_only        = Z_HUFFMAN_ONLY
        };
        
        class filebuf : public std::streambuf {
            enum { buffer_size = 10 };
            char buffer[buffer_size];
            gzFile file;
        
        public:
            filebuf();
            virtual ~filebuf();

            bool is_open() const;
            filebuf * open(const char * path, int mode,
                           level = default_compression,
                           strategy = default_strategy);
            filebuf * close();

        protected:
            virtual int underflow();
            virtual int overflow(int = EOF);
        };
        
        class ifstream : public std::istream {
            filebuf fbuf;
        
        public:
            ifstream();
            explicit ifstream(const char * path, level = default_compression,
                              strategy = default_strategy);
            virtual ~ifstream();

            filebuf * rdbuf() const;
            bool is_open() const;
            void open(const char * path, level = default_compression,
                      strategy = default_strategy);
            void close();
        };
        
        //
        // filebuf
        //

        int const lookback(4);

        filebuf::filebuf(): file(0) {
            this->setg(this->buffer + lookback,  // beginning of putback area
                       this->buffer + lookback,  // read position
                       this->buffer + lookback); // end position
        }

        filebuf::~filebuf() {
            this->close();
        }

        bool filebuf::is_open() const {
            return (this->file != 0);
        }

        filebuf * filebuf::open(const char * path,
                                const int mode,
                                const level comp_level,
                                const strategy comp_strategy) {
            using std::ios;
            
            if (this->file) { return 0; }

            //
            // zlib only supports the "rb" and "wb" modes, so we bail on anything
            // else.
            //
            static const char read_mode_string[] = "rb";
            static const char write_mode_string[] = "wb";
            const char * mode_string = 0;
            if (mode == (ios::binary | ios::in)) {
                mode_string = read_mode_string;
            } else if (   (mode == (ios::binary | ios::out))
                       || (mode == (ios::binary | ios::out | ios::trunc))) {
                mode_string = write_mode_string;
            } else {
                return 0;
            }

            this->file = gzopen(path, mode_string);
            if (!this->file) { return 0; }

            gzsetparams(this->file, comp_level, comp_strategy);
            return this;
        }

        filebuf * filebuf::close() {
            if (!this->file) { return 0; }
            gzclose(this->file);
            this->file = 0;
            return this;
        }

        int filebuf::underflow() {
            if (this->gptr() < this->egptr()) { return *this->gptr(); }

            //
            // Process the size of the putback area; use the number of characters read,
            // but at most four.
            //
            int num_putback = this->gptr() - this->eback();
            if (num_putback > lookback) { num_putback = lookback; }

            std::copy(this->gptr() - num_putback, this->gptr(),
                      this->buffer + (lookback - num_putback));

            //
            // Read new characters.
            //
            int num = gzread(this->file,
                             this->buffer + lookback,
                             filebuf::buffer_size - lookback);
            
            if (num <= 0) { return EOF; } // Error condition or end of file.

            //
            // Reset the buffer pointers.
            //
            this->setg(buffer + (lookback - num_putback), // Beginning of putback area.
                       buffer + lookback,                 // Read position.
                       buffer + lookback + num);          // End of buffer.

            //
            // Return the next character.
            //
            return *this->gptr();
        }

        int filebuf::overflow(int c) {
            //
            // This probably ought to be buffered, but this will do for now.
            //
            if (c != EOF) {
                if (gzputc(file, c) == -1) { return EOF; }
            }
            return c;
        }


        //
        // ifstream
        //

        ifstream::ifstream(): std::istream(&fbuf) {}

        ifstream::ifstream(const char * path, level lev, strategy strat):
                std::istream(&fbuf) {
            this->open(path, lev, strat);
        }

        ifstream::~ifstream() {}

        filebuf * ifstream::rdbuf() const {
            return const_cast<filebuf *>(&this->fbuf);
        }

        bool ifstream::is_open() const { return this->fbuf.is_open(); }

        void ifstream::open(const char * path, level lev, strategy strat) {
            using std::ios;
            if (!this->fbuf.open(path, ios::binary | ios::in, lev, strat)) {
#   ifdef _WIN32
                this->clear(failbit);
#   else
                this->setstate(failbit);
#   endif
            }
        }

        void ifstream::close() { this->fbuf.close(); }
    }
}
# endif // OPENVRML_HAVE_ZLIB

namespace OpenVRML {

/**
 * @class Doc2
 *
 * @brief A class to contain document references.
 *
 * Doc2 is a hack of Doc. When the ANTLR parser was added to %OpenVRML, a Doc
 * work-alike was needed that would read from a std::istream instead of a C
 * @c FILE @c *. Doc2's purpose is to fill that need, and to remind us through
 * its ugliness just how badly both it and Doc need to be replaced with an I/O
 * solution that doesn't suck.
 */

/**
 * @var char * Doc2::url_
 *
 * @brief The URL.
 */

/**
 * @var char * Doc2::tmpfile_
 *
 * @brief Name of the temporary file created for the local copy of the resource.
 */

/**
 * @var std::istream * Doc2::istm_
 *
 * @brief A file descriptor for reading the local copy of the resource.
 */

/**
 * @var std::ostream * Doc2::ostm_
 *
 * @brief A pointer to a std::ostream used for writing the resource.
 */

/**
 * @brief Constructor.
 *
 * @param url       an HTTP or file URL.
 * @param relative  the Doc2 that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
Doc2::Doc2(const std::string & url, const Doc2 * relative)
  : url_(0), tmpfile_(0), istm_(0), ostm_(0)
{
    if (url.length() > 0) {
        this->seturl(url.c_str(), relative);
    }
}

/**
 * @brief Destructor.
 */
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

namespace {
    const char * stripProtocol(const char * url) {
        const char * s = url;

# ifdef _WIN32
        if (strncmp(s+1,":/",2) == 0) {
            return url;
        }
# endif

        // strip off protocol if any
        while (*s && isalpha(*s)) {
            ++s;
        }

        if (*s == ':') {
            return s + 1;
        }

        return url;
    }

    bool isAbsolute(const char * url) {
      const char *s = stripProtocol(url);
      return ( *s == '/' || *(s+1) == ':' );
    }
}

/**
 * @brief Set the URL.
 *
 * @param url       the new URL.
 * @param relative  the Doc2 that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
void Doc2::seturl(const char * url, const Doc2 * relative) {
    delete [] this->url_;
    this->url_ = 0;
    
    if (url) {
        
        delete this->istm_;
        this->istm_ = 0;
        delete this->ostm_;
        this->ostm_ = 0;
        
        const char * path = "";

#ifdef _WIN32     
// Convert windows path stream to standard URL
	  char *p = (char *)url;
	  for(;*p != '\0';p++)
		  if(*p == '\\')*p = '/';
#endif
        
        if (relative && !isAbsolute(url)) {
	    path = relative->urlPath();
        }
        
        this->url_ = new char[strlen(path) + strlen(url) + 1];
        strcpy(this->url_, path);
        
        if (strlen(url) > 2 && url[0] == '.' && url[1] == '/') {
            strcat(this->url_, url + 2); // skip "./"
        } else {
            strcat(this->url_, url);
        }
    }
}

/**
 * @brief Get the URL.
 *
 * @return the URL.
 */
const char * Doc2::url() const { return this->url_; }

/**
 * @brief Get the portion of the path likely to correspond to a file name
 *      without its extension.
 *
 * @return the portion of the last path element preceding the last '.' in the
 *      path, or an empty string if the last path element is empty.
 */
const char * Doc2::urlBase() const {
    if (!url_) { return ""; }
    
    static char path[1024];
    char * p, * s = path;
    strncpy(path, url_, sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0';
    if ((p = strrchr(s, '/')) != 0) {
        s = p + 1;
    } else if ((p = strchr(s, ':')) != 0) {
        s = p + 1;
    }
    
    if ((p = strrchr(s, '.')) != 0) {
        *p = '\0';
    }
    
    return s;
}

/**
 * @brief Get the portion of the path likely to correspond to a file name
 *      extension.
 *
 * @return the portion of the last path element succeeding the last '.' in the
 *      path, or an empty string if the last path element includes no '.'.
 */
const char * Doc2::urlExt() const {
    if (!url_) { return ""; }
    
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

/**
 * @brief Get the URL without the last component of the path.
 *
 * In spite of its name, this method does not return the URL's path.
 *
 * @return the portion of the URL including the scheme, the authority, and all
 *      but the last component of the path.
 */
const char * Doc2::urlPath() const {
    if (!url_) { return ""; }
    
    static char path[1024];
    
    strcpy(path, url_);
    char * slash;
    if ((slash = strrchr(path, '/')) != 0) {
        *(slash+1) = '\0';
    } else {
        path[0] = '\0';
    }
    
    return &path[0]; 
}

/**
 * @brief Get the URL scheme.
 *
 * @return the URL scheme.
 */
const char * Doc2::urlProtocol() const {
    if (url_) {
        static char protocol[12];
        const char * s = url_;
        
# ifdef _WIN32
        if (strncmp(s+1,":/",2) == 0) {
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
        if (*s == ':') {
            return protocol;
        }
    }
    
    return "file";
}

/**
 * @brief Get the fragment identifier.
 *
 * @return the fragment identifier, including the leading '#', or an empty
 *      string if there is no fragment identifier.
 */
const char * Doc2::urlModifier() const {
    const char * mod = url_ ? strrchr(url_, '#') : 0;
    return mod;
}

/**
 * @brief Get the fully qualified name of a local file that is the downloaded
 *      resource at @a d_url.
 *
 * @return the fully qualified name of a local file that is the downloaded
 *      resource at @a d_url.
 */
const char * Doc2::localName() {
    static char buf[1024];
    if (filename(buf, sizeof(buf))) { return buf; }
    return 0;
}

/**
 * @brief Get the path of the local file that is the downloaded resource at
 *      @a d_url.
 *
 * @return the path of the local file that is the downloaded resource at
 *      @a d_url.
 */
const char * Doc2::localPath() {
    static char buf[1024];
    
    if (filename(buf, sizeof(buf))) {
        
        char * s = strrchr(buf, '/');
        if (s) {
            *(s+1) = '\0';
        }
        
        return buf;
    }
    
    return 0;
}

/**
 * @brief Get an input stream for the resource.
 *
 * @return an input stream for the resource.
 */
std::istream & Doc2::inputStream() {
    if (!this->istm_) {
        
        char fn[256];
        
        filename(fn, sizeof(fn));
        if (strcmp(fn, "-") == 0) {
            this->istm_ = &std::cin;
        } else {
# ifdef OPENVRML_HAVE_ZLIB
            this->istm_ = new z::ifstream(
#   ifdef macintosh
                                          convertCommonToMacPath(fn, sizeof(fn))
#   else
                                          fn
#   endif
                                          );
# else
            this->istm_ = new std::ifstream(
#   ifdef macintosh
                                          convertCommonToMacPath(fn, sizeof(fn))
#   else
                                          fn
#   endif
                                          );
# endif
        }
    }
    
    return *this->istm_;
}

/**
 * @brief Get an output stream for the resource.
 *
 * @return an output stream for the resource.
 */
std::ostream & Doc2::outputStream() {
    if (!ostm_) {
        ostm_ = new std::ofstream(stripProtocol(url_), std::ios::out);
    }
    return *this->ostm_;
}

/**
 * @brief Converts a url into a local filename.
 *
 * @retval fn   a character buffer to hold the local filename.
 * @param nfn   the number of elements in the buffer @p fn points to.
 */
bool Doc2::filename(char * fn, int nfn) {
    fn[0] = '\0';
    
    char * e = 0;
    const char * s = stripProtocol(url_);
    
    if ((e = strrchr(s,'#')) != 0) {
        *e = '\0';
    }
    
    const char * protocol = urlProtocol();
    
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

#ifdef _WIN32   
  // Does not like "//C:" skip "// "  
   if(s)
	   if(strlen(s)>2 && s[0] == '/' && s[1] == '/')s=s+2;
#endif  
  
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
    if ( fn[i] == '/' ) {
      macfn[macfnpos] = ':';
      macfnpos++;
    }
    else {
      if ( fn[i] == '.' ) {
         if ( (i+2 < nfn) && (fn[i+1] == '.') && (fn[i+2] == '/') ) {
           // replace "../" with an extra :
           macfn[macfnpos] = ':';
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

} // namespace OpenVRML
