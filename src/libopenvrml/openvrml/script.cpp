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

# include "script.h"
# include "browser.h"
# include "scene.h"
# include <openvrml/local/dl.h>
# include <openvrml/local/uri.h>
# include <private.h>
# include <boost/array.hpp>
# include <boost/functional.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/mpl/for_each.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/utility.hpp>
# include <algorithm>
# include <functional>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif


using namespace boost::multi_index::detail;  // for scope_guard

 /**
 * @file openvrml/script.h
 *
 * @brief Pluggable scripting engine support.
 */

/**
 * @class openvrml::script openvrml/script.h
 *
 * @brief Abstract class implemented by scripting language bindings.
 *
 * The runtime instantiates subclasses of script for each VRML97 Script node;
 * and calls its methods appropriately to execute %script code.
 */

/**
 * @internal
 *
 * @typedef openvrml::script::direct_output_map_t
 *
 * @brief Map of direct outputs.
 */

/**
 * @internal
 *
 * @var openvrml::script::direct_output_map_t openvrml::script::direct_output_map_
 *
 * @brief Map of direct outputs.
 */

/**
 * @var openvrml::script_node & openvrml::script::node
 *
 * @brief A reference to the @c script_node that uses this @c script object.
 */

/**
 * @brief Construct.
 *
 * @param[in] node  a reference to the @c script_node that uses this @c script
 *                  object.
 */
openvrml::script::script(script_node & node):
    node(node)
{}

/**
 * @brief Destroy.
 */
openvrml::script::~script()
{}

/**
 * @brief Initialize the Script node.
 *
 * Delegates to @c #do_initialize.
 *
 * @param[in] timestamp the current time.
 */
void openvrml::script::initialize(double timestamp)
{
    this->do_initialize(timestamp);
    this->process_direct_output(timestamp);
}

/**
 * @fn void openvrml::script::do_initialize(double timestamp)
 *
 * @brief Initialize the Script node.
 *
 * @param[in] timestamp the current time.
 */

/**
 * @brief Process an event.
 *
 * Delegates to @c #do_process_event.
 *
 * @param[in] id        eventIn identifier.
 * @param[in] value     event value.
 * @param[in] timestamp the current time.
 */
void openvrml::script::process_event(const std::string & id,
                                     const field_value & value,
                                     double timestamp)
{
    this->do_process_event(id, value, timestamp);
    this->process_direct_output(timestamp);
}

/**
 * @fn void openvrml::script::do_process_event(const std::string & id, const field_value & value, double timestamp)
 *
 * @brief Process an event.
 *
 * @param[in] id        @c eventIn identifier.
 * @param[in] value     event value.
 * @param[in] timestamp the current time.
 */

/**
 * @brief Execute %script code after processing events.
 *
 * Delegates to @c #do_events_processed.
 *
 * @param[in] timestamp the current time.
 */
void openvrml::script::events_processed(double timestamp)
{
    this->do_events_processed(timestamp);
}

/**
 * @fn void openvrml::script::do_events_processed(double timestamp)
 *
 * @brief Execute %script code after processing events.
 *
 * @param[in] timestamp the current time.
 */

/**
 * @brief Shut down the Script node.
 *
 * Delegates to @c #do_shutdown.
 *
 * @param[in] timestamp the current time.
 */
void openvrml::script::shutdown(double timestamp)
{
    this->do_shutdown(timestamp);
    this->process_direct_output(timestamp);
}

/**
 * @fn void openvrml::script::do_shutdown(double timestamp)
 *
 * @brief Shut down the Script node.
 *
 * @param[in] timestamp the current time.
 */

/**
 * @brief Whether direct output is enabled for the Script node.
 *
 * @return @c true if direct output is enabled for the Script node; @c false
 *         otherwise.
 */
bool openvrml::script::direct_output() const OPENVRML_NOTHROW
{
    return this->node.direct_output.value();
}

/**
 * @brief Whether the @c browser may delay sending input events to the
 *        @c script until its outputs are needed by the browser.
 *
 * @return @c true if the @c browser may delay sending input events to the
 *         @c script until its outputs are needed by the @c browser; @c false
 *         otherwise.
 */
bool openvrml::script::must_evaluate() const OPENVRML_NOTHROW
{
    return this->node.must_evaluate.value();
}

/**
 * @brief Set the value of a field.
 *
 * @param[in] id    field identifier.
 * @param[in] value new value.
 *
 * @exception unsupported_interface if the Script node has no field @p id.
 * @exception std::bad_cast         if @p value is the wrong type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void openvrml::script::field(const std::string & id, const field_value & value)
    OPENVRML_THROW3(unsupported_interface, std::bad_cast, std::bad_alloc)
{
    const script_node::field_value_map_t::iterator field =
        this->node.field_value_map_.find(id);
    if (field == this->node.field_value_map_.end()) {
        throw unsupported_interface(this->node.type_,
                                    node_interface::field_id,
                                    id);
    }
    field->second->assign(value); // throws std::bad_cast, std::bad_alloc
}

/**
 * @brief Add an event for direct output processing at the end of script
 *        execution.
 *
 * @param[in] listener  the @c event_listener to which the event should be
 *                      sent.
 * @param[in] value     the value to send.
 *
 * @exception field_value_type_mismatch if @p listener is not the correct type
 *                                      to process events of @p value's type.
 * @exception std::bad_alloc            if memory allocation fails.
 */
void
openvrml::script::direct_output(event_listener & listener,
                                const boost::shared_ptr<field_value> & value)
    OPENVRML_THROW2(field_value_type_mismatch, std::bad_alloc)
{
    assert(value);
    if (listener.type() != value->type()) {
        throw field_value_type_mismatch();
    }
    this->direct_output_map_[&listener] = value;
}

namespace {
    struct OPENVRML_LOCAL direct_output_processor {
        direct_output_processor(openvrml::event_listener & listener,
                                openvrml::field_value & value,
                                const double timestamp):
            listener_(&listener),
            value_(&value),
            timestamp_(timestamp)
        {}

        template <typename T>
        void operator()(T) const
        {
            if (T::field_value_type_id == this->listener_->type()) {
                using boost::polymorphic_downcast;
                using openvrml::field_value_listener;
                dynamic_cast<field_value_listener<T> &>(*this->listener_)
                    .process_event(*polymorphic_downcast<T *>(this->value_),
                                   this->timestamp_);
            }
        }

    private:
        openvrml::event_listener * listener_;
        openvrml::field_value * value_;
        double timestamp_;
    };
}

/**
 * @internal
 *
 * @brief Process direct outputs in @a #direct_output_map_.
 *
 * This function is called at the end of initialization and processing normal
 * events.
 *
 * @post @c script::direct_output_map_.empty() is @c true.
 */
void openvrml::script::process_direct_output(double timestamp)
{
    for (direct_output_map_t::const_iterator output =
             this->direct_output_map_.begin();
         output != this->direct_output_map_.end();
         ++output) {
        using boost::mpl::for_each;
        using openvrml_::field_value_types;
        for_each<field_value_types>(direct_output_processor(*output->first,
                                                            *output->second,
                                                            timestamp));
    }
    this->direct_output_map_.clear();
}


/**
 * @class openvrml::script_factory
 *
 * @brief An abstract factory for @c script%s.
 */

/**
 * @brief Construct.
 */
openvrml::script_factory::script_factory() OPENVRML_NOTHROW
{}

/**
 * @brief Destroy.
 */
openvrml::script_factory::~script_factory() OPENVRML_NOTHROW
{}

/**
 * @fn std::auto_ptr<openvrml::script> openvrml::script_factory::create_script(script_node & node, const boost::shared_ptr<resource_istream> & source)
 *
 * @brief Create a @c script.
 *
 * Implementations of this function are called in the implementation of
 * @c script_node. A @c script instance is created to encapsulate scripting
 * logic in a Script node instance.
 */

