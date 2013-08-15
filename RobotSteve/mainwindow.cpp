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

    interpreter.setCode(QStringList{
                            "SOLANGE",
                            "*solange",
                            "WEnn",
                            "WENN asdf DANN",
                            "SONST",
                            "*wenn",
                            "RECHTSDREHEN",
                            "*WENN"});
    interpreter.dumpCode();
}

MainWindow::~MainWindow()
{
    delete ui;
}
