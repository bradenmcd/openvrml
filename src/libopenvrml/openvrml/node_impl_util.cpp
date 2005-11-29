// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2005  Braden McDaniel
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

# include "node_impl_util.h"

/**
 * @namespace openvrml::node_impl_util
 *
 * @brief Utility classes to facilitate node implementations.
 *
 * While the API comprising the members of the <code>openvrml</code> namespace
 * provides everything <em>necessary</em> to implement nodes, node
 * implementation can be very verbose. The members of the
 * <code>openvrml::node_impl_util</code> namespace can make node implementation
 * more concise by abstracting and providing code that many node
 * implementations are likely to have in common.
 *
 * Specifically, <code>node_type_impl</code> centralizes the logic for
 * generalized field access. By using an instance of this class template
 * for your <code>openvrml::node_type</code> implementation you can avoid a lot
 * of tedious and repetitive code to implement
 * <code>openvrml::node::do_field</code>,
 * <code>openvrml::node::do_event_listener</code>, and
 * <code>openvrml::node::do_event_emitter</code>.
 */

/**
 * @class openvrml::node_impl_util::ptr_to_polymorphic_mem
 *
 * @brief Abstract base class for a pointer-to-polymorphic-member type.
 *
 * In conjunction with <code>ptr_to_polymorphic_mem_impl</code>, this class
 * circumvents the &ldquo;contravariance problem&rdquo; with pointer-to-member
 * types. That is, the type <code>Base Object::*</code> is not compatible with
 * the type <code>Derived Object::*</code>.
 */

/**
 * @fn openvrml::node_impl_util::ptr_to_polymorphic_mem<MemberBase, Object>::~ptr_to_polymorphic_mem() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn MemberBase & openvrml::node_impl_util::ptr_to_polymorphic_mem::deref(Object & obj)
 *
 * @brief Dereference.
 *
 * @return a reference to the base type of the member.
 */

/**
 * @fn const MemberBase & openvrml::node_impl_util::ptr_to_polymorphic_mem::deref(const Object & obj)
 *
 * @brief Dereference.
 *
 * @return a <code>const</code> reference to the base type of the member.
 */


/**
 * @class openvrml::node_impl_util::ptr_to_polymorphic_mem_impl
 *
 * @brief Concrete pointer-to-polymorphic-member type.
 *
 * @sa openvrml::node_impl_util::ptr_to_polymorphic_mem
 */

/**
 * @var Member Object::* openvrml::node_impl_util::ptr_to_polymorphic_mem_impl::ptr_to_mem
 *
 * @brief Pointer-to-member.
 */

/**
 * @fn openvrml::node_impl_util::ptr_to_polymorphic_mem_impl::ptr_to_polymorphic_mem_impl(Member Object::* ptr_to_mem)
 *
 * @brief Construct.
 *
 * @param ptr_to_mem    pointer-to-member.
 */

/**
 * @fn openvrml::node_impl_util::ptr_to_polymorphic_mem_impl::~ptr_to_polymorphic_mem_impl() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn MemberBase & openvrml::node_impl_util::ptr_to_polymorphic_mem_impl::deref(Object & obj)
 *
 * @brief Dereference.
 *
 * @return a reference to the base type of the member.
 */

/**
 * @fn const MemberBase & openvrml::node_impl_util::ptr_to_polymorphic_mem_impl::deref(const Object & obj)
 *
 * @brief Dereference.
 *
 * @return a <code>const</code> reference to the base type of the member.
 */


/**
 * @class openvrml::node_impl_util::abstract_node_type
 *
 * @brief Abstract base for <code>openvrml::node_type</code> subclasses.
 *
 * This class is realized by instances of the <code>node_type_impl</code> class
 * template.
 *
 * @sa openvrml::node_impl_util::node_type_impl
 */

/**
 * @brief Construct.
 *
 * @param node_class    the <code>node_class</code>.
 * @param id            the <code>node_type</code> identifier.
 */
openvrml::node_impl_util::abstract_node_type::
abstract_node_type(const openvrml::node_class & node_class,
                 const std::string & id):
    node_type(node_class, id)
{}

/**
 * @brief Destroy.
 */
openvrml::node_impl_util::abstract_node_type::~abstract_node_type() throw ()
{}

/**
 * @fn const openvrml::field_value & openvrml::node_impl_util::abstract_node_type::field_value(const openvrml::node & node, const std::string & id) const throw (openvrml::unsupported_interface)
 *
 * @brief @p node's field_value corresponding to @p id.
 *
 * @param node  the <code>openvrml::node</code> for which to return the
 *              <code>openvrml::field_value</code>.
 * @param id    field identifier.
 * 
 * @return @p node's <code>openvrml::field_value</code> corresponding to
 *         the field identifier @p id.
 *
 * @exception openvrml::unsupported_interface   if @p node has no field
 *                                              @p id.
 */

