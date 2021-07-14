#include "basketmodel.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QBrush>
#include <QColor>
#include <QDebug>

basketModel::basketModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void basketModel::setDataArray(QJsonArray data)
{
    this->beginResetModel();
    items.clear();
    bool rebuild = false;
    for (int seasonIndex = 0; seasonIndex < data.count(); ++seasonIndex)
    {
         QJsonObject seasonObject = data[seasonIndex].toObject();

         if (!seasonExists(seasonObject["season_name"].toString()))
         {
             rebuild = true;
             break;
         }

         TbasketSeason aSeason;
         aSeason.season_name = seasonObject["season_name"].toString();
         if (seasons.indexOf(seasonObject["season_name"].toString()) == -1)
            seasons.append(seasonObject["season_name"].toString());

         TbasketLine aSeasonLine;
         aSeasonLine.lineType = 1;
         aSeasonLine.season_name = seasonObject["season_name"].toString();
         aSeasonLine.lineDescription = seasonObject["season_name"].toString();
         items.append(aSeasonLine);
         QJsonArray feedArray = seasonObject["feeds"].toArray();
         for (int feedIndex = 0; feedIndex < feedArray.count(); feedIndex++)
         {
             QJsonObject feedObject = feedArray[feedIndex].toObject();

             if (!feedExists(feedObject["feed_type_code"].toString(), feedObject["feed_item_code"].toString()))
             {
                 rebuild = true;
                 break;
             }

             TbasketFeed aFeed;
             aFeed.feed_type_code = feedObject["feed_type_code"].toString();
             aFeed.feed_item_code = feedObject["feed_item_code"].toString();
             if (feeds.indexOf(feedObject["feed_type_code"].toString() + "|" + feedObject["feed_item_code"].toString()) == -1)
                feeds.append(feedObject["feed_type_code"].toString() + "|" + feedObject["feed_item_code"].toString());

             TbasketLine aFeedLine;
             aFeedLine.lineType = 2;
             aFeedLine.season_name = seasonObject["season_name"].toString();
             aFeedLine.feed_type_code = feedObject["feed_type_code"].toString();
             aFeedLine.feed_item_code = feedObject["feed_item_code"].toString();
             QString feed_type_name;
             QSqlQuery query(this->db);
             if (query.exec("SELECT feed_type_name FROM lkp_feedtype WHERE feed_type_code = '" + feedObject["feed_type_code"].toString() + "'"))
             {
                 if (query.first())
                 {
                     feed_type_name = query.value(0).toString();
                 }
             }
             QString feed_item_name;
             if (query.exec("SELECT feed_item_name FROM lkp_feeditem WHERE feed_type_code = '" + feedObject["feed_type_code"].toString() + "' AND feed_item_code = '" + feedObject["feed_item_code"].toString()+ "'"))
             {
                 if (query.first())
                 {
                     feed_item_name = query.value(0).toString();
                 }
             }
             aFeedLine.lineDescription = feed_item_name + " of " + feed_type_name;
             items.append(aFeedLine);

             QJsonArray livestockArray = feedObject["livestock"].toArray();
             for (int livestockIndex = 0; livestockIndex < livestockArray.count(); livestockIndex++)
             {
                 QJsonObject livestockObject = livestockArray[livestockIndex].toObject();
                 if (!livestockExists(livestockObject["livetype_code"].toString()))
                 {
                     rebuild = true;
                     break;
                 }
                 if( livestock.indexOf(livestockObject["livetype_code"].toString()) == -1)
                    livestock.append(livestockObject["livetype_code"].toString());
                 TbasketLivestock aLivestock;
                 aLivestock.livetype_code = livestockObject["livetype_code"].toString();
                 aLivestock.allocation = livestockObject["allocation"].toDouble(0);
                 aFeed.livestock.append(aLivestock);
             }
             if (aFeed.livestock.count() != livestock.count())
                 rebuild = true;
             if (rebuild)
                 break;
             aSeason.feeds.append(aFeed);
         }
         if (rebuild)
         {
             break;
         }

         TbasketLine aSummaryLine;
         aSummaryLine.lineType = 3;
         aSummaryLine.season_name = seasonObject["season_name"].toString();
         aSummaryLine.lineDescription = "Total allocation";
         items.append(aSummaryLine);

         TbasketLine aSeparationLine;
         aSeparationLine.lineType = 4;
         aSeparationLine.season_name = seasonObject["season_name"].toString();
         aSeparationLine.lineDescription = "";
         items.append(aSeparationLine);
         if (aSeason.feeds.count() != feeds.count())
         {
             rebuild = true;
             break;
         }
         basket.append(aSeason);
    }
    this->endResetModel();
    if (rebuild)
    {
        qDebug() << "Rebuilding";
        this->basket.clear();
        for (int season=0; season < seasons.count(); season++)
        {
            TbasketSeason aSeason;
            aSeason.season_name = seasons[season];
            for (int feedIndex = 0; feedIndex < feeds.count(); feedIndex++)
            {
                TbasketFeed aFeed;
                QStringList parts = feeds[feedIndex].split("|");
                aFeed.feed_type_code = parts[0];
                aFeed.feed_item_code = parts[1];
                for (int livestockIndex = 0; livestockIndex < livestock.count(); livestockIndex++)
                {
                    TbasketLivestock aLivestock;
                    aLivestock.livetype_code = livestock[livestockIndex];
                    aLivestock.allocation = 0;
                    aFeed.livestock.append(aLivestock);
                }
                aSeason.feeds.append(aFeed);
            }
            basket.append(aSeason);
        }

        this->rebuildItems();
    }
}

