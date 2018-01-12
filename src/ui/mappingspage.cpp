#include "mappingspage.h"
#include "ui_mappingspage.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHash>
#include <QJsonDocument>
#include <QToolBar>
#include <QLabel>
#include <QDebug>
#include "addeditmapping.h"

MappingsPage::MappingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MappingsPage)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    ui->mappings->setModel(model);
    mappingsListToolbar = new QToolBar(tr("Mappings List"));
    mappingsListToolbar->addWidget(new QLabel(tr("Mappings")));
    actionAddMapping = mappingsListToolbar->addAction(tr("Add"));
    actionRemoveMapping = mappingsListToolbar->addAction(tr("Remove"));
    actionEditMapping = mappingsListToolbar->addAction(tr("Edit"));
    actionClearMappings = mappingsListToolbar->addAction(tr("Clear"));
    connect(ui->mappings->selectionModel(), &QItemSelectionModel::currentChanged, this, &MappingsPage::onMappingSelected);
    connect(actionAddMapping, &QAction::triggered, this, &MappingsPage::addMapping);
    connect(actionRemoveMapping, &QAction::triggered, this, &MappingsPage::removeMapping);
    connect(actionEditMapping, &QAction::triggered, this, &MappingsPage::editMapping);
    connect(actionClearMappings, &QAction::triggered, this, &MappingsPage::clearMappings);
    onMappingSelected();
}

MappingsPage::~MappingsPage()
{
    delete ui;
}

void MappingsPage::setWiremock(const Wiremock::ptr &wiremock)
{
    this->wiremock = wiremock;
    connect(wiremock.get(), &Wiremock::mappingsAdded, this, [this](const Wiremock::Mappings &mappings){
        for(auto mapping: mappings) {
            auto id_column = new QStandardItem(mapping.id);
            id_column->setData(mapping.checksum());
            model->appendRow(id_column);
        }
    });
    connect(wiremock.get(), &Wiremock::mappingsRemoved, this, [this](const Wiremock::Mappings &mappings){
        for(auto mapping: mappings) {
            for(int row=0; row<model->rowCount(); row++) {
                if(model->item(row)->data().toString() == mapping.checksum()) {
                    model->removeRow(row);
                    break;
                }
            }
        }
    });
}

QList<QToolBar *> MappingsPage::providedToolBars() const
{
    return {mappingsListToolbar};
}

void MappingsPage::showItem(const Wiremock::Mapping &mapping)
{
    if(!mapping.valid)
        return;
    ui->json->setText(QJsonDocument::fromVariant(mapping.wiremock_data).toJson(QJsonDocument::Indented));
}

void MappingsPage::onMappingSelected()
{
    auto current = ui->mappings->selectionModel()->currentIndex();
    selectedMapping = {};
    if(current.isValid()) {
        auto checksum = model->item(current.row())->data().toString();
        selectedMapping = wiremock->mapping(checksum);
    }

    actionEditMapping->setEnabled(!selectedMapping.id.isEmpty());
    actionRemoveMapping->setEnabled(!selectedMapping.id.isEmpty());
    ui->json->clear();
    showItem(selectedMapping);
    if(selectedMapping.id.isEmpty())
        return;
}

void MappingsPage::addMapping()
{
    qDebug() << "Add mapping";
    auto addEditMapping = new AddEditMapping({}, this);
    connect(addEditMapping, &AddEditMapping::mappingCreated, this, [this] (const QVariantMap &mapping) {
        wiremock->addMapping(mapping);
    });
    addEditMapping->show();
}

void MappingsPage::clearMappings()
{
    qDebug() << "Clearing mappings";
    wiremock->clearMappings();
}

void MappingsPage::editMapping()
{
    if(selectedMapping.id.isEmpty())
        return;
    qDebug() << "Editing mapping: " << selectedMapping.id;
    auto addEditMapping = new AddEditMapping(selectedMapping, this);
    connect(addEditMapping, &AddEditMapping::mappingChanged, this, [this] (const QString oldId, const QVariantMap &mapping) {
        wiremock->removeMapping(oldId);
        wiremock->addMapping(mapping);
    });
    addEditMapping->show();
}

void MappingsPage::removeMapping()
{
    if(selectedMapping.id.isEmpty())
        return;
    qDebug() << "Removing mapping: " << selectedMapping.id;
    wiremock->removeMapping(selectedMapping.id);
}
