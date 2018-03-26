#include "std_c++.h"
#include "gtk/gtk.h"
#include "CWebGet/CWebGet.h"

GtkWidget *add_menu_bar
            (GtkWidget *parent);
GtkWidget *add_menu_bar_cascade
            (GtkWidget *menu_bar, const string &label);
GtkWidget *add_menu_button
            (GtkWidget *menu, const string &label,
             GtkSignalFunc proc, gpointer data);

class GWebGet {
 private:
  CWebGet   *webget_;
  GtkWidget *url_text_widget_;
  GtkWidget *list_widget_;

 public:
  GWebGet();
 ~GWebGet();

  void createInterface();

  static gint delete_event_cb
               (GtkWidget *w, GdkEventAny *e, gpointer data);
  static void load_pressed_cb
               (GtkWidget *w, gpointer data);
  static void text_activate_cb
               (GtkWidget *w, gpointer data);
  static void help_activate_cb
               (GtkWidget *w, gpointer data);
  static void exit_activate_cb
               (GtkWidget *w, gpointer data);
};

int
main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  GWebGet *webget = new GWebGet();

  webget->createInterface();

  gtk_main();

  return 0;
}

GWebGet::
GWebGet() :
 webget_(NULL), url_text_widget_(NULL), list_widget_(NULL)
{
}

GWebGet::
~GWebGet()
{
  delete webget_;
}

void
GWebGet::
createInterface()
{
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

  gtk_window_set_title(GTK_WINDOW(window), "GWebGet");

  gtk_signal_connect(GTK_OBJECT(window), "delete_event",
                     GTK_SIGNAL_FUNC(&GWebGet::delete_event_cb),
                     (gpointer) this);

  //----

  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

  //----

  GtkWidget *menu_bar = gtk_menu_bar_new();

  gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

  GtkWidget *file_menu = add_menu_bar_cascade(menu_bar, "File");
  GtkWidget *help_menu = add_menu_bar_cascade(menu_bar, "Help");

  add_menu_button(file_menu, "Exit",
                  GTK_SIGNAL_FUNC(exit_activate_cb),
                  (gpointer) "Hello");
  add_menu_button(help_menu, "Help",
                  GTK_SIGNAL_FUNC(&GWebGet::help_activate_cb),
                  (gpointer) this);

  //----

  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);

  GtkWidget *label = gtk_label_new("URL");

  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

  url_text_widget_ = gtk_entry_new();

  gtk_signal_connect(GTK_OBJECT(url_text_widget_), "activate",
                     GTK_SIGNAL_FUNC(&GWebGet::text_activate_cb),
                     (gpointer) this);

  gtk_box_pack_start(GTK_BOX(hbox), url_text_widget_, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  //----

  GtkWidget *functions_hbox = gtk_hbox_new(FALSE, 0);

  GtkWidget *button = gtk_button_new_with_label("Load");

  gtk_box_pack_start(GTK_BOX(functions_hbox), button, FALSE, FALSE, 0);

  gtk_signal_connect(GTK_OBJECT(button), "pressed",
                     GTK_SIGNAL_FUNC(&GWebGet::load_pressed_cb),
                     (gpointer) this);

  gtk_box_pack_start(GTK_BOX(vbox), functions_hbox, FALSE, FALSE, 0);

  //----

  GtkWidget *swindow = gtk_scrolled_window_new(NULL, NULL);

  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swindow),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_ALWAYS);

  gtk_box_pack_start(GTK_BOX(vbox), swindow, TRUE, TRUE, 0);

  //----

  list_widget_ = gtk_clist_new(2);

  gtk_clist_set_column_title(GTK_CLIST(list_widget_), 0, "Name"  );
  gtk_clist_set_column_title(GTK_CLIST(list_widget_), 1, "Loaded");

  gtk_clist_set_column_width(GTK_CLIST(list_widget_), 0, 400);

  gtk_clist_set_selection_mode(GTK_CLIST(list_widget_),
                               GTK_SELECTION_EXTENDED);

  gtk_clist_column_titles_show(GTK_CLIST(list_widget_));

  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(swindow),
                                        list_widget_);

  //----

  gtk_container_add(GTK_CONTAINER(window), vbox);

  //----

  gtk_widget_show_all(window);
}

