#include "addfeed.h"
#include "ui_addfeed.h"
#include <QSqlQuery>
#include <QDebug>

addFeed::addFeed(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addFeed)
{
    ui->setupUi(this);
    setWindowTitle(tr("Add new feed"));
}

addFeed::~addFeed()
{
    delete ui;
}

void addFeed::loadData(QSqlDatabase cleaned_db)
{
    this->db = cleaned_db;
    QSqlQuery query(cleaned_db);
    if (query.exec("SELECT feed_type_code,feed_type_name FROM lkp_feedtype"))
    {
        while (query.next())
        {
            ui->cbm_crop->addItem(query.value(1).toString(), QVariant(query.value(0).toString()));
        }
    }
    if (ui->cbm_crop->count() > 0)
    {
        ui->cbm_crop->setCurrentIndex(0);
        if (query.exec("SELECT feed_item_code,feed_item_name FROM lkp_feeditem WHERE feed_type_code = '" + ui->cbm_crop->currentData().toString() + "'"))
        {
            while (query.next())
            {
                ui->cbm_feed->addItem(query.value(1).toString(), QVariant(query.value(0).toString()));
            }
        }
        if (ui->cbm_feed->count() > 0)
            ui->cbm_feed->setCurrentIndex(0);
    }
}

QString addFeed::getCropCode()
{
    if (ui->cbm_crop->currentIndex() >= 0)
        return ui->cbm_crop->currentData().toString();
    return "";
}

QString addFeed::getFeedCode()
{
    if (ui->cbm_feed->currentIndex() >= 0)
        return ui->cbm_feed->currentData().toString();
    return "";
}

void addFeed::on_cbm_crop_currentIndexChanged(int index)
{
    QString crop = ui->cbm_crop->itemData(index).toString();
    ui->cbm_feed->clear();
    QSqlQuery query(db);
    if (query.exec("SELECT feed_item_code,feed_item_name FROM lkp_feeditem WHERE feed_type_code = '" + crop + "'"))
    {
        while (query.next())
        {
            ui->cbm_feed->addItem(query.value(1).toString(), QVariant(query.value(0).toString()));
        }
    }
    if (ui->cbm_feed->count() > 0)
        ui->cbm_feed->setCurrentIndex(0);
}
