#ifndef DATABASEMODEL_H
#define DATABASEMODEL_H

#include <QAbstractTableModel>

struct databaseType
{
    QString code;
    QString name;
    QString path;
    bool removeable;
};
typedef databaseType TDatabase;

class databaseModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit databaseModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,const QVariant &value,int role = Qt::EditRole);
    Qt::ItemFlags flags (const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void loadDatabases(QString dbFile);
    bool isDBRemovable(int row);
    void saveDatabases();
    void setDBName(int row, QString name);
    void deleteDatabase(int row);
    bool addDatabase(QString code, QString name, QString path);
    QString getDBPath(int row);
    QString getDBCode(int row);
    QString getDBName(int row);

    QString databaseFile;
    QList <TDatabase> items;
};

#endif // DATABASEMODEL_H
