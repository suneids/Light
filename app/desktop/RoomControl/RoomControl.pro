QT       += core gui serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    greenhouse.cpp \
    led_strip.cpp \
    ledpointwidget.cpp \
    ledsegmentwidget.cpp \
    light.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow_radio.cpp \
    planner.cpp \
    protocol.cpp \
    timetracker.cpp

HEADERS += \
    greenhouse.h \
    led_strip.h \
    ledpointwidget.h \
    ledsegmentwidget.h \
    light.h \
    mainwindow.h \
    planner.h \
    protocol.h \
    timetracker.h

FORMS += \
    TaskItem.ui \
    greenhouse.ui \
    led_strip.ui \
    ledpointwidget.ui \
    ledsegmentwidget.ui \
    mainwindow.ui \
    planner.ui \
    timetracker.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
