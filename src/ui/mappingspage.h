#ifndef MAPPINGSPAGE_H
#define MAPPINGSPAGE_H

#include <QWidget>
#include "core/wiremock.h"
#include "toolbarprovider.h"
namespace Ui {
class MappingsPage;
}

class QStandardItemModel;
class QAction;
class MappingsPage : public QWidget, public ToolbarProvider
{
    Q_OBJECT

public:
    explicit MappingsPage(QWidget *parent = 0);
    ~MappingsPage();
    void setWiremock(const Wiremock::ptr &wiremock);
    QList<QToolBar *> providedToolBars() const override;
private slots:
    void showItem(const Wiremock::Mapping &mapping);
    void onMappingSelected();
    void addMapping();
    void clearMappings();
    void editMapping();
    void removeMapping();
private:
    Ui::MappingsPage *ui;
    Wiremock::ptr wiremock;
    QStandardItemModel *model;
    QToolBar *mappingsListToolbar;
    Wiremock::Mapping selectedMapping;
    QAction *actionAddMapping, *actionRemoveMapping, *actionClearMappings, *actionEditMapping;
};

#endif // MAPPINGSPAGE_H
