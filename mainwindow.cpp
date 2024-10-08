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

#include <QtWidgets>

#include "mainwindow.h"
#include "cleanedstudy.h"
#include "settings.h"
#include "aboutcleaned.h"
#include "runmodelsdialog.h"
#include "versiondialog.h"
#include "managedatabases.h"
#include "selectdatabase.h"
#include "comparemodelsdialog.h"
#include "technicalmanualcleand.h"

MainWindow::MainWindow()
    : mdiArea(new QMdiArea)
{
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, &QMdiArea::subWindowActivated,
            this, &MainWindow::updateMenus);

    createActions();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("CLEANED"));
    setUnifiedTitleAndToolBarOnMac(true);
    this->database_file = "./cleaned.sqlite";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    selectDatabase selDB;
    selDB.exec();
    if (selDB.selected_code != "")
    {
        CleanedStudy *child = createMdiChild();
        child->setDatabaseCode(selDB.selected_code);
        if (child->newFile())
        {
            mdiArea->addSubWindow(child);
            child->show();
        }
        else
            child->close();
    }
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this,tr("Select a study file"), ".", "JSON (*.json)");
    if (!fileName.isEmpty())
        openFile(fileName);
}

bool MainWindow::openFile(const QString &fileName)
{
    if (QMdiSubWindow *existing = findMdiChild(fileName)) {
        mdiArea->setActiveSubWindow(existing);
        return true;
    }
    const bool succeeded = loadFile(fileName);
    if (succeeded)
        statusBar()->showMessage(tr("File loaded"), 2000);
    return succeeded;
}

bool MainWindow::loadFile(const QString &fileName)
{
    CleanedStudy *child = createMdiChild();
    const bool succeeded = child->loadFile(fileName);
    if (succeeded)
    {
        mdiArea->addSubWindow(child);
        child->show();
        MainWindow::prependToRecentFiles(fileName);
    }
    else
        child->close();

    return succeeded;
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

void MainWindow::report_error(QString error_message)
{
    statusBar()->showMessage(error_message);
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

bool MainWindow::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

void MainWindow::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void MainWindow::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        openFile(action->data().toString());
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    CleanedStudy *child = activeMdiChild();
    if (child && child->saveAs()) {
        statusBar()->showMessage(tr("File saved"), 2000);
        MainWindow::prependToRecentFiles(child->currentFile());
    }
}


void MainWindow::show_about()
{
    AboutCleaned about_screen(this);
    about_screen.exec();
}

void MainWindow::show_version()
{
    VersionDialog version_screen;
    version_screen.exec();
}
void MainWindow::show_technicalmanual()
{
    TechnicalManual technicalmanual_screen;
    technicalmanual_screen.exec();
}


void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != nullptr);
    saveAct->setEnabled(hasMdiChild);
    runAct->setEnabled(hasMdiChild);
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    if (windows.size() > 1)
        compareAct->setEnabled(true);
    else
        compareAct->setEnabled(false);
    databasesAct->setEnabled(!hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    windowMenuSeparatorAct->setVisible(hasMdiChild);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(windowMenuSeparatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    windowMenuSeparatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);
        CleanedStudy *child = qobject_cast<CleanedStudy *>(mdiSubWindow->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action = windowMenu->addAction(text, mdiSubWindow, [this, mdiSubWindow]() {
            mdiArea->setActiveSubWindow(mdiSubWindow);
        });
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
    }
}

CleanedStudy *MainWindow::createMdiChild()
{
    CleanedStudy *child = new CleanedStudy;        
    return child;
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    fileMenu->addSeparator();

    QMenu *recentMenu = fileMenu->addMenu(tr("Recent..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = fileMenu->addSeparator();

    setRecentFilesVisible(MainWindow::hasRecentFiles());

    fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);

    fileMenu->addSeparator();

    const QIcon dbManIcon = QIcon(":/images/db.png");
    databasesAct = new QAction(dbManIcon, tr("&parameters "), this);
    QKeySequence db_sequence(Qt::CTRL + Qt::Key_P);
    QList< QKeySequence>db_shortcuts;
    db_shortcuts.append(db_sequence);
    databasesAct->setShortcuts(db_shortcuts);
    databasesAct->setStatusTip(tr("Manage parameters"));
    connect(databasesAct, &QAction::triggered, this, &MainWindow::manageDatabases);
    fileMenu->addAction(databasesAct);

    fileMenu->addSeparator();

//! [0]
    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::closeAllWindows);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAct);
