#ifndef REQUESTSPAGE_H
#define REQUESTSPAGE_H

#include <QWidget>
#include "wiremock.h"
#include <QTimer>

namespace Ui {
class RequestsPage;
}

class QTimer;
class RequestsPage : public QWidget
{
    Q_OBJECT
public:
    explicit RequestsPage(QWidget *parent = 0);
    ~RequestsPage();
    inline void setWiremock(const Wiremock::ptr &wiremock) { this->_wiremock = wiremock; }
private:
    Ui::RequestsPage *ui;
    QTimer _refreshTimer;
    Wiremock::ptr _wiremock;
};

#endif // REQUESTSPAGE_H
