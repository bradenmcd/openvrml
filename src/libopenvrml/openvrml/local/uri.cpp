// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2008, 2010  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# include "uri.h"
# include <list>
# include <cerrno>
# include <cstring>
# ifdef _WIN32
#   include <boost/scope_exit.hpp>
# endif

openvrml::local::uri::uri() OPENVRML_THROW1(std::bad_alloc):
    scheme_begin(this->str_.begin()),
    scheme_end(this->str_.begin()),
    scheme_specific_part_begin(this->str_.begin()),
    scheme_specific_part_end(this->str_.begin()),
    authority_begin(this->str_.begin()),
    authority_end(this->str_.begin()),
    userinfo_begin(this->str_.begin()),
    userinfo_end(this->str_.begin()),
    host_begin(this->str_.begin()),
    host_end(this->str_.begin()),
    port_begin(this->str_.begin()),
    port_end(this->str_.begin()),
    path_begin(this->str_.begin()),
    path_end(this->str_.begin()),
    query_begin(this->str_.begin()),
    query_end(this->str_.begin()),
    fragment_begin(this->str_.begin()),
    fragment_end(this->str_.begin())
{}

openvrml::local::uri::uri(const std::string & str)
    OPENVRML_THROW2(openvrml::invalid_url, std::bad_alloc):
    str_(str),
    scheme_begin(this->str_.begin()),
    scheme_end(this->str_.begin()),
    scheme_specific_part_begin(this->str_.begin()),
    scheme_specific_part_end(this->str_.begin()),
    authority_begin(this->str_.begin()),
    authority_end(this->str_.begin()),
    userinfo_begin(this->str_.begin()),
    userinfo_end(this->str_.begin()),
    host_begin(this->str_.begin()),
    host_end(this->str_.begin()),
    port_begin(this->str_.begin()),
    port_end(this->str_.begin()),
    path_begin(this->str_.begin()),
    path_end(this->str_.begin()),
    query_begin(this->str_.begin()),
    query_end(this->str_.begin()),
    fragment_begin(this->str_.begin()),
    fragment_end(this->str_.begin())
{
    using std::string;
    using namespace boost::spirit::classic;

    actions a(*this);
    uri_grammar<actions> g(a);

    string::const_iterator begin = this->str_.begin();
    string::const_iterator end = this->str_.end();

    if (!parse(begin, end, g, space_p).full) {
        throw openvrml::invalid_url();
    }
}

openvrml::local::uri::uri(const uri & id) OPENVRML_THROW1(std::bad_alloc):
    str_(id.str_),
    scheme_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                    id.scheme_begin)),
    scheme_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                  id.scheme_end)),
    scheme_specific_part_begin(
        this->str_.begin() + std::distance(id.str_.begin(),
                                           id.scheme_specific_part_begin)),
    scheme_specific_part_end(
        this->str_.begin() + std::distance(id.str_.begin(),
                                           id.scheme_specific_part_end)),
    authority_begin(this->str_.begin()
                    + std::distance(id.str_.begin(), id.authority_begin)),
    authority_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                     id.authority_end)),
    userinfo_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                      id.userinfo_begin)),
    userinfo_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                    id.userinfo_end)),
    host_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                  id.host_begin)),
    host_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                id.host_end)),
    port_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                  id.port_begin)),
    port_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                id.port_end)),
    path_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                  id.path_begin)),
    path_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                id.path_end)),
    query_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                   id.query_begin)),
    query_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                 id.query_end)),
    fragment_begin(this->str_.begin() + std::distance(id.str_.begin(),
                                                      id.fragment_begin)),
    fragment_end(this->str_.begin() + std::distance(id.str_.begin(),
                                                    id.fragment_end))
{}

openvrml::local::uri & openvrml::local::uri::operator=(const uri & id)
    OPENVRML_THROW1(std::bad_alloc)
{
    uri temp(id);
    this->swap(temp);
    return *this;
}

openvrml::local::uri::operator const std::string() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return this->str_;
}

