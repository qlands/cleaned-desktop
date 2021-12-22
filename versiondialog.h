#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include <QDialog>

namespace Ui {
class VersionDialog;
}

class VersionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VersionDialog(QWidget *parent = nullptr);
    ~VersionDialog();

private slots:
    void on_cmdclose_clicked();

private:
    Ui::VersionDialog *ui;
};

#endif // VERSIONDIALOG_H
