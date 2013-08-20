/*
 * Author: Fabian Vogt
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 * or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 * Use in public and private schools for educational purposes strongly permitted!
 */

#include <iostream>

#include "glview.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "steveinterpreter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    interpreter(&world),
    world(5, 5)
{
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(new GLView(&world, this));

    connect(ui->actionStarten, SIGNAL(triggered()), this, SLOT(runCode()));
    connect(ui->actionSchritt, SIGNAL(triggered()), this, SLOT(step()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(stopExecution()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::runCode()
{
    startExecution();
    interpreter.dumpCode();

    try {
        while(!interpreter.executionFinished())
            interpreter.executeLine();
    }
    catch (SteveInterpreterException e) {
        handleError(e);
    }

    stopExecution();
}

void MainWindow::step()
{
    startExecution();
    try {
        interpreter.executeLine();
    }
    catch (SteveInterpreterException e) {
        handleError(e);
    }

    interpreter.dumpCode();
    if(interpreter.executionFinished())
        stopExecution();
}

void MainWindow::startExecution()
{
    if(execution_started)
        return;

    execution_started = true;
    ui->codeEdit->setReadOnly(true);
    code = ui->codeEdit->toPlainText().split("\n");
    interpreter.reset();

    try {
    interpreter.setCode(code);
    } catch (SteveInterpreterException e)
    {
        handleError(e);
    }
}

void MainWindow::stopExecution()
{
    //Not really needed, but who cares :-)
    if(!execution_started)
        return;

    ui->codeEdit->setReadOnly(false);
    execution_started = false;
}

void MainWindow::handleError(SteveInterpreterException &e)
{
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

    stopExecution();
}
