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

    speed_slider = new QSlider(Qt::Horizontal, this);
    speed_slider->setMinimum(0);
    speed_slider->setMaximum(2000);
    speed_slider->setValue(500);
    ui->mainToolBar->addWidget(speed_slider);

    clock.setSingleShot(true);

    connect(ui->actionStarten, SIGNAL(triggered()), this, SLOT(runCode()));
    connect(ui->actionSchritt, SIGNAL(triggered()), this, SLOT(step()));
    connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(reset()));
    connect(speed_slider, SIGNAL(sliderMoved(int)), this, SLOT(setSpeed(int)));
    connect(&clock, SIGNAL(timeout()), this, SLOT(clockEvent()));

    setSpeed(speed_slider->value());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::runCode()
{
    //Start
    if(!clock.isActive())
    {
        ui->actionStarten->setText(QApplication::trUtf8("Stoppen"));
        ui->actionSchritt->setDisabled(true);

        automatic = true;

        clockEvent();
    }
    else //Stop
    {
        ui->actionStarten->setText(QApplication::trUtf8("Starten"));
        ui->actionSchritt->setEnabled(true);

        clock.stop();
    }
}

void MainWindow::step()
{
    automatic = false;

    clockEvent();
}

void MainWindow::startExecution() throw (SteveInterpreterException)
{
    if(execution_started)
        return;

    execution_started = true;
    ui->codeEdit->setReadOnly(true);
    code = ui->codeEdit->toPlainText().split("\n");

    ui->codeEdit->clear();
    for(auto& line : code)
        ui->codeEdit->appendHtml(Qt::convertFromPlainText(line));

    world.reset();
    interpreter.reset();
    interpreter.setCode(code);
}

void MainWindow::stopExecution()
{
    clock.stop();
    ui->actionStarten->setText(QApplication::trUtf8("Starten"));
    ui->actionSchritt->setEnabled(true);
    ui->codeEdit->setReadOnly(false);
    execution_started = false;
}

void MainWindow::reset()
{
    stopExecution();
    interpreter.reset();
    world.reset();

    ui->codeEdit->clear();
    for(auto& line : code)
        ui->codeEdit->appendHtml(Qt::convertFromPlainText(line));
}

void MainWindow::handleError(SteveInterpreterException &e)
{
    //TODO: This function should use convertFromPlainText as well
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

void MainWindow::setSpeed(int ms)
{
    speed_ms = ms;
    world.setSpeed(ms);
}

void MainWindow::clockEvent()
{
    try {
        startExecution();

        //TODO: Redo!
        /*ui->codeEdit->clear();

        QTextCursor cursor(ui->codeEdit->document());
        QTextBlockFormat bf = cursor.blockFormat();
        cursor.movePosition(QTextCursor::Start);

        for(int line = 0; line < code.size(); line++)
        {
            if(line == interpreter.getLine())
                bf.setBackground(QBrush(QColor(255, 255, 0)));
            else
                bf.clearBackground();

            cursor.setBlockFormat(bf);

            ui->codeEdit->appendHtml(Qt::convertFromPlainText(code[line]));
        }*/

        interpreter.executeLine();
        /*interpreter.dumpCode();
        world.dumpWorld();*/

        if(interpreter.executionFinished())
            stopExecution();
        else if(automatic)
            clock.start(speed_ms);
    }
    catch (SteveInterpreterException &e) {
        stopExecution();
        handleError(e);
    }
}
