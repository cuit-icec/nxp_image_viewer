#include "imageprocessor.h"

#include <QMessageBox>
#include <QPainter>
#include <QDebug>

typedef void (*Interface_Process)(uchar *data, const int width, const int height);
typedef void (*Interface_setCallback_t)(struct InternalOps fn, void *object);

ImageProcessor::ImageProcessor(QWidget *parent) : QObject(parent)
{

}

bool ImageProcessor::loadDLL(const QString &path)
{
    Interface_setCallback_t Interface_Init;

    /* Free previously loaded library */
    this->freeDLL();
    /* Load library */
    this->m_hDLL = LoadLibrary((const wchar_t *)path.utf16());
    if (this->m_hDLL != NULL) {
        /* Set callback */
        Interface_Init =
                reinterpret_cast<Interface_setCallback_t>(GetProcAddress(this->m_hDLL, "Interface_Init"));
        if (Interface_Init) {
            Interface_Init(ImageProcessor::ops, this);
        } else {
            return false;
        }
        /* Get other function */
        this->interfaceProcess =
                reinterpret_cast<Interface_Process>(GetProcAddress(this->m_hDLL, "Interface_Process"));
        this->interfaceParams =
                reinterpret_cast<Interface_Params>(GetProcAddress(this->m_hDLL, "Interface_Params"));
        this->interfaceParamsNum =
                reinterpret_cast<Interface_ParamsNum>(GetProcAddress(this->m_hDLL, "Interface_ParamsNum"));
        /* Check if all load successfully */
        if (this->interfaceProcess &&
            this->interfaceParams &&
            this->interfaceParamsNum) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

/**
 * @brief Free loaded DLL
 * @return True if free success, else return false
 */
bool ImageProcessor::freeDLL()
{
    if (this->m_hDLL && FreeLibrary(this->m_hDLL)) {
        this->m_hDLL = nullptr;
        return true;
    } else {
        return false;
    }
}

QPixmap ImageProcessor::process(const QString &path)
{
    QPixmap result;
    QImage image(path);
    QPainter painter;

    if (this->m_hDLL != NULL && !image.isNull()) {
        if (this->interfaceProcess) {
            /* New mark layer pixmap */
            /* NOTE: Previous pixmap will auto destroy */
            this->m_markLayerPixmap = QPixmap(image.width(), image.height());
            this->m_markLayerPixmap.fill(QColorConstants::Transparent);

            /* Convert to Grayscale8 */
            image = image.convertToFormat(QImage::Format_Grayscale8);
            /* Process image */
            this->interfaceProcess(image.bits(), image.width(), image.height());
            result = QPixmap::fromImage(image);

            /* Paint mark layer image to result pixmap */
            painter.begin(&result);
            painter.drawPixmap(result.rect(), this->m_markLayerPixmap);
            painter.end();
        }
    }
    return result;
}

/**
 * @brief return interface parameters
 * @return Pointer to interface paramerers, return nullptr if failed
 */
InterfaceParam *ImageProcessor::params()
{
    if (this->interfaceParams) {
        return this->interfaceParams();
    } else {
        return nullptr;
    }
}

/**
 * @brief ImageProcessor::interfaceParamsNum
 * @return Number of interface parameters, return -1 if failed
 */
int ImageProcessor::paramsNum()
{
    if (this->interfaceParamsNum) {
        return this->interfaceParamsNum();
    } else {
        return -1;
    }
}

/**
 * @brief Draw point over processing image
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color in RGB888
 * @param object Pointer to ImageProcessor
 */
void ImageProcessor::drawPoint(int x, int y, uint32_t color, void *object)
{
    QPainter painter;
    ImageProcessor *processor = static_cast<ImageProcessor *>(object);

    painter.begin(&processor->m_markLayerPixmap);
    /* Ignore the alpha component */
    painter.setPen(QColor(color));
    painter.drawPoint(x, y);
    painter.end();
}
