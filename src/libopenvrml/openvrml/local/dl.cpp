// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2008  Braden McDaniel
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
# include <string>

namespace {
}

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

    struct OPENVRML_LOCAL win32_search_path_tokenizer {
        win32_search_path_tokenizer()
        {}

        template <typename Iterator, typename Token>
            bool operator()(Iterator & next, Iterator end, Token & tok)
        {
            while (next != end) {
                if (*next == ';') {
                    ++next;
                    break;
                }
                this->tok_.push_back(*next);
                ++next;
            }
            if (!this->tok_.empty()) {
                tok = this->tok_;
                this->tok_.clear();
                return true;
            }
            return false;
        }

        void reset()
        {
            this->tok_.clear();
        }

    private:
        std::string tok_;
    };
}

int
openvrml::local::dl::foreachfile(const char * search_path,
                                 int (*func)(const char * filename, void * data),
                                 void * data)
{
# ifdef _WIN32
    using boost::filesystem::path;
    using boost::filesystem::directory_iterator;
    typedef boost::tokenizer<win32_search_path_tokenizer> tokenizer_t;

    std::vector<path> search_dirs;
    win32_search_path_tokenizer tokenizer_func;
    std::string search_path_str(search_path);
    tokenizer_t tokenizer(search_path_str, tokenizer_func);
    for (tokenizer_t::const_iterator token = tokenizer.begin();
         token != tokenizer.end();
         ++token) {
        search_dirs.push_back(path(*token));
    }

    int result = 0;
    for (std::vector<path>::const_iterator dir = search_dirs.begin();
         dir != search_dirs.end();
         ++dir) try {
            for (directory_iterator entry(*dir);
                 entry != directory_iterator();
                 ++entry) {
                result = (func)(entry->path().external_file_string().c_str(),
                                data);
                if (result != 0) { return result; }
            }
        } catch (boost::filesystem::filesystem_error &) {}
    return result;
# else
    return lt_dlforeachfile(search_path, func, data);
# endif
}

openvrml::local::dl::handle openvrml::local::dl::open(const char * filename)
{
# ifdef _WIN32
    const char * last_dot = strrchr(filename, '.');
    if (strcmp(last_dot, ".dll") != 0) { return 0; }
    return LoadLibrary(filename);
# else
    return lt_dlopenext(filename);
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

void * openvrml::local::dl::sym(handle h, const char * name)
{
# ifdef _WIN32
    return GetProcAddress(h, name);
# else
    return lt_dlsym(h, name);
# endif
}
