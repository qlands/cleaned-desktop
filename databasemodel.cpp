#include "databasemodel.h"
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

databaseModel::databaseModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

int databaseModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

int databaseModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant databaseModel::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        if (index.column() == 0) return items[index.row()].code;
        if (index.column() == 1) return items[index.row()].name;
        if (index.column() == 2) return items[index.row()].path;
    }
    return QVariant();
}

bool databaseModel::setData(const QModelIndex &index,const QVariant &value,int role)
{
    if (role == Qt::EditRole)
    {
        if (index.column() == 1)
        {
            if (value.toString().simplified() != "")
            {
                if (items[index.row()].removeable == true)
                {
                    items[index.row()].name = value.toString();
                    saveDatabases();
                    return true;
                }
            }
        }
    }
    return false;
}

Qt::ItemFlags databaseModel::flags (const QModelIndex &index) const
{
    if (index.column() != 1)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
    {
        if (items[index.row()].removeable == true)
            return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
        else
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

QVariant databaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Vertical)
        {
            return "   ";
        }
        if (orientation == Qt::Horizontal)
        {
            if (section == 0) return "Code";
            if (section == 1) return "Name";
            if (section == 2) return "Path";
        }
    }
    return QVariant();
}

void databaseModel::loadDatabases(QString dbFile)
{
    QFile loadFile(dbFile);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open database file.");
        return;
    }
    databaseFile = dbFile;
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject databases = loadDoc.object();
    QString databaseCode;
    foreach(databaseCode, databases.keys())
    {
        TDatabase a_item;
        QJsonObject a_database = databases[databaseCode].toObject();
        a_item.code = databaseCode;
        a_item.name = a_database["name"].toString();
        a_item.path = a_database["path"].toString();
        a_item.removeable = a_database["removeable"].toBool();
        items.append(a_item);
    }
    loadFile.close();
}

bool databaseModel::isDBRemovable(int row)
{
    return items[row].removeable;
}

void databaseModel::deleteDatabase(int row)
{
    this->beginResetModel();
    if (items[row].removeable == true)
    {
        items.removeAt(row);
    }
    this->endResetModel();
    saveDatabases();
}

void databaseModel::setDBName(int row, QString name)
{
    this->beginResetModel();
    if (items[row].removeable == false)
    {
        items[row].name = name;
    }
    this->endResetModel();
    saveDatabases();
}

bool databaseModel::addDatabase(QString code, QString name, QString path)
{
    TDatabase a_item;
    foreach(a_item, items)
    {
        if (a_item.code == code)
            return false;
    }
    this->beginResetModel();
    a_item.code = code;
    a_item.name = name;
    a_item.path = path;
    a_item.removeable = true;
    items.append(a_item);
    saveDatabases();
    this->endResetModel();
    return true;
}

QString databaseModel::getDBPath(int row)
{
    return items[row].path;
}

QString databaseModel::getDBCode(int row)
{
    return items[row].code;
}
QString databaseModel::getDBName(int row)
{
    return items[row].name;
}

void databaseModel::saveDatabases()
{
    TDatabase a_item;
    QJsonObject databases;
    foreach(a_item, items)
    {
        QJsonObject dbData;
        dbData["name"] = a_item.name;
        dbData["path"] = a_item.path;
        dbData["removeable"] = a_item.removeable;
        databases[a_item.code] = dbData;
    }

    QFile saveFile(databaseFile);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't save file.");
    }
    saveFile.write(QJsonDocument(databases).toJson());
    saveFile.close();
}
