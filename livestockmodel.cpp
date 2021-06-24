#include "livestockmodel.h"
#include <QColor>
#include <QBrush>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QDebug>

livestockModel::livestockModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void livestockModel::setDataArray(QJsonArray data)
{
    this->beginResetModel();
    items.clear();
    for (int seasonIndex = 0; seasonIndex < data.size(); ++seasonIndex)
    {
         QJsonObject seasonObject = data[seasonIndex].toObject();
         Tlivestock aLivestock;
         aLivestock.livetype_code = seasonObject["livetype_code"].toString();
         aLivestock.livetype_desc = seasonObject["livetype_desc"].toString();
         aLivestock.herd_composition = seasonObject["herd_composition"].toDouble(0);
         aLivestock.annual_milk = seasonObject["annual_milk"].toDouble(0);
         aLivestock.annual_wool = seasonObject["annual_wool"].toDouble(0);
         aLivestock.annual_growth = seasonObject["annual_growth"].toDouble(0);
         aLivestock.time_in_stable = seasonObject["time_in_stable"].toDouble(0);
         aLivestock.time_in_non_roofed_enclosure = seasonObject["time_in_non_roofed_enclosure"].toDouble(0);
         aLivestock.time_in_onfarm_grazing = seasonObject["time_in_onfarm_grazing"].toDouble(0);
         aLivestock.time_in_offfarm_grazing = seasonObject["time_in_offfarm_grazing"].toDouble(0);
         aLivestock.distance_to_pasture = seasonObject["distance_to_pasture"].toDouble(0);
         aLivestock.manure_in_stable = seasonObject["manure_in_stable"].toDouble(0);
         aLivestock.manure_in_non_roofed_enclosure = seasonObject["manure_in_non_roofed_enclosure"].toDouble(0);
         aLivestock.manure_in_field = seasonObject["manure_in_field"].toDouble(0);
         aLivestock.body_weight = seasonObject["body_weight"].toDouble(0);
         aLivestock.litter_size = seasonObject["litter_size"].toDouble(0);
         aLivestock.lactation_length = seasonObject["lactation_length"].toDouble(0);
         aLivestock.proportion_growth = seasonObject["proportion_growth"].toDouble(0);
         aLivestock.lw_gain = seasonObject["lw_gain"].toDouble(0);
         aLivestock.grazing_displacement = seasonObject["grazing_displacement"].toDouble(0);         
         aLivestock.cp_maintenance = seasonObject["cp_maintenance"].toDouble(0);
         aLivestock.cp_grazing = seasonObject["cp_grazing"].toDouble(0);
         aLivestock.cp_pregnancy = seasonObject["cp_pregnancy"].toDouble(0);
         aLivestock.cp_lactation = seasonObject["cp_lactation"].toDouble(0);
         aLivestock.cp_lactmilk = seasonObject["cp_lactmilk"].toDouble(0);
         aLivestock.cp_growth = seasonObject["cp_growth"].toDouble(0);
         aLivestock.birth_interval = seasonObject["birth_interval"].toDouble(0);
         aLivestock.protein_milkcontent = seasonObject["protein_milkcontent"].toDouble(0);
         aLivestock.fat_content = seasonObject["fat_content"].toDouble(0);
         aLivestock.energy_milkcontent = seasonObject["energy_milkcontent"].toDouble(0);
         aLivestock.energy_meatcontent = seasonObject["energy_meatcontent"].toDouble(0);
         aLivestock.protein_meatcontent = seasonObject["protein_meatcontent"].toDouble(0);
         aLivestock.carcass_fraction = seasonObject["carcass_fraction"].toDouble(0);
         aLivestock.energy_eggcontent = seasonObject["energy_eggcontent"].toDouble(0);
         aLivestock.n_content = seasonObject["n_content"].toDouble(0);
         aLivestock.water_requirement = seasonObject["water_requirement"].toDouble(0);
         aLivestock.meat_product = seasonObject["meat_product"].toString();
         aLivestock.milk_product = seasonObject["milk_product"].toString();
         aLivestock.oneoff_cost = seasonObject["oneoff_cost"].toDouble(0);
         aLivestock.operational_cost = seasonObject["operational_cost"].toDouble(0);
         aLivestock.oneoff_labour = seasonObject["oneoff_labour"].toDouble(0);
         aLivestock.operational_labour = seasonObject["operational_labour"].toDouble(0);
         aLivestock.meat_price = seasonObject["meat_price"].toDouble(0);
         aLivestock.milk_price = seasonObject["milk_price"].toDouble(0);
         aLivestock.ipcc_meth_ef_t1 = seasonObject["ipcc_ef_category_t1"].toDouble(0);
         aLivestock.ipcc_meth_ef_t2 = seasonObject["ipcc_ef_category_t2"].toDouble(0);
         aLivestock.ipcc_meth_man = seasonObject["ipcc_meth_man_category"].toDouble(0);
         aLivestock.ipcc_meth_exc = seasonObject["ipcc_n_exc_category"].toDouble(0);

         aLivestock.manureman_non_roofed_enclosure = seasonObject["manureman_non_roofed_enclosure"].toString();
         aLivestock.manureman_offfarm_grazing = seasonObject["manureman_offfarm_grazing"].toString();
         aLivestock.manureman_onfarm_grazing = seasonObject["manureman_onfarm_grazing"].toString();
         aLivestock.manureman_stable = seasonObject["manureman_stable"].toString();
         aLivestock.manure_onfarm_fraction = seasonObject["manure_onfarm_fraction"].toDouble(0);
         aLivestock.manure_sales_fraction = seasonObject["manure_sales_fraction"].toDouble(0);
         aLivestock.body_weight_weaning = seasonObject["body_weight_weaning"].toDouble(0);
         aLivestock.body_weight_year_one = seasonObject["body_weight_year_one"].toDouble(0);
         aLivestock.adult_weight = seasonObject["adult_weight"].toDouble(0);
         aLivestock.work_hour = seasonObject["work_hour"].toDouble(0);
         aLivestock.piglets_relying_on_milk = seasonObject["piglets_relying_on_milk"].toDouble(0);

         items.append(aLivestock);
    }
    this->endResetModel();
}

