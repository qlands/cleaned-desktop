#include "settings.h"
#include "ui_settings.h"
#include <QFileDialog>

ModelSettings::ModelSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
    this->setWindowTitle("Model settings");
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
