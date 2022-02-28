#include "selectdatabase.h"
#include "ui_selectdatabase.h"
#include <QSettings>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

selectDatabase::selectDatabase(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selectDatabase)
{
    ui->setupUi(this);
    this->setWindowTitle("Select a database");
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QString database_file = settings.value("database_file").toString();

    QFile loadFile(database_file);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open database file.");
        this->close();
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject databases = loadDoc.object();
    QString databaseCode;
    foreach(databaseCode, databases.keys())
    {
        if (databaseCode != "empty")
        {
            QJsonObject a_database = databases[databaseCode].toObject();
            ui->cmbdb->addItem(a_database["name"].toString(), databaseCode);
        }
    }
    if (ui->cmbdb->count() > 0)
        ui->cmbdb->setCurrentIndex(0);
    loadFile.close();
}

selectDatabase::~selectDatabase()
{
    delete ui;
}

void selectDatabase::on_buttonBox_accepted()
{
    if (ui->cmbdb->currentIndex() >= 0)
        selected_code = ui->cmbdb->currentData().toString();
    else
        selected_code = "";
    this->close();
}


void selectDatabase::on_buttonBox_rejected()
{
    selected_code = "";
    this->close();
}

