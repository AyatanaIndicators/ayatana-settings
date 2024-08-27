/*
    Copyright 2020-2024 Robert Tari <robert@tari.in>

    This file is part of Ayatana Settings.

    Ayatana Settings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ayatana Settings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ayatana Settings. If not, see <http://www.gnu.org/licenses/gpl-3.0.txt>.
*/

#include "glib.h"
#include "gtk.h"

static gboolean m_bInit = FALSE;
static gboolean m_bMate = FALSE;
static GSettings *m_pSettingsSession = NULL;
static GSettings *m_pSettingsDateTime = NULL;
static GSettings *m_pSettingsSound = NULL;
static GSettings *m_pSettingsPower = NULL;
static GSettings *m_pSettingsNotifications = NULL;

static gboolean isEnabled (gchar *sIndicator)
{
    gboolean bEnabled = TRUE;
    const gchar *sHome = g_get_home_dir ();
    gchar *sDesktop = g_strdup_printf (".config/autostart/ayatana-indicator-%s.desktop", sIndicator);
    gchar *sPath = g_build_filename (sHome, sDesktop, NULL);
    g_free (sDesktop);
    gboolean bExists = g_file_test (sPath, G_FILE_TEST_EXISTS);

    if (bExists)
    {
        GKeyFile *pKeyFile = g_key_file_new ();
        GError *pError = NULL;
        g_key_file_load_from_file (pKeyFile, sPath, G_KEY_FILE_NONE, &pError);

        if (pError)
        {
            g_key_file_free (pKeyFile);
            g_free (sPath);
            g_error ("Panic: Failed loading key file: %s", pError->message);
            g_clear_error (&pError);

            return FALSE;
        }

        gboolean bKey = g_key_file_has_key (pKeyFile, "Desktop Entry", "X-MATE-Autostart-enabled", &pError);

        if (pError)
        {
            g_key_file_free (pKeyFile);
            g_free (sPath);
            g_error ("Panic: Failed parsing key file for X-MATE-Autostart-enabled: %s", pError->message);
            g_clear_error (&pError);

            return FALSE;
        }

        if (bKey)
        {
            bEnabled = g_key_file_get_boolean (pKeyFile, "Desktop Entry", "X-MATE-Autostart-enabled", &pError);

            if (pError)
            {
                g_key_file_free (pKeyFile);
                g_free (sPath);
                g_error ("Panic: Failed getting the value of X-MATE-Autostart-enabled from the key file: %s", pError->message);
                g_clear_error (&pError);

                return FALSE;
            }
        }
        else
        {
            gboolean bKey = g_key_file_has_key (pKeyFile, "Desktop Entry", "X-GNOME-Autostart-enabled", &pError);

            if (pError)
            {
                g_key_file_free (pKeyFile);
                g_free (sPath);
                g_error ("Panic: Failed parsing key file for X-GNOME-Autostart-enabled: %s", pError->message);
                g_clear_error (&pError);

                return FALSE;
            }

            if (bKey)
            {
                bEnabled = g_key_file_get_boolean (pKeyFile, "Desktop Entry", "X-GNOME-Autostart-enabled", &pError);

                if (pError)
                {
                    g_key_file_free (pKeyFile);
                    g_free (sPath);
                    g_error ("Panic: Failed getting the value of X-GNOME-Autostart-enabled from the key file: %s", pError->message);
                    g_clear_error (&pError);

                    return FALSE;
                }
            }
        }

        g_key_file_free (pKeyFile);
    }

    g_free (sPath);

    return bEnabled;
}

