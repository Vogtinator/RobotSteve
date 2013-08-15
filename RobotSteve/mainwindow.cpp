#include <iostream>
#include <QDebug>

#include "glview.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "steveinterpreter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    world(5, 5)
{
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(new GLView(&world, this));

    SteveInterpreter interpreter;
    try {
    interpreter.setCode(QStringList{"WENN asdf DANN", "SCHRITT", "SONST", "RECHTSDREHEN", "*WENN"});
    }
    catch (QString s)
    {
        std::cout << interpreter.getLine();
        throw s;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
