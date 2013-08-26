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

#include "glworld.h"
#include "steveinterpreter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    world(5, 5, this),
    interpreter(&world)
{
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(&world);

    connect(ui->actionStarten, SIGNAL(triggered()), this, SLOT(runCode()));
    connect(ui->actionSchritt, SIGNAL(triggered()), this, SLOT(step()));
    connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(reset()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::runCode()
{
    try {
        startExecution();
        interpreter.dumpCode();
        world.dumpWorld();

        while(!interpreter.executionFinished())
            interpreter.executeLine();

        world.dumpWorld();
    }
    catch (SteveInterpreterException &e) {
        handleError(e);
    }
    catch (std::string &e)
    {
        std::cerr << e << std::endl;
    }

    stopExecution();
}

void MainWindow::step()
{
    try {
        startExecution();
        interpreter.executeLine();
        interpreter.dumpCode();
        world.dumpWorld();
    }
    catch (SteveInterpreterException &e) {
        handleError(e);
    }

    if(interpreter.executionFinished())
        stopExecution();
}

void MainWindow::startExecution() throw (SteveInterpreterException)
{
    if(execution_started)
        return;

    execution_started = true;
    ui->codeEdit->setReadOnly(true);
    code = ui->codeEdit->toPlainText().split("\n");

    ui->codeEdit->clear();
    ui->codeEdit->appendHtml(code.join("<br>"));

    world.reset();
    interpreter.reset();
    interpreter.setCode(code);
}

void MainWindow::stopExecution()
{
    //Not really needed, but who cares :-)
    if(!execution_started)
        return;

    ui->codeEdit->setReadOnly(false);
    execution_started = false;
}

void MainWindow::reset()
{
    stopExecution();
    interpreter.reset();
    world.reset();
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
