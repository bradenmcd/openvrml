// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML Mozilla plug-in
//
// Copyright 2004, 2005, 2006, 2007, 2008  Braden McDaniel
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; if not, see <http://www.gnu.org/licenses/>.
//

# include <map>
# include <memory>
# include <sstream>
# include <stdexcept>
# include <vector>
# include <sys/socket.h>
# include <sys/wait.h>
# include <boost/concept_check.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/noncopyable.hpp>
# include <boost/ref.hpp>
# include <boost/scoped_ptr.hpp>
# include <npfunctions.h>
# if defined MOZ_X11
#   include <dbus/dbus-glib-bindings.h>
# else
#   error Unsupported toolkit.
# endif

# include <config.h>

# define OPENVRML_NP_TYPE_BROWSER_HOST (openvrml_np_browser_host_get_type())
# define OPENVRML_NP_BROWSER_HOST(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), OPENVRML_NP_TYPE_BROWSER_HOST, OpenvrmlNpBrowserHost))
# define OPENVRML_NP_BROWSER_HOST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), OPENVRML_NP_TYPE_BROWSER_HOST, OpenvrmlNpBrowserHostClass))

G_BEGIN_DECLS

typedef struct OpenvrmlNpBrowserHost_ OpenvrmlNpBrowserHost;
typedef struct OpenvrmlNpBrowserHostClass_ OpenvrmlNpBrowserHostClass;

struct OpenvrmlNpBrowserHost_ {
    GObject parent;
    gchar * path;
    NPP npp;
};

struct OpenvrmlNpBrowserHostClass_ {
    GObjectClass parent;
    DBusGConnection * connection;
    gchar * host_name;
};

GType openvrml_np_browser_host_get_type();

int
openvrml_np_browser_host_get_url(OpenvrmlNpBrowserHost * host, const char * url,
                                 GError ** error);
void openvrml_np_browser_host_set_world_url(OpenvrmlNpBrowserHost * host,
                                            const char * url, GError ** error);

G_END_DECLS

# include "browser-host-server-glue.h"
# include "browser-factory-client-glue.h"
# include "browser-client-glue.h"

using namespace boost::multi_index::detail;  // for scope_guard

namespace {

    void printerr(const char * str);

    class plugin_instance;

    typedef bool (plugin_instance::*script_callback_t)(const NPVariant *,
                                                       uint32_t,
                                                       NPVariant *);

    //
    // Mozilla doesn't like us to use NPN_GetStringIdentifier during static
    // initialization; so this is a singleton.
    //
    class script_callback_map : public std::map<NPIdentifier,
                                                script_callback_t>,
                                boost::noncopyable {
        static boost::scoped_ptr<const script_callback_map> instance_;

        script_callback_map();

    public:
        static const script_callback_map & instance();
    };


    class plugin_instance : boost::noncopyable {
        const NPP npp;
        void * window;
        int x, y;
        int width, height;
        OpenvrmlNpBrowserHost * browser_host;
        DBusGProxy * browser;

    public:
        NPObject * const npobj;

        explicit plugin_instance(NPP npp) throw (std::bad_alloc);
        ~plugin_instance() throw ();

        void set_window(NPWindow & window)
            throw (std::bad_alloc, std::runtime_error);
        void HandleEvent(void * event) throw ();

        NPError new_stream(NPMIMEType type, NPStream * stream);
        NPError destroy_stream(NPStream * stream);
        int32_t write(NPStream * stream, int32_t len, void * buffer);

        //
        // Scripting API method implementations.
        //
        bool get_name(const NPVariant * args, uint32_t argCount,
                      NPVariant * result);
        bool get_version(const NPVariant * args, uint32_t argCount,
                         NPVariant * result);
    };
} // namespace

G_DEFINE_TYPE(OpenvrmlNpBrowserHost, openvrml_np_browser_host, G_TYPE_OBJECT)

