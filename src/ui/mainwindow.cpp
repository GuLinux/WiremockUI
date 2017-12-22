#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QInputDialog>
#include "core/wiremock.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wiremock(make_shared<Wiremock>(this->_settings))
{
    ui->setupUi(this);
    ui->requests->setWiremock(this->wiremock);
    connect(ui->actionServer_address, &QAction::triggered, this, &MainWindow::setServerAddress);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setServerAddress()
{
    QString newAddress = QInputDialog::getText(this, tr("Server Address"), tr("Enter the Wiremock server address to be used\nExample: http://localhost:8888"));
    if(! newAddress.isEmpty()) {
        _settings.server_address(newAddress);
    }
}