void livestockModel::setDatabase(QSqlDatabase cleaned_db)
{
    this->db = cleaned_db;
}

int livestockModel::columnCount(const QModelIndex &) const
{
    return 58;
}

int livestockModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

bool livestockModel::livestockExists(QString code)
{
    for (int i = 0; i < items.count(); i++)
    {
        if (items[i].livetype_code == code)
        {
            return true;
        }
    }
    return false;
}

Qt::ItemFlags livestockModel::flags(const QModelIndex &index) const
{
    if (index.column() > 0)
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


QJsonArray livestockModel::getLivestockArray()
{
    QJsonArray livestockArray;
    for (int i = 0; i < items.count(); i++)
    {
        QJsonObject livestockObject;
        livestockObject["livetype_code"] = items[i].livetype_code;
        livestockObject["livetype_desc"] = items[i].livetype_desc;
        livestockObject["herd_composition"] = items[i].herd_composition;
        livestockObject["annual_milk"] = items[i].annual_milk;
        livestockObject["annual_wool"] = items[i].annual_wool;
        livestockObject["annual_growth"] = items[i].annual_growth;
        livestockObject["time_in_stable"] = items[i].time_in_stable;
        livestockObject["time_in_non_roofed_enclosure"] = items[i].time_in_non_roofed_enclosure;
        livestockObject["time_in_onfarm_grazing"] = items[i].time_in_onfarm_grazing;
        livestockObject["time_in_offfarm_grazing"] = items[i].time_in_offfarm_grazing;
        livestockObject["distance_to_pasture"] = items[i].distance_to_pasture;
        livestockObject["manure_in_stable"] = items[i].manure_in_stable;
        livestockObject["manure_in_non_roofed_enclosure"] = items[i].manure_in_non_roofed_enclosure;
        livestockObject["manure_in_field"] = items[i].manure_in_field;
        livestockObject["body_weight"] = items[i].body_weight;
        livestockObject["litter_size"] = items[i].litter_size;
        livestockObject["lactation_length"] = items[i].lactation_length;
        livestockObject["proportion_growth"] = items[i].proportion_growth;
        livestockObject["lw_gain"] = items[i].lw_gain;
        livestockObject["grazing_displacement"] = items[i].grazing_displacement;
        livestockObject["cp_maintenance"] = items[i].cp_maintenance;
        livestockObject["cp_grazing"] = items[i].cp_grazing;
        livestockObject["cp_pregnancy"] = items[i].cp_pregnancy;
        livestockObject["cp_lactation"] = items[i].cp_lactation;
        livestockObject["cp_lactmilk"] = items[i].cp_lactmilk;
        livestockObject["cp_growth"] = items[i].cp_growth;
        livestockObject["birth_interval"] = items[i].birth_interval;
        livestockObject["protein_milkcontent"] = items[i].protein_milkcontent;
        livestockObject["fat_content"] = items[i].fat_content;
        livestockObject["energy_milkcontent"] = items[i].energy_milkcontent;
        livestockObject["energy_meatcontent"] = items[i].energy_meatcontent;
        livestockObject["protein_meatcontent"] = items[i].protein_meatcontent;
        livestockObject["carcass_fraction"] = items[i].carcass_fraction;
        livestockObject["energy_eggcontent"] = items[i].energy_eggcontent;
        livestockObject["n_content"] = items[i].n_content;
        livestockObject["water_requirement"] = items[i].water_requirement;
        livestockObject["meat_product"] = items[i].meat_product;
        livestockObject["milk_product"] = items[i].milk_product;
        livestockObject["oneoff_cost"] = items[i].oneoff_cost;
        livestockObject["operational_cost"] = items[i].operational_cost;
        livestockObject["oneoff_labour"] = items[i].oneoff_labour;
        livestockObject["operational_labour"] = items[i].operational_labour;
        livestockObject["meat_price"] = items[i].meat_price;
        livestockObject["milk_price"] = items[i].milk_price;
        livestockObject["ipcc_ef_category_t1"] = items[i].ipcc_meth_ef_t1;
        livestockObject["ipcc_ef_category_t2"] = items[i].ipcc_meth_ef_t2;
        livestockObject["ipcc_meth_man_category"] = items[i].ipcc_meth_man;
        livestockObject["ipcc_n_exc_category"] = items[i].ipcc_meth_exc;

        livestockObject["manureman_non_roofed_enclosure"] = items[i].manureman_non_roofed_enclosure;
        livestockObject["manureman_offfarm_grazing"] = items[i].manureman_offfarm_grazing;
        livestockObject["manureman_onfarm_grazing"] = items[i].manureman_onfarm_grazing;
        livestockObject["manureman_stable"] = items[i].manureman_stable;
        livestockObject["manure_onfarm_fraction"] = items[i].manure_onfarm_fraction;
        livestockObject["manure_sales_fraction"] = items[i].manure_sales_fraction;
        livestockObject["body_weight_weaning"] = items[i].body_weight_weaning;
        livestockObject["body_weight_year_one"] = items[i].body_weight_year_one;
        livestockObject["adult_weight"] = items[i].adult_weight;
        livestockObject["work_hour"] = items[i].work_hour;
        livestockObject["piglets_relying_on_milk"] = items[i].piglets_relying_on_milk;

        livestockArray.append(livestockObject);
    }
    return livestockArray;
}

void livestockModel::addNewLivestock(QString code, QString region)
{
    if (livestockExists(code))
        return;
    QSqlQuery query(this->db);
    if (query.exec("SELECT * FROM lkp_livetype WHERE livetype_code = '" + code + "'"))
    {
        this->beginResetModel();
        while (query.next())
        {
            Tlivestock aLivestock;
            aLivestock.livetype_code = code;
            aLivestock.livetype_desc = query.value("livetype_desc").toString();
            aLivestock.herd_composition = 0;
            aLivestock.annual_milk = 0;
            aLivestock.annual_wool = 0;
            aLivestock.annual_growth = 0;
            aLivestock.time_in_stable = 0;
            aLivestock.time_in_non_roofed_enclosure = 0;
            aLivestock.time_in_onfarm_grazing = 0;
            aLivestock.time_in_offfarm_grazing = 0;
            aLivestock.distance_to_pasture = 0;
            aLivestock.manure_in_stable = 0;
            aLivestock.manure_in_non_roofed_enclosure = 0;
            aLivestock.manure_in_field = 0;
            aLivestock.body_weight = query.value("body_weight").toDouble(0);
            aLivestock.litter_size = query.value("litter_size").toDouble(0);
            aLivestock.lactation_length = query.value("lactation_length").toDouble(0);
            aLivestock.proportion_growth = query.value("proportion_growth").toDouble(0);
            aLivestock.lw_gain = query.value("lw_gain").toDouble(0);
            aLivestock.grazing_displacement = query.value("grazing_displacement").toDouble(0);            
            aLivestock.cp_maintenance = query.value("cp_maintenance").toDouble(0);
            aLivestock.cp_grazing = query.value("cp_grazing").toDouble(0);
            aLivestock.cp_pregnancy = query.value("cp_pregnancy").toDouble(0);
            aLivestock.cp_lactation = query.value("cp_lactation").toDouble(0);
            aLivestock.cp_lactmilk = query.value("cp_lactmilk").toDouble(0);
            aLivestock.cp_growth = query.value("cp_growth").toDouble(0);
            aLivestock.birth_interval = query.value("birth_interval").toDouble(0);
            aLivestock.protein_milkcontent = query.value("protein_milkcontent").toDouble(0);
            aLivestock.fat_content = query.value("fat_content").toDouble(0);
            aLivestock.energy_milkcontent = query.value("energy_milkcontent").toDouble(0);
            aLivestock.energy_meatcontent = query.value("energy_meatcontent").toDouble(0);
            aLivestock.protein_meatcontent = query.value("protein_meatcontent").toDouble(0);
            aLivestock.carcass_fraction = query.value("carcass_fraction").toDouble(0);
            aLivestock.energy_eggcontent = query.value("energy_eggcontent").toDouble(0);
            aLivestock.n_content = query.value("n_content").toDouble(0);
            aLivestock.water_requirement = query.value("water_requirement").toDouble(0);
            aLivestock.meat_product = query.value("meat_product").toString();
            aLivestock.milk_product = query.value("milk_product").toString();
            aLivestock.oneoff_cost = query.value("oneoff_cost").toDouble(0);
            aLivestock.operational_cost = query.value("operational_cost").toDouble(0);
            aLivestock.oneoff_labour = query.value("oneoff_labour").toDouble(0);
            aLivestock.operational_labour = query.value("operational_labour").toDouble(0);
            aLivestock.meat_price = query.value("meat_price").toDouble(0);
            aLivestock.milk_price = query.value("milk_price").toDouble(0);
            aLivestock.ipcc_meth_ef_t1 = 0;
            if (region == "AFRICA")
                aLivestock.ipcc_meth_ef_t1 = query.value("ipcc_meth_ef_t1_afri").toDouble(0);
            if (region == "LATIN")
                aLivestock.ipcc_meth_ef_t1 = query.value("ipcc_meth_ef_t1_lati").toDouble(0);
            if (region == "ASIA")
                aLivestock.ipcc_meth_ef_t1 = query.value("ipcc_meth_ef_t1_asia").toDouble(0);
            aLivestock.ipcc_meth_ef_t2 = query.value("ipcc_ef_category_t2").toDouble(0);
            aLivestock.ipcc_meth_man = 0;
            if (region == "AFRICA")
                aLivestock.ipcc_meth_man = query.value("ipcc_meth_man_afri").toDouble(0);
            if (region == "LATIN")
                aLivestock.ipcc_meth_man = query.value("ipcc_meth_man_lati").toDouble(0);
            if (region == "ASIA")
                aLivestock.ipcc_meth_man = query.value("ipcc_meth_man_asia").toDouble(0);
            aLivestock.ipcc_meth_exc = 0;
            if (region == "AFRICA")
                aLivestock.ipcc_meth_exc = query.value("ipcc_meth_exc_afri").toDouble(0);
            if (region == "LATIN")
                aLivestock.ipcc_meth_exc = query.value("ipcc_meth_exc_lati").toDouble(0);
            if (region == "ASIA")
                aLivestock.ipcc_meth_exc = query.value("ipcc_meth_exc_asia").toDouble(0);

            aLivestock.manureman_non_roofed_enclosure = "Solid storage";
            aLivestock.manureman_offfarm_grazing = "Solid storage";
            aLivestock.manureman_onfarm_grazing = "Solid storage";
            aLivestock.manureman_stable = "Solid storage";
            aLivestock.manure_onfarm_fraction = 0;
            aLivestock.manure_sales_fraction = 0;
            aLivestock.body_weight_weaning = 0;
            aLivestock.body_weight_year_one = 0;
            aLivestock.adult_weight = 0;
            aLivestock.work_hour = 0;
            aLivestock.piglets_relying_on_milk = 0;


            items.append(aLivestock);
        }
        this->endResetModel();
    }
}

QVariant livestockModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return "   ";
        }
        if (orientation == Qt::Horizontal)
        {
            if (section == 0) return "Category";
            if (section == 1) return "Herd composition (nr)";
            if (section == 2) return "Average annual milk (kg)";
            if (section == 3) return "Average annual wool (kg)";
            if (section == 4) return "Average annual growth per animal (kg)";
            if (section == 5) return "Time spent in stable (fraction of day)";
            if (section == 6) return "Collection of manure in stable (fraction)";
            if (section == 7) return "Manure management in stable";
            if (section == 8) return "Time spent in non-roofed enclosure  (fraction of day)";
            if (section == 9) return "Collection of manure in non-roofed enclosure (fraction)";
            if (section == 10) return "Manure management in non-roofed enclosure";
            if (section == 11) return "Time spent grazing  pasture/fields on-farm (fraction of day)";
            if (section == 12) return "Collection of manure in fields/pasture on-farm (fraction)";
            if (section == 13) return "Manure management during on-farm grazing";
            if (section == 14) return "Time spent grazing off-farm (fraction of day)";
            if (section == 15) return "Manure management during off-farm grazing";
            if (section == 16) return "Distance stable / enclosure to pasture (km)";
            if (section == 17) return "Fraction of manure coming from the farm";
            if (section == 18) return "Fraction of manure for sale";
            if (section == 19) return "Average Body weight (kg)";
            if (section == 20) return "The live bodyweight at weaning (kgs)";
            if (section == 21) return "The live bodyweight of sheep and goat at 1-year old or at slaughter (live-weight) if slaughtered prior to 1 year of age (kgs)";
            if (section == 22) return "The mature body weight of an adult animal (kgs)";
            if (section == 23) return "Number of hours of work per day by an animal";
            if (section == 24) return "Litter size  (pigs)";
            if (section == 25) return "Number of piglets relying on milk";
            if (section == 26) return "Lactation length (small ruminants/pigs, days)";
            if (section == 27) return "Proportion growth piglets covered by milk (%)";
            if (section == 28) return "LW gain piglets (kg/day)";
            if (section == 29) return "Grazing displacement (km/day)";
            if (section == 30) return "Crude Protein/Lysine requirement maintenance (kg/day)";
            if (section == 31) return "Crude Protein requirement grazing (kg/km)";
            if (section == 32) return "Crude Protein/ Lysine requirement pregnancy (kg)";
            if (section == 33) return "Crude Protein/ Lysine requirement lactation (kg/ lactation)";
            if (section == 34) return "Crude Protein requirement  (kg/kg milk)";
            if (section == 35) return "Crude Protein/ Lysine requirement growth (kg/kg LWG)";
            if (section == 36) return "Parturition interval (years)";
            if (section == 37) return "Protein content milk (%)";
            if (section == 38) return "Fat content milk (%)";
            if (section == 39) return "Energy content milk";
            if (section == 40) return "Energy content meat";
            if (section == 41) return "Protein content meat (%)";
            if (section == 42) return "Carcass fraction";
            if (section == 43) return "Energy content eggs";
            if (section == 44) return "Average N content manure";
            if (section == 45) return "water requirement (litres/day)";
            if (section == 46) return "Meat product";
            if (section == 47) return "Milk product";
            if (section == 48) return "One-off costs (USD/animal)";
            if (section == 49) return "Operational costs (USD per animal year)";
            if (section == 50) return "Labour one-off per animal (days/yr)";
            if (section == 51) return "Labour (days/animal/year)";
            if (section == 52) return "Meat price (USD/kg)";
            if (section == 53) return "Milk price (USD/kg)";
            if (section == 54) return "Methane emissions enteric fermentation - Tier 1";
            if (section == 55) return "Methane emissions enteric fermentation - Tier 2";
            if (section == 56) return "Methane emissions manure - Tier 1";
            if (section == 57) return "Default N-excretion rates Tier 1";
        }
    }
    return QVariant();
}

