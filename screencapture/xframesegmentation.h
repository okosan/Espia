#ifndef XFRAMESEGMENTATION_H
#define XFRAMESEGMENTATION_H

#include <stdint.h>

#include "xscreenframe.h"

const int SEGMENT_WIDTH = 16;
const int SEGMENT_HEIGHT = 16;
const int SEGMENT_BPP = 2;

struct XImageSegment
{
    uint32_t checksum;
    uint8_t x_index;
    uint8_t y_index;
    uint32_t frame_id;
    //uint8_t pixel_data[SEGMENT_WIDTH * SEGMENT_HEIGHT * SEGMENT_BPP];
    uint16_t pixel_data565[SEGMENT_WIDTH * SEGMENT_HEIGHT];
    // greyscale
    // YUV - 8 bit + 4 bit - > 12
    // YUV - 8 bit + 2 bit - > 10
};

class XFrameSegmentation
{
public:
    XFrameSegmentation();

    void lockFrame(const XScreenFrame &frame);
    bool getNextSegment(XImageSegment *pRetSegment);

private:
    XScreenFrame m_frame;
    int m_nextSegmentNumX;
    int m_nextSegmentNumY;

    int m_maxIndexX;
    int m_maxIndexY;

    bool m_allowSegmentation;
};

uint16_t convertXBGR8888toRGB565(uint32_t color_xbgr8888);
uint32_t convertRGB565toXRGB8888(uint16_t color_rgb565);

#endif // XFRAMESEGMENTATION_H
