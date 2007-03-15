// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2006, 2007  Braden McDaniel
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
# include <list>
# include <stdexcept>
# include <string>
# include <vector>
# include <boost/concept_check.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/ref.hpp>
# include <curl/curl.h>
# include <libgnomevfs/gnome-vfs.h>
# include <libgnomeui/libgnomeui.h>
# include <glade/glade.h>
# include <glade/glade-build.h>

# include "filechooserdialog.h"

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace boost::multi_index::detail; // for scope_guard

extern "C" {
    //
    // GTK+ signal handlers; libglade needs them to have external linkage in
    // order to connect them.
    //
    void openvrml_player_on_about_activated(GtkWindow * window);
    void openvrml_player_on_file_open_activated(
        OpenvrmlPlayerFileChooserDialog * dialog);
    void openvrml_player_on_locationentry_activated(GtkEntry * entry);
    void openvrml_player_on_filechooserdialog_response(GtkDialog * dialog,
                                                       gint arg1,
                                                       gpointer user_data);
    void openvrml_player_quit();

    //
    // The following functions are 'extern "C"' because they get passed as
    // callbacks to C APIs; however, their symbols don't need to be exposed.
    //
    G_GNUC_INTERNAL
    gboolean openvrml_player_request_data_available(GIOChannel * source,
                                                    GIOCondition condition,
                                                    gpointer data);
    G_GNUC_INTERNAL gboolean openvrml_player_curl_prepare(GSource * source,
                                                          gint * timeout);
    G_GNUC_INTERNAL gboolean openvrml_player_curl_check(GSource * source);
    G_GNUC_INTERNAL
    gboolean openvrml_player_curl_dispatch(GSource * source,
                                           GSourceFunc callback,
                                           gpointer user_data);
    G_GNUC_INTERNAL void openvrml_player_curl_finalize(GSource * source);
    G_GNUC_INTERNAL
    gboolean openvrml_player_curl_source_callback(gpointer data);
    G_GNUC_INTERNAL size_t openvrml_player_curl_write(void * ptr,
                                                      size_t size,
                                                      size_t nmemb,
                                                      void * stream);
    G_GNUC_INTERNAL void openvrml_player_watch_child(GPid pid,
                                                     gint status,
                                                     gpointer data);
    G_GNUC_INTERNAL
    GtkWidget *
    openvrml_player_build_file_chooser_dialog(GladeXML * xml,
                                              GType widget_type,
                                              GladeWidgetInfo * info);
}

namespace {
    G_GNUC_INTERNAL const char app_name[] = "OpenVRML Player";
    G_GNUC_INTERNAL const char app_id[] = "openvrml-player";

    G_GNUC_INTERNAL GIOChannel * command_channel;
    G_GNUC_INTERNAL GIOChannel * request_channel;
    G_GNUC_INTERNAL guint request_channel_watch_id;

    G_GNUC_INTERNAL ssize_t write_command(const std::string & command);
    G_GNUC_INTERNAL GSource * curl_source_new(CURLM * multi_handle);
    G_GNUC_INTERNAL GladeXML * xml_new(GnomeProgram & program);
    G_GNUC_INTERNAL void load_url(const gchar * url);


    class G_GNUC_INTERNAL curl_stream_data {
        CURL * const handle_;
        std::vector<char> url_;
        bool initialized_;

    public:
        curl_stream_data(CURL * handle, const char * url);

        CURL * handle() const;
        const char * url() const;
        void initialize();
        bool initialized() const;
    };

    typedef std::map<CURL *, curl_stream_data> stream_data_map_t;

    struct curl_source_callback_data {
        GSource * curl_source;
        stream_data_map_t stream_data_map;
    };

    struct request_data {
        std::stringstream request_line;
        curl_source_callback_data * source_callback_data;
    };
}

# define OPENVRML_PLAYER_CURL_EASY_RETURN_VAL_IF_ERROR(code, val) \
    if G_LIKELY((code) == CURLE_OK) {} else {                     \
        g_return_if_fail_warning(G_LOG_DOMAIN,                    \
                                 __PRETTY_FUNCTION__,             \
                                 curl_easy_strerror(code));       \
        return (val);                                             \
    }

