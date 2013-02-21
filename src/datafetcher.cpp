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
#include <QtCore/QDateTime>
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

void DataFetcher::fetch(QString urlString, QString username, QString password)
{
	QUrl url = QUrl::fromUserInput(urlString + "/api/xml?depth=2");
    m_errorMessage = "fetching " + url.toString() + "\n";
    url.setUserName(username);
    url.setPassword(password);

    if (!url.isValid()) {
        emit finished(true, "invalid URL");
        return;
    }

    if (url.scheme() == "file") {
        QString filename = url.toLocalFile();
        QFile file(filename);
        if (file.open(QFile::ReadOnly)) {
            QXmlStreamReader xmlReader(file.readAll());
            parseXml(xmlReader);
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
	if(username.length() > 0)
		request.setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(username).arg(password).toAscii()).toBase64());
    mNetworkAccess.proxyFactory()->setUseSystemConfiguration(true);
    mNetworkAccess.get( request );
}

void DataFetcher::fetchQueue(QString urlString, QString username, QString password)
{
	QUrl url = QUrl::fromUserInput(urlString + "/queue/api/xml");
	m_errorMessage = "fetching " + url.toString() + "\n";
	url.setUserName(username);
	url.setPassword(password);

	if (!url.isValid()) {
		emit finished(true, "invalid URL");
		return;
	}

	if (url.scheme() == "file") {
		QString filename = url.toLocalFile();
		QFile file(filename);
		if (file.open(QFile::ReadOnly)) {
			QXmlStreamReader xmlReader(file.readAll());
			parseXml(xmlReader);
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
	if(username.length() > 0)
		request.setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(username).arg(password).toAscii()).toBase64());
	mNetworkAccess.proxyFactory()->setUseSystemConfiguration(true);
	mNetworkAccess.get( request );
}

void DataFetcher::finished(QNetworkReply* repl)
{
    bool error = repl->error() != QNetworkReply::NoError;
    if (error) {
		m_errorMessage = "Error code (NOT HTTP-Code!) : " + QString::number(repl->error()) + " / HTTP code " + repl->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString() +" \n\t" + repl->errorString() +"\n";
    }
    else {
        QXmlStreamReader xmlReader(repl->readAll());
        parseXml(xmlReader);
    }
    emit finished(error, m_errorMessage);
    m_errorMessage.clear();
}

bool endOfElement(QString elemName,QXmlStreamReader &xml)
{
 return (xml.isEndElement() && xml.name() == elemName);
}

void readUntilEndOf(QString /*elem*/, QXmlStreamReader &xml) {
    while (!(xml.error() || xml.atEnd() || endOfElement("job",xml)))
        xml.readNext();
}

void DataFetcher::parseXml(QXmlStreamReader &xml)
{
    bool rootOK = false;
    while (!(xml.atEnd() || xml.error())) {
        if (xml.readNext() == QXmlStreamReader::StartElement) {
			if (xml.name() == "hudson" || xml.name() == "queue")
                rootOK = true;
            else if (rootOK && xml.name() == "job")
                parseJob(xml);
			else if (rootOK && xml.name() == "item")
				parseItem(xml);
            else
                xml.skipCurrentElement();
        }
    }
    if (xml.error()) {
        m_errorMessage += "XML ERROR:" + QString::number( xml.lineNumber() ) + ": " + xml.errorString();
        qWarning() << m_errorMessage;
    }
}


void DataFetcher::parseJob(QXmlStreamReader &xml)
{
    currentProject.clear();
    while (!(xml.error() || xml.atEnd() || endOfElement("job",xml)))
    {
        if (xml.readNextStartElement())
        {
            if (xml.name() == "name")
                currentProject.name = xml.readElementText();
            else if (xml.name() == "url")
                currentProject.link = xml.readElementText();
            else if (xml.name() == "color")
                currentProject.color = xml.readElementText();
            else if (xml.name() == "healthReport")
                parseProjectHealth(xml);
            else if (xml.name() == "lastBuild")
                parseLastBuild(xml);
            else
                xml.skipCurrentElement();
        }
    }
    emit projectItemReady(currentProject);
}
void DataFetcher::parseProjectHealth(QXmlStreamReader &xml)
{
    while (!(xml.error() || xml.atEnd() || endOfElement("healthReport",xml)))
    {
        if (xml.readNextStartElement()) {
            if (xml.name() == "score")
                currentProject.healthInPercent = xml.readElementText().toInt();
            else
                xml.skipCurrentElement();
        }
    }
}

void DataFetcher::parseLastBuild(QXmlStreamReader &xml)
{
    while (!(xml.error() || xml.atEnd() || endOfElement("lastBuild",xml)))
    {
        if (xml.readNextStartElement()) {
            if (xml.name() == "userName")
                currentProject.lastBuildUsername = xml.readElementText();
            if (xml.name() == "timestamp") {
                qulonglong timestamp= ((qulonglong)xml.readElementText().toULongLong());
                currentProject.date = QDateTime::fromMSecsSinceEpoch( timestamp ).toString(Qt::SystemLocaleLongDate);
            }
            if (xml.name() == "result")
                currentProject.lastBuildOK = xml.readElementText() == "SUCCESS";
        }
    }
}

void DataFetcher::parseItem(QXmlStreamReader &xml)
{
	currentProject.clear();
	while (!(xml.error() || xml.atEnd() || endOfElement("item",xml)))
	{
		if (xml.readNextStartElement())
		{
			if (xml.name() == "blocked")
				currentItem.blocked = xml.readElementText() == "true" ? true : false;
			else if (xml.name() == "buildable")
				currentItem.buildable = xml.readElementText() == "true" ? true : false;
			else if (xml.name() == "stuck")
				currentItem.stuck = xml.readElementText() == "true" ? true : false;
			else if (xml.name() == "why")
				currentItem.why = xml.readElementText();
			else if (xml.name() == "action")
				parseItemAction(xml);
			else if (xml.name() == "task")
				parseItemTask(xml);
			else
				xml.skipCurrentElement();
		}
	}
	emit queueItemReady(currentItem);
}
void DataFetcher::parseItemAction(QXmlStreamReader &xml)
{
	while (!(xml.error() || xml.atEnd() || endOfElement("action",xml)))
	{
		if (xml.readNextStartElement()) {
			if(xml.name() == "cause" && xml.readNextStartElement()) {
				if(xml.name() == "shortDescription")
					currentItem.cause = xml.readElementText();
			}
		}
	}
}
void DataFetcher::parseItemTask(QXmlStreamReader &xml)
{
	while (!(xml.error() || xml.atEnd() || endOfElement("task",xml)))
	{
		if (xml.readNextStartElement()) {
			if(xml.name() == "name") {
				currentItem.task = xml.readElementText();
			}
			if(xml.name() == "color") {
				currentItem.color = xml.readElementText();
			}
		}
	}
}
