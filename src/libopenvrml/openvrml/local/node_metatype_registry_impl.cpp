// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2008  Braden McDaniel
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

# include "node_metatype_registry_impl.h"
# include <openvrml/browser.h>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <sstream>

using namespace boost::multi_index::detail;  // for scope_guard

const char openvrml::local::node_metatype_registry_impl::sym[33] =
    "openvrml_register_node_metatypes";

int openvrml_open_node_module(const char * const filename, void * const data)
{
    using openvrml::node_metatype_registry;
    using namespace openvrml::local;

    node_metatype_registry_impl & registry =
        *static_cast<node_metatype_registry_impl *>(data);

    const dl::handle handle = dl::open(filename);
    if (!handle) { return 0; } // Ignore things we can't open.
    scope_guard handle_guard = make_guard(dl::close, handle);

    //
    // Make sure the module has what we're looking for.
    //
    const void * const sym =
        dl::sym(handle, openvrml::local::node_metatype_registry_impl::sym);
    if (!sym) { return 0; } // handle_guard will close the module.

    const bool succeeded = registry.module_handles_.insert(handle).second;
    assert(succeeded);
    handle_guard.dismiss();
    return 0;
}

namespace {

# ifdef _WIN32
    const char pathsep_char = ';';
# else
    const char pathsep_char = ':';
# endif
}

/**
 * @internal
 *
 * @brief Construct.
 *
 * @param[in,out] b the @c browser.
 */
openvrml::local::node_metatype_registry_impl::
node_metatype_registry_impl(openvrml::browser & b):
    browser_(b)
{
    using namespace openvrml::local;
    int result = dl::init();
    if (result != 0) {
        throw std::runtime_error("dlinit_failure");
    }

    std::ostringstream node_path;
    node_path << OPENVRML_PKGLIBDIR_ "/node";
    const char * const node_path_env = getenv("OPENVRML_NODE_PATH");
    if (node_path_env) {
        node_path << pathsep_char << node_path_env;
    }

    result = dl::foreachfile(node_path.str().c_str(),
                             openvrml_open_node_module,
                             this);
    assert(result == 0); // We always return 0 from the callback.
}

/**
 * @internal
 *
 * @brief Destroy.
 */
openvrml::local::node_metatype_registry_impl::~node_metatype_registry_impl()
    OPENVRML_NOTHROW
{
    using namespace openvrml::local;

    //
    // We need to clear the map so that the node_metatypes get destroyed before
    // we unload the libraries their code lives in.
    //
    this->node_metatype_map_.clear();

    //
    // Anyone holding on to a shared_ptr<node_metatype> for a node_metatype
    // in these libraries is about to be screwed--but they probably shouldn't
    // have been doing that anyway.
    //
    std::for_each(this->module_handles_.begin(), this->module_handles_.end(),
                  dl::close);
    dl::exit(); // Don't care if this fails.  What would we do?
}

/**
 * @brief The @c browser.
 *
 * @return the @c browser.
 */
openvrml::browser &
openvrml::local::node_metatype_registry_impl::browser() const OPENVRML_NOTHROW
{
    return this->browser_;
}

namespace {

    struct OPENVRML_LOCAL register_node_metatypes_for_module {
        explicit register_node_metatypes_for_module(
            openvrml::node_metatype_registry & registry):
            registry_(registry)
        {}

        void operator()(openvrml::local::dl::handle const handle) const
        {
            using openvrml::node_metatype_registry;
            using openvrml::register_node_metatypes_func;
            using namespace openvrml::local;
            void * sym = dl::sym(handle, node_metatype_registry_impl::sym);
            assert(sym); // We already made sure this would work.
            const register_node_metatypes_func register_node_metatypes =
                reinterpret_cast<register_node_metatypes_func>(sym);
            register_node_metatypes(this->registry_);
        }

    private:
        openvrml::node_metatype_registry & registry_;
    };
}

/**
 * @brief Register the node metatypes.
 *
 * Call @c openvrml_register_node_metatypes for each module.
 *
 * @param[in,out] registry  the @c node_metatype_registry.
 */
void
openvrml::local::node_metatype_registry_impl::
register_node_metatypes(node_metatype_registry & registry)
{
    std::for_each(this->module_handles_.begin(), this->module_handles_.end(),
                  register_node_metatypes_for_module(registry));
}

