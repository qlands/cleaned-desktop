#ifndef DELEGATORDIALOG_H
#define DELEGATORDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace Ui {
class delegatorDialog;
}

class delegatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit delegatorDialog(QWidget *parent = nullptr);
    ~delegatorDialog();
    void load_manure(QSqlDatabase db);
    void set_caption(QString caption);
    void load_sourceType();
    void load_waterregime();
    void load_reice_ecosystem_type();
    void load_organic_amendment();
    void load_landcover(QSqlDatabase db);
    void load_slope(QSqlDatabase db);
    void load_grassmanagement(QSqlDatabase db);
    QString selected_code;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::delegatorDialog *ui;
};

#endif // DELEGATORDIALOG_H
