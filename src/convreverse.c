/*
 * ConvReverse - reverse every word typed
 * Copyright (C) 2009 Angelo D'Autilia aka sYdRo <sydro@salug.it>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1301, USA.
 *
 *
 */


/* TODO

   - Eliminare la formattazione quando si attiva il plugin

*/


#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define PURPLE_PLUGINS

#include <conversation.h>
#include <gtkconv.h>
#include <gtkplugin.h>
#include <version.h>
#include <gtkprefs.h>

#include <string.h>

#include "convreverse.h"

static const char * const PREF_DISABLED = "/plugins/core/convreverse/bool_disabled";

PurplePlugin *convreverse_plugin = NULL;


//REPLACE HTML CHAR TAG CODE
static void
replace(char *message[], const char *strRepl, const char *repl){

  char buffer[strlen(message[0])];
  char *p;
  char strTemp[strlen(message[0])];

  strcpy(strTemp,message[0]);
  strcpy(buffer,"");

  while(p = strstr(strTemp,strRepl)) {
    strncat(buffer,strTemp,p-strTemp);
    strcat(buffer,repl);
    strcpy(strTemp,p+strlen(strRepl));
  }

  if (strTemp != NULL ) strcat(buffer,strTemp);

  strcpy(message[0], buffer);

}

static void
detach_from_gtkconv(PidginConversation *gtkconv, gpointer null)
{
  GtkWidget *box, *OnOffLabel = NULL;

  box = gtkconv->toolbar;
  OnOffLabel = g_object_get_data(G_OBJECT(box), PLUGIN_ID "-onofflabel");
  if (OnOffLabel)
    gtk_container_remove(GTK_CONTAINER(box), OnOffLabel);

  gtk_widget_queue_draw(pidgin_conv_get_window(gtkconv)->window);
}


static void
attach_to_gtkconv(PidginConversation *gtkconv, gpointer null)
{
  GtkWidget *box, *OnOffLabel;
  box = gtkconv->toolbar;
  OnOffLabel = g_object_get_data(G_OBJECT(box), PLUGIN_ID "-onofflabel");
  g_return_if_fail(OnOffLabel == NULL);

  OnOffLabel = gtk_label_new(NULL);
  gtk_widget_set_name(OnOffLabel, "convreverse_label");
  gboolean flag = !purple_prefs_get_bool("/plugins/gtk/convreverse/bool_disabled");

  if (flag) {
    gtk_label_set_text(GTK_LABEL(OnOffLabel), "R");
  }
  else {
    gtk_label_set_text(GTK_LABEL(OnOffLabel), "D");
  }


  gtk_box_pack_end(GTK_BOX(box), OnOffLabel, FALSE, FALSE,0);
  gtk_widget_show_all(OnOffLabel);

  g_object_set_data(G_OBJECT(box), PLUGIN_ID "-onofflabel", OnOffLabel);

  gtk_widget_queue_draw(pidgin_conv_get_window(gtkconv)->window);

}

static void
detach_from_pidgin_window(PidginWindow *win, gpointer null)
{
  g_list_foreach(pidgin_conv_window_get_gtkconvs(win), (GFunc)detach_from_gtkconv, NULL);
}

static void
attach_to_pidgin_window(PidginWindow *win, gpointer null)
{
  g_list_foreach(pidgin_conv_window_get_gtkconvs(win), (GFunc)attach_to_gtkconv, NULL);
}

static void
detach_from_all_windows()
{
  g_list_foreach(pidgin_conv_windows_get_list(), (GFunc)detach_from_pidgin_window, NULL);
}

static void
attach_to_all_windows()
{
  g_list_foreach(pidgin_conv_windows_get_list(), (GFunc)attach_to_pidgin_window, NULL);
}

