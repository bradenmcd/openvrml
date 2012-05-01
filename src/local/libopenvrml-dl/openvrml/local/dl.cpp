// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2008, 2009, 2010  Braden McDaniel
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

# include "dl.h"
# include <boost/filesystem.hpp>
# include <boost/ref.hpp>
# include <boost/scope_exit.hpp>
# include <boost/tokenizer.hpp>
# include <sstream>

int openvrml::local::dl::init()
{
# ifdef _WIN32
    return 0;
# else
    return lt_dlinit();
# endif
}

int openvrml::local::dl::exit()
{
# ifdef _WIN32
    return 0;
# else
    return lt_dlexit();
# endif
}

namespace {
    struct ltforeachfile_data {
        int (*func)(const std::string & filename, void * data);
        void * data;
    };
}

extern "C"
OPENVRML_LOCAL
int openvrml_dl_ltforeachfile(const char * filename, void * data)
{
    ltforeachfile_data * const d = static_cast<ltforeachfile_data *>(data);
    return (d->func)(filename, d->data);
}

int
openvrml::local::dl::
foreachfile(const std::vector<boost::filesystem::path> & search_path,
            int (*func)(const std::string & filename, void * data),
            void * data)
{
# ifdef _WIN32
    using boost::filesystem::path;
    using boost::filesystem::directory_iterator;

    int result = 0;
    for (std::vector<path>::const_iterator dir = search_path.begin();
         dir != search_path.end();
         ++dir) try {
        for (directory_iterator entry(*dir);
             entry != directory_iterator();
             ++entry) {
            result = (func)(entry->path().generic_string(), data);
            if (result != 0) { return result; }
        }
    } catch (boost::filesystem::filesystem_error &) {}
    return result;
# else
    std::ostringstream path;
    std::vector<boost::filesystem::path>::const_iterator dir =
        search_path.begin();
    while (dir != search_path.end()) {
        path << dir->generic_string();
        if (++dir != search_path.end()) { path << LT_PATHSEP_CHAR; }
    }
    ltforeachfile_data ltdata = { func, data };
    return lt_dlforeachfile(path.str().c_str(),
                            openvrml_dl_ltforeachfile,
                            &ltdata);
# endif
}

/**
 * @internal
 *
 * @brief Prepend @p dir to the dynamic library search path.
 */
int
openvrml::local::dl::prepend_to_searchpath(const boost::filesystem::path & dir)
{
    const std::string & dir_str = dir.string();
    const char * const dir_c_str = dir_str.c_str();
# ifdef _WIN32
    return SetDllDirectory(dir_c_str);
# else
    const char * const before = lt_dlgetsearchpath();
    return lt_dlinsertsearchdir(before, dir_c_str);
# endif
}

openvrml::local::dl::handle
openvrml::local::dl::open(const std::string & filename)
{
# ifdef _WIN32
    std::string::size_type last_dot_index = filename.find_last_of('.');
    if (filename.substr(last_dot_index) != ".dll") { return 0; }
    return LoadLibrary(filename.c_str());
# else
    return lt_dlopenext(filename.c_str());
# endif
}

const std::string openvrml::local::dl::error()
{
# ifdef _WIN32
    const DWORD err = GetLastError();
    char * buf = 0;
    BOOST_SCOPE_EXIT((buf)) {
        LocalFree(buf);
    } BOOST_SCOPE_EXIT_END
    static const LPCVOID source = 0;
    static const DWORD buf_size = 0;
    static va_list * const args = 0;
    const DWORD buf_chars = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
                                          | FORMAT_MESSAGE_FROM_SYSTEM,
                                          source,
                                          err,
                                          LANG_USER_DEFAULT,
                                          reinterpret_cast<LPSTR>(&buf),
                                          buf_size,
                                          args);
    assert(buf_chars != DWORD(0)); // If FormatMessage failed, just give up.
    const std::string buf_str(buf ? buf : "");
    return buf_str;
# else
    return lt_dlerror();
# endif
}

int openvrml::local::dl::close(handle h)
{
# ifdef _WIN32
    return FreeLibrary(h);
# else
    return lt_dlclose(h);
# endif
}

void * openvrml::local::dl::sym(handle h, const std::string & name)
{
# ifdef _WIN32
    return GetProcAddress(h, name.c_str());
# else
    return lt_dlsym(h, name.c_str());
# endif
}
