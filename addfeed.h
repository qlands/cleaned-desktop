#ifndef ADDFEED_H
#define ADDFEED_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class addFeed;
}

class addFeed : public QDialog
{
    Q_OBJECT

public:
    explicit addFeed(QWidget *parent = nullptr);
    ~addFeed();
    void loadData(QSqlDatabase cleaned_db);
    QString getCropCode();
    QString getFeedCode();
private slots:
    void on_cbm_crop_currentIndexChanged(int index);

private:
    Ui::addFeed *ui;
    QSqlDatabase db;
};

#endif // ADDFEED_H