static void
change_to_gtkconv(PidginConversation *gtkconv, gpointer null)
{

  g_return_if_fail(gtkconv != NULL);

  GtkWidget *box, *OnOffLabel = NULL;
  box = gtkconv->toolbar;

  OnOffLabel = g_object_get_data(G_OBJECT(box), PLUGIN_ID "-onofflabel");

  gboolean flag = !purple_prefs_get_bool("/plugins/gtk/convreverse/bool_disabled");

  if(OnOffLabel){
    if (flag) {
      gtk_label_set_text(GTK_LABEL(OnOffLabel), "R");
    }
    else {
      gtk_label_set_text(GTK_LABEL(OnOffLabel), "D");
    }
  }

  gtk_box_pack_end(GTK_BOX(box), OnOffLabel, FALSE, FALSE,0);
  gtk_widget_show_all(OnOffLabel);
  gtk_widget_queue_draw(pidgin_conv_get_window(gtkconv)->window);

}

static void
change_to_pidgin_window(PidginWindow *win, gpointer null)
{
  g_list_foreach(pidgin_conv_window_get_gtkconvs(win), (GFunc)change_to_gtkconv, NULL);
}

static void
change_to_all_windows()
{
  g_list_foreach(pidgin_conv_windows_get_list(), (GFunc)change_to_pidgin_window, NULL);
}


static void
conversation_create_cb(PurpleConversation *conv, gpointer null)
{
  PidginConversation *gtkconv = PIDGIN_CONVERSATION(conv);
  g_return_if_fail(gtkconv != NULL);
  attach_to_gtkconv(gtkconv, NULL);
}

//FUCTION TO REVERSE CONVERSATION
static void
conversation_reverse_cb(PurpleAccount *account, const char *receiver, char **message)
{

  gboolean flag = !purple_prefs_get_bool("/plugins/gtk/convreverse/bool_disabled");

  if(flag){

    replace(&message[0],"&amp;","&");
    replace(&message[0],"&quot;","\"");
    replace(&message[0],"&lt;","<");
    replace(&message[0],"&gt;",">");

    int size = strlen(message[0]);
    char *message_temp;
    message_temp = (char*)malloc((size*4+1)*sizeof(char));
    int j=size-1;
    int i=0;


    for(i=0; i<j; i++)
      {
        char temp = message[0][i];
        message[0][i] = message[0][j];
        message[0][j] = temp;
        j--;
      }


    i=0;
    j=0;

    strcpy(message_temp,"");

    while(message[0][i] != '\0'){

      int temp_code = (int)message[0][i];
      int offset = 0;
      int codificato = FALSE;

      if (temp_code >= 33 && temp_code <=126) { codificato=TRUE; offset = 33; }

      int pos = temp_code - offset;


      if (codificato)
        strcat(message_temp,coding[pos]);
      else{
        if (pos < 0 ) {
          //caratteri fuori dalla tabella ascii standard
          //NB: non e' una soluzione ottimale, problema con i caratteri a 3 byte
          char tempchar[3];
          tempchar[0] = message[0][i];
          tempchar[1] = message[0][i+1];
          tempchar[2] = '\0';
          strcat(message_temp,tempchar);
          i++;
        }
        else {
          char tempchar[2];
          tempchar[0] = message[0][i];
          tempchar[1] = '\0';
          strcat(message_temp,tempchar);
        }

      }

      i++;
    }


    int size_msgtemp = strlen(message_temp) + 1;
    message[0] = message_temp;
    message[0][size_msgtemp]='\0';
  }
}


static void
active_deactive()
{

  gboolean flag = !purple_prefs_get_bool("/plugins/gtk/convreverse/bool_disabled");

  if (flag) {
    purple_prefs_set_bool("/plugins/gtk/convreverse/bool_disabled", TRUE);
    purple_notify_message(convreverse_plugin, PURPLE_NOTIFY_MSG_ERROR,
                          "ConvReverse Plugin", "You're not writing flipped yet:(",
                          NULL, NULL, NULL);
    change_to_all_windows();
  }
  else
    {
      purple_prefs_set_bool("/plugins/gtk/convreverse/bool_disabled", FALSE);
      purple_notify_message(convreverse_plugin, PURPLE_NOTIFY_MSG_INFO,
                            "ConvReverse Plugin", "You're writing flipped now...yeah :)",
                            NULL, NULL, NULL);
      change_to_all_windows();
    }

}

