#ifndef RASTERWINDOW_H
#define RASTERWINDOW_H

#include <QtGui>

#include <vector>

#include <QTimer>

#include "xtiming.h"

#include "xscreenframe.h"
#include "xframesegmentation.h"


struct Point2D
{
    float x;
    float y;
    Point2D()
    {
        x = 0;
        y = 0;
    }

    Point2D(float _x, float _y)
    {
        x = _x;
        y = _y;
    }
};


class RasterWindow : public QWindow
{
    Q_OBJECT
public:
    explicit RasterWindow(QWindow *parent = 0);

    virtual void render(QPainter *painter);

    virtual ~RasterWindow();

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;

    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;

private:
    QBackingStore *m_backingStore;
    bool m_update_pending;

    QImage m_imgScreen;
    int m_screenW;
    int m_screenH;

    void visualiseScreenImage();

    void putPixel(float _xc, float _yc, int pixel_size = 5, QColor color = QColor(200, 210, 0));
    void drawPoint(const Point2D &p, int point_size = 5);

    void renderLine(int _x1, int _y1, int _x2, int _y2);
    void renderLine(const Point2D &v1, const Point2D &v2);

    bool initScreenSnapshotSystem();
    bool makeScreenSnapshot();

    int m_systemScreenW;
    int m_systemScreenH;

    int m_desktopW;
    int m_desktopH;
    HWND m_hDesktopWnd;
    HDC m_hDesktopDC;
    HDC m_hCaptureDC;

    HBITMAP m_hCaptureBitmap;
    int m_captureErrorCount;


    XScreenFrame m_screenFrame;
    long m_numSnapshots;

    XFrameSegmentation  m_segmenter;

    QTimer timer;

    XTiming m_xtimer;
    double m_time1;
    double m_time2;
    double m_time3;


    long m_numFrameRenderings;

private slots:
    void screenCaptureTimerSlot();
};

#endif // RASTERWINDOW_H