/**
 * @class openvrml::script_factory_registry
 *
 * @brief Registry of
 *        <code>@link script_factory script_factories@endlink</code>.
 *
 * In general there should be one @c script_factory registered per
 * &ldquo;scripting engine&rdquo;; i.e., supported language in the Script
 * node.  An instance of the @c script_factory_registry is passed to
 * @c openvrml_register_script_factory, which should be implemented by
 * modules that add support for a scripting language.
 */

/**
 * @internal
 *
 * @var class openvrml::script_factory_registry::script_node
 *
 * @brief @c script_node is the only class that should ever need to
 *        instantiate @c script_factory_registry.
 */

namespace {

# ifdef _WIN32
    const char pathsep_char = ';';
# else
    const char pathsep_char = ':';
# endif
}

extern "C" int openvrml_get_script_factory(const char * filename, void * data);

/**
 * @internal
 *
 * @brief @c script_factory_registry implementation.
 *
 * @c script_factory_registry uses the p-impl idiom.
 */
class openvrml::script_factory_registry::impl :
    boost::noncopyable {

    friend int (::openvrml_get_script_factory)(const char * filename,
                                               void * data);

    typedef std::set<local::dl::handle> module_handle_set;
    module_handle_set module_handles_;

    typedef std::map<std::string, boost::shared_ptr<script_factory> >
        factory_map;
    factory_map media_type_map_;
    factory_map uri_scheme_map_;

public:
    impl();
    ~impl() OPENVRML_NOTHROW;

    void register_factories(script_factory_registry & registry);

    bool register_factory(
        const std::set<std::string> & media_types,
        const std::set<std::string> & uri_schemes,
        const boost::shared_ptr<script_factory> & factory)
        OPENVRML_THROW2(std::bad_alloc, std::invalid_argument);

    const boost::shared_ptr<script_factory>
    find_using_media_type(const std::string & media_type) const;

    const boost::shared_ptr<script_factory>
    find_using_uri_scheme(const std::string & uri_scheme) const;
};

int openvrml_get_script_factory(const char * const filename, void * data)
{
    assert(data);

    using openvrml::script_factory_registry;
    using namespace openvrml::local;

    script_factory_registry::impl & registry =
        *static_cast<script_factory_registry::impl *>(data);

    const dl::handle handle = dl::open(filename);
    if (!handle) { return 0; } // Ignore things we can't open.
    scope_guard handle_guard = make_guard(dl::close, handle);

    //
    // Make sure the module has what we're looking for.
    //
    const void * sym = dl::sym(handle, "openvrml_script_LTX_register_factory");
    if (!sym) { return 0; } // handle_guard will close the module.

    const bool succeeded = registry.module_handles_.insert(handle).second;
    assert(succeeded);
    handle_guard.dismiss();
    return 0;
}

/**
 * @internal
 *
 * @brief Construct.
 */
openvrml::script_factory_registry::impl::impl()
{
    using namespace openvrml::local;
    int result = dl::init();
    if (result != 0) {
        throw std::runtime_error("dlinit failure");
    }

    std::ostringstream script_path;
    script_path << OPENVRML_PKGLIBDIR_ "/script";
    const char * const script_path_env = getenv("OPENVRML_SCRIPT_PATH");
    if (script_path_env) {
        script_path << pathsep_char << script_path_env;
    }

    result = dl::foreachfile(script_path.str().c_str(),
                             openvrml_get_script_factory,
                             this);
    assert(result == 0); // We always return 0 from the callback.
}

/**
 * @internal
 *
 * @brief Destroy.
 */
openvrml::script_factory_registry::impl::~impl() OPENVRML_NOTHROW
{
    using namespace openvrml::local;
    std::for_each(this->module_handles_.begin(), this->module_handles_.end(),
                  dl::close);
    dl::exit(); // Don't care if this fails.  What would we do?
}

void
openvrml::script_factory_registry::impl::
register_factories(script_factory_registry & registry)
{
    using namespace openvrml::local;
    for (module_handle_set::const_iterator handle(
             this->module_handles_.begin());
         handle != this->module_handles_.end();
         ++handle) {
        void * sym = dl::sym(*handle, "openvrml_script_LTX_register_factory");
        assert(sym); // We already made sure this would work.
        void (* const register_factory)(script_factory_registry &) =
            reinterpret_cast<void (*)(script_factory_registry &)>(sym);
        register_factory(registry);
    }
}

namespace {
    typedef std::map<std::string, boost::shared_ptr<openvrml::script_factory> >
        factory_map_t;

    struct OPENVRML_LOCAL add_factory_entry : std::unary_function<std::string,
                                                                  void> {
        add_factory_entry(
            const boost::shared_ptr<openvrml::script_factory> & factory,
            factory_map_t & factory_map):
            factory_(factory),
            factory_map_(&factory_map)
        {}

        void operator()(const std::string & id) const
        {
            const bool succeeded =
                this->factory_map_->insert(make_pair(id, this->factory_))
                .second;
            assert(succeeded);
        }

    private:
        const boost::shared_ptr<openvrml::script_factory> factory_;
        factory_map_t * const factory_map_;
    };
}

bool
openvrml::script_factory_registry::impl::
register_factory(const std::set<std::string> & media_types,
                 const std::set<std::string> & uri_schemes,
                 const boost::shared_ptr<script_factory> & factory)
    OPENVRML_THROW2(std::bad_alloc, std::invalid_argument)
{
    using std::invalid_argument;
    using std::set;
    using std::string;

    if (media_types.empty() && uri_schemes.empty()) {
        throw invalid_argument("no media types or URI schemes specified");
    }

    if (!factory) {
        throw invalid_argument("null script_factory pointer");
    }

    for (set<string>::const_iterator media_type = media_types.begin();
         media_type != media_types.end();
         ++media_type) {
        const factory_map::const_iterator pos =
            this->media_type_map_.find(*media_type);
        if (pos != this->media_type_map_.end()) { return false; }
    }

    for (set<string>::const_iterator uri_scheme = uri_schemes.begin();
         uri_scheme != uri_schemes.end();
         ++uri_scheme) {
        const factory_map::const_iterator pos =
            this->uri_scheme_map_.find(*uri_scheme);
        if (pos != this->uri_scheme_map_.end()) { return false; }
    }

    std::for_each(media_types.begin(), media_types.end(),
                  add_factory_entry(factory, this->media_type_map_));
    std::for_each(uri_schemes.begin(), uri_schemes.end(),
                  add_factory_entry(factory, this->uri_scheme_map_));
    return true;
}

const boost::shared_ptr<openvrml::script_factory>
openvrml::script_factory_registry::impl::
find_using_media_type(const std::string & media_type) const
{
    const factory_map::const_iterator pos =
        this->media_type_map_.find(media_type);
    return (pos != this->media_type_map_.end())
        ? pos->second
        : boost::shared_ptr<script_factory>();
}

const boost::shared_ptr<openvrml::script_factory>
openvrml::script_factory_registry::impl::
find_using_uri_scheme(const std::string & uri_scheme) const
{
    const factory_map::const_iterator pos =
        this->uri_scheme_map_.find(uri_scheme);
    return (pos != this->uri_scheme_map_.end())
        ? pos->second
        : boost::shared_ptr<script_factory>();
}

/**
 * @internal
 *
 * @brief Construct.
 */
openvrml::script_factory_registry::script_factory_registry():
    impl_(new impl)
{
    this->impl_->register_factories(*this);
}

/**
 * @internal
 *
 * @brief Destroy.
 */
openvrml::script_factory_registry::~script_factory_registry()
{}

/**
 * @brief Register a factory for creating concrete @c script%s.
 *
 * This function should be called in a script engine module's implementation
 * of @c openvrml_script_LTX_register_factory.
 *
 * @param[in] media_types   the set of MIME media types that identify scripts
 *                          supported by the module.
 * @param[in] uri_schemes   the set of URI schemes that identify scripts
 *                          supported by the module.
 * @param[in] factory       the factory.
 *
 * @return @c true if the factory was registered successfully; @c false
 *         otherwise
 */
