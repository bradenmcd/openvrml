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

# include "component.h"
# include "xml_reader.h"
# include <private.h>
# include <openvrml/browser.h>
# include <openvrml/scope.h>
# include <boost/lexical_cast.hpp>
# include <iostream>
# include <cstring>

class openvrml::local::component::xml_reader : boost::noncopyable {
    enum parse_state {
        none,
        component,
        level,
        requires,
        node,
        field
    };

    local::component & component_;
    parse_state state_;
    std::vector<component::level>::value_type * current_level_;
    level::value_type * current_node_;

public:
    explicit xml_reader(local::component & c);

    void read(const std::string & filename)
        OPENVRML_THROW1(std::runtime_error);

private:
    void process_node(local::xml_reader & reader);
    void start_element(local::xml_reader & reader);
    void end_element(local::xml_reader & reader);
};

openvrml::local::component::xml_reader::xml_reader(local::component & c):
    component_(c),
    state_(none),
    current_level_(0),
    current_node_(0)
{}

void openvrml::local::component::xml_reader::read(const std::string & filename)
    OPENVRML_THROW1(std::runtime_error)
{
    openvrml::local::xml_reader reader(filename);
    int result;
    while ((result = reader.read()) == 1) {
        this->process_node(reader);
    }

    if (result != 0) {
        throw std::runtime_error('\"' + filename + "\" failed to parse");
    }
}

void
openvrml::local::component::xml_reader::
process_node(openvrml::local::xml_reader & reader)
{
    const int node_type = reader.node_type();
    switch (node_type) {
    case openvrml::local::xml_reader::element_id:
        this->start_element(reader);
        break;
    case openvrml::local::xml_reader::end_element_id:
        this->end_element(reader);
        break;
    default:
        break;
    }
}

void
openvrml::local::component::xml_reader::
start_element(openvrml::local::xml_reader & reader)
{
    using std::pair;
    using std::strcmp;
    using std::string;
    using openvrml::node_interface;

    int move_to_attr_result;
    node_interface interface_;

    switch (this->state_) {
    case component::xml_reader::none:
        this->state_ = component::xml_reader::component;
        move_to_attr_result = reader.move_to_first_attribute();
        if (move_to_attr_result > 0) {
            this->component_.id_ = reader.value();
        }
        break;
    case component::xml_reader::component:
        this->state_ = local::component::xml_reader::level;
        this->component_.levels_.push_back(component::level());
        this->current_level_ = &this->component_.levels_.back();
        break;
    case component::xml_reader::level:
        if (reader.local_name() == "requires") {
            string id;
            size_t level;
            move_to_attr_result = reader.move_to_first_attribute();
            while (move_to_attr_result > 0) {
                using boost::lexical_cast;
                if (reader.local_name() == "id") {
                    id = reader.value();
                } else if (reader.local_name() == "level") {
                    level = lexical_cast<size_t>(reader.value());
                }
                move_to_attr_result = reader.move_to_next_attribute();
            }

            this->current_level_->dependencies_.insert(
                make_pair(id, level));
        } else if (reader.local_name() == "node") {
            this->state_ = component::xml_reader::node;

            string id;
            component::node_type_decl node_type;
            move_to_attr_result = reader.move_to_first_attribute();
            while (move_to_attr_result > 0) {
                if (reader.local_name() == "id") {
                    id = reader.value();
                } else if (reader.local_name() == "metatype-id") {
                    node_type.metatype_id = reader.value();
                }
                move_to_attr_result = reader.move_to_next_attribute();
            }
            pair<component::level::iterator, bool> result =
                this->current_level_->insert(std::make_pair(id, node_type));
            if (result.second) {
                this->current_node_ = &*result.first;
            }
        } else {
            throw std::runtime_error("unexpected element: "
                                     + reader.local_name());
        }
        break;
    case component::xml_reader::requires:
        break;
    case component::xml_reader::node:
        move_to_attr_result = reader.move_to_first_attribute();
        while (move_to_attr_result > 0) {
            using boost::lexical_cast;
            using openvrml::field_value;

            if (reader.local_name() == "id") {
                interface_.id = reader.value();
            } else if (reader.local_name() == "type") {
                try {
                    interface_.field_type =
                        lexical_cast<field_value::type_id>(reader.value());
                } catch (const boost::bad_lexical_cast &) {
                    throw std::runtime_error(
                        "invalid field value type identifier \""
                        + reader.value() + '\"');
                }
            } else if (reader.local_name() == "access-type") {
                try {
                    interface_.type =
                        lexical_cast<node_interface::type_id>(
                            reader.value());
                } catch (const boost::bad_lexical_cast &) {
                    throw std::runtime_error(
                        "invalid field access type identifier \""
                        + reader.value() + '\"');
                }
            }
            move_to_attr_result = reader.move_to_next_attribute();
        }
        this->current_node_->second.interfaces.insert(interface_);
        break;
    case component::xml_reader::field: default:
        assert(false);
    }
}

