// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2000, 2001, 2002, 2003, 2004  Braden McDaniel
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

# include <cassert>
# include <boost/thread/recursive_mutex.hpp>
# include "node_ptr.h"
# include "browser.h"

/**
 * @class openvrml::node_ptr
 *
 * @brief A reference-counted smart pointer for <code>node</code>s.
 */

/**
 * @var openvrml::node_ptr::script_node
 *
 * @brief Script nodes can be self-referential, so <code>node_ptr</code> works
 *        some special magic.
 */

namespace {
    typedef std::map<openvrml::node *, size_t> count_map_t;
    count_map_t count_map;
    boost::recursive_mutex count_map_mutex;
}

/**
 * @internal
 *
 * @var std::map<node *, size_t>::value_type * openvrml::node_ptr::count_ptr
 *
 * @brief Pointer to an entry in count_map.
 */

namespace {

    class self_ref_node : public openvrml::node {
    public:
        self_ref_node();
        virtual ~self_ref_node() throw ();

    private:
        virtual const openvrml::field_value &
        do_field(const std::string & id) const
            throw (openvrml::unsupported_interface);
        virtual openvrml::event_listener &
        do_event_listener(const std::string & id)
            throw (openvrml::unsupported_interface);
        virtual openvrml::event_emitter &
        do_event_emitter(const std::string & id)
            throw (openvrml::unsupported_interface);
    };

    //
    // Since nothing should ever actually use the self_ref_node instance,
    // there's no need to go to the trouble to give it a valid node_type.
    //
    char not_remotely_a_node_type;
    self_ref_node::self_ref_node():
        node(reinterpret_cast<const openvrml::node_type &>(
                 not_remotely_a_node_type),
             boost::shared_ptr<openvrml::scope>())
    {}

    self_ref_node::~self_ref_node() throw ()
    {}

    const openvrml::field_value &
    self_ref_node::do_field(const std::string &) const
        throw (openvrml::unsupported_interface)
    {
        static const openvrml::sfbool val;
        return val;
    }

    openvrml::event_listener &
    self_ref_node::do_event_listener(const std::string &)
        throw (openvrml::unsupported_interface)
    {
        class dummy_listener : public openvrml::sfbool_listener {
        public:
            dummy_listener(self_ref_node & n):
                openvrml::event_listener(n),
                openvrml::sfbool_listener(n)
            {}

            virtual ~dummy_listener() throw ()
            {}

        private:
            virtual const std::string do_eventin_id() const throw ()
            {
                return std::string();
            }

            virtual void do_process_event(const openvrml::sfbool &, double)
                throw (std::bad_alloc)
            {}
        };

        static dummy_listener listener(*this);
        return listener;
    }

    openvrml::event_emitter &
    self_ref_node::do_event_emitter(const std::string &)
        throw (openvrml::unsupported_interface)
    {
        class dummy_emitter : public openvrml::sfbool_emitter {
        public:
            explicit dummy_emitter(const openvrml::sfbool & value):
                openvrml::event_emitter(value),
                openvrml::sfbool_emitter(value)
            {}

        private:
            virtual const std::string do_eventout_id() const throw ()
            {
                return std::string();
            }            
        };

        openvrml::sfbool val;
        static dummy_emitter emitter(val);
        return emitter;
    }
}

/**
 * @brief Special value used when initializing a script_node.
 *
 * One should never attempt to dereference this value. It is useful only
 * for comparison.
 */
const openvrml::node_ptr openvrml::node_ptr::self(new self_ref_node);

/**
 * @brief Construct.
 *
 * @param node a pointer to a node
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::node_ptr::node_ptr(node * const node) throw (std::bad_alloc):
    count_ptr(0)
{
    boost::recursive_mutex::scoped_lock lock(count_map_mutex);
    if (node) {
        count_map_t::iterator pos = count_map.find(node);
        if (pos == count_map.end()) {
            const std::pair<count_map_t::iterator, bool>
                    result(count_map.insert(count_map_t::value_type(node, 0)));
            assert(result.second);
            assert(result.first->first == node);
            pos = result.first;
        }
        ++pos->second;
        this->count_ptr = &*pos;
    }
}

/**
 * @brief Construct a copy.
 *
 * @param ptr
 */
