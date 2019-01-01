#ifndef RASTERWINDOW_H
#define RASTERWINDOW_H

#include <QtGui>

#include <vector>

#include <QTimer>

#include "xtiming.h"

#include "xscreenframe.h"

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
    int m_screenW = 640;
    int m_screenH = 640;



    void putPixel(float _xc, float _yc, int pixel_size = 5, QColor color = QColor(200, 210, 0));
    void drawPoint(const Point2D &p, int point_size = 5);

    void renderLine(int _x1, int _y1, int _x2, int _y2);
    void renderLine(const Point2D &v1, const Point2D &v2);

    bool initScreenSnapshotSystem();
    bool makeScreenSnapshot();

    void visualiseScreenImage();

    void runOcrSegmentation();
    void drawOcrOverlay(QPainter &painter);

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
    long m_numSnapshots = 0;
    int mouse_screen_x = 0;
    int mouse_screen_y = 0;
    int mouse_global_x = 0;
    int mouse_global_y = 0;

    QTimer timer;

    XTiming m_xtimer;
    double m_time1 = 0;
    double m_time2 = 0;
    double m_time3 = 0;


    long m_numFrameRenderings = 0;

private slots:
    void screenCaptureTimerSlot();
};

#endif // RASTERWINDOW_H
