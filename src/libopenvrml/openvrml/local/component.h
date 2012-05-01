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

# ifndef OPENVRML_LOCAL_COMPONENT_H
#   define OPENVRML_LOCAL_COMPONENT_H

#   include <openvrml/node.h>
#   include <boost/filesystem.hpp>
#   include <boost/ptr_container/ptr_map.hpp>

namespace openvrml {

    namespace local {

        class OPENVRML_LOCAL component {
            struct node_type_decl {
                openvrml::node_interface_set interfaces;
                std::string metatype_id;
            };

            class xml_reader;

            class level : std::map<std::string, node_type_decl> {
                friend class xml_reader;

            public:
                typedef std::map<std::string, size_t> dependencies_t;

            private:
                dependencies_t dependencies_;

            public:
                typedef std::map<std::string, node_type_decl> base_t;
                using base_t::value_type;
                using base_t::iterator;
                using base_t::const_iterator;
                using base_t::begin;
                using base_t::end;
                using base_t::find;
                using base_t::insert;

                const dependencies_t & requires() const;
            };

            std::string id_;
            std::vector<level> levels_;

        public:
            explicit component(const std::string & filename)
                OPENVRML_THROW1(std::runtime_error);

            const std::string & id() const OPENVRML_NOTHROW;
            size_t support_level() const OPENVRML_NOTHROW;
            void add_to_scope(const openvrml::browser & b,
                              openvrml::scope & scope,
                              size_t level) const
                OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
            void
            add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                                      size_t level) const
                OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);

        private:
            static bool
                add_type_desc(openvrml::node_type_decls & type_descs,
                              const std::string & node_type_id,
                              const openvrml::node_interface_set & interfaces)
                OPENVRML_THROW1(std::bad_alloc);

            void add_scope_entry(
                const openvrml::browser & b,
                const char * node_name,
                const openvrml::node_interface_set & interface_set,
                const char * urn,
                openvrml::scope & scope) const
                OPENVRML_THROW2(openvrml::unsupported_interface,
                                std::bad_alloc);
        };


        OPENVRML_LOCAL
        extern const class OPENVRML_LOCAL component_registry :
            boost::ptr_map<std::string, component> {
        public:
            component_registry()
                OPENVRML_THROW2(boost::filesystem::filesystem_error,
                                std::bad_alloc);

            using boost::ptr_map<std::string, component>::at;
        } component_registry_;


        class OPENVRML_LOCAL profile {
            typedef std::map<std::string, size_t> map_t;
            map_t components_;

        protected:
            void add_component(const std::string & name, size_t level)
                OPENVRML_THROW1(std::bad_alloc);

        public:
            std::auto_ptr<openvrml::scope>
            create_root_scope(const openvrml::browser & browser,
                              const std::string & uri) const
                OPENVRML_THROW1(std::bad_alloc);

            std::auto_ptr<openvrml::node_type_decls>
            create_node_type_desc_map() const
                OPENVRML_THROW1(std::bad_alloc);
        };


        OPENVRML_LOCAL
        extern const class OPENVRML_LOCAL profile_registry :
            boost::ptr_map<std::string, profile> {
        public:
            profile_registry();

            using boost::ptr_map<std::string, profile>::at;
        } profile_registry_;


        class OPENVRML_LOCAL vrml97_profile : public profile {
        public:
            static const char * const id;

            vrml97_profile();
        };


        class OPENVRML_LOCAL x3d_core_profile : public profile {
        public:
            static const char * const id;

            x3d_core_profile();
        };


        class OPENVRML_LOCAL x3d_interchange_profile : public profile {
        public:
            static const char * const id;

            x3d_interchange_profile();
        };


        class OPENVRML_LOCAL x3d_interactive_profile : public profile {
        public:
            static const char * const id;

            x3d_interactive_profile();
        };


        class OPENVRML_LOCAL x3d_mpeg4_profile : public profile {
        public:
            static const char * const id;

            x3d_mpeg4_profile();
        };


        class OPENVRML_LOCAL x3d_immersive_profile : public profile {
        public:
            static const char * const id;

            x3d_immersive_profile();
        };


        class OPENVRML_LOCAL x3d_full_profile : public profile {
        public:
            static const char * const id;

            x3d_full_profile();
        };
    }
}

# endif // ifndef OPENVRML_LOCAL_COMPONENT_H
