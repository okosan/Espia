#include "xscreenframe.h"

#include <cassert>

XScreenFrame::XScreenFrame()
{

}

void XScreenFrame::init(int frameW, int frameH, int bpp)
{
    m_imageW = frameW;
    m_imageH = frameH;

    m_bpp = bpp;

    m_lineSizeBytes = m_imageW * (m_bpp / 8);

    m_vImageData.resize(m_lineSizeBytes * m_imageH);
    m_frameId = -1;


}

bool XScreenFrame::copyCapturedData(HDC hDcBitmap, HBITMAP hBitmapCapture, int frameId)
{
    char *m_pImageData = (char*)(&(m_vImageData[0]));

    assert(m_bpp == 32);

    BITMAPINFOHEADER bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biPlanes = 1;
    bmi.biBitCount = m_bpp;   // can support 24 bit raster size?
    bmi.biWidth = m_imageW;
    bmi.biHeight = -((long)m_imageH);
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = 0;  // 3 * ScreenX * ScreenY;

    int numScanLinesProcessed = GetDIBits(hDcBitmap, hBitmapCapture, 0, m_imageH,
                                          (void*)m_pImageData,
                                          (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

    m_frameId = frameId;

    if (numScanLinesProcessed != m_imageH)
    {
        return false;
    }
    return true;
}

#if 0
int XScreenFrame::getPixelColorFromCaptureData(int x, int y)
{
    assert(false);
    char *m_pCaptureData = (char*)(&(m_vCaptureData[0]));
    //int color = m_pCaptureData[4 * ((m_systemScreenW * y) + x)];
    int color = ((int*)((ptrdiff_t)m_pCaptureData + (ptrdiff_t)(4 * ((m_systemScreenW * y) + x))))[0];
    return color;
}
#endif
