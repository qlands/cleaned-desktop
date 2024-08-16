#ifndef COMPAREMODELSDIALOG_H
#define COMPAREMODELSDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QListWidgetItem>

namespace Ui {
class compareModelsDialog;
}


class CustomListWidgetItem : public QObject, public QListWidgetItem
{
    Q_OBJECT
public:
    CustomListWidgetItem(const QString &text, QListWidget *parent = nullptr)
        : QListWidgetItem(text, parent)
    {
        setText("");
        QWidget *widget = new QWidget(parent);

        QHBoxLayout *layout = new QHBoxLayout(widget);
        layout->setContentsMargins(0, 0, 0, 0);

        _label = new QLabel(text, widget);
        layout->addWidget(_label);

        _radioButton = new QRadioButton(widget);
        connect(_radioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonToggled(bool)));
        _radioButton->setText("Is base?");
        layout->addWidget(_radioButton);

        parent->setItemWidget(this, widget);
    }

    QRadioButton* getRadioButton() const {
        return _radioButton;
    }

    QString getResult() {
        return _label->text();
    }

private slots:
    void radioButtonToggled([[maybe_unused]] bool checked) {
        emit listWidget()->itemChanged(this);
    }
private:

    QLabel *_label = nullptr;
    QRadioButton *_radioButton = nullptr;
};

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
    void handleItemChanged(QListWidgetItem *item);
private:
    Ui::compareModelsDialog *ui;
};

#endif // COMPAREMODELSDIALOG_H