static void onToggleButtonEnableToggled (GtkToggleButton *pButton, gpointer pData)
{
    if (!m_bInit)
    {
        return;
    }

    const gchar *sHome = g_get_home_dir ();
    gchar *sAutostartFolder = g_build_filename (sHome, ".config/autostart", NULL);
    gboolean bExists = g_file_test (sAutostartFolder, G_FILE_TEST_IS_DIR);

    if (!bExists)
    {
        g_mkdir_with_parents (sAutostartFolder, 755);
    }

    g_free (sAutostartFolder);
    const gchar *sIndicator = gtk_widget_get_name (GTK_WIDGET (pButton));
    gchar *sDesktop = g_strdup_printf (".config/autostart/ayatana-indicator-%s.desktop", sIndicator);
    gchar *sPath = g_build_filename (sHome, sDesktop, NULL);
    g_free (sDesktop);
    bExists = g_file_test (sPath, G_FILE_TEST_EXISTS);

    if (!bExists)
    {
        gchar *sPathIn = g_strdup_printf ("/etc/xdg/autostart/ayatana-indicator-%s.desktop", sIndicator);
        file_Copy (sPathIn, sPath);
        g_free (sPathIn);
    }

    GKeyFile *pKeyFile = g_key_file_new ();
    GError *pError = NULL;
    g_key_file_load_from_file (pKeyFile, sPath, G_KEY_FILE_NONE, &pError);

    if (pError)
    {
        g_key_file_free (pKeyFile);
        g_free (sPath);
        g_error ("Panic: Failed loading key file: %s", pError->message);
        g_clear_error (&pError);

        return;
    }

    gboolean bEnabled = gtk_toggle_button_get_active (pButton);

    if (m_bMate)
    {
        g_key_file_set_boolean (pKeyFile, "Desktop Entry", "X-MATE-Autostart-enabled", bEnabled);
    }
    else
    {
        g_key_file_set_boolean (pKeyFile, "Desktop Entry", "X-GNOME-Autostart-enabled", bEnabled);
    }

    g_key_file_save_to_file (pKeyFile, sPath, &pError);

    if (pError)
    {
        g_key_file_free (pKeyFile);
        g_free (sPath);
        g_error ("Panic: Failed saving key file: %s", pError->message);
        g_clear_error (&pError);

        return;
    }

    g_key_file_free (pKeyFile);
    g_free (sPath);
}

static void onSpinButtonNotificationsValueChanged (GtkSpinButton *pSpinButton, gpointer pData)
{
    gint nValue = gtk_spin_button_get_value_as_int (pSpinButton);
    g_settings_set_int (m_pSettingsNotifications, "max-items", nValue);
}

static void saveFilterList ()
{
    GStrvBuilder *pBuilder = g_strv_builder_new ();
    GtkStringList *pStringList = GTK_STRING_LIST (application_GetObject ("StringListNotificationsFilters"));
    guint nItems = g_list_model_get_n_items (G_LIST_MODEL (pStringList));

    for (guint nItem = 0; nItem < nItems; nItem++)
    {
        const gchar *sModelText = gtk_string_list_get_string (pStringList, nItem);
        g_strv_builder_add (pBuilder, (gchar*) sModelText);
    }

    GStrv lItems = g_strv_builder_end (pBuilder);
    g_strv_builder_unref (pBuilder);
    g_settings_set_strv (m_pSettingsNotifications, "filter-list", (const char* const*) lItems);
    g_strfreev (lItems);
}

static void onButtonNotificationsAddClicked (GtkButton *pButton, gpointer pData)
{
    GObject *pEntryNotifications = application_GetObject ("EntryNotifications");
    const gchar *sText = gtk_editable_get_text (GTK_EDITABLE (pEntryNotifications));
    gchar *sTextDup = g_strdup (sText);
    g_strstrip (sTextDup);
    guint nLength = string_Length (sTextDup);

    if (nLength)
    {
        gboolean bDuplicate = FALSE;
        GtkStringList *pStringList = GTK_STRING_LIST (application_GetObject ("StringListNotificationsFilters"));
        guint nItems = g_list_model_get_n_items (G_LIST_MODEL (pStringList));

        for (guint nItem = 0; nItem < nItems; nItem++)
        {
            const gchar *sModelText = gtk_string_list_get_string (pStringList, nItem);
            gboolean bEqual = string_Equal ((gchar*) sModelText, (gchar*) sText);

            if (bEqual)
            {
                bDuplicate = TRUE;

                break;
            }
        }

        if (!bDuplicate)
        {
            gtk_string_list_append (pStringList, sTextDup);
            saveFilterList ();
        }
    }

    g_free (sTextDup);
    gtk_editable_set_text (GTK_EDITABLE (pEntryNotifications), "");
}

static void onButtonNotificationsRemoveClicked (GtkButton *pButton, gpointer pData)
{
    GtkSingleSelection *pSelection = GTK_SINGLE_SELECTION (application_GetObject ("SingleSelectionNotifications"));
    guint nSelected = gtk_single_selection_get_selected (pSelection);
    GtkStringList *pStringList = GTK_STRING_LIST (application_GetObject ("StringListNotificationsFilters"));

    if (nSelected != GTK_INVALID_LIST_POSITION)
    {
        gtk_string_list_remove (pStringList, nSelected);
        saveFilterList ();
    }
}

