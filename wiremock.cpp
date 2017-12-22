#include "wiremock.h"
#include "settings.h"
#include <QtNetwork/QNetworkReply>
#include <QDebug>
#include <QJsonDocument>
#include <list>
#include <algorithm>

using namespace std;

namespace {
QUrl requestsURL(const Settings &settings, int64_t since=0) {
    QUrl url = settings.server_address();
    url.setPath("/__admin/requests");
    if(since > 0)
        url.setQuery(QString("since=%1").arg(since));
    return url;
}
}

Wiremock::Wiremock(Settings &settings) : settings{settings}
{

}

Wiremock::Requests Wiremock::requests() const
{
    return _cache.value(settings.server_address()).requests;
}

void Wiremock::queryRequests()
{
    auto url = requestsURL(settings, cache().since());
    qDebug() << "querying url:" << url;
    auto reply = client.get(QNetworkRequest{url});
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        requestsReceived(reply);
    });
}

void Wiremock::requestsReceived(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error fetching requests from" << reply->url() << ":" << reply->errorString();
    }
    auto body = QJsonDocument::fromJson(reply->readAll());

    QVariantList requests = body.toVariant().toMap()["requests"].toList();
    qDebug() << reply->url() << reply->errorString() << reply->rawHeaderList();
}

Wiremock::Cache &Wiremock::cache()
{
    auto url = settings.server_address();
    if(! _cache.contains(url))
        _cache[url] = {};
    return _cache[url];
}

Wiremock::Request Wiremock::Request::from(const QVariantMap &value)
{

}

uint64_t Wiremock::Cache::since() const
{
    if(requests.length() > 0) {
        auto max = max_element(requests.begin(), requests.end(), [](const Request &a, const Request &b) {return a.loggedDate < b.loggedDate; });
        return max->loggedDate;
    }
    return 0;
}
