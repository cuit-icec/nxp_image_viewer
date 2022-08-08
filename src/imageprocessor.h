#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <Windows.h>

#include <QObject>
#include <QString>
#include <QPixmap>


struct InternalOps {
    void (*drawPoint)(int x, int y, uint32_t color, void *object);
};

struct InterfaceParam {
    const char *name;
    float value;
    float maximum;
    float minimum;
};

typedef void (*Interface_Process)(uchar *data, int width, int height);

typedef InterfaceParam *(*Interface_Params)();

typedef int (*Interface_ParamsNum)();

class ImageProcessor : public QObject {
  Q_OBJECT
  public:
    explicit ImageProcessor(QWidget *parent = nullptr);

    bool loadDLL(const QString &path);

    bool freeDLL();

    QPixmap process(const QString &path);

    InterfaceParam *params();

    int paramsNum();

  private:
    HMODULE m_hDLL;
    QPixmap m_markLayerPixmap;

    /* Interface function pointer */
    Interface_Process interfaceProcess;
    Interface_Params interfaceParams;
    Interface_ParamsNum interfaceParamsNum;

    static void drawPoint(int x, int y, uint32_t color, void *object);

    static constexpr InternalOps ops = {drawPoint};
};

#endif // IMAGEPROCESSOR_H
