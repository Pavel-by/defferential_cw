#include "drawer.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    Drawer* drawer = new Drawer();

    ui->setupUi(this);
    ui->verticalLayout->addWidget(drawer, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
