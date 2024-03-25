#include <QtWidgets>
#include "cleanedstudy.h"
#include "ui_cleanedstudy.h"
#include <QUuid>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QJsonDocument>
#include <QDebug>
#include <QDoubleValidator>
#include "addlivestock.h"
#include "addfeed.h"
#include "combodelegate.h"
#include <QBrush>
#include <QMessageBox>
#include "addfertilizer.h"
#include "addseason.h"
#include <QFileInfo>
#include "delegatordialog.h"
#include "xlsxdocument.h"
#include <QFont>
#include <QFontMetrics>
#include <QFileInfo>

CleanedStudy::CleanedStudy(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CleanedStudy),
    _resultTab(nullptr),
    _progressTab(nullptr)
{
    ui->setupUi(this);
    _resultTab = ui->tabResult;
    _progressTab = ui->study_results_tab;
    ui->tabWidget_2->removeTab(2);
    ui->tabWidget_2->removeTab(1);
    static int sequenceNumber = 1;
    model_running = false;
    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
    m_seasons = new seasonModel(this);
    connect(m_seasons,SIGNAL(seasonChanged(QString, QString)),this,SLOT(season_changed(QString, QString)));
    ui->seasonsView->setModel(m_seasons);
    m_livestock = new livestockModel(this);
    ui->livestockView->setModel(m_livestock);
    ui->stackedWidget->setCurrentIndex(0);
    m_feeds = new feedsModel(this);
    ui->feedsView->setModel(m_feeds);
    m_process = new QProcess(this);
    connect(m_process,SIGNAL(finished(int)),this,SLOT(modelFinished(int)));
    connect(m_process,SIGNAL(readyReadStandardError()),this,SLOT(readyReadStandardError()));
    connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadStandardOutput()));
    connect(ui->pngList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onImageSelected(QListWidgetItem*,QListWidgetItem*)));

    m_cropInputs = new cropInputsModel(this);
    ui->cropsView->setModel(m_cropInputs);
    m_fertilizer = new fertilizerModel(this);
    ui->fertilizerView->setModel(m_fertilizer);

    m_basktet = new basketModel(this);
    ui->basketView->setModel(m_basktet);

    // Track chages in the models
    connect(m_seasons,SIGNAL(modelChanged()),this,SLOT(modelChanged()));
    connect(m_livestock,SIGNAL(modelChanged()),this,SLOT(modelChanged()));
    connect(m_feeds,SIGNAL(modelChanged()),this,SLOT(modelChanged()));
    connect(m_cropInputs,SIGNAL(modelChanged()),this,SLOT(modelChanged()));
    connect(m_fertilizer,SIGNAL(modelChanged()),this,SLOT(modelChanged()));
    connect(m_basktet,SIGNAL(modelChanged()),this,SLOT(modelChanged()));

    BrowseChildren(this->ui->tabWidget);

}

CleanedStudy::~CleanedStudy()
{
    ui->pngLabel->setPixmap(QPixmap());
    _pngs.clear();
    m_process->kill();
    delete ui;
}

bool CleanedStudy::openDatabase()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QString database_file = settings.value("database_file").toString();

    QFile loadFile(database_file);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        QMessageBox msgBox;
        msgBox.setText("Cannot open the database library file");
        msgBox.exec();
        return false;
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject databases = loadDoc.object();
    if (databases.keys().indexOf(this->database_code) >= 0)
    {
        QJsonObject a_database = databases[this->database_code].toObject();

        QUuid connectionUUID=QUuid::createUuid();
        QString strconnectionUUID=connectionUUID.toString().replace("{","").replace("}","");

        this->db = QSqlDatabase::addDatabase("QSQLITE",strconnectionUUID);
        QString database_file = a_database["path"].toString();
        if (QFile::exists(database_file))
        {
            this->db.setDatabaseName(database_file);
            if (!this->db.open())
            {
                QMessageBox msgBox;
                msgBox.setText("Cannot open the database");
                msgBox.exec();
                return false;
            }
            loading = true;
            load_models();
            loading = false;
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("The database file for code \'" + this->database_code + "\" does not exist");
            msgBox.exec();
            return false;
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("The database code \"" + this->database_code + "\" is not in the library");
        msgBox.exec();
        return false;
    }
    loadFile.close();
    return true;
}

void CleanedStudy::setDatabaseCode(QString code)
{
    database_code = code;
}

bool CleanedStudy::newFile()
{
    if (!openDatabase())
    {
        return false;
    }
    static int sequenceNumber = 1;
    studyModified = true;
    loading = false;
    isUntitled = true;
    curFile = tr("document%1.json").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
    ui->seasonsView->resizeColumnsToContents();
    //ui->livestockView->resizeColumnsToContents();
    ui->livestockView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    ui->livestockView->horizontalHeader()->setMinimumHeight(150);

    for (int ncol=0; ncol < m_livestock->columnCount(); ncol++)
    {
        livestock_colums.append(ui->livestockView->columnWidth(ncol));
    }


    //ui->feedsView->resizeColumnsToContents();
    ui->feedsView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    ui->feedsView->horizontalHeader()->setMinimumHeight(120);

    for (int ncol=0; ncol < m_feeds->columnCount(); ncol++)
    {
        feed_colums.append(ui->feedsView->columnWidth(ncol));
    }

    //ui->cropsView->resizeColumnsToContents();
    ui->cropsView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    ui->cropsView->horizontalHeader()->setMinimumHeight(120);

    this->study_object["database_code"] = this->database_code;

    //TODO: Notify that is a new file
//    connect(document(), &QTextDocument::contentsChanged,
//            this, &MdiChild::documentWasModified);
    return true;
}

bool CleanedStudy::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Cleaned study file"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QByteArray studyData = file.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(studyData));
    this->study_object = loadDoc.object();
    if (this->study_object.keys().indexOf("database_code") >= 0)
    {
        this->database_code = this->study_object["database_code"].toString();
        if (!openDatabase())
            return false;
    }
    else
    {
        this->database_code = "base";
        this->study_object["database_code"] = "base";
        if (!openDatabase())
            return false;
    }
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    loading = true;
    loadStudyObject();
    loading = false;
    QGuiApplication::restoreOverrideCursor();

    setCurrentFile(fileName);


    QString outFile = curFile;
    outFile = outFile.replace(".json","_result.json");
    QFileInfo fileInfo(outFile);
    if (fileInfo.exists())
    {
        ui->stackedWidget->setCurrentIndex(2);
        QFile results_file(outFile);
        results_file.open(QFile::ReadOnly | QFile::Text);
        ui->json_result->setText(results_file.readAll());
        ui->json_result->setReadOnly(true);
    }


    // TODO: We need to find a way to notify that the file has changed
//    connect(document(), &QTextDocument::contentsChanged,
//            this, &MdiChild::documentWasModified);

    return true;
}