/**
 * @fn const openvrml::event_listener & openvrml::node_impl_util::abstract_node_type::event_listener(openvrml::node & node, const std::string & id) const throw (openvrml::unsupported_interface)
 *
 * @brief @p node's <code>openvrml::event_listener</code> corresponding to
 *        the eventIn identifier @p id.
 *
 * @param node  the <code>openvrml::node</code> for which to return the
 *              <code>openvrml::event_listener</code>.
 * @param id    eventIn identifier.
 * 
 * @return @p node's <code>openvrml::event_listener</code> corresponding to
 *         the eventIn identifier @p id.
 *
 * @exception openvrml::unsupported_interface   if @p node has no eventIn
 *                                              @p id.
 */

/**
 * @fn const openvrml::event_emitter & openvrml::node_impl_util::abstract_node_type::event_emitter(openvrml::node & node, const std::string & id) const throw (openvrml::unsupported_interface)
 *
 * @brief @p node's <code>openvrml::event_emitter</code> corresponding to
 *        the eventOut identifier @p id.
 *
 * @param node  the <code>openvrml::node</code> for which to return the
 *              <code>openvrml::event_emitter</code>.
 * @param id    eventOut identifier.
 * 
 * @return @p node's <code>openvrml::event_emitter</code> corresponding to
 *         the eventOut identifier @p id.
 *
 * @exception openvrml::unsupported_interface   if @p node has no eventOut
 *                                              @p id.
 */


/**
 * @class openvrml::node_impl_util::node_type_impl
 *
 * @brief A template for concrete <code>node_type</code>s.
 */

/**
 * @var class openvrml::node_impl_util::node_type_impl::event_listener_base<Node>
 *
 * @brief The <code>event_listener_base</code> class template accesses
 * <code>node_type_impl<Node>::event_listener_map</code>.
 */

/**
 * @var class openvrml::node_impl_util::node_type_impl::event_emitter_base<Node>
 *
 * @brief The <code>event_emitter_base</code> class template accesses
 * <code>node_type_impl<Node>::event_emitter_map</code>.
 */

/**
 * @typedef boost::shared_ptr<openvrml::node_impl_util::ptr_to_polymorphic_mem<openvrml::field_value, Node> > openvrml::node_impl_util::node_type_impl<Node>::field_ptr_ptr
 *
 * @brief A <code>shared_ptr</code> to a pointer-to-member wrapper
 *        template.
 */

/**
 * @typedef boost::shared_ptr<openvrml::node_impl_util::ptr_to_polymorphic_mem<openvrml::event_listener, Node> > openvrml::node_impl_util::node_type_impl<Node>::event_listener_ptr_ptr
 *
 * @brief A <code>shared_ptr</code> to a pointer-to-member wrapper
 *        template.
 */

/**
 * @typedef boost::shared_ptr<openvrml::node_impl_util::ptr_to_polymorphic_mem<openvrml::event_emitter, Node> > openvrml::node_impl_util::node_type_impl<Node>::event_emitter_ptr_ptr
 *
 * @brief A <code>shared_ptr</code> to a pointer-to-member wrapper
 *        template.
 */

/**
 * @var openvrml::node_interface_set openvrml::node_impl_util::node_type_impl<Node>::interfaces_
 *
 * @brief The set of <code>node_interface</code>s for the
 *        <code>node_type</code>.
 */

/**
 * @typedef std::map<std::string, openvrml::node_impl_util::node_type_impl<Node>::field_ptr_ptr> openvrml::node_impl_util::node_type_impl<Node>::field_value_map_t
 *
 * @brief Map of pointers to <code>openvrml::field_value</code> node
 *        members.
 */

/**
 * @typedef std::map<std::string, openvrml::node_impl_util::node_type_impl<Node>::event_listener_ptr_ptr> openvrml::node_impl_util::node_type_impl<Node>::event_listener_map_t
 *
 * @brief Map of pointers to <code>openvrml::event_listener</code> node
 *        members.
 */

/**
 * @typedef std::map<std::string, openvrml::node_impl_util::node_type_impl<Node>::event_emitter_ptr_ptr> openvrml::node_impl_util::node_type_impl<Node>::event_emitter_map_t
 *
 * @brief Map of pointers to <code>openvrml::event_emitter</code> node
 *        members.
 */

