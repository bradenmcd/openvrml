// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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
# include <boost/scoped_ptr.hpp>
# include <mozilla-config.h>
# include <npupp.h>
# if defined MOZ_X11
#   include <fcntl.h>
#   include <gdk/gdkx.h>
# else
#   error Unsupported toolkit.
# endif

using namespace boost::multi_index::detail;  // for scope_guard

namespace {

    void printerr(const char * str);

    extern "C" gboolean request_data_available(GIOChannel * source,
                                               GIOCondition condition,
                                               gpointer data);
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
        friend gboolean request_data_available(GIOChannel * source,
                                               GIOCondition condition,
                                               gpointer data);

        const NPP npp;
        GdkNativeWindow window;
        int x, y;
        int width, height;
        GIOChannel * command_channel;
        GIOChannel * request_channel;
        guint request_channel_watch_id;
        std::stringstream request_line;

    public:
        NPObject * const npobj;

        explicit plugin_instance(NPP npp) throw (std::bad_alloc);
        ~plugin_instance() throw ();

        void set_window(NPWindow & window)
            throw (std::bad_alloc, std::runtime_error);
        void HandleEvent(void * event) throw ();
        ssize_t write_command(const std::string & command);

        //
        // Scripting API method implementations.
        //
        bool get_name(const NPVariant * args, uint32_t argCount,
                      NPVariant * result);
        bool get_version(const NPVariant * args, uint32_t argCount,
                         NPVariant * result);
    };
} // namespace

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
    return "model/vrml:wrl:VRML world;"
           "x-world/x-vrml:wrl:VRML world";
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
                uint16 /* mode */,
                int16 /* argc */,
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
                      uint16 * const stype)
{
    if (!instance) { return NPERR_INVALID_INSTANCE_ERROR; }
    *stype = NP_NORMAL;

    assert(instance->pdata);
    plugin_instance & pluginInstance =
        *static_cast<plugin_instance *>(instance->pdata);

    std::ostringstream command;
    command << "new-stream " << ptrdiff_t(stream) << ' ' << type << ' '
            << stream->url << '\n';
    const ssize_t bytes_written = pluginInstance.write_command(command.str());
    return (bytes_written < 0)
        ? NPERR_GENERIC_ERROR
        : NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(const NPP instance,
                          NPStream * const stream,
                          NPError /* reason */)
{
    if (!instance || !instance->pdata) { return NPERR_INVALID_INSTANCE_ERROR; }

    plugin_instance & pluginInstance =
        *static_cast<plugin_instance *>(instance->pdata);

    std::ostringstream command;
    command << "destroy-stream " << ptrdiff_t(stream) << '\n';
    const ssize_t bytes_written = pluginInstance.write_command(command.str());
    return (bytes_written < 0)
        ? NPERR_GENERIC_ERROR
        : NPERR_NO_ERROR;
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

int32 NPP_WriteReady(NPP, NPStream *)
{
    return STREAMBUFSIZE;
}


int32 NPP_Write(const NPP instance,
                NPStream * const stream,
                int32 /* offset */,
                const int32 len,
                void * const buffer)
{
    if (!instance || !instance->pdata) { return 0; }

    plugin_instance & pluginInstance =
        *static_cast<plugin_instance *>(instance->pdata);

    std::ostringstream command;
    command << "write " << ptrdiff_t(stream) << ' ' << ' ' << len << '\n';
    for (int32 i = 0; i < len; ++i) {
        command.put(static_cast<char *>(buffer)[i]);
    }
    const ssize_t bytes_written = pluginInstance.write_command(command.str());

    return bytes_written; // The number of bytes accepted.
}

void NPP_StreamAsFile(const NPP instance,
                      NPStream *,
                      const char * /* fname */)
{
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

int16 NPP_HandleEvent(NPP, void * /* event */)
{
    return true;
}

void NPP_URLNotify(NPP,
                   const char * /* url */,
                   NPReason,
                   void * /* notifyData */)
{}

jref NPP_GetJavaClass()
{
    return 0;
}

NPError NPP_GetValue(const NPP npp,
                     const NPPVariable variable,
                     void * const value)
{
    if (!npp) { return NPERR_INVALID_INSTANCE_ERROR; }

    NPError err = NPERR_NO_ERROR;
    plugin_instance * instance = 0;

    switch (variable) {
    case NPPVpluginNeedsXEmbed:
        *static_cast<PRBool *>(value) = PR_TRUE;
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

void NPN_ReleaseVariantValue(NPVariant * variant)
{
    CallNPN_ReleaseVariantValueProc(mozillaFuncs.releasevariantvalue, variant);
}

NPIdentifier NPN_GetStringIdentifier(const NPUTF8 * name)
{
    return CallNPN_GetStringIdentifierProc(mozillaFuncs.getstringidentifier,
                                           name);
}

void NPN_GetStringIdentifiers(const NPUTF8 ** names,
                              int32_t nameCount,
                              NPIdentifier * identifiers)
{
    CallNPN_GetStringIdentifiersProc(mozillaFuncs.getstringidentifiers,
                                     names,
                                     nameCount,
                                     identifiers);
}

NPIdentifier NPN_GetIntIdentifier(int32_t intid)
{
    return CallNPN_GetIntIdentifierProc(mozillaFuncs.getintidentifier, intid);
}

bool NPN_IdentifierIsString(NPIdentifier * identifier)
{
    return CallNPN_IdentifierIsStringProc(mozillaFuncs.identifierisstring,
                                          identifier);
}

NPUTF8 * NPN_UTF8FromIdentifier(NPIdentifier identifier)
{
    return CallNPN_UTF8FromIdentifierProc(mozillaFuncs.utf8fromidentifier,
                                          identifier);
}

int32_t NPN_IntFromIdentifier(NPIdentifier identifier)
{
    return CallNPN_IntFromIdentifierProc(mozillaFuncs.intfromidentifier,
                                         identifier);
}

NPObject * NPN_CreateObject(NPP npp, NPClass * aClass)
{
    return CallNPN_CreateObjectProc(mozillaFuncs.createobject, npp, aClass);
}

NPObject * NPN_RetainObject(NPObject * npobj)
{
    return CallNPN_RetainObjectProc(mozillaFuncs.retainobject, npobj);
}

void NPN_ReleaseObject(NPObject * npobj)
{
    CallNPN_ReleaseObjectProc(mozillaFuncs.releaseobject, npobj);
}

bool NPN_Invoke(NPP npp, NPObject * npobj, NPIdentifier methodName,
                const NPVariant * args, uint32_t argCount, NPVariant * result)
{
    return CallNPN_InvokeProc(mozillaFuncs.invoke,
                              npp, npobj, methodName, args, argCount, result);
}

bool NPN_InvokeDefault(NPP npp, NPObject * npobj, const NPVariant * args,
                       uint32_t argCount, NPVariant * result)
{
    return CallNPN_InvokeDefaultProc(mozillaFuncs.invokeDefault,
                                     npp, npobj, args, argCount, result);
}

bool NPN_Evaluate(NPP npp, NPObject * npobj, NPString * script,
                  NPVariant * result)
{
    return CallNPN_EvaluateProc(mozillaFuncs.evaluate, npp, npobj, script,
                                result);
}

bool NPN_GetProperty(NPP npp, NPObject * npobj, NPIdentifier propertyName,
                     NPVariant * result)
{
    return CallNPN_GetPropertyProc(mozillaFuncs.getproperty,
                                   npp, npobj, propertyName, result);
}

bool NPN_SetProperty(NPP npp, NPObject * npobj, NPIdentifier propertyName,
                     const NPVariant * result)
{
    return CallNPN_SetPropertyProc(mozillaFuncs.setproperty,
                                   npp, npobj, propertyName, result);
}

bool NPN_RemoveProperty(NPP npp, NPObject * npobj, NPIdentifier propertyName)
{
    return CallNPN_RemovePropertyProc(mozillaFuncs.removeproperty,
                                      npp, npobj, propertyName);
}

bool NPN_HasProperty(NPP npp, NPObject * npobj, NPIdentifier propertyName)
{
    return CallNPN_HasPropertyProc(mozillaFuncs.hasproperty,
                                   npp, npobj, propertyName);
}

bool NPN_HasMethod(NPP npp, NPObject * npobj, NPIdentifier methodName)
{
    return CallNPN_HasMethodProc(mozillaFuncs.hasmethod,
                                 npp, npobj, methodName);
}

void NPN_SetException(NPObject * npobj, const NPUTF8 * message)
{
    return CallNPN_SetExceptionProc(mozillaFuncs.setexception, npobj, message);
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
        command_channel(0),
        request_channel(0),
        request_channel_watch_id(0),
        npobj(NPN_CreateObject(this->npp, &npclass))
    {
        if (!this->npobj) { throw std::bad_alloc(); }
    }

    plugin_instance::~plugin_instance() throw ()
    {
        if (this->request_channel_watch_id) {
            const gboolean succeeded =
                g_source_remove(this->request_channel_watch_id);
            g_assert(succeeded);
        }

        if (this->request_channel) {
            GError * error = 0;
            const gboolean flush = false;
            GIOStatus status = g_io_channel_shutdown(this->request_channel,
                                                     flush,
                                                     &error);
            if (status != G_IO_STATUS_NORMAL) {
                if (error) {
                    printerr(error->message);
                    g_error_free(error);
                }
            }

            g_io_channel_unref(this->request_channel);
        }

        if (this->command_channel) {
            GError * error = 0;
            const gboolean flush = false;
            GIOStatus status = g_io_channel_shutdown(this->command_channel,
                                                     flush,
                                                     &error);
            if (status != G_IO_STATUS_NORMAL) {
                if (error) {
                    printerr(error->message);
                    g_error_free(error);
                }
            }

            g_io_channel_unref(this->command_channel);
        }

        NPN_ReleaseObject(this->npobj);
    }

    void plugin_instance::set_window(NPWindow & window)
        throw (std::bad_alloc, std::runtime_error)
    {
        assert(window.window);
        if (this->window) {
            //
            // The plug-in window is unchanged. Resize the window and exit.
            //
            return;
        }

        using std::string;
        using std::vector;
        using boost::lexical_cast;

        this->window = GdkNativeWindow(ptrdiff_t(window.window));

        //
        // The OPENVRML_GTKPLUG environment variable overrides the default
        // path to the child process executable.  To allow OPENVRML_GTKPLUG
        // to include arguments (rather than just be a path to an
        // executable), it is parsed with g_shell_parse_argv.  This is
        // particularly useful in case we want to run the child process in
        // a harness like valgrind.
        //
        gint openvrml_gtkplug_cmd_argc = 0;
        gchar ** openvrml_gtkplug_cmd_argv = 0;
        scope_guard openvrml_gtkplug_cmd_argv_guard =
            make_guard(g_strfreev, openvrml_gtkplug_cmd_argv);
        boost::ignore_unused_variable_warning(openvrml_gtkplug_cmd_argv_guard);
        const gchar * const openvrml_gtkplug_cmd =
            g_getenv("OPENVRML_GTKPLUG");
        if (!openvrml_gtkplug_cmd) {
            openvrml_gtkplug_cmd_argc = 1;
            openvrml_gtkplug_cmd_argv =
                static_cast<gchar **>(g_malloc0(sizeof (gchar *) * 2));
            if (!openvrml_gtkplug_cmd_argv) { throw std::bad_alloc(); }
            openvrml_gtkplug_cmd_argv[0] =
                g_strdup(OPENVRML_LIBEXECDIR_ "/openvrml-gtkplug");
            if (!openvrml_gtkplug_cmd_argv[0]) { throw std::bad_alloc(); }
        } else {
            GError * error = 0;
            scope_guard error_guard = make_guard(g_error_free, error);
            gboolean succeeded =
                g_shell_parse_argv(openvrml_gtkplug_cmd,
                                   &openvrml_gtkplug_cmd_argc,
                                   &openvrml_gtkplug_cmd_argv,
                                   &error);
            if (!succeeded) {
                throw std::runtime_error(error
                                         ? error->message
                                         : "g_shell_parse_argv failure");
            }
            error_guard.dismiss();
        }

        string socket_id_arg = lexical_cast<string>(this->window);
        const char * socket_id_arg_c_str = socket_id_arg.c_str();
        vector<char> socket_id_arg_vec(
            socket_id_arg_c_str,
            socket_id_arg_c_str + socket_id_arg.length() + 1);

        const gint argv_size = openvrml_gtkplug_cmd_argc + 2;
        gchar ** const argv =
            static_cast<gchar **>(g_malloc(sizeof (gchar *) * argv_size));
        if (!argv) { throw std::bad_alloc(); }
        scope_guard argv_guard = make_guard(g_free, argv);
        boost::ignore_unused_variable_warning(argv_guard);
        gint i;
        for (i = 0; i < openvrml_gtkplug_cmd_argc; ++i) {
            argv[i] = openvrml_gtkplug_cmd_argv[i];
        }
        argv[i++] = &socket_id_arg_vec.front();
        argv[i]   = 0;

        gchar * const working_dir = g_get_current_dir();
        if (!working_dir) { throw std::bad_alloc(); };
        scope_guard working_dir_guard = make_guard(g_free, working_dir);
        boost::ignore_unused_variable_warning(working_dir_guard);

        gchar ** envp = 0;
        GPid * const child_pid = 0;
        gint standard_input, standard_output;
        gint * const standard_error = 0;
        GError * error = 0;
        scope_guard error_guard = make_guard(g_error_free, error);
        gboolean succeeded = g_spawn_async_with_pipes(working_dir,
                                                      argv,
                                                      envp,
                                                      GSpawnFlags(0),
                                                      0,
                                                      0,
                                                      child_pid,
                                                      &standard_input,
                                                      &standard_output,
                                                      standard_error,
                                                      &error);
        if (!succeeded) {
            throw std::runtime_error(error
                                     ? error->message
                                     : "g_spawn_async_with_pipes failure");
        }

        //
        // Don't dismiss "error_guard" yet; we reuse "error" below.
        //

        this->command_channel = g_io_channel_unix_new(standard_input);
        if (!this->command_channel) { throw std::bad_alloc(); }
        const GIOStatus status =
            g_io_channel_set_encoding(this->command_channel,
                                      0, // binary (no encoding)
                                      &error);
        if (status != G_IO_STATUS_NORMAL) {
            throw std::runtime_error(error
                                     ? error->message
                                     : "g_io_channel_set_encoding failure");
        }
        error_guard.dismiss();

        this->request_channel = g_io_channel_unix_new(standard_output);
        if (!this->request_channel) { throw std::bad_alloc(); }
        this->request_channel_watch_id =
            g_io_add_watch(this->request_channel,
                           G_IO_IN,
                           request_data_available,
                           this);
    }

    void plugin_instance::HandleEvent(void *) throw ()
    {}

    ssize_t plugin_instance::write_command(const std::string & command)
    {
        if (!this->command_channel) { return 0; }

        gsize bytes_written;
        GError * error = 0;
        GIOStatus status = g_io_channel_write_chars(this->command_channel,
                                                    command.data(),
                                                    command.length(),
                                                    &bytes_written,
                                                    &error);
        if (status != G_IO_STATUS_NORMAL) {
            if (error) {
                printerr(error->message);
                g_error_free(error);
            }
            return -1;
        }

        do {
            status = g_io_channel_flush(this->command_channel, &error);
        } while (status == G_IO_STATUS_AGAIN);

        if (status != G_IO_STATUS_NORMAL) {
            if (error) {
                printerr(error->message);
                g_error_free(error);
            }
            return -1;
        }

        return bytes_written;
    }

    bool plugin_instance::get_name(const NPVariant * const /* args */,
                                   const uint32_t /* argCount */,
                                   NPVariant * const result)
    {
        static const std::string name = PACKAGE_NAME;
        NPUTF8 * const name_str =
            static_cast<NPUTF8 *>(NPN_MemAlloc(sizeof (NPUTF8) * name.length()));
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

    gboolean request_data_available(GIOChannel * const source,
                                    GIOCondition,
                                    const gpointer data)
    {
        using std::string;

        plugin_instance & pluginInstance = *static_cast<plugin_instance *>(data);

        gchar c;
        do {
            gsize bytes_read;
            GError * error = 0;
            const GIOStatus status =
                g_io_channel_read_chars(source, &c, 1, &bytes_read, &error);
            if (status == G_IO_STATUS_ERROR) {
                if (error) {
                    g_warning(error->message);
                    g_error_free(error);
                }
                return false;
            }
            if (status == G_IO_STATUS_EOF) { return false; }
            if (status == G_IO_STATUS_AGAIN) { continue; }
            g_return_val_if_fail(status == G_IO_STATUS_NORMAL, false);

            g_assert(bytes_read == 1);

            if (c != '\n') { pluginInstance.request_line.put(c); }

        } while (g_io_channel_get_buffer_condition(source) & G_IO_IN);

        if (c == '\n') {
            string request_type;
            pluginInstance.request_line >> request_type;
            if (request_type == "get-url") {
                string url, target;
                pluginInstance.request_line >> url >> target;
                const NPError result =
                    NPN_GetURL(pluginInstance.npp,
                               url.c_str(),
                               target.empty() ? 0 : target.c_str());
                std::ostringstream command;
                command << "get-url-result " << url << ' ' << result << '\n';
                const ssize_t bytes_written =
                    pluginInstance.write_command(command.str());
                if (bytes_written != ssize_t(command.str().length())) {
                    // XXX
                    // XXX Do what here? Console message?
                    // XXX
                }
            }
        }

        return true;
    }
} // namespace
