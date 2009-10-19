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

# include "scene.h"
# include "browser.h"
# include <openvrml/local/uri.h>
# include <openvrml/local/parse_vrml.h>
# include <private.h>
# include <boost/function.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

/**
 * @file openvrml/scene.h
 *
 * @brief Definition of @c openvrml::scene.
 */

/**
 * @class openvrml::scene openvrml/scene.h
 *
 * @brief A scene in the VRML world.
 */

/**
 * @internal
 *
 * @var openvrml::browser * const openvrml::scene::browser_
 *
 * @brief A reference to the @c browser associated with the @c scene.
 */

/**
 * @internal
 *
 * @var openvrml::scene * const openvrml::scene::parent_
 *
 * @brief A pointer to the parent @c scene.
 *
 * If the @c scene is the root @c scene, @a parent will be 0.
 */

/**
 * @internal
 *
 * @var boost::shared_mutex openvrml::scene::nodes_mutex_
 *
 * @brief Mutex protecting @a nodes_.
 */

/**
 * @internal
 *
 * @var openvrml::mfnode openvrml::scene::nodes_
 *
 * @brief The nodes for the scene.
 */

/**
 * @internal
 *
 * @var boost::shared_mutex openvrml::scene::url_mutex_
 *
 * @brief Mutex protecting @a url_.
 */

/**
 * @internal
 *
 * @var const std::string openvrml::scene::url_
 *
 * @brief The URI for the scene.
 *
 * This may be a relative or an absolute reference.
 */

/**
 * @internal
 *
 * @var boost::shared_mutex openvrml::scene::meta_mutex_
 *
 * @brief Mutex protecting @c #meta_.
 */

/**
 * @internal
 *
 * @var std::map<std::string, std::string> openvrml::scene::meta_
 *
 * @brief Scene metadata map.
 */

/**
 * @internal
 *
 * @var boost::thread_group openvrml::scene::stream_reader_threads_
 *
 * @brief Stream reader thread group.
 */

/**
 * @brief Construct.
 *
 * @param[in] browser   the @c browser associated with the @c scene.
 * @param[in] parent    the parent @c scene.
 */
openvrml::scene::scene(openvrml::browser & browser, scene * parent)
    OPENVRML_NOTHROW:
    browser_(&browser),
    parent_(parent)
{}

/**
 * @brief Destroy.
 */
openvrml::scene::~scene() OPENVRML_NOTHROW
{
    this->stream_reader_threads_.join_all();
}

/**
 * @brief Get the associated @c browser.
 *
 * @return the associated @c browser.
 */
openvrml::browser & openvrml::scene::browser() const OPENVRML_NOTHROW
{
    return *this->browser_;
}

/**
 * @brief Get the parent @c scene.
 *
 * @return the parent @c scene, or 0 if this is the root @c scene.
 */
openvrml::scene * openvrml::scene::parent() const OPENVRML_NOTHROW
{
    return this->parent_;
}

/**
 * @brief Load the @c scene from a stream.
 *
 * @param[in,out] in    an input stream.
 *
 * @exception bad_media_type    if @p in.type() is not
 *                              &ldquo;model/vrml&rdquo;,
 *                              &ldquo;x-world/x-vrml&rdquo;, or
 *                              &ldquo;model/x3d-vrml&rdquo;.
 * @exception invalid_vrml      if @p in has invalid syntax.
 */
void openvrml::scene::load(resource_istream & in)
{
    {
        using boost::unique_lock;
        using boost::shared_mutex;
        unique_lock<shared_mutex>
            nodes_lock(this->nodes_mutex_),
            url_lock(this->url_mutex_),
            meta_lock(this->meta_mutex_);

        this->nodes_.clear();
        this->meta_.clear();
        this->url_ = in.url();
        local::parse_vrml(in, in.url(), in.type(),
                          *this, this->nodes_, this->meta_);
    }
    this->scene_loaded();
}

