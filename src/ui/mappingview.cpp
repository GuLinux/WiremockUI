#include "mappingview.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QJsonDocument>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QSpacerItem>
#include <QGroupBox>
#include <QComboBox>

using namespace std;
class MappingView::RequestWidget : public QGroupBox {
Q_OBJECT
public:
    RequestWidget(Wiremock::Mapping &mapping, QWidget *parent = 0);
public slots:
    void refresh();
private:
    Wiremock::Mapping &mapping;
    QComboBox *method;
};

class MappingView::ResponseWidget : public QGroupBox {
Q_OBJECT
public:
    ResponseWidget(Wiremock::Mapping &mapping, QWidget *parent = 0);
public slots:
    void refresh();
private:
    Wiremock::Mapping &mapping;
};


MappingView::MappingView(QWidget *parent) : QWidget(parent)
{
    setLayout(new QVBoxLayout());
    layout()->addWidget(new QLabel(tr("Id"), this));
    layout()->addWidget(_id = new QLineEdit(this));
    layout()->addWidget(new QLabel(tr("Priority"), this));
    layout()->addWidget(_priority = new QSpinBox(this));
    _priority->setMinimum(0);
    _priority->setMaximum(99);
    layout()->addWidget(_requestWidget = new RequestWidget(_mapping, this));
    layout()->addWidget(_responseWidget = new ResponseWidget(_mapping, this));
    layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
}

void MappingView::setMapping(const Wiremock::Mapping &mapping)
{
    _mapping = mapping;
    refresh();
}

Wiremock::Mapping MappingView::mapping() const
{
    return mapping();
}

void MappingView::refresh()
{
    _id->setText(_mapping.id);
    _priority->setValue(_mapping.priority);
    _requestWidget->refresh();
    _responseWidget->refresh();
}

#include "mappingview.moc"

MappingView::ResponseWidget::ResponseWidget(Wiremock::Mapping &mapping, QWidget *parent) : QGroupBox{parent}, mapping{mapping}
{
    setTitle(tr("Response"));
}

void MappingView::ResponseWidget::refresh()
{

}

MappingView::RequestWidget::RequestWidget(Wiremock::Mapping &mapping, QWidget *parent) : QGroupBox{parent}, mapping{mapping}
{
    setTitle(tr("Request"));
    setLayout(new QVBoxLayout());
    layout()->addWidget(new QLabel(tr("Method")));
    layout()->addWidget(method = new QComboBox(this));
    method->setEditable(false);
    method->addItems({"ANY", "GET", "POST", "PUT", "DELETE", "HEAD", "TRACE", "OPTIONS"});
}

void MappingView::RequestWidget::refresh()
{
    method->setCurrentText(mapping.request.value("method").toString());
}
