#ifndef XSCREENFRAME_H
#define XSCREENFRAME_H

#include <vector>

#include <windows.h>

class XScreenFrame
{
public:
    XScreenFrame();

    void init(int frameW, int frameH, int bpp = 32);

    bool copyCapturedData(HDC hDcBitmap, HBITMAP hBitmapCapture, int frameId);

    const char * getImageDataPtr() const
    {
        return (char*)(&(m_vImageData[0]));
    }

    const char * getImageDataLinePtr(int lineNumber) const
    {
        if (lineNumber >= m_imageH)
            return NULL;

        return (char*)(&(m_vImageData[0])) + m_lineSizeBytes * lineNumber;
    }

    char * getWriteableImageDataPtr()
    {
        return (char*)(&(m_vImageData[0]));
    }

    char * getWriteableImageDataLinePtr(int lineNumber)
    {
        if (lineNumber >= m_imageH)
            return NULL;

        return (char*)(&(m_vImageData[0])) + m_lineSizeBytes * lineNumber;
    }

    int getFrameId()
    {
        return m_frameId;
    }

    int getWidth()
    {
        return m_imageW;
    }

    int getHeight()
    {
        return m_imageH;
    }

private:
    std::vector<char> m_vImageData;

    int m_imageW;
    int m_imageH;
    int m_bpp;
    int m_frameId;

    ptrdiff_t m_lineSizeBytes;
};

#endif // XSCREENFRAME_H