/**
 * @brief Initialize the @c scene.
 *
 * @param[in] timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::scene::initialize(const double timestamp)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::shared_lock;
    using boost::shared_mutex;
    shared_lock<shared_mutex> lock(this->nodes_mutex_);
    for (std::vector<boost::intrusive_ptr<node> >::iterator node(
             this->nodes_.begin());
         node != this->nodes_.end();
         ++node) {
        assert(*node);
        (*node)->initialize(*this, timestamp);
        if (child_node * const child = node_cast<child_node *>(node->get())) {
            child->relocate();
        }
    }
}

/**
 * @brief Get metadata.
 *
 * @param[in] key   metadata key.
 *
 * @return the metadata value associated with @p key.
 *
 * @exception std::invalid_argument if there is no value associated with
 *                                  @p key.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const std::string openvrml::scene::meta(const std::string & key) const
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    using boost::shared_lock;
    using boost::shared_mutex;
    shared_lock<shared_mutex> lock(this->meta_mutex_);

    using std::map;
    using std::string;

    const map<string, string>::const_iterator pos = this->meta_.find(key);
    if (pos == this->meta_.end()) {
        throw std::invalid_argument("no metadata value associated with \""
                                    + key + "\"");
    }
    return pos->second;
}

/**
 * @brief Set metadata.
 *
 * @param[in] key   metadata key.
 * @param[in] value metadata value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::scene::meta(const std::string & key, const std::string & value)
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::unique_lock;
    using boost::shared_mutex;
    unique_lock<shared_mutex> lock(this->meta_mutex_);
    this->meta_[key] = value;
}

/**
 * @brief Get the metadata keys.
 *
 * @return the metadata keys.
 */
const std::vector<std::string> openvrml::scene::meta_keys() const
    OPENVRML_THROW1(std::bad_alloc)
{
    using boost::shared_lock;
    using boost::shared_mutex;
    shared_lock<shared_mutex> lock(this->meta_mutex_);

    using std::map;
    using std::string;
    using std::vector;

    vector<string> result;
    for (map<string, string>::const_iterator entry = this->meta_.begin();
         entry != this->meta_.end();
         ++entry) {
        result.push_back(entry->first);
    }
    return result;
}

/**
 * @brief Root @c node%s for the @c scene.
 *
 * @return the root @c node%s for the @c scene.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::vector<boost::intrusive_ptr<openvrml::node> >
openvrml::scene::nodes() const OPENVRML_THROW1(std::bad_alloc)
{
    using boost::shared_lock;
    using boost::shared_mutex;
    shared_lock<shared_mutex> lock(this->nodes_mutex_);
    return this->nodes_;
}

/**
 * @brief Set the root @c node%s for the @c scene.
 *
 * This function calls @c #shutdown to shut down the
 * <code>scene</code>'s existing @c node%s.
 *
 * @param[in] n the new root @c node%s for the @c scene.
 *
 * @exception std::invalid_argument if any of the @c node%s in @p n has
 *                                  already been initialized.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void openvrml::scene::nodes(const std::vector<boost::intrusive_ptr<node> > & n)
    OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
{
    using boost::unique_lock;
    using boost::shared_mutex;

    class check_uninitialized_traverser : public node_traverser {
    private:
        virtual void on_entering(node & n)
        {
            if (n.scene()) {
                throw std::invalid_argument("node already initialized");
            }
        }
    } check_uninitialized;
    check_uninitialized.traverse(n);

    unique_lock<shared_mutex> lock(this->nodes_mutex_);
    const double now = browser::current_time();
    this->shutdown(now);
    this->nodes_ = n;
}

/**
 * @brief Get the root @c scope.
 *
 * @return the root @c scope.
 */
const openvrml::scope * openvrml::scene::root_scope() const
    OPENVRML_NOTHROW
{
    using boost::shared_lock;
    using boost::shared_mutex;
    shared_lock<shared_mutex> lock(this->nodes_mutex_);
    return this->nodes_.empty()
        ? 0
        : &this->nodes_.front()->scope();
}

/**
 * @brief Get the absolute URI for the @c scene.
 *
 * @return the absolute URI for the @c scene.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
const std::string openvrml::scene::url() const OPENVRML_THROW1(std::bad_alloc)
{
    using boost::shared_lock;
    using boost::shared_mutex;
    shared_lock<shared_mutex> lock(this->url_mutex_);
    using std::string;
    using local::uri;
    return (this->parent_ && !this->url_.empty() && relative(uri(this->url_)))
        ? string(resolve_against(uri(this->url_), uri(this->parent_->url())))
        : this->url_;
}

/**
 * @brief Render the scene.
 *
 * @param[in,out] viewer    a @c viewer to render to.
 * @param[in]     context   a @c rendering_context.
 */
void openvrml::scene::render(openvrml::viewer & viewer,
                             rendering_context context)
{
    using boost::shared_lock;
    using boost::shared_mutex;
    shared_lock<shared_mutex> lock(this->nodes_mutex_);
    for (std::vector<boost::intrusive_ptr<node> >::iterator n(
             this->nodes_.begin());
         n != this->nodes_.end();
         ++n) {
        assert(*n);
        child_node * child = node_cast<child_node *>(n->get());
        if (child) { child->render_child(viewer, context); }
    }
}

