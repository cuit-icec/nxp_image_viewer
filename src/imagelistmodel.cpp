#include "imagelistmodel.h"

#include <QPainter>
#include <QUrl>
#include <QDebug>

ImageListModel::ImageListModel(QObject *parent) : QAbstractListModel(parent)
{
    qRegisterMetaType< QVector<int> >("QVector<int>");
}

int ImageListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return this->m_data.count();
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)
    QVariant res;

    res.setValue(this->m_data.at(index.row()));
    return res;
}

bool ImageListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    ImageItem item;

    if (row >= 0) {
        /* Must call beginInsertRows before inserting data */
        this->beginInsertRows(parent, row, row + count);
        for (int i = 0; i < count; i++) {
            this->m_data.insert(row + i, item);
        }
        this->endInsertRows();
        return true;
    } else {
        return false;
    }
}

bool ImageListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    /* Border check */
    if (row + count <= this->m_data.size() && count && row >= 0) {
        /* Must call beginRemoveRows before remove rows */
        this->beginRemoveRows(parent, row, row + count - 1);
        this->m_data.remove(row, count);
        this->endRemoveRows();
        return true;
    } else {
        return false;
    }
}

bool ImageListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    if (index.row() < this->m_data.size() && index.row() >= 0) {
        this->m_data[index.row()] = value.value<ImageItem>();
        emit dataChanged(index, index);
        return true;
    } else {
        return false;
    }
}


/**
 * @brief Constructor
 * @param parent parent pointer
 */
ImageListItemDelegate::ImageListItemDelegate(QObject *parent) : QItemDelegate(parent) {}

void ImageListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    ImageItem item = index.data().value<ImageItem>();
    const QRect &drawAera = option.rect;

    /* Draw background rect if selected */
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QColorConstants::Transparent);
        painter->setBrush(QBrush(qRgb(49, 106, 197)));
        painter->drawRect(drawAera.x(), drawAera.y(),
                          drawAera.width(), item.thumbnail.height());
    }
    /* Draw thumbnail */
    painter->drawPixmap(drawAera.x() + (drawAera.width() - item.thumbnail.width()) / 2, drawAera.y(),
                        item.thumbnail.width(), item.thumbnail.height(),
                        item.thumbnail);
    /* Draw label */
    painter->setPen(QColorConstants::Black);
    painter->drawText(drawAera.x(), drawAera.y() + item.thumbnail.height(),
                      drawAera.width(), this->m_space, Qt::AlignCenter,
                      item.filename);
}

QSize ImageListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    QSize size;
    ImageItem item = index.data().value<ImageItem>();

    size.setHeight(item.thumbnail.height() + this->m_space);
    size.setWidth(260);

    return size;
}
