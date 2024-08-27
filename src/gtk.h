#ifndef __GTK__
#define __GTK__

#include <glib/gi18n.h>
#include <locale.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct
{
   gchar *sId;
   GCallback pCallback;
} ApplicationCallbackSymbol;

typedef struct
{
   gchar *sUi;
   GCallback pCallback;
   ApplicationCallbackSymbol *lSymbols;
} ApplicationStartup;

static GtkBuilder *m_pBuilder = NULL;
static GSettings *m_pDesktopSettings = NULL;
static GSettings *m_pA11ySettings = NULL;

static inline void application_OnShutdown (GApplication *pApplication, gpointer pData)
{
    if (pData)
    {
        GCallback pCallback = (GCallback) pData;
        pCallback ();
    }

    g_object_unref (m_pBuilder);
    g_clear_object (&m_pDesktopSettings);
    g_clear_object (&m_pA11ySettings);
}

static inline GObject* application_GetObject (gchar *sId)
{
    GObject *pObject = gtk_builder_get_object (m_pBuilder, sId);

    return pObject;
}

static inline void application_OnActivate (GApplication *pApplication, gpointer pData)
{
    GList *lWindows = gtk_application_get_windows (GTK_APPLICATION (pApplication));

    if (lWindows)
    {
        return;
    }

    GObject *pWindow = gtk_builder_get_object (m_pBuilder, "window");
    gtk_window_present (GTK_WINDOW (pWindow));
    gtk_application_add_window (GTK_APPLICATION (pApplication), GTK_WINDOW (pWindow));
    GCallback pCallback = (GCallback) pData;

    if (pCallback)
    {
        pCallback ();
    }
}

static inline void application_onThemeChanged (GSettings *pSettings, const gchar *sKey, gpointer pData)
{
    gchar *sColorScheme = g_settings_get_string (m_pDesktopSettings, "color-scheme");
    gboolean bDark = string_Equal (sColorScheme, "prefer-dark");
    g_free (sColorScheme);
    gboolean bHighContrast = g_settings_get_boolean (m_pA11ySettings, "high-contrast");
    GtkSettings *pGtkSettings = gtk_settings_get_default ();

    if (bHighContrast && bDark)
    {
        g_object_set (pGtkSettings, "gtk-theme-name", "Default-hc-dark", NULL);
    }
    else if (bHighContrast)
    {
        g_object_set (pGtkSettings, "gtk-theme-name", "Default-hc", NULL);
    }
    else if (bDark)
    {
        gtk_settings_reset_property (pGtkSettings, "gtk-theme-name");
    }
    else
    {
        gtk_settings_reset_property (pGtkSettings, "gtk-theme-name");
    }

    g_object_set (pGtkSettings, "gtk-application-prefer-dark-theme", bDark, NULL);
}

static inline void application_OnStartup (GApplication *pApplication, gpointer pData)
{
    ApplicationStartup *pStartup = (ApplicationStartup*) pData;
    m_pBuilder = gtk_builder_new ();
    GtkBuilderScope *pScope = gtk_builder_get_scope (m_pBuilder);
    guint nId = 0;

    while (pStartup->lSymbols[nId].pCallback != NULL)
    {
        gtk_builder_cscope_add_callback_symbol (GTK_BUILDER_CSCOPE (pScope), pStartup->lSymbols[nId].sId, pStartup->lSymbols[nId].pCallback);
        nId++;
    }

    gtk_builder_add_from_resource (m_pBuilder, pStartup->sUi, NULL);
    g_free (pStartup->sUi);
    gtk_builder_set_translation_domain (m_pBuilder, GETTEXT_PACKAGE);
    GSettingsSchemaSource *pSource = g_settings_schema_source_get_default ();
    GSettingsSchema *pSchema = g_settings_schema_source_lookup (pSource, "org.gnome.desktop.interface", FALSE);

    if (pSchema)
    {
        gboolean bKey = g_settings_schema_has_key (pSchema, "color-scheme");
        g_settings_schema_unref (pSchema);

        if (bKey)
        {
            m_pDesktopSettings = g_settings_new ("org.gnome.desktop.interface");
            g_signal_connect (m_pDesktopSettings, "changed::color-scheme", G_CALLBACK (application_onThemeChanged), NULL);
        }
    }

    pSchema = g_settings_schema_source_lookup (pSource, "org.gnome.desktop.a11y.interface", FALSE);

    if (pSchema)
    {
        gboolean bKey = g_settings_schema_has_key (pSchema, "high-contrast");
        g_settings_schema_unref (pSchema);

        if (bKey)
        {
            m_pA11ySettings = g_settings_new ("org.gnome.desktop.a11y.interface");
            g_signal_connect (m_pA11ySettings, "changed::high-contrast", G_CALLBACK (application_onThemeChanged), NULL);
        }
    }

    if (m_pDesktopSettings && m_pA11ySettings)
    {
        application_onThemeChanged (NULL, NULL, NULL);
    }

    if (pStartup->pCallback)
    {
        pStartup->pCallback ();
    }
}

