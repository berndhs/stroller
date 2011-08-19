
TEMPLATE = app

VERSION = $$system( grep "ProgramVersion::VersionNumber" src/version.cpp \
        | awk \'{print $3}\' \
        | sed 's+\[\\(\\"\\;\\)]++g' )

QT += core declarative gui
CONFIG += mobility
MOBILITY += location
MOBILITY += sensors
MOBILITY += systeminfo

OBJECTS_DIR = tmp/obj
MOC_DIR = tmp/moc
RCC_DIR = tmp/rcc

QMAKE_CXXFLAGS += -std=c++0x

message ("VERSION $$VERSION")
message ("compile flags $$QMAKE_CXXFLAGS")
message ("CONFIG $$CONFIG")
message ("MOBILITY $$MOBILITY")

TARGET = stroller

INCLUDEPATH += ./include

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qml/stroller.qml \
    stroller.desktop

SOURCES += \
    src/main.cpp \
    src/geo-ticker.cpp \
    src/geo-test-source.cpp \
    src/orientation.cpp \
    src/version.cpp \
    src/deliberate.cpp \

RESOURCES += \
    stroller.qrc

unix:!symbian:!maemo5 {
    target.path = /opt/stroller/bin
    INSTALLS += target
}

HEADERS += \
    include/geo-ticker.h \
    include/geo-test-source.h \
    include/orientation.h \
    include/version.h \
    include/deliberate.h 

unix:!symbian:!maemo5 {
    desktopfile.files = $${TARGET}.desktop
    desktopfile.path = /usr/share/applications
    INSTALLS += desktopfile
}

unix:!symbian:!maemo5 {
    icon.files = stroller.png
    icon.path = /usr/share/icons/hicolor/64x64/apps
    INSTALLS += icon
}
