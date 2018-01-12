#ifndef TOOLBARPROVIDER_H
#define TOOLBARPROVIDER_H
#include <QList>

class QToolBar;
class ToolbarProvider {
  public:
    virtual QList<QToolBar *> providedToolBars() const = 0;
};

#endif // TOOLBARPROVIDER_H
