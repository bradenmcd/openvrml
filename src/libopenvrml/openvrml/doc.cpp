// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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

# include <cstdlib>
# include <cctype>
# include <cstring>
# include <fstream>
# include <regex.h>

# include "private.h"
# include "doc.h"
# include "system.h"
# include "browser.h"

namespace openvrml {

/**
 * @class doc
 *
 * @brief A class to contain document references.
 *
 * This is just a shell until a real http protocol library is found...
 */

/**
 * @var char * doc::url_
 *
 * @brief The URL.
 */

/**
 * @var std::ostream * doc::out_
 *
 * @brief A pointer to a std::ostream used for writing the resource.
 */

/**
 * @var FILE * doc::fp_
 *
 * @brief A file descriptor for reading the local copy of the resource.
 */

/**
 * @var char * doc::tmpfile_
 *
 * @brief Name of the temporary file created for the local copy of the
 *        resource.
 */

/**
 * @brief Constructor.
 *
 * @param url       an HTTP or file URL.
 * @param relative  the doc that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
doc::doc(const std::string & url, const doc * relative):
    url_(0),
    out_(0),
    fp_(0),
    tmpfile_(0)
{
    if (!url.empty()) { this->seturl(url.c_str(), relative); }
}

/**
 * @brief Constructor.
 *
 * @param url       an HTTP or file URL.
 * @param relative  the doc2 that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
doc::doc(const std::string & url, const doc2 * relative):
    url_(0),
    out_(0),
    fp_(0),
    tmpfile_(0)
{
    if (!url.empty()) { this->seturl(url.c_str(), relative); }
}

/**
 * @brief Destructor.
 */
doc::~doc()
{
    delete [] this->url_;
    delete this->out_;
    if (this->tmpfile_) {
        the_system->remove_file(this->tmpfile_);
        delete [] this->tmpfile_;
    }
}

namespace {
    const char * stripProtocol(const char *url)
    {
      const char *s = url;

#ifdef _WIN32
      if (strncmp(s+1,":/",2) == 0) return url;
#endif

      // strip off protocol if any
      while (*s && isalpha(*s)) ++s;

      if (*s == ':')
        return s + 1;

      return url;
    }

