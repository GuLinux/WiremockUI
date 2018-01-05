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
