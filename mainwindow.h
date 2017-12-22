#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settings.h"
#include <memory>

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
    Settings _settings;
    std::shared_ptr<Wiremock> wiremock;

public slots:
    void setServerAddress();
};

#endif // MAINWINDOW_H
