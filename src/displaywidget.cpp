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

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QFont>
#include <QtGui/QPixmap>
#include <QHelpEvent>
#include <QToolTip>
#include <QAction>

using namespace Jenkins;
using namespace Jenkins::Internal;



DisplayWidget::DisplayWidget(Projects* projects) :
    m_errorIcon(new QLabel(this)),
    m_passIcon(new QLabel(this)),
    m_noConnectionIcon(new QLabel(this)),
    m_projects(projects)
{

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
    errorLayout->addStretch(2);


    m_errorIcon->setAlignment(Qt::AlignCenter);
    m_passIcon->setAlignment(Qt::AlignLeft);

    m_errorIcon->setPixmap(QPixmap(":/jenkins/resources/red-ball.png"));
    m_noConnectionIcon->setPixmap(QPixmap(":/jenkins/resources/red-cross.png"));
    m_passIcon->setPixmap(QPixmap(":/jenkins/resources/green-ball.png"));

    connect(m_projects, SIGNAL(projectsChanged()), this, SLOT(updateState()));

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
    } else {
        m_noConnectionIcon->setHidden(true);
        m_errorIcon->setHidden(false);
        m_passIcon->setHidden(false);
        bool error = m_projects->hasError();
        int numProjects = m_projects->size();
        m_passIcon->setEnabled(!error && numProjects > 0);
        m_errorIcon->setEnabled(error && numProjects > 0);
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
    if (m_projects == 0) return "";

    if (m_projects->connectionError()) {
        return QString(tr("Cannot connect to server: ")) + m_projects->connectionErrorMessage();
    }

    int numProjects = m_projects->size();
    if (numProjects == 0) {
        return tr("no projects");
    }

    QString rc;
    rc +="<table width=\"600\">\n<tr>";
    rc += QString("<td>Project Name</td>");
    rc += QString("<td>Project Health</td>");
    rc += QString("<td>Last Build Date</td>");
    rc += QString("<td>Last Build User</td>");
    rc +="</tr>\n";
    for (int i = 0 ; i < numProjects; i++) {
        Project proj = m_projects->project(i);
        QString color = proj.lastBuildOK ? "green" : "red";

        rc +="<tr>";
        rc += QString("<td><font color=\"%1\">%2</font></td>").arg(proj.color).arg(proj.name);
        rc += QString("<td><font color=\"%1\">%2%</font></td>").arg(color).arg(proj.healthInPercent);
        rc += QString("<td><font color=\"%1\">%2</font></td>").arg(color).arg(proj.date);
        rc += QString("<td><font color=\"%1\">%2</font></td>").arg(color).arg(proj.lastBuildUsername);
        rc +="</tr>\n";
    }
    rc +="</table>\n";

    return rc;
}

