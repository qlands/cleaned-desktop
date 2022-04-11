#include "clonedatabase.h"
#include "ui_clonedatabase.h"
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>

cloneDatabase::cloneDatabase(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cloneDatabase)
{
    ui->setupUi(this);
    this->setWindowTitle("Clone database");
}

cloneDatabase::~cloneDatabase()
{
    delete ui;
}

void cloneDatabase::on_cmdbrowse_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),QDir::homePath() + "new.sqlite" + QDir::separator(),tr("Databases (*.sqlite)"));
    if (fileName != "")
    {
        if (fileName.indexOf(".sqlite") == -1)
            fileName = fileName + ".sqlite";
        ui->txtpath->setText(fileName);
        db_path = fileName;
    }
}

void cloneDatabase::setFrom(QString code, QString name, QString path)
{
    ui->lblcode->setText(code);
    ui->lblname->setText(name);
    ui->lblpath->setText(path);
}

void cloneDatabase::on_buttonBox_accepted()
{
    if (ui->txtpath->text() != "")
    {
        if (!QFile::exists(ui->txtpath->text()))
        {
            if (ui->txtcode->text().simplified() != "")
            {
                if (ui->txtname->text().simplified() != "")
                {
                    db_code = ui->txtcode->text();
                    db_name = ui->txtname->text();
                    db_path = ui->txtpath->text();
                    this->close();
                }
                else
                {
                    QMessageBox msgBox;
                    msgBox.setText("You need to provide a new name");
                    msgBox.exec();
                }
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText("You need to provide a new code");
                msgBox.exec();
            }
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("The database file already exists.");
            msgBox.exec();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You need to provide database new file");
        msgBox.exec();
    }
}


void cloneDatabase::on_buttonBox_rejected()
{
    this->db_code = "";
    this->close();
}

