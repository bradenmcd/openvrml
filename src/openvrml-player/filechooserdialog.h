// -*- mode: C; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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


# ifndef OPENVRML_PLAYER_FILE_CHOOSER_DIALOG_H
#   define OPENVRML_PLAYER_FILE_CHOOSER_DIALOG_H

#   include <gtk/gtkfilechooserdialog.h>

G_BEGIN_DECLS

#   define OPENVRML_PLAYER_TYPE_FILE_CHOOSER_DIALOG             (openvrml_player_file_chooser_dialog_get_type())
#   define OPENVRML_PLAYER_FILE_CHOOSER_DIALOG(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), OPENVRML_PLAYER_TYPE_FILE_CHOOSER_DIALOG, OpenvrmlPlayerFileChooserDialog))
#   define OPENVRML_PLAYER_FILE_CHOOSER_DIALOG_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), OPENVRML_PLAYER_TYPE_FILE_CHOOSER_DIALOG, OpenvrmlPlayerFileChooserDialogClass))
#   define OPENVRML_PLAYER_IS_FILE_CHOOSER_DIALOG(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OPENVRML_PLAYER_TYPE_FILE_CHOOSER_DIALOG))
#   define OPENVRML_PLAYER_IS_FILE_CHOOSER_DIALOG_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), OPENVRML_PLAYER_TYPE_FILE_CHOOSER_DIALOG))
#   define OPENVRML_PLAYER_FILE_CHOOSER_DIALOG_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), OPENVRML_PLAYER_TYPE_FILE_CHOOSER_DIALOG, OpenvrmlPlayerFileChooserDialogClass))

typedef struct OpenvrmlPlayerFileChooserDialog_ OpenvrmlPlayerFileChooserDialog;
typedef struct OpenvrmlPlayerFileChooserDialogClass_ OpenvrmlPlayerFileChooserDialogClass;

struct OpenvrmlPlayerFileChooserDialogClass_ {
    GtkFileChooserDialogClass parent_class;
};

struct OpenvrmlPlayerFileChooserDialog_ {
    GtkFileChooserDialog parent_instance;
};

GType openvrml_player_file_chooser_dialog_get_type() G_GNUC_CONST;
GtkWidget * openvrml_player_file_chooser_dialog_new(GtkWindow * parent);

G_END_DECLS

# endif // ifndef OPENVRML_PLAYER_FILE_CHOOSER_DIALOG_H
