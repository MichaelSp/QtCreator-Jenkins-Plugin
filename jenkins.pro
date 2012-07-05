TEMPLATE = lib

QT += xml network

TARGET = Jenkins
PROVIDER = CreatorPlugins

DEFINES += JENKINSPLUGIN_LIBRARY

IDE_SOURCE_TREE = C:/dev/Libs/QtSDK/QtSources/qt-creator
IDE_BUILD_TREE = C:/dev/Libs/QtSDK/QtSources/qtcreator-build-desktop

DESTDIR = $$IDE_BUILD_TREE/lib/qtcreator/plugins/$$PROVIDER

LIBS += -L$$IDE_BUILD_TREE/lib/qtcreator/plugins/Nokia

include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)
include($$IDE_SOURCE_TREE/src/plugins/coreplugin/coreplugin.pri)
include($$IDE_SOURCE_TREE/src/plugins/texteditor/texteditor.pri)


SOURCES += jenkinssettingspage.cpp \
    displaywidget.cpp \
    projects.cpp \
    datafetcher.cpp \
    jenkinsplugin.cpp

HEADERS += jenkinsplugin.h \
    jenkinssettingspage.h \
    jenkinsconstants.h \
    displaywidget.h \
    projects.h \
    datafetcher.h

FORMS += \
    jenkinssettingspage.ui

RESOURCES += \
    jenkins.qrc

OTHER_FILES += \
    jenkins.pluginspec Readme.md
