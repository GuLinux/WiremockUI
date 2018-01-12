#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "core/settings.h"
#include <memory>
#include "core/wiremock.h"

namespace Ui {
class MainWindow;
}

class Wiremock;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    Settings *settings;
    Wiremock::ptr wiremock;
    QList<QToolBar *> tabToolbars;
private slots:
    void on_actionClear_triggered();
    void on_actionServer_address_triggered();
    void on_actionClear_on_Wiremock_triggered();
    void on_tabWidget_currentChanged(int index);
};

#endif // MAINWINDOW_H
