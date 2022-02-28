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

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_cmd_ghg_clicked();

    void on_cmd_stock_clicked();

    void on_cmd_energy_clicked();

    void on_cmdbrowser_clicked();

    void on_cmdbrowsedbman_clicked();

private:
    Ui::settings *ui;
};

#endif // SETTINGS_H