void basketModel::setDatabase(QSqlDatabase cleaned_db)
{
    this->db = cleaned_db;
}

int basketModel::columnCount(const QModelIndex &) const
{
    return livestock.count()+1;
}

int basketModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

bool basketModel::livestockExists(QString code)
{
    for (int i = 0; i < livestock.count(); i++)
    {
        if (livestock[i] == code)
        {
            return true;
        }
    }
    return false;
}

bool basketModel::seasonExists(QString code)
{
    for (int i = 0; i < seasons.count(); i++)
    {
        if (seasons[i] == code)
        {
            return true;
        }
    }
    return false;
}

bool basketModel::feedExists(QString crop, QString feed)
{
    for (int i = 0; i < feeds.count(); i++)
    {
        if (feeds[i] == crop + "|" +  feed)
        {
            return true;
        }
    }
    return false;
}

Qt::ItemFlags basketModel::flags(const QModelIndex &index) const
{
    if (items[index.row()].lineType == 2)
    {
        if (index.column() > 0)
            return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled;
        else
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QJsonArray basketModel::getBasketArray()
{
    QJsonArray basketArray;
    for (int basketIndex = 0; basketIndex < basket.count(); basketIndex++)
    {
        QJsonObject seasonObject;
        seasonObject["season_name"] = basket[basketIndex].season_name;
        QJsonArray feedArray;
        for (int feedIndex = 0; feedIndex < basket[basketIndex].feeds.count(); feedIndex++)
        {
            QJsonObject feedObject;
            feedObject["feed_type_code"] = basket[basketIndex].feeds[feedIndex].feed_type_code;
            feedObject["feed_item_code"] = basket[basketIndex].feeds[feedIndex].feed_item_code;
            QJsonArray livestockArray;
            for (int livestockIndex = 0; livestockIndex < basket[basketIndex].feeds[feedIndex].livestock.count(); livestockIndex++)
            {
                QJsonObject livestockObject;
                livestockObject["livetype_code"] = basket[basketIndex].feeds[feedIndex].livestock[livestockIndex].livetype_code;
                livestockObject["allocation"] = basket[basketIndex].feeds[feedIndex].livestock[livestockIndex].allocation;
                livestockArray.append(livestockObject);
            }
            feedObject["livestock"] = livestockArray;
            feedArray.append(feedObject);
        }
        seasonObject["feeds"] = feedArray;
        basketArray.append(seasonObject);
    }
    return basketArray;
}

void basketModel::addNewLivestock(QString code, bool resetModel)
{
    if (livestockExists(code))
        return;
    livestock.append(code);
    if (resetModel)
    {
        for (int basketIndex = 0; basketIndex < basket.count(); basketIndex++)
        {
            for (int feedIndex = 0; feedIndex < basket[basketIndex].feeds.count(); feedIndex++)
            {
                TbasketLivestock aLivestock;
                aLivestock.livetype_code = code;
                aLivestock.allocation = 0;
                basket[basketIndex].feeds[feedIndex].livestock.append(aLivestock);
            }
        }
        rebuildItems();
    }
}

void basketModel::removeLivestock(QString code)
{
    if (!livestockExists(code))
        return;        
    for (int basketIndex = 0; basketIndex < basket.count(); basketIndex++)
    {
        for (int feedIndex = 0; feedIndex < basket[basketIndex].feeds.count(); feedIndex++)
        {
            int idxFound = -1;
            for (int livestockIndex = 0; livestockIndex < basket[basketIndex].feeds[feedIndex].livestock.count(); livestockIndex++)
            {
                if (basket[basketIndex].feeds[feedIndex].livestock[livestockIndex].livetype_code == code)
                {
                    idxFound = livestockIndex;
                    break;
                }
            }
            if (idxFound >= 0)
                basket[basketIndex].feeds[feedIndex].livestock.removeAt(idxFound);
        }
    }
    livestock.removeAll(code);
    rebuildItems();
}

void basketModel::rebuildItems()
{
    this->beginResetModel();
    items.clear();
    for (int seasonIndex = 0; seasonIndex < basket.count(); seasonIndex++)
    {
        TbasketLine aSeasonLine;
        aSeasonLine.lineType = 1;
        aSeasonLine.season_name = basket[seasonIndex].season_name;
        aSeasonLine.lineDescription = basket[seasonIndex].season_name;
        items.append(aSeasonLine);
        for (int feedIndex = 0; feedIndex < basket[seasonIndex].feeds.count(); feedIndex++)
        {
            TbasketLine aFeedLine;
            aFeedLine.lineType = 2;
            aFeedLine.season_name = basket[seasonIndex].season_name;
            aFeedLine.feed_type_code = basket[seasonIndex].feeds[feedIndex].feed_type_code;
            aFeedLine.feed_item_code = basket[seasonIndex].feeds[feedIndex].feed_item_code;
            QString feed_type_name;
            QSqlQuery query(this->db);
            if (query.exec("SELECT feed_type_name FROM lkp_feedtype WHERE feed_type_code = '" + basket[seasonIndex].feeds[feedIndex].feed_type_code + "'"))
            {
                if (query.first())
                {
                    feed_type_name = query.value(0).toString();
                }
            }
            QString feed_item_name;
            if (query.exec("SELECT feed_item_name FROM lkp_feeditem WHERE feed_type_code = '" + basket[seasonIndex].feeds[feedIndex].feed_type_code + "' AND feed_item_code = '" + basket[seasonIndex].feeds[feedIndex].feed_item_code + "'"))
            {
                if (query.first())
                {
                    feed_item_name = query.value(0).toString();
                }
            }
            aFeedLine.lineDescription = feed_item_name + " of " + feed_type_name;
            items.append(aFeedLine);
        }


        TbasketLine aSummaryLine;
        aSummaryLine.lineType = 3;
        aSummaryLine.season_name = basket[seasonIndex].season_name;
        aSummaryLine.lineDescription = "Total allocation";
        items.append(aSummaryLine);

        TbasketLine aSeparationLine;
        aSeparationLine.lineType = 4;
        aSeparationLine.season_name = basket[seasonIndex].season_name;
        aSeparationLine.lineDescription = "";
        items.append(aSeparationLine);
    }

    this->endResetModel();
}

void basketModel::addNewSeason(QString code, bool resetModel)
{
    if (seasonExists(code))
        return;
    seasons.append(code);
    if (resetModel)
    {
        TbasketSeason aSeason;
        aSeason.season_name = code;
        for (int feedIndex = 0; feedIndex < feeds.count(); feedIndex++)
        {
            TbasketFeed aFeed;
            QStringList parts = feeds[feedIndex].split("|");
            aFeed.feed_type_code = parts[0];
            aFeed.feed_item_code = parts[1];
            for (int livestockIndex = 0; livestockIndex < livestock.count(); livestockIndex++)
            {
                TbasketLivestock aLivestock;
                aLivestock.livetype_code = livestock[livestockIndex];
                aLivestock.allocation = 0;
                aFeed.livestock.append(aLivestock);
            }
            aSeason.feeds.append(aFeed);
        }
        basket.append(aSeason);
        rebuildItems();
    }
}

void basketModel::removeSeason(QString code)
{
    if (!seasonExists(code))
        return;
    int idxFound = -1;
    for (int seasonIndex = 0; seasonIndex < basket.count(); seasonIndex++)
    {
        if (basket[seasonIndex].season_name == code)
        {
            idxFound = seasonIndex;
            break;
        }
    }
    if (idxFound >= 0)
        basket.removeAt(idxFound);
    seasons.removeAll(code);
    rebuildItems();
}

void basketModel::addNewFeed(QString crop, QString feed, bool resetModel)
{
    if (feedExists(crop, feed))
        return;
    feeds.append(crop + "|" + feed);
    if (resetModel)
    {
        for (int seasonIndex = 0; seasonIndex < basket.count(); seasonIndex++)
        {
            TbasketFeed aFeed;
            aFeed.feed_type_code = crop;
            aFeed.feed_item_code = feed;
            for (int livestockIndex = 0; livestockIndex < livestock.count(); livestockIndex++)
            {
                TbasketLivestock aLivestock;
                aLivestock.livetype_code = livestock[livestockIndex];
                aLivestock.allocation = 0;
                aFeed.livestock.append(aLivestock);
            }
            basket[seasonIndex].feeds.append(aFeed);
        }
        rebuildItems();
    }
}

void basketModel::removeFeed(QString crop, QString feed)
{
    if (!feedExists(crop, feed))
        return;
    feeds.removeAll(crop + "|" + feed);
    for (int seasonIndex = 0; seasonIndex < basket.count(); seasonIndex++)
    {
        int idxFound = -1;
        for (int feedIndex = 0; feedIndex < basket[seasonIndex].feeds.count(); feedIndex++)
        {
            if (basket[seasonIndex].feeds[feedIndex].feed_type_code == crop && basket[seasonIndex].feeds[feedIndex].feed_item_code == feed)
            {
                idxFound = feedIndex;
                break;
            }
        }
        if (idxFound >= 0)
             basket[seasonIndex].feeds.removeAt(idxFound);
    }
    rebuildItems();
}

QVariant basketModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == 0) return "Season / Feed";
            if (section >= 1)
            {
                QString livestockName;
                QSqlQuery query(this->db);
                if (query.exec("SELECT livetype_desc FROM lkp_livetype WHERE livetype_code = '" + livestock[section-1] + "'"))
                {
                    if (query.first())
                    {
                        livestockName = query.value(0).toString();
                    }
                }
                return livestockName;
            }
        }
    }
    return QVariant();
}