# define OPENVRML_PLAYER_CURL_MULTI_RETURN_IF_ERROR(code)    \
    if G_LIKELY((code) == CURLM_OK) {} else {                \
        g_return_if_fail_warning(G_LOG_DOMAIN,               \
                                 __PRETTY_FUNCTION__,        \
                                 curl_multi_strerror(code)); \
        return;                                              \
    }

# define OPENVRML_PLAYER_CURL_MULTI_RETURN_VAL_IF_ERROR(code, val) \
    if G_LIKELY((code) == CURLM_OK) {} else {                      \
        g_return_if_fail_warning(G_LOG_DOMAIN,                     \
                                 __PRETTY_FUNCTION__,              \
                                 curl_multi_strerror(code));       \
        return (val);                                              \
    }

int main(int argc, char * argv[])
{
    using std::string;
    using std::vector;
    using boost::lexical_cast;
    using boost::ref;

    g_set_application_name(app_name);

    if (!gnome_vfs_init()) {
        g_critical("GnomeVFS initialization failed");
        return EXIT_FAILURE;
    }

    if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
        g_critical("libcurl initialization failed");
        return EXIT_FAILURE;
    }
    scope_guard curl_global_guard = make_guard(curl_global_cleanup);
    boost::ignore_unused_variable_warning(curl_global_guard);

    gchar ** remaining_args = 0;
    GOptionEntry option_entries[] = {
        {
            G_OPTION_REMAINING,
            '\0',
            0,
            G_OPTION_ARG_FILENAME_ARRAY,
            &remaining_args,
            "a URI for a VRML or X3D world",
            "[URI]"
        },
        { 0, '\0', 0, G_OPTION_ARG_NONE, 0, 0, 0 }
    };

    GOptionContext * const option_context =
        g_option_context_new("- render VRML/X3D worlds");
    static const gchar * const translation_domain = 0;
    g_option_context_add_main_entries(option_context,
                                      option_entries,
                                      translation_domain);

    GnomeProgram * const program =
        gnome_program_init(app_id,
                           PACKAGE_VERSION,
                           LIBGNOMEUI_MODULE,
                           argc,
                           argv,
                           GNOME_PARAM_GOPTION_CONTEXT, option_context,
                           GNOME_PARAM_NONE);

    glade_init();
    glade_register_widget(OPENVRML_PLAYER_TYPE_FILE_CHOOSER_DIALOG,
                          openvrml_player_build_file_chooser_dialog,
                          0,
                          0);

    GladeXML * const xml = xml_new(*program);
    glade_xml_signal_autoconnect(xml);

    GtkWidget * const app_window = glade_xml_get_widget(xml, "window");
    GtkWidget * const file_chooser_dialog =
        glade_xml_get_widget(xml, "filechooserdialog");
    gtk_window_set_transient_for(GTK_WINDOW(file_chooser_dialog),
                                 GTK_WINDOW(app_window));

    GtkWidget * const location_entry =
        glade_xml_get_widget(xml, "locationentry");
    g_signal_connect(file_chooser_dialog,
                     "response",
                     G_CALLBACK(openvrml_player_on_filechooserdialog_response),
                     location_entry);

    //
    // The OPENVRML_XEMBED environment variable overrides the default
    // path to the child process executable.  To allow OPENVRML_XEMBED
    // to include arguments (rather than just be a path to an
    // executable), it is parsed with g_shell_parse_argv.  This is
    // particularly useful in case we want to run the child process in
    // a harness like valgrind.
    //
    gint openvrml_xembed_cmd_argc = 0;
    gchar ** openvrml_xembed_cmd_argv = 0;
    scope_guard openvrml_xembed_cmd_argv_guard =
        make_guard(g_strfreev, ref(openvrml_xembed_cmd_argv));
    boost::ignore_unused_variable_warning(openvrml_xembed_cmd_argv_guard);
    const gchar * const openvrml_xembed_cmd = g_getenv("OPENVRML_XEMBED");
    if (!openvrml_xembed_cmd) {
        openvrml_xembed_cmd_argc = 1;
        openvrml_xembed_cmd_argv =
            static_cast<gchar **>(g_malloc0(sizeof (gchar *) * 2));
        if (!openvrml_xembed_cmd_argv) { throw std::bad_alloc(); }
        openvrml_xembed_cmd_argv[0] =
            g_strdup(OPENVRML_LIBEXECDIR_ "/openvrml-xembed");
        if (!openvrml_xembed_cmd_argv[0]) { throw std::bad_alloc(); }
    } else {
        GError * error = 0;
        scope_guard error_guard = make_guard(g_error_free, ref(error));
        gboolean succeeded =
            g_shell_parse_argv(openvrml_xembed_cmd,
                               &openvrml_xembed_cmd_argc,
                               &openvrml_xembed_cmd_argv,
                               &error);
        if (!succeeded) {
            throw std::runtime_error(error
                                     ? error->message
                                     : "g_shell_parse_argv failure");
        }
        error_guard.dismiss();
    }

    GtkWidget * const socket = glade_xml_get_widget(xml, "socket");
    string socket_id_arg =
        lexical_cast<string>(gtk_socket_get_id(GTK_SOCKET(socket)));
    const char * socket_id_arg_c_str = socket_id_arg.c_str();
    vector<char> socket_id_arg_vec(
        socket_id_arg_c_str,
        socket_id_arg_c_str + socket_id_arg.length() + 1);

    const gint child_argv_size = openvrml_xembed_cmd_argc + 2;
    gchar ** const child_argv =
        static_cast<gchar **>(g_malloc(sizeof (gchar *) * child_argv_size));
    if (!argv) { throw std::bad_alloc(); }
    scope_guard child_argv_guard = make_guard(g_free, child_argv);
    boost::ignore_unused_variable_warning(child_argv_guard);
    gint i;
    for (i = 0; i < openvrml_xembed_cmd_argc; ++i) {
        child_argv[i] = openvrml_xembed_cmd_argv[i];
    }
    child_argv[i++] = &socket_id_arg_vec.front();
    child_argv[i]   = 0;

    gchar * const working_dir = g_get_current_dir();
    if (!working_dir) { throw std::bad_alloc(); };
    scope_guard working_dir_guard = make_guard(g_free, working_dir);
    boost::ignore_unused_variable_warning(working_dir_guard);

    gchar ** envp = 0;
    GPid child_pid;
    gint standard_input, standard_output;
    gint * const standard_error = 0;
    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, ref(error));
    gboolean succeeded = g_spawn_async_with_pipes(working_dir,
                                                  child_argv,
                                                  envp,
                                                  G_SPAWN_DO_NOT_REAP_CHILD,
                                                  0,
                                                  0,
                                                  &child_pid,
                                                  &standard_input,
                                                  &standard_output,
                                                  standard_error,
                                                  &error);
    if (!succeeded) {
        throw std::runtime_error(error
                                 ? error->message
                                 : "g_spawn_async_with_pipes failure");
    }

    g_child_watch_add(child_pid, openvrml_player_watch_child, 0);

    //
    // Don't dismiss "error_guard" yet; we reuse "error" below.
    //

    ::command_channel = g_io_channel_unix_new(standard_input);
    if (!::command_channel) { throw std::bad_alloc(); }
    GIOStatus status = g_io_channel_set_encoding(::command_channel,
                                                 0, // binary (no encoding)
                                                 &error);
    if (status != G_IO_STATUS_NORMAL) {
        throw std::runtime_error(error
                                 ? error->message
                                 : "g_io_channel_set_encoding failure");
    }
    error_guard.dismiss();

    ::request_channel = g_io_channel_unix_new(standard_output);
    if (!::request_channel) { throw std::bad_alloc(); }

    CURLM * const multi_handle = curl_multi_init();
    g_return_val_if_fail(multi_handle, EXIT_FAILURE);
    scope_guard multi_handle_guard = make_guard(curl_multi_cleanup,
                                                multi_handle);
    boost::ignore_unused_variable_warning(multi_handle_guard);

    GSource * const curl_source = curl_source_new(multi_handle);
    scope_guard curl_source_guard = make_guard(g_source_unref, curl_source);
    boost::ignore_unused_variable_warning(curl_source_guard);

    curl_source_callback_data callback_data;
    callback_data.curl_source = curl_source;
    g_source_set_callback(curl_source,
                          openvrml_player_curl_source_callback,
                          &callback_data,
                          0);
    g_source_attach(curl_source, 0);

    request_data req_data;
    req_data.source_callback_data = &callback_data;
    ::request_channel_watch_id =
        g_io_add_watch(::request_channel,
                       G_IO_IN,
                       openvrml_player_request_data_available,
                       &req_data);

    gtk_widget_show(app_window);

    gtk_main();
}

