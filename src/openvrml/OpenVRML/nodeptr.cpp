// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 2000  Braden McDaniel
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

# include <assert.h>
# include "nodeptr.h"
# include "browser.h"

namespace OpenVRML {

/**
 * @class NodePtr
 *
 * @brief A reference-counted smart pointer for @link node nodes@endlink.
 */

/**
 * @var NodePtr::ScriptNode
 *
 * @brief Script nodes can be self-referential, so NodePtr works some special
 *      magic.
 */

namespace {
    typedef std::map<OpenVRML::node *, size_t> CountMap;
    CountMap countMap;
}

/**
 * @brief Constructor.
 *
 * @param node a pointer to a Node
 */
NodePtr::NodePtr(node * node):
    countPtr(0)
{
    if (node) {
        CountMap::iterator pos = countMap.find(node);
        if (pos == countMap.end()) {
            const std::pair<CountMap::iterator, bool>
                    result(countMap.insert(CountMap::value_type(node, 0)));
            assert(result.second);
            assert(result.first->first == node);
            pos = result.first;
        }
        ++pos->second;
        this->countPtr = &*pos;
    }
}

/**
 * @brief Copy constructor.
 *
 * @param nodePtr
 */
NodePtr::NodePtr(const NodePtr & nodePtr):
    countPtr(nodePtr.countPtr)
{
    if (this->countPtr) {
        ++this->countPtr->second;
    }
}

/**
 * @brief Reset the NodePtr to point to a different Node.
 *
 * @param node
 */
void NodePtr::reset(node * node)
{
    if (this->countPtr && this->countPtr->first == node) {
        return;
    }
    this->dispose();
    if (node) {
        std::pair<CountMap::iterator, bool>
                result(countMap.insert(CountMap::value_type(node, 1)));
        assert(result.first->first == node);
        this->countPtr = &*result.first;
        if (!result.second) { // The node already exists in the table.
            ++this->countPtr->second;
        }
    }
}

void NodePtr::dispose() throw ()
{
    if (this->countPtr) {
        --this->countPtr->second;
        if (this->countPtr->second == 0) {
            this->countPtr->first->shutdown(Browser::getCurrentTime());
            delete this->countPtr->first;
            countMap.erase(this->countPtr->first);
        }
        this->countPtr = 0;
    }
}

void NodePtr::share(CountMap::value_type * countPtr) throw ()
{
    if (this->countPtr != countPtr) {
        if (countPtr) { ++countPtr->second; }
        this->dispose();
        this->countPtr = countPtr;
    }
}

/**
 * @fn NodePtr::~NodePtr()
 *
 * @brief Destructor.
 */

/**
 * @fn NodePtr::operator bool() const
 *
 * @brief Automatic conversion to bool.
 */

/**
 * @fn NodePtr & NodePtr::operator=(const NodePtr &)
 *
 * @brief Assignment operator.
 *
 * @param nodePtr
 */

/**
 * @fn node & NodePtr::operator*() const
 *
 * @brief Dereference operator.
 *
 * @return a reference to the underlying Node.
 */

/**
 * @fn node * NodePtr::operator->() const
 *
 * @brief Access a method of the Node.
 */

/**
 * @fn node * NodePtr::get() const
 *
 * @brief Get a raw pointer to the Node.
 *
 * @return a raw pointer to the underlying Node.
 */

/**
 * @fn bool NodePtr::operator==(const NodePtr & nodePtr) const
 *
 * @brief Compare two NodePtrs.
 *
 * @return @c true if both NodePtrs point to the same Node; @c false
 *         otherwise.
 */

} // namespace OpenVRML
