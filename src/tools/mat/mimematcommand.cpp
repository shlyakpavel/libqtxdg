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

#include "mimematcommand.h"

#include "xdgdesktopfile.h"
#include "xdgmacros.h"
#include "xdgmimeapps.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

#include <iostream>

enum CommandLineParseResult {
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

enum MimeCommandMode {
    CommandModeGetDefApp,
    CommandModeSetDefApp
};

struct MimeData {
    MimeData() : mode(CommandModeGetDefApp) {}

    MimeCommandMode mode;
    QString defAppName;
    QStringList mimeTypes;
};

static CommandLineParseResult parseCommandLine(QCommandLineParser *parser, MimeData *data, QString *errorMessage)
{
    parser->clearPositionalArguments();
    parser->setApplicationDescription(QL1S("Get/Set the default application for a mimetype"));

    parser->addPositionalArgument(QL1S("mime"),QSL("mimetype(s)"),
                                  QCoreApplication::tr("[mimetype(s)...]"));

    const QCommandLineOption defAppNameOption(QStringList() << QSL("d") << QSL("default-app"),
                QSL("Application to be set as default"), QSL("app name"));

    parser->addOption(defAppNameOption);
    parser->addHelpOption();
    parser->addVersionOption();

    parser->process(QCoreApplication::arguments());

    const bool isDefAppNameSet = parser->isSet(defAppNameOption);
    QString defAppName;

    if (isDefAppNameSet)
        defAppName = parser->value(defAppNameOption);

    if (isDefAppNameSet && defAppName.isEmpty()) {
        *errorMessage = QSL("no application name");
        return CommandLineError;
    }

    QStringList mimeTypes = parser->positionalArguments();

    if (mimeTypes.size() < 2) {
        *errorMessage = QSL("mimeType missing");
        return CommandLineError;
    }

    mimeTypes.removeAt(0);

    if (!isDefAppNameSet && mimeTypes.size() > 1) {
        *errorMessage = QSL("only one mimeType, please");
        return CommandLineError;
    }
   
    data->mode = isDefAppNameSet ? CommandModeSetDefApp : CommandModeGetDefApp;
    data->defAppName = defAppName;
    data->mimeTypes = mimeTypes;

    return CommandLineOk;
}

MimeMatCommand::MimeMatCommand()
    : MatCommandInterface(QL1S("mime"),
                          QSL("Get/Set the default application for a mimetpe"))
{
}

MimeMatCommand::~MimeMatCommand()
{
}

int MimeMatCommand::run(QCommandLineParser *parser, const QStringList &arguments)
{
    MimeData data;
    QString errorMessage;
    switch(parseCommandLine(parser, &data, &errorMessage)) {
    case CommandLineOk:
        break;
    case CommandLineError:
        std::cerr << qPrintable(errorMessage);
        std::cerr << "\n\n";
        std::cerr << qPrintable(parser->helpText());
        return 1;
    case CommandLineVersionRequested:
        parser->showVersion();
        return 0;
    case CommandLineHelpRequested:
        parser->showHelp();
        Q_UNREACHABLE();
    }

    if (data.mode == CommandModeGetDefApp) {
        XdgMimeApps apps;
        const QString mimeType = data.mimeTypes.constFirst();
        XdgDesktopFile *defApp = apps.defaultApp(mimeType);
        if (defApp != nullptr) {
            std::cout << qPrintable(XdgDesktopFile::id(defApp->fileName())) << "\n";
            delete defApp;
        } else {
            std::cout << qPrintable(QSL("No default application for '%1'\n").arg(mimeType));
        }
    } else { // Set default App
        XdgDesktopFile app;
        if (!app.load(data.defAppName)) {
            std::cout << qPrintable(QSL("Could not find find '%1'\n").arg(data.defAppName));
            return 0;
        }
        
        XdgMimeApps apps;
        for (const QString &mimeType : data.mimeTypes) {
            if (!apps.setDefaultApp(mimeType, app)) {
                std::cout << qPrintable(QSL("Could not set '%1' as default for '%2'\n").arg(app.fileName(), mimeType));
            } else {
                std::cout << qPrintable(QSL("Set '%1' as default for '%2'\n").arg(app.fileName(), mimeType));
            }
        }
    }
    return 0;
}

void MimeMatCommand::showDescription()
{
}

void MimeMatCommand::showHelp(int exitCode)
{
}
