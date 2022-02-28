#ifndef SELECTDATABASE_H
#define SELECTDATABASE_H

#include <QDialog>

namespace Ui {
class selectDatabase;
}

class selectDatabase : public QDialog
{
    Q_OBJECT

public:
    explicit selectDatabase(QWidget *parent = nullptr);
    ~selectDatabase();
    QString selected_code;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::selectDatabase *ui;
};

#endif // SELECTDATABASE_H
