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
    struct Cache;
    typedef QList<Request> Requests;
    Request request(const QString &id) const;
public slots:
    void queryRequests();
    void clearRequests();
private slots:
    void requestsReceived(QNetworkReply *reply);
signals:
    void requestsCleared();
    void requestsAdded(const Requests &requests);
private:
    QNetworkAccessManager* client;
    mutable QMap<QUrl, Cache> _cache;
    Cache &cache() const;
    Settings *settings;
};

struct Wiremock::Request {
    static Wiremock::Request from(const QVariantMap &value);
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

struct Wiremock::Cache {
    Requests requests;
    uint64_t since() const;
};
#endif // WIREMOCK_H