namespace {

    struct OPENVRML_LOCAL stream_reader {
        stream_reader(std::auto_ptr<openvrml::resource_istream> in,
                      std::auto_ptr<openvrml::stream_listener> listener):
            in_(in),
            listener_(listener)
        {}

        void operator()() const
        {
            this->listener_->stream_available(this->in_->url(),
                                              this->in_->type());
            while (*this->in_) {
                std::vector<unsigned char> data;
                while (this->in_->data_available()) {
                    using openvrml::resource_istream;
                    const resource_istream::int_type c = this->in_->get();
                    if (c != resource_istream::traits_type::eof()) {
                        data.push_back(
                            resource_istream::traits_type::to_char_type(c));
                    } else {
                        break;
                    }
                }
                if (!data.empty()) {
                    this->listener_->data_available(data);
                }
            }
        }

    private:
        boost::shared_ptr<openvrml::resource_istream> in_;
        boost::shared_ptr<openvrml::stream_listener> listener_;
    };
}

/**
 * @brief Read a stream in a new thread.
 *
 * @c #read_stream takes ownership of its arguments; the resources are released
 * when reading the stream completes and the thread terminates.
 *
 * @param[in] in        an input stream.
 * @param[in] listener  a stream listener.
 */
void openvrml::scene::read_stream(std::auto_ptr<resource_istream> in,
                                  std::auto_ptr<stream_listener> listener)
{
    boost::function0<void> f = stream_reader(in, listener);
    this->stream_reader_threads_.create_thread(f);
}

struct OPENVRML_LOCAL openvrml::scene::vrml_from_url_creator {
    vrml_from_url_creator(openvrml::scene & scene,
                          const std::vector<std::string> & url,
                          const boost::intrusive_ptr<node> & node,
                          const std::string & event)
        OPENVRML_THROW2(unsupported_interface, std::bad_cast):
        scene_(&scene),
        url_(&url),
        node_(node),
        listener_(&dynamic_cast<mfnode_listener &>(
                      node->event_listener(event)))
    {}

    void operator()() const OPENVRML_NOTHROW
    {
        try {
            try {
                std::auto_ptr<resource_istream> in =
                    this->scene_->get_resource(*this->url_);
                if (!(*in)) { throw unreachable_url(); }
                mfnode nodes;
                nodes.value(
                    this->scene_->browser()
                    .create_vrml_from_stream(*in, in->type()));
                this->listener_->process_event(nodes, browser::current_time());
            } catch (std::exception & ex) {
                this->scene_->browser().err(ex.what());
                throw unreachable_url();
            } catch (...) {
                //
                // The implementation of resource_istream is provided by the
                // user; and unfortunately, operations on it could throw
                // anything.
                //
                throw unreachable_url();
            }
        } catch (std::exception & ex) {
            this->scene_->browser().err(ex.what());
        }
    }

private:
    openvrml::scene * const scene_;
    const std::vector<std::string> * const url_;
    const boost::intrusive_ptr<node> node_;
    mfnode_listener * const listener_;
};

/**
 * @brief Create nodes from a URI.
 *
 * This function executes asynchronously. When the nodes have been completely
 * loaded, they are sent to the @p event MFNode eventIn of @p node.
 *
 * @param[in] url       an alternative URI list.
 * @param[in] node      the node to which the nodes loaded from @p url should be
 *                      sent as an event.
 * @param[in] event     the event of @p node to which the new nodes will be sent.
 *
 * @exception unsupported_interface         if @p node has no eventIn @p event.
 * @exception std::bad_cast                 if the @p event eventIn of @p node
 *                                          is not an MFNode.
 * @exception boost::thread_resource_error  if thread creation fails.
 */
void
openvrml::scene::
create_vrml_from_url(const std::vector<std::string> & url,
                     const boost::intrusive_ptr<node> & node,
                     const std::string & event)
    OPENVRML_THROW3(unsupported_interface, std::bad_cast,
                    boost::thread_resource_error)
{
    boost::function0<void> f = vrml_from_url_creator(*this, url, node, event);
    this->stream_reader_threads_.create_thread(f);
}

