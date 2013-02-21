#ifndef DATAFETCHER_H
#define DATAFETCHER_H

#include <QObject>
#include <QtCore/QUrl>
#include <QtCore/QXmlStreamReader>
#include <QtNetwork/QNetworkAccessManager>

#include "projects.h"

namespace Jenkins {
namespace Internal {

class DataFetcher : public QObject
{
Q_OBJECT
public:
    DataFetcher(int maxItems, QObject *parent = 0);

signals:
	void queueItemReady(const Item& item);
    void projectItemReady(const Project& proj);

public slots:
    void fetch(QString urlString, QString username=QString(), QString password=QString());
	void fetchQueue(QString urlString, QString username=QString(), QString password=QString());
    void finished(QNetworkReply* repl);

signals:
    void finished(bool error, QString message);

private:
    void parseXml(QXmlStreamReader &xml);

    void parseJob(QXmlStreamReader &xml);
    void parseProjectHealth(QXmlStreamReader &xml);
    void parseLastBuild(QXmlStreamReader &xml);

	void parseItem(QXmlStreamReader &xml);
	void parseItemTask(QXmlStreamReader &xml);
	void parseItemAction(QXmlStreamReader &xml);

    Project currentProject;
	Item currentItem;

    QNetworkAccessManager mNetworkAccess;
    QString m_errorMessage;

    int m_items;
    int m_maxItems;

};

} // namespace Internal
} // namespace CruiseControl

#endif // DATAFETCHER_H
