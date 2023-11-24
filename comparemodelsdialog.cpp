#include "comparemodelsdialog.h"
#include "ui_comparemodelsdialog.h"
#include "comparemodelsresultdialog.h"
#include <QMessageBox>

compareModelsDialog::compareModelsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::compareModelsDialog)
{
    ui->setupUi(this);
    ui->lstmodels->setSelectionMode(QAbstractItemView::MultiSelection);
    connect(ui->lstmodels, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(handleItemChanged(QListWidgetItem*)));
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
            selectedFiles.append( dynamic_cast<RadioListItem*>(ui->lstmodels->selectedItems()[i])->getResult());
        }
        compareModelsResultDialog resultDialog(this, selectedFiles);
        resultDialog.exec();
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
    RadioListItem *newItem = new RadioListItem(modelFile, ui->lstmodels);
    newItem->setData(Qt::UserRole,modelFile);
    ui->lstmodels->addItem(newItem);
}


void compareModelsDialog::on_buttonBox_rejected()
{
    this->close();
}

void compareModelsDialog::handleItemChanged(QListWidgetItem *item)
{
    auto custom = dynamic_cast<RadioListItem*>(item);

    if (custom->getRadioButton() && custom->getRadioButton()->isChecked())
    {
        for (int i = 0; i < ui->lstmodels->count(); ++i)
        {
            auto otherItem = ui->lstmodels->item(i);
            if (otherItem != item)
            {
                dynamic_cast<RadioListItem*>(otherItem)->getRadioButton()->setChecked(false);
            }
        }
    }
}
