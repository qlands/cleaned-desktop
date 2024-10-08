#ifndef COMPAREMODELSRESULTDIALOG_H
#define COMPAREMODELSRESULTDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QListWidget>
#include <QVector>

namespace Ui {
class comparemodelsresultdialog;
}

class compareModelsResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit compareModelsResultDialog(QWidget *parent, QStringList& results, QString baseName = "");
    ~compareModelsResultDialog();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onImageSelected(QListWidgetItem *current, QListWidgetItem *previous);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void modelFinished(int exitCode);

private:
    void compare();

private:
    Ui::comparemodelsresultdialog *ui;
    QWidget *_resultTab;
    QStringList _results;
    QString _pngsDirectory;
    QString _comparisonModel;
    QString _outputFile;
    QSharedPointer<QProcess> _process;
    QVector<QSharedPointer<QPixmap>> _pngs;
    QString _baseName;
};

#endif // COMPAREMODELSRESULTDIALOG_H
