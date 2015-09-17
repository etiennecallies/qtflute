QT += widgets serialport testlib

TARGET = backup_serial
TEMPLATE = app

HEADERS += \
    dialog.h \
    masterthread.h

SOURCES += main.cpp \
    dialog.cpp \
    masterthread.cpp \

