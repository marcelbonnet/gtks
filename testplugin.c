#include <lxpanel/plugin.h>
#include <stdio.h>
#include <pulse/simple.h>
#include <pulse/error.h>

/*
 * DEPENDÊNCIAS
 * 
 * sudo apt install libpulse-dev lxpanel-dev gtk2 libfm-dev
 * 
 * COMPILANDO
 * 
 * alias c="gcc -Wall `pkg-config --cflags gtk+-2.0 lxpanel libpulse-simple` -shared -fPIC testplugin.c -o test.so `pkg-config --libs lxpanel libpulse-simple` && sudo cp test.so /usr/lib/arm-linux-gnueabihf/lxpanel/plugins/ && lxpanelctl restart"
 * 
 * 
 * place test.so file in /usr/lib/arm-linux-gnueabihf/lxpanel/plugins
 * restart the panel (using lxpanelctl restart)
 */
 
// internal to the plugin source, not used by the 'priv' variable
static int iInstanceCount = 0;

/* the plugin's id – an instance of this struct 
   is what will be assigned to 'priv' */
typedef struct 
{
  gint iMyId;
} TestPlugin; 

static void btn_callback( GtkWidget *widget,
                      gpointer   data )
{
    //g_print ("Hello again - %s was pressed\n", (char *) data);
    //http://askubuntu.com/questions/71863/ddg#72076 tentar mover um sink apenas
    /*
     * pacmd stat
     * Default sink ... alsa_output.platform-bcm2835_audio.analog-mono
     * pacmd list-sinks | grep alsa_output.platform-bcm2835_audio.analog-mono
     */
      gchar * cur_lbl = gtk_button_get_label(widget);
      int ret = g_strcmp0(cur_lbl, "F");
      
      
      
      if (ret == 0){ //se fones, trocar para HDMI
         system("pacmd set-default-sink 1");
         gtk_button_set_label(widget, "Tv");
         GtkWidget * dialog = gtk_message_dialog_new (NULL,
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_INFO,
                                 GTK_BUTTONS_CLOSE,
                                 "Feche o programa e abra de novo para ouvir na TV");
         gtk_dialog_run (GTK_DIALOG (dialog));
         gtk_widget_destroy (dialog);
      } else {
         system("pacmd set-default-sink 0");
         gtk_button_set_label(widget, "F");
         GtkWidget * dialog = gtk_message_dialog_new (NULL,
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_INFO,
                                 GTK_BUTTONS_CLOSE,
                                 "Feche o programa e abra de novo para ouvir nos Fones de Ouvido");
         gtk_dialog_run (GTK_DIALOG (dialog));
         gtk_widget_destroy (dialog);
      }
    /*
    pa_simple *s = NULL;
    int error;
    if (!(s = pa_simple_new(NULL, "LxPanel Plugin", PA_STREAM_NODIRECTION, NULL, "lxde lxpanel plugin", &ss, NULL, NULL, &error))) {
      GtkWidget * dialog = gtk_message_dialog_new (NULL,
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_CLOSE,
                                 "Pulse Audio Error: %i",
                                 error );
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
    }
    */
   
}

GtkWidget *test_constructor(LXPanel *panel, config_setting_t *settings)
{
 /* panel is a pointer to the panel and
     settings is a pointer to the configuration data
     since we don't use it, we'll make sure it doesn't
     give us an error at compilation time */
 (void)panel;
 (void)settings;

 // allocate our private structure instance
 TestPlugin *pTest = g_new0(TestPlugin, 1);

 // update the instance count
 pTest->iMyId = ++iInstanceCount;

 // make a label out of the ID
 char cIdBuf[10] = {'\0'};

 snprintf(cIdBuf, sizeof(cIdBuf), "TP-%d", pTest->iMyId);

 // create a label widget instance 
 GtkWidget *pLabel = gtk_label_new(cIdBuf);
 
 //GtkWidget *button = button = gtk_button_new();
 GtkWidget *button = gtk_button_new_with_label("[o]");
 //GtkWidget *button = gtk_toggle_button_new_with_label("S");
 
 g_signal_connect (button, "clicked",
		      G_CALLBACK (btn_callback), (gpointer) "cool button");

 // set the label to be visible
 //gtk_widget_show(pLabel);
 gtk_widget_show(button);

 // need to create a container to be able to set a border
 GtkWidget *p = gtk_event_box_new();

 // our widget doesn't have a window...
 // it is usually illegal to call gtk_widget_set_has_window() from application but for GtkEventBox it doesn't hurt
 gtk_widget_set_has_window(p, FALSE);

 // bind private structure to the widget assuming it should be freed using g_free()
 lxpanel_plugin_set_data(p, pTest, g_free);

 // set border width
 gtk_container_set_border_width(GTK_CONTAINER(p), 1);

 // add the label to the container
 //gtk_container_add(GTK_CONTAINER(p), pLabel);
 gtk_container_add(GTK_CONTAINER(p), button);

 // set the size we want
 gtk_widget_set_size_request(p, 40, 25);

 // success!!!
 return p;
}

FM_DEFINE_MODULE(lxpanel_gtk, test)

/* Plugin descriptor. */
LXPanelPluginInit fm_module_init_lxpanel_gtk = {
   .name = "TestPlugin",
   .description = "Run a test plugin.",

   // assigning our functions to provided pointers.
   .new_instance = test_constructor
};
