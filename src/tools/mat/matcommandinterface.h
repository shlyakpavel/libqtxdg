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

#ifndef MATCOMMANDINTERFACE_H
#define MATCOMMANDINTERFACE_H

#include <QStringList>

class QCommandLineParser;

class MatCommandInterface {

public:
    MatCommandInterface(const QString &name, const QString &description);
    virtual ~MatCommandInterface();

    QString name() const;
    QString description() const;
    virtual int run(QCommandLineParser *parser, const QStringList &arguments) = 0;
    virtual void showHelp(int exitCode = 0) = 0;
    virtual void showDescription() = 0;
    QString descriptionHelpText() const;

private:
    QString mName;
    QString mDescription;
};

#endif // MATCOMMANDINTERFACE_H
