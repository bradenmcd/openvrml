// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// GTK+ OpenVRML widget
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

# include <openvrml/browser.h>
# include <openvrml/gl/viewer.h>
# include <X11/keysym.h>
# include <gdk/gdkx.h>
# include <gtk/gtkgl.h>
# include "vrmlbrowser.h"

class GtkGLViewer : public openvrml::gl::viewer {
    GtkVrmlBrowser & vrml_browser;
    guint timer;

public:
    bool redrawNeeded;

    GtkGLViewer(GtkVrmlBrowser & vrml_browser);
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

//
// These functions could be "static" in C.  However, giving them external C
// linkage is the only way in C++ to guarantee that they can be used from C
// function pointers in code compiled with a C compiler.  Some compilers (SPro)
// are picky about this.
//
extern "C" {
    void gtk_vrml_browser_class_init(GtkVrmlBrowserClass *);
    void gtk_vrml_browser_init(GtkVrmlBrowser *);
    void gtk_vrml_browser_destroy(GtkObject *);
    void gtk_vrml_browser_realize(GtkWidget *);
    gint gtk_vrml_browser_expose(GtkWidget *, GdkEventExpose *);
    gint gtk_vrml_browser_configure(GtkWidget *, GdkEventConfigure *);
    gint gtk_vrml_browser_key_press(GtkWidget *, GdkEventKey *);
    gint gtk_vrml_browser_button_press(GtkWidget *, GdkEventButton *);
    gint gtk_vrml_browser_button_release(GtkWidget *, GdkEventButton *);
    gint gtk_vrml_browser_motion_notify(GtkWidget *, GdkEventMotion *);
}

namespace {
    GtkDrawingAreaClass * parent_class = 0;
    GdkGLConfig * gl_config = 0;
}

GType gtk_vrml_browser_get_type()
{
    static GType vrml_browser_type = 0;

    if (!vrml_browser_type) {
        GTypeInfo vrml_browser_info = {
            sizeof (GtkVrmlBrowserClass),
            0, // base_init
            0, // base_finalize
            reinterpret_cast<GClassInitFunc>(gtk_vrml_browser_class_init),
            0, // class_finalize
            0, // class_data
            sizeof (GtkVrmlBrowser),
            0, // n_preallocs
            reinterpret_cast<GInstanceInitFunc>(gtk_vrml_browser_init)
        };
        vrml_browser_type = g_type_register_static(GTK_TYPE_DRAWING_AREA,
                                                   "GtkVrmlBrowser",
                                                   &vrml_browser_info,
                                                   GTypeFlags(0));
    }

    return vrml_browser_type;
}

GtkWidget * gtk_vrml_browser_new()
{
    return GTK_WIDGET(g_object_new(GTK_TYPE_VRML_BROWSER, 0));
}

gboolean gtk_vrml_browser_load_url(GtkVrmlBrowser * const vrml_browser,
                                   const char * const url)
{
    using std::string;
    using std::vector;
    using openvrml::browser;

    g_assert(vrml_browser);
    g_assert(vrml_browser->browser);

    const vector<string> uri(1, url), parameter;
    static_cast<browser *>(vrml_browser->browser)->load_url(uri, parameter);
    static_cast<GtkGLViewer *>(vrml_browser->viewer)->timer_update();
    return true;
}

void
gtk_vrml_browser_class_init(GtkVrmlBrowserClass * const vrml_browser_class)
{
    parent_class =
        static_cast<GtkDrawingAreaClass *>(
            gtk_type_class(gtk_drawing_area_get_type()));

    static const int attrib_list[] = {
//        GDK_GL_ALPHA_SIZE, 1,
        GDK_GL_DOUBLEBUFFER,
        GDK_GL_DEPTH_SIZE, 1,
        GDK_GL_RGBA,
        GDK_GL_RED_SIZE, 1,
        GDK_GL_ATTRIB_LIST_NONE
    };

    gl_config = gdk_gl_config_new(attrib_list);
    g_return_if_fail(gl_config);

    GtkObjectClass * const object_class =
        GTK_OBJECT_CLASS(vrml_browser_class);
    object_class->destroy = gtk_vrml_browser_destroy;

    GtkWidgetClass * const widget_class = GTK_WIDGET_CLASS(vrml_browser_class);
    widget_class->realize               = gtk_vrml_browser_realize;
    widget_class->expose_event          = gtk_vrml_browser_expose;
    widget_class->configure_event       = gtk_vrml_browser_configure;
    widget_class->key_press_event       = gtk_vrml_browser_key_press;
    widget_class->button_press_event    = gtk_vrml_browser_button_press;
    widget_class->button_release_event  = gtk_vrml_browser_button_release;
    widget_class->motion_notify_event   = gtk_vrml_browser_motion_notify;
}

void gtk_vrml_browser_init(GtkVrmlBrowser * const vrml_browser)
{
    g_return_if_fail(gl_config);
    g_return_if_fail(vrml_browser);

    vrml_browser->browser = 0;
    vrml_browser->viewer = 0;

    static GdkGLContext * const share_list = 0;
    static const gboolean direct = false;
    static const int render_type = GDK_GL_RGBA_TYPE;
    gtk_widget_set_gl_capability(GTK_WIDGET(vrml_browser),
                                 gl_config,
                                 share_list,
                                 direct,
                                 render_type);

    try {
        using std::auto_ptr;
        using openvrml::browser;
        using openvrml::viewer;

        auto_ptr<browser> b(new browser(std::cout, std::cerr));
        auto_ptr<viewer> v(new GtkGLViewer(*vrml_browser));

        b->viewer(v.get());

        vrml_browser->browser = b.release();
        vrml_browser->viewer = v.release();
    } catch (std::bad_alloc &) {
        g_return_if_fail(vrml_browser->browser);
        g_return_if_fail(vrml_browser->viewer);
    } catch (openvrml::viewer_in_use &) {
        g_return_if_fail(false);
    }
}

void gtk_vrml_browser_destroy(GtkObject * const object)
{
    g_return_if_fail(object);
    g_return_if_fail(GTK_IS_VRML_BROWSER(object));

    while (gtk_events_pending()) { gtk_main_iteration(); }

    GtkVrmlBrowser * const vrml_browser(GTK_VRML_BROWSER(object));
    delete static_cast<openvrml::browser *>(vrml_browser->browser);
    vrml_browser->browser = 0;
    delete static_cast<GtkGLViewer *>(vrml_browser->viewer);
    vrml_browser->viewer = 0;

    if (GTK_OBJECT_CLASS(parent_class)->destroy) {
        (*GTK_OBJECT_CLASS(parent_class)->destroy)(object);
    }
}

void gtk_vrml_browser_realize(GtkWidget * const widget)
{
    g_return_if_fail(widget);
    g_return_if_fail(GTK_IS_VRML_BROWSER(widget));

    GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

    GdkWindowAttr attributes;
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.event_mask = gtk_widget_get_events(widget)
                          | GDK_EXPOSURE_MASK
                          | GDK_BUTTON_PRESS_MASK
                          | GDK_BUTTON_RELEASE_MASK
                          | GDK_KEY_PRESS_MASK
                          | GDK_POINTER_MOTION_MASK;
    attributes.visual = gtk_widget_get_visual(widget);
    attributes.colormap = gtk_widget_get_colormap(widget);

    const gint attributes_mask = GDK_WA_X
                               | GDK_WA_Y
                               | GDK_WA_VISUAL
                               | GDK_WA_COLORMAP;
    widget->window = gdk_window_new(widget->parent->window,
                                    &attributes,
                                    attributes_mask);

    widget->style = gtk_style_attach(widget->style, widget->window);

    gdk_window_set_user_data(widget->window, widget);

    gtk_style_set_background(widget->style, widget->window, GTK_STATE_ACTIVE);
}

GtkGLViewer::GtkGLViewer(GtkVrmlBrowser & vrml_browser):
    vrml_browser(vrml_browser),
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
        gtk_widget_queue_draw(GTK_WIDGET(&this->vrml_browser));
    }
}