void openvrml::local::uri::swap(uri & id) OPENVRML_NOTHROW
{
    using std::distance;
    using std::string;
    using std::swap;

    typedef std::iterator_traits<string::const_iterator>::difference_type
        diff_t;

    const diff_t id_scheme_begin_distance =
        distance<string::const_iterator>(id.str_.begin(), id.scheme_begin);
    const diff_t id_scheme_end_distance =
        distance<string::const_iterator>(id.str_.begin(), id.scheme_end);
    const diff_t id_scheme_specific_part_begin_distance =
        distance<string::const_iterator>(id.str_.begin(),
                                         id.scheme_specific_part_begin);
    const diff_t id_scheme_specific_part_end_distance =
        distance<string::const_iterator>(id.str_.begin(),
                                         id.scheme_specific_part_end);
    const diff_t id_authority_begin_distance =
        distance<string::const_iterator>(id.str_.begin(), id.authority_begin);
    const diff_t id_authority_end_distance =
        distance<string::const_iterator>(id.str_.begin(), id.authority_end);
    const diff_t id_userinfo_begin_distance =
        distance<string::const_iterator>(id.str_.begin(), id.userinfo_begin);
    const diff_t id_userinfo_end_distance =
        distance<string::const_iterator>(id.str_.begin(), id.userinfo_end);
    const diff_t id_host_begin_distance =
        distance<string::const_iterator>(id.str_.begin(), id.host_begin);
    const diff_t id_host_end_distance =
        distance<string::const_iterator>(id.str_.begin(), id.host_end);
    const diff_t id_port_begin_distance =
        distance<string::const_iterator>(id.str_.begin(), id.port_begin);
    const diff_t id_port_end_distance =
        distance<string::const_iterator>(id.str_.begin(), id.port_end);
    const diff_t id_path_begin_distance =
        distance<string::const_iterator>(id.str_.begin(), id.path_begin);
    const diff_t id_path_end_distance =
        distance<string::const_iterator>(id.str_.begin(), id.path_end);
    const diff_t id_query_begin_distance =
        distance<string::const_iterator>(id.str_.begin(), id.query_begin);
    const diff_t id_query_end_distance =
        distance<string::const_iterator>(id.str_.begin(), id.query_end);
    const diff_t id_fragment_begin_distance =
        distance<string::const_iterator>(id.str_.begin(), id.fragment_begin);
    const diff_t id_fragment_end_distance =
        distance<string::const_iterator>(id.str_.begin(), id.fragment_end);

    const diff_t this_scheme_begin_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->scheme_begin);
    const diff_t this_scheme_end_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->scheme_end);
    const diff_t this_scheme_specific_part_begin_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->scheme_specific_part_begin);
    const diff_t this_scheme_specific_part_end_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->scheme_specific_part_end);
    const diff_t this_authority_begin_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->authority_begin);
    const diff_t this_authority_end_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->authority_end);
    const diff_t this_userinfo_begin_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->userinfo_begin);
    const diff_t this_userinfo_end_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->userinfo_end);
    const diff_t this_host_begin_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->host_begin);
    const diff_t this_host_end_distance =
        distance<string::const_iterator>(this->str_.begin(), this->host_end);
    const diff_t this_port_begin_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->port_begin);
    const diff_t this_port_end_distance =
        distance<string::const_iterator>(this->str_.begin(), this->port_end);
    const diff_t this_path_begin_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->path_begin);
    const diff_t this_path_end_distance =
        distance<string::const_iterator>(this->str_.begin(), this->path_end);
    const diff_t this_query_begin_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->query_begin);
    const diff_t this_query_end_distance =
        distance<string::const_iterator>(this->str_.begin(), this->query_end);
    const diff_t this_fragment_begin_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->fragment_begin);
    const diff_t this_fragment_end_distance =
        distance<string::const_iterator>(this->str_.begin(),
                                         this->fragment_end);

    swap(this->str_, id.str_);

    id.scheme_begin = id.str_.begin() + this_scheme_begin_distance;
    id.scheme_end = id.str_.begin() + this_scheme_end_distance;
    id.scheme_specific_part_begin =
        id.str_.begin() + this_scheme_specific_part_begin_distance;
    id.scheme_specific_part_end =
        id.str_.begin() + this_scheme_specific_part_end_distance;
    id.authority_begin = id.str_.begin() + this_authority_begin_distance;
    id.authority_end = id.str_.begin() + this_authority_end_distance;
    id.userinfo_begin = id.str_.begin() + this_userinfo_begin_distance;
    id.userinfo_end = id.str_.begin() + this_userinfo_end_distance;
    id.host_begin = id.str_.begin() + this_host_begin_distance;
    id.host_end = id.str_.begin() + this_host_end_distance;
    id.port_begin = id.str_.begin() + this_port_begin_distance;
    id.port_end = id.str_.begin() + this_port_end_distance;
    id.path_begin = id.str_.begin() + this_path_begin_distance;
    id.path_end = id.str_.begin() + this_path_end_distance;
    id.query_begin = id.str_.begin() + this_query_begin_distance;
    id.query_end = id.str_.begin() + this_query_end_distance;
    id.fragment_begin = id.str_.begin() + this_fragment_begin_distance;
    id.fragment_end = id.str_.begin() + this_fragment_end_distance;

    this->scheme_begin = this->str_.begin() + id_scheme_begin_distance;
    this->scheme_end = this->str_.begin() + id_scheme_end_distance;
    this->scheme_specific_part_begin =
        this->str_.begin() + id_scheme_specific_part_begin_distance;
    this->scheme_specific_part_end =
        this->str_.begin() + id_scheme_specific_part_end_distance;
    this->authority_begin =
        this->str_.begin() + id_authority_begin_distance;
    this->authority_end = this->str_.begin() + id_authority_end_distance;
    this->userinfo_begin =
        this->str_.begin() + id_userinfo_begin_distance;
    this->userinfo_end = this->str_.begin() + id_userinfo_end_distance;
    this->host_begin = this->str_.begin() + id_host_begin_distance;
    this->host_end = this->str_.begin() + id_host_end_distance;
    this->port_begin = this->str_.begin() + id_port_begin_distance;
    this->port_end = this->str_.begin() + id_port_end_distance;
    this->path_begin = this->str_.begin() + id_path_begin_distance;
    this->path_end = this->str_.begin() + id_path_end_distance;
    this->query_begin = this->str_.begin() + id_query_begin_distance;
    this->query_end = this->str_.begin() + id_query_end_distance;
    this->fragment_begin =
        this->str_.begin() + id_fragment_begin_distance;
    this->fragment_end = this->str_.begin() + id_fragment_end_distance;
}

