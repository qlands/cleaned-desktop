#ifndef ABOUTCLEANED_H
#define ABOUTCLEANED_H

#include <QDialog>

namespace Ui {
class AboutCleaned;
}

class AboutCleaned : public QDialog
{
    Q_OBJECT

public:
    explicit AboutCleaned(QWidget *parent = nullptr);
    ~AboutCleaned();

private slots:
    void on_cmdClose_clicked();

private:
    Ui::AboutCleaned *ui;
};

#endif // ABOUTCLEANED_H