void CleanedStudy::load_models()
{
    // Load the regions Combo Box
    QSqlQuery query(this->db);
    if (query.exec("SELECT region_code,region_desc FROM lkp_region"))
    {
        while(query.next())
        {
            ui->cbm_region->addItem(query.value(1).toString(), QVariant(query.value(0).toString()));
        }
    }
    else
        emit report_error("Unable to query regions");
    if (ui->cbm_region->count() > 0)
        ui->cbm_region->setCurrentIndex(0);

    // Load the climate Combo Box
    if (query.exec("select climate_code from lkp_climate"))
    {
        while(query.next())
        {
            ui->cbm_climate->addItem(query.value(0).toString(), QVariant(query.value(0).toString()));
        }
    }
    else
        emit report_error("Unable to query climate");
    if (ui->cbm_climate->count() > 0)
    {
        ui->cbm_climate->setCurrentIndex(0);
        // Load subclimate
        ui->cbm_subclimate->clear();
        if (query.exec("select climate2_code from lkp_climate2 WHERE climate_code = '" + ui->cbm_climate->currentText() + "'"))
        {
            while(query.next())
            {
                ui->cbm_subclimate->addItem(query.value(0).toString(), QVariant(query.value(0).toString()));
            }
        }
        else
            emit report_error("Unable to query sub-climate");
        if (ui->cbm_subclimate->count() > 0)
            ui->cbm_subclimate->setCurrentIndex(0);
    }


    ui->txt_purchased_bedding->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_purchased_compost->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_purchased_manure->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_purchased_other->setValidator(new QDoubleValidator(0, 100, 2, this));

    // Waste management
    ui->txt_waste_prod_milk->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_waste_prod_meat->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_waste_dist_milk->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_waste_dist_meat->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_waste_proc_milk->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_waste_proc_meat->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_waste_cons_milk->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_waste_cons_meat->setValidator(new QDoubleValidator(0, 100, 2, this));

    //Land
    ui->txt_annual_prec->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_rain_length->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_soil_k_value->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_soil_n->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_soil_c->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_soil_clay->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_soil_bulk->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_soil_depth->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_et->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_cropland_system_ipcc->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_cropland_tillage_ipcc->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_cropland_orgmatter_ipcc->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_grassland_management_ipcc->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_grassland_implevel_ipcc->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_grassland_toarable->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_arable_tograssland->setValidator(new QDoubleValidator(0, 100, 2, this));
    bool comboHasData = false;
    if (query.exec("SELECT soil_desc,k_value FROM lkp_soil"))
    {
        while(query.next())
        {
            ui->cbm_soiltype->addItem(query.value(0).toString(), QVariant(query.value(1).toDouble()));
            comboHasData = true;
        }
    }
    else
        emit report_error("Unable to query lkp_soil");
    if (comboHasData)
        ui->cbm_soiltype->setCurrentIndex(0);

    comboHasData = false;
    if (query.exec("SELECT sys_desc,change_factor FROM lkp_croplandsystem"))
    {
        while(query.next())
        {
            ui->cbm_cropland_system->addItem(query.value(0).toString(), QVariant(query.value(1).toDouble()));
            comboHasData = true;
        }
    }
    else
        emit report_error("Unable to query lkp_croplandsystem");
    if (comboHasData)
        ui->cbm_cropland_system->setCurrentIndex(0);

    comboHasData = false;
    if (query.exec("SELECT tillage_desc,change_factor FROM lkp_tillageregime"))
    {
        while(query.next())
        {
            ui->cbm_cropland_tillage->addItem(query.value(0).toString(), QVariant(query.value(1).toDouble()));
            comboHasData = true;
        }
    }
    else
        emit report_error("Unable to query lkp_tillageregime");
    if (comboHasData)
        ui->cbm_cropland_tillage->setCurrentIndex(0);

    comboHasData = false;
    if (query.exec("SELECT orgmatter_desc,change_factor FROM lkp_organicmatter"))
    {
        while(query.next())
        {
            ui->cbm_cropland_orgmatter->addItem(query.value(0).toString(), QVariant(query.value(1).toDouble()));
            comboHasData = true;
        }
    }
    else
        emit report_error("Unable to query lkp_organicmatter");
    if (comboHasData)
        ui->cbm_cropland_orgmatter->setCurrentIndex(0);

    comboHasData = false;
    if (query.exec("SELECT management_desc,change_factor FROM lkp_grasslandman"))
    {
        while(query.next())
        {
            ui->cbm_grassland_management->addItem(query.value(0).toString(), QVariant(query.value(1).toDouble()));
            comboHasData = true;
        }
    }
    else
        emit report_error("Unable to query lkp_grasslandman");
    if (comboHasData)
        ui->cbm_grassland_management->setCurrentIndex(0);

    comboHasData = false;
    if (query.exec("SELECT grassinputlevel_desc,change_factor FROM lkp_grassinputlevel"))
    {
        while(query.next())
        {
            ui->cbm_grassland_implevel->addItem(query.value(0).toString(), QVariant(query.value(1).toDouble()));
            comboHasData = true;
        }
    }
    else
        emit report_error("Unable to query lkp_grassinputlevel");
    if (comboHasData)
        ui->cbm_grassland_implevel->setCurrentIndex(0);

    //Economic parameters

    //Livestock
    m_livestock->setDatabase(db);

    //Feeds
    m_feeds->setDatabase(db);

    //Crops
    m_cropInputs->setDatabase(db);

    //Fertilizer
    m_fertilizer->setDatabase(db);

    //Feed Basket
    m_basktet->setDatabase(db);
}

void CleanedStudy::saveStudyObject()
{
    this->study_object["farm_code"] = ui->txt_farmcode->text();
    this->study_object["farm_name"] = ui->txt_farmname->text();    
    this->study_object["region"] =  ui->cbm_region->currentData().toString();
    this->study_object["climate_zone"] =  ui->cbm_climate->currentData().toString();
    this->study_object["climate_zone_2"] =  ui->cbm_subclimate->currentData().toString();
    // Save seasons
    study_object["seasons"] = m_seasons->getSeasonArray();
    // Save manure section    
    this->study_object["purchased_manure"] = ui->txt_purchased_manure->text().toDouble();
    this->study_object["purchased_compost"] = ui->txt_purchased_compost->text().toDouble();
    this->study_object["purchased_organic_n"] = ui->txt_purchased_other->text().toDouble();    
    this->study_object["purchased_bedding"] = ui->txt_purchased_bedding->text().toDouble();
    // Save waste management
    this->study_object["waste_production_milk"] = ui->txt_waste_prod_milk->text().toDouble();
    this->study_object["waste_production_meat"] = ui->txt_waste_prod_meat->text().toDouble();
    this->study_object["waste_distribution_milk"] = ui->txt_waste_dist_milk->text().toDouble();
    this->study_object["waste_distribution_meat"] = ui->txt_waste_dist_meat->text().toDouble();
    this->study_object["waste_processing_milk"] = ui->txt_waste_proc_milk->text().toDouble();
    this->study_object["waste_processing_meat"] = ui->txt_waste_proc_meat->text().toDouble();
    this->study_object["waste_consume_milk"] = ui->txt_waste_cons_milk->text().toDouble();
    this->study_object["waste_consume_meat"] = ui->txt_waste_cons_meat->text().toDouble();
    // Save land
    this->study_object["annual_prec"] = ui->txt_annual_prec->text().toDouble();
    this->study_object["rain_length"] = ui->txt_rain_length->text().toDouble();
    this->study_object["soil_description"] = ui->cbm_soiltype->currentText();
    this->study_object["soil_k_value"] = ui->txt_soil_k_value->text().toDouble();
    this->study_object["soil_n"] = ui->txt_soil_n->text().toDouble();
    this->study_object["soil_c"] = ui->txt_soil_c->text().toDouble();
    this->study_object["soil_clay"] = ui->txt_soil_clay->text().toDouble();
    this->study_object["soil_bulk"] = ui->txt_soil_bulk->text().toDouble();
    this->study_object["soil_depth"] = ui->txt_soil_depth->text().toDouble();
    this->study_object["et"] = ui->txt_et->text().toDouble();
    this->study_object["cropland_system_ipcc"] = ui->txt_cropland_system_ipcc->text().toDouble();
    this->study_object["cropland_tillage_ipcc"] = ui->txt_cropland_tillage_ipcc->text().toDouble();
    this->study_object["cropland_orgmatter_ipcc"] = ui->txt_cropland_orgmatter_ipcc->text().toDouble();
    this->study_object["grassland_management_ipcc"] = ui->txt_grassland_management_ipcc->text().toDouble();
    this->study_object["grassland_implevel_ipcc"] = ui->txt_grassland_implevel_ipcc->text().toDouble();
    this->study_object["grassland_toarable"] = ui->txt_grassland_toarable->text().toDouble();
    this->study_object["arable_tograssland"] = ui->txt_arable_tograssland->text().toDouble();
    this->study_object["cropland_system"] = ui->cbm_cropland_system->currentText();
    this->study_object["cropland_tillage"] = ui->cbm_cropland_tillage->currentText();
    this->study_object["cropland_orgmatter"] = ui->cbm_cropland_orgmatter->currentText();
    this->study_object["grassland_management"] = ui->cbm_grassland_management->currentText();
    this->study_object["grassland_implevel"] = ui->cbm_grassland_implevel->currentText();
    //Save economic params

    // Save livestock
    study_object["livestock"] = m_livestock->getLivestockArray();
    QJsonArray feeds_array = m_feeds->getFeedsArray();
    QJsonArray crops_array = m_cropInputs->getFeedsArray();
    for (int i = 0; i < crops_array.count(); i++)
    {
//        qDebug() << "-------------------A----------------------";
//        qDebug() << crops_array[i].toObject()["feed_type_code"].toString();
//        qDebug() << crops_array[i].toObject()["feed_item_code"].toString();
//        qDebug() << "-------------------A----------------------";
        for (int b=0; b < feeds_array.count(); b++)
        {
            if (crops_array[i].toObject()["feed_type_code"].toString() == feeds_array[b].toObject()["feed_type_code"].toString())
            {
                if (crops_array[i].toObject()["feed_item_code"].toString() == feeds_array[b].toObject()["feed_item_code"].toString())
                {
                    QJsonObject obj = feeds_array[b].toObject();

                    obj["fraction_as_fertilizer"] = crops_array[i].toObject()["fraction_as_fertilizer"];
                    obj["fraction_as_manure"] = crops_array[i].toObject()["fraction_as_manure"];
                    obj["urea"] = crops_array[i].toObject()["urea"];
                    obj["npk"] = crops_array[i].toObject()["npk"];
                    obj["dap"] = crops_array[i].toObject()["dap"];
                    obj["ammonium_nitrate"] = crops_array[i].toObject()["ammonium_nitrate"];
                    obj["ammonium_sulfate"] = crops_array[i].toObject()["ammonium_sulfate"];
                    obj["n_solutions"] = crops_array[i].toObject()["n_solutions"];
                    obj["ammonia"] = crops_array[i].toObject()["ammonia"];
                    obj["n_fertilizer"] = crops_array[i].toObject()["n_fertilizer"];
                    feeds_array[b] = obj;
                }
            }
        }
    }

    study_object["feed_items"] = feeds_array;
    // Fertilizer
    study_object["fertilizer"] = m_fertilizer->getFertilizerArray();

    // FeedBasket
    if (study_object["seasons"].toArray().count() > 0 && study_object["livestock"].toArray().count() > 0 && study_object["feed_items"].toArray().count() > 0)
        study_object["feed_basket"] = m_basktet->getBasketArray();
}

