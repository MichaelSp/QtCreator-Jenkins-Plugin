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
    m_connectionErrorMessage = "";
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

QString Projects::name(int i) const
{
    if (i < 0  || i >= size()) return QString();
    return m_list[i].name;
}

int Projects::healthInPercent(int i) const
{
    if (i < 0  || i >= size()) return -1;
    return m_list[i].healthInPercent;
}

bool Projects::lastBuildOK(int i) const
{
    if (i < 0  || i >= size()) return false;
    return m_list[i].lastBuildOK;
}

QString Projects::link(int i) const
{
    if (i < 0  || i >= size()) return QString();
    return m_list[i].link;
}

void Projects::setIgnored(const QString& list)
{
    m_ignored.clear();
    QStringList items = list.split(",");
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

void Projects::setConnectionError(bool error,const QString& message)
{
    m_connectionError = error;
    m_connectionErrorMessage = message;
    emit projectsChanged();
}

void Projects::clear()
{
    m_list.clear();
    m_connectionError = false;
    m_connectionErrorMessage.clear();

    emit projectsChanged();
}


QString	Projects::resultsUrl() const
{
    if (size() == 0) return QString();

    QString linkstr = link(0);

    //drop last 2 items from url
    int pos = linkstr.lastIndexOf('/');
    if (pos == -1) return QString();
    pos = linkstr.lastIndexOf('/', pos - 1);
    if (pos == -1) return QString();
    return linkstr.left(pos);
}