//! [0]


    QMenu *modelMenu = menuBar()->addMenu(tr("&Model"));

    const QIcon runIcon = QIcon(":/images/run.png");
    runAct = new QAction(runIcon, tr("&Run R the model"), this);
    QKeySequence run_sequence(Qt::CTRL + Qt::Key_R);
    QList< QKeySequence>run_shortcuts;
    run_shortcuts.append(run_sequence);
    runAct->setShortcuts(run_shortcuts);
    runAct->setStatusTip(tr("Run the Cleaned R model"));
    connect(runAct, &QAction::triggered, this, &MainWindow::runModel);
    modelMenu->addAction(runAct);

    const QIcon compareIcon = QIcon(":/images/compare.png");
    compareAct = new QAction(compareIcon, tr("Co&mpare models"), this);
    QKeySequence compare_sequence(Qt::CTRL + Qt::Key_R);
    QList< QKeySequence>compare_shortcuts;
    compare_shortcuts.append(compare_sequence);
    compareAct->setShortcuts(compare_shortcuts);
    compareAct->setStatusTip(tr("Run the comparison model"));
    connect(compareAct, &QAction::triggered, this, &MainWindow::compareModels);
    modelMenu->addAction(compareAct);


    const QIcon settingsIcon = QIcon(":/images/settings.png");
    settingsAct = new QAction(settingsIcon, tr("&Settings"), this);
    QKeySequence settings_sequence(Qt::CTRL + Qt::Key_T);
    QList< QKeySequence>settings_shortcuts;
    run_shortcuts.append(settings_sequence);
    settingsAct->setShortcuts(settings_shortcuts);
    settingsAct->setStatusTip(tr("Cleaned model settings"));
    connect(settingsAct, &QAction::triggered, this, &MainWindow::loadSettings);
    modelMenu->addAction(settingsAct);




    fileToolBar->addSeparator();
    fileToolBar->addAction(runAct);
    fileToolBar->addAction(compareAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(databasesAct);


    windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, &QAction::triggered,
            mdiArea, &QMdiArea::closeActiveSubWindow);

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, &QAction::triggered, mdiArea, &QMdiArea::closeAllSubWindows);

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, &QAction::triggered, mdiArea, &QMdiArea::tileSubWindows);

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, &QAction::triggered, mdiArea, &QMdiArea::cascadeSubWindows);

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow);

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow);

    windowMenuSeparatorAct = new QAction(this);
    windowMenuSeparatorAct->setSeparator(true);

    updateWindowMenu();

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::show_about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

    QAction *versionAct = helpMenu->addAction(tr("&Version"), this, &MainWindow::show_version);
    versionAct->setStatusTip(tr("Show the application's Version box"));

    QAction *technicalAct = helpMenu->addAction(tr("&Manual"), this, &MainWindow::show_technicalmanual);
    versionAct->setStatusTip(tr("Show the application's Technical box"));

//    QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
//    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

CleanedStudy *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<CleanedStudy *>(activeSubWindow->widget());
    return nullptr;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    const QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();
    for (QMdiSubWindow *window : subWindows) {
        CleanedStudy *mdiChild = qobject_cast<CleanedStudy *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return nullptr;
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        QGuiApplication::setLayoutDirection(Qt::RightToLeft);
    else
        QGuiApplication::setLayoutDirection(Qt::LeftToRight);
}


void MainWindow::runModel()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QString r_file = settings.value("rscript_file","").toString();
    if (r_file.simplified() != "" && QFile::exists(r_file))
    {
        QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
        if (windows.size() > 1)
        {
            runModelsDialog modelsDialog;
            for (int i = 0; i < windows.size(); ++i) {
                QMdiSubWindow *mdiSubWindow = windows.at(i);
                CleanedStudy *child = qobject_cast<CleanedStudy *>(mdiSubWindow->widget());
                if (child->studyModified == false)
                {
                    modelsDialog.addModel(child->currentFile());

                }
            }
            modelsDialog.exec();
            if (modelsDialog.selectedFiles.count() > 0)
            {
                for (int i=0; i < modelsDialog.selectedFiles.count(); i++)
                {
                    QString modelFile = modelsDialog.selectedFiles[i];
                    for (int i = 0; i < windows.size(); ++i) {
                        QMdiSubWindow *mdiSubWindow = windows.at(i);
                        CleanedStudy *child = qobject_cast<CleanedStudy *>(mdiSubWindow->widget());
                        if (child->currentFile() == modelFile)
                        {
                            child->run();
                        }
                    }
                }
            }
        }
        else
            if (activeMdiChild() && activeMdiChild()->run())
                statusBar()->showMessage(tr("Model ran"), 2000);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You need to setup R in the settings before running a model");
        msgBox.exec();
    }
}

void MainWindow::loadSettings()
{
    ModelSettings settingsWindow;    
    settingsWindow.exec();
}

void MainWindow::manageDatabases()
{
    ManageDatabases databases;
    databases.exec();
}

void MainWindow::compareModels()
{
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    if (windows.size() > 1)
    {
        compareModelsDialog compareDialog;
        int num_models = 0;
        for (int i = 0; i < windows.size(); ++i) {
            QMdiSubWindow *mdiSubWindow = windows.at(i);
            CleanedStudy *child = qobject_cast<CleanedStudy *>(mdiSubWindow->widget());
            if (child->studyModified == false)
            {
                QFileInfo fInfo(child->currentFile());
                QString result_file = fInfo.absolutePath()+QDir::separator()+fInfo.baseName() + QDir::separator()+fInfo.baseName()+"_result.json";
                if (QFile::exists(result_file))
                {
                    compareDialog.addModel(result_file);
                    num_models++;
                }
            }
        }
        if (num_models > 1)
        {
            compareDialog.exec();
            if (compareDialog.selectedFiles.count() > 0)
            {
                for (int i=0; i < compareDialog.selectedFiles.count(); i++)
                {
                    QString resultFile = compareDialog.selectedFiles[i];

                }
            }
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Check if your models are saved and have results");
            msgBox.exec();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You need to have two or more models opened");
        msgBox.exec();
    }
}
