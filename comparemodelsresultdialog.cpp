#include "comparemodelsresultdialog.h"
#include "ui_comparemodelsresultdialog.h"
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QScrollBar>
#include <QPixmap>

compareModelsResultDialog::compareModelsResultDialog(QWidget *parent, QStringList& results) :
    QDialog(parent),
    ui(new Ui::comparemodelsresultdialog),
    _resultTab(nullptr),
    _results(results)
{
    ui->setupUi(this);
    _resultTab = ui->tab_2;
    ui->tabWidget->removeTab(1);

    connect(ui->pngList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onImageSelected(QListWidgetItem*,QListWidgetItem*)));

    _process = QSharedPointer<QProcess>::create(this);
    connect(_process.get(),SIGNAL(finished(int)),this,SLOT(modelFinished(int)));
    connect(_process.get(),SIGNAL(readyReadStandardError()),this,SLOT(readyReadStandardError()));
    connect(_process.get(),SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadStandardOutput()));


    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString folderName = QCoreApplication::applicationDirPath() + "/comparisons/" + currentDateTime.toString("yyyyMMdd-hhmmss") + "/";
    _outputFile = folderName + "comparison_output.json";
    _pngsDirectory = folderName;
    _comparisonModel = QCoreApplication::applicationDirPath() + "/R/compare_modesl.R";

    QDir dir;
    dir.mkpath(folderName);
    dir.mkpath(_pngsDirectory);

    compare();
}

void compareModelsResultDialog::readyReadStandardError() {
    QString current_data = ui->rScriptOutput->toPlainText();
    current_data = current_data + "\n" + _process->readAllStandardError();
    ui->rScriptOutput->setPlainText(current_data);
    ui->rScriptOutput->verticalScrollBar()->setValue(ui->rScriptOutput->verticalScrollBar()->maximum());
}

void compareModelsResultDialog::readyReadStandardOutput() {
    QString current_data = ui->rScriptOutput->toPlainText();
    current_data = current_data + "\n" + _process->readAllStandardOutput();
    ui->rScriptOutput->setPlainText(current_data);
    ui->rScriptOutput->verticalScrollBar()->setValue(ui->rScriptOutput->verticalScrollBar()->maximum());
}

void compareModelsResultDialog::modelFinished(int exitCode) {
    if (exitCode == 0)
    {
        QFile file(_outputFile);
        file.open(QFile::ReadOnly | QFile::Text);
        auto result = file.readAll();
        QString current_data = ui->rScriptOutput->toPlainText();
        current_data = current_data + "\n" + result;
        ui->rScriptOutput->setText(current_data);
        ui->rScriptOutput->setReadOnly(true);

        QDir directory(_pngsDirectory);
        QStringList filters;
        filters << "*.png";

        auto pngsFilenames = directory.entryList(filters);
        ui->pngList->addItems(pngsFilenames);

        for(auto& pngFilename : pngsFilenames) {
            //auto &
            _pngs.push_back(QSharedPointer<QPixmap>::create(_pngsDirectory+pngFilename));// QPixmap(_pngsDirectory+pngFilename));
        }
        ui->tabWidget->insertTab(1, _resultTab, "Comparison");
        ui->tabWidget->setCurrentWidget(_resultTab);
        if(ui->pngList->count()) {
            ui->pngList->setCurrentRow(0);
        }
    }
}

void compareModelsResultDialog::compare() {
    QStringList params;
    params.append("--vanilla");
    params.append(_comparisonModel);
    params.append(_outputFile);
    params.append(_pngsDirectory);
    for(auto& inputResult : _results) {
        params.append(inputResult);
    }
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QString r_binay_file = settings.value("rscript_file","").toString();
    if (r_binay_file.simplified() != "" && QFile::exists(r_binay_file))
    {
        QFileInfo fileInfo(settings.value("model_file","").toString());

        QStringList env = QProcess::systemEnvironment();
        QString library_path = fileInfo.absolutePath() + QDir::separator() + "library";
        QDir dir;
        if (!dir.exists(library_path))
            dir.mkdir(library_path);
        env << "R_LIBS_USER=" + library_path;
        _process->setEnvironment(env);
        _process->start(r_binay_file,params);
    }
}

void compareModelsResultDialog::onImageSelected(QListWidgetItem *current, QListWidgetItem *previous) {
    if(current) {
        auto &png= _pngs[ui->pngList->currentRow()];
        ui->pngLabel->setPixmap(QPixmap());
        ui->pngLabel->setPixmap(png->scaled(QSize(ui->pngLabel->size().width()-2, ui->pngLabel->size().height()-2), Qt::KeepAspectRatio));
    }
}

void compareModelsResultDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    if(ui->pngList->currentItem()) {
        auto &png= _pngs[ui->pngList->currentRow()];
        ui->pngLabel->setPixmap(QPixmap());
        ui->pngLabel->setPixmap(png->scaled(QSize(ui->pngLabel->size().width()-2, ui->pngLabel->size().height()-2), Qt::KeepAspectRatio));
    }
}

compareModelsResultDialog::~compareModelsResultDialog()
{
    ui->pngLabel->setPixmap(QPixmap());
    _pngs.clear();
    _process->kill();
    delete ui;
}