namespace {

    GladeXML * xml_new(GnomeProgram & program)
    {
        using std::string;

        const gchar * const gladedir = g_getenv("OPENVRML_PLAYER_GLADEDIR");
        string glade_file_str;
        const gchar * glade_file = 0;
        if (gladedir) {
            glade_file_str = string(gladedir) + "/openvrml-player.glade";
            glade_file = glade_file_str.c_str();
        }

        if (!glade_file) {
            static const gboolean only_if_exists = true;
            glade_file =
                gnome_program_locate_file(&program,
                                          GNOME_FILE_DOMAIN_APP_DATADIR,
                                          "/glade/openvrml-player.glade",
                                          only_if_exists,
                                          0);
        }

        g_return_val_if_fail(glade_file, 0);

        return glade_xml_new(glade_file, 0, 0);
    }

    G_GNUC_INTERNAL void load_url(const gchar * url)
    {
        std::ostringstream command;
        command << "load-url " << url << std::endl;
        ::write_command(command.str());
    }

    typedef std::list<GPollFD> poll_fds_t;

    struct CURLSource {
        GSource source;
        CURLM * multi_handle;
        size_t outstanding_handles;
        fd_set read_fds;
        fd_set write_fds;
        fd_set exc_fds;
        int max_fd;
        poll_fds_t * poll_fds;
    };

