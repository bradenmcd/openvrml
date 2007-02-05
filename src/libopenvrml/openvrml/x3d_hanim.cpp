// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2006  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <private.h>
# include "browser.h"
# include "node_impl_util.h"
# include "x3d_hanim.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for HAnimDisplacer nodes.
     */
    class OPENVRML_LOCAL hanim_displacer_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit hanim_displacer_metatype(openvrml::browser & browser);
        virtual ~hanim_displacer_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for HAnimHumanoid nodes.
     */
    class OPENVRML_LOCAL hanim_humanoid_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit hanim_humanoid_metatype(openvrml::browser & browser);
        virtual ~hanim_humanoid_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for HAnimJoint nodes.
     */
    class OPENVRML_LOCAL hanim_joint_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit hanim_joint_metatype(openvrml::browser & browser);
        virtual ~hanim_joint_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for HAnimSegment nodes.
     */
    class OPENVRML_LOCAL hanim_segment_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit hanim_segment_metatype(openvrml::browser & browser);
        virtual ~hanim_segment_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for HAnimSite nodes.
     */
    class OPENVRML_LOCAL hanim_site_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit hanim_site_metatype(openvrml::browser & browser);
        virtual ~hanim_site_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };
}

void register_hanim_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(hanim_displacer_metatype::id,
                        shared_ptr<node_metatype>(
                            new hanim_displacer_metatype(b)));
    b.add_node_metatype(hanim_humanoid_metatype::id,
                        shared_ptr<node_metatype>(
                            new hanim_humanoid_metatype(b)));
    b.add_node_metatype(hanim_joint_metatype::id,
                        shared_ptr<node_metatype>(
                            new hanim_joint_metatype(b)));
    b.add_node_metatype(hanim_segment_metatype::id,
                        shared_ptr<node_metatype>(
                            new hanim_segment_metatype(b)));
    b.add_node_metatype(hanim_site_metatype::id,
                        shared_ptr<node_metatype>(new hanim_site_metatype(b)));
}

namespace {
    using namespace openvrml_;

