QT += widgets serialport testlib

TARGET = controleSync
TEMPLATE = app

HEADERS += \
    dialog.h \
    masterthread.h \
    partition.h \
    note.h \
    accord.h \
    param_lecture.h \
    motorhelper.h \
    comm.h

SOURCES += \
    main.cpp \
    dialog.cpp \
    masterthread.cpp \
    partition.cpp \
    motorhelper.cpp \
    comm.cpp

