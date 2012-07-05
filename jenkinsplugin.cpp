/*
Qt creator cruise control plugin
Copyright (C) 2010  David Dibben

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "jenkinsplugin.h"
#include "datafetcher.h"
#include "jenkinssettingspage.h"
#include "displaywidget.h"

#include <utils/synchronousprocess.h>

#include <vcsbase/vcsbaseoutputwindow.h>

#include "projectexplorer/projectexplorer.h"
#include "projectexplorer/project.h"


#include <cppeditor/cppeditorconstants.h>

#include <cpptools/cpptoolsconstants.h>

#include <coreplugin/coreconstants.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/icore.h>
#include <coreplugin/modemanager.h>

#include <QtPlugin>
#include <QAction>
#include <QKeySequence>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QDesktopServices>


using namespace Jenkins;
using namespace Jenkins::Internal;

JenkinsPlugin::JenkinsPlugin() :
    m_settings(new JenkinsSettings),
    m_projects(new Projects),
    m_fetcher(new DataFetcher(20, this)),
    m_timer(new QTimer(this))
{

    connect(m_fetcher, SIGNAL(finished(bool,QString)), this, SLOT(readFinished(bool,QString)));
    connect(m_fetcher, SIGNAL(projectItemReady(Project)),
            m_projects, SLOT(add(Project)));

    connect(m_timer, SIGNAL(timeout()), this, SLOT(refresh()));

    m_reading = false;
}

JenkinsPlugin::~JenkinsPlugin()
{

}

bool JenkinsPlugin::initialize(const QStringList &arguments, QString *error_message)
{
    Q_UNUSED(arguments);
    Q_UNUSED(error_message);

    Core::ICore *core = Core::ICore::instance();

    JenkinsSettingsPage* page = new JenkinsSettingsPage(m_settings);
    addAutoReleasedObject(page);
    connect(page, SIGNAL(settingsUpdated()), this, SLOT(refresh()));

    DisplayWidget* w = new DisplayWidget(m_projects);
    core->modeManager()->addWidget(w);
    connect(w, SIGNAL(refreshRequested()), this, SLOT(refresh()));
    connect(w, SIGNAL(doubleClicked()), this, SLOT(openResults()));

    refresh();

    return true;
}

void  JenkinsPlugin::refresh()
{
    if (m_reading) return;
    m_reading = true;
    m_timer->stop();
    m_projects->clear();
    m_projects->setIgnored(m_settings->ignore);
    m_fetcher->fetch(QUrl(m_settings->url + "/api/xml?depth=1"), m_settings->username, m_settings->password);
}

void  JenkinsPlugin::readFinished(bool error,const QString& message)
{
    m_reading = false;
    m_projects->setConnectionError(error, message);
    int seconds = m_settings->refresh;
    if (seconds < 60) seconds = 60;
    m_timer->start(seconds * 1000);
}

void JenkinsPlugin::openResults()
{
    QString url = m_projects->resultsUrl();
    if (url.isEmpty()) return;

    QDesktopServices::openUrl(url);
}

void JenkinsPlugin::extensionsInitialized()
{
    m_settings->fromSettings(Core::ICore::instance()->settings());

    refresh();
}

void JenkinsPlugin::shutdown()
{
    m_timer->stop();
}


Q_EXPORT_PLUGIN(JenkinsPlugin)


