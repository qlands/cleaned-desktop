#include "aboutcleaned.h"
#include "ui_aboutcleaned.h"
#include <QWebEngineView>
#include <QStyle>
#include <QScreen>
#include <QMainWindow>

AboutCleaned::AboutCleaned(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutCleaned)
{
    ui->setupUi(this);

    ui->textLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->textLabel->setOpenExternalLinks(true);

    auto geometry = qobject_cast<QMainWindow*>(parent)->geometry();

    int top = (geometry.width() - width()) / 2;
    int left = (geometry.height() - height()) / 2;
    move(top, left);
}

AboutCleaned::~AboutCleaned()
{
    delete ui;
}

void AboutCleaned::on_cmdClose_clicked()
{
    this->close();
}