static void onActivate ()
{
    GSettingsSchemaSource *pSource = g_settings_schema_source_get_default ();
    GSettingsSchema *pSchema = NULL;
    GObject *pSwitch = NULL;

    // Session
    pSchema = g_settings_schema_source_lookup (pSource, "org.ayatana.indicator.session", FALSE);

    if (pSchema)
    {
        g_settings_schema_unref (pSchema);
        m_pSettingsSession = g_settings_new ("org.ayatana.indicator.session");
        pSwitch = application_GetObject ("SwitchSessionShowName");
        g_settings_bind (m_pSettingsSession, "show-real-name-on-panel", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchSessionRemoveLogOut");
        g_settings_bind (m_pSettingsSession, "suppress-logout-menuitem", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchSessionSuppressConfirmation");
        g_settings_bind (m_pSettingsSession, "suppress-logout-restart-shutdown", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchSessionRemoveRestart");
        g_settings_bind (m_pSettingsSession, "suppress-restart-menuitem", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchSessionRemoveShutDown");
        g_settings_bind (m_pSettingsSession, "suppress-shutdown-menuitem", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchSessionShowUsers");
        g_settings_bind (m_pSettingsSession, "user-show-menu", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        gboolean bEnabled = isEnabled ("session");
        GtkToggleButton *pToggleButton = GTK_TOGGLE_BUTTON (application_GetObject ("ToggleButtonSessionEnable"));
        gtk_toggle_button_set_active (pToggleButton, bEnabled);
    }
    else
    {
        notebook_DisablePage ("NotebookPageSession");
    }
    //~Session

    // Date/Time
    pSchema = g_settings_schema_source_lookup (pSource, "org.ayatana.indicator.datetime", FALSE);

    if (pSchema)
    {
        g_settings_schema_unref (pSchema);
        m_pSettingsDateTime = g_settings_new ("org.ayatana.indicator.datetime");
        pSwitch = application_GetObject ("SwitchDatetimeShowCalendar");
        g_settings_bind (m_pSettingsDateTime, "show-calendar", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchDatetimeShowDate");
        g_settings_bind (m_pSettingsDateTime, "show-date", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchDatetimeShowDay");
        g_settings_bind (m_pSettingsDateTime, "show-day", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchDatetimeShowEvents");
        g_settings_bind (m_pSettingsDateTime, "show-events", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchDatetimeShowSeconds");
        g_settings_bind (m_pSettingsDateTime, "show-seconds", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchDatetimeShowWeekNumbers");
        g_settings_bind (m_pSettingsDateTime, "show-week-numbers", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchDatetimeShowYear");
        g_settings_bind (m_pSettingsDateTime, "show-year", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        gboolean bEnabled = isEnabled ("datetime");
        GtkToggleButton *pToggleButton = GTK_TOGGLE_BUTTON (application_GetObject ("ToggleButtonDatetimeEnable"));
        gtk_toggle_button_set_active (pToggleButton, bEnabled);
    }
    else
    {
        notebook_DisablePage ("NotebookPageDatetime");
    }
    //~Date/Time

    // Sound
    pSchema = g_settings_schema_source_lookup (pSource, "org.ayatana.indicator.sound", FALSE);

    if (pSchema)
    {
        g_settings_schema_unref (pSchema);
        m_pSettingsSound = g_settings_new ("org.ayatana.indicator.sound");
        pSwitch = application_GetObject ("SwitchSoundAllowAmplified");
        g_settings_bind (m_pSettingsSound, "allow-amplified-volume", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        gboolean bEnabled = isEnabled ("sound");
        GtkToggleButton *pToggleButton = GTK_TOGGLE_BUTTON (application_GetObject ("ToggleButtonSoundEnable"));
        gtk_toggle_button_set_active (pToggleButton, bEnabled);
    }
    else
    {
        notebook_DisablePage ("NotebookPageSound");
    }
    //~Sound

    // Power
    pSchema = g_settings_schema_source_lookup (pSource, "org.ayatana.indicator.power", FALSE);

    if (pSchema)
    {
        g_settings_schema_unref (pSchema);
        m_pSettingsPower = g_settings_new ("org.ayatana.indicator.power");
        pSwitch = application_GetObject ("SwitchPowerShowPercentage");
        g_settings_bind (m_pSettingsPower, "show-percentage", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        pSwitch = application_GetObject ("SwitchPowerShowTime");
        g_settings_bind (m_pSettingsPower, "show-time", pSwitch, "active", G_SETTINGS_BIND_DEFAULT);
        gboolean bEnabled = isEnabled ("power");
        GtkToggleButton *pToggleButton = GTK_TOGGLE_BUTTON (application_GetObject ("ToggleButtonPowerEnable"));
        gtk_toggle_button_set_active (pToggleButton, bEnabled);
    }
    else
    {
        notebook_DisablePage ("NotebookPagePower");
    }
    //~Power

    // Messages
    pSchema = g_settings_schema_source_lookup (pSource, "org.ayatana.indicator.messages", FALSE);

    if (pSchema)
    {
        g_settings_schema_unref (pSchema);
        gboolean bEnabled = isEnabled ("messages");
        GtkToggleButton *pToggleButton = GTK_TOGGLE_BUTTON (application_GetObject ("ToggleButtonMessagesEnable"));
        gtk_toggle_button_set_active (pToggleButton, bEnabled);
    }
    else
    {
        notebook_DisablePage ("NotebookPageMessages");
    }
    //~Messages

    // Bluetooth
    pSchema = g_settings_schema_source_lookup (pSource, "org.ayatana.indicator.bluetooth", FALSE);

    if (pSchema)
    {
        g_settings_schema_unref (pSchema);
        gboolean bEnabled = isEnabled ("bluetooth");
        GtkToggleButton *pToggleButton = GTK_TOGGLE_BUTTON (application_GetObject ("ToggleButtonBluetoothEnable"));
        gtk_toggle_button_set_active (pToggleButton, bEnabled);
    }
    else
    {
        notebook_DisablePage ("NotebookPageBluetooth");
    }
    //~Bluetooth

    // Notifications
    pSchema = g_settings_schema_source_lookup (pSource, "org.ayatana.indicator.notifications", FALSE);

    if (pSchema)
    {
        g_settings_schema_unref (pSchema);
        m_pSettingsNotifications = g_settings_new ("org.ayatana.indicator.notifications");
        gint nMaxItems = g_settings_get_int (m_pSettingsNotifications, "max-items");
        GtkSpinButton *pSpinButton = GTK_SPIN_BUTTON (application_GetObject ("SpinButtonNotifications"));
        gtk_spin_button_set_value (pSpinButton, nMaxItems);
        GStrv lFilters = g_settings_get_strv (m_pSettingsNotifications, "filter-list");
        guint nFilters = g_strv_length (lFilters);
        listview_SetStringListFactory ("ListViewNotifications");
        GtkStringList *pStringList = GTK_STRING_LIST (application_GetObject ("StringListNotificationsFilters"));

        for (guint nFilter = 0; nFilter < nFilters; nFilter++)
        {
            gtk_string_list_append (pStringList, lFilters[nFilter]);
        }

        g_strfreev (lFilters);
        GStrv lHints = g_settings_get_strv (m_pSettingsNotifications, "filter-list-hints");
        guint nHints = g_strv_length (lHints);
        pStringList = GTK_STRING_LIST (application_GetObject ("StringListNotificationsHints"));

        for (guint nHint = 0; nHint < nHints; nHint++)
        {
            gtk_string_list_append (pStringList, lHints[nHint]);
        }

        g_strfreev (lHints);
        // TODO: Entry completion replacement goes here, once available
        gboolean bEnabled = isEnabled ("notifications");
        GtkToggleButton *pToggleButton = GTK_TOGGLE_BUTTON (application_GetObject ("ToggleButtonNotificationsEnable"));
        gtk_toggle_button_set_active (pToggleButton, bEnabled);
    }
    else
    {
        notebook_DisablePage ("NotebookPageNotifications");
    }
    //~Notifications

    // Keyboard
    pSchema = g_settings_schema_source_lookup (pSource, "org.ayatana.indicator.keyboard", FALSE);

    if (pSchema)
    {
        gboolean bEnabled = isEnabled ("keyboard");
        GtkToggleButton *pToggleButton = GTK_TOGGLE_BUTTON (application_GetObject ("ToggleButtonKeyboardEnable"));
        gtk_toggle_button_set_active (pToggleButton, bEnabled);
    }
    else
    {
        notebook_DisablePage ("NotebookPageKeyboard");
    }
    //~Keyboard

    m_bInit = TRUE;
}

static void onShutdown ()
{
    g_clear_object (&m_pSettingsSession);
    g_clear_object (&m_pSettingsDateTime);
    g_clear_object (&m_pSettingsSound);
    g_clear_object (&m_pSettingsPower);
    g_clear_object (&m_pSettingsNotifications);
}

static void onStartup ()
{
    const gchar *sDesktop = g_getenv ("XDG_CURRENT_DESKTOP");
    m_bMate = string_Equal ((gchar*) sDesktop, "MATE");
}

gint main (gint argc, gchar *argv[])
{
    ApplicationCallbackSymbol lSymbols[] =
    {
        {"onToggleButtonEnableToggled", G_CALLBACK (onToggleButtonEnableToggled)},
        {"onButtonNotificationsRemoveClicked", G_CALLBACK (onButtonNotificationsRemoveClicked)},
        {"onButtonNotificationsAddClicked", G_CALLBACK (onButtonNotificationsAddClicked)},
        {"onSpinButtonNotificationsValueChanged", G_CALLBACK (onSpinButtonNotificationsValueChanged)},
        {NULL, NULL}
    };

    gint nStatus = application_Init (lSymbols, "org.ayatana.ayatana-settings", onStartup, onActivate, onShutdown, argc, argv);

    return nStatus;
}
