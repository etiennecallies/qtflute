QT += widgets serialport multimedia testlib gui

TARGET = iFlute_gui
TEMPLATE = app

HEADERS += \
    partition.h \
    note.h \
    note_symb.h \
    param_lecture.h \
    motorhelper.h \
    comm.h \
    Accordeur/accordage.h \
    Accordeur/fft.h \
    Accordeur/micro.h \
    flutronic.h

SOURCES += \
    main.cpp \
    partition.cpp \
    note_symb.cpp \
    comm.cpp \
    Accordeur/accordage.cpp \
    Accordeur/fft.cpp \
    Accordeur/micro.cpp \
    flutronic.cpp

FORMS += flutronic.ui

win32: LIBS += -L$$PWD/Accordeur -llibfftw3-3
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/Accordeur/libfftw3-3.lib

INCLUDEPATH += $$PWD/Accordeur
DEPENDPATH += $$PWD/Accordeur