/**
 * @var openvrml::node_impl_util::node_type_impl<Node>::field_value_map_t openvrml::node_impl_util::node_type_impl<Node>::field_value_map
 *
 * @brief Map of pointers to <code>openvrml::field_value</code> node
 *        members.
 */

/**
 * @var openvrml::node_impl_util::node_type_impl<Node>::event_listener_map_t openvrml::node_impl_util::node_type_impl<Node>::event_listener_map
 *
 * @brief Map of pointers to <code>openvrml::event_listener</code> node
 *        members.
 */

/**
 * @var openvrml::node_impl_util::node_type_impl<Node>::event_emitter_map_t openvrml::node_impl_util::node_type_impl<Node>::event_emitter_map
 *
 * @brief Map of pointers to <code>openvrml::event_emitter</code> node
 *        members.
 */


/**
 * @class openvrml::node_impl_util::event_listener_base
 *
 * @brief Abstract base for <code>event_listener</code> implementations.
 *
 * <code>event_listener_base</code> implements
 * <code>event_listener::do_eventin_id</code>.
 */

/**
 * @typedef openvrml::node_impl_util::node_type_impl<Node> openvrml::node_impl_util::event_listener_base<Node>::node_type_t
 *
 * @brief The concrete <code>node_type</code>.
 */

/**
 * @typedef openvrml::node_impl_util::event_listener_base<Node>::node_type_t::event_listener_map_t openvrml::node_impl_util::event_listener_base<Node>::event_listener_map_t
 *
 * @brief The map of <code>event_listener</code>s.
 */

/**
 * @struct openvrml::node_impl_util::event_listener_base::event_listener_equal_to
 *
 * @brief A model of
 *        <a href="http://www.sgi.com/tech/stl/AdaptablePredicate.html">
 *        Adaptable Predicate</a>.
 *
 * @par Model of
 * <a href="http://www.sgi.com/tech/stl/AdaptablePredicate.html">Adaptable
 * Predicate</a>
 */

/**
 * @var const openvrml::event_listener * openvrml::node_impl_util::event_listener_base<Node>::event_listener_equal_to::listener_
 *
 * @brief The <code>event_listener</code> to test against.
 */

/**
 * @fn openvrml::node_impl_util::event_listener_base<Node>::event_listener_equal_to::event_listener_equal_to(const node_event_listener & listener)
 *
 * @brief Construct.
 *
 * @param listener  the <code>event_listener</code>.
 */

/**
 * @fn bool openvrml::node_impl_util::event_listener_base<Node>::event_listener_equal_to::operator()(const typename event_listener_equal_to::argument_type & arg) const
 *
 * @brief Test whether the <code>event_listener</code> dereferenced from
 *        @p arg.second is the same one the function object was constructed
 *        with.
 */

/**
 * @fn openvrml::node_impl_util::event_listener_base::event_listener_base(openvrml::node & n) throw ()
 *
 * @brief Construct.
 *
 * @param n the <code>node</code> associated with the
 *          <code>event_listener</code>.
 */

/**
 * @fn openvrml::node_impl_util::event_listener_base::~event_listener_base() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn const std::string openvrml::node_impl_util::event_listener_base::do_eventin_id() const throw ()
 *
 * @brief The associated eventIn identifier.
 *
 * @return the associated eventIn identifier.
 */


/**
 * @class openvrml::node_impl_util::event_emitter_base
 *
 * @brief Abstract base for <code>event_emitter</code> implementations.
 *
 * <code>event_emitter_base</code> implements
 * <code>openvrml::event_emitter::do_eventout_id</code>.
 */

/**
 * @var openvrml::node * openvrml::node_impl_util::event_emitter_base::node_
 *
 * @brief The node with which the <code>event_emitter</code> is associated.
 */

/**
 * @typedef openvrml::node_impl_util::node_type_impl<Node> openvrml::node_impl_util::event_emitter_base::node_type_t
 *
 * @brief The concrete <code>node_type</code>.
 */

/**
 * @typedef openvrml::node_impl_util::event_emitter_base::node_type_t::event_emitter_map_t openvrml::node_impl_util::event_emitter_base::event_emitter_map_t
 *
 * @brief The map of <code>event_emitter</code>s.
 */

/**
 * @struct openvrml::node_impl_util::event_emitter_base::event_emitter_equal_to
 *
 * @brief A model of
 *        <a href="http://www.sgi.com/tech/stl/AdaptablePredicate.html">
 *        Adaptable Predicate</a>.
 *
 * @par Model of
 * <a href="http://www.sgi.com/tech/stl/AdaptablePredicate.html">Adaptable
 * Predicate</a>
 */

