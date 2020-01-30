#-------------------------------------------------
#
# Project created by QtCreator 2019-03-26T10:19:29
#
#-------------------------------------------------

QT       += gui widgets network

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

HEADERS += \
    src/mainwindow.h \
    src/owidget.h

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/owidget.cpp

FORMS += \
    src/mainwindow.ui

RESOURCES += \
    src/shaders.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libmueb/release/ -lmueb
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libmueb/debug/ -lmueb
else:unix: LIBS += -L$$PWD/../libmueb/ -lmueb

INCLUDEPATH += $$PWD/../libmueb/include/libmueb
DEPENDPATH += $$PWD/../libmueb/include/libmueb
