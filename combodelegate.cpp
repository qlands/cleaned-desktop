#include "combodelegate.h"
#include <QLabel>
#include <QPainter>
#include <QComboBox>


comboDelegate::comboDelegate(QWidget *parent) :
    QStyledItemDelegate(parent)
{
    m_readOnly = false;
    m_parentModel = 0;
    m_sortModel = 0;
    m_editable = false;
}

void comboDelegate::setBrush(QBrush brush)
{
    brushToUse = brush;
}

void comboDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if (isNoDrawValue(index.data().toString()))
    {
        QStyledItemDelegate::paint(painter,option,index);
        return;
    }

    QString description;
    description = "Set a value";
    for (int pos = 0; pos <= m_items.count()-1; pos++)
    {
        if (m_items[pos].code.toUpper() == index.data().toString().toUpper())
        {
            description = m_items[pos].description;
            break;
        }
    }
    if (m_editable)
    {
        description = index.data().toString();
    }

    QLabel tlabel;
    QFont cfont;
    cfont = tlabel.font();
    tlabel.setText(description);
    if (!m_readOnly)
        cfont.setUnderline(true);

    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    else
        painter->fillRect(option.rect, brushToUse);

    QPen tpen;
    tpen = painter->pen();
    tpen.setColor(QColor(Qt::black));
    if (!m_parentModel)
    {
        painter->setFont(cfont);
        if (!m_readOnly)
        {
            painter->setPen(tpen);
        }
        painter->drawText(option.rect,Qt::AlignCenter,description);
    }
//    else
//    {
//        childTableModel *tm;
//        tm = qobject_cast<childTableModel *>(m_parentModel);
//        if (tm)
//        {
//            QModelIndex idx;
//            if (m_sortModel)
//                idx = m_sortModel->mapToSource(index);
//            else
//                idx = index;
//            if ((tm->getItemStatus(idx.row()) == "C") ||
//                (tm->getItemStatus(idx.row()) == "M"))
//            {
//                painter->setFont(cfont);
//                if (!m_readOnly)
//                    painter->setPen(tpen);
//                painter->drawText(option.rect,Qt::AlignCenter,description);
//            }
//            else
//            {
//                QStyledItemDelegate::paint(painter,option,index);
//            }
//        }
//        else
//        {
//            painter->setFont(cfont);
//            if (!m_readOnly)
//                painter->setPen(tpen);
//            painter->drawText(option.rect,Qt::AlignCenter,description);
//        }
//    }

}

QSize comboDelegate::sizeHint(const QStyleOptionViewItem &,
               const QModelIndex &index) const
{
    QString description;
    description = "Something";
    for (int pos = 0; pos <= m_items.count()-1; pos++)
    {
        if (m_items[pos].code.toUpper() == index.data().toString().toUpper())
        {
            description = m_items[pos].description;
            break;
        }
    }
    QLabel tlabel;
    QFontMetrics fm(tlabel.font());
    return fm.size(Qt::TextSingleLine,description);
}

QWidget *comboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &,
                           const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->setEditable(m_editable);
    int pos;

        for (pos = 0; pos <= m_items.count()-1; pos++)
        {
            editor->addItem(m_items[pos].description);
        }

    if (m_editable)
    {
        int nvalue;
        nvalue = -1;
        for (pos = 0; pos <= m_items.count()-1; pos++)
        {
            if (m_items[pos].code.toUpper() == index.data().toString().toUpper())
            {
                nvalue = pos;
                break;
            }
        }
        if (nvalue == -1)
        {
            editor->addItem(index.data().toString());
        }
    }
    return editor;
}

void comboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    int nvalue;
    nvalue = -1;

        for (int pos = 0; pos <= m_items.count()-1; pos++)
        {
            if (m_items[pos].code.toUpper() == value.toUpper())
            {
                nvalue = pos;
                break;
            }
        }

    QComboBox *combo = static_cast<QComboBox*>(editor);
    if (nvalue == -1)
    {
        if (m_editable)
        {
            combo->addItem(value);
            nvalue = combo->count()-1;
        }
    }
    combo->setCurrentIndex(nvalue);
}

void comboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const
{
    QComboBox *combo = static_cast<QComboBox*>(editor);
    int nvalue;
    nvalue = combo->currentIndex();
    QString value;
    if (nvalue >= 0)
        value = m_items[nvalue].code;
    else
        value = m_items[0].code;
    if (m_editable)
    {
        value = combo->currentText();
    }
    model->setData(index, value, Qt::EditRole);
}

void comboDelegate::updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

void comboDelegate::insertItem(QString code, QString description)
{
    TgenCmbItem item;
    item.code = code;
    item.description = description;
    m_items.append(item);
}

void comboDelegate::setReadOnly(bool readyOnly)
{
    m_readOnly = readyOnly;
}

void comboDelegate::setParentModel(QAbstractTableModel *model)
{
    m_parentModel = model;
}

void comboDelegate::setParentSortModel(QSortFilterProxyModel *sortmodel)
{
    m_sortModel = sortmodel;
}


void comboDelegate::setEditable(bool editable)
{
    m_editable = editable;
}

void comboDelegate::addNoDrawValue(QString value)
{
    m_noDrawValues.append(value);
}

bool comboDelegate::isNoDrawValue(QString value) const
{
    bool res;
    res = false;
    for (int pos = 0; pos <= m_noDrawValues.count()-1;pos++)
    {
        if (m_noDrawValues[pos] == value)
        {
            res = true;
            break;
        }
    }
    return res;
}