/**
 * @var const openvrml::node_impl_util::event_emitter_base<Node> * openvrml::node_impl_util::event_emitter_base::event_emitter_equal_to::emitter_
 *
 * @brief The <code>event_emitter</code> to test against.
 */

/**
 * @fn openvrml::node_impl_util::event_emitter_base::event_emitter_equal_to::event_emitter_equal_to(const event_emitter_base<Node> & emitter)
 *
 * @brief Construct.
 *
 * @param emitter   the <code>event_emitter</code>.
 */

/**
 * @fn bool openvrml::node_impl_util::event_emitter_base::event_emitter_equal_to::operator()(const typename event_emitter_equal_to::argument_type & arg) const
 *
 * @brief Test whether the <code>event_emitter</code> dereferenced from
 *        @p arg.second is the same one the function object was constructed
 *        with.
 */

/**
 * @fn openvrml::node_impl_util::event_emitter_base::event_emitter_base(openvrml::node & n, const field_value & value) throw ()
 *
 * @brief Construct.
 *
 * @param n     the <code>node</code> associated with the
 *              <code>event_emitter</code>.
 * @param value the field value associated with the
 *              <code>event_emitter</code>.
 */

/**
 * @fn openvrml::node_impl_util::event_emitter_base::~event_emitter_base() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn openvrml::node & openvrml::node_impl_util::event_emitter_base::node() const throw ()
 *
 * @brief The node with which the <code>event_emitter</code> is associated.
 */

/**
 * @fn const std::string openvrml::node_impl_util::event_emitter_base::do_eventout_id() const throw ()
 *
 * @brief The associated eventOut identifier.
 *
 * @return the associated eventOut identifier.
 */


/**
 * @class openvrml::node_impl_util::abstract_node
 *
 * @brief Abstract base class for node implementations.
 *
 * <code>abstract_node</code> encapsulates the mechanisms for field access and
 * mutation, event dispatch, and eventOut access.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfbool> openvrml::node_impl_util::abstract_node::sfbool_listener
 *
 * @brief <code>openvrml::sfbool</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfcolor> openvrml::node_impl_util::abstract_node::sfcolor_listener
 *
 * @brief <code>openvrml::sfcolor</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sffloat> openvrml::node_impl_util::abstract_node::sffloat_listener
 *
 * @brief <code>openvrml::sffloat</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfdouble> openvrml::node_impl_util::abstract_node::sfdouble_listener
 *
 * @brief <code>openvrml::sfdouble</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfimage> openvrml::node_impl_util::abstract_node::sfimage_listener
 *
 * @brief <code>openvrml::sfimage</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfint32> openvrml::node_impl_util::abstract_node::sfint32_listener
 *
 * @brief <code>openvrml::sfint32</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfnode> openvrml::node_impl_util::abstract_node::sfnode_listener
 *
 * @brief <code>openvrml::sfnode</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfrotation> openvrml::node_impl_util::abstract_node::sfrotation_listener
 *
 * @brief <code>openvrml::sfrotation</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfstring> openvrml::node_impl_util::abstract_node::sfstring_listener
 *
 * @brief <code>openvrml::sfstring</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sftime> openvrml::node_impl_util::abstract_node::sftime_listener
 *
 * @brief <code>openvrml::sftime</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfvec2f> openvrml::node_impl_util::abstract_node::sfvec2f_listener
 *
 * @brief <code>openvrml::sfvec2f</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfvec2d> openvrml::node_impl_util::abstract_node::sfvec2d_listener
 *
 * @brief <code>openvrml::sfvec2d</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfvec3f> openvrml::node_impl_util::abstract_node::sfvec3f_listener
 *
 * @brief <code>openvrml::sfvec3f</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::sfvec3d> openvrml::node_impl_util::abstract_node::sfvec3d_listener
 *
 * @brief <code>openvrml::sfvec3d</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mfcolor> openvrml::node_impl_util::abstract_node::mfcolor_listener
 *
 * @brief <code>openvrml::mfcolor</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mffloat> openvrml::node_impl_util::abstract_node::mffloat_listener
 *
 * @brief <code>openvrml::mffloat</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mfdouble> openvrml::node_impl_util::abstract_node::mfdouble_listener
 *
 * @brief <code>openvrml::mfdouble</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mfint32> openvrml::node_impl_util::abstract_node::mfint32_listener
 *
 * @brief <code>openvrml::mfint32</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mfnode> openvrml::node_impl_util::abstract_node::mfnode_listener
 *
 * @brief <code>openvrml::mfnode</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mfrotation> openvrml::node_impl_util::abstract_node::mfrotation_listener
 *
 * @brief <code>openvrml::mfrotation</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mfstring> openvrml::node_impl_util::abstract_node::mfstring_listener
 *
 * @brief <code>openvrml::mfstring</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mftime> openvrml::node_impl_util::abstract_node::mftime_listener
 *
 * @brief <code>openvrml::mftime</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mfvec2f> openvrml::node_impl_util::abstract_node::mfvec2f_listener
 *
 * @brief <code>openvrml::mfvec2f</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mfvec2d> openvrml::node_impl_util::abstract_node::mfvec2d_listener
 *
 * @brief <code>openvrml::mfvec2d</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mfvec3f> openvrml::node_impl_util::abstract_node::mfvec3f_listener
 *
 * @brief <code>openvrml::mfvec3f</code> listener. Provided for convenience.
 */

