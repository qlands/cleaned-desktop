#ifndef CROPINPUTSMODEL_H
#define CROPINPUTSMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>

struct cropInputsType
{
    QString feed_type_code;
    QString feed_type_name;
    QString feed_item_code;
    QString feed_item_name;
    double fraction_as_fertilizer;
    double urea;
    double npk;
    double dap;
    double ammonium_nitrate;
    double ammonium_sulfate;
    double n_solutions;
    double ammonia;
    double n_fertilizer;
};
typedef cropInputsType TcropInputs;

class cropInputsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    cropInputsModel(QObject *parent=0);
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
    void deleteFeed(int row);
signals:
    void delegateChanged(int column);
    void modelChanged();
private:
    QList <TcropInputs> items;
    QSqlDatabase db;
};

#endif // CROPINPUTSMODEL_H