    GSource * curl_source_new(CURLM * const multi_handle)
    {
        g_return_val_if_fail(multi_handle, 0);

        static GSourceFuncs curl_source_funcs = {
            openvrml_player_curl_prepare,
            openvrml_player_curl_check,
            openvrml_player_curl_dispatch,
            openvrml_player_curl_finalize
        };

        CURLSource * const source =
            static_cast<CURLSource *>(
                static_cast<void *>(
                    g_source_new(&curl_source_funcs, sizeof (CURLSource))));

        source->multi_handle        = multi_handle;
        source->outstanding_handles = 0;
        FD_ZERO(&source->read_fds);
        FD_ZERO(&source->write_fds);
        FD_ZERO(&source->exc_fds);
        source->max_fd              = -1;
        source->poll_fds            = new poll_fds_t;

        return static_cast<GSource *>(static_cast<void *>(source));
    }

    G_GNUC_INTERNAL void reset_fds(CURLSource & curl_source)
    {
        FD_ZERO(&curl_source.read_fds);
        FD_ZERO(&curl_source.write_fds);
        FD_ZERO(&curl_source.exc_fds);
        curl_source.max_fd = -1;

        const CURLMcode result = curl_multi_fdset(curl_source.multi_handle,
                                                  &curl_source.read_fds,
                                                  &curl_source.write_fds,
                                                  &curl_source.exc_fds,
                                                  &curl_source.max_fd);
        OPENVRML_PLAYER_CURL_MULTI_RETURN_IF_ERROR(result);

        if (curl_source.max_fd >= 0) {
            //
            // Resize the list of fds to be polled; initialize any new
            // entries.
            //
            const poll_fds_t::size_type prev_size =
                curl_source.poll_fds->size();
            curl_source.poll_fds->resize(curl_source.max_fd + 1);
            if (curl_source.poll_fds->size() > prev_size) {
                poll_fds_t::iterator pos = curl_source.poll_fds->begin();
                std::advance(pos, prev_size);
                for (gint fd = prev_size; pos != curl_source.poll_fds->end();
                     ++pos, ++fd) {
                    pos->fd      = fd;
                    pos->events  = 0;
                    pos->revents = 0;
                }
            }
        }

        for (poll_fds_t::iterator pos = curl_source.poll_fds->begin();
             pos != curl_source.poll_fds->end();
             ++pos) {
            gushort events = 0;
            if (FD_ISSET(pos->fd, &curl_source.read_fds)) {
                events |= G_IO_IN | G_IO_HUP | G_IO_ERR;
            }
            if (FD_ISSET(pos->fd, &curl_source.write_fds)) {
                events |= G_IO_OUT | G_IO_ERR;
            }
            if (FD_ISSET(pos->fd, &curl_source.exc_fds)) {
                events |= G_IO_ERR;
            }

            //
            // No change.
            //
            if (events == pos->events) { continue; }

            //
            // Changed; but already in the list of fds to poll.  Just update
            // the event flags.
            //
            if (events && pos->events) {
                pos->events = events;
                continue;
            }

            pos->events = events;

            if (events) {
                g_source_add_poll(&curl_source.source, &(*pos));
            } else {
                g_source_remove_poll(&curl_source.source, &(*pos));
                pos->revents = 0;
            }
        }
    }
}

