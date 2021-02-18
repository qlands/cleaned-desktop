#ifndef BASKETMODEL_H
#define BASKETMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>

struct basketLivestockType
{
    QString livetype_code;
    double allocation;
};
typedef basketLivestockType TbasketLivestock;

struct basketFeedType
{
    QString feed_type_code;
    QString feed_item_code;
    QList <TbasketLivestock> livestock;
};
typedef basketFeedType TbasketFeed;

struct basketSeasonType
{
    QString season_name;
    QList <TbasketFeed> feeds;
};
typedef basketSeasonType TbasketSeason;

struct basketLineType
{
    int lineType;
    QString season_name;
    QString feed_type_code;
    QString feed_item_code;
    QString lineDescription;
};
typedef basketLineType TbasketLine;


class basketModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    basketModel(QObject *parent=0);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,const QVariant &value,int role = Qt::EditRole);
    Qt::ItemFlags flags (const QModelIndex &index) const;
    void setDatabase(QSqlDatabase cleaned_db);
    void setDataArray(QJsonArray data);
    void addNewLivestock(QString code, bool resetModel=true);
    void removeLivestock(QString code);
    bool livestockExists(QString code);
    void addNewSeason(QString code, bool resetModel=true);
    void removeSeason(QString code);
    void addNewFeed(QString crop, QString feed, bool resetModel=true);
    void removeFeed(QString crop, QString feed);
    bool seasonExists(QString code);
    bool feedExists(QString crop, QString feed);
    QJsonArray getBasketArray();
    void changeSeasonName(QString fromSeason, QString toSeason);    
private:
    void rebuildItems();
    double getAllocation(QString season, QString crop, QString feed, QString livestock) const;
    void setAllocation(QString season, QString crop, QString feed, QString livestock, double value);
    double getTotalAllocation(QString season, QString livestock) const;
    QList <TbasketSeason> basket;
    QList <TbasketLine> items;
    QStringList seasons;
    QStringList feeds;
    QStringList livestock;
    QSqlDatabase db;
signals:
    void modelChanged();
};

#endif // BASKETMODEL_H
