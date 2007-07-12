// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// Copyright 2007  Braden McDaniel
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

# include <gtk/gtk.h>
# include <boost/concept_check.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include "filechooserdialog.h"

using namespace boost::multi_index::detail; // for scope_guard

namespace {
    G_GNUC_INTERNAL void class_init(gpointer g_class, gpointer class_data);
    G_GNUC_INTERNAL void init(GTypeInstance * instance, gpointer g_class);
}

GtkType openvrml_player_file_chooser_dialog_get_type()
{
    static GtkType type = 0;

    if (G_UNLIKELY(!type)) {
        type =
            g_type_register_static_simple(
                GTK_TYPE_FILE_CHOOSER_DIALOG,
                "OpenvrmlPlayerFileChooserDialog",
                sizeof (OpenvrmlPlayerFileChooserDialogClass),
                class_init,
                sizeof (OpenvrmlPlayerFileChooserDialog),
                init,
                GTypeFlags(0));
    }
    return type;
}

GtkWidget * openvrml_player_file_chooser_dialog_new(GtkWindow * parent)
{
    GtkWidget * const dialog =
        static_cast<GtkWidget *>(
            g_object_new(OPENVRML_PLAYER_TYPE_FILE_CHOOSER_DIALOG,
                         "title", "Open",
			 "action", GTK_FILE_CHOOSER_ACTION_OPEN,
                         NULL));
    scope_guard dialog_guard = make_guard(g_object_unref, dialog);
    boost::ignore_unused_variable_warning(dialog_guard);

    if (parent) {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    }

    gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                           GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                           NULL);

    gtk_dialog_set_default_response(GTK_DIALOG(dialog),
                                    GTK_RESPONSE_ACCEPT);

    GtkFileFilter * const world_filter = gtk_file_filter_new();
    g_return_val_if_fail(world_filter, 0);
    scope_guard world_filter_guard = make_guard(g_object_unref, world_filter);
    boost::ignore_unused_variable_warning(world_filter_guard);
    gtk_file_filter_set_name(world_filter, "VRML/X3D worlds");
    gtk_file_filter_add_mime_type(world_filter, "x-world/x-vrml");
    gtk_file_filter_add_mime_type(world_filter, "model/vrml");
    gtk_file_filter_add_mime_type(world_filter, "model/x3d+vrml");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), world_filter);
    world_filter_guard.dismiss();

    GtkFileFilter * const all_filter = gtk_file_filter_new();
    g_return_val_if_fail(all_filter, 0);
    scope_guard all_filter_guard = make_guard(g_object_unref, all_filter);
    boost::ignore_unused_variable_warning(all_filter_guard);
    gtk_file_filter_set_name(all_filter, "All files");
    gtk_file_filter_add_pattern(all_filter, "*");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_filter);
    all_filter_guard.dismiss();

    dialog_guard.dismiss();

    return dialog;
}

namespace {

    G_GNUC_INTERNAL GtkWidgetClass * parent_class;

    void class_init(const gpointer g_class, gpointer /* class_data */)
    {
        ::parent_class =
            static_cast<GtkWidgetClass *>(
                g_type_class_peek_parent(G_OBJECT_CLASS(g_class)));
    }

    void init(GTypeInstance * /* instance */, gpointer /* g_class */)
    {}
}
