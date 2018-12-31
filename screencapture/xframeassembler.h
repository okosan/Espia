#ifndef XFRAMEASSEMBLER_H
#define XFRAMEASSEMBLER_H

#include "xscreenframe.h"
#include "xframesegmentation.h"

class XFrameAssembler
{
public:
    XFrameAssembler();

    void init(int W, int H);

    // bool isValid = assembler.addSegment(segment);
    bool addSegment(const XImageSegment & segment);   // TODO: move XImageSegment to other header

    // XScreenFrame &assembledFrame = assembler.getFrame();
    const XScreenFrame& getFrame()
    {
        return m_screenFrame;
    }
private:
    XScreenFrame m_screenFrame;
};

#endif // XFRAMEASSEMBLER_H
