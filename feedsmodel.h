#ifndef FEEDSMODEL_H
#define FEEDSMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>

struct feedType
{
    QString feed_type_code;
    QString feed_type_name;
    QString feed_item_code;
    QString feed_item_name;
    int intercrop;
    double intercrop_fraction;
    double cut_carry_fraction;
    QString land_cover;
    double landcover_c_factor;
    QString slope;
    double slope_p_factor;
    double slope_length;
    QString grassman;
    double grassman_change_factor;
    double main_product_removal;
    double residue_removal;
    double residue_burnt;
    double dm_content;
    double me_content;
    double me_fresh;
    double cp_content;
    double cp_fresh;
    double de;
    double establishment_cost;
    double operational_cost;
    double establishment_labour;
    double operational_labour;
    double wfp_green;
    double wfp_blue;
    double wfp_grey;
    double emission_factor;
    double fresh_yield;
    double dm_fraction;
    double dry_yield;
    double harvest_index;
    double residue_fresh_yield;
    double residue_dm_content;
    double residue_dry_yield;
    double residue_n;
    double main_n;
    double residue_n_dm;
    double c_factor;
    double n_fixation;
    double energy;
    double water_content;
    double energy_dm;
    double usda_value;
    double kc_initial;
    double kc_midseason;
    double kc_late;
    double crop_coefficient;
    QString category;
    double trees_ha;
    double trees_dhb;
    double trees_growth;
    double trees_removal;
};
typedef feedType Tfeed;


class feedsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    feedsModel(QObject *parent=0);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,const QVariant &value,int role = Qt::EditRole);
    Qt::ItemFlags flags (const QModelIndex &index) const;
    void setDatabase(QSqlDatabase cleaned_db);
    void setDataArray(QJsonArray data);
    void addNewFeed(QString crop, QString feed);
    bool feedExists(QString crop, QString feed);
    QJsonArray getFeedsArray();
    QString getCropCode(int row);
    QString getFeedCode(int row);
    QString getFeedDesc(int row);
    void deleteFeed(int row);
signals:
    void delegateChanged(int column);
    void modelChanged();
private:
    QList <Tfeed> items;
    QSqlDatabase db;
};

#endif // FEEDSMODEL_H
