#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <ctype.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "configure-trackpoint.h"

#define SCALE 0
#define TOGGLE 2

void write_line(char *widget_name, char *sys_name, char *orig,
		int default_value, int type, int sysfs_conf) {
  char tmp[1024];
  int current_value;

  switch (type) {
  case SCALE:
    current_value = gtk_range_get_value
      (GTK_RANGE(lookup_widget(main_window, widget_name)));
    break;
  case TOGGLE:
    current_value = gtk_toggle_button_get_active
      (GTK_TOGGLE_BUTTON(lookup_widget(main_window, widget_name)));
  }

  if (current_value != default_value) {
    if (sysfs_conf) {
      sprintf(tmp, "%s/%s=%d\n", driver_dir+5, sys_name, current_value);
    } else {
      sprintf(tmp, "echo -n %d > %s/%s\n", current_value, driver_dir, sys_name);
    }
    strcat(orig, tmp);
  }
}

/* result needs to be preallocated */
void get_save_line(char *result, int sysfs_conf) {
  result[0] = '\0';

  /* make the line */
  write_line("hscale12", "sensitivity", result, 0x80, SCALE, sysfs_conf);
  write_line("hscale14", "inertia", result, 0x06, SCALE, sysfs_conf);
  write_line("hscale13", "speed", result, 0x61, SCALE, sysfs_conf);
  write_line("hscale7", "reach", result, 0x0A, SCALE, sysfs_conf);
  write_line("hscale4", "draghys", result, 0xFF, SCALE, sysfs_conf);
  write_line("hscale3", "mindrag", result, 0x14, SCALE, sysfs_conf);
  write_line("hscale1", "thresh", result, 0x08, SCALE, sysfs_conf);
  write_line("hscale2", "upthresh", result, 0xFF, SCALE, sysfs_conf);
  write_line("hscale5", "ztime", result, 0x26, SCALE, sysfs_conf);
  write_line("hscale6", "jenks", result, 0x87, SCALE, sysfs_conf);
  write_line("checkbutton1", "press_to_select", result, 0, TOGGLE, sysfs_conf);
  write_line("checkbutton3", "skipback", result, 0, TOGGLE, sysfs_conf);
}

static void check_trackpoint_conf () {
  /* write to /etc/sysfs.conf */
  gtk_widget_show(create_dialog3());
}

void save_it () {
  /* check sensitivity */
  double sensitivity =
    gtk_range_get_value(GTK_RANGE(lookup_widget(main_window, "hscale12")));

  if (sensitivity < 20) {
    /* warn user */
    gtk_widget_show(create_dialog4());
    return;
  }
  check_trackpoint_conf();
}

void
on_save_setting1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  save_it();
}

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gtk_main_quit();
}

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  static GtkWidget *about = NULL;

  if (!about) {
    about = create_about2();
    g_object_add_weak_pointer(G_OBJECT(about), (void**)&(about));
    gtk_widget_show(about);
  }
}


gboolean
on_main_delete_event                   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_main_quit();
  return FALSE;
}


void
on_checkbutton1_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  int active = gtk_toggle_button_get_active(togglebutton);

  gtk_widget_set_sensitive(GTK_WIDGET(lookup_widget(GTK_WIDGET(togglebutton),
						    "checkbutton3")),
			   active);
  gtk_widget_set_sensitive(GTK_WIDGET(lookup_widget(GTK_WIDGET(togglebutton),
						    "table1")),
			   active);
}

void read_sys(char *filename, char *widget_name, GtkWidget *main, int type) {
  /* read it and set values */
  FILE *file;
  int param;
  char tmp[256];

  sprintf(tmp, "%s/%s", driver_dir, filename);
  file = fopen(tmp, "r");
  if (file == NULL) {
    fprintf(stderr, "error opening %s for reading\n", tmp);
    exit(0);
  }
  if (fscanf(file, "%d", &param) == 0) {
    fprintf(stderr, "error reading %s\n", tmp);
    exit(0);
  }
  switch (type) {
  case SCALE:
    gtk_range_set_value(GTK_RANGE(lookup_widget(main, widget_name)), param);
    break;
  case TOGGLE:
    gtk_toggle_button_set_active
      (GTK_TOGGLE_BUTTON(lookup_widget(main, widget_name)), param);
  }
  fclose(file);
}