bool
openvrml::script_factory_registry::
register_factory(const std::set<std::string> & media_types,
                 const std::set<std::string> & uri_schemes,
                 const boost::shared_ptr<script_factory> & factory)
    OPENVRML_THROW2(std::bad_alloc, std::invalid_argument)
{
    return this->impl_->register_factory(media_types, uri_schemes, factory);
}

/**
 * @class openvrml::script_node_metatype openvrml/script.h
 *
 * @brief Class object for @c script_node%s.
 *
 * There is one @c script_node_metatype per @c browser instance.
 *
 * @see browser::scriptNodeClass
 */

/**
 * @brief Construct.
 *
 * @param[in] browser   the @c browser to be associated with the
 *                      @c script_node_metatype.
 */
openvrml::script_node_metatype::script_node_metatype(openvrml::browser & browser):
    node_metatype("urn:X-openvrml:node:Script", browser)
{}

/**
 * @brief Destroy.
 */
openvrml::script_node_metatype::~script_node_metatype() OPENVRML_NOTHROW
{}

/**
 * @brief Not implemented.
 *
 * This method is not implemented because the Script node implementation,
 * unlike other node implementations, cannot provide the implementation of
 * an @c EXTERNPROTO.  It is an error to call this method.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::script_node_metatype::do_create_type(const std::string &,
                                            const node_interface_set &) const
    OPENVRML_NOTHROW
{
    assert(false);
    return boost::shared_ptr<node_type>();
}


/**
 * @class openvrml::script_node openvrml/browser.h
 *
 * @brief Represents a VRML Script node.
 */

/**
 * @internal
 *
 * @var class openvrml::script_node::script
 *
 * @brief Abstract base class for %script runtimes.
 */

/**
 * @internal
 *
 * @brief Registry of script engine modules.
 */
openvrml::script_factory_registry
openvrml::script_node::script_factory_registry_;

/**
 * @typedef openvrml::script_node::field_value_map_t
 *
 * @brief A @c std::map that keys field values on their field name.
 */

/**
 * @class openvrml::script_node::eventout openvrml/script.h
 *
 * @brief An @c event_emitter along with the emitted value.
 */

/**
 * @var openvrml::script_node & openvrml::script_node::eventout::node_
 *
 * @brief The containing @c script_node.
 */

/**
 * @var boost::scoped_ptr<openvrml::field_value> openvrml::script_node::eventout::value_
 *
 * @brief The value.
 */

/**
 * @var bool openvrml::script_node::eventout::modified_
 *
 * @brief Flag to indicate whether @a value_ has been modified.
 */

/**
 * @var boost::scoped_ptr<openvrml::event_emitter> openvrml::script_node::eventout::emitter_
 *
 * @brief Event emitter.
 */

/**
 * @brief Construct.
 *
 * @param[in] type  field value type identifier.
 * @param[in] node  @c script_node.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::script_node::eventout::eventout(const field_value::type_id type,
                                          script_node & node)
    OPENVRML_THROW1(std::bad_alloc):
    node_(node),
    value_(field_value::create(type)),
    modified_(false),
    emitter_(create_emitter(node, *this->value_))
{}

/**
 * @brief The value that will be sent from the @c eventOut.
 *
 * @return the value that will be sent from the @c eventOut.
 */
const openvrml::field_value & openvrml::script_node::eventout::value() const
    OPENVRML_NOTHROW
{
    return *this->value_;
}

/**
 * @brief Set the value that will be sent from the @c eventOut.
 *
 * After calling this function, modified will return @c true until
 * @c emit_event is called.
 *
 * @param[in] val   field value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 * @exception std::bad_cast     if @p val is not the correct type.
 */
void openvrml::script_node::eventout::value(const field_value & val)
    OPENVRML_THROW2(std::bad_alloc, std::bad_cast)
{
    using boost::polymorphic_downcast;

    if (this->value_->type() == field_value::sfnode_id) {
        this->node_
            .assign_with_self_ref_check(
                dynamic_cast<const sfnode &>(val),
                *polymorphic_downcast<sfnode *>(this->value_.get()));
    } else if (this->value_->type() == field_value::mfnode_id) {
        this->node_
            .assign_with_self_ref_check(
                dynamic_cast<const mfnode &>(val),
                *polymorphic_downcast<mfnode *>(this->value_.get()));
    } else {
        this->value_->assign(val); // Throws std::bad_alloc, std::bad_cast.
    }
    this->modified_ = true;
}

/**
 * @brief Whether the value has been modified.
 *
 * @return @c true if the value has been changed since @c emit_event was last
 *         called; @c false otherwise.
 */
bool openvrml::script_node::eventout::modified() const OPENVRML_NOTHROW
{
    return this->modified_;
}

/**
 * @brief The @c event_emitter associated with the @c eventout.
 *
 * @return the @c event_emitter associated with the @c eventout.
 */
openvrml::event_emitter & openvrml::script_node::eventout::emitter()
    OPENVRML_NOTHROW
{
    return *this->emitter_;
}

/**
 * @brief Cause the contained @c event_emitter to emit an event.
 *
 * Events should be emitted from Script nodes by calling this function instead
 * of passing the @c event_emitter directly to @c node::emit_event.
 *
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::script_node::eventout::emit_event(const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    node::emit_event(*this->emitter_, timestamp);
    this->modified_ = false;
}

/**
 * @typedef openvrml::script_node::eventout_ptr
 *
 * @brief Reference-counted smart pointer to an @c eventout.
 */

/**
 * @typedef openvrml::script_node::eventout_map_t
 *
 * @brief Map of @c eventout instances.
 */

/**
 * @internal
 *
 * @class openvrml::script_node::script_node_type openvrml/script.h
 *
 * @brief Type objects for @c script_node%s.
 *
 * @see script_node::type_
 */

/**
 * @var openvrml::node_interface_set openvrml::script_node::script_node_type::interfaces_
 *
 * @brief Node interfaces.
 */

namespace {

    //
    // The order of the node_interfaces in this array is significant.  This
    // range is used with std::set_difference, so the elements *must* be in
    // lexicographically increasing order according to their "id" member.
    //
    const boost::array<openvrml::node_interface, 4>
    built_in_script_interfaces_ = {
        openvrml::node_interface(openvrml::node_interface::field_id,
                                 openvrml::field_value::sfbool_id,
                                 "directOutput"),
        openvrml::node_interface(openvrml::node_interface::exposedfield_id,
                                 openvrml::field_value::sfnode_id,
                                 "metadata"),
        openvrml::node_interface(openvrml::node_interface::field_id,
                                 openvrml::field_value::sfbool_id,
                                 "mustEvaluate"),
        openvrml::node_interface(openvrml::node_interface::exposedfield_id,
                                 openvrml::field_value::mfstring_id,
                                 "url")
    };
}

/**
 * @brief Construct.
 *
 * @param[in] class_    the @c node_metatype for @c script_node%s.
 */
openvrml::script_node::script_node_type::
script_node_type(script_node_metatype & class_):
    node_type(class_, "Script")
{
    for (size_t i = 0; i < built_in_script_interfaces_.size(); ++i) {
        bool succeeded =
            this->interfaces_.insert(built_in_script_interfaces_[i]).second;
        assert(succeeded);
    }
}

/**
 * @brief Destroy.
 */
openvrml::script_node::script_node_type::~script_node_type() OPENVRML_NOTHROW
{}

/**
 * @brief Add an interface.
 *
 * @param[in] interface
 *
 * @exception std::bad_alloc        if memory allocation fails.
 * @exception std::invalid_argument if the @c script_node_type already has an
 *                                  interface that conflicts with @p interface.
 */
void
openvrml::script_node::script_node_type::
add_interface(const node_interface & interface_)
    OPENVRML_THROW2(std::bad_alloc, std::invalid_argument)
{
    bool succeeded  = this->interfaces_.insert(interface_).second;
    if (!succeeded) {
        throw std::invalid_argument("Interface conflicts with an interface "
                                    "already in this set.");
    }
}

