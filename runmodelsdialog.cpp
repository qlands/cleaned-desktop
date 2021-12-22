#include "runmodelsdialog.h"
#include "ui_runmodelsdialog.h"
#include <QListWidgetItem>

runModelsDialog::runModelsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::runModelsDialog)
{
    ui->setupUi(this);
}

runModelsDialog::~runModelsDialog()
{
    delete ui;
}


void runModelsDialog::addModel(QString modelFile)
{
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(modelFile);
    newItem->setData(Qt::UserRole,modelFile);
    ui->lstmodels->addItem(newItem);
}

void runModelsDialog::on_buttonBox_accepted()
{
    if (ui->lstmodels->selectedItems().count() > 0)
    {
        for (int i=0; i < ui->lstmodels->selectedItems().count(); i++)
        {
            selectedFiles.append( ui->lstmodels->selectedItems()[i]->text());
        }
        this->close();
    }
}


void runModelsDialog::on_buttonBox_rejected()
{
    this->close();
}

