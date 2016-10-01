QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GwQv2
TEMPLATE = app

INCLUDEPATH += K:/Qt/Tools/qwt-6.1.3/include
LIBS += -LK:/Qt/Tools/qwt-6.1.3/lib -lqwt

CONFIG += qwt # QWT for visualisation

SOURCES += main.cpp\
        mainwindow.cpp \
    realtimeplot.cpp

HEADERS  += mainwindow.h \
    realtimeplot.h

FORMS    += mainwindow.ui

RESOURCES += \
    resourses.qrc
