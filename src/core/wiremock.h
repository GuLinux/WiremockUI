#ifndef WIREMOCK_H
#define WIREMOCK_H
#include <QObject>
#include <QUrl>
#include <QMap>
#include <QVariantMap>
#include <QtNetwork/QNetworkAccessManager>
#include <memory>

class Settings;
class Wiremock : public QObject
{
    Q_OBJECT
public:
    typedef std::shared_ptr<Wiremock> ptr;
    Wiremock(Settings *settings);
    struct Request;
    struct Mapping;
    typedef QList<Mapping> Mappings;
    struct Cache;
    typedef QList<Request> Requests;
    Request request(const QString &id) const;
    Mapping mapping(const QString &checksum) const;
    QString createMappingUUID();
public slots:
    void queryRequests();
    void queryMappings();
    void clearRequests();
    void addMapping(QVariantMap mapping);
    void removeMapping(const QString &id);
    void clearMappings();
private slots:
    void requestsReceived(QNetworkReply *reply);
    void mappingsReceived(QNetworkReply *reply);
signals:
    void requestsCleared();
    void requestsAdded(const Requests &requests);
    void mappingsAdded(const Mappings &mappings);
    void mappingsRemoved(const Mappings &mappings);
private:
    QNetworkAccessManager* client;
    mutable QMap<QUrl, Cache> _cache;
    Cache &cache() const;
    Settings *settings;
};

struct Wiremock::Request {
    static Request from(const QVariantMap &value);
    QVariantMap wiremock_data;
    QString id;
    QString url;
    QUrl absoluteUrl;
    QString method;
    QString clientIp;
    QVariantMap headers;
    QVariantMap cookies;
    bool browserProxyRequest;
    uint64_t loggedDate;
    QByteArray body;
    QString loggedDateString;
    QVariantMap queryParams;
    QVariantMap responseDefinition;
    struct {
        int16_t status;
        QVariantMap headers;
        QByteArray body;
    } response;
    bool wasMatched;
    QVariantMap stubMapping;
};

// TODO: since Mapping structure seems to be more dynamic than Request/Response, is it worth doing parsing of a domain structure?
struct Wiremock::Mapping {
    static Mapping from(const QVariantMap &value);
    bool valid = false;
    QVariantMap wiremock_data;
    int priority;
    QString id;
    QVariantMap request;
    QVariantMap response;
    QByteArray checksum() const;
    mutable QByteArray sha1 = {};
};

struct Wiremock::Cache {
    Requests requests;
    Mappings mappings;
    uint64_t since() const;
};
#endif // WIREMOCK_H
