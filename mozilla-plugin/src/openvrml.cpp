// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML Mozilla plug-in
// Copyright 2004  Braden N. McDaniel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# include <memory>
# include <iostream>
# include <mozilla-config.h>
# include <npupp.h>
# include <nsCOMPtr.h>
# include <nsMemory.h>
# include <nsString.h>
# include <nsIDOMWindow.h>
# include <dom/nsIDOMWindowInternal.h>
# include <dom/nsIDOMLocation.h>
# include <boost/noncopyable.hpp>
# include <boost/spirit.hpp>
# include <boost/spirit/phoenix.hpp>
# include <openvrml/browser.h>
# include <openvrml/gl/viewer.h>
# if defined MOZ_X11
#   include <X11/keysym.h>
#   include <gdk/gdkx.h>
#   include <gtk/gtk.h>
#   include <gtk/gtkgl.h>
# else
#   error Unsupported toolkit.
# endif
# include "openvrml.h"

extern "C" {
    gboolean realize(GtkWidget * widget, GdkEvent * event, gpointer data);
    gboolean expose_event(GtkWidget * widget, GdkEventExpose * event,
                          gpointer data);
    gboolean configure_event(GtkWidget * widget, GdkEventConfigure * event,
                             gpointer data);
    gboolean key_press_event(GtkWidget * widget, GdkEventKey * event,
                             gpointer data);
    gboolean button_press_event(GtkWidget * widget, GdkEventButton * event,
                                gpointer data);
    gboolean button_release_event(GtkWidget * widget, GdkEventButton * event,
                                  gpointer data);
    gboolean motion_notify_event(GtkWidget * widget, GdkEventMotion * event,
                                 gpointer data);
}

namespace {

    class PluginInstance;

    class GtkGLViewer : public openvrml::gl::viewer {
        PluginInstance & plugin_instance;
        guint timer;

    public:
        bool redrawNeeded;

        explicit GtkGLViewer(PluginInstance & plugin_instance);
        virtual ~GtkGLViewer() throw ();

        void timer_update();

    protected:
        //
        // Implement pure virtual methods from openvrml::gl::viewer.
        //
        virtual void post_redraw();
        virtual void set_cursor(openvrml::gl::viewer::cursor_style);
        virtual void swap_buffers();
        virtual void set_timer(double);
    };

    class PluginInstance : boost::noncopyable {
        friend class GtkGLViewer;
	friend class ScriptablePeer;

        std::string initialURL;
        GtkWidget * plug;
        GtkWidget * drawingArea;
        gint x, y;
        gint width, height;
        GtkGLViewer viewer;
        openvrml::browser browser;
	nsCOMPtr<VrmlBrowser> scriptablePeer;

    public:
        explicit PluginInstance(const std::string & initialURL)
            throw (std::bad_alloc);
        ~PluginInstance() throw ();

	nsISupports * GetScriptablePeer() throw ();
	openvrml::browser & GetBrowser() throw ();
        void SetWindow(NPWindow & window) throw (std::bad_alloc);
        void HandleEvent(void * event) throw ();
    };

    class ScriptablePeer : public nsIClassInfo, public VrmlBrowser {
	PluginInstance & pluginInstance;

    public:
        explicit ScriptablePeer(PluginInstance & pluginInstance);
        ~ScriptablePeer();

        NS_DECL_ISUPPORTS
        NS_DECL_NSICLASSINFO
        NS_DECL_VRMLBROWSER
    };
} // namespace

char * NP_GetMIMEDescription()
{
    return NPP_GetMIMEDescription();
}

namespace {
    NPNetscapeFuncs mozillaFuncs;

# ifdef MOZ_X11
    GdkGLConfig * glConfig;
# endif
}

/**
 * @brief Provides global initialization for a plug-in.
 *
 * @param mozTable      table of functions the plug-in can use to call into
 *                      the browser.
 * @retval pluginFuncs  table of functions that the browser can use to call
 *                      into the plug-in.
 *
 * @return
 *      - @c NPERR_INVALID_FUNCTABLE_ERROR if:
 *          - @p mozTable is 0; or
 *          - @p mozTable->size is smaller than expected; or
 *          - @p pluginFuncs is 0; or
 *          - @p pluginFuncs->size is smaller than expected.
 *      - @c NPERR_INCOMPATIBLE_VERSION_ERROR if:
 *          - @p mozTable->version indicates an incompatible version; or
 *          - (under X) the browser does not support XEmbed; or
 *          - (under X) the browser does not use Gtk2.
 *      - @c NPERR_NO_ERROR otherwise.
 *
 * @see http://devedge.netscape.com/library/manuals/2002/plugin/1.0/npp_api7.html#999264
 */
