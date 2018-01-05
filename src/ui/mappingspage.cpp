#include "mappingspage.h"
#include "ui_mappingspage.h"

MappingsPage::MappingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MappingsPage)
{
    ui->setupUi(this);
}

MappingsPage::~MappingsPage()
{
    delete ui;
}

void MappingsPage::setWiremock(const Wiremock::ptr &wiremock)
{
    this->wiremock = wiremock;
    connect(wiremock.get(), &Wiremock::mappingsAdded, this, [](const Wiremock::Mappings &mappings){
        for(auto mapping: mappings) {
            qDebug() << "+++ Mapping added:" << mapping.id;
        }
    });
    connect(wiremock.get(), &Wiremock::mappingsRemoved, this, [](const Wiremock::Mappings &mappings){
        for(auto mapping: mappings) {
            qDebug() << "--- Mapping removed:" << mapping.id;
        }
    });
}
