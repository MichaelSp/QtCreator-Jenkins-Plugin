#ifndef DATAFETCHER_H
#define DATAFETCHER_H

#include <QObject>
#include <QtCore/QUrl>
#include <QtCore/QXmlStreamReader>
#include <QtNetwork/QHttp>

namespace Jenkins {
namespace Internal {

class DataFetcher : public QObject
{
Q_OBJECT
public:
	DataFetcher(int maxItems, QObject *parent = 0);

signals:

	void projectItemReady(const QString& title, const QString& date, bool passed, const QString& url);

public slots:
	void fetch(const QUrl &url);
	void finished(int id, bool error);
	void readData(const QHttpResponseHeader &);

signals:
	void finished(bool error);

private:
	void parseXml();

	QXmlStreamReader m_xml;
	QString m_currentTag;
	QString m_linkString;
	QString m_titleString;
	QString m_dateString;
	bool m_passed;

	QHttp m_http;
	int m_connectionId;
	int m_items;
	int m_maxItems;

};

} // namespace Internal
} // namespace CruiseControl

#endif // DATAFETCHER_H
