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

# include "VrmlNodePtr.h"
# include "node.h"

using namespace OpenVRML;

/**
 * @class OpenVRML::NodePtr
 *
 * @brief A reference-counted smart pointer for <code>Node</code>s.
 */

namespace {
    CountMap countMap;
}

/**
 * @brief Constructor.
 *
 * @param node a pointer to a Node
 */
OpenVRML::NodePtr::NodePtr(Node * node): countPtr(0) {
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
OpenVRML::NodePtr::NodePtr(const NodePtr & nodePtr):
        countPtr(nodePtr.countPtr) {
    if (this->countPtr) {
        ++this->countPtr->second;
    }
}

/**
 * @brief Reset the NodePtr to point to a different Node.
 *
 * @param node
 */
void OpenVRML::NodePtr::reset(Node * node) {
    if (this->countPtr && this->countPtr->first == node) {
        return;
    }
    this->dispose();
    std::pair<CountMap::iterator, bool>
            result(countMap.insert(CountMap::value_type(node, 1)));
    assert(result.first->first == node);
    this->countPtr = &*result.first;
    if (!result.second) { // The node already exists in the table.
        ++this->countPtr->second;
    }
}

void OpenVRML::NodePtr::dispose() throw () {
    if (this->countPtr) {
        --this->countPtr->second;
        if (this->countPtr->second == 0) {
            delete this->countPtr->first;
            countMap.erase(this->countPtr->first);
        }
        this->countPtr = 0;
    }
}

void OpenVRML::NodePtr::share(CountMap::value_type * countPtr) throw () {
    if (this->countPtr != countPtr) {
        ++countPtr->second;
        this->dispose();
        this->countPtr = countPtr;
    }
}

/**
 * @fn OpenVRML::NodePtr::~NodePtr()
 *
 * @brief Destructor.
 */

/**
 * @fn OpenVRML::NodePtr::operator bool() const
 *
 * @brief Automatic conversion to bool.
 */

/**
 * @fn OpenVRML::NodePtr & OpenVRML::NodePtr::operator=(const NodePtr &)
 *
 * @brief Assignment operator.
 *
 * @param nodePtr
 */

/**
 * @fn OpenVRML::Node & OpenVRML::NodePtr::operator*() const
 *
 * @brief Dereference operator.
 *
 * @return a reference to the underlying Node.
 */

/**
 * @fn OpenVRML::Node * OpenVRML::NodePtr::operator->() const
 *
 * @brief Access a method of the Node.
 */

/**
 * @fn OpenVRML::Node * OpenVRML::NodePtr::get() const
 *
 * @brief Get a raw pointer to the Node.
 *
 * @return a raw pointer to the underlying Node.
 */

/**
 * @fn bool OpenVRML::NodePtr::operator==(const NodePtr & nodePtr) const
 *
 * @brief Compare two NodePtrs.
 *
 * @return true if both NodePtrs point to the same Node; false
 *         otherwise.
 */
