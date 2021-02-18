#ifndef ADDLIVESTOCK_H
#define ADDLIVESTOCK_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class addLivestock;
}

class addLivestock : public QDialog
{
    Q_OBJECT

public:
    explicit addLivestock(QWidget *parent = nullptr);
    ~addLivestock();
    void loadData(QSqlDatabase cleaned_db);
    QString getLivestockCode();
private:
    Ui::addLivestock *ui;    
};

#endif // ADDLIVESTOCK_H
