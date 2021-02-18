#ifndef ADDSEASON_H
#define ADDSEASON_H

#include <QDialog>

namespace Ui {
class addSeason;
}

class addSeason : public QDialog
{
    Q_OBJECT

public:
    explicit addSeason(QWidget *parent = nullptr);
    ~addSeason();
    QString getSeasonName();
private:
    Ui::addSeason *ui;
};

#endif // ADDSEASON_H
