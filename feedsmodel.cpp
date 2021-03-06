#include "feedsmodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QBrush>

feedsModel::feedsModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void feedsModel::setDataArray(QJsonArray data)
{
    this->beginResetModel();
    items.clear();
    for (int seasonIndex = 0; seasonIndex < data.size(); ++seasonIndex)
    {
         QJsonObject feedObject = data[seasonIndex].toObject();
         Tfeed aFeed;
         aFeed.feed_type_code = feedObject["feed_type_code"].toString();
         aFeed.feed_type_name = feedObject["feed_type_name"].toString();
         aFeed.feed_item_code = feedObject["feed_item_code"].toString();
         aFeed.feed_item_name = feedObject["feed_item_name"].toString();
         aFeed.intercrop = feedObject["intercrop"].toInt(0);
         aFeed.intercrop_fraction = feedObject["intercrop_fraction"].toDouble(0);
         aFeed.cut_carry_fraction = feedObject["cut_carry_fraction"].toDouble(0);
         aFeed.land_cover = feedObject["land_cover"].toString();
         aFeed.landcover_c_factor = feedObject["landcover_c_factor"].toDouble(0);
         aFeed.slope = feedObject["slope"].toString();
         aFeed.slope_p_factor = feedObject["slope_p_factor"].toDouble(0);
         aFeed.slope_length = feedObject["slope_length"].toDouble(0);
         aFeed.grassman = feedObject["grassman"].toString();
         aFeed.grassman_change_factor = feedObject["grassman_change_factor"].toDouble(0);
         aFeed.main_product_removal = feedObject["main_product_removal"].toDouble(0);
         aFeed.residue_removal = feedObject["residue_removal"].toDouble(0);
         aFeed.residue_burnt = feedObject["residue_burnt"].toDouble(0);
         aFeed.dm_content = feedObject["dm_content"].toDouble(0);
         aFeed.me_content = feedObject["me_content"].toDouble(0);
         aFeed.me_fresh = feedObject["me_fresh"].toDouble(0);
         aFeed.cp_content = feedObject["cp_content"].toDouble(0);
         aFeed.cp_fresh = feedObject["cp_fresh"].toDouble(0);
         aFeed.de = feedObject["de"].toDouble(0);
         aFeed.establishment_cost = feedObject["establishment_cost"].toDouble(0);
         aFeed.operational_cost = feedObject["operational_cost"].toDouble(0);
         aFeed.establishment_labour = feedObject["establishment_labour"].toDouble(0);
         aFeed.operational_labour = feedObject["operational_labour"].toDouble(0);
         aFeed.wfp_green = feedObject["wfp_green"].toDouble(0);
         aFeed.wfp_blue = feedObject["wfp_blue"].toDouble(0);
         aFeed.wfp_grey = feedObject["wfp_grey"].toDouble(0);
         aFeed.emission_factor = feedObject["emission_factor"].toDouble(0);
         aFeed.fresh_yield = feedObject["fresh_yield"].toDouble(0);
         aFeed.dm_content = feedObject["dm_content"].toDouble(0);
         aFeed.dry_yield = feedObject["dry_yield"].toDouble(0);
         aFeed.harvest_index = feedObject["harvest_index"].toDouble(0);
         aFeed.residue_fresh_yield = feedObject["residue_fresh_yield"].toDouble(0);
         aFeed.residue_dm_content = feedObject["residue_dm_content"].toDouble(0);
         aFeed.residue_dry_yield = feedObject["residue_dry_yield"].toDouble(0);
         aFeed.residue_n = feedObject["residue_n"].toDouble(0);
         aFeed.main_n = feedObject["main_n"].toDouble(0);
         aFeed.residue_n_dm = feedObject["residue_n_dm"].toDouble(0);
         aFeed.c_factor = feedObject["c_factor"].toDouble(0);
         aFeed.n_fixation = feedObject["n_fixation"].toDouble(0);
         aFeed.energy = feedObject["energy"].toDouble(0);
         aFeed.water_content = feedObject["water_content"].toDouble(0);
         aFeed.energy_dm = feedObject["energy_dm"].toDouble(0);
         aFeed.usda_value = feedObject["usda_value"].toDouble(0);
         aFeed.kc_initial = feedObject["kc_initial"].toDouble(0);
         aFeed.kc_midseason = feedObject["kc_midseason"].toDouble(0);
         aFeed.kc_late = feedObject["kc_late"].toDouble(0);
         aFeed.crop_coefficient = feedObject["crop_coefficient"].toDouble(0);
         aFeed.category = feedObject["category"].toString();
         aFeed.trees_ha = feedObject["trees_ha"].toDouble(0);
         aFeed.trees_dhb = feedObject["trees_dhb"].toDouble(0);
         aFeed.trees_growth = feedObject["trees_growth"].toDouble(0);
         aFeed.trees_removal = feedObject["trees_removal"].toDouble(0);
         items.append(aFeed);
    }
    this->endResetModel();
}