    bool isAbsolute(const char *url)
    {
      const char *s = stripProtocol(url);
      return ( *s == '/' || *(s+1) == ':' );
    }
}

/**
  * @brief Set the URL.
 *
 * @param url       the new URL.
 * @param relative  the doc that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
void doc::seturl(const char * const url, const doc * const relative)
{
  delete [] url_;
  url_ = 0;

  if (url)
  {
      const char *path = "";

#ifdef _WIN32
// Convert windows path stream to standard URL
	  char *p = (char *)url;
	  for(;*p != '\0';p++)
		  if(*p == '\\')*p = '/';
#endif

      if ( relative && ! isAbsolute(url) )
	    path = relative->url_path();

      url_ = new char[strlen(path) + strlen(url) + 1];
      strcpy(url_, path);

      if (strlen(url)>2 && url[0] == '.' && url[1] == '/')
        strcat(url_, url+2); // skip "./"
      else
        strcat(url_, url);
  }
}

/**
 * @brief Set the URL.
 *
 * @param url       the new URL.
 * @param relative  the doc2 that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
void doc::seturl(const char * const url, const doc2 * const relative)
{
    delete [] this->url_;
    this->url_ = 0;

    if (url) {
        std::string path;

#ifdef _WIN32
        // Convert windows path stream to standard URL
        char *p = (char *)url;
        for (; *p != '\0'; p++) { if (*p == '\\') { *p = '/'; } }
#endif

        if (relative && !isAbsolute(url)) { path = relative->url_path(); }

        this->url_ = new char[path.length() + strlen(url) + 1];
        strcpy(this->url_, path.c_str());

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
const char * doc::url() const { return url_; }

/**
 * @brief Get the portion of the path likely to correspond to a file name
 *      without its extension.
 *
 * @return the portion of the last path element preceding the last '.' in the
 *      path, or an empty string if the last path element is empty.
 */
const char * doc::url_base() const
{
  if (! url_) return "";

  static char path[1024];
  char *p, *s = path;
  strncpy(path, url_, sizeof(path)-1);
  path[sizeof(path)-1] = '\0';
  if ((p = strrchr(s, '/')) != 0)
    s = p+1;
  else if ((p = strchr(s, ':')) != 0)
    s = p+1;

  if ((p = strrchr(s, '.')) != 0)
    *p = '\0';

  return s;
}

/**
 * @brief Get the portion of the path likely to correspond to a file name
 *      extension.
 *
 * @return the portion of the last path element succeeding the last '.' in the
 *      path, or an empty string if the last path element includes no '.'.
 */
const char * doc::url_ext() const
{
  if (! url_) return "";

  static char ext[20];
  char *p;

  if ((p = strrchr(url_, '.')) != 0)
    {
      strncpy(ext, p+1, sizeof(ext)-1);
      ext[sizeof(ext)-1] = '\0';
    }
  else
    ext[0] = '\0';

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
const char * doc::url_path() const
{
  if (! url_) return "";

  static char path[1024];

  strcpy(path, url_);
  char *slash;
  if ((slash = strrchr(path, '/')) != 0)
    *(slash+1) = '\0';
  else
    path[0] = '\0';
  return &path[0];
}

/**
 * @brief Get the URL scheme.
 *
 * @return the URL scheme.
 */
const char * doc::url_protocol() const
{
  if (url_)
    {
      static char protocol[12];
      const char *s = url_;

#ifdef _WIN32
      if (strncmp(s+1,":/",2) == 0) return "file";
#endif

      for (unsigned int i=0; i<sizeof(protocol); ++i, ++s)
	{
	  if (*s == 0 || ! isalpha(*s))
	    {
	      protocol[i] = '\0';
	      break;
	    }
	  protocol[i] = tolower(*s);
	}
      protocol[sizeof(protocol)-1] = '\0';
      if (*s == ':')
	return protocol;
    }

  return "file";
}

/**
 * @brief Get the fragment identifier.
 *
 * @return the fragment identifier, including the leading '#', or an empty
 *      string if there is no fragment identifier.
 */
const char * doc::url_modifier() const
{
  char *mod = url_ ? strrchr(url_,'#') : 0;
  return mod;
}

/**
 * @brief Get the fully qualified name of a local file that is the downloaded
 *      resource at @a url_.
 *
 * @return the fully qualified name of a local file that is the downloaded
 *      resource at @a url_.
 */
const char * doc::local_name()
{
  static char buf[1024];
  if (filename(buf, sizeof(buf)))
    return &buf[0];
  return 0;
}

/**
 * @brief Get the path of the local file that is the downloaded resource at
 *      @a url_.
 *
 * @return the path of the local file that is the downloaded resource at
 *      @a url_.
 */
const char * doc::local_path()
{
  static char buf[1024];
  if (filename(buf, sizeof(buf)))
    {
      char *s = strrchr(buf, '/');
      if (s) *(s+1) = '\0';
      return &buf[0];
    }
  return 0;
}

/**
 * @brief Converts a url into a local filename.
 *
 * @retval fn   a character buffer to hold the local filename.
 * @param nfn   the number of elements in the buffer @p fn points to.
 */
bool doc::filename(char * fn, int nfn)
{
  fn[0] = '\0';

  char *e = 0, *s = const_cast<char *>(stripProtocol(url_));

  if ((e = strrchr(s,'#')) != 0)
    *e = '\0';

  const char *protocol = url_protocol();

  // Get a local copy of http files
  if (strcmp(protocol, "http") == 0)
    {
      if (tmpfile_)		// Already fetched it
	s = tmpfile_;
      else if ((s = const_cast<char *>(the_system->http_fetch(url_))))
	{
	  tmpfile_ = new char[strlen(s)+1];
	  strcpy(tmpfile_, s);
	  free(s);		// assumes tempnam or equiv...
	  s = tmpfile_;
	}
    }

  // Unrecognized protocol (need ftp here...)
  else if (strcmp(protocol, "file") != 0)
    s = 0;

#ifdef _WIN32
  // Does not like "//C:" skip "// "
   if(s)
	   if(strlen(s)>2 && s[0] == '/' && s[1] == '/')s=s+2;
#endif

  if (s)
    {
      strncpy( fn, s, nfn-1 );
      fn[nfn-1] = '\0';
    }

  if (e) *e = '#';

  return s && *s;
}

/**
 * @brief Open a file.
 *
 * @return a pointer to a FILE struct for the opened file.
 *
 * Having both fopen and outputStream is dumb.
 */
FILE *doc::fopen(const char *mode)
{
    if (this->fp_) {
        OPENVRML_PRINT_MESSAGE_(std::string(this->url_ ? this->url_ : "")
                                + "is already open.");
    }

    char fn[256];
    if (filename(fn, sizeof(fn))) {
        if (strcmp(fn, "-") == 0) {
            if (*mode == 'r') {
                fp_ = stdin;
            } else if (*mode == 'w') {
                fp_ = stdout;
            }
        } else {
            fp_ = ::fopen( fn, mode );
        }
    }
    return fp_;
}

/**
 * @brief Close a file.
 *
 * Closes the file opened with doc::fopen.
 */
void doc::fclose()
{
  if (fp_ && (strcmp(url_, "-") != 0) && (strncmp(url_, "-#", 2) != 0))
    ::fclose(fp_);

  fp_ = 0;
  if (tmpfile_)
    {
      the_system->remove_file(tmpfile_);
      delete [] tmpfile_;
      tmpfile_ = 0;
    }
}

/**
 * @brief Get an output stream for writing to the resource.
 *
 * @return an output stream.
 */
std::ostream & doc::output_stream()
{
    this->out_ = new std::ofstream(stripProtocol(url_), std::ios::out);
    return *this->out_;
}

} // namespace openvrml

# ifdef OPENVRML_ENABLE_GZIP
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
            enum { buffer_size = 16384 };
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

