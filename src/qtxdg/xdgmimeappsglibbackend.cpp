/*
 * libqtxdg - An Qt implementation of freedesktop.org xdg specs
 * Copyright (C) 2018  Luís Pereira <luis.artur.pereira@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include "xdgmimeappsglibbackend.h"
#include "qtxdglogging.h"

#include <gio/gio.h>
#include <gio/gappinfo.h>
#include <gio/gdesktopappinfo.h>

#include <QLoggingCategory>

XdgMimeAppsGLibBackend::XdgMimeAppsGLibBackend(XdgMimeAppsPrivate *d)
    : XdgMimeAppsBackendInterface(d)
{
}

XdgMimeAppsGLibBackend::~XdgMimeAppsGLibBackend()
{
}

XdgDesktopFile *XdgMimeAppsGLibBackend::defaultApp(const QString &mimeType)
{
    GAppInfo *appinfo = g_app_info_get_default_for_type(mimeType.toUtf8().constData(), false);
    if (appinfo == nullptr || !G_IS_DESKTOP_APP_INFO(appinfo)) {
        return nullptr;
    }

    const char *file = g_desktop_app_info_get_filename(G_DESKTOP_APP_INFO(appinfo));

    if (file == nullptr) {
        g_object_unref(appinfo);
        return nullptr;
    }

    const QString s = QString::fromUtf8(file);
    g_object_unref(appinfo);

    XdgDesktopFile *f = new XdgDesktopFile;
    if (f->load(s) && f->isValid())
        return f;

    delete f;
    return nullptr;
}

bool XdgMimeAppsGLibBackend::setDefaultApp(const QString &mimeType, const XdgDesktopFile &app)
{
    GDesktopAppInfo *i = g_desktop_app_info_new_from_filename(app.fileName().toUtf8().constData());
    if (i == nullptr) {
        qCWarning(QtXdgMimeAppsGLib, "Failed to load GDesktopAppInfo for '%s'",
                qPrintable(app.fileName()));
        return false;
    }

    GError *error = nullptr;
    if (g_app_info_set_as_default_for_type(G_APP_INFO(i),
                                           mimeType.toUtf8().constData(), &error) == FALSE) {
        qCWarning(QtXdgMimeAppsGLib, "Failed to set '%s' as the default for '%s'. %s",
                  g_desktop_app_info_get_filename(i), qPrintable(mimeType), error->message);

        g_error_free(error);
        g_object_unref(i);
        return false;
    }

    qCDebug(QtXdgMimeAppsGLib, "Set '%s' as the default for '%s'",
            g_desktop_app_info_get_filename(i), qPrintable(mimeType));

    g_object_unref(i);
    return true;
}
