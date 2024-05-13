QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

TARGET = ./bin/random_graph
OBJECTS_DIR = ./obj
MOC_DIR = ./obj

SOURCES += \
    configdialog.cpp \
    coordinatesgenerator.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    configdialog.h \
    coordinatesgenerator.h \
    mainwindow.h \
    qcustomplot.h

FORMS += \
    configdialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