void
openvrml::local::component::xml_reader::
end_element(openvrml::local::xml_reader &)
{
    switch (this->state_) {
    case component::xml_reader::none:
        break;
    case component::xml_reader::component:
        break;
    case component::xml_reader::level:
        this->state_ = component::xml_reader::component;
        break;
    case component::xml_reader::requires:
        break;
    case component::xml_reader::node:
        this->state_ = component::xml_reader::level;
        break;
    case component::xml_reader::field:
        break;
    }
}

openvrml::local::component::component(const std::string & filename)
    OPENVRML_THROW1(std::runtime_error)
{
    xml_reader reader(*this);
    reader.read(filename);
}

const std::string & openvrml::local::component::id() const OPENVRML_NOTHROW
{
    return this->id_;
}

std::size_t openvrml::local::component::support_level() const OPENVRML_NOTHROW
{
    return this->levels_.size();
}

bool
openvrml::local::component::
add_type_desc(openvrml::node_type_decls & type_descs,
              const std::string & node_type_id,
              const openvrml::node_interface_set & interfaces)
    OPENVRML_THROW1(std::bad_alloc)
{
    return type_descs.insert(make_pair(node_type_id, interfaces)).second;
}

void
openvrml::local::component::add_scope_entry(
    const openvrml::browser & b,
    const char * node_name,
    const openvrml::node_interface_set & interface_set,
    const char * urn,
    openvrml::scope & scope) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    using openvrml::node_type;

    const shared_ptr<node_metatype> class_ = b.node_metatype(urn);
    assert(class_);
    const shared_ptr<node_type> type = class_->create_type(node_name,
                                                           interface_set);
    const std::pair<shared_ptr<node_type>, bool> add_type_result =
        scope.add_type(type);

    //
    // If scope::add_type failed, there are two possible reasons:
    //  1. We're trying to add exactly the same node_type as already exists
    //     in the scope.  The mostly likely way this would happen is if a
    //     component is added redundantly.  There are entirely reasonable
    //     ways that could happen; e.g., if the profile incorporates
    //     component A level 1, and the X3D file also has a component
    //     statement to import component A level 2.
    //  2. node_types in two different components have the same
    //     node_type::id.
    //
    if (!add_type_result.second && *add_type_result.first != *type) {
        //
        // The way things are currently implemented, we don't want to throw
        // here.  Even if a component has a problematic node, we might as
        // well add the rest of the nodes in it.  After all, we don't know
        // how many nodes in the component we've already added.
        //
        // At some point it may be desirable to make component import
        // transactional--i.e., all nodes are added successfully, or
        // component import fails totally.  But that doesn't seem necessary
        // at this point.
        //
        b.err(std::string("interfaces for \"") + node_name + "\" are not "
              "consistent with the existing node type by that name");
    }
}

void openvrml::local::component::add_to_scope(const openvrml::browser & b,
                                              openvrml::scope & scope,
                                              const size_t level) const
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    if (level > this->support_level()) {
        throw std::invalid_argument("unsupported component level");
    }
    for (std::size_t i = 0; i < level; ++i) {
        for (component::level::const_iterator node =
                 this->levels_[i].begin();
             node != this->levels_[i].end();
             ++node) try {
                this->add_scope_entry(b,
                                      node->first.c_str(),
                                      node->second.interfaces,
                                      node->second.metatype_id.c_str(),
                                      scope);
            } catch (openvrml::unsupported_interface & ex) {
                b.err(node->second.metatype_id + ": " + ex.what());
            }
    }
}

void
openvrml::local::component::
add_to_node_type_desc_map(openvrml::node_type_decls & type_descs,
                          const size_t level) const
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    if (level == 0 || level > this->support_level()) {
        throw std::invalid_argument("unsupported component level");
    }
    for (std::size_t i = 0; i < level; ++i) {
        for (component::level::const_iterator node =
                 this->levels_[i].begin();
             node != this->levels_[i].end();
             ++node) {
            component::add_type_desc(type_descs,
                                     node->first,
                                     node->second.interfaces);
        }
    }
}