void revert_to_sys(GtkWidget *widget) {
    read_sys("reach", "hscale7", widget, SCALE);
    read_sys("draghys", "hscale4", widget, SCALE);
    read_sys("jenks", "hscale6", widget, SCALE);
    read_sys("mindrag", "hscale3", widget, SCALE);
    read_sys("inertia", "hscale14", widget, SCALE);
    read_sys("sensitivity", "hscale12", widget, SCALE);
    read_sys("speed", "hscale13", widget, SCALE);
    read_sys("thresh", "hscale1", widget, SCALE);
    read_sys("upthresh", "hscale2", widget, SCALE);
    read_sys("ztime", "hscale5", widget, SCALE);
    read_sys("press_to_select", "checkbutton1", widget, TOGGLE);
    read_sys("skipback", "checkbutton3", widget, TOGGLE);
}

void
on_revert_clicked                      (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
  revert_to_sys(GTK_WIDGET(toolbutton));
}

void
on_revert_realized                     (GtkWidget       *widget,
                                        gpointer         user_data)
{
  on_revert_clicked(GTK_TOOL_BUTTON(widget), user_data);
  on_checkbutton1_toggled
    (GTK_TOGGLE_BUTTON(lookup_widget(widget, "checkbutton1")), NULL);
  
}

void
on_closebutton1_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_main_quit();
}


void
on_error_dialog_quit_close             (GtkDialog       *dialog,
                                        gpointer         user_data)
{
  gtk_main_quit();
}

gboolean
on_error_dialog_quit_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_main_quit();
  return FALSE;
}

void write_sys(char *filename, char *widget_name, int type)
{
  FILE *file;
  int param, setting;
  char tmp[256];

  sprintf(tmp, "%s/%s", driver_dir, filename);
  file = fopen(tmp, "r+");
  if (file == NULL) {
    fprintf(stderr, "error opening %s for reading\n", tmp);
    exit(0);
  }
  if (fscanf(file, "%d", &param) == 0) {
    fprintf(stderr, "error reading %s\n", tmp);
    exit(0);
  }
  switch(type) {
  case SCALE:
    setting = gtk_range_get_value
      (GTK_RANGE(lookup_widget(main_window, widget_name)));
    break;
  case TOGGLE:
    setting = gtk_toggle_button_get_active
      (GTK_TOGGLE_BUTTON(lookup_widget(main_window, widget_name)));
  }

   fclose(file);
   file = fopen(tmp, "w+");
   if (file == NULL) {
     fprintf(stderr, "error opening %s for writing\n", tmp);
     exit(0);
   }

  if (param != setting) {
    fprintf(file, "%d", setting);
  }
  fclose(file);
}

void write_all_settings() {
    write_sys("reach", "hscale7", SCALE);
    write_sys("draghys", "hscale4", SCALE);
    write_sys("jenks", "hscale6", SCALE);
    write_sys("mindrag", "hscale3", SCALE);
    write_sys("inertia", "hscale14", SCALE);
    write_sys("sensitivity", "hscale12", SCALE);
    write_sys("speed", "hscale13", SCALE);
    write_sys("thresh", "hscale1", SCALE);
    write_sys("upthresh", "hscale2", SCALE);
    write_sys("ztime", "hscale5", SCALE);
    write_sys("press_to_select", "checkbutton1", TOGGLE);
    write_sys("skipback", "checkbutton3", TOGGLE);
}

void
on_apply_clicked                       (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
  /* check sensitivity */
  double sensitivity =
    gtk_range_get_value(GTK_RANGE(lookup_widget(GTK_WIDGET(toolbutton),
						"hscale12")));
  if (sensitivity < 20) {
    /* warn user */
    gtk_widget_show(create_dialog1());
    return;
  }

  write_all_settings(toolbutton);
}

void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

void
on_applybutton1_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
  write_all_settings();
  gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


void
on_revert2_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  revert_to_sys(GTK_WIDGET(menuitem));
}