NPError NP_Initialize(NPNetscapeFuncs * const mozTable,
                      NPPluginFuncs * const pluginFuncs)
{
    if (!mozTable || !pluginFuncs) { return NPERR_INVALID_FUNCTABLE_ERROR; }

    //
    // Check the major version passed in Mozilla's function table.  We won't
    // load if the major version is newer than what we expect.  Also check that
    // the function tables passed in are big enough for all the functions we
    // need.  (They could be bigger if Mozilla added new APIs; but that's okay;
    // we'll just ignore them.)
    //
    if ((mozTable->version >> 8) > NP_VERSION_MAJOR) {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
    if (mozTable->size < sizeof (NPNetscapeFuncs)
        || pluginFuncs->size < sizeof (NPPluginFuncs)) {
        return NPERR_INVALID_FUNCTABLE_ERROR;
    }

    //
    // Copy the fields of the Mozilla function table into our copy so we can
    // call back into Mozilla later.  Note that we need to copy the fields one
    // by one rather than assign the whole struct because the Mozilla function
    // table could be bigger than what we expect.
    //
    mozillaFuncs.version       = mozTable->version;
    mozillaFuncs.size          = mozTable->size;
    mozillaFuncs.posturl       = mozTable->posturl;
    mozillaFuncs.geturl        = mozTable->geturl;
    mozillaFuncs.geturlnotify  = mozTable->geturlnotify;
    mozillaFuncs.requestread   = mozTable->requestread;
    mozillaFuncs.newstream     = mozTable->newstream;
    mozillaFuncs.write         = mozTable->write;
    mozillaFuncs.destroystream = mozTable->destroystream;
    mozillaFuncs.status        = mozTable->status;
    mozillaFuncs.uagent        = mozTable->uagent;
    mozillaFuncs.memalloc      = mozTable->memalloc;
    mozillaFuncs.memfree       = mozTable->memfree;
    mozillaFuncs.memflush      = mozTable->memflush;
    mozillaFuncs.reloadplugins = mozTable->reloadplugins;
    mozillaFuncs.getJavaEnv    = mozTable->getJavaEnv;
    mozillaFuncs.getJavaPeer   = mozTable->getJavaPeer;
    mozillaFuncs.getvalue      = mozTable->getvalue;

    //
    // Set up the plug-in function table that Mozilla will use to call us.
    // Mozilla needs to know about our version and size an have a
    // UniversalProcPointer for every function we implement.
    //
    pluginFuncs->version       = (NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR;
    pluginFuncs->size          = sizeof (NPPluginFuncs);
    pluginFuncs->newp          = NPP_New;
    pluginFuncs->destroy       = NPP_Destroy;
    pluginFuncs->setwindow     = NPP_SetWindow;
    pluginFuncs->newstream     = NPP_NewStream;
    pluginFuncs->destroystream = NPP_DestroyStream;
    pluginFuncs->asfile        = NPP_StreamAsFile;
    pluginFuncs->writeready    = NPP_WriteReady;
    pluginFuncs->write         = NPP_Write;
    pluginFuncs->print         = NPP_Print;
    pluginFuncs->urlnotify     = NPP_URLNotify;
    pluginFuncs->event         = NPP_HandleEvent;
    pluginFuncs->getvalue      = NPP_GetValue;
    pluginFuncs->setvalue      = NPP_SetValue;
    pluginFuncs->javaClass     = 0;

# ifdef MOZ_X11
    NPError err = NPERR_NO_ERROR;

    //
    // Make sure the browser supports XEmbed plug-ins.
    //
    PRBool supportsXEmbed = PR_FALSE;
    err = CallNPN_GetValueProc(mozillaFuncs.getvalue,
                               0,
                               NPNVSupportsXEmbedBool,
                               &supportsXEmbed);

    if (err != NPERR_NO_ERROR || !supportsXEmbed) {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }

    //
    // Make sure the browser tookit is Gtk2.
    //
    NPNToolkitType toolkit = NPNToolkitType();
    err = CallNPN_GetValueProc(mozillaFuncs.getvalue,
                               0,
                               NPNVToolkit,
                               &toolkit);

    if (err != NPERR_NO_ERROR || toolkit != NPNVGtk2) {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }

    static int argc = 0;
    static char ** argv = 0;
    if (!gtk_gl_init_check(&argc, &argv)) { return NPERR_GENERIC_ERROR; }

    static const int attribList[] = {
        // GDK_GL_ALPHA_SIZE, 1,
        GDK_GL_DOUBLEBUFFER,
        GDK_GL_DEPTH_SIZE, 1,
        GDK_GL_RGBA,
        GDK_GL_RED_SIZE, 1,
        GDK_GL_ATTRIB_LIST_NONE
    };
    ::glConfig = gdk_gl_config_new(attribList);
# endif // defined MOZ_X11

    return NPP_Initialize();
}

NPError NP_Shutdown()
{
    NPP_Shutdown();
    return NPERR_NO_ERROR;
}

NPError NP_GetValue(void * future, NPPVariable variable, void * value)
{
    NPError err = NPERR_NO_ERROR;
    switch (variable) {
    case NPPVpluginNameString:
        *static_cast<const char **>(value) = "OpenVRML";
        break;
    case NPPVpluginDescriptionString:
        *static_cast<const char **>(value) = "VRML browser";
        break;
    default:
        err = NPERR_GENERIC_ERROR;
    }
    return err;
}

char * NPP_GetMIMEDescription()
{
    return "model/vrml:wrl:VRML world;"
           "x-world/x-vrml:wrl:VRML world";
}

NPError NPP_Initialize()
{
    return NPERR_NO_ERROR;
}

void NPP_Shutdown()
{
}

namespace {

    class uri {
        struct grammar : public boost::spirit::grammar<grammar> {
            struct absolute_uri_closure :
                boost::spirit::closure<absolute_uri_closure,
                                       std::string::const_iterator,
                                       std::string::const_iterator> {
                member1 scheme_begin;
                member2 scheme_end;
            };

            struct server_closure :
                boost::spirit::closure<server_closure,
                                       std::string::const_iterator,
                                       std::string::const_iterator> {
                member1 userinfo_begin;
                member2 userinfo_end;
            };

            template <typename ScannerT>
            struct definition {
                typedef boost::spirit::rule<ScannerT> rule_type;
                typedef boost::spirit::rule<ScannerT,
                                            absolute_uri_closure::context_t>
                    absolute_uri_rule_type;
                typedef boost::spirit::rule<ScannerT,
                                            server_closure::context_t>
                    server_rule_type;

                rule_type uri_reference;
                absolute_uri_rule_type absolute_uri;
                rule_type relative_uri;
                rule_type hier_part;
                rule_type opaque_part;
                rule_type uric_no_slash;
                rule_type net_path;
                rule_type abs_path;
                rule_type rel_path;
                rule_type rel_segment;
                rule_type scheme;
                rule_type authority;
                rule_type reg_name;
                server_rule_type server;
                rule_type userinfo;
                rule_type hostport;
                rule_type host;
                rule_type hostname;
                rule_type domainlabel;
                rule_type toplabel;
                rule_type ipv4address;
                rule_type port;
                rule_type path_segments;
                rule_type segment;
                rule_type param;
                rule_type pchar;
                rule_type query;
                rule_type fragment;
                rule_type uric;
                rule_type reserved;
                rule_type unreserved;
                rule_type mark;
                rule_type escaped;

                explicit definition(const grammar & self);

                const boost::spirit::rule<ScannerT> & start() const;
            };

            mutable uri & uri_ref;

            explicit grammar(uri & uri_ref) throw ();
        };

        std::string str_;
        std::string::const_iterator scheme_begin, scheme_end;
        std::string::const_iterator scheme_specific_part_begin,
                                    scheme_specific_part_end;
        std::string::const_iterator authority_begin, authority_end;
        std::string::const_iterator userinfo_begin, userinfo_end;
        std::string::const_iterator host_begin, host_end;
        std::string::const_iterator port_begin, port_end;
        std::string::const_iterator path_begin, path_end;
        std::string::const_iterator query_begin, query_end;
        std::string::const_iterator fragment_begin, fragment_end;

    public:
        uri() throw (std::bad_alloc);
        explicit uri(const std::string & str)
            throw (openvrml::invalid_url, std::bad_alloc);

        operator std::string() const throw (std::bad_alloc);

        const std::string scheme() const throw (std::bad_alloc);
        const std::string scheme_specific_part() const throw (std::bad_alloc);
        const std::string authority() const throw (std::bad_alloc);
        const std::string userinfo() const throw (std::bad_alloc);
        const std::string host() const throw (std::bad_alloc);
        const std::string port() const throw (std::bad_alloc);
        const std::string path() const throw (std::bad_alloc);
        const std::string query() const throw (std::bad_alloc);
        const std::string fragment() const throw (std::bad_alloc);

        const uri resolve_against(const uri & absolute_uri) const
            throw (std::bad_alloc);
    };

    uri::grammar::grammar(uri & uri_ref) throw ():
        uri_ref(uri_ref)
    {}

    template <typename ScannerT>
    uri::grammar::definition<ScannerT>::definition(const grammar & self)
    {
        using namespace boost::spirit;
        using namespace phoenix;

        BOOST_SPIRIT_DEBUG_NODE(uri_reference);
        BOOST_SPIRIT_DEBUG_NODE(absolute_uri);
        BOOST_SPIRIT_DEBUG_NODE(relative_uri);
        BOOST_SPIRIT_DEBUG_NODE(hier_part);
        BOOST_SPIRIT_DEBUG_NODE(opaque_part);
        BOOST_SPIRIT_DEBUG_NODE(uric_no_slash);
        BOOST_SPIRIT_DEBUG_NODE(net_path);
        BOOST_SPIRIT_DEBUG_NODE(abs_path);
        BOOST_SPIRIT_DEBUG_NODE(rel_path);
        BOOST_SPIRIT_DEBUG_NODE(rel_segment);
        BOOST_SPIRIT_DEBUG_NODE(scheme);
        BOOST_SPIRIT_DEBUG_NODE(authority);
        BOOST_SPIRIT_DEBUG_NODE(reg_name);
        BOOST_SPIRIT_DEBUG_NODE(server);
        BOOST_SPIRIT_DEBUG_NODE(userinfo);
        BOOST_SPIRIT_DEBUG_NODE(hostport);
        BOOST_SPIRIT_DEBUG_NODE(host);
        BOOST_SPIRIT_DEBUG_NODE(hostname);
        BOOST_SPIRIT_DEBUG_NODE(domainlabel);
        BOOST_SPIRIT_DEBUG_NODE(toplabel);
        BOOST_SPIRIT_DEBUG_NODE(ipv4address);
        BOOST_SPIRIT_DEBUG_NODE(port);
        BOOST_SPIRIT_DEBUG_NODE(path_segments);
        BOOST_SPIRIT_DEBUG_NODE(segment);
        BOOST_SPIRIT_DEBUG_NODE(param);
        BOOST_SPIRIT_DEBUG_NODE(pchar);
        BOOST_SPIRIT_DEBUG_NODE(query);
        BOOST_SPIRIT_DEBUG_NODE(fragment);
        BOOST_SPIRIT_DEBUG_NODE(uric);
        BOOST_SPIRIT_DEBUG_NODE(reserved);
        BOOST_SPIRIT_DEBUG_NODE(unreserved);
        BOOST_SPIRIT_DEBUG_NODE(mark);
        BOOST_SPIRIT_DEBUG_NODE(escaped);

        uri & uri_ref = self.uri_ref;

        uri_reference
            =   !(absolute_uri | relative_uri) >> !('#' >> fragment)
            ;

        absolute_uri
            =   (scheme[
                    absolute_uri.scheme_begin = arg1,
                    absolute_uri.scheme_end = arg2
                ] >> ':')[
                    var(uri_ref.scheme_begin) = absolute_uri.scheme_begin,
                    var(uri_ref.scheme_end) = absolute_uri.scheme_end
                ] >> (hier_part | opaque_part)[
                    var(uri_ref.scheme_specific_part_begin) = arg1,
                    var(uri_ref.scheme_specific_part_end) = arg2
                ]
            ;

        relative_uri
            =   (net_path | abs_path | rel_path) >> !('?' >> query)
            ;

        hier_part
            =   (net_path | abs_path) >> !('?' >> query)
            ;

        opaque_part
            =   uric_no_slash >> *uric
            ;

        uric_no_slash
            =   unreserved
            |   escaped
            |   ';'
            |   '?'
            |   ':'
            |   '@'
            |   '&'
            |   '='
            |   '+'
            |   '$'
            |   ','
            ;

        net_path
            =   "//" >> authority >> !abs_path
            ;

        abs_path
            =   ('/' >> path_segments)[
                    var(uri_ref.path_begin) = arg1,
                    var(uri_ref.path_end) = arg2
                ]
            ;

        rel_path
            =   (rel_segment >> !abs_path)[
                    var(uri_ref.path_begin) = arg1,
                    var(uri_ref.path_end) = arg2
                ]
            ;

        rel_segment
            =  +(   unreserved
                |   escaped
                |   ';'
                |   '@'
                |   '&'
                |   '='
                |   '+'
                |   '$'
                |   ','
                )
            ;

        scheme
            =   (alpha_p >> *(alpha_p | digit_p | '+' | '-' | '.'))
            ;

        authority
            =   (server | reg_name)[
                    var(uri_ref.authority_begin) = arg1,
                    var(uri_ref.authority_end) = arg2
                ]
            ;

        reg_name
            =  +(   unreserved
                |   escaped
                |   '$'
                |   ','
                |   ';'
                |   ':'
                |   '@'
                |   '&'
                |   '='
                |   '+'
                )
            ;

        server
            =  !(
                    !(userinfo[
                        server.userinfo_begin = arg1,
                        server.userinfo_end = arg2
                    ] >> '@')[
                        var(uri_ref.userinfo_begin) = server.userinfo_begin,
                        var(uri_ref.userinfo_end) = server.userinfo_end
                    ]
                    >> hostport
                )
            ;

        userinfo
            =  *(   unreserved
                |   escaped
                |   ';'
                |   ':'
                |   '&'
                |   '='
                |   '+'
                |   '$'
                |   ','
                )
            ;

        hostport
            =   host >> !(':' >> port)
            ;

        host
            =   (hostname | ipv4address)[
                    var(uri_ref.host_begin) = arg1,
                    var(uri_ref.host_end) = arg2
                ]
            ;

        hostname
            =   *(domainlabel >> '.') >> toplabel >> !ch_p('.')
            ;

        domainlabel
            =   alnum_p >> *(*ch_p('-') >> alnum_p)
            ;

        toplabel
            =   alpha_p >> *(*ch_p('-') >> alnum_p)
            ;

        ipv4address
            =   +digit_p >> '.' >> +digit_p >> '.' >> +digit_p >> '.'
                >> +digit_p
            ;

        port
            =   (*digit_p)[
                    var(uri_ref.port_begin) = arg1,
                    var(uri_ref.port_end) = arg2
                ]
            ;

        path_segments
            =   segment >> *('/' >> segment)
            ;

        segment
            =   *pchar >> *(';' >> param)
            ;

        param
            =   *pchar
            ;

        pchar
            =   unreserved
            |   escaped
            |   ':'
            |   '@'
            |   '&'
            |   '='
            |   '+'
            |   '$'
            |   ','
            ;

        query
            =   (*uric)[
                    var(uri_ref.query_begin) = arg1,
                    var(uri_ref.query_end) = arg2
                ]
            ;

        fragment
            =   (*uric)[
                    var(uri_ref.fragment_begin) = arg1,
                    var(uri_ref.fragment_end) = arg2
                ]
            ;

        uric
            =   reserved
            |   unreserved
            |   escaped
            ;

        reserved
            =   ch_p(';')
            |   '/'
            |   '?'
            |   ':'
            |   '@'
            |   '&'
            |   '='
            |   '+'
            |   '$'
            |   ','
            ;

        unreserved
            =   alnum_p
            |   mark
            ;

        mark
            =   ch_p('-')
            |   '_'
            |   '.'
            |   '!'
            |   '~'
            |   '*'
            |   '\''
            |   '('
            |   ')'
            ;

        escaped
            =   '%' >> xdigit_p >> xdigit_p
            ;
    }

    template <typename ScannerT>
    const boost::spirit::rule<ScannerT> &
    uri::grammar::definition<ScannerT>::start() const
    {
        return uri_reference;
    }

    uri::uri() throw (std::bad_alloc)
    {}

    uri::uri(const std::string & str)
        throw (openvrml::invalid_url, std::bad_alloc):
        str_(str)
    {
        using std::string;
        using namespace boost::spirit;

        grammar g(*this);

        string::const_iterator begin = this->str_.begin();
        string::const_iterator end = this->str_.end();

        if (!parse(begin, end, g, space_p).full) {
            throw openvrml::invalid_url();
        }
    }

    uri::operator std::string() const throw (std::bad_alloc)
    {
        return this->str_;
    }

    const std::string uri::scheme() const throw (std::bad_alloc)
    {
        return std::string(this->scheme_begin, this->scheme_end);
    }

    const std::string uri::scheme_specific_part() const
        throw (std::bad_alloc)
    {
        return std::string(this->scheme_specific_part_begin,
                           this->scheme_specific_part_end);
    }

    const std::string uri::authority() const throw (std::bad_alloc)
    {
        return std::string(this->authority_begin, this->authority_end);
    }

    const std::string uri::userinfo() const throw (std::bad_alloc)
    {
        return std::string(this->userinfo_begin, this->userinfo_end);
    }

    const std::string uri::host() const throw (std::bad_alloc)
    {
        return std::string(this->host_begin, this->host_end);
    }

    const std::string uri::port() const throw (std::bad_alloc)
    {
        return std::string(this->port_begin, this->port_end);
    }

    const std::string uri::path() const throw (std::bad_alloc)
    {
        return std::string(this->path_begin, this->path_end);
    }

    const std::string uri::query() const throw (std::bad_alloc)
    {
        return std::string(this->query_begin, this->query_end);
    }

    const std::string uri::fragment() const throw (std::bad_alloc)
    {
        return std::string(this->fragment_begin, this->fragment_end);
    }

    const uri uri::resolve_against(const uri & absolute_uri) const
        throw (std::bad_alloc)
    {
        using std::list;
        using std::string;

        assert(this->scheme().empty());
        assert(!absolute_uri.scheme().empty());

        string result = absolute_uri.scheme() + ':';

        if (!this->authority().empty()) {
            return uri(result + this->scheme_specific_part());
        } else {
            result += "//" + absolute_uri.authority();
        }

        string path = absolute_uri.path();
        const string::size_type last_slash_index = path.find_last_of('/');

        //
        // Chop off the leading slash and the last path segment (typically a
        // file name).
        //
        path = path.substr(1, last_slash_index);

        //
        // Append the relative path.
        //
        path += this->path();

        //
        // Put the path segments in a list to process them.
        //
        list<string> path_segments;
        string::size_type slash_index = 0;
        string::size_type segment_start_index = 0;
        do {
            slash_index = path.find('/', segment_start_index);
            string segment = path.substr(segment_start_index,
                                         slash_index - segment_start_index);
            if (!segment.empty()) {
                path_segments.push_back(segment);
            }
            segment_start_index = slash_index + 1;
        } while (slash_index != string::npos);

        //
        // Remove any "." segments.
        //
        path_segments.remove(".");

        //
        // Remove any ".." segments along with the segment that precedes them.
        //
        const list<string>::iterator begin(path_segments.begin());
        list<string>::iterator pos;
        for (pos = begin; pos != path_segments.end(); ++pos) {
            if (pos != begin && *pos == "..") {
                --(pos = path_segments.erase(pos));
                --(pos = path_segments.erase(pos));
            }
        }

        //
        // Reconstruct the path.
        //
        path = string();
        for (pos = path_segments.begin(); pos != path_segments.end(); ++pos) {
            path += '/' + *pos;
        }

        //
        // End in a slash?
        //
        if (*(this->path().end() - 1) == '/') { path += '/'; }

        result += path;

        const string query = this->query();
        if (!query.empty()) { result += '?' + query; }

        const string fragment = this->fragment();
        if (!fragment.empty()) { result += '#' + fragment; }

        uri result_uri;
        try {
            result_uri = uri(result);
        } catch (openvrml::invalid_url &) {
            assert(false); // If we constructed a bad URI, something is wrong.
        }

        return result_uri;
    }
}

/**
 * @internal
 *
 * @brief Create a new instance of a plug-in.
 *
 * NPP_New is called after NPP_Initialize and provides the MIME type,
 * embedded or full-screen display mode, and, for embedded plug-ins,
 * information about HTML @c EMBED arguments.
 *
 * The plug-in's NPP pointer is valid until the instance is destroyed with
 * NPP_Destroy.
 *
 * If instance data was saved from a previous instance of the plug-in by
 * the NPP_Destroy function, it is returned in the saved parameter for the
 * current instance to use.
 *
 * All attributes in the @c EMBED tag (standard and private) are passed in
 * NPP_New in the argn and argv arrays. Gecko ignores any non-standard
 * attributes within an @c EMBED tag. This gives developers a chance to use
 * private attributes to communicate instance-specific options or other
 * information to the plug-in. Place private options at the end of the list
 * of standard attributes in the @c EMBED Tag.
 *
 * @param pluginType    Pointer to the MIME type for new plug-in instance.
 * @param instance      Contains instance-specific private data for the plug-in
 *                      and Gecko. The data is stored in @p instance->pdata.
 * @param mode          Display mode of the plug-in. Can be NP_EMBED or
 *                      NP_FULL.
 * @param argc          Number of HTML arguments in the @c EMBED tag for an
 *                      embedded plug-in; determines the number of attributes
 *                      in the @p argn and @p argv arrays.
 * @param argn          Array of attribute names passed to the plug-in from the
 *                      @c EMBED tag.
 * @param argv          Array of attribute values passed to the plug-in from
 *                      the @c EMBED tag.
 * @param saved         Pointer to data saved by NPP_Destroy for a previous
 *                      instance of this plug-in at the same URL. If non-null,
 *                      Gecko passes ownership of the NPSavedData object back
 *                      to the plug-in. The plug-in is responsible for freeing
 *                      the memory for the NPSavedData and the buffer it
 *                      contains.
 *
 * @see http://devedge.netscape.com/library/manuals/2002/plugin/1.0/npp_api8.html#999289
 */
NPError NPP_New(const NPMIMEType pluginType,
                const NPP instance,
                const uint16 mode,
                const int16 argc,
                char * argn[],
                char * argv[],
                NPSavedData * saved)
{
    if (!instance) { return NPERR_INVALID_INSTANCE_ERROR; }

    const char * url = 0;
    for (int16 i = 0; i < argc; ++i) {
        //
        // If the plug-in is loaded into the browser window (as opposed to
        // embedded in a Web page), the URI of the world is passed in the
        // "src" attribute.
        //
        static const std::string src("src");
        if (argn[i] == src) {
            url = argv[i];
        }

        //
        // We prefer the "data" attribute to the "src" attribute; so even
        // if we get a "src" attribute, keep looking for a "data" one.  If
        // we find a "data" attribute, then we break.
        //
        static const std::string data("data");
        if (argn[i] == data) {
            url = argv[i];
            break;
        }
    }

    nsresult rv;

    nsCOMPtr<nsIDOMWindow> domWindow;
    NPError error =
        NPN_GetValue(instance,
                     NPNVDOMWindow,
                     static_cast<nsIDOMWindow **>(getter_AddRefs(domWindow)));
    if (error != NPERR_NO_ERROR) { return error; }
    assert(domWindow);
    nsCOMPtr<nsIDOMWindowInternal> windowInternal =
        do_QueryInterface(domWindow);
    if (!windowInternal) { return NPERR_GENERIC_ERROR; }
    nsCOMPtr<nsIDOMLocation> location;
    rv = windowInternal->GetLocation(getter_AddRefs(location));
    if (NS_FAILED(rv)) { return NPERR_GENERIC_ERROR; }
    assert(location);
    nsAutoString href;
    rv = location->GetHref(href);
    if (NS_FAILED(rv)) { return NPERR_GENERIC_ERROR; }

    uri plugin_data_uri(url);
    bool relative = plugin_data_uri.scheme().empty();
    if (relative) {
        plugin_data_uri =
            plugin_data_uri
            .resolve_against(uri(NS_ConvertUTF16toUTF8(href).get()));
    }

    try {
        instance->pdata = new PluginInstance(plugin_data_uri);
    } catch (std::bad_alloc &) {
        return NPERR_OUT_OF_MEMORY_ERROR;
    }
    return NPERR_NO_ERROR;
}

/**
 * @internal
 *
 * @brief Release the instance data and resources associated with a plug-in.
 *
 * Gecko calls this function when a plug-in instance is deleted, typically
 * because the user has left the page containing the instance, closed the
 * window, or quit the browser. You should delete any private instance-specific
 * information stored in the plug-in's instance->pdata at this time.
 *
 * If this function is deleting the last instance of a plug-in, NPP_Shutdown
 * is subsequently called. Use NPP_Shutdown to delete any data allocated in
 * NPP_Initialize and intended to be shared by all instances of a plug-in.
 *
 * Use the optional save parameter if you want to save and reuse some state
 * or other information. Upon the user's return to the page, this information
 * is passed to the new plug-in instance when it is created with NPP_New.
 *
 * Avoid trying to save critical data with this function. Ownership of the
 * @a buf field of the NPSavedData structure passes from the plug-in to
 * Gecko when NPP_Destroy returns. The Communicator can and will discard this
 * data based on arbitrary criteria such as its size and the user's page
 * history.
 *
 * To ensure that Gecko does not crash or leak memory when the saved data is
 * discarded, NPSavedData's @a buf field should be a flat structure (a simple
 * structure with no allocated substructures) allocated with NPN_MemAlloc.
 *
 * @param instance  Pointer to the plug-in instance to delete.
 * @param save      State or other information to save for reuse by a new
 *                  instance of this plug-in at the same URL. Passed to
 *                  NPP_New.
 *
 * @return @c NPERR_NO_ERROR if successful; @c NPERR_INVALID_INSTANCE_ERROR if
 *      @p instance is 0.
 *
 * @see http://developer.netscape.com/docs/manuals/communicator/plugin/pgfns.htm#1006838
 */
NPError NPP_Destroy(const NPP instance, NPSavedData * * const save)
{
    if (!instance) { return NPERR_INVALID_INSTANCE_ERROR; }

    /* PLUGIN DEVELOPERS:
     *    If desired, call NP_MemAlloc to create a
     *    NPSavedDate structure containing any state information
     *    that you want restored if this plugin instance is later
     *    recreated.
     */

    delete static_cast<PluginInstance *>(instance->pdata);
    instance->pdata = 0;

    return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(const NPP instance, NPWindow * const window)
{
    if (!instance || !instance->pdata) { return NPERR_INVALID_INSTANCE_ERROR; }
    try {
        assert(window);
        static_cast<PluginInstance *>(instance->pdata)
            ->SetWindow(*window);
    } catch (std::bad_alloc &) {
        return NPERR_OUT_OF_MEMORY_ERROR;
    }
    return NPERR_NO_ERROR;
}

NPError NPP_NewStream(const NPP instance,
                      const NPMIMEType type,
                      NPStream * const stream,
                      const NPBool seekable,
                      uint16 * const stype)
{
    if (!instance) { return NPERR_INVALID_INSTANCE_ERROR; }
    *stype = NP_ASFILEONLY;
    return NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(const NPP instance,
                          NPStream * const stream,
                          const NPError reason)
{
    if (!instance || !instance->pdata) { return NPERR_INVALID_INSTANCE_ERROR; }

    PluginInstance * const pluginInstance =
        static_cast<PluginInstance *>(instance->pdata);

    return NPERR_NO_ERROR;
}

/* PLUGIN DEVELOPERS:
 *    These next 2 functions are directly relevant in a plug-in which
 *    handles the data in a streaming manner. If you want zero bytes
 *    because no buffer space is YET available, return 0. As long as
 *    the stream has not been written to the plugin, Navigator will
 *    continue trying to send bytes.  If the plugin doesn't want them,
 *    just return some large number from NPP_WriteReady(), and
 *    ignore them in NPP_Write().  For a NP_ASFILE stream, they are
 *    still called but can safely be ignored using this strategy.
 */

namespace {
    int32 STREAMBUFSIZE = 0X0FFFFFFF; /* If we are reading from a file in
                                       * NPAsFile mode so we can take any size
                                       * stream in our write call (since we
                                       * ignore it) */
}

int32 NPP_WriteReady(const NPP instance, NPStream * const stream)
{
    if (instance) {
        PluginInstance * pluginInstance =
            static_cast<PluginInstance *>(instance->pdata);
    }

    return STREAMBUFSIZE;
}


int32 NPP_Write(const NPP instance,
                NPStream * const stream,
                const int32 offset,
                const int32 len,
                void * const buffer)
{
    if (instance) {
        PluginInstance * pluginInstance =
            static_cast<PluginInstance *>(instance->pdata);
    }

    return len; /* The number of bytes accepted */
}

void NPP_StreamAsFile(const NPP instance,
                      NPStream * const stream,
                      const char * const fname)
{
    assert(instance);
}

void NPP_Print(const NPP instance, NPPrint * const printInfo)
{
    if (!printInfo) { return; }

    if (instance) {
        PluginInstance * pluginInstance =
                static_cast<PluginInstance *>(instance->pdata);

        if (printInfo->mode == NP_FULL) {
            /*
             * PLUGIN DEVELOPERS:
             *    If your plugin would like to take over
             *    printing completely when it is in full-screen mode,
             *    set printInfo->pluginPrinted to TRUE and print your
             *    plugin as you see fit.  If your plugin wants Netscape
             *    to handle printing in this case, set
             *    printInfo->pluginPrinted to FALSE (the default) and
             *    do nothing.  If you do want to handle printing
             *    yourself, printOne is true if the print button
             *    (as opposed to the print menu) was clicked.
             *    On the Macintosh, platformPrint is a THPrint; on
             *    Windows, platformPrint is a structure
             *    (defined in npapi.h) containing the printer name, port,
             *    etc.
             */

            void * platformPrint = printInfo->print.fullPrint.platformPrint;
            NPBool printOne = printInfo->print.fullPrint.printOne;

            /* Do the default*/
            printInfo->print.fullPrint.pluginPrinted = false;
        }
        else {    /* If not fullscreen, we must be embedded */
            /*
             * PLUGIN DEVELOPERS:
             *    If your plugin is embedded, or is full-screen
             *    but you returned false in pluginPrinted above, NPP_Print
             *    will be called with mode == NP_EMBED.  The NPWindow
             *    in the printInfo gives the location and dimensions of
             *    the embedded plugin on the printed page.  On the
             *    Macintosh, platformPrint is the printer port; on
             *    Windows, platformPrint is the handle to the printing
             *    device context.
             */

            NPWindow * printWindow = &printInfo->print.embedPrint.window;
            void * platformPrint = printInfo->print.embedPrint.platformPrint;
        }
    }
}

int16 NPP_HandleEvent(const NPP instance, void * const event)
{
    assert(instance);
    assert(instance->pdata);
    try {
        PluginInstance * pluginInstance =
            static_cast<PluginInstance *>(instance->pdata);
    } catch (...) {
        return false;
    }
    return true;
}

void NPP_URLNotify(const NPP instance,
                   const char * const url,
                   const NPReason reason,
                   void * const notifyData)
{
}

jref NPP_GetJavaClass()
{
    return 0;
}

NPError NPP_GetValue(const NPP instance,
                     const NPPVariable variable,
                     void * const value)
{
    if (!instance) { return NPERR_INVALID_INSTANCE_ERROR; }

    NPError err = NPERR_NO_ERROR;
    static const nsIID scriptableIID = VRMLBROWSER_IID;
    nsISupports * scriptablePeer = 0;
    nsIID * scriptableIID_ptr = 0;
    PluginInstance * pluginInstance = 0;

    switch (variable) {
    case NPPVpluginScriptableInstance:
        assert(instance->pdata);
        pluginInstance =
            static_cast<PluginInstance *>(instance->pdata);
        scriptablePeer = pluginInstance->GetScriptablePeer();
        assert(scriptablePeer);
        //
        // Add reference for the caller requesting the object.
        //
        NS_ADDREF(scriptablePeer);
        *static_cast<nsISupports **>(value) = scriptablePeer;
	break;
    case NPPVpluginScriptableIID:
        try {
            scriptableIID_ptr =
                static_cast<nsIID *>(NPN_MemAlloc(sizeof (nsIID)));
            if (!scriptableIID_ptr) { throw std::bad_alloc(); }
            *scriptableIID_ptr = scriptableIID;
            *static_cast<nsIID **>(value) = scriptableIID_ptr;
        } catch (std::bad_alloc &) {
            err = NPERR_OUT_OF_MEMORY_ERROR;
        }
        break;
    case NPPVpluginNeedsXEmbed:
        *static_cast<PRBool *>(value) = PR_TRUE;
        break;
    default:
        err = NP_GetValue(instance, variable, value);
    }
    return err;
}

NPError NPP_SetValue(const NPP instance,
                     const NPNVariable variable,
                     void * const value)
{
    return NPERR_NO_ERROR;
}

void NPN_Version(int * plugin_major,
                 int * plugin_minor,
                 int * mozilla_major,
                 int * mozilla_minor)
{
    *plugin_major = NP_VERSION_MAJOR;
    *plugin_minor = NP_VERSION_MINOR;
    *mozilla_major = mozillaFuncs.version >> 8;
    *mozilla_minor = mozillaFuncs.version & 0xff;
}

NPError NPN_GetURLNotify(NPP instance,
                         const char * url,
                         const char * target,
                         void * notifyData)
{
    const int navMinorVers = mozillaFuncs.version & 0xFF;
    return (navMinorVers >= NPVERS_HAS_NOTIFICATION)
        ? CallNPN_GetURLNotifyProc(mozillaFuncs.geturlnotify,
                                   instance,
                                   url,
                                   target,
                                   notifyData)
        : NPERR_INCOMPATIBLE_VERSION_ERROR;
}

NPError NPN_GetURL(NPP instance, const char * url, const char * target)
{
    return CallNPN_GetURLProc(mozillaFuncs.geturl, instance, url, target);
}

NPError NPN_PostURLNotify(NPP instance,
                          const char * url,
                          const char * window,
                          uint32 len,
                          const char * buf,
                          NPBool file,
                          void * notifyData)
{
    const int navMinorVers = mozillaFuncs.version & 0xFF;
    return (navMinorVers >= NPVERS_HAS_NOTIFICATION)
        ? CallNPN_PostURLNotifyProc(mozillaFuncs.posturlnotify,
                                    instance,
                                    url,
                                    window,
                                    len,
                                    buf,
                                    file,
                                    notifyData)
        : NPERR_INCOMPATIBLE_VERSION_ERROR;
}

NPError NPN_PostURL(NPP instance,
                    const char * url,
                    const char * window,
                    uint32 len,
                    const char * buf,
                    NPBool file)
{
    return CallNPN_PostURLProc(mozillaFuncs.posturl,
                               instance,
                               url,
                               window,
                               len,
                               buf,
                               file);
}

NPError NPN_RequestRead(NPStream * stream, NPByteRange * rangeList)
{
    return CallNPN_RequestReadProc(mozillaFuncs.requestread,
                                   stream,
                                   rangeList);
}

NPError NPN_NewStream(NPP instance,
                      NPMIMEType type,
                      const char * target,
                      NPStream ** stream)
{
    const int navMinorVersion = mozillaFuncs.version & 0xFF;
    return (navMinorVersion >= NPVERS_HAS_STREAMOUTPUT)
        ? CallNPN_NewStreamProc(mozillaFuncs.newstream,
                                instance,
                                type,
                                target,
                                stream)
        : NPERR_INCOMPATIBLE_VERSION_ERROR;
}

int32 NPN_Write(NPP instance, NPStream * stream, int32 len, void * buffer)
{
    const int navMinorVersion = mozillaFuncs.version & 0xFF;
    return (navMinorVersion >= NPVERS_HAS_STREAMOUTPUT)
        ? CallNPN_WriteProc(mozillaFuncs.write, instance, stream, len, buffer)
        : -1;
}

NPError NPN_DestroyStream(NPP instance, NPStream* stream, NPError reason)
{
    const int navMinorVersion = mozillaFuncs.version & 0xFF;
    return (navMinorVersion >= NPVERS_HAS_STREAMOUTPUT)
        ? CallNPN_DestroyStreamProc(mozillaFuncs.destroystream,
                                    instance,
                                    stream,
                                    reason)
        : NPERR_INCOMPATIBLE_VERSION_ERROR;
}

void NPN_Status(NPP instance, const char * message)
{
    CallNPN_StatusProc(mozillaFuncs.status, instance, message);
}

const char * NPN_UserAgent(NPP instance)
{
    return CallNPN_UserAgentProc(mozillaFuncs.uagent, instance);
}

void * NPN_MemAlloc(uint32 size)
{
    return CallNPN_MemAllocProc(mozillaFuncs.memalloc, size);
}

void NPN_MemFree(void * ptr)
{
    CallNPN_MemFreeProc(mozillaFuncs.memfree, ptr);
}

uint32 NPN_MemFlush(uint32 size)
{
    return CallNPN_MemFlushProc(mozillaFuncs.memflush, size);
}

void NPN_ReloadPlugins(NPBool reloadPages)
{
    CallNPN_ReloadPluginsProc(mozillaFuncs.reloadplugins, reloadPages);
}

JRIEnv * NPN_GetJavaEnv()
{
    return CallNPN_GetJavaEnvProc(mozillaFuncs.getJavaEnv);
}

jref NPN_GetJavaPeer(NPP instance)
{
    return CallNPN_GetJavaPeerProc(mozillaFuncs.getJavaPeer, instance);
}

NPError NPN_GetValue(NPP instance, NPNVariable variable, void * value)
{
    return CallNPN_GetValueProc(mozillaFuncs.getvalue,
                                instance,
                                variable,
                                value);
}

NPError NPN_SetValue(NPP instance, NPPVariable variable, void * value)
{
    return CallNPN_SetValueProc(mozillaFuncs.setvalue,
                                instance,
                                variable,
                                value);
}

void NPN_InvalidateRect(NPP instance, NPRect * invalidRect)
{
    CallNPN_InvalidateRectProc(mozillaFuncs.invalidaterect,
                               instance,
                               invalidRect);
}

void NPN_InvalidateRegion(NPP instance, NPRegion invalidRegion)
{
    CallNPN_InvalidateRegionProc(mozillaFuncs.invalidateregion,
                                 instance,
                                 invalidRegion);
}

void NPN_ForceRedraw(NPP instance)
{
    CallNPN_ForceRedrawProc(mozillaFuncs.forceredraw, instance);
}

namespace {

    ScriptablePeer::ScriptablePeer(PluginInstance & pluginInstance):
	pluginInstance(pluginInstance)
    {
        NS_INIT_ISUPPORTS();
    }

    ScriptablePeer::~ScriptablePeer()
    {}

    NS_IMPL_ISUPPORTS2(ScriptablePeer, nsIClassInfo, VrmlBrowser)

    ///////////////////////////////////////////////////////////////////////////
    //
    // nsIClassInfo implementation
    //

    NS_IMETHODIMP ScriptablePeer::GetFlags(PRUint32 * aFlags)
    {
        *aFlags = nsIClassInfo::PLUGIN_OBJECT | nsIClassInfo::DOM_OBJECT;
        return NS_OK;
    }

    NS_IMETHODIMP
    ScriptablePeer::
    GetImplementationLanguage(PRUint32 * aImplementationLanguage)
    {
        *aImplementationLanguage = nsIProgrammingLanguage::CPLUSPLUS;
        return NS_OK;
    }

    NS_IMETHODIMP ScriptablePeer::GetInterfaces(PRUint32 * count,
                                                nsIID *** array)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetHelperForLanguage(PRUint32 language,
                                                       nsISupports ** _retval)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetContractID(char ** aContractID)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP
    ScriptablePeer::GetClassDescription(char ** aClassDescription)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetClassID(nsCID ** aClassID)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetClassIDNoAlloc(nsCID * aClassIDNoAlloc)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    ///////////////////////////////////////////////////////////////////////////
    //
    // VrmlBrowser implementation
    //

    NS_IMETHODIMP ScriptablePeer::GetName(char ** _retval)
    {
	if (!_retval) { return NS_ERROR_NULL_POINTER; }

	const std::string & name = this->pluginInstance.GetBrowser().name();
	const size_t bufferSize = sizeof (char) * (name.length() + 1);

	*_retval = static_cast<char *>(nsMemory::Clone(name.c_str(),
                                                       bufferSize));
        if (!*_retval) { return NS_ERROR_OUT_OF_MEMORY; }

        return NS_OK;
    }

    NS_IMETHODIMP ScriptablePeer::GetVersion(char ** _retval)
    {
	if (!_retval) { return NS_ERROR_NULL_POINTER; }

	const std::string & version =
            this->pluginInstance.GetBrowser().version();
	const size_t bufferSize = sizeof (char) * (version.length() + 1);

	*_retval = static_cast<char *>(nsMemory::Clone(version.c_str(),
                                                       bufferSize));
        if (!*_retval) { return NS_ERROR_OUT_OF_MEMORY; }

        return NS_OK;
    }

    NS_IMETHODIMP ScriptablePeer::GetCurrentSpeed(float * _retval)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetCurrentFrameRate(float * _retval)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetWorldURL(char ** _retval)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::ReplaceWorld(PRUint32 nodeArraySize,
                                               VrmlNode ** nodeArray)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::LoadURL(PRUint32 urlArraySize,
                                          const char ** url,
                                          PRUint32 paramArraySize,
                                          const char ** parameter)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::SetDescription(const char * description)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP
    ScriptablePeer::CreateVrmlFromString(const char * vrmlSyntax,
                                         PRUint32 * nodeArraySize,
                                         VrmlNode *** nodeArray)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::CreateVrmlFromURL(PRUint32 urlArraySize,
                                                    const char ** url,
                                                    VrmlNode * node,
                                                    const char * event)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetNode(const char * name,
                                          VrmlNode ** _retval)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::AddRoute(VrmlNode * fromNode,
                                           const char * fromEventOut,
                                           VrmlNode * toNode,
                                           const char * toEventIn)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::DeleteRoute(VrmlNode * fromNode,
                                              const char * fromEventOut,
                                              VrmlNode * toNode,
                                              const char * toEvent)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::BeginUpdate()
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::EndUpdate()
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::Dispose()
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP
    ScriptablePeer::AddBrowserListener(VrmlBrowserListener * listener)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP
    ScriptablePeer::RemoveBrowserListener(VrmlBrowserListener * listener)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }


    PluginInstance::PluginInstance(const std::string & initialURL)
        throw (std::bad_alloc):
        initialURL(initialURL),
        plug(0),
        drawingArea(0),
        x(0),
        y(0),
        width(0),
        height(0),
        viewer(*this),
        browser(std::cout, std::cerr),
	scriptablePeer(new ScriptablePeer(*this))
    {
        this->browser.viewer(&this->viewer);
    }

    PluginInstance::~PluginInstance() throw ()
    {
        gtk_container_remove(GTK_CONTAINER(this->plug), this->drawingArea);
    }

    nsISupports * PluginInstance::GetScriptablePeer() throw ()
    {
	return this->scriptablePeer;
    }

    openvrml::browser & PluginInstance::GetBrowser() throw ()
    {
        //
        // This is lame.  gtk_vrml_browser::browser is an implementation
        // detail.  It really needs to be exposed; but that would require
        // a C++ type in what ought to be a C interface.  We probably need
        // simply to punt on the idea of gtk_vrml_browser ever being usable
        // from C.
        //
        return *static_cast<openvrml::browser *>(0);
    }

    void PluginInstance::SetWindow(NPWindow & window)
        throw (std::bad_alloc)
    {
        assert(window.window);
        if (this->plug) {
            //
            // The plug-in window is unchanged. Resize the window and exit.
            //
            gtk_widget_set_size_request(this->drawingArea,
                                        window.width,
                                        window.height);
        } else {
            using std::vector;
            using std::string;

            this->plug =
                gtk_plug_new(reinterpret_cast<GdkNativeWindow>(window.window));
            if (!this->plug) { throw std::bad_alloc(); }

            this->drawingArea = gtk_drawing_area_new();
            if (!this->drawingArea) { throw std::bad_alloc(); }

            assert(::glConfig);

            static GdkGLContext * const shareList = 0;
            static const gboolean direct = false;
            static const int renderType = GDK_GL_RGBA_TYPE;
            gtk_widget_set_gl_capability(this->drawingArea,
                                         ::glConfig,
                                         shareList,
                                         direct,
                                         renderType);

            gtk_widget_add_events(this->drawingArea,
                                  GDK_EXPOSURE_MASK
                                  | GDK_BUTTON_PRESS_MASK
                                  | GDK_BUTTON_RELEASE_MASK
                                  | GDK_KEY_PRESS_MASK
                                  | GDK_POINTER_MOTION_MASK);

            g_signal_connect(G_OBJECT(this->drawingArea),
                             "expose_event",
                             G_CALLBACK(expose_event),
                             &this->viewer);
            g_signal_connect(G_OBJECT(this->drawingArea),
                             "configure_event",
                             G_CALLBACK(configure_event),
                             &this->viewer);
            g_signal_connect(G_OBJECT(this->drawingArea),
                             "key_press_event",
                             G_CALLBACK(key_press_event),
                             &this->viewer);
            g_signal_connect(G_OBJECT(this->drawingArea),
                             "button_press_event",
                             G_CALLBACK(button_press_event),
                             &this->viewer);
            g_signal_connect(G_OBJECT(this->drawingArea),
                             "button_release_event",
                             G_CALLBACK(button_release_event),
                             &this->viewer);
            g_signal_connect(G_OBJECT(this->drawingArea),
                             "motion_notify_event",
                             G_CALLBACK(motion_notify_event),
                             &this->viewer);

            gtk_container_add(GTK_CONTAINER(this->plug), this->drawingArea);

            gtk_widget_show_all(this->plug);

            const vector<string> uri(1, this->initialURL), parameter;
            this->browser.load_url(uri, parameter);
            this->viewer.timer_update();
        }
    }

    void PluginInstance::HandleEvent(void * event) throw ()
    {}

    GtkGLViewer::GtkGLViewer(PluginInstance & plugin_instance):
        plugin_instance(plugin_instance),
        timer(0),
        redrawNeeded(false)
    {}

    GtkGLViewer::~GtkGLViewer() throw ()
    {
        if (this->timer) { g_source_remove(timer); }
    }

    void GtkGLViewer::post_redraw()
    {
        if (!this->redrawNeeded) {
            this->redrawNeeded = true;
            gtk_widget_queue_draw(
                GTK_WIDGET(this->plugin_instance.drawingArea));
        }
    }

    void GtkGLViewer::set_cursor(cursor_style style)
    {
        GdkCursor * cursor(0);
        GtkWidget * const widget =
            GTK_WIDGET(this->plugin_instance.drawingArea);

        switch(style) {
        case cursor_inherit:
            XDefineCursor(GDK_WINDOW_XDISPLAY(widget->window),
                          GDK_WINDOW_XWINDOW(widget->window),
                          None);
            return;

        case cursor_info:
            cursor = gdk_cursor_new(GDK_HAND1);
            break;

        case cursor_cycle:
            cursor = gdk_cursor_new(GDK_EXCHANGE);
            break;

        case cursor_up_down:
            cursor = gdk_cursor_new(GDK_SB_V_DOUBLE_ARROW);
            break;

        case cursor_crosshair:
            cursor = gdk_cursor_new(GDK_CROSSHAIR);
            break;

        default:
            cursor = gdk_cursor_new(GDK_ARROW);
        }

        gdk_window_set_cursor(widget->window, cursor);
        gdk_cursor_destroy(cursor);
    }

    void GtkGLViewer::swap_buffers()
    {
        GtkWidget * const widget =
            GTK_WIDGET(this->plugin_instance.drawingArea);
        GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
        gdk_gl_drawable_swap_buffers(gl_drawable);
    }

    gint timeout_callback(const gpointer ptr)
    {
        assert(ptr);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(ptr);
        viewer.timer_update();
        return false;
    }

    void GtkGLViewer::set_timer(const double t)
    {
        if (!this->timer) {
            this->timer = g_timeout_add(guint(10.0 * (t + 1)),
                                        GtkFunction(timeout_callback),
                                        this);
        }
    }

    void GtkGLViewer::timer_update()
    {
        this->timer = 0;
        this->viewer::update();
    }
} // namespace

gboolean realize(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    
}

gboolean expose_event(GtkWidget * const widget,
                      GdkEventExpose * const event,
                      const gpointer data)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    g_assert(data);
    GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
    if (event->count == 0
        && gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        viewer.redraw();
    }
    viewer.redrawNeeded = false;
    return true;
}

gboolean configure_event(GtkWidget * const widget,
                         GdkEventConfigure * const event,
                         const gpointer data)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(data);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
        viewer.resize(widget->allocation.width, widget->allocation.height);
    }
    return true;
}

