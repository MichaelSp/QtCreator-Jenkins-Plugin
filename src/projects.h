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

#ifndef PROJECTS_H
#define PROJECTS_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QSet>

namespace Jenkins {
namespace Internal {

struct Project
{
    QString	name;
    QString date;
    QString color;
    QString link;
    QString lastBuildUsername;
    int healthInPercent;
    bool lastBuildOK;
    void clear() {
        name.clear();
        date.clear();
        color = QLatin1String("red");
        link.clear();
        lastBuildOK=false;
        healthInPercent=-1;
        lastBuildUsername.clear();
    }
};

struct Item
{
	QString why;
	QString task;
	QString color;
	QString cause;

	QDateTime inQueueSince;

	bool stuck;
	bool blocked;
	bool buildable;
};

class Projects : public QObject
{
Q_OBJECT
public:
        Projects();

        bool	hasError() const;
        bool	connectionError() const;
        QString connectionErrorMessage() const{return m_connectionErrorMessage;}

        int		size() const;
        Project	project(int i) const;

		int		queueSize() const;
		Item	item(int i) const;

        void	setIgnored(const QString& list);

public slots:

        void	setConnectionError(bool error, const QString &message);
        void	add(const Project &newProject);
		void	add(const Item &newItem);
        void	clear();

signals:

        void	projectsChanged();
		void	queueChanged();

private:

        QList<Project>	m_list;
		QList<Item>		m_queueList;
        bool			m_connectionError;
        QString m_connectionErrorMessage;
        QSet<QString>	m_ignored;
};

} // namespace Internal
} // namespace CruiseControl

#endif // PROJECTS_H