gboolean openvrml_player_curl_prepare(GSource * const source,
                                      gint * const timeout_)
{
    CURLSource * const curl_source =
        static_cast<CURLSource *>(static_cast<void *>(source));

    reset_fds(*curl_source);

    long t;
    *timeout_ = (curl_multi_timeout(curl_source->multi_handle, &t) == CURLM_OK)
              ? gint(t)
              : -1;
    return *timeout_ != -1;
}

gboolean openvrml_player_curl_check(GSource * const source)
{
    CURLSource * const curl_source =
        static_cast<CURLSource *>(static_cast<void *>(source));

    FD_ZERO(&curl_source->read_fds);
    FD_ZERO(&curl_source->write_fds);
    FD_ZERO(&curl_source->exc_fds);

    bool events_pending = false;
    for (poll_fds_t::const_iterator pos = curl_source->poll_fds->begin();
         pos != curl_source->poll_fds->end();
         ++pos) {
        gushort revents = pos->revents;
        if (revents == 0) { continue; }

        events_pending = true;

        if (revents & (G_IO_IN | G_IO_PRI)) {
            FD_SET(pos->fd, &curl_source->read_fds);
        }
        if (revents & G_IO_OUT) {
            FD_SET(pos->fd, &curl_source->write_fds);
        }
        if (revents & (G_IO_ERR | G_IO_HUP)) {
            FD_SET(pos->fd, &curl_source->exc_fds);
        }
    }

    const bool ready_for_dispatch =
        events_pending || curl_source->outstanding_handles > 0;
    return ready_for_dispatch;
}

gboolean openvrml_player_curl_dispatch(GSource * const source,
                                       const GSourceFunc callback,
                                       const gpointer user_data)
{
    CURLSource * const curl_source =
        static_cast<CURLSource *>(static_cast<void *>(source));
    int running_handles;
    CURLMcode perform_result;
    do {
        perform_result = curl_multi_perform(curl_source->multi_handle,
                                            &running_handles);
    } while (perform_result == CURLM_CALL_MULTI_PERFORM);

    if (running_handles == 0) { reset_fds(*curl_source); }

    if (callback) { (*callback)(user_data); }

    return true;
}

void openvrml_player_curl_finalize(GSource * const source)
{
    CURLSource * const curl_source =
        static_cast<CURLSource *>(static_cast<void *>(source));
    reset_fds(*curl_source);
    delete curl_source->poll_fds;
}