QVariant livestockModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0) return items[index.row()].livetype_desc;
    }
    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        if (index.column() == 1) return items[index.row()].herd_composition;
        if (index.column() == 2) return items[index.row()].annual_milk;
        if (index.column() == 3) return items[index.row()].annual_wool;
        if (index.column() == 4) return items[index.row()].annual_growth;
        if (index.column() == 5) return items[index.row()].time_in_stable;
        if (index.column() == 6) return items[index.row()].manure_in_stable;
        if (index.column() == 7) return items[index.row()].manureman_stable;
        if (index.column() == 8) return items[index.row()].time_in_non_roofed_enclosure;
        if (index.column() == 9) return items[index.row()].manure_in_non_roofed_enclosure;
        if (index.column() == 10) return items[index.row()].manureman_non_roofed_enclosure;
        if (index.column() == 11) return items[index.row()].time_in_onfarm_grazing;
        if (index.column() == 12) return items[index.row()].manure_in_field;
        if (index.column() == 13) return items[index.row()].manureman_onfarm_grazing;
        if (index.column() == 14) return items[index.row()].time_in_offfarm_grazing;
        if (index.column() == 15) return items[index.row()].manureman_offfarm_grazing;
        if (index.column() == 16) return items[index.row()].distance_to_pasture;
        if (index.column() == 17) return items[index.row()].manure_onfarm_fraction;
        if (index.column() == 18) return items[index.row()].manure_sales_fraction;
        if (index.column() == 19) return items[index.row()].body_weight;
        if (index.column() == 20) return items[index.row()].body_weight_weaning;
        if (index.column() == 21) return items[index.row()].body_weight_year_one;
        if (index.column() == 22) return items[index.row()].adult_weight;
        if (index.column() == 23) return items[index.row()].work_hour;
        if (index.column() == 24) return items[index.row()].litter_size;
        if (index.column() == 25) return items[index.row()].piglets_relying_on_milk;
        if (index.column() == 26) return items[index.row()].lactation_length;
        if (index.column() == 27) return items[index.row()].proportion_growth;
        if (index.column() == 28) return items[index.row()].lw_gain;
        if (index.column() == 29) return items[index.row()].grazing_displacement;
        if (index.column() == 30) return items[index.row()].cp_maintenance;
        if (index.column() == 31) return items[index.row()].cp_grazing;
        if (index.column() == 32) return items[index.row()].cp_pregnancy;
        if (index.column() == 33) return items[index.row()].cp_lactation;
        if (index.column() == 34) return items[index.row()].cp_lactmilk;
        if (index.column() == 35) return items[index.row()].cp_growth;
        if (index.column() == 36) return items[index.row()].birth_interval;
        if (index.column() == 37) return items[index.row()].protein_milkcontent;
        if (index.column() == 38) return items[index.row()].fat_content;
        if (index.column() == 39) return items[index.row()].energy_milkcontent;
        if (index.column() == 40) return items[index.row()].energy_meatcontent;
        if (index.column() == 41) return items[index.row()].protein_meatcontent;
        if (index.column() == 42) return items[index.row()].carcass_fraction;
        if (index.column() == 43) return items[index.row()].energy_eggcontent;
        if (index.column() == 44) return items[index.row()].n_content;
        if (index.column() == 45) return items[index.row()].water_requirement;
        if (index.column() == 46) return items[index.row()].meat_product;
        if (index.column() == 47) return items[index.row()].milk_product;
        if (index.column() == 48) return items[index.row()].oneoff_cost;
        if (index.column() == 49) return items[index.row()].operational_cost;
        if (index.column() == 50) return items[index.row()].oneoff_labour;
        if (index.column() == 51) return items[index.row()].operational_labour;
        if (index.column() == 52) return items[index.row()].meat_price;
        if (index.column() == 53) return items[index.row()].milk_price;
        if (index.column() == 54) return items[index.row()].ipcc_meth_ef_t1;
        if (index.column() == 55) return items[index.row()].ipcc_meth_ef_t2;
        if (index.column() == 56) return items[index.row()].ipcc_meth_man;
        if (index.column() == 57) return items[index.row()].ipcc_meth_exc;
    }
    if (role == Qt::BackgroundRole)
    {        
        if (index.column() > 0 && index.column() < 19)
        {            
            QBrush Background(QColor(169,208,142)); //We can change this to a nice color
            return Background;
        }
        if (index.column() >= 19)
        {            
            QBrush Background(QColor(244,176,132)); //We can change this to a nice color
            return Background;
        }

    }
    if (role == Qt::ForegroundRole)
    {
        if (index.column() > 0)
        {
            QBrush Background(QColor("black")); //We can change this to a nice color
            return Background;
        }
    }

    return QVariant();
}

