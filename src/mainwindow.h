#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QFormLayout>
#include <QMap>
#include <QTimer>
#include <imagelistmodel.h>
#include <imageprocessor.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadImage(const QString &path);

private slots:
    void onActionTriggered(QAction *action);
    void onImageListViewCurrentChanged(const QModelIndex &current);
    void onImageViewerMouseMoved(QPoint point, QColor color);
    void onSpinBoxValueChanged(double value);

    void onNextButtonClicked();
    void onToEndButtonClicked();
    void onPrevButtonClicked();
    void onToBeginButtonClicked();
    void onPlayButtonClicked();
    void onCompileButtonClicked();

    void onCodeTextChanged();

    void onPlayTimerTimeout();

private:
    Ui::MainWindow *ui;
    QFormLayout *m_paramWidgetLayout;
    ImageListModel m_imageModel;
    ImageListItemDelegate m_itemDeletage;
    ImageProcessor m_imageProcessor;
    QMap<QObject *, InterfaceParam *> m_spinboxToIndexMap;
    QTimer m_playTimer;
    bool m_isNeedCompile = true;

    bool compile();
    void loadParams();
    QPixmap processCurrentImage();
};

struct ImageLoader {
    MainWindow *parent;
    ImageLoader(MainWindow *parent);
    void operator()(const QString &path) const;
};

#endif // MAINWINDOW_H