void
on_restore_initial_values1_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  /* sensitivity */
  gtk_range_set_value(GTK_RANGE(lookup_widget(GTK_WIDGET(menuitem),
					      "hscale12")),
		      0x80);

  /* inertia */
  gtk_range_set_value(GTK_RANGE(lookup_widget(GTK_WIDGET(menuitem),
					      "hscale14")),
		      0x06);

  /* speed */
  gtk_range_set_value(GTK_RANGE(lookup_widget(GTK_WIDGET(menuitem),
					      "hscale13")),
		      0x61);

  /* reach */
  gtk_range_set_value(GTK_RANGE(lookup_widget(GTK_WIDGET(menuitem),
					      "hscale7")),
		      0x0A);

  /* draghys */
  gtk_range_set_value(GTK_RANGE(lookup_widget(GTK_WIDGET(menuitem),
					      "hscale4")),
		      0xFF);

  /* mindrag */
  gtk_range_set_value(GTK_RANGE(lookup_widget(GTK_WIDGET(menuitem),
					      "hscale3")),
		      0x14);

  /* thresh */
  gtk_range_set_value(GTK_RANGE(lookup_widget(GTK_WIDGET(menuitem),
					      "hscale1")),
		      0x08);

  /* upthresh */
  gtk_range_set_value(GTK_RANGE(lookup_widget(GTK_WIDGET(menuitem),
					      "hscale2")),
		      0xFF);

  /* ztime */
  gtk_range_set_value(GTK_RANGE(lookup_widget(GTK_WIDGET(menuitem),
					      "hscale5")),
		      0x26);

  /* jenks */
  gtk_range_set_value(GTK_RANGE(lookup_widget(GTK_WIDGET(menuitem),
					      "hscale6")),
		      0x87);

  /* press to select */
  gtk_toggle_button_set_active
    (GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(menuitem), "checkbutton1")),
     FALSE);

  /* skipback */
  gtk_toggle_button_set_active
    (GTK_TOGGLE_BUTTON(lookup_widget(GTK_WIDGET(menuitem), "checkbutton3")),
     FALSE);
}

void
on_cancelbutton2_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
  char text[1024], result[1024];
  GtkWidget *widget = create_dialog2();

  get_save_line(result, FALSE);
  sprintf(text, "To save settings, please copy the following lines into your start up file.\n\n%s", result);
  gtk_label_set_text(GTK_LABEL(lookup_widget(widget, "label38")), text);
  gtk_widget_show(widget);
  gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

void
on_save_button_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
  save_it();
}

int is_trackpoint_line_p(char *line) {
  /* check "devices/platform/i8042/serio" string */
#define LENGTH 28

  if (strncmp(line, "devices/platform/i8042/serio", LENGTH) == 0) {
    return TRUE;
  }

  return FALSE;
}

void
on_okbutton2_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  struct stat statBuf;
  FILE *input_file, *output_file;
  char *line = NULL;
  size_t len;
  char result[1024];
  char *original = "/etc/sysfs.conf";
  GtkWidget *widget = create_dialog5();

  if (stat("/etc/sysfs.conf", &statBuf)) {
    /* write a new file */
    output_file = fopen(original, "w");
    gtk_label_set_text(GTK_LABEL(lookup_widget(widget, "label41")),
		       "TrackPoint settings saved. Please make sure you have sysfsutils installed.");
  } else {
    /* move the file to backup*/
    char *backup = "/etc/sysfs.conf.bak";
    rename(original, backup);
    /* write a new one */
    output_file = fopen(original, "w");
    input_file = fopen(backup, "r");
    while (getline(&line, &len, input_file) != -1) {
      /* remove the old line */
      if (!is_trackpoint_line_p(line)) {
	fputs(line, output_file);
      }
    }
    if (line) {
      free(line);
    }
    fclose(input_file);
  }

  get_save_line(result, TRUE);
  fputs(result, output_file);
  fclose(output_file);
  chmod(original,
	S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  gtk_widget_show(widget);
  gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

void
on_okbutton3_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  check_trackpoint_conf();
  gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}
