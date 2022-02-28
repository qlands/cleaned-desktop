#include "managedatabases.h"
#include "ui_managedatabases.h"
#include <QSettings>
#include "importdatabase.h"
#include <QMessageBox>
#include <QFile>
#include "clonedatabase.h"
#include <QDebug>
#include <QProcess>

ManageDatabases::ManageDatabases(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageDatabases)
{
    ui->setupUi(this);
    m_databases = new databaseModel(this);
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    m_databases->loadDatabases(settings.value("database_file").toString());
    ui->tableView->setModel(m_databases);
    ui->tableView->resizeColumnsToContents();
    this->setWindowTitle("Database manager");

    QString dbman_file = settings.value("dbman_file","").toString();
    if (dbman_file.simplified() != "" && QFile::exists(dbman_file))
    {
        ui->cmdeditdata->setEnabled(true);
        db_browser_binary = dbman_file;
    }
    else
    {
        ui->cmdeditdata->setEnabled(false);
        ui->cmdeditdata->setText("You need to configure DB Browser for SQLite in the settings");
    }
}

ManageDatabases::~ManageDatabases()
{
    delete ui;
}

void ManageDatabases::on_cmdClose_clicked()
{
    this->close();
}


void ManageDatabases::on_cmdImport_clicked()
{
    importDatabase impDB;
    impDB.exec();
    if (impDB.db_code != "")
    {
        if (!m_databases->addDatabase(impDB.db_code, impDB.db_name, impDB.db_path))
        {
            QMessageBox msgBox;
            msgBox.setText("The code already exists");
            msgBox.exec();
        }
    }
}


void ManageDatabases::on_cmdClone_clicked()
{
    if (ui->tableView->selectionModel()->selectedRows().count() == 1)
    {
        int row;
        row = ui->tableView->selectionModel()->selectedRows()[0].row();
        QString db_path = m_databases->getDBPath(row);
        if (QFile::exists(db_path))
        {
            cloneDatabase clone;
            QString db_code = m_databases->getDBCode(row);
            QString db_name = m_databases->getDBName(row);
            clone.setFrom(db_code,db_name,db_path);
            clone.exec();
            if (clone.db_code != "")
            {
                if (m_databases->addDatabase(clone.db_code, clone.db_name, clone.db_path))
                {
                    QFile::copy(db_path, clone.db_path);
                }
                else
                {
                    QMessageBox msgBox;
                    msgBox.setText("The code already exists");
                    msgBox.exec();
                }
            }
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("The database file does not exist");
            msgBox.exec();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You need to select a database");
        msgBox.exec();
    }
}


void ManageDatabases::on_cmdDelete_clicked()
{
    if (ui->tableView->selectionModel()->selectedRows().count() == 1)
    {
        int row;
        row = ui->tableView->selectionModel()->selectedRows()[0].row();
        if (m_databases->isDBRemovable(row))
        {
            QString db_code = m_databases->getDBCode(row);
            QMessageBox msgBox;
            msgBox.setText("Delete database \"" + db_code + "\"");
            msgBox.setInformativeText("Do you want to delete this database?");
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Ok)
            {
                m_databases->deleteDatabase(row);
            }
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("You cannot delete this database");
            msgBox.exec();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You need to select a database");
        msgBox.exec();
    }
}


void ManageDatabases::on_cmdeditdata_clicked()
{
    if (ui->tableView->selectionModel()->selectedRows().count() == 1)
    {
        int row;
        row = ui->tableView->selectionModel()->selectedRows()[0].row();
        QString db_path = m_databases->getDBPath(row);
        if (QFile::exists(db_path))
        {
            QString db_code = m_databases->getDBCode(row);
            if (db_code != "empty")
            {
                QStringList arguments;
                arguments << db_path;
                QProcess *myProcess = new QProcess(this);
                myProcess->start(db_browser_binary, arguments);
                myProcess->waitForFinished(-1);
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText("You cannot edit the empty database. You need to clone it first.");
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
}

void ManageDatabases::set_controls(bool status)
{
    ui->cmdeditdata->setEnabled(status);
    ui->cmdClone->setEnabled(status);
    ui->cmdClose->setEnabled(status);
    ui->cmdDelete->setEnabled(status);
    ui->cmdImport->setEnabled(status);
    ui->tableView->setEnabled(status);
}