void feedsModel::setDatabase(QSqlDatabase cleaned_db)
{
    this->db = cleaned_db;
}

int feedsModel::columnCount(const QModelIndex &) const
{
    return 52;
}

int feedsModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

bool feedsModel::feedExists(QString crop, QString feed)
{
    for (int i = 0; i < items.count(); i++)
    {
        if ((items[i].feed_type_code == crop) && (items[i].feed_item_code == feed))
        {
            return true;
        }
    }
    return false;
}

Qt::ItemFlags feedsModel::flags(const QModelIndex &index) const
{
    if (index.column() > 1)
    {
        if (index.column() != 3)
        {
            if (index.column() != 2)
                return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
            else
                return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
        }
        else
        {
            if (items[index.row()].intercrop == 1)
                return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
            else
                return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
        }
    }
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QJsonArray feedsModel::getFeedsArray()
{
    QJsonArray feedsArray;
    for (int i = 0; i < items.count(); i++)
    {
        QJsonObject feedObject;
        feedObject["feed_type_code"] = items[i].feed_type_code;
        feedObject["feed_type_name"] = items[i].feed_type_name;
        feedObject["feed_item_code"] = items[i].feed_item_code;
        feedObject["feed_item_name"] = items[i].feed_item_name;
        feedObject["intercrop"] = items[i].intercrop;
        feedObject["intercrop_fraction"] = items[i].intercrop_fraction;
        feedObject["cut_carry_fraction"] = items[i].cut_carry_fraction;
        feedObject["land_cover"] = items[i].land_cover;
        feedObject["landcover_c_factor"] = items[i].landcover_c_factor;
        feedObject["slope"] = items[i].slope;
        feedObject["slope_p_factor"] = items[i].slope_p_factor;
        feedObject["slope_length"] = items[i].slope_length;
        feedObject["grassman"] = items[i].grassman;
        feedObject["grassman_change_factor"] = items[i].grassman_change_factor;
        feedObject["main_product_removal"] = items[i].main_product_removal;
        feedObject["residue_removal"] = items[i].residue_removal;
        feedObject["residue_burnt"] = items[i].residue_burnt;
        feedObject["dm_content"] = items[i].dm_content;
        feedObject["me_content"] = items[i].me_content;
        feedObject["me_fresh"] = items[i].me_fresh;
        feedObject["cp_content"] = items[i].cp_content;
        feedObject["cp_fresh"] = items[i].cp_fresh;
        feedObject["de"] = items[i].de;
        feedObject["establishment_cost"] = items[i].establishment_cost;
        feedObject["operational_cost"] = items[i].operational_cost;
        feedObject["establishment_labour"] = items[i].establishment_labour;
        feedObject["operational_labour"] = items[i].operational_labour;
        feedObject["wfp_green"] = items[i].wfp_green;
        feedObject["wfp_blue"] = items[i].wfp_blue;
        feedObject["wfp_grey"] = items[i].wfp_grey;
        feedObject["emission_factor"] = items[i].emission_factor;
        feedObject["fresh_yield"] = items[i].fresh_yield;
        feedObject["dm_content"] = items[i].dm_content;
        feedObject["dry_yield"] = items[i].dry_yield;
        feedObject["harvest_index"] = items[i].harvest_index;
        feedObject["residue_fresh_yield"] = items[i].residue_fresh_yield;
        feedObject["residue_dm_content"] = items[i].residue_dm_content;
        feedObject["residue_dry_yield"] = items[i].residue_dry_yield;
        feedObject["residue_n"] = items[i].residue_n;
        feedObject["main_n"] = items[i].main_n;
        feedObject["residue_n_dm"] = items[i].residue_n_dm;
        feedObject["c_factor"] = items[i].c_factor;
        feedObject["n_fixation"] = items[i].n_fixation;
        feedObject["energy"] = items[i].energy;
        feedObject["water_content"] = items[i].water_content;
        feedObject["energy_dm"] = items[i].energy_dm;
        feedObject["usda_value"] = items[i].usda_value;
        feedObject["kc_initial"] = items[i].kc_initial;
        feedObject["kc_midseason"] = items[i].kc_midseason;
        feedObject["kc_late"] = items[i].kc_late;
        feedObject["crop_coefficient"] = items[i].crop_coefficient;
        feedObject["category"] = items[i].category;
        feedObject["trees_ha"] = items[i].trees_ha;
        feedObject["trees_dhb"] = items[i].trees_dhb;
        feedObject["trees_growth"] = items[i].trees_growth;
        feedObject["trees_removal"] = items[i].trees_removal;
        feedsArray.append(feedObject);
    }
    return feedsArray;
}

void feedsModel::addNewFeed(QString crop, QString feed)
{
    if (feedExists(crop, feed))
        return;
    QSqlQuery query(this->db);
    Tfeed aFeed;
    aFeed.feed_type_code = crop;
    aFeed.feed_item_code = feed;
    aFeed.intercrop = 0;
    aFeed.intercrop_fraction = 0;
    aFeed.cut_carry_fraction = 0;
    aFeed.landcover_c_factor = 0;
    if (query.exec("SELECT landcover_code,c_factor FROM lkp_landcover LIMIT 1"))
    {
        if (query.first())
        {
            aFeed.land_cover = query.value(0).toString();
            aFeed.landcover_c_factor = query.value(1).toDouble(0);
        }
    }
    aFeed.slope_p_factor = 0;
    if (query.exec("SELECT slope_code,p_factor FROM lkp_slope LIMIT 1"))
    {
        if (query.first())
        {
            aFeed.slope = query.value(0).toString();
            aFeed.slope_p_factor = query.value(1).toDouble(0);
        }
    }
    aFeed.slope_length = 0;
    aFeed.grassman_change_factor = 0;
    if (query.exec("SELECT management_code,change_factor FROM lkp_grasslandman"))
    {
        if (query.first())
        {
            aFeed.grassman = query.value(0).toString();
            aFeed.grassman_change_factor = query.value(1).toDouble(0);
        }
    }
    aFeed.main_product_removal = 0;
    aFeed.residue_removal = 0;
    aFeed.residue_burnt = 0;
    if (query.exec("SELECT * FROM lkp_feeditem WHERE feed_type_code = '" + crop + "' AND feed_item_code = '" + feed + "'"))
    {
        if (query.first())
        {
            aFeed.feed_item_name = query.value("feed_item_name").toString();
            aFeed.dm_content = query.value("dm_content").toDouble(0);
            aFeed.me_content = query.value("me_content").toDouble(0);
            aFeed.me_fresh = query.value("me_fresh").toDouble(0);
            aFeed.cp_content = query.value("cp_content").toDouble(0);
            aFeed.cp_fresh = query.value("cp_fresh").toDouble(0);
            aFeed.de = query.value("de").toDouble(0);
            aFeed.establishment_cost = query.value("establishment_cost").toDouble(0);
            aFeed.operational_cost = query.value("operational_cost").toDouble(0);
            aFeed.establishment_labour = query.value("establishment_labour").toDouble(0);
            aFeed.operational_labour = query.value("operational_labour").toDouble(0);
            aFeed.wfp_green = query.value("wfp_green").toDouble(0);
            aFeed.wfp_blue = query.value("wfp_blue").toDouble(0);
            aFeed.wfp_grey = query.value("wfp_grey").toDouble(0);
            aFeed.emission_factor = query.value("emission_factor").toDouble(0);
        }
    }
    if (query.exec("SELECT * FROM lkp_feedtype WHERE feed_type_code = '" + crop + "'"))
    {
        if (query.first())
        {
            aFeed.feed_type_name = query.value("feed_type_name").toString();
            aFeed.fresh_yield = query.value("fresh_yield").toDouble(0);
            aFeed.dm_fraction = query.value("dm_fraction").toDouble(0);
            aFeed.dry_yield = query.value("dry_yield").toDouble(0);
            aFeed.harvest_index = query.value("harvest_index").toDouble(0);
            aFeed.residue_fresh_yield = query.value("residue_fresh_yield").toDouble(0);
            aFeed.residue_dm_content = query.value("residue_dm_content").toDouble(0);
            aFeed.residue_dry_yield = query.value("residue_dry_yield").toDouble(0);
            aFeed.residue_n = query.value("residue_n").toDouble(0);
            aFeed.main_n = query.value("main_n").toDouble(0);
            aFeed.residue_n_dm = query.value("residue_n_dm").toDouble(0);
            aFeed.c_factor = query.value("c_factor").toDouble(0);
            aFeed.n_fixation = query.value("n_fixation").toDouble(0);
            aFeed.energy = query.value("energy").toDouble(0);
            aFeed.water_content = query.value("water_content").toDouble(0);
            aFeed.energy_dm = query.value("energy_dm").toDouble(0);
            aFeed.usda_value = query.value("usda_value").toDouble(0);
            aFeed.kc_initial = query.value("kc_initial").toDouble(0);
            aFeed.kc_midseason = query.value("kc_midseason").toDouble(0);
            aFeed.kc_late = query.value("kc_late").toDouble(0);
            aFeed.crop_coefficient = query.value("crop_coefficient").toDouble(0);
            aFeed.category = query.value("category").toString();
            aFeed.trees_ha = query.value("trees_ha").toDouble(0);
            aFeed.trees_dhb = query.value("trees_dhb").toDouble(0);
            aFeed.trees_growth = query.value("trees_growth").toDouble(0);
            aFeed.trees_removal = query.value("trees_removal").toDouble(0);
        }

    }
    this->beginResetModel();
    items.append(aFeed);
    this->endResetModel();
}

QVariant feedsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return "   ";
        }
        if (orientation == Qt::Horizontal)
        {
            if (section == 0) return "Crop";
            if (section == 1) return "Feed";
            if (section == 2) return "Intercropping (yes / no)";
            if (section == 3) return "IF intercropping, fraction of field occupied by this crop";
            if (section == 4) return "Cut-and-carry (fraction)";
            if (section == 5) return "Land cover";
            if (section == 6) return "Land cover C Factor";
            if (section == 7) return "Slope";
            if (section == 8) return "Slope P Factor";
            if (section == 9) return "Length of slope (m)";
            if (section == 10) return "Management grassland";
            if (section == 11) return "Grassland change factor";
            if (section == 12) return "Main product removal (fraction)";
            if (section == 13) return "Crop residue removal from field (fraction)";
            if (section == 14) return "Crop residue burnt (fraction)";
            if (section == 15) return "DM content (%)";
            if (section == 16) return "ME content (MJ/kg DM)";
            if (section == 17) return "ME (MJ/kg fresh)";
            if (section == 18) return "CP content (% DM)";
            if (section == 19) return "CP content (% fresh)";
            if (section == 20) return "DE (fraction)";
            if (section == 21) return "Establishment cost (USD/ha)";
            if (section == 22) return "Operational costs (USD/yr/ha)";
            if (section == 23) return "Establishment labour (days/ha)";
            if (section == 24) return "Operational labour (days/yr/ha)";
            if (section == 25) return "WFP green (m3/kg MS)";
            if (section == 26) return "WFP blue (m3/kg MS)";
            if (section == 27) return "WFP grey (m3/kg MS)";
            if (section == 28) return "Emission factor (kg CO2e/kg)";
            if (section == 29) return "Main product Fresh Yield (t FW/ha)";
            if (section == 30) return "Main product DM content fraction";
            if (section == 31) return "Main Product Dry Yield (t DM/ha)";
            if (section == 32) return "Average Harvest index";
            if (section == 33) return "Crop residue  Fresh yield (t FW/ha)";
            if (section == 34) return "Crop residue DM content fraction";
            if (section == 35) return "Crop residue  Dry Yield(t DM/ha)";
            if (section == 36) return "Crop residue N content per area (kg N/ ha)";
            if (section == 37) return "Main product N conten (kg N/kg DM)";
            if (section == 38) return "Crop residue N content (kg N/kg DM)";
            if (section == 39) return "C (crop cover) factor";
            if (section == 40) return "N fixation by crop (kg N/ha/yr)";
            if (section == 41) return "Energy  (kcal per FW 100g )";
            if (section == 42) return "Water content (g per 100 g)";
            if (section == 43) return "Energy (kcal per 100 g DM)";
            if (section == 44) return "USDA nutrition data base entry #";
            if (section == 45) return "Kc: Initial";
            if (section == 46) return "Kc: MidSeason";
            if (section == 47) return "Kc: Late";
            if (section == 48) return "Crop coefficient (Kc) avg";
            if (section == 49) return "Category";
            if (section == 50) return "Trees/ha";
            if (section == 51) return "Trees DBH";
            if (section == 52) return "Trees annual growth (kg)";
            if (section == 53) return "Trees annual removal (kg)";
        }
    }
    return QVariant();
}

