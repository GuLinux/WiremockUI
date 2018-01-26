#ifndef MAPPINGVIEW_H
#define MAPPINGVIEW_H

#include <QWidget>
#include "core/wiremock.h"
class QLineEdit;
class QSpinBox;
class MappingView : public QWidget
{
    Q_OBJECT
public:
    explicit MappingView(QWidget *parent = nullptr);
    void setMapping(const Wiremock::Mapping &mapping);
    Wiremock::Mapping mapping() const;
signals:

public slots:
    void refresh();
private:
    Wiremock::Mapping _mapping;
    QLineEdit *_id;
    QSpinBox *_priority;
    class RequestWidget;
    RequestWidget *_requestWidget;
    class ResponseWidget;
    ResponseWidget *_responseWidget;
};

#endif // MAPPINGVIEW_H
