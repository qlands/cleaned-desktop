#ifndef IMPORTDATABASE_H
#define IMPORTDATABASE_H

#include <QDialog>

namespace Ui {
class importDatabase;
}

class importDatabase : public QDialog
{
    Q_OBJECT

public:
    explicit importDatabase(QWidget *parent = nullptr);
    ~importDatabase();
    QString db_name;
    QString db_code;
    QString db_path;

private slots:
    void on_cmdBrowse_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::importDatabase *ui;
};

#endif // IMPORTDATABASE_H