/**
 * @brief Get the interfaces for the @c node.
 *
 * @return the interfaces for the @c node.
 */
const openvrml::node_interface_set &
openvrml::script_node::script_node_type::do_interfaces() const
    OPENVRML_NOTHROW
{
    return this->interfaces_;
}

/**
 * @brief Clone the Script node that has this @c node_type.
 *
 * Since the @c node_type for a @c script_node is only available once a
 * @c script_node is instantiated, you have to have a @c script_node instance
 * in order to be able to use this method.  The &ldquo;primordial&rdquo;
 * @c script_node instance must be created with a call to the
 * @c script_node constructor.
 *
 * @param[in] scope             the @c scope to which the @c node should
 *                              belong.
 * @param[in] initial_values    a map of initial values for the
 *                              <code>node</code>'s @c field%s and
 *                              @c exposedField%s.
 *
 * @exception unsupported_interface if @p initial_values specifies a field
 *                                  name that is not supported by the node
 *                                  type.
 * @exception std::bad_cast         if a value in @p initial_values is the
 *                                  wrong type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::intrusive_ptr<openvrml::node>
openvrml::script_node::script_node_type::
do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
               const initial_value_map & initial_values) const
    OPENVRML_THROW3(unsupported_interface, std::bad_cast, std::bad_alloc)
{
    using std::insert_iterator;
    using std::set_difference;
    using boost::intrusive_ptr;
    using boost::polymorphic_downcast;

    //
    // The const_cast here is a bit messy; the alternative would be to make
    // script_node_type a friend of node_type. That's not straightforward
    // since script_node_type is an inner class of script_node.
    //
    script_node_metatype & scriptNodeClass =
        *const_cast<script_node_metatype *>(
            polymorphic_downcast<const script_node_metatype *>(
                &this->metatype()));

    //
    // First, we need the set of node_interfaces *without* the built-in
    // Script node interfaces (url, directOutput, mustEvaluate, metadata).
    //
    node_interface_set interfaces;
    insert_iterator<node_interface_set> interface_inserter(interfaces,
                                                           interfaces.begin());
    set_difference(this->interfaces_.begin(),
                   this->interfaces_.end(),
                   built_in_script_interfaces_.begin(),
                   built_in_script_interfaces_.end(),
                   interface_inserter,
                   node_interface_set::key_compare());

    boost::intrusive_ptr<node> n;
    try {
        n = intrusive_ptr<node>(new script_node(scriptNodeClass,
                                                scope,
                                                interfaces,
                                                initial_values));
    } catch (std::invalid_argument & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        assert(false);
    }
    return n;
}

/**
 * @internal
 *
 * @brief Event listener.
 */
template <typename FieldValue>
class openvrml::script_node::script_event_listener :
    public node_field_value_listener<FieldValue> {

    const std::string id;

public:
    script_event_listener(const std::string & id, script_node & node);
    virtual ~script_event_listener() OPENVRML_NOTHROW;

private:
    virtual const std::string do_eventin_id() const OPENVRML_NOTHROW;
    virtual void do_process_event(const FieldValue & value, double timestamp)
        OPENVRML_THROW1(std::bad_alloc);
};


/**
 * @var const std::string openvrml::script_node::script_event_listener::id
 *
 * @brief @c eventIn identifier.
 */

/**
 * @brief Construct.
 *
 * @param[in] id    @c eventIn identifier.
 * @param[in] node  @c script_node.
 */
template <typename FieldValue>
openvrml::script_node::script_event_listener<FieldValue>::
script_event_listener(const std::string & id,
                      script_node & node):
    node_event_listener(node),
    node_field_value_listener<FieldValue>(node),
    id(id)
{}

/**
 * @brief Destroy.
 */
template <typename FieldValue>
openvrml::script_node::script_event_listener<FieldValue>::
~script_event_listener() OPENVRML_NOTHROW
{}

/**
 * @brief The associated @c eventIn identifier.
 *
 * @return the associated @c eventIn identifier.
 */
template <typename FieldValue>
const std::string
openvrml::script_node::script_event_listener<FieldValue>::do_eventin_id() const
    OPENVRML_NOTHROW
{
    return this->id;
}

/**
 * @brief Process an event.
 *
 * @param[in] value     event value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
template <typename FieldValue>
void openvrml::script_node::script_event_listener<FieldValue>::
do_process_event(const FieldValue & value,
                 const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    assert(dynamic_cast<openvrml::script_node *>(&this->node()));
    openvrml::script_node & script_node =
        dynamic_cast<openvrml::script_node &>(this->node());
    if (script_node.script_) {
        script_node.script_->process_event(this->id, value, timestamp);
    }
    ++script_node.events_received;
}

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfbool_listener
 *
 * @brief @c sfbool event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfcolor_listener
 *
 * @brief @c sfcolor event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfdouble_listener
 *
 * @brief @c sfdouble event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfcolorrgba_listener
 *
 * @brief @c sfcolorrgba event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sffloat_listener
 *
 * @brief @c sffloat event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfimage_listener
 *
 * @brief @c sfimage event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfint32_listener
 *
 * @brief @c sfint32 event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfnode_listener
 *
 * @brief @c sfnode event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfrotation_listener
 *
 * @brief @c sfrotation event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfstring_listener
 *
 * @brief @c sfstring event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sftime_listener
 *
 * @brief @c sftime event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfvec2d_listener
 *
 * @brief @c sfvec2d event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfvec2f_listener
 *
 * @brief @c sfvec2f event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfvec3d_listener
 *
 * @brief @c sfvec3d event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::sfvec3f_listener
 *
 * @brief @c sfvec3f event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfbool_listener
 *
 * @brief @c mfbool event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfcolor_listener
 *
 * @brief @c mfcolor event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfdouble_listener
 *
 * @brief @c mfdouble event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfcolorrgba_listener
 *
 * @brief @c mfcolorrgba event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mffloat_listener
 *
 * @brief @c mffloat event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfimage_listener
 *
 * @brief @c mfimage event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfint32_listener
 *
 * @brief @c mfint32 event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfnode_listener
 *
 * @brief @c mfnode event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfrotation_listener
 *
 * @brief @c mfrotation event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfstring_listener
 *
 * @brief @c mfstring event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mftime_listener
 *
 * @brief @c mftime event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfvec2d_listener
 *
 * @brief @c mfvec2d event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfvec2f_listener
 *
 * @brief @c mfvec2f event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfvec3d_listener
 *
 * @brief @c mfvec3d event listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::mfvec3f_listener
 *
 * @brief @c mfvec3f event listener.
 */

/**
 * @internal
 *
 * @brief Function object for @c boost::mpl::for_each; create a
 *        @c script_event_listener.
 */
struct OPENVRML_LOCAL openvrml::script_node::script_event_listener_creator {
    script_event_listener_creator(
        const field_value::type_id type,
        const std::string & id,
        script_node & node,
        boost::shared_ptr<openvrml::event_listener> & listener):
        type_(type),
        id_(&id),
        node_(&node),
        listener_(&listener)
    {}

    template <typename T>
    void operator()(T) const
    {
        if (T::field_value_type_id == this->type_) {
            this->listener_->reset(new script_event_listener<T>(*this->id_,
                                                                *this->node_));
        }
    }

private:
    field_value::type_id type_;
    const std::string * id_;
    script_node * node_;
    boost::shared_ptr<openvrml::event_listener> * listener_;
};

/**
 * @internal
 *
 * @brief Create a Script node event listener.
 *
 * @param[in] type  the type of listener to create.
 * @param[in] id    @c eventIn identifier.
 * @param[in] node  the containing script_node.
 *
 * @return a Script node event listener.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const boost::shared_ptr<openvrml::event_listener>
openvrml::script_node::create_listener(const field_value::type_id type,
                                       const std::string & id,
                                       script_node & node)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::mpl::for_each;
    using openvrml_::field_value_types;
    boost::shared_ptr<openvrml::event_listener> listener;
    for_each<field_value_types>(
        script_event_listener_creator(type, id, node, listener));
    assert(listener);
    return listener;
}

/**
 * @internal
 *
 * @brief Event emitter.
 */
