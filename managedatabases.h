#ifndef MANAGEDATABASES_H
#define MANAGEDATABASES_H

#include <QDialog>
#include "databasemodel.h"

namespace Ui {
class ManageDatabases;
}

class ManageDatabases : public QDialog
{
    Q_OBJECT

public:
    explicit ManageDatabases(QWidget *parent = nullptr);
    ~ManageDatabases();

private slots:
    void on_cmdClose_clicked();

    void on_cmdImport_clicked();

    void on_cmdClone_clicked();

    void on_cmdDelete_clicked();

    void on_cmdeditdata_clicked();

private:
    Ui::ManageDatabases *ui;
    databaseModel *m_databases;
    QString db_browser_binary;
    void set_controls(bool status);
};

#endif // MANAGEDATABASES_H
