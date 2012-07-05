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
#include <QSet>

namespace Jenkins {
namespace Internal {

struct Project
{
	QString	name;
	QString date;
	bool passed;
	QString link;
};

class Projects : public QObject
{
Q_OBJECT
public:
		Projects();

		bool	hasError() const;
		bool	connectionError() const;

		int		size() const;
		QString	name(int i) const;
		QString	date(int i) const;
		bool	passed(int i) const;
		QString	link(int i) const;

		void	setIgnored(const QString& list);

		QString	resultsUrl() const;

public slots:

		void	setConnectionError(bool error);
		void	add(const QString& name, const QString& date, bool passed, const QString& link);
		void	clear();

signals:

		void	projectsChanged();

private:

		QList<Project>	m_list;
		bool			m_connectionError;
		QSet<QString>	m_ignored;
};

} // namespace Internal
} // namespace CruiseControl

#endif // PROJECTS_H
