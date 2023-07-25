#include "technicalmanualcleand.h"
#include "ui_technicalmanualcleand.h"
#include <QWebEngineView>



TechnicalManual::TechnicalManual(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TechnicalManual)
{
    ui->setupUi(this);
    QWebEngineView *view = new QWebEngineView(parent);
    view->load(QUrl("https://drive.google.com/file/d/1jBoQ5q70MmdAYQcqhkcT79zbX-I4rPZ1/view?usp=drive_link/"));
    view->show();
    ui->centralLayout->addWidget(view);
    this->setWindowTitle("Manual");
    this->setWindowState(Qt::WindowMaximized);

}

TechnicalManual::~TechnicalManual()
{
    delete ui;
}

void TechnicalManual::on_cmdClose_clicked()
{
       this->close();
}

