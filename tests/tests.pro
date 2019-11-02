include(../defaults.pri)

QT -= qt core gui

CONFIG   -= app_bundle
CONFIG += c++14 console

LIBS += -L../../libKitsunemimiCommon/src -lKitsunemimiCommon
LIBS += -L../../libKitsunemimiCommon/src/debug -lKitsunemimiCommon
LIBS += -L../../libKitsunemimiCommon/src/release -lKitsunemimiCommon
INCLUDEPATH += ../../libKitsunemimiCommon/include

LIBS += -L../../libKitsunemimiJson/src -lKitsunemimiJson
LIBS += -L../../libKitsunemimiJson/src/debug -lKitsunemimiJson
LIBS += -L../../libKitsunemimiJson/src/release -lKitsunemimiJson
INCLUDEPATH += ../../libKitsunemimiJson/include

INCLUDEPATH += $$PWD

LIBS += -L../src -lKitsunemimiJinja2

SOURCES += \
        main.cpp \
    libKitsunemimiJinja2/jinja2_converter_test.cpp

HEADERS += \
    libKitsunemimiJinja2/jinja2_converter_test.h
