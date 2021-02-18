#include "addseason.h"
#include "ui_addseason.h"

addSeason::addSeason(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addSeason)
{
    ui->setupUi(this);
}

addSeason::~addSeason()
{
    delete ui;
}

QString addSeason::getSeasonName()
{
    return ui->txt_season->text();
}
