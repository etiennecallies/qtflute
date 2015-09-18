TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

SOURCES += main.cpp \
    partition.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    partition.h \
    note.h \
    accord.h \
    param_lecture.h

