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

#include "displaywidget.h"

#include <utils/stylehelper.h>

#include <QWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QPixmap>
#include <QHelpEvent>
#include <QToolTip>
#include <QAction>
#include <QApplication>

using namespace Jenkins;
using namespace Jenkins::Internal;



DisplayWidget::DisplayWidget(Projects* projects) :
    m_errorIcon(new QLabel(this)),
    m_passIcon(new QLabel(this)),
    m_noConnectionIcon(new QLabel(this)),
	m_queue(new QLabel(this)),
    m_projects(projects)
{
	QFont font = QApplication::font();
	font.setWeight( 10 );
	m_queue->setFont( font );
	m_queue->setStyleSheet(QLatin1String("color: white;"));

    setContextMenuPolicy(Qt::ActionsContextMenu);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(8, 2, 0, 2);
    layout->setSpacing(2);
    setLayout(layout);
    QHBoxLayout *errorLayout = new QHBoxLayout;
    errorLayout->setSpacing(2);
    layout->addLayout(errorLayout);
    errorLayout->addStretch(1);
    errorLayout->addWidget(m_errorIcon);
    errorLayout->addWidget(m_noConnectionIcon);
    errorLayout->addWidget(m_passIcon);
	errorLayout->addStretch(1);
	errorLayout->addWidget(m_queue);
	errorLayout->addStretch(2);


    m_errorIcon->setAlignment(Qt::AlignCenter);
    m_passIcon->setAlignment(Qt::AlignLeft);

    m_errorIcon->setPixmap(QPixmap(QLatin1String(":/jenkins/resources/red-ball.png")));
    m_noConnectionIcon->setPixmap(QPixmap(QLatin1String(":/jenkins/resources/red-cross.png")));
    m_passIcon->setPixmap(QPixmap(QLatin1String(":/jenkins/resources/green-ball.png")));

    connect(m_projects, SIGNAL(projectsChanged()), this, SLOT(updateState()));
	connect(m_projects, SIGNAL(queueChanged()), this, SLOT(updateState()));

    QAction* refresh = new QAction(tr("Refresh"),this);
    connect(refresh, SIGNAL(triggered()), this, SIGNAL(refreshRequested()));
    addAction(refresh);

    updateState();
}

void DisplayWidget::updateState()
{
    if (m_projects == 0) return;

    if (m_projects->connectionError()) {
        m_noConnectionIcon->setHidden(false);
        m_errorIcon->setHidden(true);
        m_passIcon->setHidden(true);
		m_queue->setHidden(true);
    } else {
        m_noConnectionIcon->setHidden(true);
        m_errorIcon->setHidden(false);
        m_passIcon->setHidden(false);
        bool error = m_projects->hasError();
        int numProjects = m_projects->size();
        m_passIcon->setEnabled(!error && numProjects > 0);
        m_errorIcon->setEnabled(error && numProjects > 0);
		int queue = m_projects->queueSize();
		m_queue->setEnabled( true );
		m_queue->setText( QString::number(queue) );
    }
}

bool DisplayWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent*>(event);

        QToolTip::showText(helpEvent->globalPos(), tooltipText());
        return true;
    }
    return QWidget::event(event);
}

void DisplayWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    emit doubleClicked();
}

QString	DisplayWidget::tooltipText() const
{
    if (m_projects == 0) return QString();

    if (m_projects->connectionError()) {
        return QString(tr("Cannot connect to server: ")) + m_projects->connectionErrorMessage();
    }

    int numProjects = m_projects->size();
    if (numProjects == 0) {
        return tr("no projects");
    }

    QString rc;

	if(m_projects->queueSize())
	{
		rc += QLatin1String("<table width=\"600\">\n");
		rc += QLatin1String("<caption>Current builds</caption>\n");
		rc += QLatin1String("<tr><td>Project Name</td>");
		rc += QLatin1String("<td>Why</td>");
		rc += QLatin1String("<td>Blocked</td>");
		rc += QLatin1String("<td>Buildable</td>");
		rc += QLatin1String("</tr>\n");
		for (int i = 0 ; i < m_projects->queueSize(); i++) {
			Item item = m_projects->item(i);

			rc += QLatin1String("<tr>");

			rc += QString::fromLatin1("<td>%1</td>").arg(item.task);
			rc += QString::fromLatin1("<td>%1</td>").arg(item.why);
			rc += QString::fromLatin1("<td>%1</td>").arg(QLatin1String(item.blocked ? "Yes" : "No"));
			rc += QString::fromLatin1("<td>%1</td>").arg(QLatin1String(item.buildable ? "Yes" : "No"));

			rc += QLatin1String("</tr>");
		}
		rc += QLatin1String("</table>");
	}

	rc +=QLatin1String("<table width=\"600\">\n<tr>");
    rc += QString(QLatin1String("<td>Project Name</td>"));
    rc += QString(QLatin1String("<td>Project Health</td>"));
    rc += QString(QLatin1String("<td>Last Build Date</td>"));
    rc += QString(QLatin1String("<td>Last Build User</td>"));
    rc += QLatin1String("</tr>\n");
    for (int i = 0 ; i < numProjects; i++) {
        Project proj = m_projects->project(i);
        QString color = QLatin1String(proj.lastBuildOK ? "green" : "red");

        rc +=QLatin1String("<tr>");
        rc += QString::fromLatin1("<td><font color=\"%1\">%2</font></td>").arg(proj.color).arg(proj.name);
        rc += QString::fromLatin1("<td><font color=\"%1\">%2%</font></td>").arg(color).arg(proj.healthInPercent);
        rc += QString::fromLatin1("<td><font color=\"%1\">%2</font></td>").arg(color).arg(proj.date);
        rc += QString::fromLatin1("<td><font color=\"%1\">%2</font></td>").arg(color).arg(proj.lastBuildUsername);
        rc += QLatin1String("</tr>\n");
    }
    rc += QLatin1String("</table>\n");

    return rc;
}

