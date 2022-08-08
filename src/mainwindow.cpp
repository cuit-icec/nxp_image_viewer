#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include <QFileDialog>
#include <QProgressDialog>
#include <QDoubleSpinBox>
#include <QBitmap>
#include <QDebug>
#include <QMessageBox>

const char *aboutInformation = "<html><head/><body><p>"
                               "<span style=\" font-size:large; font-weight:600;\">智能车图像上位机 0.1.0</span></p>"
                               "<p>Based on Qt " QT_VERSION_STR "<br/><br/>Built on " __DATE__ " " __TIME__ "<br/><br/>"
                               "Copyright 2021 CUIT ICEC. All rights reserved.</p>"
                               "<p>The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, "
                               "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.</p></body></html>";

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    /* Set layout align center */
    ui->controlButtonsLayout->setAlignment(Qt::AlignCenter);
    ui->controlButtonsLayout->setSizeConstraint(QLayout::SetFixedSize);

    /* Set model and delegate */
    ui->imageListView->setModel(&this->m_imageModel);
    ui->imageListView->setItemDelegate(&this->m_itemDeletage);

    /* Set parameter widget layout */
    this->m_paramWidgetLayout = new QFormLayout(ui->params);
    ui->params->setLayout(this->m_paramWidgetLayout);

    /* Config timer */
    this->m_playTimer.setInterval(100);

    /* Connect menubar trigger signal */
    QObject::connect(ui->menubar, &QMenuBar::triggered, this, &MainWindow::onActionTriggered);
    /* Connect ImageListView current change signal */
    QObject::connect(ui->imageListView, &ImageListView::currentChange, this,
                     &MainWindow::onImageListViewCurrentChanged);
    /* Connect navigation buttons */
    QObject::connect(ui->nextButton, &QPushButton::clicked, this, &MainWindow::onNextButtonClicked);
    QObject::connect(ui->toEndButton, &QPushButton::clicked, this, &MainWindow::onToEndButtonClicked);
    QObject::connect(ui->prevButton, &QPushButton::clicked, this, &MainWindow::onPrevButtonClicked);
    QObject::connect(ui->toBeginButton, &QPushButton::clicked, this, &MainWindow::onToBeginButtonClicked);
    QObject::connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
    /* Connect run button signal */
    QObject::connect(ui->compileButton, &QPushButton::clicked, this, &MainWindow::onCompileButtonClicked);
    /* Connect ImageViewer mouseMoved signal */
    QObject::connect(ui->imageViewer, &ImageViewer::mouseMoved, this, &MainWindow::onImageViewerMouseMoved);
    /* Connect TextEdit textChanged signal */
    QObject::connect(ui->code, &QTextEdit::textChanged, this, &MainWindow::onCodeTextChanged);
    /* Connect signal for m_playTimer */
    QObject::connect(&this->m_playTimer, &QTimer::timeout, this, &MainWindow::onPlayTimerTimeout);
}

MainWindow::~MainWindow() {
    delete ui;
}

/**
 * @brief On action triggered
 * @param action Pointer to triggered action
 */
void MainWindow::onActionTriggered(QAction *action) {
    ImageLoader loader(this);
    QFutureWatcher<void> watcher;

    if (action == ui->openFilesAct) {
        auto fileList = QFileDialog::getOpenFileNames(this,
                                                      "打开图像序列",
                                                      QDir::homePath(),
                                                      "图像文件 (*.png *.jpg *.bmp)");
        QProgressDialog dialog("正在载入图片", "Cancel", 0, 1, this, Qt::Dialog);
        if (fileList.length()) {
            /* Remove old images */
            this->m_imageModel.removeRows(0, this->m_imageModel.rowCount());
            /* Setup dialog */
            dialog.setMaximum(static_cast<int>(fileList.length()));
            /* Set future */
            /* FIXME: Add mutex */
            watcher.setFuture(QtConcurrent::map(fileList, loader));
            QObject::connect(&watcher, &QFutureWatcher<void>::progressValueChanged, this,
                             [&dialog](int value) {
                               dialog.setValue(value);
                             });
            QObject::connect(&dialog, &QProgressDialog::canceled, this,
                             [&watcher]() {
                               watcher.cancel();
                             });
            dialog.exec();
        }
    } else if (action == ui->exitAct) {
        QApplication::quit();
    } else if (action == ui->aboutAct) {
        QMessageBox::about(this, "关于 智能车图像上位机", aboutInformation);
    } else if (action == ui->aboutQtAct) {
        QMessageBox::aboutQt(this);
    }
}