void openvrml_np_browser_host_init(OpenvrmlNpBrowserHost * const host)
{
    static unsigned long count = 0;
    host->path = g_strdup_printf("/org/openvrml/BrowserHost/%u/%lu",
                                 getpid(), count++);
    dbus_g_connection_register_g_object(
        OPENVRML_NP_BROWSER_HOST_GET_CLASS(host)->connection,
        host->path,
        G_OBJECT(host));
}

namespace {
    enum np_host_signal_id {
        shutdown_id,
        last_signal_id
    };

    G_GNUC_INTERNAL guint signals[last_signal_id];
}

void openvrml_np_browser_host_class_init(OpenvrmlNpBrowserHostClass * klass)
{
    signals[shutdown_id] =
        g_signal_new("shutdown",
                     G_OBJECT_CLASS_TYPE(klass),
                     GSignalFlags(G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED),
                     0,
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, boost::ref(error));
    klass->connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
    if (!klass->connection) {
        g_critical("Failed to open connection to bus: %s", error->message);
        return;
    }

    klass->host_name = g_strdup_printf("org.openvrml.BrowserHost-%u",
                                       getpid());

    DBusGProxy * driver_proxy =
        dbus_g_proxy_new_for_name(klass->connection,
                                  DBUS_SERVICE_DBUS,
                                  DBUS_PATH_DBUS,
                                  DBUS_INTERFACE_DBUS);
    scope_guard driver_proxy_guard = make_guard(g_object_unref, driver_proxy);
    boost::ignore_unused_variable_warning(driver_proxy_guard);

    guint request_ret;
    if (!org_freedesktop_DBus_request_name(driver_proxy,
                                           klass->host_name,
                                           0, &request_ret,
                                           &error)) {
        g_critical("Request for name \"%s\" failed: %s",
                   klass->host_name, error->message);
        return;
    }

    dbus_g_object_type_install_info(
        OPENVRML_NP_TYPE_BROWSER_HOST,
        &dbus_glib_openvrml_np_browser_host_object_info);
    error_guard.dismiss();
}

int openvrml_np_browser_host_get_url(OpenvrmlNpBrowserHost * const host,
                                     const char * const url,
                                     GError ** /* error */)
{
    return NPN_GetURL(host->npp, url, 0);
}

void openvrml_np_browser_host_set_world_url(OpenvrmlNpBrowserHost * /* host */,
                                            const char * /* url */,
                                            GError ** /* error */)
{
}

char * NP_GetMIMEDescription()
{
    return NPP_GetMIMEDescription();
}

namespace {
    NPNetscapeFuncs mozillaFuncs;
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
    mozillaFuncs.version              = mozTable->version;
    mozillaFuncs.size                 = mozTable->size;
    mozillaFuncs.posturl              = mozTable->posturl;
    mozillaFuncs.geturl               = mozTable->geturl;
    mozillaFuncs.geturlnotify         = mozTable->geturlnotify;
    mozillaFuncs.requestread          = mozTable->requestread;
    mozillaFuncs.newstream            = mozTable->newstream;
    mozillaFuncs.write                = mozTable->write;
    mozillaFuncs.destroystream        = mozTable->destroystream;
    mozillaFuncs.status               = mozTable->status;
    mozillaFuncs.uagent               = mozTable->uagent;
    mozillaFuncs.memalloc             = mozTable->memalloc;
    mozillaFuncs.memfree              = mozTable->memfree;
    mozillaFuncs.memflush             = mozTable->memflush;
    mozillaFuncs.reloadplugins        = mozTable->reloadplugins;
    mozillaFuncs.getJavaEnv           = mozTable->getJavaEnv;
    mozillaFuncs.getJavaPeer          = mozTable->getJavaPeer;
    mozillaFuncs.getvalue             = mozTable->getvalue;
    mozillaFuncs.getstringidentifier  = mozTable->getstringidentifier;
    mozillaFuncs.getstringidentifiers = mozTable->getstringidentifiers;
    mozillaFuncs.getintidentifier     = mozTable->getintidentifier;
    mozillaFuncs.identifierisstring   = mozTable->identifierisstring;
    mozillaFuncs.utf8fromidentifier   = mozTable->utf8fromidentifier;
    mozillaFuncs.intfromidentifier    = mozTable->intfromidentifier;
    mozillaFuncs.createobject         = mozTable->createobject;
    mozillaFuncs.retainobject         = mozTable->retainobject;
    mozillaFuncs.releaseobject        = mozTable->releaseobject;
    mozillaFuncs.invoke               = mozTable->invoke;
    mozillaFuncs.invokeDefault        = mozTable->invokeDefault;
    mozillaFuncs.evaluate             = mozTable->evaluate;
    mozillaFuncs.getproperty          = mozTable->getproperty;
    mozillaFuncs.setproperty          = mozTable->setproperty;
    mozillaFuncs.removeproperty       = mozTable->removeproperty;
    mozillaFuncs.hasproperty          = mozTable->hasproperty;
    mozillaFuncs.hasmethod            = mozTable->hasmethod;
    mozillaFuncs.releasevariantvalue  = mozTable->releasevariantvalue;
    mozillaFuncs.setexception         = mozTable->setexception;

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
    NPBool supportsXEmbed = false;
    err = NPN_GetValue(0, NPNVSupportsXEmbedBool, &supportsXEmbed);

