#ifndef REQUESTSPAGE_H
#define REQUESTSPAGE_H

#include <QWidget>
#include "core/wiremock.h"
#include <QTimer>
#include "toolbarprovider.h"

namespace Ui {
class RequestsPage;
}

class QTimer;
class QStandardItemModel;
class TrafficPage : public QWidget, public ToolbarProvider
{
    Q_OBJECT
public:
    explicit TrafficPage(QWidget *parent = 0);
    ~TrafficPage();
    void setWiremock(const Wiremock::ptr &wiremock);
    void showItem(const Wiremock::Request &request);
    QList<QToolBar*> providedToolBars() const override;
public slots:
    void clear();
private:
    Ui::RequestsPage *ui;
    QTimer _refreshTimer;
    Wiremock::ptr _wiremock;
    QStandardItemModel *requestsModel;
    QToolBar *requeststToolbar;

};

#endif // REQUESTSPAGE_H
