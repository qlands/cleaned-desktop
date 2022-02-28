#ifndef CLONEDATABASE_H
#define CLONEDATABASE_H

#include <QDialog>

namespace Ui {
class cloneDatabase;
}

class cloneDatabase : public QDialog
{
    Q_OBJECT

public:
    explicit cloneDatabase(QWidget *parent = nullptr);
    ~cloneDatabase();
    void setFrom(QString code, QString name, QString path);
    QString db_code;
    QString db_name;
    QString db_path;
private slots:
    void on_cmdbrowse_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::cloneDatabase *ui;
};

#endif // CLONEDATABASE_H