const std::string openvrml::local::uri::scheme() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return std::string(this->scheme_begin, this->scheme_end);
}

const std::string openvrml::local::uri::scheme_specific_part() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return std::string(this->scheme_specific_part_begin,
                       this->scheme_specific_part_end);
}

const std::string openvrml::local::uri::authority() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return std::string(this->authority_begin, this->authority_end);
}

const std::string openvrml::local::uri::userinfo() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return std::string(this->userinfo_begin, this->userinfo_end);
}

const std::string openvrml::local::uri::host() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return std::string(this->host_begin, this->host_end);
}

const std::string openvrml::local::uri::port() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return std::string(this->port_begin, this->port_end);
}

const std::string openvrml::local::uri::path() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return std::string(this->path_begin, this->path_end);
}

const std::string openvrml::local::uri::query() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return std::string(this->query_begin, this->query_end);
}

const std::string openvrml::local::uri::fragment() const
    OPENVRML_THROW1(std::bad_alloc)
{
    return std::string(this->fragment_begin, this->fragment_end);
}

const openvrml::local::uri
openvrml::local::resolve_against(const uri & relative_uri,
                                 const uri & absolute_uri)
    OPENVRML_THROW1(std::bad_alloc)
{
    using std::list;
    using std::string;
    using std::ostringstream;

    assert(relative(relative_uri));
    assert(!relative(absolute_uri));

    ostringstream result;
    result.unsetf(ostringstream::skipws);
    result << absolute_uri.scheme() << ':';

    if (!relative_uri.authority().empty()) {
        result << relative_uri.scheme_specific_part();
        return uri(result.str());
    } else {
        result << "//" << absolute_uri.authority();
    }

    string abs_uri_path = absolute_uri.path();
    if (abs_uri_path.empty()) { abs_uri_path = "/"; }
    const string::size_type last_slash_index =
        abs_uri_path.find_last_of('/');

    //
    // Chop off the leading slash and the last path segment (typically a
    // file name).
    //
    ostringstream path;
    path.unsetf(ostringstream::skipws);
    path << abs_uri_path.substr(1, last_slash_index);

    //
    // Append the relative path.
    //
    path << relative_uri.path();

    //
    // Put the path segments in a list to process them.
    //
    list<string> path_segments;
    string::size_type slash_index = 0;
    string::size_type segment_start_index = 0;
    do {
        slash_index = path.str().find('/', segment_start_index);
        string segment =
            path.str().substr(segment_start_index,
                              slash_index - segment_start_index);
        if (!segment.empty()) {
            path_segments.push_back(segment);
        }
        segment_start_index = slash_index + 1;
    } while (slash_index != string::npos);

    //
    // Remove any "." segments.
    //
    path_segments.remove(".");

    //
    // Remove any ".." segments along with the segment that precedes them.
    //
    list<string>::iterator pos = path_segments.begin();
    while (pos != path_segments.end()) {
        if (*pos != "..") { ++pos; continue; }
        //
        // If pos == begin after the first erase operation, we're
        // dealing with a bogus path.  The best we can do is Not
        // Crash.  That means we'll emit *some* absolute URI from this
        // function; but it may not be very useful.
        //
        pos = path_segments.erase(pos);
        if (pos == path_segments.begin()) { continue; }
        pos = path_segments.erase(--pos);
    }

    //
    // Reconstruct the path.
    //
    path.clear();
    path.str(string());
    for (list<string>::const_iterator pos = path_segments.begin();
         pos != path_segments.end();
         ++pos) {
        path << '/' << *pos;
    }

    //
    // End in a slash?
    //
    if (*(relative_uri.path().end() - 1) == '/') { path << '/'; }

    result << path.str();

    const string query = relative_uri.query();
    if (!query.empty()) { result << '?' << query; }

    const string fragment = relative_uri.fragment();
    if (!fragment.empty()) { result << '#' << fragment; }

    uri result_uri;
    try {
        result_uri = uri(result.str());
    } catch (openvrml::invalid_url &) {
        assert(false); // If we constructed a bad URI, something is wrong.
    }

    return result_uri;
}