gboolean openvrml_player_curl_source_callback(const gpointer data)
{
    curl_source_callback_data * const callback_data =
        static_cast<curl_source_callback_data *>(data);
    CURLSource * const curl_source =
        static_cast<CURLSource *>(
            static_cast<void *>(callback_data->curl_source));

    CURLMsg * msg;
    int msgs_in_queue;
    while ((msg = curl_multi_info_read(curl_source->multi_handle,
                                       &msgs_in_queue))) {
        if (msg->msg == CURLMSG_DONE) {
            const stream_data_map_t::size_type num_erased =
                callback_data->stream_data_map.erase(msg->easy_handle);
            g_assert(num_erased == 1);
            g_assert(curl_source->outstanding_handles > 0);
            --curl_source->outstanding_handles;
            std::ostringstream command;
            command << "destroy-stream " << ptrdiff_t(msg->easy_handle)
                    << '\n';
            const ssize_t bytes_written = ::write_command(command.str());
            g_return_val_if_fail(
                bytes_written == ssize_t(command.str().length()),
                false);

            //
            // Note that the call to curl_multi_remove_handle invalidates the
            // data pointed to by msg.  Save the easy_handle in a temporary
            // variable so we can use it with curl_easy_cleanup below.
            //
            CURL * const easy_handle = msg->easy_handle;
            const CURLMcode multi_remove_result =
                curl_multi_remove_handle(curl_source->multi_handle,
                                         easy_handle);
            OPENVRML_PLAYER_CURL_MULTI_RETURN_VAL_IF_ERROR(multi_remove_result,
                                                           false);
            //
            // If we get an error from curl_multi_remove_handle, the cleanup
            // won't happen; but that's probably safer than trying to go ahead
            // and do the cleanup in that case.
            //
            curl_easy_cleanup(easy_handle);
        }
    }
    return true;
}

gboolean openvrml_player_request_data_available(GIOChannel * const source,
                                                GIOCondition,
                                                const gpointer data)
{
    using std::string;
    using std::vector;
    using boost::ref;

    request_data & req_data = *static_cast<request_data *>(data);

    gchar c;
    do {
        gsize bytes_read;
        GError * error = 0;
        scope_guard error_guard = make_guard(g_error_free, ref(error));
        const GIOStatus status =
            g_io_channel_read_chars(source, &c, 1, &bytes_read, &error);
        if (status == G_IO_STATUS_ERROR) {
            if (error) { g_warning(error->message); }
            return false;
        }
        if (status == G_IO_STATUS_EOF) { return false; }
        if (status == G_IO_STATUS_AGAIN) { continue; }
        g_return_val_if_fail(status == G_IO_STATUS_NORMAL, false);

        g_assert(bytes_read == 1);

        if (c != '\n') { req_data.request_line.put(c); }
        error_guard.dismiss();
    } while (g_io_channel_get_buffer_condition(source) & G_IO_IN);

    if (c == '\n') {
        string request_type;
        req_data.request_line >> request_type;
        if (request_type == "get-url") {
            typedef std::map<CURL *, curl_stream_data> stream_data_map_t;
            string url, target;
            req_data.request_line >> url >> target;
            CURL * const handle = curl_easy_init();
            g_assert(req_data.source_callback_data);
            stream_data_map_t::iterator pos =
                req_data.source_callback_data->stream_data_map.insert(
                    std::make_pair(handle,
                                   curl_stream_data(handle, url.c_str())))
                .first;
            g_assert(
                pos != req_data.source_callback_data->stream_data_map.end());
            curl_stream_data & stream_data = pos->second;
            CURLcode setopt_result;
            setopt_result = curl_easy_setopt(handle,
                                             CURLOPT_FAILONERROR, true);
            OPENVRML_PLAYER_CURL_EASY_RETURN_VAL_IF_ERROR(setopt_result,
                                                          false);
            setopt_result = curl_easy_setopt(handle,
                                             CURLOPT_URL, stream_data.url());
            OPENVRML_PLAYER_CURL_EASY_RETURN_VAL_IF_ERROR(setopt_result,
                                                          false);

            setopt_result = curl_easy_setopt(handle,
                                             CURLOPT_WRITEFUNCTION,
                                             openvrml_player_curl_write);
            OPENVRML_PLAYER_CURL_EASY_RETURN_VAL_IF_ERROR(setopt_result,
                                                          false);

            setopt_result = curl_easy_setopt(handle,
                                             CURLOPT_WRITEDATA, &stream_data);
            OPENVRML_PLAYER_CURL_EASY_RETURN_VAL_IF_ERROR(setopt_result,
                                                          false);

            CURLSource * const curl_source =
                static_cast<CURLSource *>(
                    static_cast<void *>(
                        req_data.source_callback_data->curl_source));

            const CURLMcode add_handle_result =
                curl_multi_add_handle(curl_source->multi_handle, handle);
            OPENVRML_PLAYER_CURL_MULTI_RETURN_VAL_IF_ERROR(add_handle_result,
                                                           false);
            ++curl_source->outstanding_handles;
            std::ostringstream command;
            command << "get-url-result " << url << ' ' << 0 << '\n';
            const ssize_t bytes_written = write_command(command.str());
            g_return_val_if_fail(
                bytes_written == ssize_t(command.str().length()),
                false);
            if (req_data.source_callback_data->stream_data_map.size() == 1) {
                int running_handles;
                CURLMcode perform_result;
                do {
                    perform_result =
                        curl_multi_perform(curl_source->multi_handle,
                                           &running_handles);
                } while (perform_result == CURLM_CALL_MULTI_PERFORM);
            }
        }

        req_data.request_line.clear();
    }

    return true;
}

