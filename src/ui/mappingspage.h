#ifndef MAPPINGSPAGE_H
#define MAPPINGSPAGE_H

#include <QWidget>

namespace Ui {
class MappingsPage;
}

class MappingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit MappingsPage(QWidget *parent = 0);
    ~MappingsPage();

private:
    Ui::MappingsPage *ui;
};

#endif // MAPPINGSPAGE_H
