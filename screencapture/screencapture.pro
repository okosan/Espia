
CONFIG += c++11
TARGET = screencapture

INCLUDEPATH += $$PWD
SOURCES += $$PWD/rasterwindow.cpp \
    xframesegmentation.cpp \
    xscreenframe.cpp \
    xframeassembler.cpp

HEADERS += $$PWD/rasterwindow.h \
    xtiming.h \
    xframesegmentation.h \
    xscreenframe.h \
    xframeassembler.h

SOURCES += \
    main.cpp

LIBS += -lgdi32

