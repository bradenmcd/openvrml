// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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

# ifndef OPENVRML_SCENE_H
#   define OPENVRML_SCENE_H

#   include <openvrml-common.h>
#   include <openvrml/bad_url.h>
#   include <openvrml/node.h>
#   include <boost/thread.hpp>

namespace openvrml {

    class browser;
    class resource_istream;
    class stream_listener;

    class OPENVRML_API scene : boost::noncopyable {
        struct vrml_from_url_creator;

        openvrml::browser * const browser_;
        scene * const parent_;

        mutable read_write_mutex nodes_mutex_;
        std::vector<boost::intrusive_ptr<node> > nodes_;

        mutable read_write_mutex url_mutex_;
        std::string url_;

        mutable read_write_mutex meta_mutex_;
        std::map<std::string, std::string> meta_;

        boost::thread_group stream_reader_threads_;

    public:
        explicit scene(openvrml::browser & browser, scene * parent = 0)
            OPENVRML_NOTHROW;
        virtual ~scene() OPENVRML_NOTHROW;

        openvrml::browser & browser() const OPENVRML_NOTHROW;
        scene * parent() const OPENVRML_NOTHROW;
        void load(resource_istream & in);
        void initialize(double timestamp) OPENVRML_THROW1(std::bad_alloc);
        const std::string meta(const std::string & key) const
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
        void meta(const std::string & key, const std::string & value)
            OPENVRML_THROW1(std::bad_alloc);
        const std::vector<std::string> meta_keys() const
            OPENVRML_THROW1(std::bad_alloc);
        const std::vector<boost::intrusive_ptr<node> > nodes() const
            OPENVRML_THROW1(std::bad_alloc);
        void nodes(const std::vector<boost::intrusive_ptr<node> > & n)
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
        const scope * root_scope() const OPENVRML_NOTHROW;
        const std::string url() const OPENVRML_THROW1(std::bad_alloc);
        void render(openvrml::viewer & viewer, rendering_context context);
        void load_url(const std::vector<std::string> & url,
                      const std::vector<std::string> & parameter)
            OPENVRML_THROW1(std::bad_alloc);
        std::auto_ptr<resource_istream>
        get_resource(const std::vector<std::string> & url) const
            OPENVRML_THROW2(no_alternative_url, std::bad_alloc);
        void read_stream(std::auto_ptr<resource_istream> in,
                         std::auto_ptr<stream_listener> listener);
        void create_vrml_from_url(const std::vector<std::string> & url,
                                  const boost::intrusive_ptr<node> & node,
                                  const std::string & event)
            OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                            boost::thread_resource_error);
        void shutdown(double timestamp) OPENVRML_NOTHROW;

    private:
        virtual void scene_loaded();
    };
}

# endif // OPENVRML_SCENE_H
