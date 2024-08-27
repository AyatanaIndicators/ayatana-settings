#ifndef __GLIB__
#define __GLIB__

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

static inline void file_Copy (gchar *sPathIn, gchar *sPathOut)
{
    GError *pError = NULL;
    GFile *pFileIn = g_file_new_for_path (sPathIn);
    GFile *pFileOut = g_file_new_for_path (sPathOut);
    g_file_copy (pFileIn, pFileOut, G_FILE_COPY_NONE, NULL, NULL, NULL, &pError);
    g_object_unref (pFileIn);
    g_object_unref (pFileOut);

    if (pError)
    {
        g_error ("Panic: Failed copying file from %s to %s: %s", sPathIn, sPathOut, pError->message);
        g_clear_error (&pError);

        return;
    }
}

static inline gboolean string_Equal (gchar *sText, gchar *sText2)
{
    gint nEquality = g_strcmp0 (sText, sText2);

    return (nEquality == 0);
}

static inline guint string_Length (gchar *sText)
{
    glong nLength = g_utf8_strlen (sText, -1);

    return nLength;
}

static inline gchar* string_Replace (gchar *sText, gchar *sFind, gchar *sReplace)
{
    GString *sString = g_string_new (sText);
    g_string_replace (sString, sFind, sReplace, 0);
    gchar *sNewText = g_string_free_and_steal (sString);

    return sNewText;
}

static inline gchar* string_Remove (gchar *sText, gchar *sRemove)
{
    gchar *sNewText = string_Replace (sText, sRemove, "");

    return sNewText;
}

#define string_ToInt atoi

G_END_DECLS

#endif
