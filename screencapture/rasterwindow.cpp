#include "rasterwindow.h"

#include <vector>
#include <cmath>
#include <string>
#include <cassert>

#include <windows.h>

#include <list>

RasterWindow::RasterWindow(QWindow *parent)
    : QWindow(parent)
    , m_update_pending(false)
{
    m_backingStore = new QBackingStore(this);
    create();

    setGeometry(600, 200, 900, 700);

    m_screenW = 640;
    m_screenH = 640;

    // QImage imgScreen(800, 600, QImage::Format_RGBX8888);
    m_imgScreen = QImage(m_screenW, m_screenH, QImage::Format_RGBX8888);

    initScreenSnapshotSystem();

    //timer.setInterval(1000/30);
    timer.setInterval(1000/10);

    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(screenCaptureTimerSlot()) );

    timer.start();

    m_xtimer.init();

    m_time1 = 0;
    m_time2 = 0;
    m_time3 = 0;

    m_numSnapshots = 0;
    m_numFrameRenderings = 0;
}

RasterWindow::~RasterWindow()
{
    ReleaseDC(m_hDesktopWnd, m_hDesktopDC);

    //double singleCaptureTime = m_time1 / (double)m_numSnapshots;
    //double singleFrameVisualiseTime = m_time3 / (double)m_numFrameRenderings;

    if (m_hCaptureDC != 0)
    {
        DeleteDC(m_hCaptureDC);
    }
    if (m_hCaptureBitmap != 0)
    {
        DeleteObject(m_hCaptureBitmap);
    }
}


void RasterWindow::screenCaptureTimerSlot()
{
    //Beep(1500, 50);
    renderNow();
}

bool RasterWindow::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest)
    {
        m_update_pending = false;
        renderNow();
        return true;
    }
    return QWindow::event(event);
}