double basketModel::getAllocation(QString season, QString crop, QString feed, QString livestock) const
{
    for (int seasonIndex = 0; seasonIndex < basket.count(); seasonIndex++)
    {
        if (basket[seasonIndex].season_name == season)
        {
            for (int feedIndex = 0; feedIndex < basket[seasonIndex].feeds.count(); feedIndex++)
            {
                if (basket[seasonIndex].feeds[feedIndex].feed_type_code == crop && basket[seasonIndex].feeds[feedIndex].feed_item_code == feed)
                {
                    for (int livestockIndex = 0; livestockIndex < basket[seasonIndex].feeds[feedIndex].livestock.count(); livestockIndex++)
                    {
                        if (basket[seasonIndex].feeds[feedIndex].livestock[livestockIndex].livetype_code == livestock)
                            return basket[seasonIndex].feeds[feedIndex].livestock[livestockIndex].allocation;
                    }
                }
            }
        }
    }
    return -1;
}

double basketModel::getTotalAllocation(QString season, QString livestock) const
{
    double total = 0;
    for (int seasonIndex = 0; seasonIndex < basket.count(); seasonIndex++)
    {
        if (basket[seasonIndex].season_name == season)
        {
            for (int feedIndex = 0; feedIndex < basket[seasonIndex].feeds.count(); feedIndex++)
            {
                for (int livestockIndex = 0; livestockIndex < basket[seasonIndex].feeds[feedIndex].livestock.count(); livestockIndex++)
                {
                    if (basket[seasonIndex].feeds[feedIndex].livestock[livestockIndex].livetype_code == livestock)
                        total = total + basket[seasonIndex].feeds[feedIndex].livestock[livestockIndex].allocation;
                }
            }
        }
    }
    return total;
}

