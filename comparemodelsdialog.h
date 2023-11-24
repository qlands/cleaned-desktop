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


class RadioListItem : public QObject, public QListWidgetItem
{
    Q_OBJECT
public:
    RadioListItem(const QString &text, QListWidget *parent = nullptr)
        : QListWidgetItem(text, parent)
    {
        setText("");
        // Create a widget for the item
        QWidget *widget = new QWidget(parent);

        // Set up the layout for the widget
        QHBoxLayout *layout = new QHBoxLayout(widget);
        layout->setContentsMargins(0, 0, 0, 0);
        //layout->setStretch(0, 3);
        //layout->setStretch(1, 1);

        // Add the text to the first column
        textLabel = new QLabel(text, widget);
        layout->addWidget(textLabel);

        // Add the radio button to the second column
        radioBtn = new QRadioButton(widget);
        connect(radioBtn, SIGNAL(toggled(bool)), this, SLOT(radioButtonToggled(bool)));
        radioBtn->setText("Is base?");
        layout->addWidget(radioBtn);

        // Set the widget for the item
        parent->setItemWidget(this, widget);
    }

    QRadioButton* getRadioButton() const {
        return radioBtn;
    }

    QString getResult() {
        return textLabel->text();
    }

private slots:
    void radioButtonToggled(bool checked) {
        emit listWidget()->itemChanged(this);
    }
private:

    QLabel *textLabel = nullptr;
    QRadioButton *radioBtn = nullptr;
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