openvrml::node_ptr::node_ptr(const node_ptr & ptr) throw ():
    count_ptr(ptr.count_ptr)
{
    boost::recursive_mutex::scoped_lock lock(count_map_mutex);
    if (this->count_ptr) {
        ++this->count_ptr->second;
    }
}

/**
 * @fn openvrml::node_ptr::~node_ptr()
 *
 * @brief Destructor.
 */

/**
 * @fn openvrml::node_ptr::operator bool() const
 *
 * @brief Automatic conversion to bool.
 */

/**
 * @fn openvrml::node_ptr & openvrml::node_ptr::operator=(const node_ptr &)
 *
 * @brief Assignment operator.
 *
 * @param ptr
 */

/**
 * @fn openvrml::node & openvrml::node_ptr::operator*() const
 *
 * @brief Dereference operator.
 *
 * @return a reference to the underlying node.
 */

/**
 * @fn openvrml::node * openvrml::node_ptr::operator->() const
 *
 * @brief Access a method of the node.
 */

/**
 * @fn openvrml::node * openvrml::node_ptr::get() const
 *
 * @brief Get a raw pointer to the node.
 *
 * @return a raw pointer to the underlying node.
 */

/**
 * @fn void openvrml::node_ptr::swap(node_ptr & ptr) throw ()
 *
 * @brief Swap the values of the node_ptr and @p ptr.
 */

/**
 * @brief Reset the node_ptr to point to a different node.
 *
 * @param node
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::node_ptr::reset(node * const node) throw (std::bad_alloc)
{
    boost::recursive_mutex::scoped_lock lock(count_map_mutex);
    if (this->count_ptr && this->count_ptr->first == node) {
        return;
    }
    this->dispose();
    if (node) {
        std::pair<count_map_t::iterator, bool>
                result(count_map.insert(count_map_t::value_type(node, 1)));
        assert(result.first->first == node);
        this->count_ptr = &*result.first;
        if (!result.second) { // The node already exists in the table.
            ++this->count_ptr->second;
        }
    }
}

/**
 * @brief Relinquish ownership of the node.
 *
 * Decrement the reference count; if it drops to zero, call node::shutdown
 * on the node, delete the node, and remove its entry from the count map.
 */
void openvrml::node_ptr::dispose() throw ()
{
    boost::recursive_mutex::scoped_lock lock(count_map_mutex);
    if (this->count_ptr) {
        --this->count_ptr->second;
        if (this->count_ptr->second == 0) {
            this->count_ptr->first->shutdown(browser::current_time());
            delete this->count_ptr->first;
            count_map.erase(this->count_ptr->first);
        }
        this->count_ptr = 0;
    }
}

/**
 * @brief Share ownership of a node.
 *
 * @param count_ptr a pointer to an entry in the count map to share.
 */
void
openvrml::node_ptr::share(std::map<node *, size_t>::value_type * count_ptr)
    throw ()
{
    boost::recursive_mutex::scoped_lock lock(count_map_mutex);
    if (this->count_ptr != count_ptr) {
        if (count_ptr) { ++count_ptr->second; }
        this->dispose();
        this->count_ptr = count_ptr;
    }
}

/**
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs point to the same node; @c false
 *         otherwise.
 */
bool openvrml::operator==(const node_ptr & lhs, const node_ptr & rhs) throw ()
{
    return lhs.count_ptr == rhs.count_ptr;
}

/**
 * @relatesalso node_ptr
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs point to different
 *         @link openvrml::node nodes@endlink; @c false otherwise.
 */
bool openvrml::operator!=(const node_ptr & lhs, const node_ptr & rhs) throw ()
{
    return !(lhs == rhs);
}