size_t openvrml_player_curl_write(void * const ptr,
                                  const size_t size,
                                  const size_t nmemb,
                                  void * const stream)
{
    curl_stream_data & stream_data = *static_cast<curl_stream_data *>(stream);

    if (!stream_data.initialized()) {
        using boost::ref;

        const char * type = 0;
        CURLcode getinfo_result =
            curl_easy_getinfo(stream_data.handle(),
                              CURLINFO_CONTENT_TYPE, &type);
        OPENVRML_PLAYER_CURL_EASY_RETURN_VAL_IF_ERROR(getinfo_result, 0);

        GnomeVFSFileInfo * info = 0;
        scope_guard info_guard = make_guard(gnome_vfs_file_info_unref,
                                            ref(info));
        if (!type) {
            info = gnome_vfs_file_info_new();
            GnomeVFSResult get_file_info_result =
                gnome_vfs_get_file_info(stream_data.url(),
                                        info,
                                        GNOME_VFS_FILE_INFO_GET_MIME_TYPE);
            if (get_file_info_result != GNOME_VFS_OK) {
                g_critical("%s",
                           gnome_vfs_result_to_string(get_file_info_result));
            }
            type = gnome_vfs_file_info_get_mime_type(info);
        } else {
            info_guard.dismiss();
        }

        std::ostringstream command;
        command << "new-stream " << ptrdiff_t(stream_data.handle()) << ' '
                << (type ? type : "application/octet-stream") << ' '
                << stream_data.url() << '\n';
        write_command(command.str());
        stream_data.initialize();
    }

    std::ostringstream command;
    command << "write " << ptrdiff_t(stream_data.handle()) << ' '
            << size * nmemb << '\n';
    for (const char * data = static_cast<char *>(ptr);
         data != static_cast<char *>(ptr) + size * nmemb;
         ++data) {
        command.put(*data);
    }

    const ssize_t bytes_written = ::write_command(command.str());
    g_assert(bytes_written == ssize_t(command.str().length()));

    return size_t(data - static_cast<char *>(ptr));
}

void openvrml_player_on_about_activated(GtkWindow * const parent)
{
    const gchar website[] = "http://openvrml.org";
    const gchar * authors[] = {
        "Braden McDaniel <braden@endoframe.com>",
        0
    };
    const gchar copyright[] = "Copyright 2006";
    const gchar license[] =
        "This program is free software; you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by "
        "the Free Software Foundation; either version 2 of the License, or "
        "(at your option) any later version.\n\n"

        "This program is distributed in the hope that it will be useful, but "
        "WITHOUT ANY WARRANTY; without even the implied warranty of "
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU "
        "General Public License for more details.\n\n"

        "You should have received a copy of the GNU General Public License "
        "along with this program; if not, write to the Free Software "
        "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 "
        "USA";

    gtk_show_about_dialog(parent,
                          "name", app_name,
                          "version", PACKAGE_VERSION,
                          "website", website,
                          "authors", authors,
                          "copyright", copyright,
                          "license", license,
                          "wrap-license", true,
                          NULL);
}

