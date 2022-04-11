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
    QString source_type;
    int intercrop;
    double intercrop_fraction;
    double cut_carry_fraction;
    QString land_cover;
    QString land_cover_desc;
    double landcover_c_factor;
    QString slope;
    QString slope_desc;
    double slope_p_factor;
    double slope_length;
    QString grassman;
    QString grassman_desc;
    double grassman_change_factor;
    double main_product_removal;
    double residue_removal;
    double residue_burnt;
    double dm_content;
    double me_content;    
    double cp_content;    
    double establishment_cost;
    double operational_cost;
    double establishment_labour;
    double operational_labour;
    double wfp_green;
    double wfp_blue;
    double wfp_grey;
    double emission_factor;    
    double dm_fraction;
    double harvest_index;        
    double main_n;
    //double c_factor;
    double energy;
    double water_content;    
    double usda_value;
    double kc_initial;
    double kc_midseason;
    double kc_late;
    QString category;
    double trees_ha;
    double trees_dhb;
    double trees_growth;
    double trees_removal;
    double n_content;
    double residue_n;
    double dry_yield;
    double residue_dry_yield;
    double trees_ha_dbh25;
    double average_dbh25;
    double increase_dbh25;
    double trees_ha_dbh2550;
    double average_dbh2550;
    double increase_dbh2550;
    double trees_ha_dbh50;
    double average_dbh50;
    double increase_dbh50;
    double time_horizon;
    double diameter_breast;
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