/**
 * @typedef openvrml::node_field_value_listener<openvrml::mfvec3d> openvrml::node_impl_util::abstract_node::mfvec3d_listener
 *
 * @brief <code>openvrml::mfvec3d</code> listener. Provided for convenience.
 */

/**
 * @class openvrml::node_impl_util::abstract_node::event_emitter
 *
 * @brief Concrete event emitter class template.
 */

/**
 * @fn openvrml::node_impl_util::abstract_node::event_emitter::event_emitter(openvrml::node & node, const FieldValue & value)
 *
 * @brief Construct.
 *
 * @param node  node from which events will be emitted.
 * @param value associated field value.
 */

/**
 * @fn openvrml::node_impl_util::abstract_node::event_emitter::~event_emitter() throw ()
 *
 * @brief Destroy.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfbool> openvrml::node_impl_util::abstract_node::sfbool_emitter
 *
 * @brief <code>openvrml::sfbool</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfcolor> openvrml::node_impl_util::abstract_node::sfcolor_emitter
 *
 * @brief <code>openvrml::sfcolor</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfdouble> openvrml::node_impl_util::abstract_node::sfdouble_emitter
 *
 * @brief <code>openvrml::sfdouble</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sffloat> openvrml::node_impl_util::abstract_node::sffloat_emitter
 *
 * @brief <code>openvrml::sffloat</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfimage> openvrml::node_impl_util::abstract_node::sfimage_emitter
 *
 * @brief <code>openvrml::sfimage</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfnode> openvrml::node_impl_util::abstract_node::sfnode_emitter
 *
 * @brief <code>openvrml::sfnode</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfrotation> openvrml::node_impl_util::abstract_node::sfrotation_emitter
 *
 * @brief <code>openvrml::sfrotation</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfstring> openvrml::node_impl_util::abstract_node::sfstring_emitter
 *
 * @brief <code>openvrml::sfstring</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sftime> openvrml::node_impl_util::abstract_node::sftime_emitter
 *
 * @brief <code>openvrml::sftime</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfvec2d> openvrml::node_impl_util::abstract_node::sfvec2d_emitter
 *
 * @brief <code>openvrml::sfvec2d</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfvec2f> openvrml::node_impl_util::abstract_node::sfvec2f_emitter
 *
 * @brief <code>openvrml::sfvec2f</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfvec3d> openvrml::node_impl_util::abstract_node::sfvec3d_emitter
 *
 * @brief <code>openvrml::sfvec3d</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::sfvec3f> openvrml::node_impl_util::abstract_node::sfvec3f_emitter
 *
 * @brief <code>openvrml::sfvec3f</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mfcolor> openvrml::node_impl_util::abstract_node::mfcolor_emitter
 *
 * @brief <code>openvrml::mfcolor</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mfdouble> openvrml::node_impl_util::abstract_node::mfdouble_emitter
 *
 * @brief <code>openvrml::mfdouble</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mffloat> openvrml::node_impl_util::abstract_node::mffloat_emitter
 *
 * @brief <code>openvrml::mffloat</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mfnode> openvrml::node_impl_util::abstract_node::mfnode_emitter
 *
 * @brief <code>openvrml::mfnode</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mfrotation> openvrml::node_impl_util::abstract_node::mfrotation_emitter
 *
 * @brief <code>openvrml::mfrotation</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mfstring> openvrml::node_impl_util::abstract_node::mfstring_emitter
 *
 * @brief <code>openvrml::mfstring</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mftime> openvrml::node_impl_util::abstract_node::mftime_emitter
 *
 * @brief <code>openvrml::mftime</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mfvec2d> openvrml::node_impl_util::abstract_node::mfvec2d_emitter
 *
 * @brief <code>openvrml::mfvec2d</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mfvec2f> openvrml::node_impl_util::abstract_node::mfvec2f_emitter
 *
 * @brief <code>openvrml::mfvec2f</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mfvec3d> openvrml::node_impl_util::abstract_node::mfvec3d_emitter
 *
 * @brief <code>openvrml::mfvec3d</code> event emitter.
 */