openvrml::local::component_registry::component_registry()
    OPENVRML_THROW2(boost::filesystem::filesystem_error,
                    std::bad_alloc)
{
    using std::auto_ptr;
    using boost::filesystem::path;
    using boost::filesystem::directory_iterator;

    std::string key;
    bool succeeded;

    path data_path;
    const char * datadir_env = getenv("OPENVRML_DATADIR");
    data_path = datadir_env
        ? datadir_env
        : OPENVRML_PKGDATADIR_;

    const path component_path = data_path / "component";
    for (directory_iterator entry(component_path);
         entry != directory_iterator();
         ++entry) {
        if (!is_directory(entry->path())) try {
                auto_ptr<component>
                    c(new component(entry->path().file_string()));
                std::string key = c->id();
                succeeded = this->insert(key, c.release()).second;
                assert(succeeded);
            } catch (const std::runtime_error & ex) {
                std::cerr << ex.what() << std::endl;
            }
    }
}

//
// The component_registry_ must be constructed before the
// profile_registry_.  The simplest way to ensure this is to define
// component_registry_ first in the same translation unit;
// profile_registry_ is defined below.
//
const openvrml::local::component_registry openvrml::local::component_registry_;


void
openvrml::local::profile::
add_component(const std::string & name, const int level)
    OPENVRML_THROW1(std::bad_alloc)
{
    //get the existing/new support level
    int & comp_level = components_[name];

    //update the support level.
    comp_level = std::max(level, comp_level);
}

