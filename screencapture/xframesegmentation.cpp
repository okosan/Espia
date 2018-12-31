#include "xframesegmentation.h"

XFrameSegmentation::XFrameSegmentation()
{
    m_maxIndexX = 0;
    m_maxIndexY = 0;
    m_allowSegmentation = false;
}


void XFrameSegmentation::lockFrame(const XScreenFrame &frame)
{
    m_frame = frame;
    m_nextSegmentNumX = 0;
    m_nextSegmentNumY = 0;

    m_maxIndexX = (m_frame.getWidth() + SEGMENT_WIDTH - 1) / SEGMENT_WIDTH - 1;
    m_maxIndexY = (m_frame.getHeight() + SEGMENT_HEIGHT - 1) / SEGMENT_HEIGHT - 1;

    m_allowSegmentation = true;
}

bool XFrameSegmentation::getNextSegment(XImageSegment *pRetSegment)
{
    if (!m_allowSegmentation)
    {
        return false;
    }

    pRetSegment->x_index = m_nextSegmentNumX;
    pRetSegment->y_index = m_nextSegmentNumY;
    pRetSegment->frame_id = m_frame.getFrameId();

    int index = 0;
    //m_frame.getImageDataPtr()

    int segmentOriginStartX = pRetSegment->x_index * SEGMENT_WIDTH;
    int segmentOriginStartY = pRetSegment->y_index * SEGMENT_HEIGHT;

    for (int y = 0; y < SEGMENT_HEIGHT; y++)
    {
        const char * srcScanLineStartPtr = m_frame.getImageDataLinePtr(segmentOriginStartY + y);
        if (srcScanLineStartPtr == NULL)
            continue;  // TODO: Needed to properly handle non-rounded resolutions

        for (int x = 0; x < SEGMENT_WIDTH; x++)
        {
            int colorXBGR8888 = ((int*)(srcScanLineStartPtr + 4 * (segmentOriginStartX + x)))[0];

            uint16_t color565 = convertXBGR8888toRGB565(colorXBGR8888);

            //pRetSegment->pixel_data565[index] = ((m_nextSegmentNumY % 64) << 5)  + (m_nextSegmentNumX % 32);    // W * H
            pRetSegment->pixel_data565[index] = color565;

            index++;
        }
    }

#if 0
    for (int y = 0; y < SEGMENT_HEIGHT; y++)
    {
        pRetSegment->pixel_data565[y * SEGMENT_HEIGHT + 0] = 0xFFFF;
        //pRetSegment->pixel_data565[y * SEGMENT_HEIGHT + SEGMENT_WIDTH-1] = 0xFFFF;
    }

    for (int x = 0; x < SEGMENT_WIDTH; x++)
    {
        pRetSegment->pixel_data565[0 * SEGMENT_HEIGHT + x] = 0xFFFF;
        //pRetSegment->pixel_data565[(SEGMENT_WIDTH - 1) * SEGMENT_HEIGHT + x] = 0xFFFF;
    }
#endif


    pRetSegment->checksum = 0;  // TODO: calculate checksum!!!

    // update number of next segment at X/Y
    m_nextSegmentNumX++;
    if (m_nextSegmentNumX > m_maxIndexX)
    {
        m_nextSegmentNumX = 0;

        m_nextSegmentNumY++;
        if (m_nextSegmentNumY > m_maxIndexY)
        {
            m_allowSegmentation = false;
        }
    }

    return true;
}


uint16_t convertXBGR8888toRGB565(uint32_t color_xbgr8888)
{
    uint8_t * pchannels = (uint8_t*)&color_xbgr8888;
    uint16_t blue = (uint16_t)(pchannels[0]);
    uint16_t green = (uint16_t)(pchannels[1]);
    uint16_t red = (uint16_t)(pchannels[2]);

    blue = blue >> 3;    // 0..255 / (2^3) ---> 0..31  (5 bits)
    green = green >> 2;  // 0..255 / (2^2) ---> 0..63  (6 bits)
    red = red >> 3;      // 0..255 / (2^3) ---> 0..31  (5 bits)

    uint16_t rgb565 = (red << 11) | (green << 5) | blue;

    return rgb565;
}

uint32_t convertRGB565toXRGB8888(uint16_t color_rgb565)
{
    uint16_t blue5 = color_rgb565 & 31;
    uint16_t green6 = (color_rgb565 & 2016) >> 5;
    uint16_t red5 = (color_rgb565 & 63488) >> 11;

    uint32_t blue8  = blue5 * 8;
    uint32_t green8 = green6 * 4;
    uint32_t red8   = red5 * 8;

    uint32_t xrgb8888 = (red8 << 16) | (green8 << 8) | blue8;
    return xrgb8888;
}
