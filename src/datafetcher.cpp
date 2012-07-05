/*
Qt creator cruise control plugin
Copyright (C) 2010  David Dibben

This code is based on the rssfetcher which is part of Qt Creator
Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).

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

#include "datafetcher.h"


#include <QtCore/QDebug>
#include <QtCore/QSysInfo>
#include <QtCore/QLocale>
#include <QtGui/QDesktopServices>
#include <QtGui/QLineEdit>
#include <QtNetwork/QHttp>
#include <QtNetwork/QNetworkProxyFactory>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QAuthenticator>

#include <app/app_version.h>

#include <QFile>

#include <coreplugin/coreconstants.h>

#ifdef Q_OS_UNIX
#include <sys/utsname.h>
#endif

using namespace Jenkins;
using namespace Jenkins::Internal;


static const QString getOsString()
{
    QString osString;
#if defined(Q_OS_WIN)
    switch (QSysInfo::WindowsVersion) {
    case (QSysInfo::WV_4_0):
        osString += QLatin1String("WinNT4.0");
        break;
    case (QSysInfo::WV_5_0):
        osString += QLatin1String("Windows NT 5.0");
        break;
    case (QSysInfo::WV_5_1):
        osString += QLatin1String("Windows NT 5.1");
        break;
    case (QSysInfo::WV_5_2):
        osString += QLatin1String("Windows NT 5.2");
        break;
    case (QSysInfo::WV_6_0):
        osString += QLatin1String("Windows NT 6.0");
        break;
    case (QSysInfo::WV_6_1):
        osString += QLatin1String("Windows NT 6.1");
        break;
    default:
        osString += QLatin1String("Windows NT (Unknown)");
        break;
    }
#elif defined (Q_OS_MAC)
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        osString += QLatin1String("PPC ");
    else
        osString += QLatin1String("Intel ");
    osString += QLatin1String("Mac OS X ");
    switch (QSysInfo::MacintoshVersion) {
    case (QSysInfo::MV_10_3):
        osString += QLatin1String("10_3");
        break;
    case (QSysInfo::MV_10_4):
        osString += QLatin1String("10_4");
        break;
    case (QSysInfo::MV_10_5):
        osString += QLatin1String("10_5");
        break;
    case (QSysInfo::MV_10_6):
        osString += QLatin1String("10_6");
        break;
    default:
        osString += QLatin1String("(Unknown)");
        break;
    }
#elif defined (Q_OS_UNIX)
    struct utsname uts;
    if (uname(&uts) == 0)
        osString += QString("%1 %2").arg(QLatin1String(uts.sysname))
                        .arg(QLatin1String(uts.release));
    else
        osString += QLatin1String("Unix (Unknown)");
#else
    ossttring = QLatin1String("Unknown OS");
#endif
    return osString;
}

DataFetcher::DataFetcher(int maxItems, QObject *parent)
    : QObject(parent), m_items(0), m_maxItems(maxItems)
{
    connect(&mNetworkAccess, SIGNAL(finished(QNetworkReply*)),
            SLOT(finished(QNetworkReply*)));
}

void DataFetcher::fetch(QUrl url, QString username, QString password)
{
    m_errorMessage = "fetching " + url.toString();
    url.setUserName(username);
    url.setPassword(password);
    m_xml.clear();

    if (!url.isValid()) {
        emit finished(true, "invalid URL");
        return;
    }

    if (url.scheme() == "file") {
        QString filename = url.toLocalFile();
        QFile file(filename);
        if (file.open(QFile::ReadOnly)) {
            m_xml.addData(file.readAll());
            parseXml();
        } else {
            emit finished(true, "Unable to read the file: " + filename);
        }
        file.close();
        return;
    }

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", QString("Qt-Creator/%1 (QHttp %2; %3; %4; %5 bit)")
                         .arg(Core::Constants::IDE_VERSION_LONG).arg(qVersion())
                         .arg(getOsString()).arg(QLocale::system().name())
                         .arg(QSysInfo::WordSize).toLocal8Bit());
    request.setRawHeader("Accept", "text/xml,application/xml; charset=UTF-8");
    mNetworkAccess.proxyFactory()->setUseSystemConfiguration(true);
    mNetworkAccess.get( request );
}


void DataFetcher::finished(QNetworkReply* repl)
{
    m_xml.clear();

    bool error = repl->error() != QNetworkReply::NoError;
    if (error) {
        m_errorMessage = "Error code (NOT HTTP-Code!) : " + QString::number(repl->error()) + "\n\t" + repl->errorString();
    }
    else {
        QString str = repl->readAll();
        m_xml.addData(str);
        parseXml();
    }
    emit finished(error, m_errorMessage);
    m_errorMessage.clear();
}


void DataFetcher::parseXml()
{
    bool rootOK = false;
    while (!m_xml.atEnd()) {
        m_xml.readNext();
        if (m_xml.isStartElement()) {
            if (m_xml.name() == "hudson") {
                rootOK = true;
            }
            if (rootOK && m_xml.name() == "job")
                parseJob();
        }
    }
    if (m_xml.error() && m_xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
        m_errorMessage = "XML ERROR:" + QString::number( m_xml.lineNumber() ) + ": " + m_xml.errorString();
        qWarning() << m_errorMessage;
    }
}

void DataFetcher::parseJob()
{
    while (!m_xml.atEnd() || m_xml.isEndElement() && m_xml.name() == "job") {
        m_xml.readNext();
        if (m_xml.isStartElement()) {
            if (m_xml.name() == "name")
                currentProject.name = m_xml.readElementText();
            if (m_xml.name() == "url")
                currentProject.link = m_xml.readElementText();
            if (m_xml.name() == "color")
                currentProject.color = m_xml.readElementText();
            if (m_xml.name() == "healthReport")
                parseProjectHealth();
        }
    }
    emit projectItemReady(currentProject);
    currentProject.clear();
}

void DataFetcher::parseProjectHealth()
{
    while (!m_xml.atEnd() || m_xml.isEndElement() && m_xml.name() == "healthReport") {
        m_xml.readNext();
        if (m_xml.isStartElement()) {
            if (m_xml.name() == "score")
                currentProject.healthInPercent = m_xml.readElementText().toInt();
        }
    }
}
