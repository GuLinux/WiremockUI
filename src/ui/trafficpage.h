#ifndef REQUESTSPAGE_H
#define REQUESTSPAGE_H

#include <QWidget>
#include "core/wiremock.h"
#include <QTimer>

namespace Ui {
class RequestsPage;
}

class QTimer;
class QStandardItemModel;
class TrafficPage : public QWidget
{
    Q_OBJECT
public:
    explicit TrafficPage(QWidget *parent = 0);
    ~TrafficPage();
    void setWiremock(const Wiremock::ptr &wiremock);
    void showItem(const Wiremock::Request &request);
public slots:
    void clear();
private:
    Ui::RequestsPage *ui;
    QTimer _refreshTimer;
    Wiremock::ptr _wiremock;
    QStandardItemModel *requestsModel;

};

#endif // REQUESTSPAGE_H
