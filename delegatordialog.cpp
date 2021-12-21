#include "delegatordialog.h"
#include "ui_delegatordialog.h"
#include <QListWidgetItem>

delegatorDialog::delegatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::delegatorDialog)
{
    ui->setupUi(this);
}

delegatorDialog::~delegatorDialog()
{
    delete ui;
}

void delegatorDialog::on_buttonBox_accepted()
{
    if (ui->listWidget->selectedItems().count() > 0)
    {
        this->selected_code = ui->listWidget->selectedItems()[0]->data(Qt::UserRole).toString();
        this->close();
    }
}


void delegatorDialog::on_buttonBox_rejected()
{
    this->selected_code = "";
    this->close();
}

void delegatorDialog::load_manure(QSqlDatabase db)
{
    QSqlQuery query(db);
    if (query.exec("select manureman_desc from lkp_manureman"))
    {
        while (query.next())
        {
             QListWidgetItem *newItem = new QListWidgetItem;
             newItem->setText(query.value(0).toString());
             newItem->setData(Qt::UserRole,query.value(0).toString());
             ui->listWidget->addItem(newItem);
        }
    }
}

void delegatorDialog::set_caption(QString caption)
{
    ui->label->setText(caption);
}

void delegatorDialog::load_sourceType()
{
    QStringList sourceTypes;
    sourceTypes << "Main";
    sourceTypes << "Residue";
    sourceTypes << "Purchased";
    for (int pos = 0; pos < sourceTypes.count(); pos++)
    {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(sourceTypes[pos]);
        newItem->setData(Qt::UserRole,sourceTypes[pos]);
        ui->listWidget->addItem(newItem);
    }
}

void delegatorDialog::load_landcover(QSqlDatabase db)
{
    QSqlQuery query(db);
    if (query.exec("SELECT landcover_code,landcover_desc FROM lkp_landcover"))
    {
        while (query.next())
        {
            QListWidgetItem *newItem = new QListWidgetItem;
            newItem->setText(query.value(1).toString());
            newItem->setData(Qt::UserRole,query.value(0).toString());
            ui->listWidget->addItem(newItem);
        }
    }
}

void delegatorDialog::load_slope(QSqlDatabase db)
{
    QSqlQuery query(db);
    if (query.exec("SELECT slope_code,slope_desc FROM lkp_slope"))
    {
        while (query.next())
        {
            QListWidgetItem *newItem = new QListWidgetItem;
            newItem->setText(query.value(1).toString());
            newItem->setData(Qt::UserRole,query.value(0).toString());
            ui->listWidget->addItem(newItem);
        }
    }
}

void delegatorDialog::load_grassmanagement(QSqlDatabase db)
{
    QSqlQuery query(db);
    if (query.exec("SELECT management_code,management_desc FROM lkp_grasslandman"))
    {
        while (query.next())
        {
            QListWidgetItem *newItem = new QListWidgetItem;
            newItem->setText(query.value(1).toString());
            newItem->setData(Qt::UserRole,query.value(0).toString());
            ui->listWidget->addItem(newItem);
        }
    }
}
