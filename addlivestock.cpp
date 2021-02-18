#include "addlivestock.h"
#include "ui_addlivestock.h"
#include <QSqlQuery>

addLivestock::addLivestock(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addLivestock)
{
    ui->setupUi(this);
    setWindowTitle(tr("Add new livestock"));
}

addLivestock::~addLivestock()
{
    delete ui;
}

void addLivestock::loadData(QSqlDatabase cleaned_db)
{
    QSqlQuery query(cleaned_db);
    if (query.exec("SELECT livetype_code,livetype_desc FROM lkp_livetype"))
    {
        while (query.next())
        {
            ui->cmb_livestock->addItem(query.value(1).toString(), QVariant(query.value(0).toString()));
        }
    }
    if (ui->cmb_livestock->count() > 0)
        ui->cmb_livestock->setCurrentIndex(0);
}

QString addLivestock::getLivestockCode()
{
    if (ui->cmb_livestock->currentIndex() >= 0)
        return ui->cmb_livestock->currentData().toString();
    return "";
}
