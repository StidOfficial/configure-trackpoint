#include <gnome.h>


void
on_save_setting1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_main_delete_event                   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_checkbutton1_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_revert_clicked                      (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_revert_realized                     (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_closebutton1_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_error_dialog_quit_close             (GtkDialog       *dialog,
                                        gpointer         user_data);

void
on_closebutton1_clicked                (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_error_dialog_quit_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_toolbutton1_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_apply_clicked                       (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_applybutton1_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_revert2_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_restore_initial_values1_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_cancelbutton2_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_button_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_okbutton2_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_okbutton3_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancelbutton1_clicked               (GtkButton       *button,
                                        gpointer         user_data);
