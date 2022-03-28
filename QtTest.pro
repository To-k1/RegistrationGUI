QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Registrating.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Registrating.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    QtTest_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


INCLUDEPATH += \
D:/OpenCV3.4.16/opencv/build/include \
D:/OpenCV3.4.16/opencv/build/include/opencv \
D:/OpenCV3.4.16/opencv/build/include/opencv2

win32:CONFIG(release, debug|release): LIBS += D:/OpenCV3.4.16/opencv/build/x64/vc14/lib/opencv_world3416.lib
else:win32:CONFIG(debug, debug|release): LIBS += D:/OpenCV3.4.16/opencv/build/x64/vc14/lib/opencv_world3416d.lib

UI_DIR=./

RESOURCES += \
    res.qrc