QVariant feedsModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0) return items[index.row()].feed_type_name;
        if (index.column() == 1) return items[index.row()].feed_item_name;
    }
    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        if (index.column() == 2)
        {
            if (items[index.row()].intercrop == 1)
                return "True";
            else
                return "False";
        }
        if (index.column() == 3)
        {
            if (role == Qt::DisplayRole)
                return items[index.row()].intercrop_fraction;
            else
            {
                if (items[index.row()].intercrop == 1)
                    return items[index.row()].intercrop_fraction;
            }
        }
        if (index.column() == 4) return items[index.row()].cut_carry_fraction;
        if (index.column() == 5) return items[index.row()].land_cover;
        if (index.column() == 6) return items[index.row()].landcover_c_factor;
        if (index.column() == 7) return items[index.row()].slope;
        if (index.column() == 8) return items[index.row()].slope_p_factor;
        if (index.column() == 9) return items[index.row()].slope_length;
        if (index.column() == 10) return items[index.row()].grassman;
        if (index.column() == 11) return items[index.row()].grassman_change_factor;
        if (index.column() == 12) return items[index.row()].main_product_removal;
        if (index.column() == 13) return items[index.row()].residue_removal;
        if (index.column() == 14) return items[index.row()].residue_burnt;
        if (index.column() == 15) return items[index.row()].dm_content;
        if (index.column() == 16) return items[index.row()].me_content;
        if (index.column() == 17) return items[index.row()].me_fresh;
        if (index.column() == 18) return items[index.row()].cp_content;
        if (index.column() == 19) return items[index.row()].cp_fresh;
        if (index.column() == 20) return items[index.row()].de;
        if (index.column() == 21) return items[index.row()].establishment_cost;
        if (index.column() == 22) return items[index.row()].operational_cost;
        if (index.column() == 23) return items[index.row()].establishment_labour;
        if (index.column() == 24) return items[index.row()].operational_labour;
        if (index.column() == 25) return items[index.row()].wfp_green;
        if (index.column() == 26) return items[index.row()].wfp_blue;
        if (index.column() == 27) return items[index.row()].wfp_grey;
        if (index.column() == 28) return items[index.row()].emission_factor;
        if (index.column() == 29) return items[index.row()].fresh_yield;
        if (index.column() == 30) return items[index.row()].dm_content;
        if (index.column() == 31) return items[index.row()].dry_yield;
        if (index.column() == 32) return items[index.row()].harvest_index;
        if (index.column() == 33) return items[index.row()].residue_fresh_yield;
        if (index.column() == 34) return items[index.row()].residue_dm_content;
        if (index.column() == 35) return items[index.row()].residue_dry_yield;
        if (index.column() == 36) return items[index.row()].residue_n;
        if (index.column() == 37) return items[index.row()].main_n;
        if (index.column() == 38) return items[index.row()].residue_n_dm;
        if (index.column() == 39) return items[index.row()].c_factor;
        if (index.column() == 40) return items[index.row()].n_fixation;
        if (index.column() == 41) return items[index.row()].energy;
        if (index.column() == 42) return items[index.row()].water_content;
        if (index.column() == 43) return items[index.row()].energy_dm;
        if (index.column() == 44) return items[index.row()].usda_value;
        if (index.column() == 45) return items[index.row()].kc_initial;
        if (index.column() == 46) return items[index.row()].kc_midseason;
        if (index.column() == 47) return items[index.row()].kc_late;
        if (index.column() == 48) return items[index.row()].crop_coefficient;
        if (index.column() == 49) return items[index.row()].category;
        if (index.column() == 50) return items[index.row()].trees_ha;
        if (index.column() == 51) return items[index.row()].trees_dhb;
        if (index.column() == 52) return items[index.row()].trees_growth;
        if (index.column() == 53) return items[index.row()].trees_removal;
    }
    if (role == Qt::BackgroundRole)
    {
        if ((index.column() >= 2) && (index.column() <= 14))
        {
            QBrush Background(QColor(169,209,142)); //We can change this to a nice color
            return Background;
        }
        if ((index.column() >= 15) && (index.column() <= 28))
        {
            QBrush Background(QColor(244,177,131)); //We can change this to a nice color
            return Background;
        }
        if (index.column() >= 29)
        {
            QBrush Background(QColor(143,170,220)); //We can change this to a nice color
            return Background;
        }

    }
    if (role == Qt::ForegroundRole)
    {
        if (index.column() >= 2)
        {
            QBrush Background(QColor("black")); //We can change this to a nice color
            return Background;
        }
    }
    if (role == Qt::CheckStateRole)
    {
        if (index.column() == 2)
        {
            if (items[index.row()].intercrop == 1)
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
    }

    return QVariant();
}

