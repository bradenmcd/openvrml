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

/**
 * @class VrmlNodePtr
 * A reference-counted smart pointer for <code>VrmlNode</code>s.
 */
# include "VrmlNodePtr.h"
# include "VrmlNode.h"

namespace {
    CountMap countMap;
}

/**
 * @brief Constructor.
 *
 * @param node a pointer to a VrmlNode
 */
VrmlNodePtr::VrmlNodePtr(VrmlNode * node) {
    if (node) {
        std::pair<CountMap::iterator, bool>
                result(countMap.insert(CountMap::value_type(node, 1)));
        assert(result.first->first == node);
        this->countPtr = &*result.first;
        if (!result.second) {
            ++this->countPtr->second;
        }
    } else {
        this->countPtr = 0;
    }
}

/**
 * @brief Copy constructor.
 *
 * @param nodePtr
 */
VrmlNodePtr::VrmlNodePtr(const VrmlNodePtr & nodePtr):
        countPtr(nodePtr.countPtr) {
    if (this->countPtr) {
        ++this->countPtr->second;
    }
}

/**
 * @brief Assignment operator.
 *
 * @param nodePtr
 */
VrmlNodePtr & VrmlNodePtr::operator=(const VrmlNodePtr & nodePtr) {
    if (this->countPtr != nodePtr.countPtr) {
        this->dispose();
        if (this->countPtr = nodePtr.countPtr) {
            ++this->countPtr->second;
        }
    }
    return *this;
}

/**
 * @brief Reset the VrmlNodePtr to point to a different VrmlNode.
 *
 * @param node
 */
void VrmlNodePtr::reset(VrmlNode * node) {
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

void VrmlNodePtr::dispose() {
    if (this->countPtr) {
        --this->countPtr->second;
        if (this->countPtr->second == 0) {
            delete this->countPtr->first;
            countMap.erase(this->countPtr->first);
        }
        this->countPtr = 0;
    }
}

/**
 * @fn VrmlNodePtr::~VrmlNodePtr()
 *
 * @brief Destructor.
 */

/**
 * @fn VrmlNodePtr::operator bool() const
 *
 * @brief Automatic conversion to bool.
 */

/**
 * @fn VrmlNode & VrmlNodePtr::operator*() const
 *
 * @brief Dereference operator.
 *
 * @return a reference to the underlying VrmlNode.
 */

/**
 * @fn VrmlNode * VrmlNodePtr::operator->() const
 *
 * @brief Access a method of the VrmlNode.
 */

/**
 * @fn VrmlNode * VrmlNodePtr::get() const
 *
 * @brief Get a raw pointer to the VrmlNode.
 *
 * @return a raw pointer to the underlying VrmlNode.
 */

/**
 * @fn bool VrmlNodePtr::operator==(const VrmlNodePtr & nodePtr) const
 *
 * @brief Compare two VrmlNodePtrs.
 *
 * @return true if both VrmlNodePtrs point to the same VrmlNode; false
 *         otherwise.
 */
