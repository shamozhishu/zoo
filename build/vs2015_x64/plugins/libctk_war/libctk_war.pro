#-------------------------------------------------
#
# Project created by QtCreator 2020-06-03T10:24:15
#
#-------------------------------------------------

QT       -= gui

TARGET = libctk_war
TEMPLATE = lib

DEFINES += LIBCTK_WAR_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../../plugins/libctk_war/AllyArmy.cpp \
    ../../../../plugins/libctk_war/Battlefield.cpp \
    ../../../../plugins/libctk_war/BlueArmy.cpp \
    ../../../../plugins/libctk_war/Effect.cpp \
    ../../../../plugins/libctk_war/InstanceReleaser.cpp \
    ../../../../plugins/libctk_war/LuaScript.cpp \
    ../../../../plugins/libctk_war/RedArmy.cpp \
    ../../../../plugins/libctk_war/Stationary.cpp \
    ../../../../plugins/libctk_war/WarActivator.cpp \
    ../../../../plugins/libctk_war/WarCommander.cpp \
    ../../../../plugins/libctk_war/WarComponents.cpp \
    ../../../../plugins/libctk_war/WarReporter.cpp \
    ../../../../plugins/libctk_war/WarServiceImpl.cpp \
    ../../../../plugins/libctk_war/WarSimulator.cpp \
    ../../../../plugins/libctk_war/Weapon.cpp

HEADERS += \
    ../../../../plugins/libctk_war/AllyArmy.h \
    ../../../../plugins/libctk_war/Battlefield.h \
    ../../../../plugins/libctk_war/BlueArmy.h \
    ../../../../plugins/libctk_war/CommonDef.h \
    ../../../../plugins/libctk_war/Effect.h \
    ../../../../plugins/libctk_war/InstanceReleaser.h \
    ../../../../plugins/libctk_war/LuaScript.h \
    ../../../../plugins/libctk_war/PublicEnum.h \
    ../../../../plugins/libctk_war/RedArmy.h \
    ../../../../plugins/libctk_war/Stationary.h \
    ../../../../plugins/libctk_war/WarActivator.h \
    ../../../../plugins/libctk_war/WarCommander.h \
    ../../../../plugins/libctk_war/WarComponents.h \
    ../../../../plugins/libctk_war/WarReporter.h \
    ../../../../plugins/libctk_war/WarServiceImpl.h \
    ../../../../plugins/libctk_war/WarSimulator.h \
    ../../../../plugins/libctk_war/Weapon.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    ../../../../plugins/libctk_war/libctk_war.qrc \
    ../../../../plugins/libctk_war/MANIFEST.MF \
    ../../../../plugins/libctk_war/ToLuaWar.pkg
