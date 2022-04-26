#include "comparemodelsdialog.h"
#include "ui_comparemodelsdialog.h"
#include <QMessageBox>

compareModelsDialog::compareModelsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::compareModelsDialog)
{
    ui->setupUi(this);
}

compareModelsDialog::~compareModelsDialog()
{
    delete ui;
}

void compareModelsDialog::on_buttonBox_accepted()
{
    if (ui->lstmodels->selectedItems().count() > 1)
    {
        for (int i=0; i < ui->lstmodels->selectedItems().count(); i++)
        {
            selectedFiles.append( ui->lstmodels->selectedItems()[i]->text());
        }
        this->close();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You need to select two or more models");
        msgBox.exec();
    }
}

void compareModelsDialog::addModel(QString modelFile)
{
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(modelFile);
    newItem->setData(Qt::UserRole,modelFile);
    ui->lstmodels->addItem(newItem);
}


void compareModelsDialog::on_buttonBox_rejected()
{
    this->close();
}