/**
 * @typedef openvrml::node_impl_util::abstract_node::event_emitter<openvrml::mfvec3f> openvrml::node_impl_util::abstract_node::mfvec3f_emitter
 *
 * @brief <code>openvrml::mfvec3f</code> event emitter.
 */

/**
 * @class openvrml::node_impl_util::abstract_node::exposedfield
 *
 * @brief exposedField implementation.
 */

/**
 * @fn openvrml::node_impl_util::abstract_node::exposedfield::exposedfield(openvrml::node & node, const typename FieldValue::value_type & value)
 *
 * @brief Construct.
 *
 * @param node  the node.
 * @param value the initial value.
 */

/**
 * @fn openvrml::node_impl_util::abstract_node::exposedfield::exposedfield(const exposedfield<FieldValue> & obj) throw ()
 *
 * @brief Construct a copy.
 *
 * @param obj   the instance to copy.
 */

/**
 * @fn openvrml::node_impl_util::abstract_node::exposedfield::~exposedfield() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn std::auto_ptr<openvrml::field_value> openvrml::node_impl_util::abstract_node::exposedfield::do_clone() const throw (std::bad_alloc)
 *
 * @brief Polymorphically construct a copy.
 */

/**
 * @fn openvrml::node_impl_util::abstract_node::abstract_node(const node_type & type, const boost::shared_ptr<openvrml::scope> & scope)
 *
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope the scope to which the node belongs.
 */

/**
 * @fn openvrml::node_impl_util::abstract_node::~abstract_node() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn const openvrml::field_value & openvrml::node_impl_util::abstract_node::do_field(const std::string & id) const throw (unsupported_interface)
 *
 * @brief Get a field value for a node.
 *
 * @param id    a field name.
 *
 * @exception unsupported_interface  if the node has no field @p id.
 */

/**
 * @fn openvrml::event_listener & openvrml::node_impl_util::abstract_node::do_event_listener(const std::string & id) throw (unsupported_interface)
 *
 * @brief Get an event listener.
 *
 * This method is called by node::event_listener.
 *
 * @param id    eventIn identifier.
 *
 * @return the event listener.
 *
 * @exception unsupported_interface if the node has no eventIn @p id.
 */

/**
 * @fn openvrml::event_emitter & openvrml::node_impl_util::abstract_node::do_event_emitter(const std::string & id) throw (unsupported_interface)
 *
 * @brief Get an event emitter.
 *
 * This method is called by node::event_emitter.
 *
 * @param id    eventOut identifier.
 *
 * @return the event emitter.
 *
 * @exception unsupported_interface if the node has no eventOut @p id.
 */

/**
 * @class openvrml::node_impl_util::node_type_impl::field_ptr
 *
 * @brief Concrete pointer-to-member wrapper.
 */

/**
 * @fn openvrml::node_impl_util::node_type_impl::field_ptr::field_ptr(FieldMember Node::* ptr_to_mem)
 *
 * @brief Construct.
 *
 * @param ptr_to_mem    a pointer to an <code>openvrml::field_value</code>
 *                      member.
 */

/**
 * @class openvrml::node_impl_util::node_type_impl::event_listener_ptr
 *
 * @brief Concrete pointer-to-member wrapper.
 */

/**
 * @fn openvrml::node_impl_util::node_type_impl::event_listener_ptr::event_listener_ptr(EventListenerMember Node::* ptr_to_mem)
 *
 * @brief Construct
 *
 * @param ptr_to_mem    a pointer to an
 *                      <code>openvrml::event_listener</code> member.
 */

/**
 * @class openvrml::node_impl_util::node_type_impl::event_emitter_ptr
 *
 * @brief Concrete pointer-to-member wrapper.
 */

/**
 * @fn openvrml::node_impl_util::node_type_impl::event_emitter_ptr::event_emitter_ptr(EventEmitterMember Node::* ptr_to_mem)
 *
 * @brief Construct
 *
 * @param ptr_to_mem    a pointer to an
 *                      <code>openvrml::event_emitter</code> member.
 */

