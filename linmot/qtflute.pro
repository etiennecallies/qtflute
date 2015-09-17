QT += widgets serialport testlib

TARGET = controleSync
TEMPLATE = app

HEADERS += \
    dialog.h \
    masterthread.h

SOURCES += \
    main.cpp \
    dialog.cpp \
    masterthread.cpp