void
openvrml_player_on_file_open_activated(
    OpenvrmlPlayerFileChooserDialog * const dialog)
{
    const gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        gchar * uri = 0;
        uri = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog));
        g_return_if_fail(uri);
        scope_guard uri_guard = make_guard(g_free, uri);
        boost::ignore_unused_variable_warning(uri_guard);
        ::load_url(uri);
    }

    gtk_widget_hide(GTK_WIDGET(dialog));
}

void openvrml_player_on_locationentry_activated(GtkEntry * const entry)
{
    const gchar * const uri = gtk_entry_get_text(entry);
    ::load_url(uri);
}

void openvrml_player_on_filechooserdialog_response(GtkDialog * const dialog,
                                                   const gint arg1,
                                                   const gpointer user_data)
{
    if (arg1 == GTK_RESPONSE_ACCEPT) {
        GtkEntry * const location_entry = static_cast<GtkEntry *>(user_data);
        gchar * uri = 0;
        uri = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog));
        g_return_if_fail(uri);
        scope_guard uri_guard = make_guard(g_free, uri);
        boost::ignore_unused_variable_warning(uri_guard);
        gtk_entry_set_text(location_entry, uri);
    }
}

//
// Start the quit dance.  We just shut down the I/O channels here.  When the
// child process gets EOF on the request_channel, it will shut down.  The
// openvrml-player process (us) ultimately shuts down in the watch_child
// callback.
//
void openvrml_player_quit()
{
    gboolean succeeded = g_source_remove(::request_channel_watch_id);
    g_assert(succeeded);

    GError * error = 0;
    const gboolean flush = false;
    GIOStatus status = g_io_channel_shutdown(::request_channel, flush, &error);
    if (status != G_IO_STATUS_NORMAL) {
        if (error) {
            g_critical(error->message);
            g_error_free(error);
        }
    }
    g_io_channel_unref(::request_channel);

    error = 0;
    status = g_io_channel_shutdown(::command_channel, flush, &error);
    if (status != G_IO_STATUS_NORMAL) {
        if (error) {
            g_critical(error->message);
            g_error_free(error);
        }
    }
    g_io_channel_unref(::command_channel);
}

//
// Once the child process has terminated, it's time for us to go.
//
void openvrml_player_watch_child(const GPid pid,
                                 gint /* status */,
                                 gpointer /* data */)
{
    g_spawn_close_pid(pid);
    gtk_main_quit();
}

GtkWidget *
openvrml_player_build_file_chooser_dialog(GladeXML * /* xml */,
                                          GType /* widget_type */,
                                          GladeWidgetInfo * /* info */)
{
    GtkWidget * widget = openvrml_player_file_chooser_dialog_new(0);
    return widget;
}

namespace {

    ssize_t write_command(const std::string & command)
    {
        g_assert(::command_channel);

        gsize bytes_written;
        GError * error = 0;
        GIOStatus status = g_io_channel_write_chars(::command_channel,
                                                    command.data(),
                                                    command.length(),
                                                    &bytes_written,
                                                    &error);
        if (status != G_IO_STATUS_NORMAL) {
            if (error) {
                g_critical(error->message);
                g_error_free(error);
            }
            return -1;
        }

        do {
            status = g_io_channel_flush(::command_channel, &error);
        } while (status == G_IO_STATUS_AGAIN);

        if (status != G_IO_STATUS_NORMAL) {
            if (error) {
                g_critical(error->message);
                g_error_free(error);
            }
            return -1;
        }

        return bytes_written;
    }

    curl_stream_data::curl_stream_data(CURL * const handle,
                                       const char * const url):
        handle_(handle),
        url_(url, url + strlen(url) + 1), // Get the trailing null.
        initialized_(false)
    {}

    CURL * curl_stream_data::handle() const
    {
        return this->handle_;
    }

    const char * curl_stream_data::url() const
    {
        return &this->url_.front();
    }

    void curl_stream_data::initialize()
    {
        this->initialized_ = true;
    }

    bool curl_stream_data::initialized() const
    {
        return this->initialized_;
    }
} // namespace
