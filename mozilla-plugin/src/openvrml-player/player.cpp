// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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

# include <cstdlib>
# include <boost/lexical_cast.hpp>
# include <boost/thread/thread.hpp>
# include <unistd.h>
// Must include before X11 headers.
# include <boost/numeric/conversion/converter.hpp>
# include <gdk/gdkx.h>
# include <argp.h>

# include "browser.h"
# include "gtkglviewer.h"
# include "command_istream.h"
# include "plugin_streambuf.h"

using namespace openvrml_player;

namespace {

    const char application_name[] = "OpenVRML Player";
}

extern "C" {
    const char * argp_program_version = "OpenVRML Player " PACKAGE_VERSION;
    const char * argp_program_bug_address = PACKAGE_BUGREPORT;

    error_t parse_opt(int key, char * arg, argp_state * state);
}

namespace openvrml_player {

    struct command_istream_reader {
        explicit command_istream_reader(command_istream & in,
                                        openvrml::browser & browser):
            in_(&in),
            browser_(&browser)
        {}

        void operator()() const
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
                        uninitialized_plugin_streambuf_set_.find(url);

                    if (!streambuf) {
                        //
                        // If the world_url is empty, this is the primordial
                        // stream.
                        //
                        if (this->browser_->world_url().empty()) {
                            g_assert(uninitialized_plugin_streambuf_set_.size()
                                     == 1);
                            streambuf =
                                uninitialized_plugin_streambuf_set_.front();
                            this->browser_->world_url(url);
                        } else {
                            g_warning("Attempt to create an unrequested "
                                      "stream.");
                            continue;
                        }
                    }
                    streambuf->init(type);
                    bool succeeded = ::plugin_streambuf_map
                        .insert(make_pair(stream_id, streambuf)).second;
                    g_assert(succeeded);
                    succeeded =
                        uninitialized_plugin_streambuf_set_.erase(streambuf);
                    g_assert(succeeded);
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
                    pos->second->npstream_destroyed();
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
        }

    private:
        command_istream * in_;
        openvrml::browser * browser_;
    };
}

namespace {

    enum option_id {
        gtk_socket_id_id = 128,
        read_fd_id,
        write_fd_id
    };

    argp_option options[] = {
        {
            "gtk-socket-id",
            gtk_socket_id_id,
            "GTK_SOCKET_ID",
            0,
            "GtkSocket id",
            0
        },
        {
            "read-fd",
            read_fd_id,
            "READ_FD",
            0,
            "file descriptor for reading commands",
            0
        },
        {
            "write-fd",
            write_fd_id,
            "WRITE_FD",
            0,
            "file descriptor for writing commands",
            0
        },
        { 0, 0, 0, 0, 0, 0 }
    };

    struct arguments {
        GdkNativeWindow socket_id;
        int read_fd;
        int write_fd;
    };

    struct initial_stream_reader {
        initial_stream_reader(
            const boost::shared_ptr<plugin_streambuf> & streambuf,
            openvrml::browser & browser):
            streambuf_(streambuf),
            browser_(&browser)
        {}

        void operator()() const
        {
            class plugin_istream : public std::istream {
                boost::shared_ptr<plugin_streambuf> streambuf_;

            public:
                explicit plugin_istream(
                    const boost::shared_ptr<plugin_streambuf> & streambuf):
                    std::istream(streambuf.get()),
                    streambuf_(streambuf)
                {}

                virtual ~plugin_istream() throw ()
                {}
            } in(this->streambuf_);
            std::vector<boost::intrusive_ptr<openvrml::node> > nodes =
                this->browser_->create_vrml_from_stream(in);
            this->browser_->replace_world(nodes);
        }

    private:
        boost::shared_ptr<plugin_streambuf> streambuf_;
        openvrml::browser * browser_;
    };

    GIOChannel * command_channel;
    GIOChannel * request_channel;
}

