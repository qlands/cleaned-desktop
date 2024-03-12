#ifndef ABOUTCLEANED_H
#define ABOUTCLEANED_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>

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
    void onLinkActivated(const QString &link) {
        // Handle the link activation, for example, open the link in the default web browser
        QDesktopServices::openUrl(QUrl(link));
    }
private:
    Ui::AboutCleaned *ui;
};

#endif // ABOUTCLEANED_H
