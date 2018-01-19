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
class FunctionFilterModel;
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
private slots:
    void on_requestsFilter_returnPressed();

private:
    Ui::RequestsPage *ui;
    QTimer _refreshTimer;
    Wiremock::ptr _wiremock;
    QStandardItemModel *requestsModel;
    FunctionFilterModel *filterModel;
    QToolBar *requeststToolbar;
    bool check(const QString expression, const Wiremock::Request &request);

    QString failingJavascript;
signals:
    void expressionError(const QString &message, const QString failingJavascript);
};

#endif // REQUESTSPAGE_H
