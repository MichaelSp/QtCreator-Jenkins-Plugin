TEMPLATE = lib

QT += xml network

TARGET = Jenkins
PROVIDER = Nokia

DEFINES += JENKINSPLUGIN_LIBRARY

IDE_SOURCE_TREE = /home/hgiraud/Dev/qt-creator-2.4.1-src/src
IDE_BUILD_TREE = /home/hgiraud/Dev/qt-creator-2.4.1-src/bin

DESTDIR = .

LIBS += -L/home/hgiraud/Dev/qt-creator-2.4.1-src/lib/qtcreator -L/home/hgiraud/Dev/qt-creator-2.4.1-src/lib/qtcreator/plugins/Nokia

include($$IDE_SOURCE_TREE/qtcreatorplugin.pri)
include($$IDE_SOURCE_TREE/src/plugins/coreplugin/coreplugin.pri)
include($$IDE_SOURCE_TREE/src/plugins/texteditor/texteditor.pri)


SOURCES += src/jenkinssettingspage.cpp \
    src/displaywidget.cpp \
    src/projects.cpp \
    src/datafetcher.cpp \
    src/jenkinsplugin.cpp

HEADERS += src/jenkinsplugin.h \
    src/jenkinssettingspage.h \
    src/jenkinsconstants.h \
    src/displaywidget.h \
    src/projects.h \
    src/datafetcher.h

FORMS += \
    src/jenkinssettingspage.ui

RESOURCES += \
    jenkins.qrc

OTHER_FILES += \
    jenkins.pluginspec Readme.md
