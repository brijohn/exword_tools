/* TextLoader - program for transfering text files to EX-Word dictionaries
 *
 * Copyright (C) 2010 - Brian Johnson <brijohn@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <exword.h>

#include <config.h>

#define GLADEFILE PKGDATADIR "/TextLoader.glade"

typedef struct _TextLoader {
	GtkBuilder *builder;
	exword_t *handle;
} TextLoader;

TextLoader * textloader_init(GError **error)
{
	GtkWidget *window;
	GtkWidget *file_list;
    	static GtkTargetEntry targetentries[] =
	{
	  {"STRING",        0, 0 },
	  {"text/plain",    0, 0 },
	  {"text/uri-list", 0, 1 },
	};
	*error = NULL;
	TextLoader *self = malloc(sizeof(struct _TextLoader));
	if (!self) {
		*error = g_error_new_literal(0x1000, 0x1000, _("Out of memory"));
		return NULL;
	}
	self->builder = gtk_builder_new();
	guint ret = gtk_builder_add_from_file(self->builder, GLADEFILE, error);
	if (*error) {
		free(self);
		return NULL;
	}
	window = GTK_WIDGET(gtk_builder_get_object (self->builder, "window"));
	file_list = GTK_WIDGET(gtk_builder_get_object (self->builder, "file_list"));
	gtk_drag_dest_set(file_list, GTK_DEST_DEFAULT_ALL, targetentries, 3,
			  GDK_ACTION_COPY|GDK_ACTION_MOVE|GDK_ACTION_LINK);
	gtk_builder_connect_signals(self->builder, self );
	gtk_window_set_title(GTK_WINDOW(window), _("TextLoader"));
	gtk_widget_show(window);
	self->handle = NULL;
	return self;
}

int textloader_is_sd_available(TextLoader* self)
{
	directory_entry_t *entries;
	uint16_t count;
	int i;
	int found = 0;
	if (exword_setpath(self->handle, "", SETPATH_NOCREATE) == 0x20) {
		if (exword_list(self->handle, &entries, &count) == 0x20) {
			for (i = 0; i < count; i++) {
				if (strcmp(entries[i].name, "_SD_00") == 0)
					found = 1;
			}
			free(entries);
		}
	}
	return found;
}

int textloader_capacity(TextLoader *self)
{
	GtkStatusbar *status;
	exword_capacity_t cap;
	gchar *message;
	guint  id;
	if (exword_get_capacity(self->handle, &cap) == 0x20) {
		status = GTK_STATUSBAR (gtk_builder_get_object (self->builder, "statusbar"));
		id = gtk_statusbar_get_context_id(status, "capacity");
		message = g_strdup_printf("%s: %u / %u", _("Capacity"), cap.total, cap.used);
		gtk_statusbar_pop(status, id);
		gtk_statusbar_push(status, id, message);
		g_free(message);
	}
	return 1;
}

int textloader_list_files(TextLoader *self)
{
	GtkTreeIter iter;
	GtkListStore *files;
	directory_entry_t *entries;
	uint16_t count;
	GtkToggleButton *mem;
	int i;
	char *filename, *ext;
	int len;
	files = GTK_LIST_STORE (gtk_builder_get_object (self->builder, "filestore"));
	mem = GTK_TOGGLE_BUTTON (gtk_builder_get_object (self->builder, "mem"));
	gtk_list_store_clear(files);
	if (gtk_toggle_button_get_active(mem)) {
		if (exword_setpath(self->handle, "\\_INTERNAL_00", SETPATH_NOCREATE) != 0x20)
			return 0;
	} else {
		if (exword_setpath(self->handle, "\\_SD_00", SETPATH_NOCREATE) != 0x20)
			return 0;
	}
	if (exword_list(self->handle, &entries, &count) != 0x20)
		return 0;
	for (i = 0; i < count; i++) {
		if (entries[i].flags & LIST_F_UNICODE)
			filename = utf16_to_locale(&filename, &len, entries[i].name, entries[i].size - 3);
		else
			filename = entries[i].name;
		if (filename != NULL) {
			ext = strrchr(filename, '.');
			if (ext != NULL && (!strcmp(ext, ".TXT") || !strcmp(ext, ".txt"))) {
				gtk_list_store_append(files, &iter);
				gtk_list_store_set(files, &iter, 0, filename, 1, entries[i].flags, -1);
			}
			if (entries[i].flags & LIST_F_UNICODE)
				free(filename);
		}
	}
	textloader_capacity(self);
	free(entries);
	return 1;
}

int textloader_delete_file(TextLoader *self, GError **error)
{
	int ret;
	GtkTreeView *file_list;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GValue val={0,};
	char *filename;
	*error = NULL;
	file_list = GTK_TREE_VIEW(gtk_builder_get_object (self->builder, "file_list"));
	selection = gtk_tree_view_get_selection(file_list);
	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get_value(model, &iter, 0, &val);
		ret = exword_remove_file(self->handle, (char *)g_value_get_string(&val));
		g_value_unset(&val);
		if (ret == 0x44) {
			*error = g_error_new_literal(0x1000, 0x1003, _("File not found."));
			return 0;
		}
		if (ret != 0x20) {
			*error = g_error_new_literal(0x1000, 0x1004, _("Internal error."));
			return 0;
		}
		textloader_list_files(self);
	}
	return 1;
}

int textloader_connect(TextLoader *self, GError **error)
{
	GtkTreeIter iter;
	GtkComboBox *select_locale;
	GtkTreeModel *locales;
	GtkWidget *sdcard;
	GtkButton *connect;
	GValue val={0,};
	*error = NULL;
	if (!self->handle) {
		sdcard = GTK_WIDGET (gtk_builder_get_object (self->builder, "sdcard"));
		select_locale = GTK_COMBO_BOX (gtk_builder_get_object (self->builder, "select_locale"));
		connect = GTK_BUTTON (gtk_builder_get_object (self->builder, "connect"));
		locales = gtk_combo_box_get_model(select_locale);
		gtk_combo_box_get_active_iter(select_locale, &iter);
		gtk_tree_model_get_value(locales, &iter, 1, &val);
		self->handle = exword_open2(OPEN_TEXT | (g_value_get_uint(&val) & 0xFF));
		g_value_unset(&val);
		if (!self->handle) {
			*error = g_error_new_literal(0x1000, 0x1001, _("Device not found."));
			return 0;
		}
		if (exword_connect(self->handle) != 0x20) {
			*error = g_error_new_literal(0x1000, 0x1002, _("Connect failed."));
			exword_close(self->handle);
			self->handle = NULL;
			return 0;
		}
		gtk_button_set_label(connect, _("  Disconnect  "));
		gtk_widget_set_sensitive(GTK_WIDGET(select_locale), FALSE);
		if (!textloader_is_sd_available(self))
			gtk_widget_set_sensitive(sdcard, FALSE);
		else
			gtk_widget_set_sensitive(sdcard, TRUE);
		exword_setpath(self->handle, "\\_INTERNAL_00\\", SETPATH_NOCREATE);
		textloader_list_files(self);
	}
	return 1;
}

int textloader_disconnect(TextLoader *self)
{
	GtkWidget *sdcard, *mem;
	GtkListStore * files;
	GtkWidget *select_locale;
	GtkButton *connect;
	GtkStatusbar *status;
	guint id;
	status = GTK_STATUSBAR (gtk_builder_get_object (self->builder, "statusbar"));
	files = GTK_LIST_STORE (gtk_builder_get_object (self->builder, "filestore"));
	mem = GTK_WIDGET (gtk_builder_get_object (self->builder, "mem"));
	sdcard = GTK_WIDGET (gtk_builder_get_object (self->builder, "sdcard"));
	select_locale = GTK_WIDGET (gtk_builder_get_object (self->builder, "select_locale"));
	connect = GTK_BUTTON (gtk_builder_get_object (self->builder, "connect"));
	id = gtk_statusbar_get_context_id(status, "capacity");
	gtk_statusbar_remove_all(status, id);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mem), TRUE);
	gtk_widget_set_sensitive(sdcard, FALSE);
	gtk_list_store_clear(files);
	gtk_button_set_label(connect, _("  Connect  "));
	gtk_widget_set_sensitive(select_locale, TRUE);
	if (self->handle) {
		exword_disconnect(self->handle);
		exword_close(self->handle);
		self->handle = NULL;
	}
	return 1;
}


void textloader_display_error(TextLoader *self, gchar* message)
{
	GtkWidget *dialog, *window;
	window = GTK_WIDGET(gtk_builder_get_object (self->builder, "window"));
	dialog = gtk_message_dialog_new(GTK_WINDOW(window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE,
					"%s", message);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void textloader_deinit(TextLoader* self)
{
	if(self) {
		textloader_disconnect(self);
		g_object_unref(G_OBJECT(self->builder));
		free(self);
	}
}

void on_connect_clicked (GtkObject * widget, gpointer data)
{
	TextLoader *self = (TextLoader *)data;
	GError *err = NULL;
	if (!self->handle) {
		if (!textloader_connect(self, &err)) {
			textloader_display_error(self, err->message);
			g_error_free(err);
		}
	} else {
		textloader_disconnect(self);
	}
}

void on_mem_clicked (GtkObject * widget, gpointer data)
{
	TextLoader *self = (TextLoader *)data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		textloader_list_files(self);
	}
}

void on_sdcard_clicked (GtkObject * widget, gpointer data)
{
	TextLoader *self = (TextLoader *)data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		textloader_list_files(self);
	}
}

void on_delete_file_clicked (GtkObject * widget, gpointer data)
{
	GError *err = NULL;
	TextLoader *self = (TextLoader *)data;
	if (!textloader_delete_file(self, &err)) {
		textloader_display_error(self, err->message);
		g_error_free(err);
	}
}

void on_destroy (GtkObject * widget, gpointer data)
{
	TextLoader *self = (TextLoader *)data;
	textloader_deinit(self);
	gtk_main_quit ();
}

void on_upload_file(GtkWidget *wgt, GdkDragContext *context, int x, int y,
		    GtkSelectionData *seldata, guint info, guint time,
		    gpointer data)
{
	int i;
	char *ext;
	gchar ** uri_list;
	GFile *file;
	char *buffer;
	gsize length;
	GError *err = NULL;
	TextLoader *self = (TextLoader *)data;
	if (!self->handle)
		return;
	uri_list = g_uri_list_extract_uris((const gchar *)seldata->data);
	for (i = 0; uri_list[i] != NULL; i++) {
		file = g_file_new_for_uri(uri_list[i]);
		ext = strrchr(g_file_get_basename(file), '.');
		if (ext != NULL && (!strcmp(ext, ".TXT") || !strcmp(ext, ".txt"))) {
			if (g_file_load_contents(file, NULL, &buffer, &length, NULL, &err)) {
				exword_send_file(self->handle, g_file_get_basename(file), buffer, length);
				textloader_list_files(self);
				free(buffer);
			} else {
				textloader_display_error(self, err->message);
			}
		}
		g_object_unref(G_OBJECT(file));
	}
	g_strfreev(uri_list);
	gtk_drag_finish(context, TRUE, FALSE, time);
}

int main(int argc, char** argv)
{
	GError *err = NULL;
	gtk_init(&argc, &argv);
	bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
	TextLoader *text = textloader_init(&err);
	if (text) {
        	gtk_main();
	} else {
		fprintf(stderr, "%s\n", err->message);
		g_error_free(err);
	}
	return 0;
}

