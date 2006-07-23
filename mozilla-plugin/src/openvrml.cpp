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

# include <cerrno>
# include <list>
# include <memory>
# include <sstream>
# include <stdexcept>
# include <vector>
# include <sys/socket.h>
# include <sys/wait.h>
# include <boost/lexical_cast.hpp>
# include <boost/noncopyable.hpp>
# include <mozilla-config.h>
# include <npupp.h>
# include <nsCOMPtr.h>
# include <nsIServiceManagerUtils.h>
# include <nsMemory.h>
# include <nsString.h>
# include <nsIConsoleService.h>
# if defined MOZ_X11
#   include <fcntl.h>
#   include <gdk/gdkx.h>
# else
#   error Unsupported toolkit.
# endif
# include "openvrml.h"

namespace {

    void printerr(const char * str);

    class ScriptablePeer;

    extern "C" gboolean request_data_available(GIOChannel * source,
                                               GIOCondition condition,
                                               gpointer data);

    class PluginInstance : boost::noncopyable {
        friend class ScriptablePeer;
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
        nsCOMPtr<VrmlBrowser> scriptablePeer;

    public:
        explicit PluginInstance(NPP npp) throw (std::bad_alloc);
        ~PluginInstance() throw ();

        nsISupports * GetScriptablePeer() throw ();
        void SetWindow(NPWindow & window) throw (std::bad_alloc);
        void HandleEvent(void * event) throw ();
        ssize_t WriteCommand(const std::string & command);
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
    nsCOMPtr<nsIConsoleService> console_service;
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
# endif // defined MOZ_X11

