#ifndef TECHNICALMANUAL_H
#define TECHNICALMANUAL_H

#include <QWidget>
#include <QDialog>


namespace Ui {
class TechnicalManual;
}

class TechnicalManual : public QDialog
{
    Q_OBJECT

public:
    explicit TechnicalManual(QWidget *parent = nullptr);
    ~TechnicalManual();

private slots:
    void on_cmdClose_clicked();

private:
    Ui::TechnicalManual *ui;
};

#endif // TECHNICALMANUAL_H
