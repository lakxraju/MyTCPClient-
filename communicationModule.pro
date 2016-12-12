TEMPLATE = app
TARGET = communicationModule

QT = core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp \
    sockettest.cpp

HEADERS += \
    corewindow.h \
    sockettest.h