void CleanedStudy::loadStudyObject()
{
    studyModified = false;
    ui->txt_farmcode->setText(study_object["farm_code"].toString());
    ui->txt_farmname->setText(study_object["farm_name"].toString());
    int region_index = ui->cbm_region->findData(this->study_object["region"].toVariant());
    if (region_index >= 0)
        ui->cbm_region->setCurrentIndex(region_index);

    //Load climate
    int climate_index = ui->cbm_climate->findData(this->study_object["climate_zone"].toVariant());
    if (climate_index >= 0)
    {
        ui->cbm_climate->setCurrentIndex(climate_index);
        ui->cbm_subclimate->clear();
        QSqlQuery query(this->db);
        if (query.exec("select climate2_code from lkp_climate2 WHERE climate_code = '" + ui->cbm_climate->currentText() + "'"))
        {
            while(query.next())
            {
                ui->cbm_subclimate->addItem(query.value(0).toString(), QVariant(query.value(0).toString()));
            }
        }
        climate_index = ui->cbm_subclimate->findData(this->study_object["climate_zone_2"].toVariant());
        if (climate_index >= 0)
            ui->cbm_subclimate->setCurrentIndex(climate_index);
    }



    // Load seasons
    if (study_object.contains("seasons") && study_object["seasons"].isArray())
    {
        QJsonArray seasonsArray;
        seasonsArray = study_object["seasons"].toArray();
        m_seasons->setDataArray(seasonsArray);
    }
    ui->seasonsView->resizeColumnsToContents();

    if (study_object.contains("purchased_manure"))
        ui->txt_purchased_manure->setText(QString::number(study_object["purchased_manure"].toDouble()));
    if (study_object.contains("purchased_compost"))
        ui->txt_purchased_compost->setText(QString::number(study_object["purchased_compost"].toDouble()));
    if (study_object.contains("purchased_organic_n"))
        ui->txt_purchased_other->setText(QString::number(study_object["purchased_organic_n"].toDouble()));
    if (study_object.contains("purchased_bedding"))
        ui->txt_purchased_bedding->setText(QString::number(study_object["purchased_bedding"].toDouble()));

    // Load waste management
    if (study_object.contains("waste_production_milk"))
        ui->txt_waste_prod_milk->setText(QString::number(study_object["waste_production_milk"].toDouble()));
    if (study_object.contains("waste_production_meat"))
        ui->txt_waste_prod_meat->setText(QString::number(study_object["waste_production_meat"].toDouble()));
    if (study_object.contains("waste_distribution_milk"))
        ui->txt_waste_dist_milk->setText(QString::number(study_object["waste_distribution_milk"].toDouble()));
    if (study_object.contains("waste_distribution_meat"))
        ui->txt_waste_dist_meat->setText(QString::number(study_object["waste_distribution_meat"].toDouble()));
    if (study_object.contains("waste_processing_milk"))
        ui->txt_waste_proc_milk->setText(QString::number(study_object["waste_processing_milk"].toDouble()));
    if (study_object.contains("waste_processing_meat"))
        ui->txt_waste_proc_meat->setText(QString::number(study_object["waste_processing_meat"].toDouble()));
    if (study_object.contains("waste_consume_milk"))
        ui->txt_waste_cons_milk->setText(QString::number(study_object["waste_consume_milk"].toDouble()));
    if (study_object.contains("waste_consume_meat"))
        ui->txt_waste_cons_meat->setText(QString::number(study_object["waste_consume_meat"].toDouble()));
    //Load land
    int combo_index;
    if (study_object.contains("cropland_system"))
    {
        combo_index = ui->cbm_cropland_system->findText(study_object["cropland_system"].toString());
        if (combo_index >= 0)
            ui->cbm_cropland_system->setCurrentIndex(combo_index);
    }
    if (study_object.contains("cropland_tillage"))
    {
        combo_index = ui->cbm_cropland_tillage->findText(study_object["cropland_tillage"].toString());
        if (combo_index >= 0)
            ui->cbm_cropland_tillage->setCurrentIndex(combo_index);
    }
    if (study_object.contains("cropland_orgmatter"))
    {
        combo_index = ui->cbm_cropland_orgmatter->findText(study_object["cropland_orgmatter"].toString());
        if (combo_index >= 0)
            ui->cbm_cropland_orgmatter->setCurrentIndex(combo_index);
    }
    if (study_object.contains("grassland_management"))
    {
        combo_index = ui->cbm_grassland_management->findText(study_object["grassland_management"].toString());
        if (combo_index >= 0)
            ui->cbm_grassland_management->setCurrentIndex(combo_index);
    }
    if (study_object.contains("grassland_implevel"))
    {
        combo_index = ui->cbm_grassland_implevel->findText(study_object["grassland_implevel"].toString());
        if (combo_index >= 0)
            ui->cbm_grassland_implevel->setCurrentIndex(combo_index);
    }
    if (study_object.contains("annual_prec"))
        ui->txt_annual_prec->setText(QString::number(study_object["annual_prec"].toDouble()));
    if (study_object.contains("rain_length"))
        ui->txt_rain_length->setText(QString::number(study_object["rain_length"].toDouble()));

    if (study_object.contains("soil_description"))
    {
        int soil_index = ui->cbm_soiltype->findText(study_object["soil_description"].toString());
        if (soil_index >= 0)
            ui->cbm_soiltype->setCurrentIndex(soil_index);
    }
    else
         ui->cbm_soiltype->setCurrentIndex(0);

    if (study_object.contains("soil_k_value"))
        ui->txt_soil_k_value->setText(QString::number(study_object["soil_k_value"].toDouble()));
    else
    {
        if (ui->cbm_soiltype->currentData().isValid())
            ui->txt_soil_k_value->setText(QString::number(ui->cbm_soiltype->currentData().toDouble()));
    }
    if (study_object.contains("soil_n"))
        ui->txt_soil_n->setText(QString::number(study_object["soil_n"].toDouble()));
    if (study_object.contains("soil_c"))
        ui->txt_soil_c->setText(QString::number(study_object["soil_c"].toDouble()));
    if (study_object.contains("soil_clay"))
        ui->txt_soil_clay->setText(QString::number(study_object["soil_clay"].toDouble()));
    if (study_object.contains("soil_bulk"))
        ui->txt_soil_bulk->setText(QString::number(study_object["soil_bulk"].toDouble()));
    if (study_object.contains("soil_depth"))
        ui->txt_soil_depth->setText(QString::number(study_object["soil_depth"].toDouble()));
    if (study_object.contains("et"))
        ui->txt_et->setText(QString::number(study_object["et"].toDouble()));
    if (study_object.contains("cropland_system_ipcc"))
        ui->txt_cropland_system_ipcc->setText(QString::number(study_object["cropland_system_ipcc"].toDouble()));
    else
    {
        if (ui->cbm_cropland_system->currentData().isValid())
            ui->txt_cropland_system_ipcc->setText(QString::number(ui->cbm_cropland_system->currentData().toDouble()));
    }
    if (study_object.contains("cropland_tillage_ipcc"))
        ui->txt_cropland_tillage_ipcc->setText(QString::number(study_object["cropland_tillage_ipcc"].toDouble()));
    else
    {
        if (ui->cbm_cropland_tillage->currentData().isValid())
            ui->txt_cropland_tillage_ipcc->setText(QString::number(ui->cbm_cropland_tillage->currentData().toDouble()));
    }
    if (study_object.contains("cropland_orgmatter_ipcc"))
        ui->txt_cropland_orgmatter_ipcc->setText(QString::number(study_object["cropland_orgmatter_ipcc"].toDouble()));
    else
    {
        if (ui->cbm_cropland_orgmatter->currentData().isValid())
            ui->txt_cropland_orgmatter_ipcc->setText(QString::number(ui->cbm_cropland_orgmatter->currentData().toDouble()));
    }
    if (study_object.contains("grassland_management_ipcc"))
        ui->txt_grassland_management_ipcc->setText(QString::number(study_object["grassland_management_ipcc"].toDouble()));
    else
    {
        if (ui->cbm_grassland_management->currentData().isValid())
            ui->txt_grassland_management_ipcc->setText(QString::number(ui->cbm_grassland_management->currentData().toDouble()));
    }
    if (study_object.contains("grassland_implevel_ipcc"))
        ui->txt_grassland_implevel_ipcc->setText(QString::number(study_object["grassland_implevel_ipcc"].toDouble()));
    else
    {
        if (ui->cbm_grassland_implevel->currentData().isValid())
            ui->txt_grassland_implevel_ipcc->setText(QString::number(ui->cbm_grassland_implevel->currentData().toDouble()));
    }
    if (study_object.contains("grassland_toarable"))
        ui->txt_grassland_toarable->setText(QString::number(study_object["grassland_toarable"].toDouble()));
    if (study_object.contains("arable_tograssland"))
        ui->txt_arable_tograssland->setText(QString::number(study_object["arable_tograssland"].toDouble()));
    // Load economic parameters


    // Load livestock
    if (study_object.contains("livestock") && study_object["livestock"].isArray())
    {
        QJsonArray livestockArray;
        livestockArray = study_object["livestock"].toArray();
        m_livestock->setDataArray(livestockArray);
    }

    ui->livestockView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    ui->livestockView->horizontalHeader()->setMinimumHeight(150);
    for (int ncol=0; ncol < m_livestock->columnCount(); ncol++)
    {
        if (ncol != 7 && ncol != 10 && ncol != 13 && ncol != 15)
            livestock_colums.append(ui->livestockView->columnWidth(ncol));
        else
        {
            livestock_colums.append(ui->livestockView->columnWidth(ncol));
            for (int nrow = 0; nrow < m_livestock->rowCount(); nrow++)
            {
                QModelIndex index = m_livestock->index(nrow, ncol);

                int width=ui->livestockView->fontMetrics().horizontalAdvance(m_livestock->data(index).toString());
                width = width + 10;
                if (width > livestock_colums[ncol])
                {
                    livestock_colums[ncol] = width;
                    ui->livestockView->setColumnWidth(ncol, width);
                }
            }
        }
    }

    // Load feeds
    if (study_object.contains("feed_items") && study_object["feed_items"].isArray())
    {
        QJsonArray feedsArray;
        feedsArray = study_object["feed_items"].toArray();
        m_feeds->setDataArray(feedsArray);
        m_cropInputs->setDataArray(feedsArray);
    }

    //ui->feedsView->resizeColumnsToContents();
    ui->feedsView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    ui->feedsView->horizontalHeader()->setMinimumHeight(120);
    for (int ncol=0; ncol < m_feeds->columnCount(); ncol++)
    {
        if (ncol != 2 && ncol != 6 && ncol != 8 && ncol != 11 && ncol != 19 && ncol != 20 && ncol != 21)
            feed_colums.append(ui->feedsView->columnWidth(ncol));
        else
        {
            feed_colums.append(ui->feedsView->columnWidth(ncol));
            for (int nrow = 0; nrow < m_feeds->rowCount(); nrow++)
            {
                QModelIndex index = m_feeds->index(nrow, ncol);

                int width=ui->feedsView->fontMetrics().horizontalAdvance(m_feeds->data(index).toString());
                width = width + 10;
                if (width > feed_colums[ncol])
                {
                    feed_colums[ncol] = width;
                    ui->feedsView->setColumnWidth(ncol, width);
                }
            }
        }
    }



    //ui->cropsView->resizeColumnsToContents();
    ui->cropsView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    ui->cropsView->horizontalHeader()->setMinimumHeight(120);


    // Load fertilizer
    if (study_object.contains("fertilizer") && study_object["fertilizer"].isArray())
    {
        QJsonArray fertilizerArray;
        fertilizerArray = study_object["fertilizer"].toArray();
        m_fertilizer->setDataArray(fertilizerArray);
    }
    ui->fertilizerView->resizeColumnsToContents();

    // Load the FeedBasket
    if (study_object.contains("feed_basket") && study_object["feed_basket"].isArray())
    {
        if (study_object.contains("seasons") && study_object["seasons"].isArray())
        {
            QJsonArray seasonArray = study_object["seasons"].toArray();
            for (int seasonIndex = 0; seasonIndex < seasonArray.count(); seasonIndex++)
            {
                QJsonObject seasonObject = seasonArray[seasonIndex].toObject();
                m_basktet->addNewSeason(seasonObject["season_name"].toString(), false);
            }
        }
        if (study_object.contains("livestock") && study_object["livestock"].isArray())
        {
            QJsonArray livestockArray = study_object["livestock"].toArray();
            for (int livestockIndex = 0; livestockIndex < livestockArray.count(); livestockIndex ++)
            {
                QJsonObject livestockObject = livestockArray[livestockIndex].toObject();
                m_basktet->addNewLivestock(livestockObject["livetype_code"].toString(), false);
            }
        }
        if (study_object.contains("feed_items") && study_object["feed_items"].isArray())
        {
            QJsonArray feedsArray = study_object["feed_items"].toArray();
            for (int feedIndex = 0; feedIndex < feedsArray.count(); feedIndex++)
            {
                QJsonObject feedObject = feedsArray[feedIndex].toObject();
                m_basktet->addNewFeed(feedObject["feed_type_code"].toString(), feedObject["feed_item_code"].toString(), false);
            }
        }

        QJsonArray basketArray;
        basketArray = study_object["feed_basket"].toArray();
        m_basktet->setDataArray(basketArray);
        ui->basketView->resizeColumnsToContents();
    }
    else
    {
        if (study_object.contains("seasons") && study_object["seasons"].isArray())
        {
            if (study_object.contains("livestock") && study_object["livestock"].isArray())
            {
                if (study_object.contains("feed_items") && study_object["feed_items"].isArray())
                {
                    QJsonArray basketArray;
                    QJsonArray seasonArray = study_object["seasons"].toArray();
                    for (int seasonIndex = 0; seasonIndex < seasonArray.count(); seasonIndex++)
                    {
                        QJsonObject seasonObject = seasonArray[seasonIndex].toObject();
                        QJsonObject seasonBasketObject;
                        seasonBasketObject["season_name"] = seasonObject["season_name"].toString();
                        m_basktet->addNewSeason(seasonObject["season_name"].toString(), false);
                        QJsonArray feedBasketArray;

                        QJsonArray feedsArray = study_object["feed_items"].toArray();
                        for (int feedIndex = 0; feedIndex < feedsArray.count(); feedIndex++)
                        {
                            QJsonObject feedObject = feedsArray[feedIndex].toObject();
                            QJsonObject feedBasketObject;
                            feedBasketObject["feed_type_code"] = feedObject["feed_type_code"].toString();
                            feedBasketObject["feed_item_code"] = feedObject["feed_item_code"].toString();
                            m_basktet->addNewFeed(feedObject["feed_type_code"].toString(), feedObject["feed_item_code"].toString(), false);
                            QJsonArray livestockBasketArray;

                            QJsonArray livestockArray = study_object["livestock"].toArray();
                            for (int livestockIndex = 0; livestockIndex < livestockArray.count(); livestockIndex ++)
                            {
                                QJsonObject livestockObject = livestockArray[livestockIndex].toObject();
                                QJsonObject livestockBasketObject;
                                livestockBasketObject["livetype_code"] =  livestockObject["livetype_code"].toString();
                                livestockBasketObject["allocation"] = 0;
                                m_basktet->addNewLivestock(livestockObject["livetype_code"].toString(), false);
                                livestockBasketArray.append(livestockBasketObject);
                            }
                            feedBasketObject["livestock"] = livestockBasketArray;
                            feedBasketArray.append(feedBasketObject);
                        }
                        seasonBasketObject["feeds"] = feedBasketArray;
                        basketArray.append(seasonBasketObject);
                    }                    
                    qDebug() << "Set Array";
                    m_basktet->setDataArray(basketArray);
                    ui->basketView->resizeColumnsToContents();
                    qDebug() << "After set Array";
                }
            }
        }
    }
}


