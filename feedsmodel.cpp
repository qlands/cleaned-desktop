#include "feedsmodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QBrush>
#include <QFont>

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
         aFeed.source_type = feedObject["source_type"].toString();
         aFeed.water_regime = feedObject["water_regime"].toString();
         aFeed.ecosystem_type = feedObject["ecosystem_type"].toString();
         aFeed.organic_amendment = feedObject["organic_amendment"].toString();
         aFeed.intercrop = feedObject["intercrop"].toInt(0);
         aFeed.intercrop_fraction = feedObject["intercrop_fraction"].toDouble(0);
         aFeed.cut_carry_fraction = feedObject["cut_carry_fraction"].toDouble(0);
         aFeed.land_cover = feedObject["land_cover"].toString();
         aFeed.land_cover_desc = feedObject["land_cover_desc"].toString();
         aFeed.landcover_c_factor = feedObject["landcover_c_factor"].toDouble(0);
         aFeed.slope = feedObject["slope"].toString();
         aFeed.slope_desc = feedObject["slope_desc"].toString();
         aFeed.slope_p_factor = feedObject["slope_p_factor"].toDouble(0);
         aFeed.slope_length = feedObject["slope_length"].toDouble(0);
         aFeed.grassman_desc = feedObject["grassman_desc"].toString();
         aFeed.grassman = feedObject["grassman"].toString();
         aFeed.grassman_change_factor = feedObject["grassman_change_factor"].toDouble(0);
         aFeed.main_product_removal = feedObject["main_product_removal"].toDouble(0);
         aFeed.residue_removal = feedObject["residue_removal"].toDouble(0);
         aFeed.residue_burnt = feedObject["residue_burnt"].toDouble(0);         
         aFeed.n_content = feedObject["n_content"].toDouble(0);
         aFeed.residue_n = feedObject["residue_n"].toDouble(0);
         aFeed.dry_yield = feedObject["dry_yield"].toDouble(0);
         aFeed.residue_dry_yield = feedObject["residue_dry_yield"].toDouble(0);
         aFeed.dm_content = feedObject["dm_content"].toDouble(0);
         aFeed.me_content = feedObject["me_content"].toDouble(0);
         aFeed.cp_content = feedObject["cp_content"].toDouble(0);
         aFeed.cultivation_period = feedObject["cultivation_period"].toInt(0);
         aFeed.dm_content = feedObject["dm_content"].toDouble(0);         
         aFeed.main_n = feedObject["main_n"].toDouble(0);         

         aFeed.energy = feedObject["energy"].toDouble(0);
         aFeed.water_content = feedObject["water_content"].toDouble(0);
         aFeed.usda_value = feedObject["usda_value"].toDouble(0);
         aFeed.kc_initial = feedObject["kc_initial"].toDouble(0);
         aFeed.kc_midseason = feedObject["kc_midseason"].toDouble(0);
         aFeed.kc_late = feedObject["kc_late"].toDouble(0);
         aFeed.category = feedObject["category"].toString();
         aFeed.trees_ha = feedObject["trees_ha"].toDouble(0);
         aFeed.trees_dhb = feedObject["trees_dhb"].toDouble(0);
         aFeed.trees_growth = feedObject["trees_growth"].toDouble(0);
         aFeed.trees_removal = feedObject["trees_removal"].toDouble(0);
         aFeed.trees_ha_dbh25 = feedObject["trees_ha_dbh25"].toDouble(0);
         aFeed.average_dbh25 = feedObject["average_dbh25"].toDouble(0);
         aFeed.increase_dbh25 = feedObject["increase_dbh25"].toDouble(0);
         aFeed.trees_ha_dbh2550 = feedObject["trees_ha_dbh2550"].toDouble(0);
         aFeed.average_dbh2550 = feedObject["average_dbh2550"].toDouble(0);
         aFeed.increase_dbh2550 = feedObject["increase_dbh2550"].toDouble(0);
         aFeed.trees_ha_dbh50 = feedObject["trees_ha_dbh50"].toDouble(0);
         aFeed.average_dbh50 = feedObject["average_dbh50"].toDouble(0);
         aFeed.increase_dbh50 = feedObject["increase_dbh50"].toDouble(0);
         aFeed.time_horizon = feedObject["time_horizon"].toDouble(0);
         aFeed.diameter_breast = feedObject["diameter_breast"].toDouble(0);

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
    return 46;
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
        if (index.column() != 4)
        {
            if (index.column() != 3)
            {
                if (index.column() != 2 && index.column() != 6 && index.column() != 8 && index.column() != 11 && index.column() != 19 && index.column() != 21 && index.column() != 22)
                    return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
                else
                    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
            }
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
        feedObject["source_type"] = items[i].source_type;
        feedObject["water_regime"] = items[i].water_regime;
        feedObject["organic_amendment"] = items[i].organic_amendment;
        feedObject["ecosystem_type"] = items[i].ecosystem_type;
        feedObject["intercrop"] = items[i].intercrop;
        feedObject["intercrop_fraction"] = items[i].intercrop_fraction;
        feedObject["cut_carry_fraction"] = items[i].cut_carry_fraction;
        feedObject["land_cover"] = items[i].land_cover;
        feedObject["land_cover_desc"] = items[i].land_cover_desc;
        feedObject["landcover_c_factor"] = items[i].landcover_c_factor;
        feedObject["slope"] = items[i].slope;
        feedObject["slope_desc"] = items[i].slope_desc;
        feedObject["slope_p_factor"] = items[i].slope_p_factor;
        feedObject["slope_length"] = items[i].slope_length;
        feedObject["grassman"] = items[i].grassman;
        feedObject["grassman_desc"] = items[i].grassman_desc;
        feedObject["grassman_change_factor"] = items[i].grassman_change_factor;
        feedObject["main_product_removal"] = items[i].main_product_removal;
        feedObject["residue_removal"] = items[i].residue_removal;
        feedObject["residue_burnt"] = items[i].residue_burnt;
        feedObject["residue_n"] = items[i].residue_n;
        feedObject["n_content"] = items[i].n_content;
        feedObject["dry_yield"] = items[i].dry_yield;
        feedObject["residue_dry_yield"] = items[i].residue_dry_yield;

        feedObject["dm_content"] = items[i].dm_content;
        feedObject["me_content"] = items[i].me_content;
        feedObject["cp_content"] = items[i].cp_content;
        feedObject["cultivation_period"] = items[i].cultivation_period;
        feedObject["dm_content"] = items[i].dm_content;        
        feedObject["main_n"] = items[i].main_n;        

        feedObject["energy"] = items[i].energy;
        feedObject["water_content"] = items[i].water_content;
        feedObject["usda_value"] = items[i].usda_value;
        feedObject["kc_initial"] = items[i].kc_initial;
        feedObject["kc_midseason"] = items[i].kc_midseason;
        feedObject["kc_late"] = items[i].kc_late;
        feedObject["category"] = items[i].category;
        feedObject["trees_ha"] = items[i].trees_ha;
        feedObject["trees_dhb"] = items[i].trees_dhb;
        feedObject["trees_growth"] = items[i].trees_growth;
        feedObject["trees_removal"] = items[i].trees_removal;
        feedObject["trees_ha_dbh25"] = items[i].trees_ha_dbh25;
        feedObject["average_dbh25"] = items[i].average_dbh25;
        feedObject["increase_dbh25"] = items[i].increase_dbh25;
        feedObject["trees_ha_dbh2550"] = items[i].trees_ha_dbh2550;
        feedObject["average_dbh2550"] = items[i].average_dbh2550;
        feedObject["increase_dbh2550"] = items[i].increase_dbh2550;
        feedObject["trees_ha_dbh50"] = items[i].trees_ha_dbh50;
        feedObject["average_dbh50"] = items[i].average_dbh50;
        feedObject["increase_dbh50"] = items[i].increase_dbh50;
        feedObject["time_horizon"] = items[i].time_horizon;
        feedObject["diameter_breast"] = items[i].diameter_breast;


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
    aFeed.source_type = "Main";
    aFeed.intercrop = 0;
    aFeed.intercrop_fraction = 0;
    aFeed.cut_carry_fraction = 0;
    aFeed.landcover_c_factor = 0;
    aFeed.ecosystem_type ="";
    aFeed.organic_amendment="";
    aFeed.water_regime ="";


    if (query.exec("SELECT landcover_code,landcover_desc,c_factor FROM lkp_landcover LIMIT 1"))
    {
        if (query.first())
        {
            aFeed.land_cover = query.value(0).toString();
            aFeed.land_cover_desc = query.value(1).toString();
            aFeed.landcover_c_factor = query.value(2).toDouble(0);
        }
    }
    aFeed.slope_p_factor = 0;
    if (query.exec("SELECT slope_code,slope_desc,p_factor FROM lkp_slope LIMIT 1"))
    {
        if (query.first())
        {
            aFeed.slope = query.value(0).toString();
            aFeed.slope_desc = query.value(1).toString();
            aFeed.slope_p_factor = query.value(2).toDouble(0);
        }
    }
    aFeed.slope_length = 0;
    aFeed.grassman_change_factor = 0;
    if (query.exec("SELECT management_code,management_desc,change_factor FROM lkp_grasslandman"))
    {
        if (query.first())
        {
            aFeed.grassman = query.value(0).toString();
            aFeed.grassman_desc = query.value(1).toString();
            aFeed.grassman_change_factor = query.value(2).toDouble(0);
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
            aFeed.cp_content = query.value("cp_content").toDouble(0);

            aFeed.n_content = 0;
            aFeed.residue_n = 0;

            aFeed.cultivation_period = query.value("cultivation_period").toInt(0);


        }
    }
    if (query.exec("SELECT * FROM lkp_feedtype WHERE feed_type_code = '" + crop + "'"))
    {
        if (query.first())
        {
            aFeed.feed_type_name = query.value("feed_type_name").toString();
            aFeed.dm_fraction = query.value("dm_fraction").toDouble(0);            
            aFeed.main_n = query.value("main_n").toDouble(0);

            aFeed.dry_yield = query.value("dry_yield").toDouble(0);
            aFeed.residue_dry_yield = query.value("residue_dry_yield").toDouble(0);

            aFeed.energy = query.value("energy").toDouble(0);
            aFeed.water_content = query.value("water_content").toDouble(0);
            aFeed.usda_value = query.value("usda_value").toDouble(0);
            aFeed.kc_initial = query.value("kc_initial").toDouble(0);
            aFeed.kc_midseason = query.value("kc_midseason").toDouble(0);
            aFeed.kc_late = query.value("kc_late").toDouble(0);
            aFeed.category = query.value("category").toString();
            aFeed.trees_ha = query.value("trees_ha").toDouble(0);
            aFeed.trees_dhb = query.value("trees_dhb").toDouble(0);
            aFeed.trees_growth = query.value("trees_growth").toDouble(0);
            aFeed.trees_removal = query.value("trees_removal").toDouble(0);
            aFeed.trees_ha_dbh25 = query.value("trees_ha_dbh25").toDouble(0);
            aFeed.average_dbh25 = query.value("average_dbh25").toDouble(0);
            aFeed.increase_dbh25 = query.value("increase_dbh25").toDouble(0);
            aFeed.trees_ha_dbh2550 = query.value("trees_ha_dbh2550").toDouble(0);
            aFeed.average_dbh2550 = query.value("average_dbh2550").toDouble(0);
            aFeed.increase_dbh2550 = query.value("increase_dbh2550").toDouble(0);
            aFeed.trees_ha_dbh50 = query.value("trees_ha_dbh50").toDouble(0);
            aFeed.average_dbh50 = query.value("average_dbh50").toDouble(0);
            aFeed.increase_dbh50 = query.value("increase_dbh50").toDouble(0);
            aFeed.time_horizon = query.value("time_horizon").toDouble(0);
            aFeed.diameter_breast = query.value("diameter_breast").toDouble(0);;
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
            if (section == 2) return "Source type";
            if (section == 3) return "Intercropping (yes / no)";
            if (section == 4) return "IF intercropping, fraction of field occupied by this crop";
            if (section == 5) return "Cut-and-carry (fraction)";
            if (section == 6) return "Land cover";
            if (section == 7) return "Land cover C Factor";
            if (section == 8) return "Slope";
            if (section == 9) return "Slope P Factor";
            if (section == 10) return "Length of slope (m)";
            if (section == 11) return "Management grassland";
            if (section == 12) return "Grassland change factor";
            if (section == 13) return "Main product removal (fraction)";
            if (section == 14) return "Crop residue removal from field (fraction)";
            if (section == 15) return "Crop residue burnt (fraction)";
            if (section == 16) return "DM content (%)";
            if (section == 17) return "ME content (MJ/kg DM)";
            if (section == 18) return "CP content (% DM)";
            if (section == 19) return "water regime";
            if (section == 20) return "cultivation period";
            if (section == 21) return "reice ecosystem type";
            if (section == 22) return "reice organic amendment";
            if (section == 23) return "Main poduct dry yield (t DM/ha)";
            if (section == 24) return "Residue dry yield (t DM/ha)";
            if (section == 25) return "Main product N content (kg N/kg DM)";
            if (section == 26) return "Reside N content (kg N/kg DM)";
            if (section == 27) return "Kc: Initial";
            if (section == 28) return "Kc: MidSeason";
            if (section == 29) return "Kc: Late";
            if (section == 30) return "Category";
            if (section == 31) return "Trees/ha";
            if (section == 32) return "Trees DBH";
            if (section == 33) return "Trees annual growth (kg)";
            if (section == 34) return "Trees annual removal (kg)";
            if (section == 35) return "Number of trees per hectare with a diameter at breast height of less than 25cm";
            if (section == 36) return "Average diameter at breast height of trees with a DBH of less than 25cm";
            if (section == 37) return "Diameter at breast height increase in trees with a DBH of less than 25cm (cm/year)";
            if (section == 38) return "Number of trees per hectare with a diameter at breast height of 25-50cm";
            if (section == 39) return "Average diameter at breast height of trees with a DBH of 25-50cm";
            if (section == 40) return "Diameter at breast height increase in trees with a DBH of 25-50cm (cm/year)";
            if (section == 41) return "Number of trees per hectare with a diameter at breast height more than 50cm";
            if (section == 42) return "Average diameter at breast height of trees with a DBH more than 50cm";
            if (section == 43) return "Diameter at breast height increase in trees with a DBH of more than 50cm (cm/year)";
            if (section == 44) return "Time it takes for tree to mature (years)";
            if (section == 45) return "Tree breast diameter (cm)";
            //if (section == 46) return "Time horizon (years)";

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
        //if (index.column() == 29) return items[index.row()].water_regime;
        if (index.column() == 2) return items[index.row()].source_type;
        if (index.column() == 3)
        {
            if (items[index.row()].intercrop == 1)
                return "Yes";
            else
                return "No";
        }
        if (index.column() == 4)
        {
            if (role == Qt::DisplayRole)
                return items[index.row()].intercrop_fraction;
            else
            {
                if (items[index.row()].intercrop == 1)
                    return items[index.row()].intercrop_fraction;
            }
        }
        if (index.column() == 5) return items[index.row()].cut_carry_fraction;

        if (index.column() == 19)
        {
            QSqlQuery query(db);
            if (query.exec("SELECT waterregime_desc, waterregime_factor FROM lkp_waterregime where waterregime_desc = '" + items[index.row()].water_regime + "'"))
            {
                if (query.first())
                {
                    return query.value(0).toString();//first Record
                }
            }
        }
        if (index.column() == 21) return items[index.row()].ecosystem_type;

        if (index.column() == 22)
         return items[index.row()].organic_amendment;

        if (index.column() == 6)
        {
            QSqlQuery query(db);
            if (query.exec("SELECT landcover_desc FROM lkp_landcover where landcover_code = '" + items[index.row()].land_cover + "'"))
            {
                if (query.first())
                {
                    return query.value(0).toString();
                }
            }
        }
        if (index.column() == 7) return items[index.row()].landcover_c_factor;
        if (index.column() == 8)
        {
            QSqlQuery query(db);
            if (query.exec("SELECT slope_desc FROM lkp_slope where slope_code = '" + items[index.row()].slope + "'"))
            {
                if (query.first())
                {
                    return query.value(0).toString();
                }
            }
        }
        if (index.column() == 9) return items[index.row()].slope_p_factor;
        if (index.column() == 10) return items[index.row()].slope_length;
        if (index.column() == 11)
        {
            QSqlQuery query(db);
            if (query.exec("SELECT management_desc FROM lkp_grasslandman where management_code = '" + items[index.row()].grassman + "'"))
            {
                if (query.first())
                {
                    return query.value(0).toString();
                }
            }
        }
        if (index.column() == 12) return items[index.row()].grassman_change_factor;
        if (index.column() == 13) return items[index.row()].main_product_removal;
        if (index.column() == 14) return items[index.row()].residue_removal;
        if (index.column() == 15) return items[index.row()].residue_burnt;

        if (index.column() == 16) return items[index.row()].dm_content;
        if (index.column() == 17) return items[index.row()].me_content;
        if (index.column() == 18) return items[index.row()].cp_content;
        if (index.column() == 19) return items[index.row()].water_regime;
        if (index.column() == 20) return items[index.row()].cultivation_period;
        if (index.column() == 21) return items[index.row()].ecosystem_type;
        if (index.column() == 22) return items[index.row()].organic_amendment;

        if (index.column() == 23) return items[index.row()].dry_yield;
        if (index.column() == 24) return items[index.row()].residue_dry_yield;
        if (index.column() == 25) return items[index.row()].n_content;
        if (index.column() == 26) return items[index.row()].residue_n;
        if (index.column() == 27) return items[index.row()].kc_initial;
        if (index.column() == 28) return items[index.row()].kc_midseason;
        if (index.column() == 29) return items[index.row()].kc_late;
        if (index.column() == 30) return items[index.row()].category;

        if (index.column() == 31) return items[index.row()].trees_ha;
        if (index.column() == 32) return items[index.row()].trees_dhb;
        if (index.column() == 33) return items[index.row()].trees_growth;
        if (index.column() == 34) return items[index.row()].trees_removal;
        if (index.column() == 35) return items[index.row()].trees_ha_dbh25;
        if (index.column() == 36) return items[index.row()].average_dbh25;
        if (index.column() == 37) return items[index.row()].increase_dbh25;
        if (index.column() == 38) return items[index.row()].trees_ha_dbh2550;
        if (index.column() == 39) return items[index.row()].average_dbh2550;
        if (index.column() == 40) return items[index.row()].increase_dbh2550;
        if (index.column() == 41) return items[index.row()].trees_ha_dbh50;
        if (index.column() == 42) return items[index.row()].average_dbh50;
        if (index.column() == 43) return items[index.row()].increase_dbh50;
        if (index.column() == 44) return items[index.row()].time_horizon;
        if (index.column() == 45) return items[index.row()].diameter_breast;
        //if (index.column() == 46) return items[index.row()].time_horizon;

    }
    if (role == Qt::BackgroundRole)
    {
        if ((index.column() >= 2) && (index.column() <= 15))
        {
            QBrush Background(QColor(169,209,142)); //We can change this to a nice color
            return Background;
        }
        if ((index.column() >= 16) && (index.column() <= 22))
        {
            QBrush Background(QColor(244,177,131)); //We can change this to a nice color
            return Background;
        }
        if ((index.column() >= 23) )
        {
            QBrush Background(QColor(143,170,220)); //We can change this to a nice color
            return Background;
        }

    }
    if (role == Qt::FontRole)
    {
        if (index.column() == 2 || index.column() == 6 || index.column() == 8 || index.column() == 11 || index.column() == 19 || index.column() == 21 || index.column() == 22)
        {
            QFont font;
            font.setUnderline(true);
            return font;
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
        if (index.column() == 3)
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
            if (index.column() != 2 && index.column() != 3 && index.column() != 6 && index.column() != 8 && index.column() != 11 && index.column() != 19 && index.column() != 21 && index.column() != 22 && index.column() != 30)
            {
                bool ok;
                double newValue = value.toDouble(&ok);
                if (ok)
                {
                    if (index.column() == 4)
                    {
                        if (items[index.row()].intercrop == 1)
                            items[index.row()].intercrop_fraction = newValue;
                    }
                    if (index.column() == 5) items[index.row()].cut_carry_fraction = newValue;

                    // 6 = Land cover type

                    if (index.column() == 7) items[index.row()].landcover_c_factor = newValue;

                    // 8 = Slope type

                    if (index.column() == 9) items[index.row()].slope_p_factor = newValue;
                    if (index.column() == 10) items[index.row()].slope_length = newValue;
                    if (index.column() == 12) items[index.row()].grassman_change_factor  = newValue;
                    if (index.column() == 13) items[index.row()].main_product_removal = newValue;
                    if (index.column() == 14) items[index.row()].residue_removal = newValue;
                    if (index.column() == 15) items[index.row()].residue_burnt = newValue;

                    if (index.column() == 16)  items[index.row()].dm_content = newValue;
                    if (index.column() == 17)  items[index.row()].me_content = newValue;
                    if (index.column() == 18)  items[index.row()].cp_content = newValue;
                    if (index.column() == 20)  items[index.row()].cultivation_period = newValue;

                    if (index.column() == 23)  items[index.row()].dry_yield = newValue;
                    if (index.column() == 24)  items[index.row()].residue_dry_yield = newValue;
                    if (index.column() == 25)  items[index.row()].n_content = newValue;
                    if (index.column() == 26)  items[index.row()].residue_n = newValue;
                    if (index.column() == 27)  items[index.row()].kc_initial = newValue;
                    if (index.column() == 28)  items[index.row()].kc_midseason = newValue;
                    if (index.column() == 29)  items[index.row()].kc_late = newValue;
                    if (index.column() == 31)  items[index.row()].trees_ha = newValue;
                    if (index.column() == 32)  items[index.row()].trees_dhb = newValue;
                    if (index.column() == 33)  items[index.row()].trees_growth = newValue;
                    if (index.column() == 34)  items[index.row()].trees_removal = newValue;
                    if (index.column() == 35)  items[index.row()].trees_ha_dbh25 = newValue;
                    if (index.column() == 36)  items[index.row()].average_dbh25 = newValue;
                    if (index.column() == 37)  items[index.row()].increase_dbh25 = newValue;
                    if (index.column() == 38)  items[index.row()].trees_ha_dbh2550 = newValue;
                    if (index.column() == 39)  items[index.row()].average_dbh2550 = newValue;
                    if (index.column() == 40)  items[index.row()].increase_dbh2550 = newValue;
                    if (index.column() == 41)  items[index.row()].trees_ha_dbh50 = newValue;
                    if (index.column() == 42)  items[index.row()].average_dbh50 = newValue;
                    if (index.column() == 43)  items[index.row()].increase_dbh50 = newValue;
                    if (index.column() == 44)  items[index.row()].time_horizon = newValue;
                    if (index.column() == 45)  items[index.row()].diameter_breast = newValue;
                    //if (index.column() == 46)  items[index.row()].time_horizon = newValue;

                    emit modelChanged();
                    return true;
                }
            }
            else
            {
                if (index.column() == 19)
                {
                    items[index.row()].water_regime = value.toString();
                    QSqlQuery query(db);
                    if (query.exec("SELECT waterregime_desc,waterregime_factor FROM lkp_waterregime where waterregime_code = '" + value.toString() + "'"))
                    {
                        if (query.first())
                        {
                            items[index.row()].water_regime = query.value(0).toString();
                        }
                    }
                    emit delegateChanged(index.column());
                }
                if (index.column() == 21)
                {
                    items[index.row()].ecosystem_type = value.toString();
                    emit delegateChanged(index.column());
                }

                if (index.column() == 22)
                {
                    {
                        items[index.row()].organic_amendment = value.toString();
                        emit delegateChanged(index.column());
                    }
                }
                if (index.column() == 2)
                {
                    items[index.row()].source_type = value.toString();
                    emit delegateChanged(index.column());
                }
                if (index.column() == 6)
                {
                    items[index.row()].land_cover = value.toString();
                    QSqlQuery query(db);
                    if (query.exec("SELECT landcover_desc,c_factor FROM lkp_landcover where landcover_code = '" + value.toString() + "'"))
                    {
                        if (query.first())
                        {
                            items[index.row()].land_cover_desc = query.value(0).toString();
                            items[index.row()].landcover_c_factor = query.value(1).toDouble(0);
                        }
                    }
                    emit delegateChanged(index.column());
                }
                if (index.column() == 8)
                {
                    items[index.row()].slope = value.toString();
                    QSqlQuery query(db);
                    if (query.exec("SELECT slope_desc,p_factor FROM lkp_slope WHERE slope_code = '" + value.toString() + "'"))
                    {
                        if (query.first())
                        {
                            items[index.row()].slope_desc = query.value(0).toString();
                            items[index.row()].slope_p_factor = query.value(1).toDouble(0);
                        }
                    }
                    emit delegateChanged(index.column());
                }
                if (index.column() == 11)
                {
                    items[index.row()].grassman = value.toString();
                    QSqlQuery query(db);
                    if (query.exec("SELECT management_desc,change_factor FROM lkp_grasslandman WHERE management_code = '" + value.toString() + "'"))
                    {
                        if (query.first())
                        {
                            items[index.row()].grassman_desc = query.value(0).toString();
                            items[index.row()].grassman_change_factor = query.value(1).toDouble(0);
                        }
                    }
                    emit delegateChanged(index.column());
                }                
                if (index.column() == 30)
                    items[index.row()].category = value.toString();
                emit modelChanged();
                return true;
            }
        }
    }
    if (role == Qt::CheckStateRole)
    {
        if (index.column() == 3)
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

QString feedsModel::getCropDesc(int row)
{
    if (row < items.count())
    {
        return items[row].feed_type_name;
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
