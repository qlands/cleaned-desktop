#ifndef LIVESTOCKMODEL_H
#define LIVESTOCKMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>

struct livestockType
{
    QString livetype_code;
    QString livetype_desc;
    double herd_composition;
    double annual_milk;
    double annual_wool;
    double annual_growth;    
    double time_in_stable;
    double time_in_non_roofed_enclosure;
    double time_in_onfarm_grazing;
    double time_in_offfarm_grazing;
    double distance_to_pasture;
    double manure_in_stable;
    double manure_in_non_roofed_enclosure;
    double manure_in_field;
    double body_weight;
    double litter_size;
    double lactation_length;
    double proportion_growth;
    double lw_gain;
    double grazing_displacement;
    double cp_maintenance;
    double cp_grazing;
    double cp_pregnancy;
    double cp_lactation;
    double cp_lactmilk;
    double cp_growth;
    double birth_interval;
    double protein_milkcontent;
    double fat_content;
    double energy_milkcontent;
    double energy_meatcontent;
    double protein_meatcontent;
    double carcass_fraction;
    double energy_eggcontent;
    double n_content;
    double water_requirement;
    QString meat_product;
    QString milk_product;
    //double oneoff_cost;
   // double operational_cost;
    //double oneoff_labour;
    //double operational_labour;
   // double meat_price;
    //double milk_price;
    QString ipcc_meth_ef_t1;
    QString ipcc_meth_ef_t2;
    QString ipcc_meth_man;
    QString ipcc_meth_exc;

    QString manureman_non_roofed_enclosure;
    QString manureman_offfarm_grazing;
    QString manureman_onfarm_grazing;
    QString manureman_stable;
    double manure_onfarm_fraction;
    double manure_sales_fraction;
    double body_weight_weaning;
    double body_weight_year_one;
    double adult_weight;
    double work_hour;
    double piglets_relying_on_milk;

    double me_maintenance;
    double me_grazing;
    double me_pregnancy;
    double me_lactation;
    double me_lactmilk;
    double me_growth;

};
typedef livestockType Tlivestock;

class livestockModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    livestockModel(QObject *parent=0);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,const QVariant &value,int role = Qt::EditRole);
    Qt::ItemFlags flags (const QModelIndex &index) const;
    void setDatabase(QSqlDatabase cleaned_db);
    void setDataArray(QJsonArray data);
    void addNewLivestock(QString code, QString region);
    bool livestockExists(QString code);
    QJsonArray getLivestockArray();
    QString getLivestockCode(int row);
    QString getLivestockDesc(int row);
    void deleteLivestock(int row);    
signals:
    void delegateChanged(int column);
    void modelChanged();
private:
    QList <Tlivestock> items;
    QSqlDatabase db;
};

#endif // LIVESTOCKMODEL_H
