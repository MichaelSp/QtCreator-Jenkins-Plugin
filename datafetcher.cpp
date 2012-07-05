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
    connect(&m_http, SIGNAL(readyRead(const QHttpResponseHeader &)),
             this, SLOT(readData(const QHttpResponseHeader &)));

    connect(&m_http, SIGNAL(requestFinished(int, bool)),
             this, SLOT(finished(int, bool)));
}

void DataFetcher::fetch(const QUrl &url)
{
    m_xml.clear();

    if (!url.isValid()) {
        finished(0, true);
        return;
    }

    if (url.scheme() == "file") {
        QString filename = url.toLocalFile();
        QFile file(filename);
        if (file.open(QFile::ReadOnly)) {
            m_xml.addData(file.readAll());
            parseXml();
            finished(0, false);
        } else {
            finished(0, true);
        }
        file.close();
        return;
    }


    QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery(QNetworkProxyQuery(url));
    if (proxies.count() > 0)
        m_http.setProxy(proxies.first());
    m_http.setHost(url.host(), url.port());
    QString agentStr = QString("Qt-Creator/%1 (QHttp %2; %3; %4; %5 bit)")
                    .arg(Core::Constants::IDE_VERSION_LONG).arg(qVersion())
                    .arg(getOsString()).arg(QLocale::system().name())
                    .arg(QSysInfo::WordSize);
    QHttpRequestHeader header("GET", url.path());
    //qDebug() << agentStr;
    header.setValue("User-Agent", agentStr);
    header.setValue("Host", url.host());
    m_connectionId = m_http.request(header);
}

void DataFetcher::readData(const QHttpResponseHeader &resp)
{
    if (resp.statusCode() != 200)
        m_http.abort();
    else {
        m_xml.addData(m_http.readAll());
        parseXml();
    }
}

void DataFetcher::finished(int id, bool error)
{
    Q_UNUSED(id)
    m_items = 0;
    m_xml.clear();
    emit finished(error);
}


void DataFetcher::parseXml()
{
    while (!m_xml.atEnd()) {
        m_xml.readNext();
        if (m_xml.isStartElement()) {
            if (m_xml.name() == "item") {
                m_titleString.clear();
                m_dateString.clear();
                m_linkString.clear();
                m_passed = true;
            }
            m_currentTag = m_xml.name().toString();
        } else if (m_xml.isEndElement()) {
            if (m_xml.name() == "item") {
                m_items++;
                if (m_items > m_maxItems)
                    return;
                emit projectItemReady(m_titleString, m_dateString, m_passed, m_linkString);
            }

        } else if (m_xml.isCharacters() && !m_xml.isWhitespace()) {
            if (m_currentTag == "title") {
                QString title = m_xml.text().toString();
                int pos = title.toLower().indexOf("passed");
                if (pos < 0) {
                    pos = title.toLower().indexOf("failed");
                }
                m_titleString = (pos > 0 ? title.left(pos) : title).trimmed();
            } else if (m_currentTag == "pubDate") {
                m_dateString += m_xml.text().toString();
            } else if (m_currentTag == "pubDate") {
                    m_dateString += m_xml.text().toString();
            } else if (m_currentTag == "description") {
                m_passed = (m_xml.text().toString().toLower().indexOf("passed") > 0);
            }else if (m_currentTag == "link") {
                m_linkString += m_xml.text().toString();
            }
        }
    }
    if (m_xml.error() && m_xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
        qWarning() << "XML ERROR:" << m_xml.lineNumber() << ": " << m_xml.errorString();
        m_http.abort();
    }
}
