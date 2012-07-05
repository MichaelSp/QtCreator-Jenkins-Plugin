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

#include "jenkinssettingspage.h"
#include "jenkinsconstants.h"
#include <coreplugin/icore.h>

#include <QSettings>

#include "ui_jenkinssettingspage.h"

namespace Jenkins {
namespace Internal {


JenkinsSettings::JenkinsSettings()
{
    refresh = 360;
}

void JenkinsSettings::toSettings(QSettings *s) const
{
    s->beginGroup(QLatin1String(Constants::JENKINS_SETTINGSGROUP));
    s->setValue(QLatin1String(Constants::JENKINS_URL), url);
    s->setValue(QLatin1String(Constants::JENKINS_IGNORED), ignore);
    s->setValue(QLatin1String(Constants::JENKINS_REFRESH), refresh);
    s->setValue(QLatin1String(Constants::JENKINS_USERNAME), username);
    s->setValue(QLatin1String(Constants::JENKINS_PASSWORD), password);

    s->endGroup();
}

void JenkinsSettings::fromSettings(QSettings *s)
{
    s->beginGroup(QLatin1String(Constants::JENKINS_SETTINGSGROUP));
    url= s->value(QLatin1String(Constants::JENKINS_URL), QLatin1String("")).toString();
    ignore = s->value(QLatin1String(Constants::JENKINS_IGNORED), QLatin1String("")).toString();
    refresh= s->value(QLatin1String(Constants::JENKINS_REFRESH), 360).toInt();
    username = s->value(QLatin1String(Constants::JENKINS_USERNAME)).toString();
    password = s->value(QLatin1String(Constants::JENKINS_PASSWORD)).toString();
    s->endGroup();
}


bool JenkinsSettings::equals(const JenkinsSettings &rhs) const
{
    return (url == rhs.url && ignore == rhs.ignore && refresh == rhs.refresh && username == rhs.username && password == rhs.password);
}

// ------------------ JenkinsSettingsWidget

JenkinsSettingsWidget::JenkinsSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JenkinsSettingsWidget)
{
    ui->setupUi(this);
}

JenkinsSettingsWidget::~JenkinsSettingsWidget()
{
    delete ui;
}



JenkinsSettings JenkinsSettingsWidget::settings() const
{
    JenkinsSettings rc;
    rc.url = ui->urlEdit->text();
    rc.ignore = ui->ignoreEdit->text();
    rc.refresh = ui->refreshSpinBox->value();
    rc.username = ui->edtUsername->text();
    rc.password = ui->edtPassword->text();

    // remove trailing '/'
    if (rc.url.at(rc.url.size()-1) == '/')
        rc.url = rc.url.left(rc.url.size()-1);
    return rc;
}


void JenkinsSettingsWidget::setSettings(const JenkinsSettings &s)
{
    ui->urlEdit->setText(s.url );
    ui->ignoreEdit->setText(s.ignore);
    ui->refreshSpinBox->setValue(s.refresh);
    ui->edtUsername->setText(s.username);
    ui->edtPassword->setText(s.password);
}


// --------------- JenkinsSettingsPage

JenkinsSettingsPage::JenkinsSettingsPage(QSharedPointer<JenkinsSettings> &settings,
                                         QObject *parent) :
    Core::IOptionsPage(parent),
    m_settings(settings)
{
}

JenkinsSettingsPage::~JenkinsSettingsPage()
{
}

QString JenkinsSettingsPage::id() const
{
    return QLatin1String(Constants::JENKINS_SETTINGS_ID);
}

QString JenkinsSettingsPage::displayName() const
{
    return QCoreApplication::translate("Jenkins", Constants::JENKINS_SETTINGS_ID);
}

QString JenkinsSettingsPage::category() const
{
    return QLatin1String(Constants::JENKINS_SETTINGS_CATEGORY);
}

QString JenkinsSettingsPage::displayCategory() const
{
    return QCoreApplication::translate("Jenkins", Constants::JENKINS_SETTINGS_CATEGORY);
}

QIcon  JenkinsSettingsPage::categoryIcon() const
{
    return QIcon();
}

QWidget *JenkinsSettingsPage::createPage(QWidget *parent)
{

    m_widget = new JenkinsSettingsWidget(parent);
    m_widget->setSettings(*m_settings);
    return m_widget;
}

void JenkinsSettingsPage::apply()
{
    if (m_widget) {
        const JenkinsSettings newSettings = m_widget->settings();
        if (newSettings != *m_settings) {
            *m_settings = newSettings;
            m_settings->toSettings(Core::ICore::instance()->settings());
            emit settingsUpdated();
        }
    }
}


} // namespace Internal
} // namespace Jenkins
