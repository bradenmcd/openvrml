// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML Mozilla plug-in
// Copyright 2004, 2005, 2006  Braden N. McDaniel
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

# include <sstream>
# include <boost/thread/thread.hpp>
# include <unistd.h>
// Must include before X11 headers.
# include <boost/numeric/conversion/converter.hpp>
# include <gtk/gtk.h>
# include <openvrml/browser.h>

# include "gtkvrmlbrowser.h"
# include "command_istream.h"
# include "plugin_streambuf.h"
# include "flag.h"

using namespace openvrml_player;

namespace {

    const char application_name[] = "OpenVRML Player";

    //
    // We don't already know what the URI of the initial stream is until we
    // start getting that data from the browser.  This is a placeholder that
    // is used to identify the plugin_streambuf that will be used to receive
    // the initial stream data.
    //
    const char initial_stream_uri[] = "x-openvrml-initial:";

    //
    // Used to signal the various threads that we're done.  "quit" is
    // initiated when the command_istream gets EOF.  At that point, the
    // command_istream_reader thread sets quit_flag to true.  This flag is
    // checked by the command_channel_loop thread and the main (rendering)
    // thread, signaling them to terminate.
    //
    flag quit_flag;
}

namespace openvrml_player {

    struct command_istream_reader {
        explicit command_istream_reader(command_istream & in):
            in_(&in)
        {}

        void operator()() const throw ()
        {
            using std::string;

            string command_line;
            while (getline(*this->in_, command_line)) {
                using std::istringstream;

                istringstream command_line_stream(command_line);
                string command;
                command_line_stream >> command;
                if (command == "new-stream") {
                    using boost::shared_ptr;

                    size_t stream_id;
                    std::string type, url;
                    command_line_stream >> stream_id >> type >> url;

                    shared_ptr<plugin_streambuf> streambuf =
                        uninitialized_plugin_streambuf_map_.find(url);

                    static bool got_initial_stream = false;
                    if (!streambuf) {
                        if (!got_initial_stream) {
                            g_assert(uninitialized_plugin_streambuf_map_.size()
                                     == 1);
                            streambuf =
                                uninitialized_plugin_streambuf_map_.front();
                            got_initial_stream = true;
                        } else {
                            g_warning("Attempt to create an unrequested "
                                      "stream.");
                            continue;
                        }
                    }
                    streambuf->init(stream_id, url, type);
                } else if (command == "destroy-stream") {
                    size_t stream_id;
                    command_line_stream >> stream_id;
                    plugin_streambuf_map_t::iterator pos = 
                        plugin_streambuf_map.find(stream_id);
                    if (pos == plugin_streambuf_map.end()) {
                        g_warning("Attempt to destroy a nonexistent stream.");
                        continue;
                    }
                    pos->second->buf_.put(std::char_traits<char>::eof());
                    pos->second->buf_.npstream_destroyed();
                    plugin_streambuf_map.erase(pos);
                } else if (command == "write") {
                    size_t stream_id, offset, length;
                    command_line_stream >> stream_id >> offset >> length;
                    plugin_streambuf_map_t::const_iterator pos =
                        plugin_streambuf_map.find(stream_id);
                    if (pos == plugin_streambuf_map.end()) {
                        g_warning("Attempt to write to a nonexistent stream.");
                        continue;
                    }
                    for (size_t i = 0; i < length; ++i) {
                        pos->second->buf_.put(this->in_->get());
                    }
                }
            }

            //
            // Got EOF from the command stream. Time to shut down.
            //
            ::quit_flag.value(true);

            //
            // We can safely shut down the Gtk main loop from here.
            //
            // (However, trying to shut down the command_main loop results in
            // it deadlocking in g_main_context_check's call to poll. So we
            // still use the annoying quit_flag condition variable (see above)
            // to end that thread).
            //
            gtk_main_quit();
        }

    private:
        command_istream * in_;
    };
}

extern "C" gboolean
openvrml_player_command_channel_loop_quit_event(gpointer data);

namespace {

    GdkNativeWindow socket_id;

    GOptionEntry options[] = {
        {
            "gtk-socket-id",
            0,
            0,
            G_OPTION_ARG_INT,
            &socket_id,
            "GtkSocket id",
            "GTK_SOCKET_ID"
        },
        { 0, 0, 0, G_OPTION_ARG_NONE, 0, 0, 0 }
    };

    struct initial_stream_reader {
        initial_stream_reader(
            const boost::shared_ptr<plugin_streambuf> & streambuf,
            GtkVrmlBrowser & vrml_browser):
            streambuf_(streambuf),
            vrml_browser_(&vrml_browser)
        {}

        void operator()() const throw ()
        {
            class plugin_istream : public openvrml::resource_istream {
                boost::shared_ptr<plugin_streambuf> streambuf_;

            public:
                explicit plugin_istream(
                    const boost::shared_ptr<plugin_streambuf> & streambuf):
                    openvrml::resource_istream(streambuf.get()),
                    streambuf_(streambuf)
                {}

                virtual ~plugin_istream() throw ()
                {}

            private:
                virtual const std::string do_url() const throw (std::bad_alloc)
                {
                    return this->streambuf_->url();
                }

                virtual const std::string do_type() const
                    throw (std::bad_alloc)
                {
                    return this->streambuf_->type();
                }

                virtual bool do_data_available() const throw ()
                {
                    return this->streambuf_->data_available();
                }
            } in(this->streambuf_);
            gtk_vrml_browser_set_world(this->vrml_browser_, in);
        }

