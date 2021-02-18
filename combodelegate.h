#ifndef COMBODELEGATE_H
#define COMBODELEGATE_H

#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QBrush>

struct genCmbItem
{
  QString code;
  QString description;
  QPixmap image;
};
typedef genCmbItem TgenCmbItem;

class comboDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    comboDelegate(QWidget *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                           const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
             const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void insertItem(QString code, QString description);

    void setReadOnly(bool readyOnly);
    void setParentModel(QAbstractTableModel *model);
    void setParentSortModel(QSortFilterProxyModel *sortmodel);
    void setEditable(bool editable);
    void addNoDrawValue(QString value);
    void setBrush(QBrush brush);

private:
    QList<TgenCmbItem> m_items;
    bool m_readOnly;
    QAbstractTableModel *m_parentModel;
    QSortFilterProxyModel *m_sortModel;
    bool m_editable;
    QStringList m_noDrawValues;
    bool isNoDrawValue(QString value) const;
    QBrush brushToUse;
};

#endif // COMBODELEGATE_H