/**
 * @internal
 *
 * @brief Create an absolute &ldquo;file&rdquo; URL from a relative path.
 *
 * This function constructs a syntactically valid &ldquo;file&rdquo; URL;
 * it does not check to see whether the file actually exists.
 *
 * @param[in] relative_uri  a relative URI.
 *
 * @return an absolute &ldquo;file&rdquo; URL corresponding to
 *         @p relative_uri.
 *
 * @exception std::runtime_error    if the current working directory
 *                                  cannot be determined.
 * @exception std::bad_alloc        if memory allocation fails.
 *
 * @sa ftp://ftp.rfc-editor.org/in-notes/rfc1738.txt
 */
const openvrml::local::uri
openvrml::local::create_file_url(const uri & relative_uri)
    OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
{
    assert(relative(relative_uri));

    using std::string;
    using std::ostringstream;

    ostringstream base_uri;
    base_uri.unsetf(ostringstream::skipws);
    base_uri << "file://";

    const string relative_path = relative_uri.path();

    if (!relative_path.empty() && relative_path[0] == '/') {
        base_uri << relative_path;
        return uri(base_uri.str());
    }

    std::string cwd = getcwd();
# ifdef _WIN32
    //
    // Path must begin with a slash.  openvrml_getcwd returns something
    // that begins with a drive letter on Windows.
    //
    cwd = '/' + cwd;

    //
    // Replace backslashes with (forward) slashes.
    //
    std::replace_if(cwd.begin() + 1, cwd.end(),
                    std::bind2nd(std::equal_to<char>(), '\\'), '/');
# endif
    base_uri << cwd;

    //
    // Don't trust getcwd implementations not to vary on this--add a
    // trailing slash if there isn't one.
    //
    if (base_uri.str()[base_uri.str().length() - 1] != '/') {
        base_uri << '/';
    }

    const uri result = resolve_against(relative_uri, uri(base_uri.str()));
    return result;
}

/**
 * @internal
 *
 * @brief Get the current directory.
 *
 * This function wraps @c getcwd on POSIX and @c GetCurrentDirectory on
 * Windows.
 *
 * @return the current directory.
 *
 * @exception std::runtime_error   if getcwd or GetCurrentDirectory raise
 *                                 some arbitrary error.
 * @exception std::bad_alloc       if memory allocation fails.
 */
const std::string openvrml::local::getcwd()
    OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
{
# ifdef _WIN32
    std::vector<char> curdir_buf(MAX_PATH);
    DWORD result = 0;
    while ((result = GetCurrentDirectory(curdir_buf.size(),
                                         &curdir_buf.front()))
           > curdir_buf.size()) {
        curdir_buf.resize(result);
    }
    if (result == 0) {
        using boost::ref;
        DWORD error = GetLastError();
        void * msgBuf = 0;
        BOOST_SCOPE_EXIT((&msgBuf)) {
            LocalFree(msgBuf);
        } BOOST_SCOPE_EXIT_END
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
                      | FORMAT_MESSAGE_FROM_SYSTEM
                      | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      error,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      reinterpret_cast<LPSTR>(&msgBuf),
                      0, NULL);
        throw std::runtime_error(static_cast<char *>(msgBuf));
    }
    //
    // GetCurrentDirectory isn't guaranteed to include a drive letter.
    //
    std::vector<char> buf(MAX_PATH);
    while ((result = GetFullPathName(&curdir_buf.front(),
                                     buf.size(),
                                     &buf.front(),
                                     NULL))
           > buf.size()) {
        buf.resize(result);
    }
    if (result == 0) {
        using boost::ref;
        DWORD error = GetLastError();
        void * msgBuf = 0;
        BOOST_SCOPE_EXIT((&msgBuf)) {
            LocalFree(msgBuf);
        } BOOST_SCOPE_EXIT_END
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
                      | FORMAT_MESSAGE_FROM_SYSTEM
                      | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      error,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      reinterpret_cast<LPSTR>(&msgBuf),
                      0, NULL);
        throw std::runtime_error(static_cast<char *>(msgBuf));
    }
# else
    char * result = 0;
    std::vector<char> buf(PATH_MAX);
    while (!(result = ::getcwd(&buf.front(), buf.size()))
           && (errno == ERANGE)) {
        buf.resize(buf.size() * 2);
    }
    if (result == 0) {
        assert(errno != 0);
        assert(errno != EFAULT);
        assert(errno != EINVAL);
        throw std::runtime_error(std::strerror(errno));
    }
# endif
    return std::string(&buf.front());
}