void RasterWindow::renderLater()
{
    if (!m_update_pending)
    {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

void RasterWindow::resizeEvent(QResizeEvent *resizeEvent)
{
    m_backingStore->resize(resizeEvent->size());
    if (isExposed())
        renderNow();
}

void RasterWindow::exposeEvent(QExposeEvent *)
{
    if (isExposed())
    {
        renderNow();
    }
}

bool RasterWindow::initScreenSnapshotSystem()
{
    m_systemScreenW = GetSystemMetrics(SM_CXSCREEN);
    m_systemScreenH = GetSystemMetrics(SM_CYSCREEN);

    m_desktopW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    m_desktopH = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    m_hDesktopWnd = GetDesktopWindow();
    m_hDesktopDC = GetDC(m_hDesktopWnd);

    m_hCaptureDC = CreateCompatibleDC(m_hDesktopDC);
    m_hCaptureBitmap = CreateCompatibleBitmap(m_hDesktopDC, m_systemScreenW, m_systemScreenH);

    SelectObject(m_hCaptureDC, m_hCaptureBitmap);

    m_captureErrorCount = 0;

    m_screenFrame.init(m_systemScreenW, m_systemScreenH);

    return true;
}

bool RasterWindow::makeScreenSnapshot()
{
    // https://www.codeproject.com/Articles/5051/Various-methods-for-capturing-the-screen

    // Capture
    double current_time1 = m_xtimer.getTime();
    BOOL captureResult = BitBlt(m_hCaptureDC, 0, 0, m_systemScreenW, m_systemScreenH,
                                m_hDesktopDC, 0, 0, SRCCOPY | CAPTUREBLT);
    //SaveCapturedBitmap(hCaptureBitmap); //Place holder - Put your code
    //here to save the captured image to disk
    if (captureResult == 0)
    {
        m_captureErrorCount ++;
    }

    double current_time2 = m_xtimer.getTime();
    m_time1 += (current_time2 - current_time1);

    m_numSnapshots++;

    // Copy data from captured frame
    bool copyResult = m_screenFrame.copyCapturedData(m_hCaptureDC, m_hCaptureBitmap, m_numSnapshots);
    if (!copyResult)
    {
        m_captureErrorCount ++;
    }

    double current_time3 = m_xtimer.getTime();

    m_time2 += (current_time3 - current_time2);



    return true;
}

void RasterWindow::visualiseScreenImage()
{
    //m_imgScreen.fill(QColor(0, 0, 96));

    //drawPoint(400, 300);

    makeScreenSnapshot();
    //m_imgScreen.drawPoint(400, 300);

    double current_time4 = m_xtimer.getTime();

    if (1)
    {
        const char *m_pCaptureData = m_screenFrame.getImageDataPtr();

        // Use mouse pointer coords to center captured image view
        POINT p;
        if (!GetCursorPos(&p))
        {
            p.x = 0;
            p.y = 0;
        }

        // Calculate offsets of image capture into destination surfac
        const int min_x = 0;
        const int max_x = m_systemScreenW - m_screenW;

        const int min_y = 0;
        const int max_y = m_systemScreenH - m_screenH;

        int offset_x = p.x - m_screenW/2;
        int offset_y = p.y - m_screenH/2;

        offset_x = std::max(offset_x, min_x);
        offset_x = std::min(offset_x, max_x);

        offset_y = std::max(offset_y, min_y);
        offset_y = std::min(offset_y, max_y);

        // TODO: disable mouse croshair by default later
        const bool debug_draw_mouse_point = true;
        const int debug_cursor_size = 30;
        int mouse_dst_x = p.x - offset_x;
        int mouse_dst_y = p.y - offset_y;

        for (int dst_y = 0; dst_y < m_screenH; dst_y++)
        {
            const int src_y = dst_y + offset_y;

            const ptrdiff_t lineSizeBytes = 4 * m_systemScreenW;
            const ptrdiff_t currentLineOffsetBytes = lineSizeBytes * src_y;
            const ptrdiff_t srcScanLineStart = (ptrdiff_t)m_pCaptureData + (ptrdiff_t)currentLineOffsetBytes;

            const uchar * dstScanLineStartPtr = m_imgScreen.scanLine(dst_y);

            for (int dst_x = 0; dst_x < m_screenW; dst_x++)
            {
                const int src_x = dst_x + offset_x;


                //m_imgScreen.setPixelColor(QPoint(i, y), QColor((10+i + j)%255, abs((256+i-y))%255, (128+i*2+y)%255));

                int color = ((int*)(srcScanLineStart + (ptrdiff_t)(4*src_x)))[0];
                // collor swizzle - swap red-blue
                {
                    char * pchannels = (char*)&color;
                    char blue = pchannels[0];
                    char red = pchannels[2];

                    // Color swizzle
                    pchannels[0] = red;
                    pchannels[2] = blue;
                }

                // DEBUG: draw mouse croshair
                if (debug_draw_mouse_point &&
                    (((abs(dst_x - mouse_dst_x) < debug_cursor_size) && (dst_y == mouse_dst_y)) ||
                     ((dst_x == mouse_dst_x) && (abs(dst_y - mouse_dst_y) < debug_cursor_size))
                    ))
                {
                    color = 0;  // black color for crosshair
                }


                if (1)
                {
                    // Use the faster direct output to canvas
                    uchar * dstPixelPtr = dstScanLineStartPtr + 4*dst_x;
                    int *pDstColor = (int*)dstPixelPtr;

                    pDstColor[0] = color;
                }
                else
                {
                    QColor colorOutput( (QRgb)(color) );
                    m_imgScreen.setPixelColor(QPoint(dst_x, dst_y), colorOutput);
                }
            }
        }
    }
    double current_time5 = m_xtimer.getTime();

    m_time3 += (current_time5 - current_time4);

    m_numFrameRenderings ++;
}


void RasterWindow::renderNow()
{
    if (!isExposed())
        return;

    QRect rect(0, 0, width(), height());
    m_backingStore->beginPaint(rect);

    QPaintDevice *device = m_backingStore->paintDevice();
    QPainter painter(device);

    painter.fillRect(0, 0, width(), height(), Qt::red);

    visualiseScreenImage();

    painter.drawImage(0, 0, m_imgScreen);
    render(&painter);

    m_backingStore->endPaint();
    m_backingStore->flush(rect);
}

void RasterWindow::render(QPainter *painter)
{
    //painter->drawText(QRectF(0, 0, width(), height()), Qt::AlignCenter, QStringLiteral("QWindow"));
}

void RasterWindow::putPixel(float _xc, float _yc, int pixel_size, QColor color)
{
    int xc = (int)_xc;
    int yc = (int)_yc;

    for (int dy = -pixel_size/2; dy <= pixel_size/2; dy++)
    {
        for (int dx = -pixel_size/2; dx <= pixel_size/2; dx++)
        {
            int x = xc + dx;
            int y = yc + dy;

            if ((x >= m_screenW) || (x < 0) || (y < 0) || (y >= m_screenH))
                continue;

            m_imgScreen.setPixelColor( QPoint(x, y), color );
        }
    }
}

void RasterWindow::drawPoint(const Point2D &p, int point_size)
{
    putPixel(p.x, p.y, point_size);
}

void RasterWindow::renderLine(int _x1, int _y1, int _x2, int _y2)
{
    const float K_EPSILON = 0.0005f;

    putPixel(_x1, _y1, 1);
    putPixel(_x2, _y2, 1);

    float x1 = (float)_x1;
    float y1 = (float)_y1;

    float x2 = (float)_x2;
    float y2 = (float)_y2;

    // y = k * x + b
    float k = (y2 - y1) / (x2 - x1);
    if (fabs(x2 - x1) < K_EPSILON)
    {
        k = 0;
    }
    float b = -k*x1 + y1;

    int left_x = std::min(_x1, _x2);
    int right_x = std::max(_x1, _x2);

    for (int my_x = left_x; my_x <= right_x; my_x++)
    {
        float x = (float)my_x;
        float y = k * x + b;
        putPixel(x, y, 1);
    }


    int left_y = std::min(_y1, _y2);
    int right_y = std::max(_y1, _y2);

    for (int my_y = left_y; my_y <= right_y; my_y++)
    {
        float y = (float)my_y;
        // float y = k * x + b;
        float x = (y - b) / k;
        if (k == 0)
        {
            x = 0;
        }
        putPixel(x, y, 1);
    }
}

void RasterWindow::renderLine(const Point2D &v1, const Point2D &v2)
{
    int x1 = v1.x;
    int y1 = v1.y;
    int x2 = v2.x;
    int y2 = v2.y;
    renderLine(x1, y1, x2, y2);
}