    class OPENVRML_LOCAL hanim_displacer_node :
        public abstract_node<hanim_displacer_node> {

        friend class hanim_displacer_metatype;

        exposedfield<mfint32> coord_index_;
        exposedfield<mfvec3f> displacements_;
        exposedfield<sfstring> name_;
        exposedfield<sffloat> weight_;

    public:
        hanim_displacer_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~hanim_displacer_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL hanim_humanoid_node :
        public abstract_node<hanim_humanoid_node>,
        public child_node {

        friend class hanim_humanoid_metatype;

        exposedfield<sfvec3f> center_;
        exposedfield<mfstring> info_;
        exposedfield<mfnode> joints_;
        exposedfield<sfstring> name_;
        exposedfield<sfrotation> rotation_;
        exposedfield<sfvec3f> scale_;
        exposedfield<sfrotation> scale_orientation_;
        exposedfield<mfnode> segments_;
        exposedfield<mfnode> sites_;
        exposedfield<mfnode> skeleton_;
        exposedfield<mfnode> skin_;
        exposedfield<sfnode> skin_coord_;
        exposedfield<sfnode> skin_normal_;
        exposedfield<sfvec3f> translation_;
        exposedfield<sfstring> version_;
        exposedfield<mfnode> viewpoints_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        hanim_humanoid_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~hanim_humanoid_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL hanim_joint_node :
        public abstract_node<hanim_joint_node>,
        public child_node {

        friend class hanim_joint_metatype;

        class add_children_listener : public event_listener_base<self_t>,
                                      public mfnode_listener {
            public:
                explicit add_children_listener(self_t & node);
                virtual ~add_children_listener() OPENVRML_NOTHROW;

            private:
                virtual void do_process_event(const mfnode & fraction,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_children_listener : public event_listener_base<self_t>,
                                         public mfnode_listener {
            public:
                explicit remove_children_listener(self_t & node);
                virtual ~remove_children_listener() OPENVRML_NOTHROW;

            private:
                virtual void do_process_event(const mfnode & fraction,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);
        };

        add_children_listener add_children_listener_;
        remove_children_listener remove_children_listener_;
        exposedfield<sfvec3f> center_;
        exposedfield<mfnode> children_;
        exposedfield<mfnode> displacers_;
        exposedfield<sfrotation> limit_orientation_;
        exposedfield<mffloat> llimit_;
        exposedfield<sfstring> name_;
        exposedfield<sfrotation> rotation_;
        exposedfield<sfvec3f> scale_;
        exposedfield<sfrotation> scale_orientation_;
        exposedfield<mfint32> skin_coord_index_;
        exposedfield<mffloat> skin_coord_weight_;
        exposedfield<mffloat> stiffness_;
        exposedfield<sfvec3f> translation_;
        exposedfield<mffloat> ulimit_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        hanim_joint_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~hanim_joint_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL hanim_segment_node :
        public abstract_node<hanim_segment_node>,
        public child_node {

        friend class hanim_segment_metatype;

        class add_children_listener : public event_listener_base<self_t>,
                                      public mfnode_listener {
            public:
                explicit add_children_listener(self_t & node);
                virtual ~add_children_listener() OPENVRML_NOTHROW;

            private:
                virtual void do_process_event(const mfnode & fraction,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_children_listener : public event_listener_base<self_t>,
                                         public mfnode_listener {
            public:
                explicit remove_children_listener(self_t & node);
                virtual ~remove_children_listener() OPENVRML_NOTHROW;

            private:
                virtual void do_process_event(const mfnode & fraction,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);
        };

        add_children_listener add_children_listener_;
        remove_children_listener remove_children_listener_;
        exposedfield<sfvec3f> center_of_mass_;
        exposedfield<mfnode> children_;
        exposedfield<sfnode> coord_;
        exposedfield<mfnode> displacers_;
        exposedfield<sffloat> mass_;
        exposedfield<mffloat> moments_of_inertia_;
        exposedfield<sfstring> name_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        hanim_segment_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~hanim_segment_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL hanim_site_node :
        public abstract_node<hanim_site_node>,
        public child_node {

        friend class hanim_site_metatype;

        class add_children_listener : public event_listener_base<self_t>,
                                      public mfnode_listener {
            public:
                explicit add_children_listener(self_t & node);
                virtual ~add_children_listener() OPENVRML_NOTHROW;

            private:
                virtual void do_process_event(const mfnode & fraction,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_children_listener : public event_listener_base<self_t>,
                                         public mfnode_listener {
            public:
                explicit remove_children_listener(self_t & node);
                virtual ~remove_children_listener() OPENVRML_NOTHROW;

            private:
                virtual void do_process_event(const mfnode & fraction,
                                              double timestamp)
                    OPENVRML_THROW1(std::bad_alloc);
        };

        add_children_listener add_children_listener_;
        remove_children_listener remove_children_listener_;
        exposedfield<sfvec3f> center_;
        exposedfield<mfnode> children_;
        exposedfield<sfstring> name_;
        exposedfield<sfrotation> rotation_;
        exposedfield<sfvec3f> scale_;
        exposedfield<sfrotation> scale_orientation_;
        exposedfield<sfvec3f> translation_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        hanim_site_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~hanim_site_node() OPENVRML_NOTHROW;
    };


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const hanim_displacer_metatype::id =
        "urn:X-openvrml:node:HAnimDisplacer";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c hanim_displacer_metatype.
     */
    hanim_displacer_metatype::hanim_displacer_metatype(openvrml::browser & browser):
        node_metatype(hanim_displacer_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    hanim_displacer_metatype::~hanim_displacer_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating HAnimDisplacer nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by hanim_displacer_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    hanim_displacer_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 5> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "coordIndex"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3f_id,
                           "displacements"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "weight")
        };
        typedef node_type_impl<hanim_displacer_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<sfnode> >(
                            &hanim_displacer_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<sfnode> >(
                            &hanim_displacer_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<sfnode> >(
                            &hanim_displacer_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<mfint32> >(
                            &hanim_displacer_node::coord_index_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<mfint32> >(
                            &hanim_displacer_node::coord_index_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<mfint32> >(
                            &hanim_displacer_node::coord_index_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<mfvec3f> >(
                            &hanim_displacer_node::displacements_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<mfvec3f> >(
                            &hanim_displacer_node::displacements_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<mfvec3f> >(
                            &hanim_displacer_node::displacements_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<sfstring> >(
                            &hanim_displacer_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<sfstring> >(
                            &hanim_displacer_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<sfstring> >(
                            &hanim_displacer_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<sffloat> >(
                            &hanim_displacer_node::weight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<sffloat> >(
                            &hanim_displacer_node::weight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_displacer_node>::exposedfield<sffloat> >(
                            &hanim_displacer_node::weight_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const hanim_humanoid_metatype::id =
        "urn:X-openvrml:node:HAnimHumanoid";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c hanim_humanoid_metatype.
     */
    hanim_humanoid_metatype::hanim_humanoid_metatype(openvrml::browser & browser):
        node_metatype(hanim_humanoid_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    hanim_humanoid_metatype::~hanim_humanoid_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating HAnimHumanoid nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by hanim_humanoid_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    hanim_humanoid_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 19> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "center"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "info"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "joints"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "rotation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "scale"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "scaleOrientation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "segments"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "sites"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "skeleton"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "skin"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "skinCoord"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "skinNormal"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "translation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "version"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "viewpoints"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        typedef node_type_impl<hanim_humanoid_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfnode> >(
                            &hanim_humanoid_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfnode> >(
                            &hanim_humanoid_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfnode> >(
                            &hanim_humanoid_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfvec3f> >(
                            &hanim_humanoid_node::center_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfvec3f> >(
                            &hanim_humanoid_node::center_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfvec3f> >(
                            &hanim_humanoid_node::center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfstring> >(
                            &hanim_humanoid_node::info_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfstring> >(
                            &hanim_humanoid_node::info_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfstring> >(
                            &hanim_humanoid_node::info_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::joints_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::joints_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::joints_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfstring> >(
                            &hanim_humanoid_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfstring> >(
                            &hanim_humanoid_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfstring> >(
                            &hanim_humanoid_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfrotation> >(
                            &hanim_humanoid_node::rotation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfrotation> >(
                            &hanim_humanoid_node::rotation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfrotation> >(
                            &hanim_humanoid_node::rotation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfvec3f> >(
                            &hanim_humanoid_node::scale_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfvec3f> >(
                            &hanim_humanoid_node::scale_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfvec3f> >(
                            &hanim_humanoid_node::scale_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfrotation> >(
                            &hanim_humanoid_node::scale_orientation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfrotation> >(
                            &hanim_humanoid_node::scale_orientation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfrotation> >(
                            &hanim_humanoid_node::scale_orientation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::segments_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::segments_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::segments_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::sites_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::sites_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::sites_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::skeleton_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::skeleton_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::skeleton_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::skin_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::skin_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::skin_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfnode> >(
                            &hanim_humanoid_node::skin_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfnode> >(
                            &hanim_humanoid_node::skin_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfnode> >(
                            &hanim_humanoid_node::skin_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfnode> >(
                            &hanim_humanoid_node::skin_normal_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfnode> >(
                            &hanim_humanoid_node::skin_normal_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfnode> >(
                            &hanim_humanoid_node::skin_normal_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfvec3f> >(
                            &hanim_humanoid_node::translation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfvec3f> >(
                            &hanim_humanoid_node::translation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfvec3f> >(
                            &hanim_humanoid_node::translation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfstring> >(
                            &hanim_humanoid_node::version_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfstring> >(
                            &hanim_humanoid_node::version_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<sfstring> >(
                            &hanim_humanoid_node::version_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::viewpoints_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::viewpoints_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_humanoid_node>::exposedfield<mfnode> >(
                            &hanim_humanoid_node::viewpoints_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &hanim_humanoid_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &hanim_humanoid_node::bbox_size_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const hanim_joint_metatype::id =
        "urn:X-openvrml:node:HAnimJoint";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c hanim_joint_metatype.
     */
    hanim_joint_metatype::hanim_joint_metatype(openvrml::browser & browser):
        node_metatype(hanim_joint_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    hanim_joint_metatype::~hanim_joint_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating HAnimJoint nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by hanim_joint_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    hanim_joint_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 19> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "center"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "displacers"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "limitOrientation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "llimit"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "rotation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "scale"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "scaleOrientation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "skinCoordIndex"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "skinCoordWeight"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "stiffness"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "translation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "ulimit"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        typedef node_type_impl<hanim_joint_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfnode> >(
                            &hanim_joint_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfnode> >(
                            &hanim_joint_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfnode> >(
                            &hanim_joint_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        hanim_joint_node::add_children_listener>(
                            &hanim_joint_node::add_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        hanim_joint_node::remove_children_listener>(
                            &hanim_joint_node::remove_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfvec3f> >(
                            &hanim_joint_node::center_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfvec3f> >(
                            &hanim_joint_node::center_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfvec3f> >(
                            &hanim_joint_node::center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mfnode> >(
                            &hanim_joint_node::children_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mfnode> >(
                            &hanim_joint_node::children_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mfnode> >(
                            &hanim_joint_node::children_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mfnode> >(
                            &hanim_joint_node::displacers_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mfnode> >(
                            &hanim_joint_node::displacers_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mfnode> >(
                            &hanim_joint_node::displacers_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfrotation> >(
                            &hanim_joint_node::limit_orientation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfrotation> >(
                            &hanim_joint_node::limit_orientation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfrotation> >(
                            &hanim_joint_node::limit_orientation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::llimit_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::llimit_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::llimit_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfstring> >(
                            &hanim_joint_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfstring> >(
                            &hanim_joint_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfstring> >(
                            &hanim_joint_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfrotation> >(
                            &hanim_joint_node::rotation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfrotation> >(
                            &hanim_joint_node::rotation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfrotation> >(
                            &hanim_joint_node::rotation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfvec3f> >(
                            &hanim_joint_node::scale_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfvec3f> >(
                            &hanim_joint_node::scale_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfvec3f> >(
                            &hanim_joint_node::scale_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfrotation> >(
                            &hanim_joint_node::scale_orientation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfrotation> >(
                            &hanim_joint_node::scale_orientation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfrotation> >(
                            &hanim_joint_node::scale_orientation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mfint32> >(
                            &hanim_joint_node::skin_coord_index_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mfint32> >(
                            &hanim_joint_node::skin_coord_index_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mfint32> >(
                            &hanim_joint_node::skin_coord_index_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::skin_coord_weight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::skin_coord_weight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::skin_coord_weight_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::stiffness_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::stiffness_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::stiffness_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfvec3f> >(
                            &hanim_joint_node::translation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfvec3f> >(
                            &hanim_joint_node::translation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<sfvec3f> >(
                            &hanim_joint_node::translation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::ulimit_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::ulimit_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_joint_node>::exposedfield<mffloat> >(
                            &hanim_joint_node::ulimit_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &hanim_joint_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &hanim_joint_node::bbox_size_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const hanim_segment_metatype::id =
        "urn:X-openvrml:node:HAnimSegment";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c hanim_segment_metatype.
     */
    hanim_segment_metatype::hanim_segment_metatype(openvrml::browser & browser):
        node_metatype(hanim_segment_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    hanim_segment_metatype::~hanim_segment_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating HAnimSegment nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by hanim_segment_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    hanim_segment_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 12> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "centerOfMass"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "coord"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "displacers"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "mass"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "momentsOfInertia"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        typedef node_type_impl<hanim_segment_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfnode> >(
                            &hanim_segment_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfnode> >(
                            &hanim_segment_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfnode> >(
                            &hanim_segment_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        hanim_segment_node::add_children_listener>(
                            &hanim_segment_node::add_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        hanim_segment_node::remove_children_listener>(
                            &hanim_segment_node::remove_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfvec3f> >(
                            &hanim_segment_node::center_of_mass_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfvec3f> >(
                            &hanim_segment_node::center_of_mass_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfvec3f> >(
                            &hanim_segment_node::center_of_mass_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<mfnode> >(
                            &hanim_segment_node::children_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<mfnode> >(
                            &hanim_segment_node::children_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<mfnode> >(
                            &hanim_segment_node::children_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfnode> >(
                            &hanim_segment_node::coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfnode> >(
                            &hanim_segment_node::coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfnode> >(
                            &hanim_segment_node::coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<mfnode> >(
                            &hanim_segment_node::displacers_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<mfnode> >(
                            &hanim_segment_node::displacers_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<mfnode> >(
                            &hanim_segment_node::displacers_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sffloat> >(
                            &hanim_segment_node::mass_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sffloat> >(
                            &hanim_segment_node::mass_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sffloat> >(
                            &hanim_segment_node::mass_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<mffloat> >(
                            &hanim_segment_node::moments_of_inertia_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<mffloat> >(
                            &hanim_segment_node::moments_of_inertia_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<mffloat> >(
                            &hanim_segment_node::moments_of_inertia_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfstring> >(
                            &hanim_segment_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfstring> >(
                            &hanim_segment_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_segment_node>::exposedfield<sfstring> >(
                            &hanim_segment_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &hanim_segment_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &hanim_segment_node::bbox_size_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @brief @c node_metatype identifier.
     */
    const char * const hanim_site_metatype::id =
        "urn:X-openvrml:node:HAnimSite";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c hanim_site_metatype.
     */
    hanim_site_metatype::hanim_site_metatype(openvrml::browser & browser):
        node_metatype(hanim_site_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    hanim_site_metatype::~hanim_site_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating HAnimSite nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by hanim_site_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    hanim_site_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 12> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "center"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "rotation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "scale"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "scaleOrientation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "translation"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        typedef node_type_impl<hanim_site_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfnode> >(
                            &hanim_site_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfnode> >(
                            &hanim_site_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfnode> >(
                            &hanim_site_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        hanim_site_node::add_children_listener>(
                            &hanim_site_node::add_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        hanim_site_node::remove_children_listener>(
                            &hanim_site_node::remove_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfvec3f> >(
                            &hanim_site_node::center_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfvec3f> >(
                            &hanim_site_node::center_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfvec3f> >(
                            &hanim_site_node::center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_site_node>::exposedfield<mfnode> >(
                            &hanim_site_node::children_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_site_node>::exposedfield<mfnode> >(
                            &hanim_site_node::children_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_site_node>::exposedfield<mfnode> >(
                            &hanim_site_node::children_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfstring> >(
                            &hanim_site_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfstring> >(
                            &hanim_site_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfstring> >(
                            &hanim_site_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfrotation> >(
                            &hanim_site_node::rotation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfrotation> >(
                            &hanim_site_node::rotation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfrotation> >(
                            &hanim_site_node::rotation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfvec3f> >(
                            &hanim_site_node::scale_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfvec3f> >(
                            &hanim_site_node::scale_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfvec3f> >(
                            &hanim_site_node::scale_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfrotation> >(
                            &hanim_site_node::scale_orientation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfrotation> >(
                            &hanim_site_node::scale_orientation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfrotation> >(
                            &hanim_site_node::scale_orientation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfvec3f> >(
                            &hanim_site_node::translation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfvec3f> >(
                            &hanim_site_node::translation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<hanim_site_node>::exposedfield<sfvec3f> >(
                            &hanim_site_node::translation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &hanim_site_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &hanim_site_node::bbox_size_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @class hanim_displacer_node
     *
     * @brief Represents HAnimDisplacer node instances.
     */

    /**
     * @var hanim_displacer_node::hanim_displacer_metatype
     *
     * @brief Class object for HAnimDisplacer nodes.
     */

    /**
     * @var hanim_displacer_node::coord_index_
     *
     * @brief coord_index exposedField
     */

    /**
     * @var hanim_displacer_node::displacements_
     *
     * @brief displacements exposedField
     */

    /**
     * @var hanim_displacer_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var hanim_displacer_node::weight_
     *
     * @brief weight exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    hanim_displacer_node::
    hanim_displacer_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        coord_index_(*this),
        displacements_(*this),
        name_(*this),
        weight_(*this)
    {}

    /**
     * @brief Destroy.
     */
    hanim_displacer_node::~hanim_displacer_node() OPENVRML_NOTHROW
    {}


    /**
     * @class hanim_humanoid_node
     *
     * @brief Represents HAnimHumanoid node instances.
     */

    /**
     * @var hanim_humanoid_node::hanim_humanoid_metatype
     *
     * @brief Class object for HAnimHumanoid nodes.
     */

    /**
     * @var hanim_humanoid_node::center_
     *
     * @brief center exposedField
     */

    /**
     * @var hanim_humanoid_node::info_
     *
     * @brief info exposedField
     */

    /**
     * @var hanim_humanoid_node::joints_
     *
     * @brief joints exposedField
     */

    /**
     * @var hanim_humanoid_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var hanim_humanoid_node::rotation_
     *
     * @brief rotation exposedField
     */

    /**
     * @var hanim_humanoid_node::scale_
     *
     * @brief scale exposedField
     */

    /**
     * @var hanim_humanoid_node::scale_orientation_
     *
     * @brief scale_orientation exposedField
     */

    /**
     * @var hanim_humanoid_node::segments_
     *
     * @brief segments exposedField
     */

    /**
     * @var hanim_humanoid_node::sites_
     *
     * @brief sites exposedField
     */

    /**
     * @var hanim_humanoid_node::skeleton_
     *
     * @brief skeleton exposedField
     */

    /**
     * @var hanim_humanoid_node::skin_
     *
     * @brief skin exposedField
     */

    /**
     * @var hanim_humanoid_node::skin_coord_
     *
     * @brief skin_coord exposedField
     */

    /**
     * @var hanim_humanoid_node::skin_normal_
     *
     * @brief skin_normal exposedField
     */

    /**
     * @var hanim_humanoid_node::translation_
     *
     * @brief translation exposedField
     */

    /**
     * @var hanim_humanoid_node::version_
     *
     * @brief version exposedField
     */

    /**
     * @var hanim_humanoid_node::viewpoints_
     *
     * @brief viewpoints exposedField
     */

    /**
     * @var hanim_humanoid_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var hanim_humanoid_node::bbox_size_
     *
     * @brief bbox_size field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    hanim_humanoid_node::
    hanim_humanoid_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        center_(*this),
        info_(*this),
        joints_(*this),
        name_(*this),
        rotation_(*this),
        scale_(*this, vec3f(1,1,1)),
        scale_orientation_(*this),
        segments_(*this),
        sites_(*this),
        skeleton_(*this),
        skin_(*this),
        skin_coord_(*this),
        skin_normal_(*this),
        translation_(*this),
        version_(*this),
        viewpoints_(*this),
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    hanim_humanoid_node::~hanim_humanoid_node() OPENVRML_NOTHROW
    {}


    /**
     * @class hanim_joint_node
     *
     * @brief Represents HAnimJoint node instances.
     */

    /**
     * @var hanim_joint_node::hanim_joint_metatype
     *
     * @brief Class object for HAnimJoint nodes.
     */

    /**
     * @var hanim_joint_node::add_children_
     *
     * @brief add_children eventIn
     */

    /**
     * @var hanim_joint_node::remove_children_
     *
     * @brief remove_children eventIn
     */

    /**
     * @var hanim_joint_node::center_
     *
     * @brief center exposedField
     */

    /**
     * @var hanim_joint_node::children_
     *
     * @brief children exposedField
     */

    /**
     * @var hanim_joint_node::displacers_
     *
     * @brief displacers exposedField
     */

    /**
     * @var hanim_joint_node::limit_orientation_
     *
     * @brief limit_orientation exposedField
     */

    /**
     * @var hanim_joint_node::llimit_
     *
     * @brief llimit exposedField
     */

    /**
     * @var hanim_joint_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var hanim_joint_node::rotation_
     *
     * @brief rotation exposedField
     */

    /**
     * @var hanim_joint_node::scale_
     *
     * @brief scale exposedField
     */

    /**
     * @var hanim_joint_node::scale_orientation_
     *
     * @brief scale_orientation exposedField
     */

    /**
     * @var hanim_joint_node::skin_coord_index_
     *
     * @brief skin_coord_index exposedField
     */

    /**
     * @var hanim_joint_node::skin_coord_weight_
     *
     * @brief skin_coord_weight exposedField
     */

    /**
     * @var hanim_joint_node::stiffness_
     *
     * @brief stiffness exposedField
     */

    /**
     * @var hanim_joint_node::translation_
     *
     * @brief translation exposedField
     */

    /**
     * @var hanim_joint_node::ulimit_
     *
     * @brief ulimit exposedField
     */

    /**
     * @var hanim_joint_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var hanim_joint_node::bbox_size_
     *
     * @brief bbox_size field
     */

    hanim_joint_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    hanim_joint_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void hanim_joint_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    hanim_joint_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    hanim_joint_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void hanim_joint_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    hanim_joint_node::
    hanim_joint_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        center_(*this),
        children_(*this),
        displacers_(*this),
        limit_orientation_(*this),
        llimit_(*this),
        name_(*this),
        rotation_(*this),
        scale_(*this, vec3f(1,1,1)),
        scale_orientation_(*this),
        skin_coord_index_(*this),
        skin_coord_weight_(*this),
        stiffness_(*this, vector<float>(3,0.0f)),
        translation_(*this),
        ulimit_(*this),
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    hanim_joint_node::~hanim_joint_node() OPENVRML_NOTHROW
    {}


    /**
     * @class hanim_segment_node
     *
     * @brief Represents HAnimSegment node instances.
     */

    /**
     * @var hanim_segment_node::hanim_segment_metatype
     *
     * @brief Class object for HAnimSegment nodes.
     */

    /**
     * @var hanim_segment_node::add_children_
     *
     * @brief add_children eventIn
     */

    /**
     * @var hanim_segment_node::remove_children_
     *
     * @brief remove_children eventIn
     */

    /**
     * @var hanim_segment_node::center_of_mass_
     *
     * @brief center_of_mass exposedField
     */

    /**
     * @var hanim_segment_node::children_
     *
     * @brief children exposedField
     */

    /**
     * @var hanim_segment_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var hanim_segment_node::displacers_
     *
     * @brief displacers exposedField
     */

    /**
     * @var hanim_segment_node::mass_
     *
     * @brief mass exposedField
     */

    /**
     * @var hanim_segment_node::moments_of_inertia_
     *
     * @brief moments_of_inertia exposedField
     */

    /**
     * @var hanim_segment_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var hanim_segment_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var hanim_segment_node::bbox_size_
     *
     * @brief bbox_size field
     */

    hanim_segment_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    hanim_segment_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void hanim_segment_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    hanim_segment_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    hanim_segment_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void hanim_segment_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    hanim_segment_node::
    hanim_segment_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        center_of_mass_(*this),
        children_(*this),
        coord_(*this),
        displacers_(*this),
        mass_(*this),
        moments_of_inertia_(*this, vector<float>(9,0.0f)),
        name_(*this),
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    hanim_segment_node::~hanim_segment_node() OPENVRML_NOTHROW
    {}


    /**
     * @class hanim_site_node
     *
     * @brief Represents HAnimSite node instances.
     */

    /**
     * @var hanim_site_node::hanim_site_metatype
     *
     * @brief Class object for HAnimSite nodes.
     */

    /**
     * @var hanim_site_node::add_children_
     *
     * @brief add_children eventIn
     */

    /**
     * @var hanim_site_node::remove_children_
     *
     * @brief remove_children eventIn
     */

    /**
     * @var hanim_site_node::center_
     *
     * @brief center exposedField
     */

    /**
     * @var hanim_site_node::children_
     *
     * @brief children exposedField
     */

    /**
     * @var hanim_site_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var hanim_site_node::rotation_
     *
     * @brief rotation exposedField
     */

    /**
     * @var hanim_site_node::scale_
     *
     * @brief scale exposedField
     */

    /**
     * @var hanim_site_node::scale_orientation_
     *
     * @brief scale_orientation exposedField
     */

    /**
     * @var hanim_site_node::translation_
     *
     * @brief translation exposedField
     */

    /**
     * @var hanim_site_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var hanim_site_node::bbox_size_
     *
     * @brief bbox_size field
     */

    hanim_site_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    hanim_site_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void hanim_site_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    hanim_site_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    hanim_site_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void hanim_site_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    hanim_site_node::
    hanim_site_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        center_(*this),
        children_(*this),
        name_(*this),
        rotation_(*this),
        scale_(*this, vec3f(1,1,1)),
        scale_orientation_(*this),
        translation_(*this),
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    hanim_site_node::~hanim_site_node() OPENVRML_NOTHROW
    {}
}
