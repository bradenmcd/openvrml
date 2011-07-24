// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2009, 2010  Braden McDaniel
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

# include "conf.h"
# include "error.h"
# include <boost/ref.hpp>
# include <boost/tokenizer.hpp>
# ifdef _WIN32
#   include <windows.h>
#   include <vector>
# else
#   include <cstdlib>
# endif

namespace {

    class OPENVRML_LOCAL no_environment_var : public std::runtime_error {
    public:
        explicit no_environment_var(const std::string & var);
    };

    no_environment_var::no_environment_var(const std::string & var):
        std::runtime_error("no environment variable \"" + var + "\"")
    {}

    OPENVRML_LOCAL const std::string get_env(const std::string & var)
        OPENVRML_THROW1(no_environment_var)
    {
# ifdef _WIN32
        DWORD size;
        std::vector<char> buf(128);
        while ((size = GetEnvironmentVariable(var.c_str(),
                                              &buf.front(),
                                              buf.size()))
               && size > buf.size()) {
            buf.resize(size);
        }
        if (size == 0) { throw no_environment_var(var); }
        return &buf.front();
# else
        const char * val = std::getenv(var.c_str());
        if (!val) { throw no_environment_var(var); }
        return val;
# endif
    }

# ifdef _WIN32
    const std::string query_registry_value(HKEY key, const std::string & name)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        using openvrml::local::throw_runtime_error_from_win32_system_error;

        LONG result;
        static DWORD * const reserved;
        DWORD type;
        std::vector<BYTE> data(64);
        DWORD size = data.size();
        while ((result = RegQueryValueEx(key,
                                         name.c_str(),
                                         reserved,
                                         &type,
                                         &data.front(),
                                         &size))
               == ERROR_MORE_DATA) {
            data.resize(data.size() * 2);
        }

        if (result != 0) {
            throw_runtime_error_from_win32_system_error(result);
        }

        _ASSERTE(type == REG_SZ);

        return std::string(&data.front(), &data.front() + size - 1);
    }

    class no_registry_key : public std::runtime_error {
    public:
        no_registry_key(): std::runtime_error("no registry key") {}
    };

    const std::string get_registry_setting(const std::string & name)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        using openvrml::local::throw_runtime_error_from_win32_system_error;

        static const ULONG options = 0;
        HKEY key;
        LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                   "SOFTWARE\\OpenVRML\\OpenVRML",
                                   options,
                                   KEY_READ,
                                   &key);
        if (result != ERROR_SUCCESS) {
            if (result == ERROR_FILE_NOT_FOUND) { throw no_registry_key(); }
            throw_runtime_error_from_win32_system_error(result);
        }

        return query_registry_value(key, name);
    }
# endif
}

const boost::filesystem::path openvrml::local::conf::datadir()
    OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
{
    boost::filesystem::path result;
    try {
        result = get_env("OPENVRML_DATADIR");
    } catch (const no_environment_var &) {
# ifdef _WIN32
        result = get_registry_setting("Datadir");
# else
        result = OPENVRML_PKGDATADIR_;
# endif
    }
    return result;
}

namespace {
    const char path_separator =
# ifdef _WIN32
        ';';
# else
        ':';
# endif

    struct OPENVRML_LOCAL search_path_tokenizer {
        template <typename Iterator, typename Token>
            bool operator()(Iterator & next, Iterator end, Token & tok)
        {
            while (next != end) {
                if (*next == path_separator) {
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

const std::vector<boost::filesystem::path> openvrml::local::conf::node_path()
    OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
{
    using std::back_inserter;
    using std::copy;
    using boost::filesystem::path;
    typedef boost::tokenizer<search_path_tokenizer> tokenizer_t;

    std::vector<path> result;
    search_path_tokenizer tokenizer_func;

    std::string env_path;
    try {
        env_path = get_env("OPENVRML_NODE_PATH");
    } catch (const no_environment_var &) {}

    tokenizer_t env_path_tokenizer(env_path, tokenizer_func);
    copy(env_path_tokenizer.begin(), env_path_tokenizer.end(),
         back_inserter(result));

    std::string system_path;
# ifdef _WIN32
    try {
        system_path = get_registry_setting("NodePath");
    } catch (no_registry_key &) {}
# else
    system_path = OPENVRML_PKGLIBDIR_ "/node";
# endif

    tokenizer_t system_path_tokenizer(system_path, tokenizer_func);
    copy(system_path_tokenizer.begin(), system_path_tokenizer.end(),
         back_inserter(result));

    return result;
}

const std::vector<boost::filesystem::path> openvrml::local::conf::script_path()
    OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
{
    using std::back_inserter;
    using std::copy;
    using boost::filesystem::path;
    typedef boost::tokenizer<search_path_tokenizer> tokenizer_t;

    std::vector<path> result;
    search_path_tokenizer tokenizer_func;

    std::string env_path;
    try {
        env_path = get_env("OPENVRML_SCRIPT_PATH");
    } catch (const no_environment_var &) {}

    tokenizer_t env_path_tokenizer(env_path, tokenizer_func);
    copy(env_path_tokenizer.begin(), env_path_tokenizer.end(),
         back_inserter(result));

    std::string system_path;
# ifdef _WIN32
    try {
        system_path = get_registry_setting("ScriptPath");
    } catch (no_registry_key &) {}
# else
    system_path = OPENVRML_PKGLIBDIR_ "/script";
# endif

    tokenizer_t system_path_tokenizer(system_path, tokenizer_func);
    copy(system_path_tokenizer.begin(), system_path_tokenizer.end(),
         back_inserter(result));

    return result;
}
