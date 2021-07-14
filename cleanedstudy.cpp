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

CleanedStudy::CleanedStudy(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CleanedStudy)
{
    ui->setupUi(this);
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
    m_seasons = new seasonModel(this);
    connect(m_seasons,SIGNAL(seasonChanged(QString, QString)),this,SLOT(season_changed(QString, QString)));
    ui->seasonsView->setModel(m_seasons);
    m_livestock = new livestockModel(this);
    ui->livestockView->setModel(m_livestock);

    m_feeds = new feedsModel(this);
    ui->feedsView->setModel(m_feeds);

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
    delete ui;
}

void CleanedStudy::setDatabase(QString databaseFile)
{
    QUuid connectionUUID=QUuid::createUuid();
    QString strconnectionUUID=connectionUUID.toString().replace("{","").replace("}","");

    this->db = QSqlDatabase::addDatabase("QSQLITE",strconnectionUUID);
    this->db.setDatabaseName(databaseFile);
    if (!this->db.open())
    {
        emit report_error("Unable to open the Cleaned Database");
    }
    loading = true;
    load_models();
    loading = false;
}

void CleanedStudy::newFile()
{
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

    //ui->feedsView->resizeColumnsToContents();
    ui->feedsView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    ui->feedsView->horizontalHeader()->setMinimumHeight(120);

    //ui->cropsView->resizeColumnsToContents();
    ui->cropsView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    ui->cropsView->horizontalHeader()->setMinimumHeight(120);

    //TODO: Notify that is a new file
//    connect(document(), &QTextDocument::contentsChanged,
//            this, &MdiChild::documentWasModified);
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
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QByteArray studyData = file.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(studyData));
    this->study_object = loadDoc.object();
    loading = true;
    loadStudyObject();
    loading = false;
    QGuiApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

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
    ui->txt_land_oppcost->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_cba_discount_rate->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->txt_cba_years->setValidator(new QDoubleValidator(0, 100, 2, this));

    //Livestock
    m_livestock->setDatabase(db);
    connect(m_livestock,SIGNAL(delegateChanged(int)),this,SLOT(livestock_delegate_changed(int)));

    QBrush manureManagementBackground(QColor(169,209,142));
    comboDelegate *manureStableDelegate = new comboDelegate(this);
    manureStableDelegate->setBrush(manureManagementBackground);

    comboDelegate *manureEnclosureDelegate = new comboDelegate(this);
    manureEnclosureDelegate->setBrush(manureManagementBackground);

    comboDelegate *manureOnFarmGrazingDelegate = new comboDelegate(this);
    manureOnFarmGrazingDelegate->setBrush(manureManagementBackground);

    comboDelegate *manureOffFarmGrazingDelegate = new comboDelegate(this);
    manureOffFarmGrazingDelegate->setBrush(manureManagementBackground);

    if (query.exec("select manureman_desc from lkp_manureman"))
    {
        while (query.next())
        {
            manureStableDelegate->insertItem(query.value(0).toString(),query.value(0).toString());
            manureEnclosureDelegate->insertItem(query.value(0).toString(),query.value(0).toString());
            manureOnFarmGrazingDelegate->insertItem(query.value(0).toString(),query.value(0).toString());
            manureOffFarmGrazingDelegate->insertItem(query.value(0).toString(),query.value(0).toString());
        }
    }
    ui->livestockView->setItemDelegateForColumn(7,manureStableDelegate);
    ui->livestockView->setItemDelegateForColumn(10,manureEnclosureDelegate);
    ui->livestockView->setItemDelegateForColumn(13,manureOnFarmGrazingDelegate);
    ui->livestockView->setItemDelegateForColumn(15,manureOffFarmGrazingDelegate);

    //Feeds
    m_feeds->setDatabase(db);
    connect(m_feeds,SIGNAL(delegateChanged(int)),this,SLOT(feeds_delegate_changed(int)));

    comboDelegate *landCoverDelegate = new comboDelegate(this);
    if (query.exec("SELECT landcover_code,landcover_desc FROM lkp_landcover"))
    {
        while (query.next())
        {
            landCoverDelegate->insertItem(query.value(0).toString(),query.value(1).toString());
        }
    }
    QBrush Background(QColor(169,209,142)); //We can change this to a nice color
    landCoverDelegate->setBrush(Background);
    ui->feedsView->setItemDelegateForColumn(6,landCoverDelegate);

    comboDelegate *slopeDelegate = new comboDelegate(this);
    if (query.exec("SELECT slope_code,slope_desc FROM lkp_slope"))
    {
        while (query.next())
        {
            slopeDelegate->insertItem(query.value(0).toString(),query.value(1).toString());
        }
    }
    slopeDelegate->setBrush(Background);
    ui->feedsView->setItemDelegateForColumn(8,slopeDelegate);

    comboDelegate *grassDelegate = new comboDelegate(this);
    if (query.exec("SELECT management_code,management_desc FROM lkp_grasslandman"))
    {
        while (query.next())
        {
            grassDelegate->insertItem(query.value(0).toString(),query.value(1).toString());
        }
    }
    grassDelegate->setBrush(Background);
    ui->feedsView->setItemDelegateForColumn(11,grassDelegate);

    comboDelegate *sourceTypeDelegate = new comboDelegate(this);
    sourceTypeDelegate->insertItem("Main","Main");
    sourceTypeDelegate->insertItem("Residue","Residue");
    sourceTypeDelegate->insertItem("Purchased","Purchased");
    sourceTypeDelegate->setBrush(Background);
    ui->feedsView->setItemDelegateForColumn(2,sourceTypeDelegate);


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
    this->study_object["land_oppcost"] = ui->txt_land_oppcost->text().toDouble();
    this->study_object["cba_discount_rate"] = ui->txt_cba_discount_rate->text().toDouble();
    this->study_object["cba_years"] = ui->txt_cba_years->text().toDouble();
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
    if (study_object.contains("land_oppcost"))
        ui->txt_land_oppcost->setText(QString::number(study_object["land_oppcost"].toDouble()));
    if (study_object.contains("cba_discount_rate"))
        ui->txt_cba_discount_rate->setText(QString::number(study_object["cba_discount_rate"].toDouble()));
    if (study_object.contains("cba_years"))
        ui->txt_cba_years->setText(QString::number(study_object["cba_years"].toDouble()));


    // Load livestock
    if (study_object.contains("livestock") && study_object["livestock"].isArray())
    {
        QJsonArray livestockArray;
        livestockArray = study_object["livestock"].toArray();
        m_livestock->setDataArray(livestockArray);
    }
    //ui->livestockView->resizeColumnsToContents();
    ui->livestockView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    ui->livestockView->horizontalHeader()->setMinimumHeight(150);

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
        return saveFile(curFile);
    }
}

bool CleanedStudy::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

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

void CleanedStudy::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
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

void CleanedStudy::feeds_delegate_changed(int column)
{
    ui->feedsView->resizeColumnToContents(column);
}

void CleanedStudy::livestock_delegate_changed(int column)
{
    ui->livestockView->resizeColumnToContents(column);
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
        QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
        if (settings.value("model_file","").toString() != "")
            return true;
        else
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Run model");
            msgBox.setText("You need to indicate the model file in the settings");
            msgBox.exec();
            return false;
        }
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
