#ifndef IMAGELISTMODEL_H
#define IMAGELISTMODEL_H

#include <QAbstractListModel>
#include <QItemDelegate>

#include <QVector>
#include <QPixmap>

struct ImageItem {
    QPixmap thumbnail;
    QString path;
    QString filename;
};
Q_DECLARE_METATYPE(ImageItem);

class ImageListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ImageListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    QVector<ImageItem> m_data;
};

class ImageListItemDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    ImageListItemDelegate(QObject *parent = nullptr);

    // QAbstractItemDelegate interface
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    const int m_space = 36;
};

#endif // IMAGELISTMODEL_H
