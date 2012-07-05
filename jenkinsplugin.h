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

#ifndef CRUISECONTROLPLUGIN_H
#define CRUISECONTROLPLUGIN_H

#include "projects.h"
#include "jenkinssettingspage.h"
#include <extensionsystem/iplugin.h>


#include <QSharedPointer>
#include <QStringList>
#include <QTimer>

namespace Jenkins {
namespace Internal {

struct JenkinsSettings;
class DataFetcher;

class JenkinsPlugin  : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    JenkinsPlugin();
    ~JenkinsPlugin();

    bool initialize(const QStringList &arguments, QString *error_message);
    void extensionsInitialized();
    void shutdown();


private slots:

    void	refresh();
    void	readFinished(bool error);
    void	openResults();

private:


    QSharedPointer<JenkinsSettings>	m_settings;
    Projects*						m_projects;
    DataFetcher*					m_fetcher;
    QTimer*							m_timer;
    bool							m_reading;
};

} // namespace Internal
} // namespace CruiseControl

#endif // CruiseControlPLUGIN_H
