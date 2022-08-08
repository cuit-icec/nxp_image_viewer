#include "imageviewer.h"

#include <QGraphicsItem>
#include <QDebug>

ImageViewer::ImageViewer(QWidget *parent) : QGraphicsView(parent), m_pixmapItem(nullptr)
{
    /* Enable mouse trace for receiving event
     * even if no buttons are pressed */
    this->setMouseTracking(true);

    /* Set scene */
    this->setScene(&this->m_scene);
}

/**
 * @brief Set pixmap
 * @param pixmap Pixmap for replacement
 */
void ImageViewer::setPixmap(const QPixmap &pixmap)
{
    if (this->m_pixmapItem) {
        /* Remove previous pixmap from scene */
        this->m_scene.removeItem(this->m_pixmapItem);
        /* Manually free the menmory, because after it has been
         * removed from scene, it will no longerdelete item */
        delete this->m_pixmapItem;
    }

    /* Add new pixmap item to scene */
    this->m_pixmapItem = this->m_scene.addPixmap(pixmap);
    /* Resize scene rect */
    this->m_scene.setSceneRect(0, 0, pixmap.width(), pixmap.height());
    this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);
}

QPixmap ImageViewer::pixmap()
{
    if (this->m_pixmapItem) {
        return this->m_pixmapItem->pixmap();
    } else {
        return QPixmap();
    }
}

/**
 * @brief Mouse move event
 * @param event Pointer to QMouseEvent
 */
void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    QColor color;
    QPoint point;

    if (this->m_pixmapItem) {
        point = this->mapToScene(event->pos()).toPoint();
        color = this->m_pixmapItem->pixmap().toImage().pixelColor(point);
        /* When position is not valid, color is invalid */
        if (color.isValid()) {
            emit this->mouseMoved(point, color);
        }
    }
}

/**
 * @brief Widget resize event
 * @param event pointer to QResizeEvent
 */
void ImageViewer::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);
}
