
CONFIG += c++11
TARGET = screencapture

INCLUDEPATH += $$PWD
SOURCES += $$PWD/rasterwindow.cpp \
    xscreenframe.cpp

HEADERS += $$PWD/rasterwindow.h \
    xtiming.h \
    xscreenframe.h

SOURCES += \
    main.cpp

LIBS += -lgdi32