bool CleanedStudy::save()
{    
    if (isUntitled) {
        return saveAs();
    } else {
        if (m_seasons->getTotalDays() != 365)
        {
            QMessageBox::warning(this,tr("Seasons"),tr("Warning: The total number of days allocated to seasons is not 365"));
        }
        return saveFile(curFile);
    }
}

bool CleanedStudy::saveAs()
{
    if (m_seasons->getTotalDays() != 365)
    {
        QMessageBox::warning(this,tr("Seasons"),tr("Warning: The total number of days allocated to seasons is not 365"));
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;
    if (fileName.toLower().indexOf(".json") < 0)
        fileName = fileName + ".json";
    return saveFile(fileName);
}

bool CleanedStudy::saveFile(const QString &fileName)
{    
    QString errorMessage;
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    saveStudyObject();
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(this->study_object).toJson());
        qDebug() << fileName;
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("Cleaned"), errorMessage);
        return false;
    }
    studyModified = false;
    setCurrentFile(fileName);
    return true;
}

QString CleanedStudy::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

bool CleanedStudy::saveJsonArrayToExcel(const int& Row,const int& Col, const QString& title ,const QJsonArray& jsonArray, const QStringList& columns, QXlsx::Document& xlsx,const QString& tabName="tab1") {

    int row =Row;
    int col =Col;
    QXlsx::Format formatYellowAndCenter;
    formatYellowAndCenter.setPatternBackgroundColor(QColor(Qt::yellow));
    formatYellowAndCenter.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    formatYellowAndCenter.setBorderStyle(QXlsx::Format::BorderThin);

    QXlsx::Format formatGreenAndCenter;
    formatGreenAndCenter.setPatternBackgroundColor(QColor(Qt::green));
    formatGreenAndCenter.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    formatGreenAndCenter.setBorderStyle(QXlsx::Format::BorderThick);

    QXlsx::Format formatThinBorder;
    formatThinBorder.setBorderStyle(QXlsx::Format::BorderThin);


    if (tabName !='-'){
      xlsx.addSheet(tabName);
        }
    xlsx.write(row,col,title);
    row++;
    //col++;

    xlsx.mergeCells("A1:J1", formatYellowAndCenter);
    xlsx.write ("A1", QString("=HYPERLINK(\"%1\",\"%2\")").arg(QCoreApplication::applicationDirPath() + "/ReadMe.pdf").arg("Ctrl-Clik here to open the README"));

    // Write the specified columns as column headers
    for (const QString& column : columns) {
                    xlsx.write(row, col++, column, formatThinBorder);
            }

            QXlsx::CellRange titleRange(Row, Col, Row, col-1);
        xlsx.mergeCells(titleRange, formatGreenAndCenter);
    
    // Write JSON array data to Excel
    for (const QJsonValue& jsonValue : jsonArray) {
        if (!jsonValue.isObject()) {
            qWarning() << "JSON array contains non-object elements.";
            return false;
        }

        QJsonObject jsonObject = jsonValue.toObject();
                    col = Col;
            row++;
        
        for (const QString& column : columns) {
                            xlsx.write(row, col++, jsonObject.value(column).toVariant(), formatThinBorder);
                    }
    }
            QXlsx::CellRange cr(Row, Col, Row + jsonArray.count()-1, col-1);
        xlsx.autosizeColumnWidth(cr);
    
    return true;
}

