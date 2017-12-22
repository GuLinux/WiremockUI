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
    Wiremock(Settings &settings);
    struct Request;
    struct Cache;
    typedef QList<Request> Requests;
    Requests requests() const;
public slots:
    void queryRequests();
private slots:
    void requestsReceived(QNetworkReply *reply);
signals:
    void requestsUpdated();
private:
    Settings &settings;
    QNetworkAccessManager client;
    QMap<QUrl, Cache> _cache;
    Cache &cache();
};

struct Wiremock::Request {
    static Wiremock::Request from(const QVariantMap &value);
    QString id;
    QString url;
    QUrl absoluteUrl;
    QString method;
    QString clientIp;
    QMap<QString, QString> headers;
    QMap<QString, QString> cookies;
    bool browserProxyRequest;
    uint64_t loggedDate;
    QString bodyAsBase64;
    QByteArray body;
    QString loggedDateString;
    QVariantMap queryParams;
    QVariantMap responseDefinition;
    struct {
        int16_t status;
        QMap<QString, QString> headers;
        QString bodyAsBase64;
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
