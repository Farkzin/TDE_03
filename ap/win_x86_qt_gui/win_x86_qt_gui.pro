QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    ../common/behavior.cpp \
    ../common/chronometer.cpp \
    ../common/sm1.cpp \
    ../common/sm2.cpp \
    ../common/sm3.cpp \
    ../common/sm4.cpp

HEADERS += \
    ../common/behavior.h \
    ../common/chronometer.h \
    ../common/definitions.h \
    ../common/hal.h \
    ../common/sm.h \
    ../common/sm1.h \
    ../common/sm2.h \
    ../common/sm3.h \
    ../common/sm4.h

SOURCES += \
    behaviorthread.cpp \
    cedatank.cpp \
    hal.cpp \
    main.cpp \
    mainwindow.cpp \
    pipesview.cpp \
    plant.cpp

HEADERS += \
    behaviorthread.h \
    cedatank.h \
    led.h \
    mainwindow.h \
    pipesview.h \
    plant.h \
    sim_pins.h

FORMS += \
    mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