// actions callback function
static void
plugin_action_test_cb(PurplePluginAction *action)
{

  active_deactive();

}


//Action events handler
static GList *
plugin_actions(PurplePlugin *plugin, gpointer context)
{

  GList *list = NULL;
  PurplePluginAction *action = NULL;

  action = purple_plugin_action_new("Active/Deactive ConvReverse", plugin_action_test_cb);

  list = g_list_append(list, action);

  return list;
}


static void
convrev_menu_cb(PurpleBlistNode *node, GList **menu, gpointer plugin)
{
  PurpleMenuAction *action;

  if (!PURPLE_BLIST_NODE_IS_BUDDY(node) && !PURPLE_BLIST_NODE_IS_CONTACT(node))
    return;

  action = purple_menu_action_new("Convereverse Active/Deactive",
                                  PURPLE_CALLBACK(active_deactive), plugin, NULL);
  (*menu) = g_list_prepend(*menu, action);
}

// PLUGIN LOAD FUNCTION
static gboolean
plugin_load(PurplePlugin *plugin)
{

  purple_prefs_set_bool("/plugins/gtk/convreverse/bool_disabled", TRUE);

  purple_signal_connect(purple_conversations_get_handle(), "conversation-created", plugin, PURPLE_CALLBACK(conversation_create_cb), NULL);
  purple_signal_connect(purple_conversations_get_handle(), "sending-im-msg", plugin, PURPLE_CALLBACK(conversation_reverse_cb), NULL);
  purple_signal_connect(purple_blist_get_handle(), "blist-node-extended-menu", plugin, PURPLE_CALLBACK(convrev_menu_cb), NULL);

  convreverse_plugin = plugin;

  attach_to_all_windows();
  return TRUE;
}



// PLUGIN UNLOAD FUNCTION
static gboolean
plugin_unload(PurplePlugin *plugin)
{
  detach_from_all_windows();

  return TRUE;
}


static PurplePluginInfo info =
  {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,                           /**< major version */
    PURPLE_MINOR_VERSION,                           /**< minor version */
    PURPLE_PLUGIN_STANDARD,                         /**< type */
    PIDGIN_PLUGIN_TYPE,                             /**< ui_requirement */
    0,                                              /**< flags */
    NULL,                                           /**< dependencies */
    PURPLE_PRIORITY_DEFAULT,                        /**< priority */
    PLUGIN_ID,                                      /**< id */
    PLUGIN_NAME,                                    /**< name */
    PLUGIN_VERSION,                                 /**< version */
    PLUGIN_SUMMARY,                                 /**< summary */
    PLUGIN_DESCRIPTION,                             /**< description */
    PLUGIN_AUTHOR,                                  /**< author */
    PLUGIN_WEBSITE,                                 /**< homepage */
    plugin_load,                                    /**< load */
    plugin_unload,                                  /**< unload */
    NULL,                                           /**< destroy */
    NULL,                                           /**< ui_info */
    NULL,                                           /**< extra_info */
    NULL,                                           /**< prefs_info */
    plugin_actions,                                 /**< actions */
    NULL,
    NULL,
    NULL,
    NULL
  };

static void
init_plugin(PurplePlugin *plugin)
{

  purple_prefs_add_none("/plugins/gtk");
  purple_prefs_add_none("/plugins/gtk/convreverse");
  purple_prefs_add_bool("/plugins/gtk/convreverse/bool_disabled", TRUE);

}


PURPLE_INIT_PLUGIN(ConvReverse, init_plugin, info)
