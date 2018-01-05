#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QInputDialog>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(Settings::instance()),
    wiremock(make_shared<Wiremock>(settings))
{
    ui->setupUi(this);
    ui->traffic->setWiremock(this->wiremock);
    if(!settings->geometry().isEmpty())
        restoreGeometry(settings->geometry());
    if(!settings->state().isEmpty())
        restoreState(settings->state());
}

MainWindow::~MainWindow()
{
    settings->geometry(saveGeometry());
    settings->state(saveState());
    delete ui;
}

void MainWindow::on_actionClear_triggered()
{
    ui->traffic->clear();
}

void MainWindow::on_actionServer_address_triggered()
{
    QString newAddress = QInputDialog::getText(this,
                                               tr("Server Address"),
                                               tr("Enter the Wiremock server address to be used\nExample: http://localhost:8888"),
                                               QLineEdit::Normal,
                                               settings->server_address().toString());
    if(! newAddress.isEmpty()) {
        settings->server_address(newAddress);
    }
}

void MainWindow::on_actionClear_on_Wiremock_triggered()
{
    wiremock->clearRequests();
}
