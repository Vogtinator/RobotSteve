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
    ui->viewFrame->addWidget(&world);

    highlighter = new SteveHighlighter(ui->codeEdit, &interpreter);

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
    connect(ui->viewSwitch, SIGNAL(toggled(bool)), this, SLOT(switchViews(bool)));
    connect(ui->codeEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    setSpeed(speed_slider->value());
}

MainWindow::~MainWindow()
{
    //Slight hack prevents crashing on destruct
    ui->codeEdit->blockSignals(true);
    delete ui;
    delete highlighter;
}

void MainWindow::runCode()
{
    //Start
    if(!clock.isActive())
    {
        world.setSpeed(speed_ms);
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
    //The user can't be really fast, so animations always on
    world.setSpeed(2000);

    clockEvent();
}

void MainWindow::startExecution() throw (SteveInterpreterException)
{
    if(execution_started)
        return;

    ui->statusBar->showMessage(QApplication::trUtf8("Parsen.."));

    execution_started = true;
    ui->codeEdit->setReadOnly(true);
    code = ui->codeEdit->toPlainText().split("\n");

    world.reset();
    interpreter.reset();
    interpreter.setCode(code);

    if(automatic)
        ui->statusBar->showMessage(QApplication::trUtf8("Programm läuft"));
    else
        ui->statusBar->showMessage(QApplication::trUtf8("Programm pausiert"));
}

void MainWindow::stopExecution()
{
    clock.stop();
    ui->statusBar->showMessage(QApplication::trUtf8("Programm beendet"));
    ui->actionStarten->setText(QApplication::trUtf8("Starten"));
    ui->actionSchritt->setEnabled(true);
    ui->codeEdit->setReadOnly(false);
    execution_started = false;

    highlighter->resetHighlight();
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
    ui->statusBar->showMessage(e.what());

    QTextCharFormat format;
    format.setForeground(Qt::white);
    format.setBackground(Qt::red);
    format.setFontItalic(true);
    format.setFontWeight(QFont::Bold);

    highlighter->highlight(e.getLine(), format, e.getAffected());
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

        if(automatic)
            clock.start(speed_ms);

        if(speed_ms > 0 || !automatic)
        {
            QTextCharFormat format;
            format.setBackground(Qt::yellow);
            format.setFontWeight(QFont::Bold);

            highlighter->highlight(interpreter.getLine(), format);
        }

        interpreter.executeLine();
        /*interpreter.dumpCode();
        world.dumpWorld();*/

        if(interpreter.executionFinished())
            stopExecution();
    }
    catch (SteveInterpreterException &e)
    {
        stopExecution();
        handleError(e);
    }
}

void MainWindow::switchViews(bool which)
{
    world.setVisible(!which);
    //TODO
}

void MainWindow::textChanged()
{
    //If user changes the code, the highlighting will no longer be valid
    highlighter->resetHighlight();
}
