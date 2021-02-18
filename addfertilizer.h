#ifndef ADDFERTILIZER_H
#define ADDFERTILIZER_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class addFertilizer;
}

class addFertilizer : public QDialog
{
    Q_OBJECT

public:
    explicit addFertilizer(QWidget *parent = nullptr);
    ~addFertilizer();
    void loadData(QSqlDatabase cleaned_db);
    QString getFertilizerCode();
private:
    Ui::addFertilizer *ui;
};

#endif // ADDFERTILIZER_H
