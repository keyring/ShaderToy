TEMPLATE = app

QT += qml quick widgets
CONFIG += c++11

SOURCES += main.cpp \
    codeEditor.cpp \
    playground.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    codeEditor.h \
    playground.h
