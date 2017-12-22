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

    QVariantList requests_variants = body.toVariant().toMap()["requests"].toList();
    Requests requests;
    std::transform(requests_variants.begin(), requests_variants.end(), back_inserter(requests), [](QVariant v) { return Request::from(v.toMap()); });
    sort(requests.begin(), requests.end(), [](const Request &a, const Request &b) { return a.loggedDate < b.loggedDate; });
    cache().requests.append(requests);
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
    auto response = value["response"].toMap();
    return {
        value["url"].toString(),
        value["url"].toString(),
        value["absoluteUrl"].toUrl(),        
        value["method"].toString(),
        value["clientIp"].toString(),
        value["headers"].toMap(),
        value["cookies"].toMap(),
        value["browserProxyRequest"].toBool(),
        static_cast<uint64_t>(value["loggedDate"].toLongLong()),
        value["bodyAsBase64"].toString(),
        value["body"].toByteArray(),
        value["loggedDateString"].toString(),
        value["queryParams"].toMap(),
        value["responseDefinition"].toMap(),
        {
            static_cast<int16_t>(response["status"].toInt()),
            response["headers"].toMap(),
            response["bodyAsBase64"].toString(),
            response["body"].toByteArray(),
        },
        value["wasMatched"].toBool(),
        value["stubMapping"].toMap(),
    };

//    bool wasMatched;
//    QVariantMap stubMapping;

}

uint64_t Wiremock::Cache::since() const
{
    if(requests.length() > 0) {
        auto max = max_element(requests.begin(), requests.end(), [](const Request &a, const Request &b) {return a.loggedDate < b.loggedDate; });
        return max->loggedDate;
    }
    return 0;
}