std::auto_ptr<openvrml::scope>
openvrml::local::profile::create_root_scope(const openvrml::browser & browser,
                                            const std::string & uri) const
    OPENVRML_THROW1(std::bad_alloc)
{
    using openvrml::scope;

    std::auto_ptr<scope> root_scope(new scope(uri));
    for (map_t::const_iterator entry = this->components_.begin();
         entry != this->components_.end();
         ++entry) {
        try {
            const component & c = local::component_registry_.at(entry->first);
            c.add_to_scope(browser, *root_scope, entry->second);
        } catch (boost::bad_ptr_container_operation & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        } catch (std::invalid_argument & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }
    return root_scope;
}

std::auto_ptr<openvrml::node_type_decls>
openvrml::local::profile::create_node_type_desc_map() const
    OPENVRML_THROW1(std::bad_alloc)
{
    using openvrml::node_type_decls;

    std::auto_ptr<node_type_decls> node_type_descs(new node_type_decls);
    for (map_t::const_iterator entry = this->components_.begin();
         entry != this->components_.end();
         ++entry) {
        try {
            const component & c = local::component_registry_.at(entry->first);
            c.add_to_node_type_desc_map(*node_type_descs, entry->second);
        } catch (boost::bad_ptr_container_operation & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        } catch (std::invalid_argument & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }
    return node_type_descs;
}

openvrml::local::profile_registry::profile_registry()
{
    std::string key;
    bool succeeded;

    key = vrml97_profile::id;
    succeeded = this->insert(key, new vrml97_profile).second;
    assert(succeeded);

    key = x3d_core_profile::id;
    succeeded = this->insert(key, new x3d_core_profile).second;
    assert(succeeded);

    key = x3d_interchange_profile::id;
    succeeded = this->insert(key, new x3d_interchange_profile).second;
    assert(succeeded);

    key = x3d_interactive_profile::id;
    succeeded = this->insert(key, new x3d_interactive_profile).second;
    assert(succeeded);

    key = x3d_mpeg4_profile::id;
    succeeded = this->insert(key, new x3d_mpeg4_profile).second;
    assert(succeeded);

    key = x3d_immersive_profile::id;
    succeeded = this->insert(key, new x3d_immersive_profile).second;
    assert(succeeded);

    key = x3d_full_profile::id;
    succeeded = this->insert(key, new x3d_full_profile).second;
    assert(succeeded);
}

const openvrml::local::profile_registry openvrml::local::profile_registry_;

const char * const openvrml::local::vrml97_profile::id = "VRML97";

openvrml::local::vrml97_profile::vrml97_profile()
{
    this->add_component("VRML97", 1);
}


const char * const openvrml::local::x3d_core_profile::id = "Core";

openvrml::local::x3d_core_profile::x3d_core_profile()
{
    this->add_component("Core", 1);
}


const char * const openvrml::local::x3d_interchange_profile::id = "Interchange";

openvrml::local::x3d_interchange_profile::x3d_interchange_profile()
{
    this->add_component("Core", 1);
    this->add_component("Time", 1);
    this->add_component("Grouping", 1);
    this->add_component("Rendering", 3);
    this->add_component("Shape", 1);
    this->add_component("Geometry3D", 2);
    this->add_component("Lighting", 1);
    this->add_component("Texturing", 2);
    this->add_component("Interpolation", 2);
    this->add_component("Navigation", 1);
    this->add_component("EnvironmentalEffects", 1);
}


const char * const openvrml::local::x3d_interactive_profile::id = "Interactive";

openvrml::local::x3d_interactive_profile::x3d_interactive_profile()
{
    this->add_component("Core", 1);
    this->add_component("Time", 1);
    this->add_component("Grouping", 2);
    this->add_component("Rendering", 2);
    this->add_component("Shape", 1);
    this->add_component("Geometry3D", 3);
    this->add_component("Lighting", 2);
    this->add_component("Texturing", 2);
    this->add_component("Interpolation", 2);
    this->add_component("PointingDeviceSensor", 1);
    this->add_component("KeyDeviceSensor", 1);
    this->add_component("EnvironmentalSensor", 1);
    this->add_component("Navigation", 1);
    this->add_component("EnvironmentalEffects", 1);
    this->add_component("EventUtilities", 1);
}


const char * const openvrml::local::x3d_mpeg4_profile::id = "MPEG-4";

openvrml::local::x3d_mpeg4_profile::x3d_mpeg4_profile()
{
    this->add_component("Core", 1);
    this->add_component("Time", 1);
    this->add_component("Networking", 2);
    this->add_component("Grouping", 2);
    this->add_component("Rendering", 1);
    this->add_component("Shape", 1);
    this->add_component("Geometry3D", 2);
    this->add_component("Lighting", 2);
    this->add_component("Texturing", 2);
    this->add_component("Interpolation", 2);
    this->add_component("PointingDeviceSensor", 1);
    this->add_component("EnvironmentalSensor", 1);
    this->add_component("Navigation", 1);
    this->add_component("EnvironmentalEffects", 1);
}


const char * const openvrml::local::x3d_immersive_profile::id = "Immersive";

openvrml::local::x3d_immersive_profile::x3d_immersive_profile()
{
    this->add_component("Core", 2);
    this->add_component("Time", 1);
    this->add_component("Networking", 3);
    this->add_component("Grouping", 2);
    this->add_component("Rendering", 3);
    this->add_component("Shape", 2);
    this->add_component("Geometry3D", 4);
    this->add_component("Geometry2D", 1);
    this->add_component("Text", 1);
    this->add_component("Sound", 1);
    this->add_component("Lighting", 2);
    this->add_component("Texturing", 3);
    this->add_component("Interpolation", 2);
    this->add_component("PointingDeviceSensor", 1);
    this->add_component("KeyDeviceSensor", 2);
    this->add_component("EnvironmentalSensor", 2);
    this->add_component("Navigation", 2);
    this->add_component("EnvironmentalEffects", 2);
    this->add_component("EventUtilities", 1);
}


const char * const openvrml::local::x3d_full_profile::id = "Full";

openvrml::local::x3d_full_profile::x3d_full_profile()
{
    this->add_component("Core", 2);
    this->add_component("Time", 2);
    this->add_component("Networking", 3);
    this->add_component("Grouping", 3);
    this->add_component("Rendering", 4);
    this->add_component("Shape", 3);
    this->add_component("Geometry3D", 4);
    this->add_component("Geometry2D", 2);
    this->add_component("Text", 1);
    this->add_component("Sound", 1);
    this->add_component("Lighting", 3);
    this->add_component("Texturing", 3);
    this->add_component("Interpolation", 3);
    this->add_component("PointingDeviceSensor", 1);
    this->add_component("KeyDeviceSensor", 2);
    this->add_component("EnvironmentalSensor", 2);
    this->add_component("Navigation", 2);
    this->add_component("EnvironmentalEffects", 3);
    this->add_component("Geospatial", 1);
    this->add_component("H-Anim", 1);
    this->add_component("NURBS", 4);
    this->add_component("DIS", 1);
    this->add_component("EventUtilities", 1);
}