/**
 * @fn openvrml::node_impl_util::node_type_impl::node_type_impl(const openvrml::node_class & node_class, const std::string & id)
 *
 * @brief Construct.
 *
 * @param node_class    the <code>node_class</code>.
 * @param id            the <code>node_type</code> identifier.
 */

/**
 * @fn openvrml::node_impl_util::node_type_impl::~node_type_impl() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn void openvrml::node_impl_util::node_type_impl::add_eventin(const openvrml::field_value::type_id type, const std::string & id, const event_listener_ptr_ptr & event_listener) throw (std::invalid_argument, std::bad_alloc)
 *
 * @brief Add an eventIn.
 *
 * @param type              the field value type.
 * @param id                the eventIn identifier.
 * @param event_listener    the <code>openvrml::event_listener</code>
 *                          associated with the eventIn.
 *
 * @exception std::invalid_argument if an interface with a conflicting
 *                                  @p id has already been added to the
 *                                  <code>node_type</code>.
 * @exception std::bad_alloc        if memory allocation fails.
 */

/**
 * @fn void openvrml::node_impl_util::node_type_impl::add_eventout(const openvrml::field_value::type_id type, const std::string & id, const event_emitter_ptr_ptr & event_emitter) throw (std::invalid_argument, std::bad_alloc)
 *
 * @brief Add an eventOut.
 *
 * @param type          the field value type.
 * @param id            the eventOut identifier.
 * @param event_emitter the <code>openvrml::event_emitter</code>
 *                      associated with the eventOut.
 *
 * @exception std::invalid_argument if an interface with a conflicting
 *                                  @p id has already been added to the
 *                                  <code>node_type</code>.
 * @exception std::bad_alloc        if memory allocation fails.
 */

/**
 * @fn void openvrml::node_impl_util::node_type_impl::add_exposedfield(const openvrml::field_value::type_id type, const std::string & id, const event_listener_ptr_ptr & event_listener, const field_ptr_ptr & field, const event_emitter_ptr_ptr & event_emitter) throw (std::invalid_argument, std::bad_alloc)
 *
 * @brief Add an exposedField.
 *
 * @param type              the field value type.
 * @param id                the exposedField identifier.
 * @param event_listener    the <code>openvrml::event_listener</code>
 *                          associated with the exposedField.
 * @param field             the <code>openvrml::field_value</code>
 *                          associated with the exposedField.
 * @param event_emitter     the <code>openvrml::event_emitter</code>
 *                          associated with the exposedField.
 *
 * @exception std::invalid_argument if an interface with a conflicting
 *                                  @p id has already been added to the
 *                                  <code>node_type</code>.
 * @exception std::bad_alloc        if memory allocation fails.
 */

/**
 * @fn void openvrml::node_impl_util::node_type_impl::add_field(const openvrml::field_value::type_id type, const std::string & id, const field_ptr_ptr & nodeFieldPtrPtr) throw (std::invalid_argument, std::bad_alloc)
 *
 * @brief Add a field.
 *
 * @param type              the field value type.
 * @param id                the field identifier.
 * @param field             the <code>openvrml::field_value</code>
 *                          associated with the field.
 *
 * @exception std::invalid_argument if an interface with a conflicting
 *                                  @p id has already been added to the
 *                                  <code>node_type</code>.
 * @exception std::bad_alloc        if memory allocation fails.
 */

/**
 * @fn const openvrml::field_value & openvrml::node_impl_util::node_type_impl::field_value(const openvrml::node & node, const std::string & id) const throw (openvrml::unsupported_interface)
 *
 * @brief @p node's <code>openvrml::field_value</code> corresponding to the
 *        field identifier @p id.
 *
 * Delegates to <code>node_type_impl<Node>::do_field_value</code>.
 *
 * @param node  the <code>openvrml::node</code> for which to return the
 *              <code>openvrml::field_value</code>.
 * @param id    field identifier.
 * 
 * @return @p node's <code>openvrml::field_value</code> corresponding to
 *         the field identifier @p id.
 *
 * @exception openvrml::unsupported_interface   if @p node has no field
 *                                              @p id.
 */

/**
 * @fn const openvrml::field_value & openvrml::node_impl_util::node_type_impl::do_field_value(const Node & node, const std::string & id) const throw (openvrml::unsupported_interface)
 *
 * @brief @p node's <code>openvrml::field_value</code> corresponding to the
 *        field identifier @p id.
 *
 * @param node  the node for which to return the
 *              <code>openvrml::field_value</code>.
 * @param id    field identifier.
 * 
 * @return @p node's <code>openvrml::field_value</code> corresponding to
 *         the field identifier @p id.
 *
 * @exception openvrml::unsupported_interface   if @p node has no field
 *                                              @p id.
 */

