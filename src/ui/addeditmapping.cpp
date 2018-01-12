#include "addeditmapping.h"
#include "ui_addeditmapping.h"

#include <QDebug>
#include <QJsonDocument>
#include <QMessageBox>

AddEditMapping::AddEditMapping(const Wiremock::Mapping &mapping, QWidget *parent) :
    QDialog(parent),
    mapping{mapping},
    ui(new Ui::AddEditMapping)
{
    ui->setupUi(this);
    connect(this, &QDialog::finished, this, &QDialog::deleteLater);
    if(mapping.valid) {
        ui->json->setPlainText(QJsonDocument::fromVariant(mapping.wiremock_data).toJson(QJsonDocument::Indented));
    }
}

AddEditMapping::~AddEditMapping()
{
    delete ui;
}

void AddEditMapping::accept()
{
    qDebug() <<  json2map().isEmpty();
    if(! validate()) {
        QMessageBox::warning(this, tr("Invalid JSON"), tr("An invalid JSON was detected."));
        return;
    }
    if(mapping.valid) {
        emit mappingChanged(mapping.id, json2map());
    } else {
        emit mappingCreated(json2map());
    }
    QDialog::accept();
}

QVariantMap AddEditMapping::json2map() const
{
    return QJsonDocument::fromJson(ui->json->toPlainText().toLatin1()).toVariant().toMap();
}

bool AddEditMapping::validate() const
{
    auto json = json2map();
    if(json.empty())
        return false;
    return true;
}

