#include "cropinputsmodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QBrush>
#include <QColor>

cropInputsModel::cropInputsModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void cropInputsModel::setDataArray(QJsonArray data)
{
    this->beginResetModel();
    items.clear();
    for (int seasonIndex = 0; seasonIndex < data.size(); ++seasonIndex)
    {
         QJsonObject cropInputsObject = data[seasonIndex].toObject();
         TcropInputs aCropInput;
         aCropInput.feed_type_code = cropInputsObject["feed_type_code"].toString();
         aCropInput.feed_type_name = cropInputsObject["feed_type_name"].toString();
         aCropInput.feed_item_code = cropInputsObject["feed_item_code"].toString();
         aCropInput.feed_item_name = cropInputsObject["feed_item_name"].toString();

         aCropInput.fraction_as_fertilizer = cropInputsObject["fraction_as_fertilizer"].toDouble(0);

         aCropInput.urea = cropInputsObject["urea"].toDouble(0);
         aCropInput.npk = cropInputsObject["npk"].toDouble(0);
         aCropInput.dap = cropInputsObject["dap"].toDouble(0);
         aCropInput.ammonium_nitrate = cropInputsObject["ammonium_nitrate"].toDouble(0);
         aCropInput.ammonium_sulfate = cropInputsObject["ammonium_sulfate"].toDouble(0);
         aCropInput.n_solutions = cropInputsObject["n_solutions"].toDouble(0);
         aCropInput.ammonia = cropInputsObject["ammonia"].toDouble(0);         


         items.append(aCropInput);
    }
    this->endResetModel();
}

void cropInputsModel::setDatabase(QSqlDatabase cleaned_db)
{
    this->db = cleaned_db;
}

int cropInputsModel::columnCount(const QModelIndex &) const
{
    return 10;
}

int cropInputsModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

bool cropInputsModel::feedExists(QString crop, QString feed)
{
    for (int i = 0; i < items.count(); i++)
    {
        if ((items[i].feed_type_code == crop) && (items[i].feed_item_code == feed))
        {
            return true;
        }
    }
    return false;
}

Qt::ItemFlags cropInputsModel::flags(const QModelIndex &index) const
{
    if (index.column() > 1)
    {
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QJsonArray cropInputsModel::getFeedsArray()
{
    QJsonArray feedsArray;
    for (int i = 0; i < items.count(); i++)
    {
        QJsonObject feedObject;
        feedObject["feed_type_code"] = items[i].feed_type_code;
        feedObject["feed_item_code"] = items[i].feed_item_code;
        feedObject["fraction_as_fertilizer"] = items[i].fraction_as_fertilizer;        
        feedObject["urea"] = items[i].urea;
        feedObject["npk"] = items[i].npk;
        feedObject["dap"] = items[i].dap;
        feedObject["ammonium_nitrate"] = items[i].ammonium_nitrate;
        feedObject["ammonium_sulfate"] = items[i].ammonium_sulfate;
        feedObject["n_solutions"] = items[i].n_solutions;
        feedObject["ammonia"] = items[i].ammonia;        

        feedsArray.append(feedObject);
    }
    return feedsArray;
}

void cropInputsModel::addNewFeed(QString crop, QString feed)
{
    if (feedExists(crop, feed))
        return;
    QSqlQuery query(this->db);
    TcropInputs aCropInput;
    aCropInput.feed_type_code = crop;
    aCropInput.feed_item_code = feed;

    aCropInput.fraction_as_fertilizer = 0;    
    aCropInput.urea = 0;
    aCropInput.npk = 0;
    aCropInput.dap = 0;
    aCropInput.ammonium_nitrate = 0;
    aCropInput.ammonium_sulfate = 0;
    aCropInput.n_solutions = 0;
    aCropInput.ammonia = 0;


    if (query.exec("SELECT * FROM lkp_feeditem WHERE feed_type_code = '" + crop + "' AND feed_item_code = '" + feed + "'"))
    {
        if (query.first())
        {
            aCropInput.feed_item_name = query.value("feed_item_name").toString();
        }
    }
    if (query.exec("SELECT * FROM lkp_feedtype WHERE feed_type_code = '" + crop + "'"))
    {
        if (query.first())
        {
            aCropInput.feed_type_name = query.value("feed_type_name").toString();
        }
    }
    this->beginResetModel();
    items.append(aCropInput);
    this->endResetModel();
}


QVariant cropInputsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == 0) return "Crop";
            if (section == 1) return "Feed";
            if (section == 2) return "Fraction collected manure used as fertilizer";            
            if (section == 3) return "Urea (kg/ha)";
            if (section == 4) return "NPK (kg/ha)";
            if (section == 5) return "DAP (kg/ha)";
            if (section == 6) return "Ammonium nitrate (kg/ha)";
            if (section == 7) return "Ammonium sulfate (kg/ha)";
            if (section == 8) return "N solutions (kg/ha)";
            if (section == 9) return "Ammonia (kg/ha)";
        }
    }
    return QVariant();
}

QVariant cropInputsModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0) return items[index.row()].feed_type_name;
        if (index.column() == 1) return items[index.row()].feed_item_name;
    }
    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        if (index.column() == 2) return items[index.row()].fraction_as_fertilizer;        
        if (index.column() == 3) return items[index.row()].urea;
        if (index.column() == 4) return items[index.row()].npk;
        if (index.column() == 5) return items[index.row()].dap;
        if (index.column() == 6) return items[index.row()].ammonium_nitrate;
        if (index.column() == 7) return items[index.row()].ammonium_sulfate;
        if (index.column() == 8) return items[index.row()].n_solutions;
        if (index.column() == 9) return items[index.row()].ammonia;
    }
    if (role == Qt::BackgroundRole)
    {
        if ((index.column() >= 2) )
        {
            QBrush Background(QColor(169,209,142)); //We can change this to a nice color
            return Background;
        }
    }
    if (role == Qt::ForegroundRole)
    {
        if (index.column() >= 2)
        {
            QBrush Background(QColor("black")); //We can change this to a nice color
            return Background;
        }
    }

    return QVariant();
}

bool cropInputsModel::setData(const QModelIndex &index,const QVariant &value,int role)
{
    if (role == Qt::EditRole)
    {
        if (index.column() >= 2)
        {
            bool ok;
            double newValue = value.toDouble(&ok);
            if (ok)
            {
                if (index.column() == 2) items[index.row()].fraction_as_fertilizer = newValue;                
                if (index.column() == 3) items[index.row()].urea = newValue;
                if (index.column() == 4) items[index.row()].npk = newValue;
                if (index.column() == 5) items[index.row()].dap = newValue;
                if (index.column() == 6) items[index.row()].ammonium_nitrate = newValue;
                if (index.column() == 7) items[index.row()].ammonium_sulfate = newValue;
                if (index.column() == 8) items[index.row()].n_solutions = newValue;
                if (index.column() == 9) items[index.row()].ammonia = newValue;
                emit modelChanged();
                return true;
            }            
        }
    }
    return false;
}

void cropInputsModel::deleteFeed(int row)
{
    if (row < items.count())
    {
        this->beginResetModel();
        items.removeAt(row);
        this->endResetModel();
    }
}
