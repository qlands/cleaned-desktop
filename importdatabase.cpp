#include "importdatabase.h"
#include "ui_importdatabase.h"
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>

importDatabase::importDatabase(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::importDatabase)
{
    ui->setupUi(this);
    this->setWindowTitle("Import database");
}

importDatabase::~importDatabase()
{
    delete ui;
}

void importDatabase::on_cmdBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Database"), QDir::homePath(), tr("SQLite Files (*.sqlite)"));
    if (fileName != "")
        ui->txtpath->setText(fileName);
}


void importDatabase::on_buttonBox_accepted()
{
    if (QFile::exists(ui->txtpath->text()))
    {
        if (ui->txtcode->text() != "")
        {
            if (ui->txtname->text() != "")
            {
                this->db_name = ui->txtname->text();
                this->db_code = ui->txtcode->text();
                this->db_path = ui->txtpath->text();
                this->close();
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText("You need to provide a name");
                msgBox.exec();
            }
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("You need to provide a code");
            msgBox.exec();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("The database file does not exist");
        msgBox.exec();
    }
}


void importDatabase::on_buttonBox_rejected()
{
    this->db_code = "";
    this->close();
}

