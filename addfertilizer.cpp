#include "addfertilizer.h"
#include "ui_addfertilizer.h"
#include <QSqlQuery>

addFertilizer::addFertilizer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addFertilizer)
{
    ui->setupUi(this);
}

addFertilizer::~addFertilizer()
{
    delete ui;
}

void addFertilizer::loadData(QSqlDatabase cleaned_db)
{
    QSqlQuery query(cleaned_db);
    if (query.exec("SELECT fertilizer_code,fertilizer_desc FROM lkp_orgfertilizer"))
    {
        while (query.next())
        {
            ui->cmb_fertilizer->addItem(query.value(1).toString(), QVariant(query.value(0).toString()));
        }
    }
    if (ui->cmb_fertilizer->count() > 0)
        ui->cmb_fertilizer->setCurrentIndex(0);
}

QString addFertilizer::getFertilizerCode()
{
    if (ui->cmb_fertilizer->currentIndex() >= 0)
        return ui->cmb_fertilizer->currentData().toString();
    return "";
}
