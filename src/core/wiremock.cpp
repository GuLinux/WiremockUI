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
#include <QCryptographicHash>
#include <QUuid>

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
QUrl getMappingsURL(Settings *settings, const QString id={}) {
    QUrl url = settings->server_address();
    QString path{"/__admin/mappings"};
    if(!id.isEmpty())
        path += "/" + id;
    url.setPath(path);
    return url;
}
}

Wiremock::Wiremock(Settings *settings) : settings{settings}
{
    client = new QNetworkAccessManager(this);
    QTimer::singleShot(1000, this, &Wiremock::queryRequests);
    QTimer::singleShot(1000, this, &Wiremock::queryMappings);
}

Wiremock::Request Wiremock::request(const QString &id) const
{
    auto request_it = find_if(cache().requests.begin(), cache().requests.end(), [id](const Request &r) { return r.id == id; });
    if(request_it != cache().requests.end())
        return *request_it;
    qDebug() << "Request with id" << id << "not found";
    return {};
}

Wiremock::Mapping Wiremock::mapping(const QString &checksum) const
{
    auto mapping_it = find_if(cache().mappings.begin(), cache().mappings.end(), [checksum](const Mapping &m) { return m.checksum() == checksum; });
    if(mapping_it != cache().mappings.end())
        return *mapping_it;
    qDebug() << "Mapping with checksum" << checksum << "not found";
    return {};
}

QString Wiremock::createMappingUUID()
{
    QStringList current_uuids;
    transform(cache().mappings.begin(), cache().mappings.end(), back_inserter(current_uuids), [](const Mapping &m) { return m.id; });
    while(true) {
        auto uuid = QUuid::createUuid().toString();
        if(! current_uuids.contains(uuid))
            return uuid;
    }
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

void Wiremock::queryMappings()
{
    auto url = getMappingsURL(settings);
    qDebug() << "querying url:" << url;
    auto reply = client->get(QNetworkRequest{url});
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        mappingsReceived(reply);
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

void Wiremock::addMapping(QVariantMap mapping)
{
    auto id = mapping.value("id", mapping.value("uuid")).toString();
    if(id.isEmpty())
        id = createMappingUUID();
    mapping["id"] = id;
    mapping["uuid"] = id;
    QNetworkRequest request{getMappingsURL(settings)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    auto reply = client->post(request, QJsonDocument::fromVariant(mapping).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        // TODO: notify UI if failed
        reply->deleteLater();
    });
}

void Wiremock::removeMapping(const QString &id)
{
    if(id.isEmpty()) {
        qWarning() << "Trying to remove mapping with empty ID. Clear instead, if you want to remove all mappings.";
        return;
    }
    auto request = QNetworkRequest{getMappingsURL(settings, id)};
    auto reply = client->sendCustomRequest(request, "DELETE");
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        // TODO: notify UI if failed
        reply->deleteLater();
    });
}

void Wiremock::clearMappings()
{
    auto request = QNetworkRequest{getMappingsURL(settings)};
    auto reply = client->sendCustomRequest(request, "DELETE");
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        // TODO: notify UI if failed
        reply->deleteLater();
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

void Wiremock::mappingsReceived(QNetworkReply *reply)
{
    Scope on_exit{ [this, reply]{
            reply->deleteLater();
            QTimer::singleShot(1000, this, &Wiremock::queryMappings);
        }};
    Q_UNUSED(on_exit);
    if(reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error fetching mappings from" << reply->url() << ":" << reply->errorString();
    } else {
        auto body = QJsonDocument::fromJson(reply->readAll());

        QVariantList mappings_variants = body.toVariant().toMap()["mappings"].toList();
        Mappings mappings;
        transform(mappings_variants.begin(), mappings_variants.end(), back_inserter(mappings), [](QVariant v) { return Mapping::from(v.toMap()); });
        qDebug() << "fetched" << mappings.length() << "mappings.";
        Mappings added, removed;
        Mappings &cached = cache().mappings;
        auto compare_mappings = [](const Mapping &a, const Mapping &b) { return a.checksum() < b.checksum(); };
        set_difference(mappings.begin(), mappings.end(), cached.begin(), cached.end(), back_inserter(added), compare_mappings);
        set_difference(cached.begin(), cached.end(), mappings.begin(), mappings.end(), back_inserter(removed), compare_mappings);
        emit mappingsAdded(added);
        emit mappingsRemoved(removed);
        cache().mappings = mappings;
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

Wiremock::Mapping Wiremock::Mapping::from(const QVariantMap &value)
{
    auto response = value["response"].toMap();
    auto request = value["request"].toMap();
    QString id = value.value("id").toString();
    if(id.isEmpty())
        id = value.value("uuid").toString();
    Mapping mapping {
        true,
        value,
        value["priority"].toInt(),
        id,
        request,
        response,
    };
    mapping.checksum();
    return mapping;
}

QByteArray Wiremock::Mapping::checksum() const
{
    if(sha1.isEmpty()) {
        QCryptographicHash crypto(QCryptographicHash::Sha1);
        crypto.addData(QJsonDocument::fromVariant(wiremock_data).toBinaryData());
        sha1 = crypto.result();
    }
    return sha1;
}