bool feedsModel::setData(const QModelIndex &index,const QVariant &value,int role)
{
    if (role == Qt::EditRole)
    {
        if (index.column() > 0)
        {
            if (index.column() != 2 && index.column() != 5 && index.column() != 7 && index.column() != 10 && index.column() != 49)
            {
                bool ok;
                double newValue = value.toDouble(&ok);
                if (ok)
                {
                    if (index.column() == 3)
                    {
                        if (items[index.row()].intercrop == 1)
                            items[index.row()].intercrop_fraction = newValue;
                    }
                    if (index.column() == 4) items[index.row()].cut_carry_fraction = newValue;
                    if (index.column() == 6) items[index.row()].landcover_c_factor = newValue;
                    if (index.column() == 8) items[index.row()].slope_p_factor = newValue;
                    if (index.column() == 9) items[index.row()].slope_length = newValue;
                    if (index.column() == 11) items[index.row()].grassman_change_factor = newValue;
                    if (index.column() == 12) items[index.row()].main_product_removal = newValue;
                    if (index.column() == 13) items[index.row()].residue_removal = newValue;
                    if (index.column() == 14) items[index.row()].residue_burnt = newValue;
                    if (index.column() == 15) items[index.row()].dm_content = newValue;
                    if (index.column() == 16) items[index.row()].me_content = newValue;
                    if (index.column() == 17) items[index.row()].me_fresh = newValue;
                    if (index.column() == 18) items[index.row()].cp_content = newValue;
                    if (index.column() == 19) items[index.row()].cp_fresh = newValue;
                    if (index.column() == 20) items[index.row()].de = newValue;
                    if (index.column() == 21) items[index.row()].establishment_cost = newValue;
                    if (index.column() == 22) items[index.row()].operational_cost = newValue;
                    if (index.column() == 23) items[index.row()].establishment_labour = newValue;
                    if (index.column() == 24) items[index.row()].operational_labour = newValue;
                    if (index.column() == 25) items[index.row()].wfp_green = newValue;
                    if (index.column() == 26) items[index.row()].wfp_blue = newValue;
                    if (index.column() == 27) items[index.row()].wfp_grey = newValue;
                    if (index.column() == 28) items[index.row()].emission_factor = newValue;
                    if (index.column() == 29) items[index.row()].fresh_yield = newValue;
                    if (index.column() == 30) items[index.row()].dm_content = newValue;
                    if (index.column() == 31) items[index.row()].dry_yield = newValue;
                    if (index.column() == 32) items[index.row()].harvest_index = newValue;
                    if (index.column() == 33) items[index.row()].residue_fresh_yield = newValue;
                    if (index.column() == 34) items[index.row()].residue_dm_content = newValue;
                    if (index.column() == 35) items[index.row()].residue_dry_yield = newValue;
                    if (index.column() == 36) items[index.row()].residue_n = newValue;
                    if (index.column() == 37) items[index.row()].main_n = newValue;
                    if (index.column() == 38) items[index.row()].residue_n_dm = newValue;
                    if (index.column() == 39) items[index.row()].c_factor = newValue;
                    if (index.column() == 40) items[index.row()].n_fixation = newValue;
                    if (index.column() == 41) items[index.row()].energy = newValue;
                    if (index.column() == 42) items[index.row()].water_content = newValue;
                    if (index.column() == 43) items[index.row()].energy_dm = newValue;
                    if (index.column() == 44) items[index.row()].usda_value = newValue;
                    if (index.column() == 45) items[index.row()].kc_initial = newValue;
                    if (index.column() == 46) items[index.row()].kc_midseason = newValue;
                    if (index.column() == 47) items[index.row()].kc_late = newValue;
                    if (index.column() == 48) items[index.row()].crop_coefficient = newValue;
                    if (index.column() == 50) items[index.row()].trees_ha = newValue;
                    if (index.column() == 51) items[index.row()].trees_dhb = newValue;
                    if (index.column() == 52) items[index.row()].trees_growth = newValue;
                    if (index.column() == 53) items[index.row()].trees_removal = newValue;
                    emit modelChanged();
                    return true;
                }
            }
            else
            {                
                if (index.column() == 5)
                {
                    items[index.row()].land_cover = value.toString();
                    QSqlQuery query(db);
                    if (query.exec("SELECT c_factor FROM lkp_landcover where landcover_code = '" + value.toString() + "'"))
                    {
                        if (query.first())
                        {
                            items[index.row()].landcover_c_factor = query.value(0).toDouble(0);
                        }
                    }
                    emit delegateChanged(index.column());
                }
                if (index.column() == 7)
                {
                    items[index.row()].slope = value.toString();
                    QSqlQuery query(db);
                    if (query.exec("SELECT p_factor FROM lkp_slope WHERE slope_code = '" + value.toString() + "'"))
                    {
                        if (query.first())
                        {
                            items[index.row()].slope_p_factor = query.value(0).toDouble(0);
                        }
                    }
                    emit delegateChanged(index.column());
                }
                if (index.column() == 10)
                {
                    items[index.row()].grassman = value.toString();
                    QSqlQuery query(db);
                    if (query.exec("SELECT change_factor FROM lkp_grasslandman WHERE management_code = '" + value.toString() + "'"))
                    {
                        if (query.first())
                        {
                            items[index.row()].grassman_change_factor = query.value(0).toDouble(0);
                        }
                    }
                    emit delegateChanged(index.column());
                }
                if (index.column() == 49)
                    items[index.row()].category = value.toString();
                emit modelChanged();
                return true;
            }
        }
    }
    if (role == Qt::CheckStateRole)
    {
        if (index.column() == 2)
        {
            if (value.toInt() == Qt::Checked)
                items[index.row()].intercrop = 1;
            else
            {
                items[index.row()].intercrop = 0;
                items[index.row()].intercrop_fraction = 0;
            }
            QModelIndex begin;
            begin = this->index(index.row(),0);
            QModelIndex end;
            end = this->index(index.row(),this->columnCount()-1);
            emit dataChanged(begin,end);
            emit modelChanged();
            return true;

        }
    }
    return false;
}

QString feedsModel::getCropCode(int row)
{
    if (row < items.count())
    {
        return items[row].feed_type_code;
    }
    return "";
}
QString feedsModel::getFeedCode(int row)
{
    if (row < items.count())
    {
        return items[row].feed_item_code;
    }
    return "";
}
QString feedsModel::getFeedDesc(int row)
{
    if (row < items.count())
    {
        return items[row].feed_item_name + " of " + items[row].feed_type_name;
    }
    return "";
}
void feedsModel::deleteFeed(int row)
{
    if (row < items.count())
    {
        this->beginResetModel();
        items.removeAt(row);
        this->endResetModel();
    }
}