bool livestockModel::setData(const QModelIndex &index,const QVariant &value,int role)
{
    if (role == Qt::EditRole)
    {
        if (index.column() > 0)
        {
            if (index.column() != 7 && index.column() != 10 && index.column() != 13 && index.column() != 15 && index.column() != 46 && index.column() != 47)
            {
                bool ok;
                double newValue = value.toDouble(&ok);
                if (ok)
                {
                    if (index.column() == 1) items[index.row()].herd_composition = newValue;
                    if (index.column() == 2) items[index.row()].annual_milk = newValue;
                    if (index.column() == 3) items[index.row()].annual_wool = newValue;
                    if (index.column() == 4) items[index.row()].annual_growth = newValue;
                    if (index.column() == 5) items[index.row()].time_in_stable = newValue;
                    if (index.column() == 6) items[index.row()].manure_in_stable = newValue;
                    if (index.column() == 8) items[index.row()].time_in_non_roofed_enclosure = newValue;
                    if (index.column() == 9) items[index.row()].manure_in_non_roofed_enclosure = newValue;
                    if (index.column() == 11) items[index.row()].time_in_onfarm_grazing = newValue;
                    if (index.column() == 12) items[index.row()].manure_in_field = newValue;
                    if (index.column() == 14) items[index.row()].time_in_offfarm_grazing = newValue;
                    if (index.column() == 16) items[index.row()].distance_to_pasture = newValue;
                    if (index.column() == 17) items[index.row()].manure_onfarm_fraction = newValue;
                    if (index.column() == 18) items[index.row()].manure_sales_fraction = newValue;
                    if (index.column() == 19) items[index.row()].body_weight = newValue;
                    if (index.column() == 20) items[index.row()].body_weight_weaning = newValue;
                    if (index.column() == 21) items[index.row()].body_weight_year_one = newValue;
                    if (index.column() == 22) items[index.row()].adult_weight = newValue;
                    if (index.column() == 23) items[index.row()].work_hour = newValue;
                    if (index.column() == 24) items[index.row()].litter_size = newValue;
                    if (index.column() == 25) items[index.row()].piglets_relying_on_milk = newValue;
                    if (index.column() == 26) items[index.row()].lactation_length = newValue;
                    if (index.column() == 27) items[index.row()].proportion_growth = newValue;
                    if (index.column() == 28) items[index.row()].lw_gain = newValue;
                    if (index.column() == 29) items[index.row()].grazing_displacement = newValue;
                    if (index.column() == 30) items[index.row()].cp_maintenance = newValue;
                    if (index.column() == 31) items[index.row()].cp_grazing = newValue;
                    if (index.column() == 32) items[index.row()].cp_pregnancy = newValue;
                    if (index.column() == 33) items[index.row()].cp_lactation = newValue;
                    if (index.column() == 34) items[index.row()].cp_lactmilk = newValue;
                    if (index.column() == 35) items[index.row()].cp_growth = newValue;
                    if (index.column() == 36) items[index.row()].birth_interval = newValue;
                    if (index.column() == 37) items[index.row()].protein_milkcontent = newValue;
                    if (index.column() == 38) items[index.row()].fat_content = newValue;
                    if (index.column() == 39) items[index.row()].energy_milkcontent = newValue;
                    if (index.column() == 40) items[index.row()].energy_meatcontent = newValue;
                    if (index.column() == 41) items[index.row()].protein_meatcontent = newValue;
                    if (index.column() == 42) items[index.row()].carcass_fraction = newValue;
                    if (index.column() == 43) items[index.row()].energy_eggcontent = newValue;
                    if (index.column() == 44) items[index.row()].n_content = newValue;
                    if (index.column() == 45) items[index.row()].water_requirement = newValue;
                    if (index.column() == 48) items[index.row()].oneoff_cost = newValue;
                    if (index.column() == 49) items[index.row()].operational_cost = newValue;
                    if (index.column() == 50) items[index.row()].oneoff_labour = newValue;
                    if (index.column() == 51) items[index.row()].operational_labour = newValue;
                    if (index.column() == 52) items[index.row()].meat_price = newValue;
                    if (index.column() == 53) items[index.row()].milk_price = newValue;
                    if (index.column() == 54) items[index.row()].ipcc_meth_ef_t1 = newValue;
                    if (index.column() == 55) items[index.row()].ipcc_meth_ef_t2 = newValue;
                    if (index.column() == 56) items[index.row()].ipcc_meth_man = newValue;
                    if (index.column() == 57) items[index.row()].ipcc_meth_exc = newValue;
                    emit modelChanged();
                    return true;
                }
            }
            else
            {
                if (index.column() == 7)
                {
                    items[index.row()].manureman_stable = value.toString();
                    emit delegateChanged(index.column());
                }
                if (index.column() == 10)
                {
                    items[index.row()].manureman_non_roofed_enclosure = value.toString();
                    emit delegateChanged(index.column());
                }
                if (index.column() == 13)
                {
                    items[index.row()].manureman_onfarm_grazing = value.toString();
                    emit delegateChanged(index.column());
                }
                if (index.column() == 15)
                {
                    items[index.row()].manureman_offfarm_grazing = value.toString();
                    emit delegateChanged(index.column());
                }
                if (index.column() == 46)
                    items[index.row()].meat_product = value.toString();
                if (index.column() == 47)
                    items[index.row()].milk_product = value.toString();
                emit modelChanged();
                return true;
            }
        }
    }
    return false;
}

QString livestockModel::getLivestockCode(int row)
{
    if (row < items.count())
    {
        return items[row].livetype_code;
    }
    return "";
}
QString livestockModel::getLivestockDesc(int row)
{
    if (row < items.count())
    {
        return items[row].livetype_desc;
    }
    return "";
}

void livestockModel::deleteLivestock(int row)
{
    if (row < items.count())
    {
        this->beginResetModel();
        items.removeAt(row);
        this->endResetModel();
    }
}