template <typename FieldValue>
class openvrml::script_node::script_event_emitter :
    public openvrml::field_value_emitter<FieldValue> {
    BOOST_CLASS_REQUIRE(FieldValue, openvrml, FieldValueConcept);

    script_node * node_;

    struct event_emitter_equal_to :
        std::unary_function<typename eventout_map_t::value_type, bool> {
        explicit event_emitter_equal_to(
            const script_event_emitter<FieldValue> & emitter)
            OPENVRML_NOTHROW:
            emitter_(&emitter)
        {}

        bool operator()(const typename eventout_map_t::value_type & arg) const
        {
            return this->emitter_ == &arg.second->emitter();
        }

    private:
        const script_event_emitter * emitter_;
    };

public:
    script_event_emitter(script_node & node, const FieldValue & value)
        OPENVRML_NOTHROW;
    virtual ~script_event_emitter() OPENVRML_NOTHROW;

private:
    virtual const std::string do_eventout_id() const OPENVRML_NOTHROW;
};

/**
 * @var openvrml::script_node * openvrml::script_node::script_event_emitter::node_
 *
 * @brief The @c script_node.
 */

/**
 * @brief Construct.
 *
 * @param[in] node  the @c script_node.
 * @param[in] value the field value with which the emitter is associated.
 */
template <typename FieldValue>
openvrml::script_node::script_event_emitter<FieldValue>::
script_event_emitter(script_node & node, const FieldValue & value)
    OPENVRML_NOTHROW:
    openvrml::event_emitter(value),
    openvrml::field_value_emitter<FieldValue>(value),
    node_(&node)
{}

/**
 * @brief Destroy.
 */
template <typename FieldValue>
openvrml::script_node::script_event_emitter<FieldValue>::
~script_event_emitter() OPENVRML_NOTHROW
{}

/**
 * @brief The @c eventOut identifier.
 *
 * @return the @c eventOut identifier.
 */
template <typename FieldValue>
const std::string
openvrml::script_node::script_event_emitter<FieldValue>::do_eventout_id() const
    OPENVRML_NOTHROW
{
    const eventout_map_t::const_iterator pos =
        std::find_if(this->node_->eventout_map_.begin(),
                     this->node_->eventout_map_.end(),
                     event_emitter_equal_to(*this));
    assert(pos != this->node_->eventout_map_.end());
    return pos->first;
}

/**
 * @internal
 *
 * @brief Function object for @c boost::mpl::for_each; create a
 *        @c script_event_emitter.
 */
struct OPENVRML_LOCAL openvrml::script_node::script_event_emitter_creator {
    script_event_emitter_creator(
        script_node & node,
        const openvrml::field_value & value,
        std::auto_ptr<openvrml::event_emitter> & emitter):
        node_(&node),
        value_(&value),
        emitter_(&emitter)
    {}

    template <typename T>
    void operator()(T) const
    {
        if (T::field_value_type_id == this->value_->type()) {
            using boost::polymorphic_downcast;
            this->emitter_->reset(
                new script_event_emitter<T>(
                    *this->node_,
                    *polymorphic_downcast<const T *>(this->value_)));
        }
    }

private:
    script_node * node_;
    const openvrml::field_value * value_;
    std::auto_ptr<openvrml::event_emitter> * emitter_;
};

/**
 * @internal
 *
 * @brief Create a Script node event emitter.
 *
 * @param[in] node  the containing @c script_node.
 * @param[in] value the node field value associated with the emitter.
 *
 * @return a Script node event emitter.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::event_emitter>
openvrml::script_node::create_emitter(script_node & node,
                                      const openvrml::field_value & value)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::mpl::for_each;
    using openvrml_::field_value_types;
    std::auto_ptr<openvrml::event_emitter> emitter;
    for_each<field_value_types>(
        script_event_emitter_creator(node, value, emitter));
    assert(emitter.get());
    return emitter;
}

/**
 * @internal
 *
 * @class openvrml::script_node::set_url_listener_t openvrml/script.h
 *
 * @brief @c set_url event listener.
 */

/**
 * @brief Construct.
 *
 * @param[in] node  a reference to the containing @c script_node.
 */
openvrml::script_node::set_url_listener_t::
set_url_listener_t(script_node & node):
    node_event_listener(node),
    node_field_value_listener<mfstring>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::script_node::set_url_listener_t::~set_url_listener_t() OPENVRML_NOTHROW
{}

/**
 * @brief Get the @c eventIn identifier.
 *
 * @return the @c eventIn identifier.
 */
const std::string
openvrml::script_node::set_url_listener_t::do_eventin_id() const OPENVRML_NOTHROW
{
    return "set_url";
}

/**
 * @brief Process an event.
 *
 * @param[in] value     new @c url value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::script_node::set_url_listener_t::
do_process_event(const mfstring & value, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    assert(dynamic_cast<openvrml::script_node *>(&this->node()));
    openvrml::script_node & script_node =
        dynamic_cast<openvrml::script_node &>(this->node());
    script_node.script_.reset();
    script_node.url_ = value;
    script_node.do_initialize(timestamp);

    //
    // url is an exposedField.
    //
    node::emit_event(script_node.url_changed_emitter_, timestamp);
}

/**
 * @internal
 *
 * @class openvrml::script_node::url_changed_emitter openvrml/script.h
 *
 * @brief @c url_changed event emitter.
 */

/**
 * @brief Construct.
 *
 * @param[in] value the associated field value.
 */
openvrml::script_node::url_changed_emitter::
url_changed_emitter(const mfstring & value) OPENVRML_NOTHROW:
    openvrml::event_emitter(value),
    openvrml::mfstring_emitter(value)
{}

/**
 * @brief Destroy.
 */
openvrml::script_node::url_changed_emitter::~url_changed_emitter() OPENVRML_NOTHROW
{}

/**
 * @brief The @c eventOut identifier.
 *
 * @return the @c eventOut identifier.
 */
const std::string
openvrml::script_node::url_changed_emitter::do_eventout_id() const OPENVRML_NOTHROW
{
    return "url_changed";
}

/**
 * @internal
 *
 * @class openvrml::script_node::set_metadata_listener openvrml/script.h
 *
 * @brief @c set_metadata event listener.
 */

/**
 * @brief Construct.
 *
 * @param[in] node  a reference to the containing @c script_node.
 */
openvrml::script_node::set_metadata_listener::
set_metadata_listener(script_node & node):
    node_event_listener(node),
    node_field_value_listener<sfnode>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::script_node::set_metadata_listener::~set_metadata_listener() OPENVRML_NOTHROW
{}

/**
 * @brief Get the @c eventIn identifier.
 *
 * @return the @c eventIn identifier.
 */
const std::string
openvrml::script_node::set_metadata_listener::do_eventin_id() const OPENVRML_NOTHROW
{
    return "set_metadata";
}

/**
 * @brief Process an event.
 *
 * @param[in] value     new metadata value.
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::script_node::set_metadata_listener::
do_process_event(const sfnode & value, const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    assert(dynamic_cast<openvrml::script_node *>(&this->node()));
    openvrml::script_node & script_node =
        dynamic_cast<openvrml::script_node &>(this->node());

    script_node.metadata_ = value;
    //
    // metadata is an exposedField.
    //
    node::emit_event(script_node.metadata_changed_emitter_, timestamp);
}

/**
 * @internal
 *
 * @class openvrml::script_node::metadata_changed_emitter openvrml/script.h
 *
 * @brief @c metadata_changed event emitter.
 */

/**
 * @brief Construct.
 *
 * @param[in] value the associated field value.
 */