/**
 * @brief Current index changed
 * @param current const reference to current index
 */
void MainWindow::onImageListViewCurrentChanged(const QModelIndex &current) {
    /* Auto scroll */
    ui->imageListView->scrollTo(current);

    if (!this->m_isNeedCompile) {
        ui->imageViewer->setPixmap(this->processCurrentImage());
    }
}

/**
 * @brief When ImageViewer emit mouseMoved, update the label text
 * @param pos Mosue hovered pixel
 * @param color Color of mosue hovered pixel
 */
void MainWindow::onImageViewerMouseMoved(QPoint point, QColor color) {
    ui->colorLabel->setText(QStringLiteral("x: %1 y: %2 灰度值: %3")
                                    .arg(point.x(), 3)
                                    .arg(point.y(), 3)
                                    .arg(qGray(color.rgb())));
}

/**
 * @brief Update parameter when value changed
 * @param value Value
 */
void MainWindow::onSpinBoxValueChanged(double value) {
    /* Update parameter */
    this->m_spinboxToIndexMap[QObject::sender()]->value = static_cast<float>(value);

    /* Process image using new parameter */
    ui->imageViewer->setPixmap(this->processCurrentImage());
}

/**
 * @brief To next image button clicked
 */
void MainWindow::onNextButtonClicked() {
    auto index = this->m_imageModel.index(ui->imageListView->currentIndex().row() + 1);
    if (index.isValid()) {
        ui->imageListView->setCurrentIndex(index);
    }
}

/**
 * @brief To end image button clicked
 */
void MainWindow::onToEndButtonClicked() {
    auto index = this->m_imageModel.index(this->m_imageModel.rowCount() - 1);
    ui->imageListView->setCurrentIndex(index);
}

/**
 * @brief To prev image button clicked
 */
void MainWindow::onPrevButtonClicked() {
    auto index = this->m_imageModel.index(ui->imageListView->currentIndex().row() - 1);
    if (index.isValid()) {
        ui->imageListView->setCurrentIndex(index);
    }
}

/**
 * @brief To begin button clicked
 */
void MainWindow::onToBeginButtonClicked() {
    auto index = this->m_imageModel.index(0);
    ui->imageListView->setCurrentIndex(index);
}

/**
 * @brief Play button clicked
 */
void MainWindow::onPlayButtonClicked() {
    if (this->m_playTimer.isActive()) {
        this->m_playTimer.stop();
        ui->playButton->setIcon(QIcon(":/icons/play.svg"));
    } else {
        this->m_playTimer.start();
        ui->playButton->setIcon(QIcon(":/icons/pause.svg"));
    }
}


/**
 * @brief Load image to model from disk, run with QtConcurrent::map
 * @param path Files to load
 */
void MainWindow::loadImage(const QString &path) {
    ImageItem item;

    item.path = path;
    item.filename = QUrl(path).fileName();
    /* Get thumbnail */
    /* NOTE: Too expensive function */
    item.thumbnail.load(path);
    item.thumbnail = item.thumbnail.scaledToWidth(200, Qt::SmoothTransformation);
    /* Insert new row */
    this->m_imageModel.insertRow(this->m_imageModel.rowCount());
    /* Get model index */
    auto modelIndex = this->m_imageModel.index(this->m_imageModel.rowCount() - 1);
    this->m_imageModel.setData(modelIndex, QVariant::fromValue(item), Qt::DisplayRole);
}

ImageLoader::ImageLoader(MainWindow *parent) : parent(parent) {}

/**
 * @brief ImageLoader::operator () override for concurrrent
 * @param path Image path
 */
void ImageLoader::operator()(const QString &path) const {
    parent->loadImage(path);
}

/**
 * @brief Process imageListView current image
 * @return
 */
QPixmap MainWindow::processCurrentImage() {
    QString path = ui->imageListView->currentIndex().data().value<ImageItem>().path;

    return this->m_imageProcessor.process(path);
}

/**
 * @brief Run button clicked
 */
