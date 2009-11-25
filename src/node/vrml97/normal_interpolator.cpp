// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
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

# include "normal_interpolator.h"
# include <openvrml/node_impl_util.h>
# include <openvrml/local/float.h>
# include <private.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL normal_interpolator_node :
        public openvrml::node_impl_util::abstract_node<normal_interpolator_node>,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::normal_interpolator_metatype;

        class set_fraction_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                      public sffloat_listener {
        public:
            explicit set_fraction_listener(normal_interpolator_node & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_fraction_listener set_fraction_listener_;
        exposedfield<openvrml::mffloat> key_;
        exposedfield<openvrml::mfvec3f> key_value_;
        openvrml::mfvec3f value_changed_;
        mfvec3f_emitter value_changed_emitter_;

    public:
        normal_interpolator_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~normal_interpolator_node() OPENVRML_NOTHROW;
    };

    /**
     * @class normal_interpolator_node
     *
     * @brief NormalInterpolator node instances.
     */

    /**
     * @var class normal_interpolator_node::normal_interpolator_metatype
     *
     * @brief Class object for NormalInterpolator nodes.
     */

    /**
     * @internal
     *
     * @class normal_interpolator_node::set_fraction_listener
     *
     * @brief set_fraction eventIn handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  normal_interpolator_node.
     */
    normal_interpolator_node::set_fraction_listener::
    set_fraction_listener(normal_interpolator_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<normal_interpolator_node>(node),
        sffloat_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    normal_interpolator_node::set_fraction_listener::
    ~set_fraction_listener()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param fraction  fraction.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    normal_interpolator_node::set_fraction_listener::
    do_process_event(const openvrml::sffloat & fraction, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            using std::vector;
            using openvrml::vec3f;

            normal_interpolator_node & node =
                dynamic_cast<normal_interpolator_node &>(this->node());

            const vector<float> & key = node.key_.mffloat::value();
            const vector<vec3f> & key_value = node.key_value_.mfvec3f::value();
            vector<vec3f> value = node.value_changed_.mfvec3f::value();

            size_t nNormals = key_value.size() / key.size();
            size_t n = key.size() - 1;

            if (fraction.value() < key[0]) {
                value.assign(key_value.begin(), key_value.begin() + nNormals);
            } else if (fraction.value() > key[n]) {
                value.assign(key_value.begin() + n * nNormals,
                             key_value.begin() + (n + 1) * nNormals);
            } else {
                // Reserve enough space for the new value
                value.resize(nNormals);

                for (size_t i = 0; i < n; ++i) {
                    if (key[i] <= fraction.value()
                        && fraction.value() <= key[i + 1]) {

                        vector<vec3f>::const_iterator v1 =
                            key_value.begin() + i * nNormals;
                        vector<vec3f>::const_iterator v2 =
                            key_value.begin() + (i + 1) * nNormals;

                        const float f =
                            (fraction.value() - key[i])
                            / (key[i + 1] - key[i]);

                        // Interpolate on the surface of unit sphere.
                        for (size_t j = 0; j < nNormals; ++j) {
                            using openvrml::local::fequal;

                            float alpha, beta;
                            const float dot_product = v1->dot(*v2);
                            if (!fequal(dot_product, 1.0f)
                                && v1->normalize() != v2->normalize()) {
                                // Vectors are not opposite and not coincident.
                                const float omega = float(acos(dot_product));
                                const float sinomega = float(sin(omega));
                                alpha =
                                    float(sin((1.0 - f) * omega)) / sinomega;
                                beta = float(sin(f * omega)) / sinomega;
                            } else {
                                // Do linear interpolation.
                                alpha = 1.0f - f;
                                beta = f;
                            }
                            value[j] = (alpha * *v1) + (beta * *v2);

                            ++v1;
                            ++v2;
                        }
                        break;
                    }
                }
            }

            node.value_changed_.mfvec3f::value(value);

            // Send the new value
            node::emit_event(node.value_changed_emitter_, timestamp);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var normal_interpolator_node::set_fraction_listener normal_interpolator_node::set_fraction_listener_
     *
     * @brief set_fraction eventIn handler.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<normal_interpolator_node>::exposedfield<openvrml::mffloat> normal_interpolator_node::key_
     *
     * @brief key exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<normal_interpolator_node>::exposedfield<openvrml::mfvec3f> normal_interpolator_node::key_value_
     *
     * @brief keyValue exposedField.
     */

    /**
     * @var openvrml::mfvec3f normal_interpolator_node::value_changed_
     *
     * @brief value_changed eventOut value.
     */

    /**
     * @var openvrml::mfvec3f_emitter normal_interpolator_node::value_changed_emitter_
     *
     * @brief value_changed eventOut emitter.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    normal_interpolator_node::
    normal_interpolator_node(const openvrml::node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<normal_interpolator_node>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        key_(*this),
        key_value_(*this),
        value_changed_emitter_(*this, this->value_changed_)
    {}

    /**
     * @brief Destroy.
     */
    normal_interpolator_node::~normal_interpolator_node()
        OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::normal_interpolator_metatype::id =
    "urn:X-openvrml:node:NormalInterpolator";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::normal_interpolator_metatype::
normal_interpolator_metatype(openvrml::browser & browser):
    node_metatype(normal_interpolator_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::normal_interpolator_metatype::
~normal_interpolator_metatype()
    OPENVRML_NOTHROW
{}

# define NORMAL_INTERPOLATOR_INTERFACE_SEQ                             \
    ((eventin,      sffloat, "set_fraction",  set_fraction_listener_)) \
    ((exposedfield, mffloat, "key",           key_))                   \
    ((exposedfield, mfvec3f, "keyValue",      key_value_))             \
    ((eventout,     mfvec3f, "value_changed", value_changed_emitter_)) \
    ((exposedfield, sfnode,  "metadata",      metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              normal_interpolator_metatype,
                                              normal_interpolator_node,
                                              NORMAL_INTERPOLATOR_INTERFACE_SEQ)
