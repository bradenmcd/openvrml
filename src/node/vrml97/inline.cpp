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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <boost/thread.hpp>
# include <openvrml/node_impl_util.h>
# include <openvrml/browser.h>
# include <private.h>
# include "inline.h"

namespace {

    class OPENVRML_LOCAL inline_node :
        public openvrml::node_impl_util::abstract_node<inline_node>,
        public openvrml::grouping_node {

        friend class openvrml_node_vrml97::inline_metatype;

        struct load_inline_scene;

        exposedfield<openvrml::mfstring> url_;
        exposedfield<openvrml::sfbool> load_;
        openvrml::sfvec3f bbox_center_;
        openvrml::sfvec3f bbox_size_;

        openvrml::scene * inline_scene_;
        bool loaded_;
        boost::scoped_ptr<boost::thread> load_inline_scene_thread_;

    public:
        inline_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~inline_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
        virtual const std::vector<boost::intrusive_ptr<openvrml::node> >
            do_children() const OPENVRML_THROW1(std::bad_alloc);

        void load();
    };

    /**
     * @class inline_node
     *
     * @brief Represents Inline node instances.
     */

    /**
     * @var class inline_node::inline_metatype
     *
     * @brief Class object for Inline nodes.
     */

    /**
     * @var openvrml::sfvec3f inline_node::bbox_center_
     *
     * @brief bboxCenter field.
     */

    /**
     * @var openvrml::sfvec3f inline_node::bbox_size_
     *
     * @brief bboxSize field.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<inline_node>::exposedfield<openvrml::mfstring> inline_node::url_
     *
     * @brief url exposedField.
     */

    /**
     * @var openvrml::scene * inline_node::inline_scene_
     *
     * @brief The contained scene.
     */

    /**
     * @var bool inline_node::loaded_
     *
     * @brief Flag to indicate whether the scene has been loaded.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    inline_node::
    inline_node(const openvrml::node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        openvrml::node_impl_util::abstract_node<inline_node>(type, scope),
        grouping_node(type, scope),
        url_(*this),
        load_(*this, true),
        inline_scene_(0),
        loaded_(false)
    {
        this->bounding_volume_dirty(true);
    }

    /**
     * @brief Destroy.
     */
    inline_node::~inline_node() OPENVRML_NOTHROW
    {
        if (this->load_inline_scene_thread_) {
            this->load_inline_scene_thread_->join();
        }
    }

    /**
     * @brief Render the node.
     *
     * Render each of the children.
     *
     * @param viewer    a @c viewer.
     * @param context   a @c rendering_context.
     */
    void inline_node::do_render_child(openvrml::viewer & viewer,
                                      const openvrml::rendering_context context)
    {
        this->load();
        if (this->inline_scene_) { this->inline_scene_->render(viewer, context); }
    }

    /**
     * @brief Get the children in the scene graph.
     *
     * @return the child nodes in the scene graph.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    const std::vector<boost::intrusive_ptr<openvrml::node> >
    inline_node::do_children() const OPENVRML_THROW1(std::bad_alloc)
    {
        static const std::vector<boost::intrusive_ptr<openvrml::node> > empty;
        return this->inline_scene_
            ? this->inline_scene_->nodes()
            : empty;
    }

    struct OPENVRML_LOCAL inline_node::load_inline_scene {

        load_inline_scene(openvrml::scene & scene,
                          const std::vector<std::string> & url):
            inline_scene_(&scene),
            url_(&url)
        {}

        void operator()() const OPENVRML_NOTHROW
        {
            try {
                using std::endl;
                using std::string;
                using std::vector;
                using openvrml::unreachable_url;

                openvrml::scene & inline_scene = *this->inline_scene_;
                const vector<string> & url = *this->url_;

                assert(inline_scene.url().empty());

                vector<boost::intrusive_ptr<node> > nodes;
                try {
                    //
                    // Any relative URLs passed here will be relative to the
                    // *parent* scene; so we call get_resource on the parent.
                    //
                    assert(inline_scene.parent());
                    std::auto_ptr<openvrml::resource_istream> in =
                        inline_scene.parent()->get_resource(url);
                    if (!(*in)) { throw unreachable_url(); }
                    inline_scene.load(*in);
                } catch (std::exception & ex) {
                    inline_scene.browser().err(ex.what());
                    throw unreachable_url();
                } catch (...) {
                    throw unreachable_url();
                }
            } catch (std::exception & ex) {
                this->inline_scene_->browser().err(ex.what());
            }
        }

    private:
        openvrml::scene * const inline_scene_;
        const std::vector<std::string> * const url_;
    };

    /**
     * @brief Load the children from the URL.
     */
    void inline_node::load()
    {
        class inline_scene : public openvrml::scene {
        public:
            inline_scene(openvrml::browser & b, openvrml::scene * parent):
                openvrml::scene(b, parent)
            {}

        private:
            virtual void scene_loaded()
            {
                this->initialize(openvrml::browser::current_time());
            }
        };

        //
        // XXX Need to check whether Url has been modified.
        //
        if (this->loaded_) { return; }

        this->loaded_ = true; // although perhaps not successfully
        this->bounding_volume_dirty(true);

        assert(this->scene());
        this->inline_scene_ = new inline_scene(this->scene()->browser(),
                                               this->scene());
        boost::function0<void> f =
            load_inline_scene(*this->inline_scene_,
                              this->url_.mfstring::value());
        this->load_inline_scene_thread_.reset(new boost::thread(f));
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::inline_metatype::id =
    "urn:X-openvrml:node:Inline";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::inline_metatype::
inline_metatype(openvrml::browser & browser):
    node_metatype(inline_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::inline_metatype::~inline_metatype() OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Inline nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by inline_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::inline_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 5> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "load")
    };

    typedef node_impl_util::node_type_impl<inline_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & inlineNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            inlineNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<inline_node>::exposedfield<mfstring> >(
                        &inline_node::url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<inline_node>::exposedfield<mfstring> >(
                        &inline_node::url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<inline_node>::exposedfield<mfstring> >(
                        &inline_node::url_)));
        } else if (*interface_ == *++supported_interface) {
            inlineNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &inline_node::bbox_center_)));
        } else if (*interface_ == *++supported_interface) {
            inlineNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &inline_node::bbox_size_)));
        } else if (*interface_ == *++supported_interface) {
            inlineNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<inline_node>::exposedfield<sfnode> >(
                        &inline_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<inline_node>::exposedfield<sfnode> >(
                        &inline_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<inline_node>::exposedfield<sfnode> >(
                        &inline_node::metadata)));
        } else if (*interface_ == *++supported_interface) {
            inlineNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<inline_node>::exposedfield<sfbool> >(
                        &inline_node::load_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<inline_node>::exposedfield<sfbool> >(
                        &inline_node::load_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<inline_node>::exposedfield<sfbool> >(
                        &inline_node::load_)));
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
