#include "versiondialog.h"
#include "ui_versiondialog.h"

VersionDialog::VersionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionDialog)
{
    ui->setupUi(this);
}

VersionDialog::~VersionDialog()
{
    delete ui;
}

void VersionDialog::on_cmdclose_clicked()
{
    this->close();
}