gboolean key_press_event(GtkWidget * const widget,
                         GdkEventKey * const event,
                         const gpointer data)
{
    using openvrml::gl::viewer;

    viewer::event_info info;
    info.event = viewer::event_key_down;

    switch (event->keyval) {
    case XK_Home:
        info.what = viewer::key_home;
        break;

    case XK_Left:
        info.what = viewer::key_left;
        break;

    case XK_Up:
        info.what = viewer::key_up;
        break;

    case XK_Right:
        info.what = viewer::key_right;
        break;

    case XK_Down:
        info.what = viewer::key_down;
        break;

    case XK_Page_Up:
        info.what = viewer::key_page_up;
        break;

    case XK_Page_Down:
        info.what = viewer::key_page_down;
        break;

    default:
        if (event->length <= 0) { return true; }
        info.what = event->string[0];
    }
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(data);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
        viewer.input(&info);
    }
    return true;
}

gboolean button_press_event(GtkWidget * const widget,
                            GdkEventButton * const event,
                            const gpointer data)
{
    using openvrml::gl::viewer;

    viewer::event_info info;
    info.event = viewer::event_mouse_click;

    switch (event->button) {
    case Button1:
        info.what = 0;
        break;

    case Button2:
        info.what = 1;
        break;

    case Button3:
        info.what = 2;
        break;
    }

    info.x = int(event->x);
    info.y = int(event->y);
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(data);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
        viewer.input(&info);
    }
    return true;
}

gboolean button_release_event(GtkWidget * const widget,
                              GdkEventButton * const event,
                              const gpointer data)
{
    using openvrml::gl::viewer;

    viewer::event_info info;
    info.event = viewer::event_mouse_release;

    switch (event->button) {
    case Button1:
        info.what = 0;
        break;

    case Button2:
        info.what = 1;
        break;

    case Button3:
        info.what = 2;
        break;
    }

    info.x = int(event->x);
    info.y = int(event->y);
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(data);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
        viewer.input(&info);
    }
    return true;
}

gboolean motion_notify_event(GtkWidget * const widget,
                             GdkEventMotion * const event,
                             const gpointer data)
{
    using openvrml::gl::viewer;

    viewer::event_info info;
    info.event = viewer::event_mouse_drag;
    info.what = 0;
    if (event->state & Button1Mask) {
        info.what = 0;
    } else if (event->state & Button2Mask) {
        info.what = 1;
    } else if (event->state & Button3Mask) {
        info.what = 2;
    } else {
        info.event = viewer::event_mouse_move;
    }

    info.x = int(event->x);
    info.y = int(event->y);
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(data);
        GtkGLViewer & viewer = *static_cast<GtkGLViewer *>(data);
        viewer.input(&info);
    }
    return true;
}
