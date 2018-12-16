QT += core gui network widgets sql

TARGET = texturemapper
TEMPLATE = app
SOURCES += main.cpp logger.hpp sqlite.hpp
HEADERS += main.h common.hpp
RESOURCES = Resources.qrc
FORMS += texturemapper.ui
CONFIG += c++17

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
