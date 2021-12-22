#ifndef RUNMODELSDIALOG_H
#define RUNMODELSDIALOG_H

#include <QDialog>

namespace Ui {
class runModelsDialog;
}

class runModelsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit runModelsDialog(QWidget *parent = nullptr);
    ~runModelsDialog();
    void addModel(QString modelFile);
    QStringList selectedFiles;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::runModelsDialog *ui;
};

#endif // RUNMODELSDIALOG_H
