QT += core serialport network

CONFIG += console c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = radio-daemon

SOURCES += \
    main.cpp \
    protocol.cpp \
    radiodaemon.cpp

HEADERS += \
    protocol.h \
    radiodaemon.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