GtkWidget *
add_menu_bar(GtkWidget *parent)
{
  GtkWidget *menu_bar = gtk_menu_bar_new();

  gtk_container_add(GTK_CONTAINER(parent), menu_bar);

  return menu_bar;
}

GtkWidget *
add_menu_bar_cascade(GtkWidget *menu_bar, const string &label)
{
  GtkWidget *menu = gtk_menu_new();

  GtkWidget *item = gtk_menu_item_new_with_label(label.c_str());

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);

  gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), item);

  return menu;
}

GtkWidget *
add_menu_button(GtkWidget *menu, const string &label,
                GtkSignalFunc proc, gpointer data)
{
  GtkWidget *item = gtk_menu_item_new_with_label(label.c_str());

  gtk_signal_connect(GTK_OBJECT(item), "activate",
                     GTK_SIGNAL_FUNC(proc),
                     data);

  gtk_menu_append(GTK_MENU(menu), item);

  return item;
}

gint
GWebGet::
delete_event_cb(GtkWidget *, GdkEventAny *, gpointer)
{
  gtk_main_quit();

  return FALSE;
}

void
GWebGet::
load_pressed_cb(GtkWidget *, gpointer data)
{
  GWebGet *gwebget = (GWebGet *) data;

  for (int i = 0; i < GTK_CLIST(gwebget->list_widget_)->rows; ++i) {
    GtkCListRow *row = (GtkCListRow *)
      g_list_nth_data(GTK_CLIST(gwebget->list_widget_)->row_list, i);

    if (row->state & GTK_STATE_SELECTED) {
      CWebGetUrl *web_url = (CWebGetUrl *)
        gtk_clist_get_row_data(GTK_CLIST(gwebget->list_widget_), i);

      cout << "Download " << web_url->getUrlStr() << endl;

      gwebget->webget_->loadPage(*web_url);
    }
  }
}

void
GWebGet::
text_activate_cb(GtkWidget *, gpointer data)
{
  GWebGet *gwebget = (GWebGet *) data;

  string url_str = gtk_entry_get_text(GTK_ENTRY(gwebget->url_text_widget_));

  delete gwebget->webget_;

  gwebget->webget_ = new CWebGet(url_str);

  CWebGetUrl web_url(&*gwebget->webget_, url_str);

  gtk_clist_clear(GTK_CLIST(gwebget->list_widget_));

  if (gwebget->webget_->loadPage(web_url)) {
    vector<CWebGetUrl *> web_urls;

    if (! gwebget->webget_->getWebUrls(web_url, web_urls))
      return;

    const char *columns[2];

    int num_web_urls = web_urls.size();

    for (int i = 0; i < num_web_urls; ++i) {
      if (! gwebget->webget_->isValidSite(*web_urls[i]))
        continue;

      columns[0] = (char *) web_urls[i]->getUrlStr().c_str();

      if (gwebget->webget_->isLoaded(*web_urls[i]))
        columns[1] = "Yes";
      else
        columns[1] = "No";

      gtk_clist_append(GTK_CLIST(gwebget->list_widget_), (gchar **) columns);

      gtk_clist_set_row_data(GTK_CLIST(gwebget->list_widget_),
                             GTK_CLIST(gwebget->list_widget_)->rows - 1,
                             web_urls[i]);
    }
  }
}

void
GWebGet::
exit_activate_cb(GtkWidget *, gpointer)
{
  gtk_main_quit();
}

void
GWebGet::
help_activate_cb(GtkWidget *, gpointer)
{
}
