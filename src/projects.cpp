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

#include "projects.h"

#include <QStringList>
#include <QDebug>

using namespace Jenkins;
using namespace Jenkins::Internal;

Projects::Projects()
{
    m_connectionError = false;
    m_connectionErrorMessage.clear();
}

bool Projects::hasError() const
{
    foreach (const Project& p, m_list) {
        if (!p.lastBuildOK) return true;
    }
    return false;
}

bool Projects::connectionError() const
{
    return m_connectionError;
}

int Projects::size() const
{
    return m_list.size();
}

int Projects::queueSize() const
{
	return m_queueList.size();
}

Project Projects::project(int i) const
{
    if (i < 0  || i >= size()) return Project();
    return m_list[i];
}

Item Projects::item(int i) const
{
	if (i < 0  || i >= queueSize()) return Item();
	return m_queueList[i];
}

void Projects::setIgnored(const QString& list)
{
    m_ignored.clear();
    QStringList items = list.split(QLatin1String(","));
    foreach (QString it, items){
        it = it.trimmed();
        m_ignored.insert(it);
    }
}

void Projects::add(const Project& newProject)
{
    if (m_ignored.contains(newProject.name)) return;

    m_list.append(newProject);

    emit projectsChanged();
}

void Projects::add(const Item& newItem)
{
	m_queueList.append(newItem);

	emit queueChanged();
}

void Projects::setConnectionError(bool error,const QString& message)
{
    m_connectionError = error;
    m_connectionErrorMessage = message;
    emit projectsChanged();
}

void Projects::clear()
{
    m_list.clear();
	m_queueList.clear();
    m_connectionError = false;
    m_connectionErrorMessage.clear();

    emit projectsChanged();
}
