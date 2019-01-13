include(../defaults.pri)

QT -= qt core gui

CONFIG   -= app_bundle
CONFIG += c++14 console

LIBS += -L../../libKitsuneCommon -lKitsuneCommon
LIBS += -L../../libKitsuneCommon/debug -lKitsuneCommon
LIBS += -L../../libKitsuneCommon/release -lKitsuneCommon
INCLUDEPATH += ../../libKitsuneCommon/include/libKitsuneCommon

INCLUDEPATH += $$PWD

LIBS += -L../src -lKitsuneJinja2

SOURCES += \
        main.cpp \
        jinja2ConverterTest.cpp

HEADERS += \
        jinja2ConverterTest.h
