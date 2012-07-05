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

#ifndef CRUISECONTROLSETINGSPAGE_H
#define CRUISECONTROLSETINGSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>
#include <QWidget>
#include <QSettings>
#include <QPointer>


namespace Jenkins {
namespace Internal {

namespace Ui {
    class JenkinsSettingsWidget;
}


struct JenkinsSettings {
    JenkinsSettings();

    QString url;
    QString ignore;
    int		refresh;
    QString username;
    QString password;

    void toSettings(QSettings *) const;
    void fromSettings(QSettings *);

    bool equals(const JenkinsSettings &rhs) const;
};

inline bool operator==(const JenkinsSettings &s1, const JenkinsSettings &s2) { return s1.equals(s2); }
inline bool operator!=(const JenkinsSettings &s1, const JenkinsSettings &s2) { return !s1.equals(s2); }


class JenkinsSettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit JenkinsSettingsWidget(QWidget *parent = 0);
    ~JenkinsSettingsWidget();

    JenkinsSettings settings() const;
    void setSettings(const JenkinsSettings &s);

private:
    Ui::JenkinsSettingsWidget *ui;
};


class JenkinsSettingsPage : public Core::IOptionsPage
{
    Q_OBJECT
    Q_DISABLE_COPY(JenkinsSettingsPage)
public:
    explicit JenkinsSettingsPage(QSharedPointer<JenkinsSettings> &settings,
                                 QObject *parent = 0);
    virtual ~JenkinsSettingsPage();

    virtual QWidget *createPage(QWidget *parent);
    virtual void apply();
    virtual void finish() { }

signals:

    void	settingsUpdated();

private:
    const QSharedPointer<JenkinsSettings> m_settings;
    QPointer<JenkinsSettingsWidget> m_widget;
};

} // namespace Internal
} // namespace CruiseControl
#endif // CruiseControlSetingsWidget_H