void GtkGLViewer::set_cursor(cursor_style style)
{
    GdkCursor * cursor(0);

    switch(style) {
    case cursor_inherit:
        XDefineCursor(
            GDK_WINDOW_XDISPLAY(GTK_WIDGET(&this->vrml_browser)->window),
            GDK_WINDOW_XWINDOW(GTK_WIDGET(&this->vrml_browser)->window),
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

    gdk_window_set_cursor(GTK_WIDGET(&this->vrml_browser)->window, cursor);
    gdk_cursor_destroy(cursor);
}

void GtkGLViewer::swap_buffers()
{
    GdkGLDrawable * const gl_drawable =
        gtk_widget_get_gl_drawable(GTK_WIDGET(&this->vrml_browser));
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
    while (gtk_events_pending()) { gtk_main_iteration(); }
    if (!this->timer) {
        this->timer = g_timeout_add(guint(1000.0 * t),
                                    GtkFunction(timeout_callback),
                                    this);
    }
}

void GtkGLViewer::timer_update()
{
    this->timer = 0;
    this->viewer::update();
}

gint gtk_vrml_browser_expose(GtkWidget * const widget,
                             GdkEventExpose * const event)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    g_assert(GTK_VRML_BROWSER(widget)->viewer);
    GtkGLViewer & viewer =
        *static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
    if (event->count == 0
        && gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        viewer.redraw();
    }
    viewer.redrawNeeded = false;

    return true;
}

gint gtk_vrml_browser_configure(GtkWidget * const widget,
                                GdkEventConfigure * const event)
{
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(GTK_VRML_BROWSER(widget)->viewer);
        GtkGLViewer & viewer =
            *static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        viewer.resize(widget->allocation.width, widget->allocation.height);
    }
    return true;
}

