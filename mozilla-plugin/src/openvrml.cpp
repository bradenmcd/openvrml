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
# include <boost/noncopyable.hpp>
# if defined MOZ_X11
#   include "gtkvrml/vrmlbrowser.h"
# else
#   error Unsupported toolkit.
# endif

namespace {

    //
    // Enumeration of the supported toolkits.
    //
    enum Toolkit {
        gtk
    };

    //
    // Template specialization is used instead of having a lot of messy
    // #ifdef's.  Instead, just a few #ifdef's determine what headers are
    // included (above), and what toolkit gets used.
    //
    // To add support for a toolkit (or platform):
    //
    //  1. Add an enumerant to the Toolkit enumeration.
    //  2. Add #ifdef's to set the toolkit variable appropriately.
    //  3. Specialize the PluginInstance template for the toolkit.
    //
    // It should be possible to factor nearly all platform- and toolkit-
    // specific code into the PluginInstance specialization.  There may be a
    // few exceptions to this, such as the XEmbed-related logic in
    // NP_Initialize.
    //
    const Toolkit toolkit =
# if defined MOZ_X11
        gtk;
# endif

    template <Toolkit ToolkitType>
    class PluginInstance : boost::noncopyable {
    public:
        explicit PluginInstance(const std::string & initialURL) throw ();
        ~PluginInstance() throw ();

        void SetWindow(NPWindow & window) throw (std::bad_alloc);
        void HandleEvent(void * event) throw ();
    };

    template <>
    class PluginInstance<gtk> : boost::noncopyable {
        std::string initialURL;
        GtkWidget * plug;
        GtkWidget * vrmlBrowser;
        gint x, y;
        gint width, height;

    public:
        explicit PluginInstance(const std::string & initialURL) throw ();
        ~PluginInstance() throw ();

        void SetWindow(NPWindow & window) throw (std::bad_alloc);
        void HandleEvent(void * event) throw ();
    };
} // namespace

extern "C"
char * NP_GetMIMEDescription()
{
    return "model/vrml:wrl:VRML world;"
           "x-world/x-vrml:wrl:VRML world";
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
extern "C"
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
    pluginFuncs->javaClass     = NULL;

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

extern "C"
NPError NP_Shutdown()
{
    NPP_Shutdown();
    return NPERR_NO_ERROR;
}

extern "C"
NPError NP_GetValue(void * future, NPPVariable variable, void * value)
{
    return NPP_GetValue(static_cast<NPP>(future), variable, value);
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
NPError NPP_New(const NPMIMEType pluginType,
                const NPP instance,
                const uint16 mode,
                const int16 argc,
                char * argn[],
                char * argv[],
                NPSavedData * saved)
{
    if (!instance) { return NPERR_INVALID_INSTANCE_ERROR; }

    try {
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
        instance->pdata = new PluginInstance<toolkit>(url);
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

    if (instance->pdata) {
        delete static_cast<PluginInstance<toolkit> *>(instance->pdata);
        instance->pdata = 0;
    }

    return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(const NPP instance, NPWindow * const window)
{
    if (!instance || !instance->pdata) { return NPERR_INVALID_INSTANCE_ERROR; }
    try {
        assert(window);
        static_cast<PluginInstance<toolkit> *>(instance->pdata)
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

    PluginInstance<toolkit> * const pluginInstance =
        static_cast<PluginInstance<toolkit> *>(instance->pdata);

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
        PluginInstance<toolkit> * pluginInstance =
            static_cast<PluginInstance<toolkit> *>(instance->pdata);
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
        PluginInstance<toolkit> * pluginInstance =
            static_cast<PluginInstance<toolkit> *>(instance->pdata);
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
        PluginInstance<toolkit> * pluginInstance =
                static_cast<PluginInstance<toolkit> *>(instance->pdata);

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
        PluginInstance<toolkit> * pluginInstance =
            static_cast<PluginInstance<toolkit> *>(instance->pdata);
    } catch (...) {
        return false;
    }
    return true;
}

void NPP_URLNotify(const NPP instance,
                   const char * const url,
                   const NPReason reason,
                   void * const notifyData)
{}

jref NPP_GetJavaClass()
{
    return 0;
}

NPError NPP_GetValue(const NPP instance,
                     const NPPVariable variable,
                     void * const value)
{
    NPError err = NPERR_NO_ERROR;

    switch (variable) {
    case NPPVpluginNameString:
        *static_cast<const char **>(value) = "OpenVRML";
        break;
    case NPPVpluginDescriptionString:
        *static_cast<const char **>(value) = "VRML browser";
        break;
    case NPPVpluginNeedsXEmbed:
        *static_cast<PRBool *>(value) = PR_TRUE;
        break;
    default:
        err = NPERR_GENERIC_ERROR;
    }
    return err;
}

NPError NPP_SetValue(const NPP instance,
                     const NPNVariable variable,
                     void * const value)
{
    return NPERR_NO_ERROR;
}

namespace {

    PluginInstance<gtk>::PluginInstance(const std::string & initialURL)
        throw ():
        initialURL(initialURL),
        plug(0),
        vrmlBrowser(0),
        x(0),
        y(0),
        width(0),
        height(0)
    {}

    PluginInstance<gtk>::~PluginInstance() throw ()
    {
        gtk_widget_destroy(this->plug);
    }

    void PluginInstance<gtk>::SetWindow(NPWindow & window)
        throw (std::bad_alloc)
    {
        assert(window.window);
        if (this->plug) {
            //
            // The plug-in window is unchanged. Resize the window and exit.
            //
            gtk_widget_set_usize(this->vrmlBrowser,
                                 window.width,
                                 window.height);
        } else {
            this->plug =
                gtk_plug_new(reinterpret_cast<GdkNativeWindow>(window.window));
            if (!this->plug) { throw std::bad_alloc(); }

            this->vrmlBrowser = gtk_vrml_browser_new();
            if (!this->vrmlBrowser) { throw std::bad_alloc(); }

            gtk_container_add(GTK_CONTAINER(this->plug), this->vrmlBrowser);

            gtk_widget_show_all(this->plug);

            gtk_vrml_browser_load_url(GTK_VRML_BROWSER(this->vrmlBrowser),
                                      this->initialURL.c_str());
        }
    }

    void PluginInstance<gtk>::HandleEvent(void * event) throw ()
    {}

} // namespace
