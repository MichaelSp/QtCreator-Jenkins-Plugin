TEMPLATE = lib

QT += xml network

TARGET = Jenkins
PROVIDER = CreatorPlugins

DEFINES += JENKINSPLUGIN_LIBRARY

IDE_SOURCE_TREE = $$(QTC_SOURCE)
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_SOURCE_TREE):IDE_SOURCE_TREE=C:/dev/Libs/QtSDK/QtSources/qt-creator
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=C:/dev/Libs/QtSDK/QtSources/qtcreator-build-desktop

DESTDIR = $$IDE_BUILD_TREE/lib/qtcreator/plugins/$$PROVIDER

LIBS += -L$$IDE_BUILD_TREE/lib/qtcreator/plugins/Nokia

include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)
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