/**
 * @brief Register a node metatype.
 *
 * @param[in] id        the string identifier associated @p metatype.
 * @param[in] metatype  the @c node_metatype.
 *
 * @exception std::invalid_argument if @p metatype is null.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void
openvrml::local::node_metatype_registry_impl::
register_node_metatype(const std::string & id,
                       const boost::shared_ptr<node_metatype> & metatype)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    if (!metatype.get()) {
        throw std::invalid_argument(
            "cannot register null node_metatype pointer");
    }
    read_write_mutex::scoped_write_lock lock(this->mutex_);
    this->node_metatype_map_[id] = metatype;
}

namespace {
    typedef std::map<std::string, boost::shared_ptr<openvrml::node_metatype> >
        node_metatype_map_t;

    struct OPENVRML_LOCAL init_node_metatype :
        std::unary_function<void, node_metatype_map_t::value_type>
    {
        init_node_metatype(openvrml::viewpoint_node * initial_viewpoint,
                        const double time)
            OPENVRML_NOTHROW:
            initial_viewpoint_(initial_viewpoint),
            time_(time)
        {}

        void operator()(const node_metatype_map_t::value_type & value) const
            OPENVRML_NOTHROW
        {
            assert(value.second);
            value.second->initialize(this->initial_viewpoint_, this->time_);
        }

    private:
        openvrml::viewpoint_node * initial_viewpoint_;
        double time_;
    };
}

/**
 * @brief Initialize the @c node_metatype%s.
 *
 * @param[in] initial_viewpoint the @c viewpoint_node that should be initially
 *                              active.
 * @param[in] timestamp         the current time.
 */
void
openvrml::local::node_metatype_registry_impl::
init(viewpoint_node * initial_viewpoint, const double timestamp)
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    std::for_each(this->node_metatype_map_.begin(),
                  this->node_metatype_map_.end(),
                  init_node_metatype(initial_viewpoint, timestamp));
}

/**
 * @brief Find a @c node_metatype.
 *
 * @param[in] id    an implementation identifier.
 *
 * @return the @c node_metatype corresponding to @p id, or a null
 *         pointer if no such @c node_metatype exists in the map.
 */
const boost::shared_ptr<openvrml::node_metatype>
openvrml::local::node_metatype_registry_impl::find(const std::string & id) const
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    const node_metatype_map_t::const_iterator pos =
        this->node_metatype_map_.find(id);
    return (pos != this->node_metatype_map_.end())
        ? pos->second
        : boost::shared_ptr<node_metatype>();
}

/**
 * @brief The @c node_metatype identifiers associated with @p node_metatype.
 *
 * @param[in] node_metatype    a @c node_metatype.
 *
 * @return the @c node_metatype identifiers associated with @p node_metatype.
 */
const std::vector<openvrml::node_metatype_id>
openvrml::local::node_metatype_registry_impl::
node_metatype_ids(const openvrml::node_metatype & node_metatype) const
    OPENVRML_THROW1(std::bad_alloc)
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    std::vector<node_metatype_id> ids;
    for (node_metatype_map_t::const_iterator entry =
             this->node_metatype_map_.begin();
         entry != this->node_metatype_map_.end();
         ++entry) {
        if (entry->second.get() == &node_metatype) {
            ids.push_back(entry->first);
        }
    }
    return ids;
}

namespace {

    struct OPENVRML_LOCAL render_node_metatype :
            std::unary_function<void, node_metatype_map_t::value_type> {
        explicit render_node_metatype(openvrml::viewer & viewer):
            viewer(&viewer)
        {}

        void operator()(const node_metatype_map_t::value_type & value) const
        {
            value.second->render(*this->viewer);
        }

    private:
        openvrml::viewer * viewer;
    };
}

/**
 * @brief Render the @c node_metatype%s.
 *
 * @param[in,out] v a @c viewer.
 */
void openvrml::local::node_metatype_registry_impl::
render(openvrml::viewer & v)
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    std::for_each(this->node_metatype_map_.begin(),
                  this->node_metatype_map_.end(),
                  render_node_metatype(v));
}

namespace {

    struct OPENVRML_LOCAL shutdown_node_metatype :
            std::unary_function<void, node_metatype_map_t::value_type> {
        explicit shutdown_node_metatype(const double timestamp):
            timestamp_(timestamp)
        {}

        void operator()(const node_metatype_map_t::value_type & value) const
        {
            value.second->shutdown(this->timestamp_);
        }

    private:
        double timestamp_;
    };
}

/**
 * @brief Shut down the @c node_metatype%s.
 *
 * @param[in] timestamp the current time.
 */
void
openvrml::local::node_metatype_registry_impl::shutdown(const double timestamp)
    OPENVRML_NOTHROW
{
    read_write_mutex::scoped_read_lock lock(this->mutex_);
    std::for_each(this->node_metatype_map_.begin(),
                  this->node_metatype_map_.end(),
                  shutdown_node_metatype(timestamp));
}
