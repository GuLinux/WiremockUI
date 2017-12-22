#include "requestspage.h"
#include "ui_requestspage.h"
#include <QTimer>

RequestsPage::RequestsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RequestsPage)
{
    ui->setupUi(this);
    connect(&_refreshTimer, &QTimer::timeout, this, [this] {
        if(_wiremock)
            _wiremock->queryRequests();
    });
    _refreshTimer.start(500);
}

RequestsPage::~RequestsPage()
{
    delete ui;
}
