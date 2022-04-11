#ifndef FERTILIZERMODEL_H
#define FERTILIZERMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>

struct fertilizerType
{
    QString fertilizer_code;
    QString fertilizer_desc;
    double quantity;
    double cost;
    double fraction;
    double percentage_n;
};
typedef fertilizerType Tfertilizer;

class fertilizerModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    fertilizerModel(QObject *parent=0);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,const QVariant &value,int role = Qt::EditRole);
    Qt::ItemFlags flags (const QModelIndex &index) const;
    void setDatabase(QSqlDatabase cleaned_db);
    void setDataArray(QJsonArray data);
    void addNewFertilizer(QString code);
    bool fertilizerExists(QString code);
    QJsonArray getFertilizerArray();
    QString getFertilizer(int row);
    void deleteFertilizer(int row);
signals:
    void modelChanged();
private:
    QList <Tfertilizer> items;
    QSqlDatabase db;
};

#endif // FERTILIZERMODEL_H