gint gtk_vrml_browser_key_press(GtkWidget * const widget,
                                GdkEventKey * const event)
{
    using openvrml::gl::viewer;

    viewer::event_info eventInfo;
    eventInfo.event = viewer::event_key_down;

    switch (event->keyval) {
    case XK_Home:
        eventInfo.what = viewer::key_home;
        break;

    case XK_Left:
        eventInfo.what = viewer::key_left;
        break;

    case XK_Up:
        eventInfo.what = viewer::key_up;
        break;

    case XK_Right:
        eventInfo.what = viewer::key_right;
        break;

    case XK_Down:
        eventInfo.what = viewer::key_down;
        break;

    case XK_Page_Up:
        eventInfo.what = viewer::key_page_up;
        break;

    case XK_Page_Down:
        eventInfo.what = viewer::key_page_down;
        break;

    default:
        if (event->length <= 0) { return true; }
        eventInfo.what = event->string[0];
    }
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(GTK_VRML_BROWSER(widget)->viewer);
        GtkGLViewer & viewer =
            *static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        viewer.input(&eventInfo);
    }
    return true;
}

gint gtk_vrml_browser_button_press(GtkWidget * const widget,
                                   GdkEventButton * const event)
{
    using openvrml::gl::viewer;

    viewer::event_info eventInfo;
    eventInfo.event = viewer::event_mouse_click;

    switch (event->button) {
    case Button1:
        eventInfo.what = 0;
        break;

    case Button2:
        eventInfo.what = 1;
        break;

    case Button3:
        eventInfo.what = 2;
        break;
    }

    eventInfo.x = int(event->x);
    eventInfo.y = int(event->y);
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(GTK_VRML_BROWSER(widget)->viewer);
        GtkGLViewer & viewer =
            *static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        viewer.input(&eventInfo);
    }
    return true;
}

gint gtk_vrml_browser_button_release(GtkWidget * const widget,
                                     GdkEventButton * const event)
{
    using openvrml::gl::viewer;

    viewer::event_info eventInfo;
    eventInfo.event = viewer::event_mouse_release;

    switch (event->button) {
    case Button1:
        eventInfo.what = 0;
        break;

    case Button2:
        eventInfo.what = 1;
        break;

    case Button3:
        eventInfo.what = 2;
        break;
    }

    eventInfo.x = int(event->x);
    eventInfo.y = int(event->y);
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(GTK_VRML_BROWSER(widget)->viewer);
        GtkGLViewer & viewer =
            *static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        viewer.input(&eventInfo);
    }
    return true;
}

gint gtk_vrml_browser_motion_notify(GtkWidget * const widget,
                                    GdkEventMotion * const event)
{
    using openvrml::gl::viewer;

    viewer::event_info eventInfo;
    eventInfo.event = viewer::event_mouse_drag;
    eventInfo.what = 0;
    if (event->state & Button1Mask) {
        eventInfo.what = 0;
    } else if (event->state & Button2Mask) {
        eventInfo.what = 1;
    } else if (event->state & Button3Mask) {
        eventInfo.what = 2;
    } else {
        eventInfo.event = viewer::event_mouse_move;
    }

    eventInfo.x = int(event->x);
    eventInfo.y = int(event->y);
    GdkGLDrawable * const gl_drawable = gtk_widget_get_gl_drawable(widget);
    g_assert(gl_drawable);
    GdkGLContext * const gl_context = gtk_widget_get_gl_context(widget);
    g_assert(gl_context);
    if (gdk_gl_drawable_make_current(gl_drawable, gl_context)) {
        g_assert(GTK_VRML_BROWSER(widget)->viewer);
        GtkGLViewer & viewer =
            *static_cast<GtkGLViewer *>(GTK_VRML_BROWSER(widget)->viewer);
        viewer.input(&eventInfo);
    }
    return true;
}
