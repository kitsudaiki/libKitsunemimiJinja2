include(../defaults.pri)

QT -= qt core gui

CONFIG   -= app_bundle
CONFIG += c++14 console

LIBS += -L../../libKitsuneCommon/src -lKitsuneCommon
LIBS += -L../../libKitsuneCommon/src/debug -lKitsuneCommon
LIBS += -L../../libKitsuneCommon/src/release -lKitsuneCommon
INCLUDEPATH += ../../libKitsuneCommon/include

LIBS += -L../../libKitsuneJson/src -lKitsuneJson
LIBS += -L../../libKitsuneJson/src/debug -lKitsuneJson
LIBS += -L../../libKitsuneJson/src/release -lKitsuneJson
INCLUDEPATH += ../../libKitsuneJson/include

INCLUDEPATH += $$PWD

LIBS += -L../src -lKitsuneJinja2

SOURCES += \
        main.cpp \
    libKitsuneJinja2/jinja2_converter_test.cpp

HEADERS += \
    libKitsuneJinja2/jinja2_converter_test.h