openvrml::script_node::metadata_changed_emitter::
metadata_changed_emitter(const sfnode & value) OPENVRML_NOTHROW:
    openvrml::event_emitter(value),
    openvrml::sfnode_emitter(value)
{}

/**
 * @brief Destroy.
 */
openvrml::script_node::metadata_changed_emitter::~metadata_changed_emitter()
    OPENVRML_NOTHROW
{}

/**
 * @brief The @c eventOut identifier.
 *
 * @return the @c eventOut identifier.
 */
const std::string
openvrml::script_node::metadata_changed_emitter::do_eventout_id() const
    OPENVRML_NOTHROW
{
    return "metadata_changed";
}

/**
 * @internal
 *
 * @var openvrml::script_node::script_node_type openvrml::script_node::type_
 *
 * @brief Type object for the @c script_node instance.
 *
 * Script @c node @c node_type%s are significantly different from other
 * @c node_type%s.  While most @c node_types are shared by the @c node
 * instances they spawn, the @c script_node_type is unique to a @c script_node
 * instance, and it shares the <code>script_node</code>'s lifetime.  This
 * reflects the fact that Script @c node%s in VRML get their functionality by
 * the addition of @c field%s, @c eventIn%s, and @c eventOut%s on a
 * per-instance basis.
 *
 * For @c script_node instances, @c node::node_type is an alias for the
 * @c script_node_type object.
 */

/**
 * @var openvrml::script_node::set_metadata_listener openvrml::script_node::set_metadata_listener_
 *
 * @brief @c set_metadata @c eventIn handler.
 */

/**
 * @var openvrml::sfnode openvrml::script_node::metadata_
 *
 * @brief @c metadata field value.
 */

/**
 * @var openvrml::script_node::metadata_changed_emitter openvrml::script_node::metadata_changed_emitter_
 *
 * @brief @c metadata_changed @c eventOut emitter.
 */

/**
 * @internal
 *
 * @var openvrml::sfbool openvrml::script_node::direct_output
 *
 * @brief @c directOutput field.
 */

/**
 * @internal
 *
 * @var openvrml::sfbool openvrml::script_node::must_evaluate
 *
 * @brief @c mustEvaluate field.
 */

/**
 * @internal
 *
 * @var openvrml::script_node::set_url_listener_t openvrml::script_node::set_url_listener
 *
 * @brief @c set_url @c eventIn handler.
 */

/**
 * @internal
 *
 * @var openvrml::mfstring openvrml::script_node::url_
 *
 * @brief @c url @c exposedField.
 */

/**
 * @internal
 *
 * @var openvrml::script_node::url_changed_emitter openvrml::script_node::url_changed_emitter_
 *
 * @brief url_changed eventOut emitter.
 */

/**
 * @internal
 *
 * @var openvrml::script_node::field_value_map_t openvrml::script_node::field_value_map_
 *
 * @brief Maps user-defined field names to their values.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::event_listener_ptr
 *
 * @brief Reference-counted smart pointer to an event_listener.
 */

/**
 * @internal
 *
 * @typedef openvrml::script_node::event_listener_map_t
 *
 * @brief Map of event listeners.
 */

/**
 * @internal
 *
 * @var openvrml::script_node::event_listener_map_t openvrml::script_node::event_listener_map_
 *
 * @brief Map of event listeners.
 */

/**
 * @internal
 *
 * @var openvrml::script_node::eventout_map_t openvrml::script_node::eventout_map_
 *
 * @brief Map of eventout instances.
 */

/**
 * @internal
 *
 * @var openvrml::script * openvrml::script_node::script_
 *
 * @brief A pointer to a script object.
 */

/**
 * @internal
 *
 * @var int openvrml::script_node::events_received
 *
 * @brief A count of the number of events received since @c #update was
 *        called.
 */

/**
 * @brief Construct.
 *
 * Unlike other concrete node types, which are always instantiated via
 * @c node_type::create_node, the @c script_node constructor is called
 * directly when creating a new @c script_node from scratch.  However, a
 * @c script_node can be duplicated (or &ldquo;cloned&rdquo;) by calling
 * @c node_type::create_node on @c #type of a @c script_node instance.  This
 * provides a consistent interface for cloning any node, regardless of its
 * type.  OpenVRML uses this internally when instantiating @c PROTO%s.
 *
 * @param[in] class_            the @c script_node_metatype.  Typically
 *                              there is one @c script_node_metatype per
 *                              browser instance.
 * @param[in] scope             the @c scope to which the node should
 *                              belong.
 * @param[in] interfaces        a @c node_interface_set containing
 *                              specifications of user-defined @c field%s,
 *                              @c eventIn%s, and @c eventOut%s particular to
 *                              the @c script_node instance.
 * @param[in] initial_values    a map of initial values for @c field%s of the
 *                              @c script_node.
 *
 * @exception unsupported_interface if @p initial_values specifies a field that
 *                                  is not supported by the @c script_node.
 * @exception std::bad_cast         if @p initial_values includes a field value
 *                                  that is the wrong type for the specified
 *                                  field.
 * @exception std::bad_alloc        if memory allocation fails.
 * @exception std::invalid_argument if:
 *                                  - @p interfaces includes an @c exposedField
 *                                    specification.
 *                                  - @p interfaces includes an interface
 *                                    specification that duplicates an existing
 *                                    Script @c node interface.
 *                                  - @p initial_values is missing an initial
 *                                    value for a user-defined field in
 *                                    @p interfaces.
 */
openvrml::script_node::
script_node(script_node_metatype & class_,
            const boost::shared_ptr<openvrml::scope> & scope,
            const node_interface_set & interfaces,
            const initial_value_map & initial_values)
    OPENVRML_THROW4(unsupported_interface, std::bad_cast, std::bad_alloc,
                    std::invalid_argument):
    node(this->type_, scope),
    bounded_volume_node(this->type_, scope),
    child_node(this->type_, scope),
    type_(class_),
    set_metadata_listener_(*this),
    metadata_changed_emitter_(this->metadata_),
    direct_output(false),
    must_evaluate(false),
    set_url_listener(*this),
    url_changed_emitter_(this->url_),
    script_(0),
    events_received(0)
{
    //
    // The refcount is incremented for the body of this constructor.  This
    // keeps the node from potentially being deleted during the refcount
    // manipulation that must be done to accommodate self-referential Script
    // nodes.
    //
    this->add_ref();
    openvrml_::scope_guard guard =
        openvrml_::make_obj_guard(*this, &script_node::remove_ref);
    boost::ignore_unused_variable_warning(guard);

    for (node_interface_set::const_iterator interface_ = interfaces.begin();
         interface_ != interfaces.end();
         ++interface_) {
        using std::invalid_argument;
        using std::auto_ptr;
        using std::pair;
        using std::string;
        using boost::shared_ptr;
        using boost::polymorphic_downcast;
        shared_ptr<openvrml::event_listener> listener;
        shared_ptr<eventout> eventout;
        initial_value_map::const_iterator initial_value;
        pair<string, shared_ptr<field_value> > field_value;
        auto_ptr<openvrml::field_value> cloned_field_value;
        bool succeeded;
        switch (interface_->type) {
        case node_interface::eventin_id:
            listener = create_listener(interface_->field_type,
                                       interface_->id,
                                       *this);
            succeeded = this->event_listener_map_
                .insert(make_pair(interface_->id, listener)).second;
            break;
        case node_interface::eventout_id:
            eventout.reset(new script_node::eventout(interface_->field_type,
                                                     *this));
            succeeded = this->eventout_map_
                .insert(make_pair(interface_->id, eventout)).second;
            break;
        case node_interface::field_id:
            initial_value = initial_values.find(interface_->id);
            if (initial_value == initial_values.end()) {
                throw invalid_argument("Missing initial value for field \""
                                       + interface_->id + "\".");
            }
            cloned_field_value = initial_value->second->clone();
            field_value = make_pair(initial_value->first,
                                    shared_ptr<openvrml::field_value>(
                                        cloned_field_value));
            //
            // Account for self-references.
            //
            // Replace instances of node_ptr::self with a real reference to
            // the current script_node.
            //
            // After making the assignment, we must decrement the node
            // pointer's count because a Script node self-reference must not be
            // an owning pointer. See the assign_with_self_ref_check functions
            // for more information.
            //
            using boost::intrusive_ptr;
            if (interface_->field_type == openvrml::field_value::sfnode_id) {
                sfnode & sfnode_field_value =
                    *polymorphic_downcast<sfnode *>(field_value.second.get());
                if (sfnode_field_value.value() == node::self_tag) {
                    const intrusive_ptr<node> self(this);
                    sfnode_field_value.value(self);
                    this->remove_ref();
                }
            } else if (interface_->field_type
                       == openvrml::field_value::mfnode_id) {
                mfnode & mfnode_field_value =
                    *polymorphic_downcast<mfnode *>(field_value.second.get());
                using std::vector;
                vector<intrusive_ptr<node > > temp =
                    mfnode_field_value.value();
                size_t self_ref_count = 0;
                for (vector<intrusive_ptr<node> >::iterator n = temp.begin();
                     n != temp.end();
                     ++n) {
                    if (*n == node::self_tag) {
                        intrusive_ptr<node> self(this);
                        *n = self;
                        ++self_ref_count;
                    }
                }
                mfnode_field_value.value(temp);
                while (self_ref_count--) { this->remove_ref(); }
            }

            succeeded = this->field_value_map_.insert(field_value).second;
            break;
        case node_interface::exposedfield_id:
            throw invalid_argument("Script cannot support user-defined "
                                   "exposedFields.");
        default:
            assert(false);
        }
        assert(succeeded);
        this->type_.add_interface(*interface_); // Throws std::invalid_argument.
    }

    for (initial_value_map::const_iterator initial_value =
             initial_values.begin();
         initial_value != initial_values.end();
         ++initial_value) {
        if (initial_value->first == "url") {
            this->url_.assign(*initial_value->second);
        } else if (initial_value->first == "directOutput") {
            this->direct_output.assign(*initial_value->second);
        } else if (initial_value->first == "mustEvaluate") {
            this->must_evaluate.assign(*initial_value->second);
        } else if (initial_value->first == "metadata") {
            this->metadata_.assign(*initial_value->second);
        }
    }
}

