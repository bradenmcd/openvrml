// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2009  Braden McDaniel
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
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/ref.hpp>
# ifdef _WIN32
#   include <windows.h>
#   include <vector>
# else
#   include <cstdlib>
# endif

using namespace boost::multi_index::detail;  // for scope_guard

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
    void throw_runtime_error(LONG result) OPENVRML_THROW1(std::runtime_error)
    {
        using boost::ref;

        static const LPCVOID source;
        LPTSTR buf = 0;
        scope_guard buf_guard = make_guard(LocalFree, ref(buf));
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
                      | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                      source,
                      result,
                      LANG_USER_DEFAULT,
                      reinterpret_cast< LPTSTR >(&buf),
                      0,
                      0);

        throw std::runtime_error(buf);
    }

    const std::string query_registry_value(HKEY key, const std::string & name)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
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

        if (result != 0) { throw_runtime_error(result); }

        _ASSERTE(type == REG_SZ);

        return std::string(&data.front(), &data.front() + size - 1);
    }

    const std::string get_registry_setting(const std::string & name)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        static const ULONG options = 0;
        HKEY key;
        LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                   "SOFTWARE\\OpenVRML\\OpenVRML",
                                   options,
                                   KEY_READ,
                                   &key);
        if (result != ERROR_SUCCESS) {
            throw_runtime_error(result);
        }

        return query_registry_value(key, name);
    }
# endif
}

const std::string openvrml::local::conf::datadir()
    OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
{
    std::string result;
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

    OPENVRML_LOCAL void append_to_path(std::string & path,
                                       const std::string & appended_part)
        OPENVRML_THROW1(std::bad_alloc)
    {
        if (!path.empty() && !appended_part.empty()) {
            path += path_separator;
        }
        path += appended_part;
    }
}

const std::string openvrml::local::conf::node_path()
    OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
{
    std::string env_path;
    try {
        env_path = get_env("OPENVRML_NODE_PATH");
    } catch (const no_environment_var &) {}

    std::string system_path;
# ifdef _WIN32
    system_path = get_registry_setting("NodePath");
# else
    system_path = OPENVRML_PKGLIBDIR_ "/node";
# endif

    std::string result = env_path;
    append_to_path(result, system_path);
    return result;
}

const std::string openvrml::local::conf::script_path()
    OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
{
    std::string env_path;
    try {
        env_path = get_env("OPENVRML_SCRIPT_PATH");
    } catch (const no_environment_var &) {}

    std::string system_path;
# ifdef _WIN32
    system_path = get_registry_setting("ScriptPath");
# else
    system_path = OPENVRML_PKGLIBDIR_ "/script";
# endif

    std::string result = env_path;
    append_to_path(result, system_path);
    return result;
}