void CleanedStudy::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        if (_resultDir.size())
            QMessageBox::information(this, "Information", "You can find the saved results in " + _resultDir);
        event->accept();
    } else {
        event->ignore();
    }
}

void CleanedStudy::documentWasModified()
{
    setWindowModified(true);
}

bool CleanedStudy::maybeSave()
{
    if (!studyModified)
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Cleaned"),
                                   tr("'%1' has been modified.\n"
                                      "Do you want to save your changes?")
                                   .arg(userFriendlyCurrentFile()),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void CleanedStudy::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString CleanedStudy::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void CleanedStudy::on_cmd_add_season_clicked()
{
    addSeason newSeason;
    int result = newSeason.exec();
    if (result == 1)
    {
        if (!m_seasons->seasonExist(newSeason.getSeasonName()))
        {
            m_seasons->addNewSeason(newSeason.getSeasonName());
            m_basktet->addNewSeason(newSeason.getSeasonName());
            ui->basketView->resizeColumnsToContents();
            studyModified = true;
        }
    }
}

void CleanedStudy::season_changed(QString fromSeason, QString toSeason)
{
    m_basktet->changeSeasonName(fromSeason, toSeason);
    studyModified = true;
}

void CleanedStudy::on_cmd_del_season_clicked()
{
    if (ui->seasonsView->selectionModel()->selectedRows().count() == 1)
    {
        int row;
        row = ui->seasonsView->selectionModel()->selectedRows()[0].row();
        QString season_name = m_seasons->getSeason(row);
        int ret = QMessageBox::warning(this, tr("Cleaned Seasons"),
                                       "Do you want to remove the season \"" + season_name + "\"",
                                       QMessageBox::Ok | QMessageBox::Cancel,
                                       QMessageBox::Ok);
        if (ret == QMessageBox::Ok)
        {
            m_seasons->deleteSeason(row);
            m_basktet->removeSeason(season_name);
            studyModified = true;
            ui->basketView->resizeColumnsToContents();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You need to select one season");
        msgBox.exec();
    }
}

void CleanedStudy::on_cbm_soiltype_currentIndexChanged(int index)
{
    ui->txt_soil_k_value->setText(QString::number(ui->cbm_soiltype->itemData(index).toDouble()));    
    if (!loading)
        studyModified = true;
}

void CleanedStudy::on_cbm_cropland_system_currentIndexChanged(int index)
{
    ui->txt_cropland_system_ipcc->setText(QString::number(ui->cbm_cropland_system->itemData(index).toDouble()));
    if (!loading)
        studyModified = true;
}

void CleanedStudy::on_cbm_cropland_tillage_currentIndexChanged(int index)
{
    ui->txt_cropland_tillage_ipcc->setText(QString::number(ui->cbm_cropland_tillage->itemData(index).toDouble()));
    if (!loading)
        studyModified = true;
}

void CleanedStudy::on_cbm_cropland_orgmatter_currentIndexChanged(int index)
{
    ui->txt_cropland_orgmatter_ipcc->setText(QString::number(ui->cbm_cropland_orgmatter->itemData(index).toDouble()));
    if (!loading)
        studyModified = true;
}

void CleanedStudy::on_cbm_grassland_management_currentIndexChanged(int index)
{
    ui->txt_grassland_management_ipcc->setText(QString::number(ui->cbm_grassland_management->itemData(index).toDouble()));
    if (!loading)
        studyModified = true;
}

void CleanedStudy::on_cbm_grassland_implevel_currentIndexChanged(int index)
{
    ui->txt_grassland_implevel_ipcc->setText(QString::number(ui->cbm_grassland_implevel->itemData(index).toDouble()));
    if (!loading)
        studyModified = true;
}

void CleanedStudy::on_cmd_add_live_clicked()
{
    addLivestock addNewLivestock;
    addNewLivestock.loadData(this->db);
    int result = addNewLivestock.exec();
    if (result == 1)
    {
        if (addNewLivestock.getLivestockCode() != "")
        {
            if (!m_livestock->livestockExists(addNewLivestock.getLivestockCode()))
            {
                m_livestock->addNewLivestock(addNewLivestock.getLivestockCode(), ui->cbm_region->currentData().toString());
                //ui->livestockView->resizeColumnsToContents();
                ui->livestockView->resizeColumnToContents(0);

                m_basktet->addNewLivestock(addNewLivestock.getLivestockCode());
                ui->basketView->resizeColumnsToContents();
                studyModified = true;
            }
        }
    }
}

void CleanedStudy::on_cmd_add_feed_clicked()
{
    addFeed addNewFeed;
    addNewFeed.loadData(this->db);
    int result = addNewFeed.exec();
    if (result == 1)
    {
        QString crop;
        QString feed;
        crop = addNewFeed.getCropCode();
        feed = addNewFeed.getFeedCode();
        if (crop != "" && feed != "")
        {
            if (!m_feeds->feedExists(crop, feed))
            {
                m_feeds->addNewFeed(crop, feed);

                ui->feedsView->resizeColumnToContents(0);
                ui->feedsView->resizeColumnToContents(1);
                ui->feedsView->resizeColumnToContents(2);
                ui->feedsView->resizeColumnToContents(6);
                ui->feedsView->resizeColumnToContents(8);
                ui->feedsView->resizeColumnToContents(11);
                ui->feedsView->resizeColumnToContents(19);
                ui->feedsView->resizeColumnToContents(20);
                ui->feedsView->resizeColumnToContents(21);


                m_cropInputs->addNewFeed(crop, feed);

                //ui->cropsView->resizeColumnsToContents();
                ui->cropsView->resizeColumnToContents(0);
                ui->cropsView->resizeColumnToContents(1);


                m_basktet->addNewFeed(crop, feed);
                ui->basketView->resizeColumnsToContents();
                studyModified = true;
            }
        }
    }

}


void CleanedStudy::on_cmd_add_fert_clicked()
{
    addFertilizer addNewFertilizer;
    addNewFertilizer.loadData(this->db);
    int result = addNewFertilizer.exec();
    if (result == 1)
    {
        QString fertilizer;
        fertilizer = addNewFertilizer.getFertilizerCode();
        if (fertilizer != "")
        {
            m_fertilizer->addNewFertilizer(fertilizer);
            ui->fertilizerView->resizeColumnsToContents();
        }
    }
}

bool CleanedStudy::run()
{
    if (!studyModified)
    {
        bool run_model = true;
        QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
        if (settings.value("model_file","").toString() == "")
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Run model");
            msgBox.setText("You need to indicate the model file in the settings");
            msgBox.exec();
            run_model = false;
        }
        if (settings.value("ghg_file","").toString() == "")
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Run model");
            msgBox.setText("You need to indicate the GHG parameters file in the settings");
            msgBox.exec();
            run_model = false;
        }
        if (settings.value("stock_file","").toString() == "")
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Run model");
            msgBox.setText("You need to indicate the Stock parameters file in the settings");
            msgBox.exec();
            run_model = false;
        }
        if (settings.value("enery_file","").toString() == "")
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Run model");
            msgBox.setText("You need to indicate the Energy parameters file in the settings");
            msgBox.exec();
            run_model = false;
        }
        QFileInfo info;
        info.setFile(settings.value("model_file","").toString());
        if (!info.exists())
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Run model");
            msgBox.setText("The model file does not exists");
            msgBox.exec();
            run_model = false;
        }
        info.setFile(settings.value("ghg_file","").toString());
        if (!info.exists())
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Run model");
            msgBox.setText("The GHG parameters file does not exists");
            msgBox.exec();
            run_model = false;
        }
        info.setFile(settings.value("stock_file","").toString());
        if (!info.exists())
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Run model");
            msgBox.setText("The Stock parameters file does not exists");
            msgBox.exec();
            run_model = false;
        }
        info.setFile(settings.value("enery_file","").toString());
        if (!info.exists())
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Run model");
            msgBox.setText("The Energy parameters file does not exists");
            msgBox.exec();
            run_model = false;
        }

        if (run_model && !model_running)
        {
            ui->tabWidget_2->insertTab(1, _progressTab, "Progress");
            ui->tabWidget_2->removeTab(2);
            ui->tabWidget_2->setCurrentIndex(1);
            model_running = true;
            ui->stackedWidget->setCurrentIndex(1);
            QStringList params;
            params.append("--vanilla");
            params.append(settings.value("model_file","").toString());
            params.append(curFile);
            params.append(settings.value("ghg_file","").toString());
            params.append(settings.value("stock_file","").toString());
            params.append(settings.value("enery_file","").toString());

            QFileInfo fInfo(curFile);
            _inputBaseName = fInfo.baseName();
            _resultDir = fInfo.absolutePath()+QDir::separator()+fInfo.baseName();
            params.append(_resultDir+QDir::separator()+fInfo.baseName()+"_result.json");
            QDir directory;
            if(!directory.exists(_resultDir)) {
                directory.mkpath(_resultDir);
            }

            qDebug() << "****************444";
            qDebug() << "Rscript " + params.join(" ");
            qDebug() << "****************444";

            QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
            QString r_binay_file = settings.value("rscript_file","").toString();
            if (r_binay_file.simplified() != "" && QFile::exists(r_binay_file))
            {
                QFileInfo fileInfo(settings.value("model_file","").toString());

                QStringList env = QProcess::systemEnvironment();
                QString library_path = fileInfo.absolutePath() + QDir::separator() + "library";
                QDir dir;
                if (!dir.exists(library_path))
                    dir.mkdir(library_path);
                env << "R_LIBS_USER=" + library_path;
                m_process->setEnvironment(env);
                m_process->start(r_binay_file,params);
                ui->cmdcancelrun->setEnabled(true);
                ui->txtruninfo->setPlainText("");
                return true;
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setWindowTitle("Run model");
                msgBox.setText("You need to indicate the R program in the settings");
                msgBox.exec();
                return false;
            }
        }
        else
            return false;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Run model");
        msgBox.setText("You need to save the case study before running the model");
        msgBox.exec();
        return false;
    }
}

