#include "wiremock.h"
#include "settings.h"
#include <QtNetwork/QNetworkReply>
#include <QDebug>
#include <QJsonDocument>
#include <list>
#include <algorithm>
#include "utils/utils.h"
#include <QTimer>
#include <QDateTime>

using namespace std;

namespace {
QUrl getRequestsURL(Settings *settings, int64_t since=0) {
    QUrl url = settings->server_address();
    url.setPath("/__admin/requests");
    if(since > 0) {
        since++;
        int msecs = since - (since/1000) * 1000;
        QString since_query = QString("since=%1.%2Z")
                .arg(QDateTime::fromMSecsSinceEpoch(since).toString(Qt::ISODate))
                .arg(msecs, 3, 10, QChar{'0'});
        url.setQuery(since_query);
    }
    return url;
}

QUrl clearRequestsURL(Settings *settings) {
    QUrl url = settings->server_address();
    url.setPath("/__admin/requests");
    return url;
}
}

Wiremock::Wiremock(Settings *settings) : settings{settings}
{
    client = new QNetworkAccessManager(this);
    QTimer::singleShot(1000, this, &Wiremock::queryRequests);
}

Wiremock::Request Wiremock::request(const QString &id) const
{
    auto request_it = find_if(cache().requests.begin(), cache().requests.end(), [id](const Request &r) { return r.id == id; });
    if(request_it != cache().requests.end())
        return *request_it;
    qDebug() << "Request with id" << id << "not found";
    return {};
}

void Wiremock::queryRequests()
{
    auto url = getRequestsURL(settings, cache().since());
    qDebug() << "querying url:" << url;
    auto reply = client->get(QNetworkRequest{url});
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        requestsReceived(reply);
    });
}

void Wiremock::clearRequests()
{
    QNetworkRequest request(clearRequestsURL(settings));
    auto reply = client->sendCustomRequest(request, "DELETE");
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        Scope on_exit{ [this, reply]{
                reply->deleteLater();
            }};
        Q_UNUSED(on_exit);
        if(reply->error() != QNetworkReply::NoError) {
            qWarning() << "Error deleting requests on" << reply->url() << ":" << reply->errorString();
        } else {
            emit requestsCleared();
        }
    });
}

void Wiremock::requestsReceived(QNetworkReply *reply)
{
    Scope on_exit{ [this, reply]{
            reply->deleteLater();
            QTimer::singleShot(1000, this, &Wiremock::queryRequests);
        }};
    Q_UNUSED(on_exit);
    if(reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error fetching requests from" << reply->url() << ":" << reply->errorString();
    } else {
        auto body = QJsonDocument::fromJson(reply->readAll());

        QVariantList requests_variants = body.toVariant().toMap()["requests"].toList();
        Requests requests;
        std::transform(requests_variants.begin(), requests_variants.end(), back_inserter(requests), [](QVariant v) { return Request::from(v.toMap()); });
        sort(requests.begin(), requests.end(), [](const Request &a, const Request &b) { return a.loggedDate < b.loggedDate; });
        if(requests.size() > 0) {
            cache().requests.append(requests);
            emit requestsAdded(requests);
        }
        qDebug() << "fetched" << requests.length() << "requests.";
    }
}

Wiremock::Cache &Wiremock::cache() const
{
    auto url = settings->server_address();
    if(! _cache.contains(url))
        _cache[url] = {};
    return _cache[url];
}

Wiremock::Request Wiremock::Request::from(const QVariantMap &value)
{
    auto response = value["response"].toMap();
    auto request = value["request"].toMap();
    return {
        value,
        value["id"].toString(),
        request["url"].toString(),
        request["absoluteUrl"].toUrl(),
        request["method"].toString(),
        request["clientIp"].toString(),
        request["headers"].toMap(),
        request["cookies"].toMap(),
        request["browserProxyRequest"].toBool(),
        static_cast<uint64_t>(request["loggedDate"].toLongLong()),
        QByteArray::fromBase64(request["bodyAsBase64"].toByteArray()),
        request["loggedDateString"].toString(),
        request["queryParams"].toMap(),
        value["responseDefinition"].toMap(),
        {
            static_cast<int16_t>(response["status"].toInt()),
            response["headers"].toMap(),
            QByteArray::fromBase64(response["bodyAsBase64"].toByteArray()),
        },
        value["wasMatched"].toBool(),
        value["stubMapping"].toMap(),
    };
}

uint64_t Wiremock::Cache::since() const
{
    if(requests.length() > 0) {
        auto max = max_element(requests.begin(), requests.end(), [](const Request &a, const Request &b) {return a.loggedDate < b.loggedDate; });
        qDebug() << "max: " << max->loggedDate << ", last: ", requests[requests.length()-1].loggedDate;
        return max->loggedDate;
    }
    return 0;
}
