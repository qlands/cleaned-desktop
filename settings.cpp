#include "settings.h"
#include "ui_settings.h"
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>

ModelSettings::ModelSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
    this->setWindowTitle("Model settings");
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    if (settings.value("model_file","").toString() != "")
    {
        ui->txt_model_file->setText(settings.value("model_file","").toString());
    }
    if (settings.value("ghg_file","").toString() != "")
    {
        ui->txt_ghg->setText(settings.value("ghg_file","").toString());
    }
    if (settings.value("stock_file","").toString() != "")
    {
        ui->txt_stock->setText(settings.value("stock_file","").toString());
    }
    if (settings.value("enery_file","").toString() != "")
    {
        ui->txt_energy->setText(settings.value("enery_file","").toString());
    }
    if (settings.value("rscript_file","").toString() != "")
    {
        ui->txtr->setText(settings.value("rscript_file","").toString());
    }
    if (settings.value("dbman_file","").toString() != "")
    {
        ui->txtdbman->setText(settings.value("dbman_file","").toString());
    }
}

ModelSettings::~ModelSettings()
{
    delete ui;
}

QString ModelSettings::getModelFile()
{
    return ui->txt_model_file->text();
}
void ModelSettings::setModelFile(QString modelFile)
{
    return ui->txt_model_file->setText(modelFile);
}


void ModelSettings::on_cmb_browse_model_clicked()
{
    const QString fileName = QFileDialog::getOpenFileName(this,tr("Select a model file"), ".", "R (*.r)");
    if (!fileName.isEmpty())
        ui->txt_model_file->setText(fileName);
}

void ModelSettings::on_buttonBox_accepted()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QFileInfo file;
    file.setFile(ui->txt_ghg->text());
    if (file.exists())
        settings.setValue("ghg_file", ui->txt_ghg->text());

    file.setFile(ui->txt_stock->text());
    if (file.exists())
        settings.setValue("stock_file", ui->txt_stock->text());

    file.setFile(ui->txt_energy->text());
    if (file.exists())
        settings.setValue("enery_file", ui->txt_energy->text());

    file.setFile(ui->txt_model_file->text());
    if (file.exists())
        settings.setValue("model_file", ui->txt_model_file->text());

    file.setFile(ui->txtr->text());
    if (file.exists())
        settings.setValue("rscript_file", ui->txtr->text());

    file.setFile(ui->txtdbman->text());
    if (file.exists())
        settings.setValue("dbman_file", ui->txtdbman->text());

    this->close();
}

void ModelSettings::on_buttonBox_rejected()
{
    this->close();
}

void ModelSettings::on_cmd_ghg_clicked()
{
    const QString fileName = QFileDialog::getOpenFileName(this,tr("Select a GHG settings file"), ".", "JSON (*.json)");
    if (!fileName.isEmpty())
        ui->txt_ghg->setText(fileName);
}

void ModelSettings::on_cmd_stock_clicked()
{
    const QString fileName = QFileDialog::getOpenFileName(this,tr("Select a stock change settings file"), ".", "JSON (*.json)");
    if (!fileName.isEmpty())
        ui->txt_stock->setText(fileName);
}

void ModelSettings::on_cmd_energy_clicked()
{
    const QString fileName = QFileDialog::getOpenFileName(this,tr("Select a energy settings file"), ".", "JSON (*.json)");
    if (!fileName.isEmpty())
        ui->txt_energy->setText(fileName);
}

void ModelSettings::on_cmdbrowser_clicked()
{
#if defined(__linux__)
    QString fileName = QFileDialog::getOpenFileName(this,tr("RScript"), QDir::homePath(), tr("Executable file (Rscript)"));
#else
    QString fileName = QFileDialog::getOpenFileName(this,tr("RScript"), QDir::homePath(), tr("Executable file (Rscript.exe)"));
#endif
    if (fileName != "")
        ui->txtr->setText(fileName);
}


void ModelSettings::on_cmdbrowsedbman_clicked()
{
#if defined(__linux__)
    QString fileName = QFileDialog::getOpenFileName(this,tr("DB Browser for SQLite"), QDir::homePath(), tr("Executable file (sqlitebrowser)"));
#else
    QString fileName = QFileDialog::getOpenFileName(this,tr("DB Browser for SQLite"), QDir::homePath(), tr("Executable file (*.exe)"));
#endif
    if (fileName != "")
        ui->txtdbman->setText(fileName);
}