    nsresult rv;
    console_service = do_GetService(NS_CONSOLESERVICE_CONTRACTID, &rv);
    if (NS_FAILED(rv)) { return NPERR_GENERIC_ERROR; }

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
        instance->pdata = new PluginInstance(instance);
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
                      NPBool /* seekable */,
                      uint16 * const stype)
{
    if (!instance) { return NPERR_INVALID_INSTANCE_ERROR; }
    *stype = NP_NORMAL;

    assert(instance->pdata);
    PluginInstance & pluginInstance =
        *static_cast<PluginInstance *>(instance->pdata);

    std::ostringstream command;
    command << "new-stream " << ptrdiff_t(stream) << ' ' << type << ' '
            << stream->url << '\n';
    const ssize_t bytes_written = pluginInstance.WriteCommand(command.str());
    return (bytes_written < 0)
        ? NPERR_GENERIC_ERROR
        : NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(const NPP instance,
                          NPStream * const stream,
                          NPError /* reason */)
{
    if (!instance || !instance->pdata) { return NPERR_INVALID_INSTANCE_ERROR; }

    PluginInstance & pluginInstance =
        *static_cast<PluginInstance *>(instance->pdata);

    std::ostringstream command;
    command << "destroy-stream " << ptrdiff_t(stream) << '\n';
    const ssize_t bytes_written = pluginInstance.WriteCommand(command.str());
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
                const int32 offset,
                const int32 len,
                void * const buffer)
{
    if (!instance || !instance->pdata) { return 0; }

    PluginInstance & pluginInstance =
        *static_cast<PluginInstance *>(instance->pdata);

    std::ostringstream command;
    command << "write " << ptrdiff_t(stream) << ' ' << offset << ' ' << len
            << '\n';
    for (int32 i = 0; i < len; ++i) {
        command.put(static_cast<char *>(buffer)[i]);
    }
    const ssize_t bytes_written = pluginInstance.WriteCommand(command.str());

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

namespace {

    void printerr(const char * str)
    {
        console_service->LogStringMessage(NS_ConvertUTF8toUTF16(str).get());
    }

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

    NS_IMETHODIMP ScriptablePeer::GetInterfaces(PRUint32 * /* count */,
                                                nsIID *** /* array */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetHelperForLanguage(PRUint32 /* language */,
                                                       nsISupports ** /* _retval */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetContractID(char ** /* aContractID */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP
    ScriptablePeer::GetClassDescription(char ** /* aClassDescription */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetClassID(nsCID ** /* aClassID */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetClassIDNoAlloc(nsCID * /* aClassIDNoAlloc */)
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

        const std::string name;
        const size_t bufferSize = sizeof (char) * (name.length() + 1);

        *_retval = static_cast<char *>(nsMemory::Clone(name.c_str(),
                                                       bufferSize));
        if (!*_retval) { return NS_ERROR_OUT_OF_MEMORY; }

        return NS_OK;
    }

    NS_IMETHODIMP ScriptablePeer::GetVersion(char ** _retval)
    {
        if (!_retval) { return NS_ERROR_NULL_POINTER; }

        const std::string version;
        const size_t bufferSize = sizeof (char) * (version.length() + 1);

        *_retval = static_cast<char *>(nsMemory::Clone(version.c_str(),
                                                       bufferSize));
        if (!*_retval) { return NS_ERROR_OUT_OF_MEMORY; }

        return NS_OK;
    }

    NS_IMETHODIMP ScriptablePeer::GetCurrentSpeed(float * /* _retval */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetCurrentFrameRate(float * /* _retval */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetWorldURL(char ** /* _retval */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::ReplaceWorld(PRUint32 /* nodeArraySize */,
                                               VrmlNode ** /* nodeArray */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::LoadURL(PRUint32 /* urlArraySize */,
                                          const char ** /* url */,
                                          PRUint32 /* paramArraySize */,
                                          const char ** /* parameter */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::SetDescription(const char * /* description */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP
    ScriptablePeer::CreateVrmlFromString(const char * /* vrmlSyntax */,
                                         PRUint32 * /* nodeArraySize */,
                                         VrmlNode *** /* nodeArray */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::CreateVrmlFromURL(PRUint32 /* urlArraySize */,
                                                    const char ** /* url */,
                                                    VrmlNode * /* node */,
                                                    const char * /* event */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::GetNode(const char * /* name */,
                                          VrmlNode ** /* _retval */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::AddRoute(VrmlNode * /* fromNode */,
                                           const char * /* fromEventOut */,
                                           VrmlNode * /* toNode */,
                                           const char * /* toEventIn */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP ScriptablePeer::DeleteRoute(VrmlNode * /* fromNode */,
                                              const char * /* fromEventOut */,
                                              VrmlNode * /* toNode */,
                                              const char * /* toEvent */)
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
    ScriptablePeer::AddBrowserListener(VrmlBrowserListener * /* listener */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP
    ScriptablePeer::RemoveBrowserListener(VrmlBrowserListener * /* listener */)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }


    PluginInstance::PluginInstance(const NPP npp) throw (std::bad_alloc):
        npp(npp),
        window(0),
        x(0),
        y(0),
        width(0),
        height(0),
        command_channel(0),
        request_channel(0),
        request_channel_watch_id(0),
        scriptablePeer(new ScriptablePeer(*this))
    {}

    PluginInstance::~PluginInstance() throw ()
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
    }

    nsISupports * PluginInstance::GetScriptablePeer() throw ()
    {
        return this->scriptablePeer;
    }

    void PluginInstance::SetWindow(NPWindow & window)
        throw (std::bad_alloc)
    {
        assert(window.window);
        if (this->window) {
            //
            // The plug-in window is unchanged. Resize the window and exit.
            //
        } else {
            using std::string;
            using std::vector;
            using boost::lexical_cast;

            this->window = GdkNativeWindow(ptrdiff_t(window.window));

            //
            // The OPENVRML_PLAYER environment variable overrides the default
            // path to the child process executable.  To allow OPENVRML_PLAYER
            // to include arguments (rather than just be a path to an
            // executable), it is parsed wit g_shell_parse_argv.  This is
            // particularly useful in case we want to run the child process in
            // a harness like valgrind.
            //
            gint openvrml_player_cmd_argc = 0;
            gchar ** openvrml_player_cmd_argv = 0;
            const gchar * const openvrml_player_cmd =
                g_getenv("OPENVRML_PLAYER");
            if (!openvrml_player_cmd) {
                openvrml_player_cmd_argc = 1;
                openvrml_player_cmd_argv =
                    static_cast<gchar **>(g_malloc(sizeof (gchar *) * 2));
                if (!openvrml_player_cmd_argv) { throw std::bad_alloc(); }
                openvrml_player_cmd_argv[0] =
                    g_strdup(OPENVRML_LIBEXECDIR_ "/openvrml-player");
                openvrml_player_cmd_argv[1] = 0;
                if (!openvrml_player_cmd_argv[0]) { throw std::bad_alloc(); }
            } else {
                GError * error = 0;
                gboolean succeeded =
                    g_shell_parse_argv(openvrml_player_cmd,
                                       &openvrml_player_cmd_argc,
                                       &openvrml_player_cmd_argv,
                                       &error);
                if (!succeeded) {
                    if (error) {
                        g_critical(error->message);
                        g_error_free(error);
                    }
                }
            }

            string socket_id_arg =
                "--gtk-socket-id=" + lexical_cast<string>(this->window);
            const char * socket_id_arg_c_str = socket_id_arg.c_str();
            vector<char> socket_id_arg_vec(
                socket_id_arg_c_str,
                socket_id_arg_c_str + socket_id_arg.length() + 1);

            const gint argv_size = openvrml_player_cmd_argc + 2;
            gchar ** const argv =
                static_cast<gchar **>(g_malloc(sizeof (gchar *) * argv_size));
            if (!argv) { throw std::bad_alloc(); }
            gint i;
            for (i = 0; i < openvrml_player_cmd_argc; ++i) {
                argv[i] = openvrml_player_cmd_argv[i];
            }
            argv[i++] = &socket_id_arg_vec.front();
            argv[i]   = 0;

            gchar * const working_directory = g_get_current_dir();
            if (!working_directory) { throw std::bad_alloc(); };
            gchar ** envp = 0;
            GPid * const child_pid = 0;
            gint standard_input, standard_output;
            gint * const standard_error = 0;
            GError * error = 0;
            gboolean succeeded = g_spawn_async_with_pipes(working_directory,
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
                if (error) {
                    g_critical(error->message);
                    g_error_free(error);
                }
            }

            // XXX
            // XXX Not the least bit exception-safe.  This needs some
            // XXX ScopeGuard love.
            // XXX
            g_free(working_directory);
            g_free(argv);
            g_strfreev(openvrml_player_cmd_argv);

            if (succeeded) {
                this->command_channel = g_io_channel_unix_new(standard_input);
                if (!this->command_channel) { throw std::bad_alloc(); }
                const GIOStatus status =
                    g_io_channel_set_encoding(this->command_channel,
                                              0, // binary (no encoding)
                                              &error);
                if (status != G_IO_STATUS_NORMAL) {
                    if (error) {
                        g_critical(error->message);
                        g_error_free(error);
                    }
                    // XXX
                    // XXX Should probably throw here instead.
                    // XXX
                    return;
                }

                this->request_channel = g_io_channel_unix_new(standard_output);
                if (!this->command_channel) { throw std::bad_alloc(); }
                this->request_channel_watch_id =
                    g_io_add_watch(this->request_channel,
                                   G_IO_IN,
                                   request_data_available,
                                   this);
            }
        }
    }

    void PluginInstance::HandleEvent(void *) throw ()
    {}

    ssize_t PluginInstance::WriteCommand(const std::string & command)
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

    gboolean request_data_available(GIOChannel * source,
                                    GIOCondition,
                                    gpointer data)
    {
        using std::string;

        PluginInstance & pluginInstance = *static_cast<PluginInstance *>(data);

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
                NPN_GetURL(pluginInstance.npp,
                           url.c_str(),
                           target.empty() ? 0 : target.c_str());
            }
        }

        return true;
    }
} // namespace
