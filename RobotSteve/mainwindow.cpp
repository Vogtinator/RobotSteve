#include <iostream>

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

    connect(ui->actionStarten, SIGNAL(triggered()), this, SLOT(runCode()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::runCode()
{
    try {
    interpreter.setCode(ui->codeEdit->toPlainText().split("\n"));
    interpreter.dumpCode();
    }
    catch (SteveInterpreterException e) {
        std::cerr << e.what() << std::endl;
    }
}
