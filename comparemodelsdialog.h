#ifndef COMPAREMODELSDIALOG_H
#define COMPAREMODELSDIALOG_H

#include <QDialog>

namespace Ui {
class compareModelsDialog;
}

class compareModelsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit compareModelsDialog(QWidget *parent = nullptr);
    ~compareModelsDialog();
    void addModel(QString modelFile);
    QStringList selectedFiles;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::compareModelsDialog *ui;
};

#endif // COMPAREMODELSDIALOG_H
