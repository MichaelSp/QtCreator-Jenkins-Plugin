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

#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include "projects.h"

#include <QWidget>
#include <QLabel>

namespace Jenkins {
namespace Internal {

class DisplayWidget : public QWidget
{
Q_OBJECT
public:
	DisplayWidget(Projects* projects);


	QString	tooltipText() const;

signals:

	void	refreshRequested();
	void	doubleClicked();

protected:

	bool	event(QEvent* event);
	void	mouseDoubleClickEvent(QMouseEvent * event);

private slots:

	void	updateState();

private:

	QLabel	*m_errorIcon;
	QLabel	*m_passIcon;
	QLabel	*m_noConnectionIcon;
	Projects *m_projects;
};

} // namespace Internal
} // namespace CruiseControl

#endif // DISPLAYWIDGET_H