    private:
        boost::shared_ptr<plugin_streambuf> streambuf_;
        GtkVrmlBrowser * vrml_browser_;
    };

    GIOChannel * request_channel;

    struct command_channel_loop {
        explicit command_channel_loop(GMainLoop & main_loop):
            main_loop_(&main_loop)
        {}

        void operator()() const throw ()
        {
            GSource * const quit = g_idle_source_new();
            const GDestroyNotify notify = 0;
            g_source_set_callback(
                quit,
                ::openvrml_player_command_channel_loop_quit_event,
                this->main_loop_,
                notify);
            guint source_id =
                g_source_attach(quit,
                                g_main_loop_get_context(this->main_loop_));
            g_return_if_fail(source_id != 0);

            g_main_loop_run(this->main_loop_);
        }

    private:
        GMainLoop * main_loop_;
    };
}

int main(int argc, char * argv[])
{
    using std::string;
    using std::vector;
    using boost::function0;
    using boost::shared_ptr;
    using boost::thread;
    using boost::thread_group;

    using namespace openvrml_player;

    g_set_application_name(application_name);

    gtk_init(&argc, &argv);
    gtk_gl_init(&argc, &argv);

    GError * error = 0;

    GOptionContext * const context =
        g_option_context_new("- render VRML worlds");
    const gchar * const translation_domain = 0;
    g_option_context_add_main_entries(context, options, translation_domain);
    g_option_context_add_group(context, gtk_get_option_group(true));
    gboolean succeeded = g_option_context_parse(context, &argc, &argv, &error);
    if (!succeeded) {
        if (error) {
            g_critical(error->message);
            g_error_free(error);
        }
        return EXIT_FAILURE;        
    }

    command_istream command_in;

    ::request_channel = g_io_channel_unix_new(1); // stdout

    GtkWidget * window = socket_id
        ? gtk_plug_new(socket_id)
        : gtk_window_new(GTK_WINDOW_TOPLEVEL);

    GtkWidget * const vrml_browser = gtk_vrml_browser_new(::request_channel);
    gtk_container_add(GTK_CONTAINER(window), vrml_browser);    

    gtk_widget_show_all(window);

    thread_group threads;

    GMainContext * command_main_context = 0;
    GMainLoop * command_main = 0;
    GIOChannel * command_channel = 0;
    GSource * command_watch = 0;

    if (argc > 1) {
        const gchar * url[] = { argv[1], 0 };
        const gchar ** parameter = 0;
        gtk_vrml_browser_load_url(GTK_VRML_BROWSER(vrml_browser),
                                  url,
                                  parameter);
    } else {
        command_main_context = g_main_context_new();
        command_main = g_main_loop_new(command_main_context, false);
        command_channel = g_io_channel_unix_new(0); // stdin
        GError * error = 0;
        GIOStatus status = g_io_channel_set_encoding(command_channel,
                                                     0, // binary (no encoding)
                                                     &error);
        if (status != G_IO_STATUS_NORMAL) {
            if (error) {
                g_critical(error->message);
                g_error_free(error);
            }
            return EXIT_FAILURE;
        }

        command_watch = g_io_create_watch(command_channel,
                                          GIOCondition(G_IO_IN | G_IO_HUP));
        const GDestroyNotify notify = 0;
        g_source_set_callback(
            command_watch,
            reinterpret_cast<GSourceFunc>(::command_data_available),
            static_cast<command_streambuf *>(command_in.rdbuf()), notify);
        guint source_id = g_source_attach(command_watch,
                                          command_main_context);
        g_return_val_if_fail(source_id != 0, EXIT_FAILURE);

        function0<void> command_channel_loop_func =
            command_channel_loop(*command_main);
        threads.create_thread(command_channel_loop_func);
        
        shared_ptr<plugin_streambuf> initial_stream(
            new plugin_streambuf(initial_stream_uri));
        bool succeeded =
            uninitialized_plugin_streambuf_map_.insert(initial_stream_uri,
                                                       initial_stream);
        g_return_val_if_fail(succeeded, EXIT_FAILURE);
        function0<void> initial_stream_reader_func =
            initial_stream_reader(initial_stream,
                                  *GTK_VRML_BROWSER(vrml_browser));
        threads.create_thread(initial_stream_reader_func);
    }

    function0<void> read_commands = command_istream_reader(command_in);
    threads.create_thread(read_commands);

    gtk_main();

    threads.join_all();

    g_source_unref(command_watch);

    GIOStatus status = g_io_channel_shutdown(command_channel, true, &error);
    if (status != G_IO_STATUS_NORMAL) {
        if (error) {
            g_critical(error->message);
            g_error_free(error);
        }
    }
    g_io_channel_unref(command_channel);

    g_main_loop_unref(command_main);
    g_main_context_unref(command_main_context);

    if (::request_channel) {
        GError * error = 0;
        const gboolean flush = false;
        GIOStatus status = g_io_channel_shutdown(::request_channel,
                                                 flush,
                                                 &error);
        if (status != G_IO_STATUS_NORMAL) {
            if (error) {
                g_critical(error->message);
                g_error_free(error);
            }
        }
    }
    g_io_channel_unref(::request_channel);
}

gboolean openvrml_player_command_channel_loop_quit_event(const gpointer data)
{
    GMainLoop * const main_loop = static_cast<GMainLoop *>(data);

    if (::quit_flag.value()) {
        g_main_loop_quit(main_loop);
        return false;
    }
    return true;
}