/**
 * @brief Destroy.
 */
openvrml::script_node::~script_node() OPENVRML_NOTHROW
{}

/**
 * @internal
 *
 * @brief Return a pointer to this @c script_node.
 *
 * @return a pointer to this @c script_node.
 */
openvrml::script_node * openvrml::script_node::to_script() OPENVRML_NOTHROW
{
    return this;
}

/**
 * @brief Update the script_node for the current time.
 *
 * @param[in] current_time  the current time.
 */
void openvrml::script_node::update(const double current_time)
{
    if (this->events_received > 0) {
        this->events_received = 0;
        if (this->script_) {
            this->script_->events_processed(current_time);
        }
    }

    //
    // For each modified eventOut, send an event.
    //
    for (eventout_map_t::iterator eventout = this->eventout_map_.begin();
         eventout != this->eventout_map_.end();
         ++eventout) {
        if (eventout->second->modified()) {
            eventout->second->emit_event(current_time);
        }
    }
}

/**
 * @brief Event listener map accessor.
 *
 * @return the event listener map.
 */
const openvrml::script_node::event_listener_map_t &
openvrml::script_node::event_listener_map() const OPENVRML_NOTHROW
{
    return this->event_listener_map_;
}

/**
 * @brief Field value map accessor.
 *
 * @return the field value map.
 */
const openvrml::script_node::field_value_map_t &
openvrml::script_node::field_value_map() const OPENVRML_NOTHROW
{
    return this->field_value_map_;
}

/**
 * @brief @c eventOut map accessor.
 *
 * @return the @c eventOut map.
 */
const openvrml::script_node::eventout_map_t &
openvrml::script_node::eventout_map() const OPENVRML_NOTHROW
{
    return this->eventout_map_;
}

/**
 * @internal
 *
 * @brief Special assignment function to take into account the fact that
 *        Script nodes can be self-referential.
 *
 * &ldquo;Undo&rdquo; the refcounting appropriately: decrement the refcount on
 * any self-references we acquire ownership of and increment the refcount on
 * any self-references for which ownership is relinquished.
 *
 * @param[in]  inval    input @c sfnode.
 * @param[out] retval   output @c sfnode.
 */
void openvrml::script_node::assign_with_self_ref_check(const sfnode & inval,
                                                       sfnode & retval) const
    OPENVRML_NOTHROW
{
    using boost::intrusive_ptr;

    const intrusive_ptr<node> & old_node = retval.value();

    //
    // About to relinquish ownership of a sfnode value. If the
    // sfnode value is this Script node, then we need to
    // *increment* our refcount, since we previously
    // *decremented* it to accommodate creating a cycle between
    // refcounted objects.
    //
    if (dynamic_cast<script_node *>(old_node.get()) == this) {
        this->add_ref();
    }

    retval = inval;
    //
    // Now, check to see if the new sfnode value is a self-
    // reference. If it is, we need to *decrement* the refcount.
    // A self-reference creates a cycle. If a Script node with
    // a self-reference were completely removed from the world,
    // it still wouldn't be deleted (if we didn't do this)
    // because the reference it held to itself would prevent the
    // refcount from ever dropping to zero.
    //
    const intrusive_ptr<node> & new_node = retval.value();
    if (dynamic_cast<script_node *>(new_node.get()) == this) {
        this->release();
    }
}

/**
 * @internal
 *
 * @brief Special assignment function to take into account the fact that
 *        Script nodes can be self referential.
 *
 * &ldquo;Undo&rdquo; the refcounting appropriately: decrement the refcount on
 * any self-references we acquire ownership of and increment the refcount on
 * any self-references for which ownership is relinquished.
 *
 * @param[in]  inval    input @c mfnode.
 * @param[out] retval   output @c mfnode.
 */
void openvrml::script_node::assign_with_self_ref_check(const mfnode & inval,
                                                       mfnode & retval) const
    OPENVRML_NOTHROW
{
    using boost::intrusive_ptr;

    mfnode::value_type::size_type i;
    for (i = 0; i < retval.value().size(); ++i) {
        const intrusive_ptr<node> & old_node = retval.value()[i];
        if (dynamic_cast<script_node *>(old_node.get()) == this) {
            this->add_ref();
        }
    }

    retval = inval;

    for (i = 0; i < retval.value().size(); ++i) {
        const intrusive_ptr<node> & new_node = retval.value()[i];
        if (dynamic_cast<script_node *>(new_node.get()) == this) {
            this->release();
        }
    }
}

/**
 * @brief Initialize.
 *
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::script_node::do_initialize(const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser().add_script(*this);

    this->events_received = 0;
    
    this->script_.reset(this->create_script().release());

    try {
        if (this->script_) { this->script_->initialize(timestamp); }
    } catch (std::bad_alloc &) {
        throw;
    } catch (std::exception & ex) {
        using std::string;
        const string id = this->id();
        this->scene()->browser().err(
            "caught exception during initialization of "
            + (id.empty() ? string("Script node") : id) + ": " + ex.what());
    } catch (...) {
        using std::string;
        const string id = this->id();
        this->scene()->browser().err(
            "caught unknown exception during initialization of "
            + (id.empty() ? string("Script node") : id));
    }

    //
    // For each modified eventOut, send an event.
    //
    for (eventout_map_t::iterator eventout = this->eventout_map_.begin();
         eventout != this->eventout_map_.end();
         ++eventout) {
        if (eventout->second->modified()) {
            eventout->second->emit_event(timestamp);
        }
    }
}

/**
 * @brief Get the value of a field.
 *
 * @param[in] id    the name of the field to get.
 *
 * @return the value for field @p id.
 *
 * @exception unsupported_interface  if the node has no field @p id.
 */
