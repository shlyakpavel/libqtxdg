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

#include "xdgmimeapps.h"
#include "xdgmimeapps_p.h"

#include "xdgdesktopfile.h"
#include "xdgmacros.h"
#include "xdgmimeappsglibbackend.h"

#include <QMutexLocker>

Q_GLOBAL_STATIC(XdgMimeAppsPrivate, staticXdgMimeApps)

XdgMimeAppsPrivate *XdgMimeAppsPrivate::instance()
{
    return staticXdgMimeApps();
}

XdgMimeAppsPrivate::XdgMimeAppsPrivate()
    : mBackend(nullptr)
{
    mBackend = new XdgMimeAppsGLibBackend(this);
}

XdgMimeAppsPrivate::~XdgMimeAppsPrivate()
{
    delete mBackend;
    mBackend = nullptr;
}

XdgMimeApps::XdgMimeApps()
      : d(staticXdgMimeApps())
{
}

XdgMimeApps::~XdgMimeApps()
{
    d = nullptr;
}

XdgDesktopFile *XdgMimeApps::defaultApp(const QString &mimeType)
{
    if (mimeType.isEmpty())
        return nullptr;

    QMutexLocker locker(&d->mutex);
    return d->mBackend->defaultApp(mimeType);
}

bool XdgMimeApps::setDefaultApp(const QString &mimeType, const XdgDesktopFile &app)
{
    if (mimeType.isEmpty() || !app.isValid())
        return false;

    if (XdgDesktopFile::id(app.fileName()).isEmpty())
        return false;

    QMutexLocker locker(&d->mutex);
    return d->mBackend->setDefaultApp(mimeType, app);
}
