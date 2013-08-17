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
    QStringList code = ui->codeEdit->toPlainText().split("\n");
    try {
        interpreter.setCode(code);
        interpreter.dumpCode();
    }
    catch (SteveInterpreterException e) {
        std::cerr << e.what() << std::endl;
        if(!e.getAffected().isEmpty())
        {
            QString line = code[e.getLineStart()];
            line.replace(e.getAffected(), QString("<i>%1</i>").arg(e.getAffected()));
            code[e.getLineStart()] = line;
        }
        else
        {
            for(int line = e.getLineStart(); line <= e.getLineEnd(); line++)
            {
                code[line].prepend("<i>");
                code[line].append("</i>");
            }
        }
        ui->codeEdit->clear();
        ui->codeEdit->appendHtml(code.join("<br>"));
    }
}