const openvrml::field_value &
openvrml::script_node::do_field(const std::string & id) const
    OPENVRML_THROW1(unsupported_interface)
{
    field_value_map_t::const_iterator itr;
    if (id == "url") {
        return this->url_;
    } else if (id == "directOutput") {
        return this->direct_output;
    } else if (id == "mustEvaluate") {
        return this->must_evaluate;
    } else if (id == "metadata") {
        return this->metadata_;
    } else if ((itr = this->field_value_map_.find(id))
            != this->field_value_map_.end()) {
        return *itr->second;
    }
    throw unsupported_interface(this->type_, node_interface::field_id, id);
}

/**
 * @brief Get an event listener.
 *
 * This method is called by @c node::event_listener.  Subclasses must
 * implement this method.
 *
 * @param[in] id    @c eventIn identifier.
 *
 * @return the event listener.
 *
 * @exception unsupported_interface if the node has no @c eventIn @p id.
 */
openvrml::event_listener &
openvrml::script_node::do_event_listener(const std::string & id)
    OPENVRML_THROW1(unsupported_interface)
{
    if (id == "url" || id == "set_url") {
        return this->set_url_listener;
    } else if (id == "metadata" || id == "set_metadata") {
        return this->set_metadata_listener_;
    } else {
        event_listener_map_t::iterator pos;
        const event_listener_map_t::iterator end =
            this->event_listener_map_.end();
        if ((pos = this->event_listener_map_.find(id)) != end) {
            return *pos->second;
        } else if ((pos = this->event_listener_map_.find("set_" + id))
                   != end) {
            return *pos->second;
        }
    }
    throw unsupported_interface(this->type_, node_interface::eventin_id, id);
}

/**
 * @brief Get an event emitter.
 *
 * This method is called by @c node::event_emitter.
 *
 * @param[in] id    @c eventOut identifier.
 *
 * @return the event emitter.
 *
 * @exception unsupported_interface if the @c node has no @c eventOut @p id.
 */
openvrml::event_emitter &
openvrml::script_node::do_event_emitter(const std::string & id)
    OPENVRML_THROW1(unsupported_interface)
{
    openvrml::event_emitter * result = 0;
    if (id == "url" || id == "url_changed") {
        result = &this->url_changed_emitter_;
    } else if (id == "metadata" || id == "metadata_changed") {
        result = &this->metadata_changed_emitter_;
    } else {
        eventout_map_t::iterator pos;
        const eventout_map_t::iterator end = this->eventout_map_.end();
        if ((pos = this->eventout_map_.find(id)) != end) {
            result = &pos->second->emitter();
        } else if ((pos = this->eventout_map_.find(id + "_changed")) != end) {
            result = &pos->second->emitter();
        } else {
            throw unsupported_interface(this->type_,
                                        node_interface::eventout_id,
                                        id);
        }
    }
    assert(result);
    return *result;
}

/**
 * @fn const openvrml::script_node::field_value_map_t & openvrml::script_node::field_value_map() const
 *
 * @brief field value map.
 *
 * @return the field value map.
 */

/**
 * @fn const openvrml::script_node::eventout_map_t & openvrml::script_node::eventout_map() const
 *
 * @brief @c eventOut map.
 *
 * @return the @c eventOut map.
 */

/**
 * @brief Called by @c node::shutdown.
 *
 * @param[in] timestamp the current time.
 */
void openvrml::script_node::do_shutdown(const double timestamp)
    OPENVRML_NOTHROW
{
    try {
        if (this->script_) { this->script_->shutdown(timestamp); }
    } catch (std::exception & ex) {
        using std::string;
        const string id = this->id();
        this->scene()->browser().err(
            "caught exception during shutdown of "
            + (id.empty() ? string("Script node") : id) + ": " + ex.what());
    } catch (...) {
        using std::string;
        const string id = this->id();
        this->scene()->browser().err(
            "caught unknown exception during shutdown of "
            + (id.empty() ? string("Script node") : id));
    }
    this->scene()->browser().remove_script(*this);
}

/**
 * @brief @c node::render_child implementation.
 *
 * @param[in,out] v         viewer implementation responsible for actually
 *                          doing the drawing.
 * @param[in]     context   generic context argument; holds things like the
 *                          accumulated modelview transform.
 */
void openvrml::script_node::do_render_child(viewer &, rendering_context)
{}

namespace {
    //
    // script_factory::create_script wants a resource_istream; but in the case
    // of an inline script, the script is contained in the URL string and
    // doesn't need to be fetched.  string_resource_istream is simply a
    // construct to accommodate a consistent interface.  Note that the url()
    // and type() members of string_resource_istream return empty strings.
    //
    class OPENVRML_LOCAL string_resource_istream :
        public openvrml::resource_istream {

        std::stringbuf buf_;

    public:
        explicit string_resource_istream(const std::string & str):
            resource_istream(&this->buf_)
        {
            this->buf_.str(str);
        }

        virtual const std::string do_url() const
            OPENVRML_THROW1(std::bad_alloc)
        {
            return std::string();
        }

        virtual const std::string do_type() const
            OPENVRML_THROW1(std::bad_alloc)
        {
            return std::string();
        }

        virtual bool do_data_available() const OPENVRML_NOTHROW
        {
            return true;
        }
    };
}

/**
 * @internal
 *
 * @brief Create a script object.
 *
 * @return a new script object.
 */
std::auto_ptr<openvrml::script> openvrml::script_node::create_script()
    OPENVRML_THROW2(no_alternative_url, std::bad_alloc)
{
    using std::string;
    using std::vector;

    boost::shared_ptr<resource_istream> script_code;
    boost::shared_ptr<script_factory> factory;

    //
    // Try each URI until we find one we like.
    //
    const vector<string> urls = this->url_.value();
    for (vector<string>::const_iterator url(urls.begin());
         url != urls.end() && !factory;
         ++url) {

        if (url->empty()) { continue; }

        //
        // First, see if the URI scheme is something we recognize.
        //
        const string::size_type colon_index = url->find_first_of(':');

        const string scheme = url->substr(0, colon_index);

        factory = script_node::script_factory_registry_.impl_
            ->find_using_uri_scheme(scheme);

        if (factory) {
            script_code.reset(
                new string_resource_istream(url->substr(colon_index + 1)));
            break;
        }

        using openvrml::local::uri;
        uri test_uri, absolute_uri;
        std::auto_ptr<resource_istream> in;
        try {
            //
            // Throw invalid_url if it isn't a valid URI.
            //
            test_uri = uri(*url);

            absolute_uri = !relative(test_uri)
                         ? test_uri
                         : (!this->scene()->parent()
                            && this->scene()->url().empty())
                             ? create_file_url(test_uri)
                             : test_uri.resolve_against(uri(this->scene()->url()));
            in = this->scene()->browser().fetcher_.get_resource(absolute_uri);
        } catch (invalid_url &) {
            std::ostringstream msg;
            msg << *url << ": invalid URI";
            this->scene()->browser().err(msg.str());
            continue;
        } catch (std::exception &) {
            std::ostringstream msg;
            msg << string(!absolute_uri.scheme().empty() ? absolute_uri
                                                         : test_uri);
            continue;
        } catch (...) {
            //
            // Swallow unrecognized exceptions.  Output to browser::err
            // happens below when "in" is found to be unusable.
            //
        }
        if (!in.get() || !(*in)) {
            std::ostringstream msg;
            msg << string(absolute_uri)
                << ": unrecognized error during resolution";
            this->scene()->browser().err(msg.str());
            continue;
        }

        factory = script_node::script_factory_registry_.impl_
            ->find_using_media_type(in->type());
        if (factory) {
            script_code = in;
            break; // Success
        }
    }

    return factory
        ? factory->create_script(*this, script_code)
        : std::auto_ptr<script>();
}
