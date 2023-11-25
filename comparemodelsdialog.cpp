#include "comparemodelsdialog.h"
#include "ui_comparemodelsdialog.h"
#include "comparemodelsresultdialog.h"
#include <QMessageBox>
#include <QFileInfo>

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
    QString baseName = "";

    if (ui->lstmodels->selectedItems().count() > 1)
    {
        for (int i=0; i < ui->lstmodels->selectedItems().count(); i++)
        {
            auto item = dynamic_cast<CustomListWidgetItem*>(ui->lstmodels->selectedItems()[i]);
            selectedFiles.append(item->getResult());
            if (item->getRadioButton()->isChecked()) {
                QFileInfo fi(item->getResult());
                baseName = fi.baseName().remove(".json");
            }
        }
        compareModelsResultDialog resultDialog(this, selectedFiles, baseName);
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
    CustomListWidgetItem *newItem = new CustomListWidgetItem(modelFile, ui->lstmodels);
    newItem->setData(Qt::UserRole,modelFile);
    ui->lstmodels->addItem(newItem);
}


void compareModelsDialog::on_buttonBox_rejected()
{
    this->close();
}

void compareModelsDialog::handleItemChanged(QListWidgetItem *item)
{
    auto custom = dynamic_cast<CustomListWidgetItem*>(item);

    if (custom->getRadioButton() && custom->getRadioButton()->isChecked())
    {
        for (int i = 0; i < ui->lstmodels->count(); ++i)
        {
            auto otherItem = ui->lstmodels->item(i);
            if (otherItem != item)
            {
                dynamic_cast<CustomListWidgetItem*>(otherItem)->getRadioButton()->setChecked(false);
            }
        }
    }
}
