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

# include <cstdlib>
# include <boost/lexical_cast.hpp>
# include <boost/spirit.hpp>
# include <boost/spirit/phoenix.hpp>
# include <unistd.h>
# include <argp.h>
# include <X11/keysym.h>
# include <gdk/gdkx.h>
# include <gtk/gtk.h>
# include <gtk/gtkgl.h>
# include <openvrml/browser.h>
# include <openvrml/gl/viewer.h>

namespace {

    const char application_name[] = "OpenVRML Player";
}

extern "C" {
    const char * argp_program_version = "OpenVRML Player " PACKAGE_VERSION;
    const char * argp_program_bug_address = PACKAGE_BUGREPORT;

    error_t parse_opt(int key, char * arg, argp_state * state);

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

    enum option_id {
        gtk_socket_id_id = 128
    };

    argp_option options[] = {
        {
            "gtk-socket-id",
            gtk_socket_id_id,
            "GTK_SOCKET_ID",
            0,
            "GtkSocket id"
        },
        {}
    };

    class GtkGLViewer : public openvrml::gl::viewer {
        static GdkGLConfig * gl_config;

        GtkWidget * drawing_area;
        guint timer;

    public:
        bool redrawNeeded;

        explicit GtkGLViewer(GtkContainer & container);
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
}

int main(int argc, char * argv[])
{
    using std::cout;
    using std::cerr;
    using std::endl;
    using std::string;
    using std::vector;

    g_set_application_name(application_name);

    gtk_init(&argc, &argv);
    gtk_gl_init(&argc, &argv);

    GdkNativeWindow socket_id = 0;

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
    argp_parse(&argp, argc, argv, 0, &uri_arg_index, &socket_id);

    GtkWidget * window = socket_id
        ? gtk_plug_new(socket_id)
        : gtk_window_new(GTK_WINDOW_TOPLEVEL);

    GtkGLViewer viewer(*(GTK_CONTAINER(window)));
    openvrml::browser b(cout, cerr);
    b.viewer(&viewer);

    gtk_widget_show_all(window);

    if (uri_arg_index < argc) {
        const vector<string> uri(1, argv[uri_arg_index]), parameter;
        b.load_url(uri, parameter);
    }

    viewer.timer_update();

    gtk_main();
}

error_t parse_opt(int key, char * arg, argp_state * state)
{
    using boost::lexical_cast;
    using boost::bad_lexical_cast;

    GdkNativeWindow & gtk_socket_id =
        *static_cast<GdkNativeWindow *>(state->input);

    switch (key) {
    case gtk_socket_id_id:
        try {
            gtk_socket_id = lexical_cast<GdkNativeWindow>(arg);
        } catch (bad_lexical_cast &) {
            argp_error(state, "GTK_SOCKET_ID must be an integer");
        }
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

namespace {

    GdkGLConfig * GtkGLViewer::gl_config(0);

    GtkGLViewer::GtkGLViewer(GtkContainer & container):
        drawing_area(gtk_drawing_area_new()),
        timer(0),
        redrawNeeded(false)
    {
        if (!this->drawing_area) { throw std::bad_alloc(); }

        if (!GtkGLViewer::gl_config) {
            static const int attrib_list[] = {
                // GDK_GL_ALPHA_SIZE, 1,
                GDK_GL_DOUBLEBUFFER,
                GDK_GL_DEPTH_SIZE, 1,
                GDK_GL_RGBA,
                GDK_GL_RED_SIZE, 1,
                GDK_GL_ATTRIB_LIST_NONE
            };
            GtkGLViewer::gl_config = gdk_gl_config_new(attrib_list);
        }

        static GdkGLContext * const share_list = 0;
        static const gboolean direct = false;
        static const int render_type = GDK_GL_RGBA_TYPE;
        gtk_widget_set_gl_capability(this->drawing_area,
                                     GtkGLViewer::gl_config,
                                     share_list,
                                     direct,
                                     render_type);

        gtk_widget_add_events(this->drawing_area,
                              GDK_EXPOSURE_MASK
                              | GDK_BUTTON_PRESS_MASK
                              | GDK_BUTTON_RELEASE_MASK
                              | GDK_KEY_PRESS_MASK
                              | GDK_POINTER_MOTION_MASK);

        g_signal_connect(G_OBJECT((this->drawing_area)),
                         "expose_event",
                         G_CALLBACK(expose_event),
                         this);
        g_signal_connect(G_OBJECT(this->drawing_area),
                         "configure_event",
                         G_CALLBACK(configure_event),
                         this);
        g_signal_connect(G_OBJECT(this->drawing_area),
                         "key_press_event",
                         G_CALLBACK(key_press_event),
                         this);
        g_signal_connect(G_OBJECT(this->drawing_area),
                         "button_press_event",
                         G_CALLBACK(button_press_event),
                         this);
        g_signal_connect(G_OBJECT(this->drawing_area),
                         "button_release_event",
                         G_CALLBACK(button_release_event),
                         this);
        g_signal_connect(G_OBJECT(this->drawing_area),
                         "motion_notify_event",
                         G_CALLBACK(motion_notify_event),
                         this);

        gtk_container_add(&container, this->drawing_area);

        
    }

    GtkGLViewer::~GtkGLViewer() throw ()
    {
        if (this->timer) { g_source_remove(timer); }
    }

    void GtkGLViewer::post_redraw()
    {
        if (!this->redrawNeeded) {
            this->redrawNeeded = true;
            gtk_widget_queue_draw(GTK_WIDGET(this->drawing_area));
        }
    }

    void GtkGLViewer::set_cursor(cursor_style style)
    {
        GdkCursor * cursor(0);
        GtkWidget * const widget = GTK_WIDGET(this->drawing_area);

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
        GtkWidget * const widget = GTK_WIDGET(this->drawing_area);
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
