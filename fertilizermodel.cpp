#include "fertilizermodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>

fertilizerModel::fertilizerModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void fertilizerModel::setDataArray(QJsonArray data)
{
    this->beginResetModel();
    items.clear();
    for (int seasonIndex = 0; seasonIndex < data.size(); ++seasonIndex)
    {
         QJsonObject fertilizerObject = data[seasonIndex].toObject();
         Tfertilizer aFertilizer;
         aFertilizer.fertilizer_code = fertilizerObject["fertilizer_code"].toString();
         aFertilizer.fertilizer_desc = fertilizerObject["fertilizer_desc"].toString();
         aFertilizer.quantity = fertilizerObject["quantity"].toDouble(0);
         aFertilizer.cost = fertilizerObject["cost"].toDouble(0);
         aFertilizer.fraction = fertilizerObject["fraction"].toDouble(0);
         aFertilizer.percentage_n = fertilizerObject["percentage_n"].toDouble(0);
         items.append(aFertilizer);
    }
    this->endResetModel();
}

void fertilizerModel::setDatabase(QSqlDatabase cleaned_db)
{
    this->db = cleaned_db;
}

int fertilizerModel::columnCount(const QModelIndex &) const
{
    return 5;
}

int fertilizerModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

bool fertilizerModel::fertilizerExists(QString code)
{
    for (int i = 0; i < items.count(); i++)
    {
        if (items[i].fertilizer_code == code)
        {
            return true;
        }
    }
    return false;
}

Qt::ItemFlags fertilizerModel::flags(const QModelIndex &index) const
{
    if (index.column() > 0)
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QJsonArray fertilizerModel::getFertilizerArray()
{
    QJsonArray fertilizerArray;
    for (int i = 0; i < items.count(); i++)
    {
        QJsonObject fertilizerObject;
        fertilizerObject["fertilizer_code"] = items[i].fertilizer_code;
        fertilizerObject["fertilizer_desc"] = items[i].fertilizer_desc;
        fertilizerObject["quantity"] = items[i].quantity;
        fertilizerObject["cost"] = items[i].cost;
        fertilizerObject["fraction"] = items[i].fraction;
        fertilizerObject["percentage_n"] = items[i].percentage_n;
        fertilizerArray.append(fertilizerObject);
    }
    return fertilizerArray;
}

void fertilizerModel::addNewFertilizer(QString code)
{
    if (fertilizerExists(code))
        return;
    QSqlQuery query(this->db);
    if (query.exec("SELECT * FROM lkp_orgfertilizer WHERE fertilizer_code = '" + code + "'"))
    {
        this->beginResetModel();
        while (query.next())
        {
            Tfertilizer aFertilizer;
            aFertilizer.fertilizer_code = code;
            aFertilizer.fertilizer_desc = query.value("fertilizer_desc").toString();
            aFertilizer.quantity = 0;
            aFertilizer.cost = 0;
            aFertilizer.fraction = 0;
            aFertilizer.percentage_n = 0;
            items.append(aFertilizer);
        }
        this->endResetModel();
    }
}

QVariant fertilizerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return "   ";
        }
        if (orientation == Qt::Horizontal)
        {
            if (section == 0) return "Fertilizer";
            if (section == 1) return "Quantity (kg)";
            if (section == 2) return "Cost";
            if (section == 3) return "Fraction";
            if (section == 4) return "% N";
        }
    }
    return QVariant();
}

QVariant fertilizerModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0) return items[index.row()].fertilizer_desc;
    }
    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        if (index.column() == 1) return items[index.row()].quantity;
        if (index.column() == 2) return items[index.row()].cost;
        if (index.column() == 3) return items[index.row()].fraction;
        if (index.column() == 4) return items[index.row()].percentage_n;
    }
    return QVariant();
}

bool fertilizerModel::setData(const QModelIndex &index,const QVariant &value,int role)
{
    if (role == Qt::EditRole)
    {
        if (index.column() > 0)
        {
            bool ok;
            double newValue = value.toDouble(&ok);
            if (ok)
            {
                if (index.column() == 1) items[index.row()].quantity = newValue;
                if (index.column() == 2) items[index.row()].cost = newValue;
                if (index.column() == 3) items[index.row()].fraction = newValue;
                if (index.column() == 4) items[index.row()].percentage_n = newValue;
                emit modelChanged();
                return true;
            }
        }
    }
    return false;
}

QString fertilizerModel::getFertilizer(int row)
{
    if (row < items.count())
    {
        return items[row].fertilizer_desc;
    }
    return "";
}
void fertilizerModel::deleteFertilizer(int row)
{
    if (row < items.count())
    {
        this->beginResetModel();
        items.removeAt(row);
        this->endResetModel();
    }
}
