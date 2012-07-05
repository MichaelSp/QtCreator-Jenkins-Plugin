#ifndef DATAFETCHER_H
#define DATAFETCHER_H

#include <QObject>
#include <QtCore/QUrl>
#include <QtCore/QXmlStreamReader>
#include <QtNetwork/QHttp>

#include "projects.h"

namespace Jenkins {
namespace Internal {

class DataFetcher : public QObject
{
Q_OBJECT
public:
    DataFetcher(int maxItems, QObject *parent = 0);

signals:

    void projectItemReady(const Project& proj);

public slots:
    void fetch(const QUrl &url, QString username=QString(), QString password=QString());
    void finished(int id, bool error);
    void readData(const QHttpResponseHeader &);

signals:
    void finished(bool error,const QString& message);

private:
    void parseXml();
    void parseJob();
    void parseProjectHealth();

    QXmlStreamReader m_xml;
    Project currentProject;

    QHttp m_http;
    int m_connectionId;
    int m_items;
    int m_maxItems;
    QString m_errorMessage;

};

} // namespace Internal
} // namespace CruiseControl

#endif // DATAFETCHER_H
