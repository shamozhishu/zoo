#-------------------------------------------------
#
# Project created by QtCreator 2020-06-03T09:33:49
#
#-------------------------------------------------

QT       += widgets opengl xml

QT       -= gui

TARGET = libctk_zoocmd_ui
TEMPLATE = lib

DEFINES += LIBCTK_ZOOCMD_UI_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += ../../../../plugins/libctk_zoocmd_ui/SetupDlg.cpp \
    ../../../../plugins/libctk_zoocmd_ui/UIActivator.cpp \
    ../../../../plugins/libctk_zoocmd_ui/ZooCmdUI.cpp \
    ../../../../plugins/libctk_zoocmd_ui/ZooCmdWidget.cpp

HEADERS += ../../../../plugins/libctk_zoocmd_ui/SetupDlg.h \
    ../../../../plugins/libctk_zoocmd_ui/UIActivator.h \
    ../../../../plugins/libctk_zoocmd_ui/ZooCmdUI.h \
    ../../../../plugins/libctk_zoocmd_ui/ZooCmdWidget.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    ../../../../plugins/libctk_zoocmd_ui/libctk_zoocmd_ui.qrc \
    ../../../../plugins/libctk_zoocmd_ui/SetupDlg.qrc \
    ../../../../plugins/libctk_zoocmd_ui/ZooCmdUI.qrc \
    ../../../../plugins/libctk_zoocmd_ui/MANIFEST.MF

FORMS += \
    ../../../../plugins/libctk_zoocmd_ui/SetupDlg.ui \
    ../../../../plugins/libctk_zoocmd_ui/ZooCmdUI.ui
