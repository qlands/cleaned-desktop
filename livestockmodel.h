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
    double er_maintenance;
    double er_grazing;
    double er_pregnancy;
    double er_lactation;
    double er_lactmilk;
    double er_growth;
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
    double oneoff_cost;
    double operational_cost;
    double oneoff_labour;
    double operational_labour;
    double meat_price;
    double milk_price;
    double ipcc_meth_ef_t1;
    double ipcc_meth_ef_t2;
    double ipcc_meth_man;
    double ipcc_meth_exc;
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
    void modelChanged();
private:
    QList <Tlivestock> items;
    QSqlDatabase db;
};

#endif // LIVESTOCKMODEL_H
