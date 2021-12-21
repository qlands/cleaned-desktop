#include "aboutcleaned.h"
#include "ui_aboutcleaned.h"
#include <QWebEngineView>

AboutCleaned::AboutCleaned(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutCleaned)
{
    ui->setupUi(this);
    QWebEngineView *view = new QWebEngineView(parent);
    view->load(QUrl("https://ciat.cgiar.org/ciat-projects/environmental-assessments-of-livestock-systems-using/"));
    view->show();
    ui->centralLayout->addWidget(view);
    this->setWindowTitle("About cleaned");
    this->setWindowState(Qt::WindowMaximized);
}

AboutCleaned::~AboutCleaned()
{
    delete ui;
}
