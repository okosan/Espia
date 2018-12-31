QT += network widgets

CONFIG += c++11

TARGET = espia

HEADERS += httpwindow.h
SOURCES += httpwindow.cpp \
           main.cpp
FORMS += authenticationdialog.ui
