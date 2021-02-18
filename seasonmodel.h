#ifndef SEASONMODEL_H
#define SEASONMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>

struct seasonType
{
    QString season_name;
    int num_days;
};
typedef seasonType Tseason;

class seasonModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    seasonModel(QObject *parent=0);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,const QVariant &value,int role = Qt::EditRole);
    Qt::ItemFlags flags (const QModelIndex &) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void setDataArray(QJsonArray data);
    void addNewSeason(QString seasonName);
    void deleteSeason(int index);
    QJsonArray getSeasonArray();
    bool seasonExist(QString season);
    QString getSeason(int row);
signals:
    void modelChanged();
    void seasonChanged(QString fromSeason, QString toSeason);
private:
    bool isDuplicated(int row, QString season);
    QList <seasonType> items;
};

#endif // SEASONMODEL_H