        ifstream::ifstream(): std::basic_istream<char>(&fbuf) {}

        ifstream::ifstream(const char * path, level lev, strategy strat):
                std::basic_istream<char>(&fbuf) {
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
# endif // OPENVRML_ENABLE_GZIP

namespace openvrml {

namespace {

    class URI {
        std::string str;
        enum { nmatch = 11 };
        regmatch_t regmatch[nmatch];

    public:
        explicit URI(const std::string & str)
                throw (invalid_url, std::bad_alloc);

        operator std::string() const throw (std::bad_alloc);

        const std::string getScheme() const throw (std::bad_alloc);
        const std::string getSchemeSpecificPart() const throw (std::bad_alloc);
        const std::string getAuthority() const throw (std::bad_alloc);
        const std::string getUserinfo() const throw (std::bad_alloc);
        const std::string getHost() const throw (std::bad_alloc);
        const std::string getPort() const throw (std::bad_alloc);
        const std::string getPath() const throw (std::bad_alloc);
        const std::string getQuery() const throw (std::bad_alloc);
        const std::string getFragment() const throw (std::bad_alloc);

        const URI resolveAgainst(const URI & absoluteURI) const
                throw (std::bad_alloc);
    };
}

/**
 * @class doc2
 *
 * @brief A class to contain document references.
 *
 * doc2 is a hack of doc. When the ANTLR parser was added to OpenVRML, a doc
 * work-alike was needed that would read from a std::istream instead of a C
 * @c FILE @c *. doc2's purpose is to fill that need, and to remind us through
 * its ugliness just how badly both it and doc need to be replaced with an I/O
 * solution that doesn't suck.
 */

/**
 * @var char * doc2::url_
 *
 * @brief The URL.
 */

/**
 * @var char * doc2::tmpfile_
 *
 * @brief Name of the temporary file created for the local copy of the
 *        resource.
 */

/**
 * @var std::istream * doc2::istm_
 *
 * @brief A file descriptor for reading the local copy of the resource.
 */

/**
 * @var std::ostream * doc2::ostm_
 *
 * @brief A pointer to a std::ostream used for writing the resource.
 */

/**
 * @brief Constructor.
 *
 * @param url       an HTTP or file URL.
 * @param relative  the doc2 that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
doc2::doc2(const std::string & url, const doc2 * relative):
    tmpfile_(0),
    istm_(0),
    ostm_(0)
{
    if (!url.empty()) {
        this->seturl(url, relative);
    }
}

/**
 * @brief Destructor.
 */
doc2::~doc2()
{
    delete istm_;
    delete ostm_;
    if (tmpfile_) {
        the_system->remove_file(tmpfile_);
        delete [] tmpfile_;
    }
}

namespace {
    const std::string stripProtocol(const std::string & url) {
        using std::string;
        const string::size_type colonPos = url.find_first_of(':');
        return (colonPos != string::npos)
                ? url.substr(colonPos + 1)
                : url;
    }

    bool isAbsolute(const std::string & url) {
        return stripProtocol(url)[0] == '/';
    }
}

/**
 * @brief Set the URL.
 *
 * @param url       the new URL.
 * @param relative  the doc2 that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
void doc2::seturl(const std::string & url, const doc2 * relative) {
    using std::string;

    this->url_ = string();

    if (!url.empty()) {

        delete this->istm_;
        this->istm_ = 0;
        delete this->ostm_;
        this->ostm_ = 0;

        string path;

        if (relative && !isAbsolute(url)) {
            path = relative->url_path();
        }

        this->url_ = path;

        if (url.length() > 2 && url[0] == '.' && url[1] == '/') {
            this->url_ += url.substr(2);
        } else {
            this->url_ += url;
        }
    }
}

/**
 * @brief Get the URL.
 *
 * @return the URL.
 */
const std::string doc2::url() const { return this->url_; }

/**
 * @brief Get the portion of the path likely to correspond to a file name
 *      without its extension.
 *
 * @return the portion of the last path element preceding the last '.' in the
 *      path, or an empty string if the last path element is empty.
 */
const std::string doc2::url_base() const {
    using std::string;

    string::size_type lastSlashPos = this->url_.find_last_of('/');
    string::size_type lastDotPos = this->url_.find_last_of('.');

    string::size_type beginPos = (lastSlashPos != string::npos)
                               ? lastSlashPos + 1
                               : 0;
    string::size_type length = (lastDotPos != string::npos)
                             ? lastDotPos - beginPos
                             : this->url_.length() - 1 - beginPos;

    return (beginPos < this->url_.length())
            ? this->url_.substr(beginPos, length)
            : "";
}

/**
 * @brief Get the portion of the path likely to correspond to a file name
 *      extension.
 *
 * @return the portion of the last path element succeeding the last '.' in the
 *      path, or an empty string if the last path element includes no '.'.
 */
const std::string doc2::url_ext() const {
    using std::string;
    string::size_type lastDotPos = this->url_.find_last_of('.');
    return (lastDotPos != string::npos)
            ? this->url_.substr(lastDotPos + 1)
            : "";
}

/**
 * @brief Get the URL without the last component of the path.
 *
 * In spite of its name, this method does not return the URL's path.
 *
 * @return the portion of the URL including the scheme, the authority, and all
 *      but the last component of the path.
 */
const std::string doc2::url_path() const {
    using std::string;

    string::size_type lastSlashPos = this->url_.find_last_of('/');

    return (lastSlashPos != string::npos)
            ? this->url_.substr(0, lastSlashPos + 1)
            : this->url_;
}

/**
 * @brief Get the URL scheme.
 *
 * @return the URL scheme.
 */
const std::string doc2::url_protocol() const {
    using std::string;

    string::size_type firstColonPos = this->url_.find_first_of(':');
    return (firstColonPos != string::npos)
            ? this->url_.substr(0, firstColonPos)
            : "file";
}

/**
 * @brief Get the fragment identifier.
 *
 * @return the fragment identifier, including the leading '#', or an empty
 *      string if there is no fragment identifier.
 */
const std::string doc2::url_modifier() const {
    using std::string;
    string::size_type lastHashPos = this->url_.find_last_of('#');
    return (lastHashPos != string::npos)
            ? this->url_.substr(lastHashPos)
            : "";
}

/**
 * @brief Get the fully qualified name of a local file that is the downloaded
 *      resource at @a url_.
 *
 * @return the fully qualified name of a local file that is the downloaded
 *      resource at @a url_.
 */
const char * doc2::local_name() {
    static char buf[1024];
    if (filename(buf, sizeof(buf))) { return buf; }
    return 0;
}

/**
 * @brief Get the path of the local file that is the downloaded resource at
 *      @a url_.
 *
 * @return the path of the local file that is the downloaded resource at
 *      @a url_.
 */
const char * doc2::local_path() {
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
std::istream & doc2::input_stream() {
    if (!this->istm_) {

        char fn[256];

        this->filename(fn, sizeof(fn));
        if (strcmp(fn, "-") == 0) {
            this->istm_ = &std::cin;
        } else {
# ifdef OPENVRML_ENABLE_GZIP
            this->istm_ = new z::ifstream(fn);
# else
            this->istm_ = new std::ifstream(fn);
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
std::ostream & doc2::output_stream() {
    if (!ostm_) {
        ostm_ = new std::ofstream(stripProtocol(url_).c_str(), std::ios::out);
    }
    return *this->ostm_;
}

/**
 * @brief Converts a url into a local filename.
 *
 * @retval fn   a character buffer to hold the local filename.
 * @param nfn   the number of elements in the buffer @p fn points to.
 */
bool doc2::filename(char * fn, const size_t nfn) {
    using std::copy;
    using std::string;

    fn[0] = '\0';

    const char * s = 0;

    const std::string protocol = this->url_protocol();

    if (protocol == "file") {
# ifdef _WIN32
        string name = URI(this->url_).getPath().substr(1);
# else
        string name = URI(this->url_).getPath();
# endif
        size_t len = (name.length() < (nfn - 1))
                   ? name.length()
                   : nfn - 1;
        copy(name.begin(), name.begin() + len, fn);
        fn[len] = '\0';
        return true;
    } else if (protocol == "http") {
        //
        // Get a local copy of http files.
        //
        if (this->tmpfile_) {    // Already fetched it
            s = this->tmpfile_;
        } else if ((s = the_system->http_fetch(this->url_.c_str()))) {
            tmpfile_ = new char[strlen(s)+1];
            strcpy(tmpfile_, s);
            free(const_cast<char *>(s));        // assumes tempnam or equiv...
            s = tmpfile_;
        }
    }
    // Unrecognized protocol (need ftp here...)
    else {
        s = 0;
    }

    if (s) {
        strncpy( fn, s, nfn-1 );
        fn[nfn-1] = '\0';
    }

    return s && *s;
}

namespace {

    const char * const expression =
            "^(([^:/?#]+):)?((//([^/?#]*))?([^?#]*)([?]([^#]*))?(#(.*))?)";
        //    12            34  5          6       7   8        9 10
    class URIRegex {
        regex_t regex;

    public:
        URIRegex() throw (std::bad_alloc);
        ~URIRegex() throw ();

        int exec(const char * str, size_t nmatch, regmatch_t pmatch[],
                 int eflags);
    };

    URIRegex::URIRegex() throw (std::bad_alloc) {
        int err = regcomp(&this->regex, expression, REG_EXTENDED);
        if (err == REG_ESPACE) { throw std::bad_alloc(); }
        assert(err == 0);
    }

    URIRegex::~URIRegex() throw () { regfree(&this->regex); }

    int URIRegex::exec(const char * str, size_t nmatch, regmatch_t pmatch[],
                       int eflags) {
        return regexec(&this->regex, str, nmatch, pmatch, eflags);
    }

    URIRegex uriRegex;

    URI::URI(const std::string & str) throw (invalid_url, std::bad_alloc):
            str(str) {
        int err = uriRegex.exec(str.c_str(), URI::nmatch, this->regmatch, 0);
        if (err != 0) { throw invalid_url(); }
    }

    URI::operator std::string() const throw (std::bad_alloc) {
        return this->str;
    }

    const std::string URI::getScheme() const throw (std::bad_alloc) {
        return (this->regmatch[2].rm_so > -1)
                ? this->str.substr(this->regmatch[2].rm_so,
                                   this->regmatch[2].rm_eo - this->regmatch[2].rm_so)
                : std::string();
    }

    const std::string URI::getSchemeSpecificPart() const
            throw (std::bad_alloc) {
        return (this->regmatch[3].rm_so > -1)
                ? this->str.substr(this->regmatch[3].rm_so,
                                   this->regmatch[3].rm_eo - this->regmatch[3].rm_so)
                : std::string();
    }

    const std::string URI::getAuthority() const throw (std::bad_alloc) {
        return (this->regmatch[4].rm_so > -1)
                ? this->str.substr(this->regmatch[4].rm_so,
                                   this->regmatch[4].rm_eo - this->regmatch[4].rm_so)
                : std::string();
    }

    const std::string URI::getUserinfo() const throw (std::bad_alloc) {
        return std::string();
    }

    const std::string URI::getHost() const throw (std::bad_alloc) {
        return std::string();
    }

    const std::string URI::getPort() const throw (std::bad_alloc) {
        return std::string();
    }

    const std::string URI::getPath() const throw (std::bad_alloc) {
        return (this->regmatch[6].rm_so > -1)
                ? this->str.substr(this->regmatch[6].rm_so,
                                   this->regmatch[6].rm_eo - this->regmatch[6].rm_so)
                : std::string();
    }

    const std::string URI::getQuery() const throw (std::bad_alloc) {
        return (this->regmatch[7].rm_so > -1)
                ? this->str.substr(this->regmatch[7].rm_so,
                                   this->regmatch[7].rm_eo - this->regmatch[7].rm_so)
                : std::string();
    }

    const std::string URI::getFragment() const throw (std::bad_alloc) {
        return (this->regmatch[10].rm_so > -1)
                ? this->str.substr(this->regmatch[10].rm_so,
                                   this->regmatch[10].rm_eo - this->regmatch[10].rm_so)
                : std::string();
    }

    const URI URI::resolveAgainst(const URI & absoluteURI) const
            throw (std::bad_alloc) {
        using std::list;
        using std::string;

        assert(this->getScheme().empty());
        assert(!absoluteURI.getScheme().empty());

        string result = absoluteURI.getScheme() + ':';

        string authority = this->getAuthority();
        if (!authority.empty()) {
            authority = absoluteURI.getAuthority();
        }
        if (!authority.empty()) {
            result += "//" + authority;
        }

        string path = absoluteURI.getPath();
        const string::size_type lastSlashIndex = path.find_last_of('/');

        //
        // Chop off the leading slash and the last path segment (typically a
        // file name).
        //
        path = path.substr(1, lastSlashIndex);

        //
        // Append the relative path.
        //
        path += this->getPath();

        //
        // Put the path segments in a list to process them.
        //
        list<string> pathSegments;
        string::size_type slashIndex = 0;
        string::size_type segmentStartIndex = 0;
        while ((slashIndex = path.find('/', segmentStartIndex))) {
            string segment = path.substr(segmentStartIndex,
                                         slashIndex - segmentStartIndex);
            if (!segment.empty()) {
                pathSegments.push_back(segment);
            }
            segmentStartIndex = slashIndex + 1;
        }

        //
        // Remove any "." segments.
        //
        pathSegments.remove(".");

        //
        // Remove any ".." segments along with the segment that precedes them.
        //
        for (std::list<string>::iterator pos; pos != pathSegments.end(); ++pos) {
            if (pos != pathSegments.begin() && *pos == "..") {
                --(pos = pathSegments.erase(pos));
                --(pos = pathSegments.erase(pos));
            }
        }

        //
        // Reconstruct the path.
        //
        path = "";
        for (std::list<string>::iterator cpos; cpos != pathSegments.end(); ++cpos) {
            path += '/' + *cpos;
        }

        //
        // End in a slash?
        //
        if (*(this->getPath().end() - 1) == '/') { path += '/'; }

        result += path;
        result += this->getQuery();
        result += this->getFragment();

        return URI(result);
    }
}

} // namespace openvrml
