#ifndef MAPPINGSPAGE_H
#define MAPPINGSPAGE_H

#include <QWidget>
#include "core/wiremock.h"

namespace Ui {
class MappingsPage;
}

class MappingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit MappingsPage(QWidget *parent = 0);
    ~MappingsPage();
    void setWiremock(const Wiremock::ptr &wiremock);
private:
    Ui::MappingsPage *ui;
    Wiremock::ptr wiremock;
};

#endif // MAPPINGSPAGE_H