    if (err != NPERR_NO_ERROR || !supportsXEmbed) {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }

    //
    // Make sure the browser tookit is Gtk2.
    //
    NPNToolkitType toolkit = NPNToolkitType();
    err = NPN_GetValue(0, NPNVToolkit, &toolkit);

    if (err != NPERR_NO_ERROR || toolkit != NPNVGtk2) {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
# endif // defined MOZ_X11

    return NPP_Initialize();
}

NPError NP_Shutdown()
{
    NPP_Shutdown();
    return NPERR_NO_ERROR;
}

NPError NP_GetValue(void *, NPPVariable variable, void * value)
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
    static const char mimeDescription[] =
        "model/x3d-vrml:x3dv:X3D world;"
        "model/vrml:wrl:VRML world;"
        "x-world/x-vrml:wrl:VRML world";
    return const_cast<char *>(&mimeDescription[0]);
}

NPError NPP_Initialize()
{
    return NPERR_NO_ERROR;
}

void NPP_Shutdown()
{}

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
NPError NPP_New(const NPMIMEType,
                const NPP instance,
                uint16_t /* mode */,
                int16_t /* argc */,
                char * /* argn */[],
                char * /* argv */[],
                NPSavedData *)
{
    if (!instance) { return NPERR_INVALID_INSTANCE_ERROR; }

    try {
        instance->pdata = new plugin_instance(instance);
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
NPError NPP_Destroy(const NPP instance, NPSavedData **)
{
    if (!instance) { return NPERR_INVALID_INSTANCE_ERROR; }

    /* PLUGIN DEVELOPERS:
     *    If desired, call NP_MemAlloc to create a
     *    NPSavedDate structure containing any state information
     *    that you want restored if this plugin instance is later
     *    recreated.
     */

    delete static_cast<plugin_instance *>(instance->pdata);
    instance->pdata = 0;

    return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(const NPP instance, NPWindow * const window)
{
    if (!instance || !instance->pdata) { return NPERR_INVALID_INSTANCE_ERROR; }
    try {
        assert(window);
        static_cast<plugin_instance *>(instance->pdata)
            ->set_window(*window);
    } catch (std::bad_alloc &) {
        return NPERR_OUT_OF_MEMORY_ERROR;
    } catch (std::runtime_error & ex) {
        g_critical(ex.what());
        return NPERR_GENERIC_ERROR;
    }
    return NPERR_NO_ERROR;
}

NPError NPP_NewStream(const NPP instance,
                      const NPMIMEType type,
                      NPStream * const stream,
                      NPBool /* seekable */,
                      uint16_t * const stype)
{
    if (!instance || !instance->pdata) { return NPERR_INVALID_INSTANCE_ERROR; }

    *stype = NP_NORMAL;

    plugin_instance & pluginInstance =
        *static_cast<plugin_instance *>(instance->pdata);

    return pluginInstance.new_stream(type, stream);
}

NPError NPP_DestroyStream(const NPP instance,
                          NPStream * const stream,
                          NPError /* reason */)
{
    if (!instance || !instance->pdata) { return NPERR_INVALID_INSTANCE_ERROR; }

    plugin_instance & pluginInstance =
        *static_cast<plugin_instance *>(instance->pdata);

    return pluginInstance.destroy_stream(stream);
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
    int32_t STREAMBUFSIZE = 0X0FFFFFFF; /* If we are reading from a file in
                                         * NPAsFile mode so we can take any
                                         * size stream in our write call
                                         * (since we ignore it) */
}

int32_t NPP_WriteReady(NPP, NPStream *)
{
    return STREAMBUFSIZE;
}


int32_t NPP_Write(const NPP instance,
                  NPStream * const stream,
                  int32_t /* offset */,
                  const int32_t len,
                  void * const buffer)
{
    if (!instance || !instance->pdata) { return 0; }

    plugin_instance & pluginInstance =
        *static_cast<plugin_instance *>(instance->pdata);

    return pluginInstance.write(stream, len, buffer);
}

void NPP_StreamAsFile(const NPP instance,
                      NPStream *,
                      const char * /* fname */)
{
    boost::ignore_unused_variable_warning(instance);
    assert(instance);
}

void NPP_Print(const NPP instance, NPPrint * const printInfo)
{
    if (!printInfo) { return; }

    if (instance) {
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

            // void * platformPrint = printInfo->print.fullPrint.platformPrint;
            // NPBool printOne = printInfo->print.fullPrint.printOne;

            /* Do the default*/
            // printInfo->print.fullPrint.pluginPrinted = false;
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

            // NPWindow * printWindow = &printInfo->print.embedPrint.window;
            // void * platformPrint = printInfo->print.embedPrint.platformPrint;
        }
    }
}

int16_t NPP_HandleEvent(NPP, void * /* event */)
{
    return true;
}

void NPP_URLNotify(NPP,
                   const char * /* url */,
                   NPReason,
                   void * /* notifyData */)
{}

NPError NPP_GetValue(const NPP npp,
                     const NPPVariable variable,
                     void * const value)
{
    if (!npp) { return NPERR_INVALID_INSTANCE_ERROR; }

    NPError err = NPERR_NO_ERROR;
    plugin_instance * instance = 0;

    switch (variable) {
    case NPPVpluginNeedsXEmbed:
        *static_cast<NPBool *>(value) = true;
        break;
    case NPPVpluginScriptableNPObject:
        assert(npp->pdata);
        instance = static_cast<plugin_instance *>(npp->pdata);
        NPN_RetainObject(instance->npobj);
        *static_cast<NPObject **>(value) = instance->npobj;
        break;
    default:
        err = NP_GetValue(instance, variable, value);
    }
    return err;
}

NPError NPP_SetValue(NPP, NPNVariable, void * /* value */)
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
        ? mozillaFuncs.geturlnotify(instance, url, target, notifyData)
        : NPERR_INCOMPATIBLE_VERSION_ERROR;
}

NPError NPN_GetURL(NPP instance, const char * url, const char * target)
{
    return mozillaFuncs.geturl(instance, url, target);
}

NPError NPN_PostURLNotify(NPP instance,
                          const char * url,
                          const char * window,
                          uint32_t len,
                          const char * buf,
                          NPBool file,
                          void * notifyData)
{
    const int navMinorVers = mozillaFuncs.version & 0xFF;
    return (navMinorVers >= NPVERS_HAS_NOTIFICATION)
        ? mozillaFuncs.posturlnotify(instance,
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
                    uint32_t len,
                    const char * buf,
                    NPBool file)
{
    return mozillaFuncs.posturl(instance, url, window, len, buf, file);
}

NPError NPN_RequestRead(NPStream * stream, NPByteRange * rangeList)
{
    return mozillaFuncs.requestread(stream, rangeList);
}

NPError NPN_NewStream(NPP instance,
                      NPMIMEType type,
                      const char * target,
                      NPStream ** stream)
{
    const int navMinorVersion = mozillaFuncs.version & 0xFF;
    return (navMinorVersion >= NPVERS_HAS_STREAMOUTPUT)
        ? mozillaFuncs.newstream(instance, type, target, stream)
        : NPERR_INCOMPATIBLE_VERSION_ERROR;
}

int32_t NPN_Write(NPP instance, NPStream * stream, int32_t len, void * buffer)
{
    const int navMinorVersion = mozillaFuncs.version & 0xFF;
    return (navMinorVersion >= NPVERS_HAS_STREAMOUTPUT)
        ? mozillaFuncs.write(instance, stream, len, buffer)
        : -1;
}

NPError NPN_DestroyStream(NPP instance, NPStream* stream, NPError reason)
{
    const int navMinorVersion = mozillaFuncs.version & 0xFF;
    return (navMinorVersion >= NPVERS_HAS_STREAMOUTPUT)
        ? mozillaFuncs.destroystream(instance, stream, reason)
        : NPERR_INCOMPATIBLE_VERSION_ERROR;
}

void NPN_Status(NPP instance, const char * message)
{
    mozillaFuncs.status(instance, message);
}

const char * NPN_UserAgent(NPP instance)
{
    return mozillaFuncs.uagent(instance);
}

void * NPN_MemAlloc(uint32_t size)
{
    return mozillaFuncs.memalloc(size);
}

void NPN_MemFree(void * ptr)
{
    mozillaFuncs.memfree(ptr);
}

uint32_t NPN_MemFlush(uint32_t size)
{
    return mozillaFuncs.memflush(size);
}

void NPN_ReloadPlugins(NPBool reloadPages)
{
    mozillaFuncs.reloadplugins(reloadPages);
}

NPError NPN_GetValue(NPP instance, NPNVariable variable, void * value)
{
    return mozillaFuncs.getvalue(instance, variable, value);
}

NPError NPN_SetValue(NPP instance, NPPVariable variable, void * value)
{
    return mozillaFuncs.setvalue(instance, variable, value);
}

void NPN_InvalidateRect(NPP instance, NPRect * invalidRect)
{
    mozillaFuncs.invalidaterect(instance, invalidRect);
}

void NPN_InvalidateRegion(NPP instance, NPRegion invalidRegion)
{
    mozillaFuncs.invalidateregion(instance, invalidRegion);
}

void NPN_ForceRedraw(NPP instance)
{
    mozillaFuncs.forceredraw(instance);
}

void NPN_ReleaseVariantValue(NPVariant * variant)
{
    mozillaFuncs.releasevariantvalue(variant);
}

NPIdentifier NPN_GetStringIdentifier(const NPUTF8 * name)
{
    return mozillaFuncs.getstringidentifier(name);
}

void NPN_GetStringIdentifiers(const NPUTF8 ** names,
                              int32_t nameCount,
                              NPIdentifier * identifiers)
{
    mozillaFuncs.getstringidentifiers(names, nameCount, identifiers);
}

NPIdentifier NPN_GetIntIdentifier(int32_t intid)
{
    return mozillaFuncs.getintidentifier(intid);
}

bool NPN_IdentifierIsString(NPIdentifier * identifier)
{
    return mozillaFuncs.identifierisstring(identifier);
}

NPUTF8 * NPN_UTF8FromIdentifier(NPIdentifier identifier)
{
    return mozillaFuncs.utf8fromidentifier(identifier);
}

int32_t NPN_IntFromIdentifier(NPIdentifier identifier)
{
    return mozillaFuncs.intfromidentifier(identifier);
}

NPObject * NPN_CreateObject(NPP npp, NPClass * aClass)
{
    return mozillaFuncs.createobject(npp, aClass);
}

NPObject * NPN_RetainObject(NPObject * npobj)
{
    return mozillaFuncs.retainobject(npobj);
}

void NPN_ReleaseObject(NPObject * npobj)
{
    mozillaFuncs.releaseobject(npobj);
}

bool NPN_Invoke(NPP npp, NPObject * npobj, NPIdentifier methodName,
                const NPVariant * args, uint32_t argCount, NPVariant * result)
{
    return mozillaFuncs.invoke(npp, npobj, methodName, args, argCount, result);
}

bool NPN_InvokeDefault(NPP npp, NPObject * npobj, const NPVariant * args,
                       uint32_t argCount, NPVariant * result)
{
    return mozillaFuncs.invokeDefault(npp, npobj, args, argCount, result);
}

bool NPN_Evaluate(NPP npp, NPObject * npobj, NPString * script,
                  NPVariant * result)
{
    return mozillaFuncs.evaluate(npp, npobj, script, result);
}

bool NPN_GetProperty(NPP npp, NPObject * npobj, NPIdentifier propertyName,
                     NPVariant * result)
{
    return mozillaFuncs.getproperty(npp, npobj, propertyName, result);
}

bool NPN_SetProperty(NPP npp, NPObject * npobj, NPIdentifier propertyName,
                     const NPVariant * result)
{
    return mozillaFuncs.setproperty(npp, npobj, propertyName, result);
}

bool NPN_RemoveProperty(NPP npp, NPObject * npobj, NPIdentifier propertyName)
{
    return mozillaFuncs.removeproperty(npp, npobj, propertyName);
}

bool NPN_HasProperty(NPP npp, NPObject * npobj, NPIdentifier propertyName)
{
    return mozillaFuncs.hasproperty(npp, npobj, propertyName);
}

bool NPN_HasMethod(NPP npp, NPObject * npobj, NPIdentifier methodName)
{
    return mozillaFuncs.hasmethod(npp, npobj, methodName);
}

void NPN_SetException(NPObject * npobj, const NPUTF8 * message)
{
    return mozillaFuncs.setexception(npobj, message);
}

namespace {

    boost::scoped_ptr<const script_callback_map> script_callback_map::instance_;

    script_callback_map::script_callback_map()
    {
        using std::make_pair;
        this->insert(make_pair(NPN_GetStringIdentifier("getName"),
                               &plugin_instance::get_name));
        this->insert(make_pair(NPN_GetStringIdentifier("getVersion"),
                               &plugin_instance::get_version));
    }

    const script_callback_map & script_callback_map::instance()
    {
        if (!script_callback_map::instance_) {
            script_callback_map::instance_.reset(new script_callback_map);
        }
        return *script_callback_map::instance_;
    }

    void printerr(const char * str)
    {
        fprintf(stderr, "%s\n", str);
    }


    struct OpenVRMLNPObject {
        NPObject npobj;
        NPP npp;
    };

    NPObject * openvrmlnpobject_allocate(NPP npp, NPClass *)
    {
        OpenVRMLNPObject * const npobj =
            static_cast<OpenVRMLNPObject *>(
                NPN_MemAlloc(sizeof (OpenVRMLNPObject)));

        npobj->npp = npp;

        return static_cast<NPObject *>(static_cast<void *>(npobj));
    }

    void openvrmlnpobject_deallocate(NPObject * const npobj)
    {
        NPN_MemFree(npobj);
    }

    void openvrmlnpobject_invalidate(NPObject * /* npobj */)
    {}

    bool openvrmlnpobject_hasMethod(NPObject *, const NPIdentifier name)
    {
        const script_callback_map::const_iterator pos =
            script_callback_map::instance().find(name);
        return pos != script_callback_map::instance().end();
    }

    bool openvrmlnpobject_invoke(NPObject * const npobj,
                                 const NPIdentifier name,
                                 const NPVariant * const args,
                                 const uint32_t argCount,
                                 NPVariant * const result)
    {
        const script_callback_map::const_iterator pos =
            script_callback_map::instance().find(name);
        assert(pos != script_callback_map::instance().end());

        OpenVRMLNPObject * const openvrml_npobj =
            static_cast<OpenVRMLNPObject *>(static_cast<void *>(npobj));

        plugin_instance * const instance =
            static_cast<plugin_instance *>(openvrml_npobj->npp->pdata);

        return (instance->*(pos->second))(args, argCount, result);
    }

    bool openvrmlnpobject_invokeDefault(NPObject * /* npobj */,
                                        const NPVariant * /* args */,
                                        uint32_t /* argCount */,
                                        NPVariant * /* result */)
    {
        return false;
    }

    bool openvrmlnpobject_hasProperty(NPObject * /* npobj */,
                                      NPIdentifier /* name */)
    {
        return false;
    }

    bool openvrmlnpobject_getProperty(NPObject * /* npobj */,
                                      NPIdentifier /* name */,
                                      NPVariant * /* result */)
    {
        return false;
    }

    bool openvrmlnpobject_setProperty(NPObject * /* npobj */,
                                      NPIdentifier /* name */,
                                      const NPVariant * /* value */)
    {
        return false;
    }

    bool openvrmlnpobject_removeProperty(NPObject * /* npobj */,
                                         NPIdentifier /* name */)
    {
        return false;
    }

    NPClass npclass = {
        NP_CLASS_STRUCT_VERSION,
        openvrmlnpobject_allocate,
        openvrmlnpobject_deallocate,
        openvrmlnpobject_invalidate,
        openvrmlnpobject_hasMethod,
        openvrmlnpobject_invoke,
        openvrmlnpobject_invokeDefault,
        openvrmlnpobject_hasProperty,
        openvrmlnpobject_getProperty,
        openvrmlnpobject_setProperty,
        openvrmlnpobject_removeProperty
    };

    plugin_instance::plugin_instance(const NPP npp) throw (std::bad_alloc):
        npp(npp),
        window(0),
        x(0),
        y(0),
        width(0),
        height(0),
        browser_host(
            OPENVRML_NP_BROWSER_HOST(
                g_object_new(OPENVRML_NP_TYPE_BROWSER_HOST, 0))),
        browser(0),
        npobj(NPN_CreateObject(this->npp, &npclass))
    {
        if (!this->browser_host) { throw std::bad_alloc(); }
        if (!this->npobj) { throw std::bad_alloc(); }

        browser_host->npp = npp;
    }

    plugin_instance::~plugin_instance() throw ()
    {
        NPN_ReleaseObject(this->npobj);
        g_signal_emit(this->browser_host, signals[shutdown_id], 0);
    }

    DBusGProxy * get_browser(DBusGConnection * const connection,
                             const char * const host_name,
                             const char * const host_path,
                             const guint64 host_id,
                             GError ** const error)
        throw ()
    {
        DBusGProxy * browser_factory =
            dbus_g_proxy_new_for_name(connection,
                                      "org.openvrml.BrowserControl",
                                      "/org/openvrml/BrowserFactory",
                                      "org.openvrml.BrowserFactory");
        g_return_val_if_fail(browser_factory, 0);
        scope_guard browser_factory_guard =
            make_guard(g_object_unref, G_OBJECT(browser_factory));
        boost::ignore_unused_variable_warning(browser_factory_guard);

        char * browser_path = 0;
        if (!org_openvrml_BrowserFactory_create_control(browser_factory,
                                                        host_name,
                                                        host_path,
                                                        host_id,
                                                        true,
                                                        &browser_path,
                                                        error)) {
            return 0;
        }

        DBusGProxy * browser =
            dbus_g_proxy_new_for_name(connection,
                                      "org.openvrml.BrowserControl",
                                      browser_path,
                                      "org.openvrml.Browser");
        return browser;
    }

    void plugin_instance::set_window(NPWindow & window)
        throw (std::bad_alloc, std::runtime_error)
    {
        assert(window.window);
        if (this->window) {
            //
            // The plug-in window is unchanged.  Resize the window and exit.
            //
            return;
        }

        using std::string;
        using std::vector;
        using boost::lexical_cast;
        using boost::ref;

        this->window = window.window;

        OpenvrmlNpBrowserHostClass * const browser_host_class =
            OPENVRML_NP_BROWSER_HOST_GET_CLASS(this->browser_host);

        GError * error = 0;
        scope_guard error_guard = make_guard(g_error_free, boost::ref(error));

        this->browser = get_browser(browser_host_class->connection,
                                    browser_host_class->host_name,
                                    this->browser_host->path,
                                    guint64(this->window),
                                    &error);
        if (!this->browser) {
            g_critical("Browser creation failed: %s", error->message);
            return;
        }

        error_guard.dismiss();
    }

    NPError plugin_instance::new_stream(const NPMIMEType type,
                                        NPStream * const stream)
    {
        if (!this->browser) { return NPERR_INVALID_INSTANCE_ERROR; }

        GError * error = 0;
        scope_guard error_guard = make_guard(g_error_free, boost::ref(error));
        gboolean result = org_openvrml_Browser_new_stream(this->browser,
                                                          guint64(stream),
                                                          type,
                                                          stream->url,
                                                          &error);
        if (!result) {
            g_critical("Call to org.openvrml.Browser.NewStream failed: %s",
                       error->message);
            return NPERR_GENERIC_ERROR;
        }
        error_guard.dismiss();
        return NPERR_NO_ERROR;
    }

    NPError plugin_instance::destroy_stream(NPStream * const stream)
    {
        if (!this->browser) { return NPERR_INVALID_INSTANCE_ERROR; }

        dbus_g_proxy_call_no_reply(this->browser,
                                   "DestroyStream",
                                   G_TYPE_UINT64, guint64(stream),
                                   G_TYPE_INVALID);
        return NPERR_NO_ERROR;
    }

    int32_t plugin_instance::write(NPStream * const stream,
                                   const int32_t len,
                                   void * const buffer)
    {
        if (!this->browser) { return 0; }

        GArray array = {};
        array.data = static_cast<char *>(buffer);
        array.len = len;

        dbus_g_proxy_call_no_reply(this->browser,
                                   "Write",
                                   G_TYPE_UINT64, guint64(stream),
                                   DBUS_TYPE_G_UCHAR_ARRAY, &array,
                                   G_TYPE_INVALID);
        return len;
    }

    void plugin_instance::HandleEvent(void *) throw ()
    {}

    bool plugin_instance::get_name(const NPVariant * const /* args */,
                                   const uint32_t /* argCount */,
                                   NPVariant * const result)
    {
        static const std::string name = PACKAGE_NAME;
        NPUTF8 * const name_str =
            static_cast<NPUTF8 *>(
                NPN_MemAlloc(sizeof (NPUTF8) * name.length()));
        std::copy(name.begin(), name.end(), name_str);
        STRINGN_TO_NPVARIANT(name_str, name.length(), *result);
        return true;
    }

    bool plugin_instance::get_version(const NPVariant * const /* args */,
                                      const uint32_t /* argCount */,
                                      NPVariant * const result)
    {
        static const std::string ver = PACKAGE_VERSION;
        NPUTF8 * const ver_str =
            static_cast<NPUTF8 *>(NPN_MemAlloc(sizeof (NPUTF8) * ver.length()));
        std::copy(ver.begin(), ver.end(), ver_str);
        STRINGN_TO_NPVARIANT(ver_str, ver.length(), *result);
        return true;
    }
} // namespace
