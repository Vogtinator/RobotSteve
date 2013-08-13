#include <QDebug>

#include "glview.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    world(5, 5)
{
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(new GLView(&world, this));
}

MainWindow::~MainWindow()
{
    delete ui;
}