/**
 * @fn openvrml::event_listener & openvrml::node_impl_util::node_type_impl::event_listener(openvrml::node & node, const std::string & id) const throw (openvrml::unsupported_interface)
 *
 * @brief @p node's <code>openvrml::event_listener</code> corresponding to
 *        the eventIn identifier @p id.
 *
 * Delegates to
 * <code>node_type_impl<Node>::do_event_listener</code>.
 *
 * @param node  the <code>openvrml::node</code> for which to return the
 *              <code>openvrml::event_listener</code>.
 * @param id    eventIn identifier.
 * 
 * @return @p node's <code>openvrml::event_listener</code> corresponding to
 *         the eventIn identifier @p id.
 *
 * @exception openvrml::unsupported_interface   if @p node has no eventIn
 *                                              @p id.
 */

/**
 * @fn openvrml::event_listener & openvrml::node_impl_util::node_type_impl::do_event_listener(Node & node, const std::string & id) const throw (openvrml::unsupported_interface)
 *
 * @brief @p node's <code>openvrml::event_listener</code> corresponding to
 *        the eventIn identifier @p id.
 *
 * @param node  the <code>openvrml::node</code> for which to return the
 *              <code>openvrml::event_listener</code>.
 * @param id    eventIn identifier.
 * 
 * @return @p node's <code>openvrml::event_listener</code> corresponding to
 *         the eventIn identifier @p id.
 *
 * @exception openvrml::unsupported_interface   if @p node has no eventIn
 *                                              @p id.
 */

/**
 * @fn openvrml::event_emitter & openvrml::node_impl_util::node_type_impl::event_emitter(openvrml::node & node, const std::string & id) const throw (openvrml::unsupported_interface)
 *
 * @brief @p node's <code>openvrml::event_emitter</code> corresponding to
 *        the eventOut identifier @p id.
 *
 * Delegates to <code>node_type_impl<Node>::do_event_emitter</code>.
 *
 * @param node  the <code>openvrml::node</code> for which to return the
 *              <code>openvrml::event_emitter</code>.
 * @param id    eventOut identifier.
 * 
 * @return @p node's <code>openvrml::event_emitter</code> corresponding to
 *         the eventOut identifier @p id.
 *
 * @exception openvrml::unsupported_interface   if @p node has no eventOut
 *                                              @p id.
 */

/**
 * @fn openvrml::event_emitter & openvrml::node_impl_util::node_type_impl::do_event_emitter(Node & node, const std::string & id) const throw (openvrml::unsupported_interface)
 *
 * @brief @p node's <code>openvrml::event_emitter</code> corresponding to
 *        the eventOut identifier @p id.
 *
 * Delegates to <code>node_type_impl<Node>::do_event_emitter</code>.
 *
 * @param node  the <code>openvrml::node</code> for which to return the
 *              <code>openvrml::event_emitter</code>.
 * @param id    eventOut identifier.
 * 
 * @return @p node's <code>openvrml::event_emitter</code> corresponding to
 *         the eventOut identifier @p id.
 *
 * @exception openvrml::unsupported_interface   if @p node has no eventOut
 *                                              @p id.
 */

/**
 * @fn const openvrml::node_interface_set & openvrml::node_impl_util::node_type_impl::do_interfaces() const throw ()
 *
 * @brief The set of <code>node_interface</code>s supported by the
 *        <code>node_type</code>.
 *
 * @return the set of <code>node_interface</code>s supported by the
 *         <code>node_type</code>.
 */

/**
 * @fn const boost::intrusive_ptr<openvrml::node> openvrml::node_impl_util::node_type_impl::do_create_node(const boost::shared_ptr<openvrml::scope> & scope, const openvrml::initial_value_map & initial_values) const throw (openvrml::unsupported_interface, std::bad_cast, std::bad_alloc)
 *
 * @brief Create a node instance.
 *
 * @param scope             the <code>scope</code> the new node will belong to.
 * @param initial_values    initial values for the new node's fields.
 *
 * @return a new node instance.
 *
 * @exception openvrml::unsupported_interface   if a field identifier in
 *                                              @p initial_values does not
 *                                              correspond to a field for the
 *                                              <code>node_type</code>.
 * @exception std::bad_cast                     if a field value type in
 *                                              @p initial_values is incorrect
 *                                              for the corresponding field for
 *                                              the <code>node_type</code>.
 * @exception std::bad_alloc                    if memory allocation fails.
 */
