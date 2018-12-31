#include "xframeassembler.h"

XFrameAssembler::XFrameAssembler()
{

}

void XFrameAssembler::init(int W, int H)
{
    m_screenFrame.init(W, H);
}

bool XFrameAssembler::addSegment(const XImageSegment & segment)
{
    //segment.frame_id;
    //segment.checksum   // TODO !!!


    int index = 0;

    int segmentStartX = segment.x_index * SEGMENT_WIDTH;
    int segmentEndX = (segment.x_index + 1) * SEGMENT_WIDTH;

    int segmentStartY = segment.y_index * SEGMENT_HEIGHT;
    int segmentEndY = (segment.y_index + 1) * SEGMENT_HEIGHT;

    for (int y = segmentStartY; y < segmentEndY; y++)
    {
        char * dstScanLineStartPtr = m_screenFrame.getWriteableImageDataLinePtr(y);

        for (int x = segmentStartX; x < segmentEndX; x++)
        {
            uint16_t colorRGB565 = segment.pixel_data565[index];
            index++;

            if (x >= m_screenFrame.getWidth() || y >= m_screenFrame.getHeight())
                continue;

            int colorXRGB8888 = convertRGB565toXRGB8888(colorRGB565);

            char * dstPixelPtr = dstScanLineStartPtr + 4*x;

            int *pDstColor = (int*)dstPixelPtr;
            pDstColor[0] = colorXRGB8888;
        }
    }

    return true;
}
