QT += widgets serialport multimedia testlib

TARGET = iFlute
TEMPLATE = app

HEADERS += \
    dialog.h \
    masterthread.h \
    partition.h \
    note.h \
    note_symb.h \
    param_lecture.h \
    motorhelper.h \
    comm.h \
    Accordeur/accordage.h \
    Accordeur/fft.h \
    Accordeur/micro.h

SOURCES += \
    main.cpp \
    dialog.cpp \
    masterthread.cpp \
    partition.cpp \
    note_symb.cpp \
    comm.cpp \
    Accordeur/accordage.cpp \
    Accordeur/fft.cpp \
    Accordeur/micro.cpp

win32: LIBS += -L$$PWD/Accordeur -llibfftw3-3
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/Accordeur/libfftw3-3.lib

INCLUDEPATH += $$PWD/Accordeur
DEPENDPATH += $$PWD/Accordeur