void MainWindow::onCompileButtonClicked() {
    if (this->m_isNeedCompile && this->compile()) {
        /* Load DLL */
        if (this->m_imageProcessor.loadDLL("interface.dll")) {
            this->loadParams();
            ui->imageViewer->setPixmap(this->processCurrentImage());
            ui->compileButton->setEnabled(false);
            this->m_isNeedCompile = false;
        } else {
            QMessageBox::warning(this, "提示", "DLL 加载失败，请检查函数接口是否正确！");
        }
    }
}

/**
 * @brief Code editor text changed, need recompile
 */
void MainWindow::onCodeTextChanged() {
    ui->compileButton->setEnabled(true);
    this->m_isNeedCompile = true;
}

/**
 * @brief m_playTimer timeout
 */
void MainWindow::onPlayTimerTimeout() {
    auto index = this->m_imageModel.index(ui->imageListView->currentIndex().row() + 1);
    if (index.isValid()) {
        ui->imageListView->setCurrentIndex(index);
    } else {
        dynamic_cast<QTimer *>(QObject::sender())->stop();
        ui->playButton->setIcon(QIcon(":/icons/play.svg"));
    }
}

/**
 * @brief Compile code
 * @return
 */
bool MainWindow::compile() {
    QFile sourceFile(":/interface/template.c");
    QString sourceCode;

    /* Add code to template */
    sourceFile.open(QIODevice::ReadOnly | QIODevice::Text);
    sourceCode = QString(sourceFile.readAll());
    sourceCode.insert(sourceCode.indexOf(QLatin1String("/* BEGIN */")) + 13, ui->code->toPlainText());
    sourceFile.close();
    /* Write code to disk */
    QFile::remove(QLatin1String("./temp.c"));
    sourceFile.setFileName(QLatin1String("./temp.c"));
    sourceFile.open(QIODevice::ReadWrite | QIODevice::Text);
    sourceFile.write(sourceCode.toUtf8());
    sourceFile.close();

    /* Free previously loaded DLL */
    this->m_imageProcessor.freeDLL();
    /* Create gcc process */
    QProcess gccProcess;
    QStringList args = QStringList()
            << QLatin1String("./temp.c")
            << QLatin1String("-shared")
            << QLatin1String("-o")
            << QLatin1String("interface.dll");
#ifdef QT_DEBUG
    gccProcess.start(QLatin1String("gcc"), args);
#else
    gccProcess.start(QLatin1String("./tool/tdm-gcc/bin/gcc"), args);
#endif
    /* Start faild */
    if (gccProcess.state() != QProcess::Running) {
        QMessageBox::warning(this, "提示", gccProcess.errorString());
        return false;
    }
    /* Timeout */
    if (gccProcess.waitForFinished(10000)) {
        /* Compile success */
        if (gccProcess.exitCode() == 0) {
            return true;
        } else {
            QMessageBox::warning(this, "提示", gccProcess.readAllStandardError());
        }
    } else {
        gccProcess.kill();
        QMessageBox::warning(this, "提示", "编译超时！");
    }

    return false;
}

/**
 * @brief Load parameters and generate widgets
 */
void MainWindow::loadParams() {
    QLabel *nameLabel;
    QDoubleSpinBox *spinBox;

    /* Remove old widget */
    int rowCount = this->m_paramWidgetLayout->rowCount();
    for (int i = 0; i < rowCount; i++) {
        this->m_paramWidgetLayout->removeRow(0);
    }
    /* Remove old map */
    this->m_spinboxToIndexMap.clear();

    /* Get parameters */
    int paramsNum = m_imageProcessor.paramsNum();
    InterfaceParam *params = m_imageProcessor.params();

    for (int i = 0; i < paramsNum; i++) {
        /* Add new widget */
        nameLabel = new QLabel(params[i].name, ui->params);
        spinBox = new QDoubleSpinBox(ui->params);
        spinBox->setRange(params[i].minimum, params[i].maximum);
        this->m_paramWidgetLayout->addRow(nameLabel, spinBox);

        /* Add widget and index to map */
        this->m_spinboxToIndexMap[spinBox] = params + i;
        /* Connect value update signal to slot */
        QObject::connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(onSpinBoxValueChanged(double)));
    }
}
