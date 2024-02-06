/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QSettings>
#include "mainwindow.h"
#include "technicalmanualcleand.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

bool extractResourceFile(const QString &resourcePath, const QString &destinationPath) {
    QFile file(destinationPath);
    if (file.exists()) {
        // The file already exists, no need to extract it again.
        return true;
    }

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << file.errorString();
        return false;
    }

    // Open the resource file
    QFile resourceFile(resourcePath);
    if (!resourceFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open resource file:" << resourceFile.errorString();
        return false;
    }

    // Copy the content from the resource file to the destination file
    QByteArray content = resourceFile.readAll();
    file.write(content);

    // Close the files
    file.close();
    resourceFile.close();

    return true;
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(cleaned);

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("CLEANED_20230809");
    QCoreApplication::setOrganizationName("Alliance Bioversity-CIAT");
    QCoreApplication::setApplicationVersion("20230809");
    QCommandLineParser parser;
    parser.setApplicationDescription("Cleaned Desktop App");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The scenario file to open.");
    parser.process(app);

    QString runDir = QDir::QDir::currentPath();
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    if (settings.value("database_file","").toString() == "")
    {
        settings.setValue("database_file", runDir + QDir::separator() + "databases.json");
    }
    if (settings.value("model_file","").toString() == "")
    {
        settings.setValue("model_file", runDir + QDir::separator() + "R" + QDir::separator() + "run_model.R");
    }
    if (settings.value("ghg_file","").toString() == "")
    {
        settings.setValue("ghg_file", runDir + QDir::separator() + "R" + QDir::separator() + "ghg_parameters.json");
    }
    if (settings.value("stock_file","").toString() == "")
    {
        settings.setValue("stock_file", runDir + QDir::separator() + "R" + QDir::separator() + "stock_change_parameters.json");
    }
    if (settings.value("enery_file","").toString() == "")
    {
        settings.setValue("enery_file", runDir + QDir::separator() + "R" + QDir::separator() + "energy_parameters.json");
    }

    if (!QFile::exists(runDir + QDir::separator() + "databases.json"))
    {
        QJsonObject databases;

        QJsonObject base_database;
        base_database["name"] = "Base database";
        base_database["path"] = runDir + QDir::separator() + "cleaned.sqlite";
        base_database["removeable"] = false;

        QJsonObject empty_database;
        empty_database["name"] = "Empty database";
        empty_database["path"] = runDir + QDir::separator() + "empty.sqlite";
        empty_database["removeable"] = false;

        databases["base"] = base_database;
        databases["empty"] = empty_database;

        QFile saveFile(runDir + QDir::separator() + "databases.json");
        if (!saveFile.open(QIODevice::WriteOnly)) {
            qWarning("Couldn't open save file.");
            exit(1);
        }
        saveFile.write(QJsonDocument(databases).toJson());
        saveFile.close();
    }

    MainWindow mainWin;
    const QStringList posArgs = parser.positionalArguments();
    for (const QString &fileName : posArgs)
        mainWin.openFile(fileName);
    QMessageBox msgBox;
    msgBox.setText("Do you want to check the user manual ?");
    msgBox.setStandardButtons(QMessageBox::Yes |  QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Save);
    int r = msgBox.exec();
    if (r==QMessageBox::Yes){
        TechnicalManual technicalmanual_screen;
        technicalmanual_screen.exec();
        mainWin.show();
        return app.exec();
        }
    else
    {
        extractResourceFile(":/docs/ReadMe.pdf", QCoreApplication::applicationDirPath() + "/ReadMe.pdf");
        mainWin.show();
        return app.exec();
    }
}
