#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class ImageViewer : public QGraphicsView
{
    Q_OBJECT
public:
    ImageViewer(QWidget *parent = nullptr);
    void setPixmap(const QPixmap &pixmap);
    QPixmap pixmap();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsScene m_scene;
    QGraphicsPixmapItem *m_pixmapItem;

signals:
    void mouseMoved(const QPoint &point, const QColor &color);
};

#endif // IMAGEVIEWER_H