int main(int argc, char * argv[])
{
    using std::string;
    using std::vector;
    using boost::function0;
    using boost::scoped_ptr;
    using boost::shared_ptr;
    using boost::thread;
    using boost::thread_group;

    using namespace openvrml_player;

    g_set_application_name(application_name);

    gtk_init(&argc, &argv);
    gtk_gl_init(&argc, &argv);

    arguments arguments;
    arguments.socket_id = 0;
    arguments.read_fd = 0;
    arguments.write_fd = 0;

    char args_doc[] = "[URI]";
    char * const doc = 0;
    argp_child * const children = 0;
    char *(*help_filter)(int key, const char * text, void * input) = 0;
    char * argp_domain = 0;
    argp argp = {
        options,
        parse_opt,
        args_doc,
        doc,
        children,
        help_filter,
        argp_domain
    };
    int uri_arg_index;
    argp_parse(&argp, argc, argv, 0, &uri_arg_index, &arguments);

    command_istream command_in;

    if (arguments.read_fd) {
        ::command_channel = g_io_channel_unix_new(arguments.read_fd);
        GError * error = 0;
        GIOStatus status = g_io_channel_set_encoding(::command_channel,
                                                     0, // binary (no encoding)
                                                     &error);
        if (status != G_IO_STATUS_NORMAL) {
            if (error) {
                g_critical(error->message);
                g_error_free(error);
            }
            return EXIT_FAILURE;
        }

        g_io_add_watch(::command_channel,
                       G_IO_IN,
                       command_data_available,
                       static_cast<command_streambuf *>(command_in.rdbuf()));
    }

    if (arguments.write_fd) {
        ::request_channel = g_io_channel_unix_new(arguments.write_fd);
    }

    GtkWidget * window = arguments.socket_id
        ? gtk_plug_new(arguments.socket_id)
        : gtk_window_new(GTK_WINDOW_TOPLEVEL);

    GtkGLViewer viewer(*(GTK_CONTAINER(window)));
    browser b(::request_channel);
    b.viewer(&viewer);

    gtk_widget_show_all(window);

    thread_group threads;

    scoped_ptr<thread> initial_stream_reader_thread;
    if (uri_arg_index < argc) {
        const vector<string> uri(1, argv[uri_arg_index]), parameter;
        b.load_url(uri, parameter);
    } else {
        shared_ptr<plugin_streambuf> initial_stream(new plugin_streambuf);
        bool succeeded =
            uninitialized_plugin_streambuf_set_.insert(initial_stream);
        g_return_val_if_fail(succeeded, EXIT_FAILURE);
        function0<void> f = initial_stream_reader(initial_stream, b);
        initial_stream_reader_thread.reset(threads.create_thread(f));
    }

    viewer.timer_update();

    function0<void> read_commands = command_istream_reader(command_in, b);
    scoped_ptr<thread> command_reader_thread(
        threads.create_thread(read_commands));

    gtk_main();

    g_io_channel_shutdown(::command_channel, false, 0);
    g_io_channel_unref(::command_channel);
}

error_t parse_opt(int key, char * arg, argp_state * state)
{
    using boost::lexical_cast;
    using boost::bad_lexical_cast;

    arguments & args = *static_cast<arguments *>(state->input);

    switch (key) {
    case gtk_socket_id_id:
        try {
            args.socket_id = lexical_cast<GdkNativeWindow>(arg);
        } catch (bad_lexical_cast &) {
            argp_error(state, "GTK_SOCKET_ID must be an integer");
        }
        break;
    case read_fd_id:
        try {
            args.read_fd = lexical_cast<int>(arg);
        } catch (bad_lexical_cast &) {
            argp_error(state, "READ_FD must be an unsigned integer");
        }
        break;
    case write_fd_id:
        try {
            args.write_fd = lexical_cast<int>(arg);
        } catch (bad_lexical_cast &) {
            argp_error(state, "WRITE_FD must be an unsigned integer");
        }
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}
