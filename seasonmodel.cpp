#include "seasonmodel.h"
#include <QJsonObject>

seasonModel::seasonModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void seasonModel::setDataArray(QJsonArray data)
{
    this->beginResetModel();
    items.clear();
    for (int seasonIndex = 0; seasonIndex < data.size(); ++seasonIndex)
    {
         QJsonObject seasonObject = data[seasonIndex].toObject();
         Tseason aSeason;
         aSeason.season_name = seasonObject["season_name"].toString();
         aSeason.num_days = seasonObject["season_length"].toInt(-1);
         items.append(aSeason);
    }
    this->endResetModel();
}

bool seasonModel::seasonExist(QString season)
{
    for (int i = 0; i < items.count(); i++)
    {
        if (items[i].season_name == season)
        {
            return true;
        }
    }
    return false;
}

void seasonModel::addNewSeason(QString seasonName)
{    
    if (seasonExist(seasonName))
        return;

    int numDays = 365;
    for (int i = 0; i < items.count(); i++)
    {
        numDays = numDays - items[i].num_days;
    }
    this->beginResetModel();
    Tseason aSeason;
    aSeason.season_name = seasonName;
    aSeason.num_days = numDays;
    items.append(aSeason);
    this->endResetModel();
}

void seasonModel::deleteSeason(int index)
{
    this->beginResetModel();
    items.removeAt(index);
    this->endResetModel();
}

QJsonArray seasonModel::getSeasonArray()
{
    QJsonArray seasonArray;
    for (int i = 0; i < items.count(); i++)
    {
        QJsonObject seasonObject;
        seasonObject["season_name"] = items[i].season_name;
        seasonObject["season_length"] = items[i].num_days;
        seasonArray.append(seasonObject);
    }
    return seasonArray;
}

int seasonModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

int seasonModel::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant seasonModel::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        if (index.column() == 0) return items[index.row()].season_name;
        if (index.column() == 1) return items[index.row()].num_days;
    }
    return QVariant();
}

bool seasonModel::isDuplicated(int row, QString season)
{
    for (int i=0; i < items.count(); i++ )
    {
        if ((items[i].season_name == season) && (row != i))
            return true;
    }
    return false;
}

bool seasonModel::setData(const QModelIndex &index,const QVariant &value,int role)
{
    if (role == Qt::EditRole)
    {
        if (index.column() == 0)
        {
            if (!isDuplicated(index.row(), value.toString()))
            {
                emit seasonChanged(items[index.row()].season_name, value.toString());
                items[index.row()].season_name = value.toString();
                return true;
            }
        }
        if (index.column() == 1)
        {
            bool ok;
            items[index.row()].num_days = value.toInt(&ok);
            if (ok)
            {               
                emit modelChanged();
                return true;
            }
        }
    }
    return false;
}

Qt::ItemFlags seasonModel::flags (const QModelIndex &) const
{
    return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant seasonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return "   ";
        }
        if (orientation == Qt::Horizontal)
        {
            if (section == 0) return "Season name";
            if (section == 1) return "Number of days";
        }
    }
    return QVariant();
}


QString seasonModel::getSeason(int row)
{
    if (row < items.count())
        return items[row].season_name;
    return "";
}
