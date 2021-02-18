#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class settings;
}

class ModelSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ModelSettings(QWidget *parent = nullptr);
    ~ModelSettings();
    QString getModelFile();
    void setModelFile(QString modelFile);

private slots:
    void on_cmb_browse_model_clicked();

private:
    Ui::settings *ui;
};

#endif // SETTINGS_H
