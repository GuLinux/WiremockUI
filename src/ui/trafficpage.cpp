#include "trafficpage.h"
#include "ui_trafficpage.h"
#include <QStandardItemModel>
#include <QJsonDocument>
#include <QSyntaxHighlighter>
#include <QToolBar>
#include <QLabel>

TrafficPage::TrafficPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RequestsPage)
{
    ui->setupUi(this);
    requestsModel = new QStandardItemModel(this);

    ui->requests->setModel(requestsModel);
    connect(ui->requests->selectionModel(), &QItemSelectionModel::currentChanged, this,
            [this](const QModelIndex &current, const QModelIndex &){
        auto request = requestsModel->itemFromIndex(current);
        if(!current.isValid() || !request) {
            ui->requestView->clear();
            ui->responseView->clear();
        } else {
            request = requestsModel->item(request->row());
            auto id = request->data().toString();
            if(id.isEmpty()) {
                qDebug() << "Empty id found";
                return;
            }
            showItem(_wiremock->request(request->data().toString()));
        }
    });
    requeststToolbar = new QToolBar(tr("Requests"));
    requeststToolbar->addWidget(new QLabel(tr("Requests")));
    requeststToolbar->addAction(tr("Clear"), this, &TrafficPage::clear);
    clear();
    // TODO: nice to have: syntax highlight for request/response bodies
    // new QSyntaxHighlighter(ui->requestBody->document());
    // new QSyntaxHighlighter(ui->responseBody->document());
}

TrafficPage::~TrafficPage()
{
    delete ui;
}

void TrafficPage::setWiremock(const Wiremock::ptr &wiremock)
{
    if(_wiremock)
        disconnect(_wiremock.get(), 0, this, 0);
    this->_wiremock = wiremock;
    connect(_wiremock.get(), &Wiremock::requestsAdded, this, [this](const Wiremock::Requests &requests){
        qDebug() << "Wiremock requests udpated with " << requests.length() << "requests.";
        for(auto request: requests) {
            qDebug() << "Adding request with date: " << request.loggedDateString;
            auto date = new QStandardItem(request.loggedDateString);
            date->setData(request.id);
            auto method = new QStandardItem(request.method);
            auto url = new QStandardItem(request.absoluteUrl.toString());
            requestsModel->appendRow({date, method, url});
        }
    });
    connect(_wiremock.get(), &Wiremock::requestsCleared, this, [this]{
        clear();
    });
}

void TrafficPage::showItem(const Wiremock::Request &request)
{
    auto headers_table = [] (const QVariantMap &headers){
        QString html;
        for(auto key: headers.keys())
            html += QString("<tr><td><i>%1</i></td><td>%2</td></tr>").arg(key).arg(headers[key].toString().toHtmlEscaped());
        return html;
    };
    QString requestTemplate = R"(
                              <table>
                              <tr><td width=200><b>ID</b></td><td>%1</td></tr>
                              <tr><td><b>Url</b></td><td>%2</td></tr>
                              <tr><td><b>Absolute Url</b></td><td>%3</td></tr>
                              <tr><td><b>Method</b></td><td>%4</td></tr>
                              <tr><td><b>client IP</b></td><td>%5</td></tr>
                              <tr><td><b>Date/timestamp</b></td><td>%6 / %7</td></tr>
                              <tr><td><b>Was matched</b></td><td>%8</td></tr>
                              <tr><td colspan=2><b>Headers</b></td></tr>
                              %9
                              </table>
                              )";
    requestTemplate = requestTemplate
            .arg(request.id)
            .arg(request.url.toHtmlEscaped())
            .arg(request.absoluteUrl.toString().toHtmlEscaped())
            .arg(request.method)
            .arg(request.clientIp)
            .arg(request.loggedDateString)
            .arg(request.loggedDate)
            .arg(request.wasMatched ? "true" : "false")
            .arg(headers_table(request.headers))
            ;
    ui->requestView->setHtml(requestTemplate);
    auto findContentType = [](const QVariantMap &headers) {
        for(auto key: headers.keys()) {
            if(key.toLower() == "content-type")
                return headers[key].toString();
        }
        return QString{};
    };

    ui->requestBody->setData(request.body, findContentType(request.headers));
    QString responseTemplate = R"(
                               <table>
                               <tr><td><b>Status</b></td><td>%1</td></tr>
                               <tr><td colspan=2><b>Headers</b></td></tr>
                               %7
                               </table>
                               )";
    responseTemplate = responseTemplate
            .arg(request.response.status)
            .arg(headers_table(request.response.headers));
    ui->responseView->setHtml(responseTemplate);
    ui->responseBody->setData(request.response.body, findContentType(request.response.headers));
    ui->requestJson->setText(QJsonDocument::fromVariant(request.wiremock_data).toJson(QJsonDocument::Indented));
}

QList<QToolBar *> TrafficPage::providedToolBars() const
{
    return {requeststToolbar};
}

void TrafficPage::clear()
{
    requestsModel->clear();
    requestsModel->setColumnCount(3);
    requestsModel->setHorizontalHeaderLabels({tr("Time"), tr("Method"), tr("URL")});
}