void CleanedStudy::on_cmd_remove_live_clicked()
{
    if (ui->livestockView->selectionModel()->selectedRows().count() == 1)
    {
        int row;
        row = ui->livestockView->selectionModel()->selectedRows()[0].row();
        QString livestock_code = m_livestock->getLivestockCode(row);
        QString livestock_desc = m_livestock->getLivestockDesc(row);
        int ret = QMessageBox::warning(this, tr("Cleaned Livestock"),
                                       "Do you want to remove the livestock \"" + livestock_desc + "\"",
                                       QMessageBox::Ok | QMessageBox::Cancel,
                                       QMessageBox::Ok);
        if (ret == QMessageBox::Ok)
        {
            m_livestock->deleteLivestock(row);
            m_basktet->removeLivestock(livestock_code);
            studyModified = true;
            ui->basketView->resizeColumnsToContents();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Remove livestock");
        msgBox.setText("You need to select one livestock");
        msgBox.exec();
    }
}

void CleanedStudy::on_cmd_remove_feed_clicked()
{
    if (ui->feedsView->selectionModel()->selectedRows().count() == 1)
    {
        int row;
        row = ui->feedsView->selectionModel()->selectedRows()[0].row();
        QString crop_code = m_feeds->getCropCode(row);
        QString feed_code = m_feeds->getFeedCode(row);
        QString feed_desc = m_feeds->getFeedDesc(row);
        int ret = QMessageBox::warning(this, tr("Cleaned Feeds"),
                                       "Do you want to remove the feed \"" + feed_desc + "\"",
                                       QMessageBox::Ok | QMessageBox::Cancel,
                                       QMessageBox::Ok);
        if (ret == QMessageBox::Ok)
        {
            m_feeds->deleteFeed(row);
            m_cropInputs->deleteFeed(row);
            m_basktet->removeFeed(crop_code, feed_code);
            studyModified = true;
            ui->basketView->resizeColumnsToContents();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Remove feed");
        msgBox.setText("You need to select one feed");
        msgBox.exec();
    }
}

void CleanedStudy::on_cmd_remove_fert_clicked()
{
    if (ui->fertilizerView->selectionModel()->selectedRows().count() == 1)
    {
        int row;
        row = ui->fertilizerView->selectionModel()->selectedRows()[0].row();
        QString fertilizer_desc = m_fertilizer->getFertilizer(row);
        int ret = QMessageBox::warning(this, tr("Cleaned Fetilizer"),
                                       "Do you want to remove the fertilizer \"" + fertilizer_desc + "\"",
                                       QMessageBox::Ok | QMessageBox::Cancel,
                                       QMessageBox::Ok);
        if (ret == QMessageBox::Ok)
        {
            m_fertilizer->deleteFertilizer(row);
            studyModified = true;
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Remove fertilizer");
        msgBox.setText("You need to select one fertilizer");
        msgBox.exec();
    }
}

void CleanedStudy::modelChanged()
{
    studyModified = true;
}

void CleanedStudy::textEdited(QString )
{
    if (!loading)
        studyModified = true;
}

void CleanedStudy::BrowseChildren(QWidget * parent)
{
    // Browse through all objects and connect the Signal textEdited of each LineEdit so we can track changes
    QLineEdit *ln;
    ln = qobject_cast<QLineEdit *>(parent);
    if (ln)
        connect(ln,SIGNAL(textEdited(QString)),this,SLOT(textEdited(QString)));

    QObjectList children = parent->children();
    QObjectList::const_iterator it = children.begin();
    QObjectList::const_iterator eIt = children.end();
    while ( it != eIt )
    {
        QWidget * pChild = (QWidget *)(*it++);
        BrowseChildren( pChild );
    }
}

void CleanedStudy::on_cbm_climate_currentIndexChanged(const QString &arg1)
{
    ui->cbm_subclimate->clear();
    QSqlQuery query(this->db);
    if (query.exec("select climate2_code from lkp_climate2 WHERE climate_code = '" + arg1 + "'"))
    {
        while(query.next())
        {
            ui->cbm_subclimate->addItem(query.value(0).toString(), QVariant(query.value(0).toString()));
        }
    }
    else
        emit report_error("Unable to query sub-climate");
    if (ui->cbm_subclimate->count() > 0)
        ui->cbm_subclimate->setCurrentIndex(0);
}

void CleanedStudy::modelFinished(int exitCode)
{
    if (exitCode == 0)
    {
        ui->stackedWidget->setCurrentIndex(2);

        QDir directory(_resultDir);
        QStringList filters;
        filters << "*.png";

        auto pngsFilenames = directory.entryList(filters);

        for(auto& pngFilename : pngsFilenames) {
            _pngs.push_back(QSharedPointer<QPixmap>::create(_resultDir+QDir::separator()+pngFilename));
            pngFilename.replace(_inputBaseName+"_result_", "");
            pngFilename.replace(".png", "");
        }
        ui->pngList->clear();
        ui->pngList->addItems(pngsFilenames);

        ui->tabWidget_2->removeTab(1);
        ui->tabWidget_2->insertTab(1, _resultTab, "Results");
        ui->tabWidget_2->setCurrentIndex(1);
        ui->tabWidget->setCurrentWidget(_resultTab);
        ui->pngList->setCurrentRow(0);

        QFileInfo fInfo(curFile);
        QString outFile = fInfo.absolutePath()+QDir::separator()+fInfo.baseName() + QDir::separator()+fInfo.baseName()+"_result.json";
        QFile file(outFile);
        file.open(QFile::ReadOnly | QFile::Text);
        auto result = file.readAll();
        exportResultToExcel(result, outFile.replace("_result.json","_result.xls"));
        ui->json_result->setText(result);
        ui->json_result->setReadOnly(true);
        model_running = false;
        emit runFinished(exitCode, this->curFile, outFile);
        ui->cmdcancelrun->setEnabled(false);
    }
    else
    {
        model_running = false;
        emit runFinished(exitCode, this->curFile, "");
        ui->cmdcancelrun->setEnabled(false);
    }
}

void CleanedStudy::exportResultToExcel(const QString& result, const QString& filename) {

    QXlsx::Document xlsx;
    QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8());
    QJsonObject jso = doc.object();


    QJsonValue value1 = jso.value(QString("land_required"));
    QJsonArray qjsArr1 = value1["TA1"].toArray();
    int TA1count=qjsArr1.count();
    QStringList columns = {"feed", "area_total", "area_non_feed", "area_feed", "rough_of", "conc_of", "conc_ip", "farm", "grasses", "tree_legume"};
saveJsonArrayToExcel(3,1,"season_1",qjsArr1,columns,xlsx,"land_required_Season");
QJsonArray qjsArr2 = value1["TA2"].toArray();
//columns = {"feed", "area_total", "area_non_feed", "area_feed", "rough_of", "conc_of", "conc_ip", "farm", "grasses", "tree_legume"};
columns = {"area_total", "area_non_feed", "area_feed", "rough_of", "conc_of", "conc_ip", "farm", "grasses", "tree_legume"};
saveJsonArrayToExcel(3,11,"season_2",qjsArr2,columns,xlsx,"-");

value1 = jso.value(QString("energy_required"));
qjsArr1 = value1["annual_results"].toArray();
int annual_resultscount=qjsArr1.count();
columns = {"energy_required_annually", "protein_required_annually", "dmi_energy_total", "dmi_cp_total", "limiting_total", "surplus_total", "me_intake", "dmi_tot", "de_intake", "ge_intake"};
saveJsonArrayToExcel(7+TA1count,1,"Annual_nutrient _& _DMI_requirements",qjsArr1,columns,xlsx,"-");

value1 = jso.value(QString("energy_required"));
qjsArr1 = value1["seasonal_results"].toArray();
columns = {"energy_required_by_season", "protein_required_by_season", "fresh_intake_required_e", "dmi_required_e", "fresh_intake_required_cp", "dmi_required_cp", "dmi_s", "limiting", "surplus_season", "me_intake_s"};
saveJsonArrayToExcel(10+annual_resultscount+TA1count,1,"Seasonal_nutrient_&_DMI_requirements",qjsArr1,columns,xlsx,"-");
//////////////////////////////////////////
value1 = jso.value(QString("feed_basket_quality"));
qjsArr1 = value1["TA1"].toArray();
TA1count=qjsArr1.count();
columns = {"livestock_category_name", "Barley (grains) OFC", "Berseem clover", "Lucerne (Medicago sativa) - forage", "Naturally occuring pasture - grazing", "Oats (Avena sativa) - hay OFR", "Sorghum (Sorghum bicolor) - forage", "Soybean (Glycine max) - whole seed IP"};
saveJsonArrayToExcel(2,1,"season_1",qjsArr1,columns,xlsx,"feed_basket_quality");
qjsArr2 = value1["TA2"].toArray();
//columns = {"livestock_category_name", "Barley (grains) OFC", "Berseem clover", "Lucerne (Medicago sativa) - forage", "Naturally occuring pasture - grazing", "Oats (Avena sativa) - hay OFR", "Sorghum (Sorghum bicolor) - forage", "Soybean (Glycine max) - whole seed IP"};
columns = {"Barley (grains) OFC", "Berseem clover", "Lucerne (Medicago sativa) - forage", "Naturally occuring pasture - grazing", "Oats (Avena sativa) - hay OFR", "Sorghum (Sorghum bicolor) - forage", "Soybean (Glycine max) - whole seed IP"};
saveJsonArrayToExcel(2,7,"season_2",qjsArr2,columns,xlsx,"-");
//////////////////////////////////////////
value1 = jso.value(QString("livestock_productivity"));
qjsArr1 = value1.toArray();
columns = {"livetype_name","tlu","total_milk","meat_production_animal","energy_kcal_year_meat","energy_kcal_year_milk","protein_kg_year_meat","protein_kg_year_milk","annual_manure_produced","daily_manure_produced"};
saveJsonArrayToExcel(2,1,"",qjsArr1,columns,xlsx,"livestock_productivity");
//////////////////////////////////////////
//Soil health


//////////////////////////////////////////
value1 = jso.value(QString("water_required"));
qjsArr1 = value1["water_use_for_production"].toArray();;
TA1count=qjsArr1.count();
columns = {"Names","tlu","Value"};
saveJsonArrayToExcel(2,2,"water_use_for_production",qjsArr1,columns,xlsx,"Water use");

qjsArr2 = value1["water_use_per_feed_item"].toArray();
//columns = {"livestock_category_name", "Barley (grains) OFC", "Berseem clover", "Lucerne (Medicago sativa) - forage", "Naturally occuring pasture - grazing", "Oats (Avena sativa) - hay OFR", "Sorghum (Sorghum bicolor) - forage", "Soybean (Glycine max) - whole seed IP"};
columns = {"feed", "feed_water_use", "non_feed_water_use", "kc_water_use_of_roughages", "kc_water_use_of_concentrates", "kc_water_use_ip_concentrates", "kc_water_use_on_farm", "kc_water_use_m3_per_kg"};
saveJsonArrayToExcel(6+TA1count,2,"water_use_per_feed_item",qjsArr2,columns,xlsx,"-");

/////////////////////////////////////////

value1 = jso.value(QString("biomass"));
qjsArr1 = value1["trees_non_feed_biomass"].toArray();;
TA1count=qjsArr1.count();
columns = {"feed_item_name","c_increase","co2_increase","c_increase_soc","co2_increase_soc","total_c_increase","total_co2_increase"};
saveJsonArrayToExcel(2,2,"trees_non_feed_biomass",qjsArr1,columns,xlsx,"Carbon sequestration");

value1 = jso.value(QString("soil_carbon"));
qjsArr1 = value1.toArray();
columns = {"total_annual_change_carbon_soils", "total_change_co2_soils"};
saveJsonArrayToExcel(5+TA1count,2,"soil_carbon",qjsArr1,columns,xlsx,"-");

/////////////////////////////////////////
value1 = jso.value(QString("ghg_emission"));
qjsArr1 = value1.toArray();
columns = {"GHG_balance", "kg_per_ha"};
saveJsonArrayToExcel(2,2,"GHG emissions",qjsArr1,columns,xlsx,"GHG emissions");
xlsx.selectSheet(0);
    xlsx.saveAs(filename);
}

void CleanedStudy::on_livestockView_doubleClicked(const QModelIndex &index)
{
    int col = index.column();
    if (col == 7 || col == 10 || col == 13 || col == 15)
    {
        delegatorDialog manure;
        manure.setWindowTitle("Manure management");
        manure.set_caption("Select a manure management type");
        manure.load_manure(this->db);
        manure.exec();
        if (manure.selected_code != "")
        {
            this->m_livestock->setData(index, manure.selected_code);
            int maxWidth = 0;
            for (int nrow = 0; nrow < m_livestock->rowCount(); nrow++)
            {
                QModelIndex index = m_livestock->index(nrow, col);
                int width = ui->livestockView->fontMetrics().horizontalAdvance(m_livestock->data(index).toString());
                width = width + 10;
                if (width > maxWidth)
                {
                    maxWidth = width;
                }
            }
            if (maxWidth > livestock_colums[col])
                livestock_colums[col] = maxWidth;
            else
            {
                if (maxWidth > 100)
                    livestock_colums[col] = maxWidth;
                else
                    livestock_colums[col] = 100;
            }
            ui->livestockView->setColumnWidth(col, livestock_colums[col]);
        }
    }
}


void CleanedStudy::on_feedsView_doubleClicked(const QModelIndex &index)
{
    int col = index.column();

    auto code = m_feeds->getCropCode(index.row());
    auto cropDesc = m_feeds->getCropDesc(index.row());
    if (col == 2 || col == 6 || col == 8 || col == 11 || (( cropDesc == "Rice") && (col == 19|| col == 20 || col == 21 )))
    {
        delegatorDialog selectDialog;
        if (col == 2)
        {
            selectDialog.load_sourceType();
            selectDialog.set_caption("Select a source type");
        }
        if (col == 6)
        {
            selectDialog.load_landcover(db);
            selectDialog.set_caption("Select a land cover");
        }
        if (col == 8)
        {
            selectDialog.load_slope(db);
            selectDialog.set_caption("Select a slope type");
        }
        if (col == 11)
        {
            selectDialog.load_grassmanagement(db);
            selectDialog.set_caption("Select a grassland management type");
        }
        if (col == 19)
        {
            selectDialog.load_waterregime();
            selectDialog.set_caption("Select a waterregime type");
        }
        if (col == 20)
        {
            selectDialog.load_reice_ecosystem_type();
            selectDialog.set_caption("Select a reice ecosystem type");
        }
        if (col == 21)
        {
            selectDialog.load_organic_amendment();
            selectDialog.set_caption("Select a reice organic_amendment");
        }
        selectDialog.exec();
        if (selectDialog.selected_code != "")
        {
            this->m_feeds->setData(index, selectDialog.selected_code);

            int maxWidth = 0;
            for (int nrow = 0; nrow < m_feeds->rowCount(); nrow++)
            {
                QModelIndex index = m_feeds->index(nrow, col);
                int width = ui->feedsView->fontMetrics().horizontalAdvance(m_feeds->data(index).toString());
                width = width + 10;
                if (width > maxWidth)
                {
                    maxWidth = width;
                }
            }
            if (maxWidth > feed_colums[col])
                feed_colums[col] = maxWidth;
            else
            {
                if (maxWidth > 100)
                    feed_colums[col] = maxWidth;
                else
                    feed_colums[col] = 100;
            }
            ui->feedsView->setColumnWidth(col, feed_colums[col]);
        }
    }
}


void CleanedStudy::on_tabWidget_currentChanged(int index)
{
    if (index == 3)
    {
        if (m_seasons->rowCount() >0 )
            ui->lblseasons->setVisible(false);
        if (m_livestock->rowCount() >0 )
            ui->lbllivestock->setVisible(false);
        if (m_feeds->rowCount() >0 )
            ui->lblfeeds->setVisible(false);
    }
}

void CleanedStudy::readyReadStandardError()
{
    QString current_data = ui->txtruninfo->toPlainText();
    current_data = current_data + "\n" + m_process->readAllStandardError();
    ui->txtruninfo->setPlainText(current_data);
    ui->txtruninfo->verticalScrollBar()->setValue(ui->txtruninfo->verticalScrollBar()->maximum());
}
void CleanedStudy::readyReadStandardOutput()
{
    QString current_data = ui->txtruninfo->toPlainText();
    current_data = current_data + "\n" + m_process->readAllStandardOutput();
    ui->txtruninfo->setPlainText(current_data);
    ui->txtruninfo->verticalScrollBar()->setValue(ui->txtruninfo->verticalScrollBar()->maximum());
}

void CleanedStudy::on_cmdcancelrun_clicked()
{
    m_process->kill();
}

void CleanedStudy::onImageSelected(QListWidgetItem *current, QListWidgetItem *previous) {
    if(current) {
        auto &png= _pngs[ui->pngList->currentRow()];
        ui->pngLabel->setPixmap(QPixmap());
        ui->pngLabel->setPixmap(png->scaled(QSize(ui->pngLabel->size().width()-2, ui->pngLabel->size().height()-2), Qt::KeepAspectRatio));
    }
}


void CleanedStudy::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if(ui->pngList->currentItem()) {
        auto &png= _pngs[ui->pngList->currentRow()];
        ui->pngLabel->setPixmap(QPixmap());
        ui->pngLabel->setPixmap(png->scaled(QSize(ui->pngLabel->size().width()-2, ui->pngLabel->size().height()-2), Qt::KeepAspectRatio));
    }
}