/**
 * @brief Load a resource into @a browser.
 *
 * This method simply resolves any relative references in @p uri and calls
 * @c browser::load_url.
 *
 * @note There are a couple of edge cases here where we are probably doing the
 *      wrong thing:
 *       - If there is a URI in the list of the form
 *         &ldquo;<code>\#NodeId</code>&rdquo; and it is not the first URI in
 *         the list, this URI will be loaded as if it were a new world rather
 *         than as a Viewpoint that should simply be bound.
 *       - If the first URI in the list is of the form
 *         &ldquo;<code>\#NodeId</code>&rdquo; and no Viewpoint named
 *         &ldquo;<code>NodeId</code>&rdquo; exists in the scene, this method
 *         will not try any subsequent URIs in the list.
 *
 * @param[in] url       an array of URIs.  Per VRML97 convention, the first
 *                      resource in the sequence that can be reached will be
 *                      loaded into the browser.
 * @param[in] parameter an array of parameters to be associated with the URIs in
 *                      @p uri.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @todo This method currently fails silently if any of the URIs in @p url is
 *       invalid.  Should this throw invalid_url?
 */
void openvrml::scene::load_url(const std::vector<std::string> & url,
                               const std::vector<std::string> & parameter)
    OPENVRML_THROW1(std::bad_alloc)
{
    using std::string;
    using local::uri;

    if (!url.empty()) {
        if (url[0][0] == '#') {
# if 0
            //
            // If the first element in uri is a Viewpoint name, bind the
            // Viewpoint.
            //
            this->browser_->setViewpoint(uri[0].substr(1));
# endif
        } else {
            std::vector<std::string> absoluteURIs(url.size());
            for (size_t i = 0; i < absoluteURIs.size(); ++i) {
                try {
                    const uri urlElement(url[i]);
                    const string value =
                        relative(urlElement)
                            ? resolve_against(urlElement, uri(this->url()))
                            : urlElement;
                    absoluteURIs[i] = value;
                } catch (invalid_url & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                }
            }
            this->browser_->load_url(absoluteURIs, parameter);
        }
    }
}

/**
 * @brief Get a resource using a list of alternative URIs.
 *
 * Relative URIs in @p url are resolved against the absolute URI of the
 * @c scene.
 *
 * @param[in] url   a list of alternative URIs.
 *
 * @return the resource.
 *
 * @exception no_alternative_url    if none of the elements of @p url can be
 *                                  resolved.
 * @exception std::bad_alloc        if memory allocation fails.
 */
std::auto_ptr<openvrml::resource_istream>
openvrml::scene::get_resource(const std::vector<std::string> & url) const
    OPENVRML_THROW2(no_alternative_url, std::bad_alloc)
{
    using std::string;
    using std::vector;
    using local::uri;

    std::auto_ptr<resource_istream> in;

    for (vector<string>::size_type i = 0; i < url.size(); ++i) {
        //
        // If we have a relative reference, resolve it against this->url();
        // unless the parent is null and this->url() is empty, in which case
        // we are loading the root scene.  In that case, construct an absolute
        // file URL.
        //
        uri test_uri, absolute_uri;
        try {
            //
            // Throw invalid_url if it isn't a valid URI.
            //
            test_uri = uri(url[i]);

            absolute_uri = !relative(test_uri)
                         ? test_uri
                         : (!this->parent() && this->url().empty())
                             ? create_file_url(test_uri)
                             : resolve_against(test_uri, uri(this->url()));
            in = this->browser().fetcher_.get_resource(absolute_uri);
        } catch (invalid_url &) {
            std::ostringstream msg;
            msg << url[i] << ": invalid URI";
            this->browser().err(msg.str());
            continue;
        } catch (std::exception & ex) {
            std::ostringstream msg;
            msg << string(!absolute_uri.scheme().empty() ? absolute_uri
                                                         : test_uri)
                << ": " << ex.what();
            this->browser().err(msg.str());
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
            this->browser().err(msg.str());
            continue;
        }
        break; // Success.
    }
    if (!in.get() || !(*in)) { throw no_alternative_url(); }
    return in;
}

/**
 * @brief Shut down the nodes in the scene.
 *
 * This function @b must be called before the @c scene is destroyed.
 *
 * @param[in] timestamp the current time.
 */
void openvrml::scene::shutdown(const double timestamp) OPENVRML_NOTHROW
{
    using boost::shared_lock;
    using boost::shared_mutex;
    shared_lock<shared_mutex> lock(this->nodes_mutex_);
    for (std::vector<boost::intrusive_ptr<node> >::iterator n(
             this->nodes_.begin());
         n != this->nodes_.end();
         ++n) {
        if (*n) { (*n)->shutdown(timestamp); }
    }
}

/**
 * @brief Function called once the @c scene has been loaded.
 *
 * @c #load calls this function once the @c scene has finished loading.  The
 * default implementation does nothing.
 */
void openvrml::scene::scene_loaded()
{}
