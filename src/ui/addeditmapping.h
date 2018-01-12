#ifndef ADDEDITMAPPING_H
#define ADDEDITMAPPING_H

#include <QDialog>
#include "core/wiremock.h"

namespace Ui {
class AddEditMapping;
}

class AddEditMapping : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditMapping(const Wiremock::Mapping &mapping = {}, QWidget *parent = 0);
    ~AddEditMapping();
public slots:
    void accept() override;
signals:
    void mappingCreated(const QVariantMap &mapping);
    void mappingChanged(const QString &id, const QVariantMap &mapping);
private:
    Ui::AddEditMapping *ui;
    Wiremock::Mapping mapping;
    QVariantMap json2map() const;
    bool validate() const;
};

#endif // ADDEDITMAPPING_H