static inline gint application_Init (ApplicationCallbackSymbol *lSymbols, gchar *sId, GCallback pOnStartup, GCallback pOnActivate, GCallback pOnShutdown, gint argc, gchar *argv[])
{
    setlocale (LC_ALL, "");
    bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
    textdomain (GETTEXT_PACKAGE);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    GtkApplication *pApplication = gtk_application_new (sId, G_APPLICATION_DEFAULT_FLAGS);
    gchar *sReplaced = string_Replace (sId, ".", "/");
    gchar *sConcatenated = g_strconcat ("/", sReplaced, "/", GETTEXT_PACKAGE, ".ui", NULL);
    g_free (sReplaced);
    ApplicationStartup cApplicationStartup = {sConcatenated, pOnStartup, lSymbols};
    g_signal_connect (pApplication, "startup", G_CALLBACK (application_OnStartup), &cApplicationStartup);
    g_signal_connect (pApplication, "activate", G_CALLBACK (application_OnActivate), pOnActivate);
    g_signal_connect (pApplication, "shutdown", G_CALLBACK (application_OnShutdown), pOnShutdown);
    gint nStatus = g_application_run (G_APPLICATION (pApplication), argc, argv);
    g_object_unref (pApplication);

    return nStatus;
}

static inline void listview_SetStringListFactory (gchar *sListView)
{
    const char *sFactory =
    "<?xml version='1.0' encoding='UTF-8'?>\n"
    "<interface>\n"
    "  <template class='GtkListItem'>\n"
    "    <property name='child'>\n"
    "      <object class='GtkInscription'>\n"
    "        <property name='xalign'>0</property>\n"
    "        <binding name='text'>\n"
    "          <lookup name='string' type='GtkStringObject'>\n"
    "            <lookup name='item'>GtkListItem</lookup>\n"
    "          </lookup>\n"
    "        </binding>\n"
    "      </object>\n"
    "    </property>\n"
    "  </template>\n"
    "</interface>\n";

    gsize nSize = strlen (sFactory);
    GBytes *pBytes = g_bytes_new_static (sFactory, nSize);
    GtkListItemFactory *pFactory = gtk_builder_list_item_factory_new_from_bytes (NULL, pBytes);
    GtkListView *pListView = GTK_LIST_VIEW (application_GetObject (sListView));
    gtk_list_view_set_factory (pListView, pFactory);
}

static inline void notebook_DisablePage (gchar *sPage)
{
    GtkNotebookPage *pPage = GTK_NOTEBOOK_PAGE (application_GetObject (sPage));
    GtkWidget *pChild = gtk_notebook_page_get_child (pPage);
    gtk_widget_set_sensitive (pChild, FALSE);
    GtkWidget *pTab = NULL;
    g_object_get (pPage, "tab", &pTab, NULL);
    gtk_widget_set_sensitive (pTab, FALSE);
    g_object_unref (pTab);
}

G_END_DECLS

#endif