void basketModel::setAllocation(QString season, QString crop, QString feed, QString livestock, double value)
{
    for (int seasonIndex = 0; seasonIndex < basket.count(); seasonIndex++)
    {
        if (basket[seasonIndex].season_name == season)
        {
            for (int feedIndex = 0; feedIndex < basket[seasonIndex].feeds.count(); feedIndex++)
            {
                if (basket[seasonIndex].feeds[feedIndex].feed_type_code == crop && basket[seasonIndex].feeds[feedIndex].feed_item_code == feed)
                {
                    for (int livestockIndex = 0; livestockIndex < basket[seasonIndex].feeds[feedIndex].livestock.count(); livestockIndex++)
                    {
                        if (basket[seasonIndex].feeds[feedIndex].livestock[livestockIndex].livetype_code == livestock)
                        {
                            basket[seasonIndex].feeds[feedIndex].livestock[livestockIndex].allocation = value;
                            return;
                        }
                    }
                }
            }
        }
    }
}

QVariant basketModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (items[index.row()].lineType == 1)
        {
            if (index.column() == 0) return items[index.row()].lineDescription;
        }
        if (items[index.row()].lineType == 3)
        {
            if (index.column() == 0)
                return "Total";
            else
            {
                QString liveCode = livestock[index.column()-1];
                return getTotalAllocation(items[index.row()].season_name, liveCode);
            }
        }
        if (items[index.row()].lineType == 4)
        {
            return QVariant();
        }
    }
    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        if (items[index.row()].lineType == 2)
        {
            if (index.column() == 0)
                return items[index.row()].lineDescription;
            if (index.column() > 0)
            {
                QString liveCode = livestock[index.column()-1];
                return getAllocation(items[index.row()].season_name, items[index.row()].feed_type_code, items[index.row()].feed_item_code, liveCode);
            }
        }
    }
    if (role == Qt::BackgroundRole)
    {
        if (items[index.row()].lineType == 4)
        {
            QBrush Background(QColor("grey")); //We can change this to a nice color
            return Background;
        }
        if (items[index.row()].lineType == 1)
        {
            QBrush Background(QColor(143,170,220)); //We can change this to a nice color
            return Background;
        }
    }
    if (role == Qt::ForegroundRole)
    {
        if (items[index.row()].lineType == 1)
        {
            QBrush Background(QColor("black")); //We can change this to a nice color
            return Background;
        }
    }
    return QVariant();
}

bool basketModel::setData(const QModelIndex &index,const QVariant &value,int role)
{
    if (role == Qt::EditRole)
    {
        if (index.column() > 0)
        {
            if (items[index.row()].lineType == 2)
            {
                bool ok;
                double newValue = value.toDouble(&ok);
                if (ok)
                {
                    if (index.column() > 0)
                    {
                        QString liveCode = livestock[index.column()-1];
                        setAllocation(items[index.row()].season_name, items[index.row()].feed_type_code, items[index.row()].feed_item_code, liveCode, newValue);
                        emit modelChanged();
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void basketModel::changeSeasonName(QString fromSeason, QString toSeason)
{
    for (int seasonIndex = 0; seasonIndex < basket.count(); seasonIndex++)
    {
        if (basket[seasonIndex].season_name == fromSeason)
        {
            basket[seasonIndex].season_name = toSeason;
            break;
        }
    }
    for (int seasonIndex = 0; seasonIndex < seasons.count(); seasonIndex++)
    {
        if (seasons[seasonIndex] == fromSeason)
        {
            seasons[seasonIndex] = toSeason;
            break;
        }
    }
    rebuildItems();
}
