// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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

# ifndef OPENVRML_LOCAL_NODE_METATYPE_REGISTRY_IMPL_H
#   define OPENVRML_LOCAL_NODE_METATYPE_REGISTRY_IMPL_H

#   include <openvrml/local/dl.h>
#   include <openvrml/node.h>

extern "C" OPENVRML_LOCAL int openvrml_open_node_module(const char * filename,
                                                        void * data);

namespace openvrml {

    class browser;
    class node_metatype_registry;
    class viewpoint_node;

    namespace local {

        class OPENVRML_LOCAL node_metatype_registry_impl : boost::noncopyable {
            friend int (::openvrml_open_node_module)(const char * filename,
                                                     void * data);
            mutable boost::shared_mutex mutex_;

            openvrml::browser & browser_;

            typedef std::set<local::dl::handle> module_handle_set;
            module_handle_set module_handles_;

            typedef std::map<std::string, boost::shared_ptr<node_metatype> >
                node_metatype_map_t;
            mutable node_metatype_map_t node_metatype_map_;

        public:
            static const char sym[33];

            explicit node_metatype_registry_impl(openvrml::browser & b);
            ~node_metatype_registry_impl() OPENVRML_NOTHROW;

            openvrml::browser & browser() const OPENVRML_NOTHROW;

            void register_node_metatypes(node_metatype_registry & registry);

            void register_node_metatype(
                const std::string & id,
                const boost::shared_ptr<node_metatype> & metatype)
                OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);

            void init(viewpoint_node * initial_viewpoint, double timestamp);

            const boost::shared_ptr<node_metatype>
            find(const std::string & id) const;

            const std::vector<node_metatype_id>
            node_metatype_ids(const node_metatype & metatype) const
                OPENVRML_THROW1(std::bad_alloc);

            void render(viewer & v);

            void shutdown(double timestamp) OPENVRML_NOTHROW;
        };
    }
}

# endif // ifndef OPENVRML_LOCAL_NODE_METATYPE_REGISTRY_IMPL_H
