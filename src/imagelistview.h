#ifndef IMAGELISTVIEW_H
#define IMAGELISTVIEW_H

#include <QListView>

class ImageListView : public QListView
{
    Q_OBJECT
public:
    ImageListView(QWidget *parent = nullptr);
protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
signals:
    void currentChange(const QModelIndex &current);
};

#endif // IMAGELISTVIEW_H
