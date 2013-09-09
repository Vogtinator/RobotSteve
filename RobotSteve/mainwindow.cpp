#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include "glworld.h"
#include "steveinterpreter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    ui{new Ui::MainWindow},
    world{5, 5, this},
    interpreter{&world}
{
    //UI
    ui->setupUi(this);
    ui->viewFrame->addWidget(&world);
    ui->chart_view->setScene(&chart_scene);
    switchViews(false);

    speed_slider = new QSlider(Qt::Horizontal, this);
    speed_slider->setMinimum(0);
    speed_slider->setMaximum(2000);
    speed_slider->setValue(500);
    ui->mainToolBar->addWidget(speed_slider);

    //Editor
    QFont font;
    font.setFamily("Monospace");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);
    ui->codeEdit->setFont(font);
    QFontMetrics metrics{font};
    ui->codeEdit->setTabStopWidth(4 * metrics.width(' '));

    //Syntax highlighting
    error_format.setForeground(Qt::white);
    error_format.setBackground(Qt::red);
    error_format.setFontItalic(true);
    error_format.setFontWeight(QFont::Bold);

    current_line_format.setBackground(Qt::yellow);
    current_line_format.setFontWeight(QFont::Bold);

    highlighter = new SteveHighlighter{ui->codeEdit, &interpreter};

    //Miscellaneous
    clock.setSingleShot(true);
    setSpeed(speed_slider->value());
    refreshButtons();

    //Signals & Slots
    connect(ui->actionStarten, SIGNAL(triggered()), this, SLOT(runCode()));
    connect(ui->actionSchritt, SIGNAL(triggered()), this, SLOT(codeStep()));
    connect(ui->actionReset, SIGNAL(triggered()), this, SLOT(reset()));
    connect(speed_slider, SIGNAL(sliderMoved(int)), this, SLOT(setSpeed(int)));
    connect(&clock, SIGNAL(timeout()), this, SLOT(clockEvent()));
    connect(ui->viewSwitch, SIGNAL(toggled(bool)), this, SLOT(switchViews(bool)));
    connect(ui->codeEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));

    //Manual control
    connect(ui->buttonStep, SIGNAL(clicked()), this, SLOT(step()));
    connect(ui->buttonLayDown, SIGNAL(clicked()), this, SLOT(layDown()));
    connect(ui->buttonPickUp, SIGNAL(clicked()), this, SLOT(pickUp()));
    connect(ui->buttonTurnLeft, SIGNAL(clicked()), this, SLOT(turnLeft()));
    connect(ui->buttonTurnRight, SIGNAL(clicked()), this, SLOT(turnRight()));
    connect(ui->buttonCube, SIGNAL(clicked()), this, SLOT(cube()));

    if(QCoreApplication::argc() > 1)
        loadFile(QCoreApplication::arguments()[1]);
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
    if(!automatic)
    {
        world.setSpeed(speed_ms);
        ui->actionStarten->setText(QApplication::trUtf8("Stoppen"));
        ui->actionSchritt->setDisabled(true);

        automatic = true;
        refreshButtons();

        clockEvent();
    }
    else
        pauseExecution();
}

void MainWindow::codeStep()
{
    automatic = false;
    //The user can't be really fast, so animations always on
    world.setSpeed(2000);

    clockEvent();

    refreshButtons();
}

void MainWindow::handleError(SteveInterpreterException &e)
{
    std::cerr << e.what() << std::endl;
    ui->statusBar->showMessage(e.what());

    highlighter->highlight(e.getLine(), error_format, e.getAffected());
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

        //Highlight the line which is being executed, but only if it's visible
        //(highlighting costs performance)
        int line = interpreter.getLine();
        if(speed_ms > 0 || !automatic)
            highlighter->highlight(interpreter.getLine(), current_line_format);

        interpreter.executeLine();

        //After a breakpoint current line has to be highlighted
        if(interpreter.hitBreakpoint() && speed_ms == 0 && automatic)
            highlighter->highlight(line, current_line_format);

        if(interpreter.executionFinished())
            stopExecution();
        else if(interpreter.hitBreakpoint())
            pauseExecution();
    }
    catch (SteveInterpreterException &e)
    {
        stopExecution();
        handleError(e);
    }
}

void MainWindow::switchViews(bool which)
{
    if(code_changed)
        setCode();

    //If still changed, code is invalid
    if(code_changed)
    {
        ui->viewSwitch->blockSignals(true);
        ui->viewSwitch->setChecked(!which);
        ui->viewSwitch->blockSignals(false);
        return;
    }

    ui->chart_view->setVisible(which);
    world.setVisible(!which);

    //Structure chart
    if(which)
    {
        chart_scene.clear();
        chart_scene.addPixmap(interpreter.structureChart());
    }
}

void MainWindow::textChanged()
{
    //If user changes the code, the highlighting will no longer be valid
    highlighter->resetHighlight();
    code_changed = true;
}

//Wait for interruption or program end
void MainWindow::startExecution() throw (SteveInterpreterException)
{
    if(execution_started)
        return;

    execution_started = true;
    ui->codeEdit->setReadOnly(true);
    if(code_changed)
        setCode();

    saved_state = world.getState();

    refreshButtons();

    if(automatic)
        ui->statusBar->showMessage(QApplication::trUtf8("Programm läuft"));
    else
        ui->statusBar->showMessage(QApplication::trUtf8("Programm pausiert"));
}

void MainWindow::pauseExecution()
{
    clock.stop();
    automatic = false;

    ui->statusBar->showMessage(QApplication::trUtf8("Programm pausiert"));
    ui->actionStarten->setText(QApplication::trUtf8("Starten"));
    ui->actionSchritt->setEnabled(true);

    refreshButtons();
}

//Wait for reset
void MainWindow::stopExecution()
{
    clock.stop();
    highlighter->resetHighlight();

    ui->codeEdit->setReadOnly(false);

    ui->statusBar->showMessage(QApplication::trUtf8("Programm beendet"));

    ui->actionSchritt->setDisabled(true);
    ui->actionStarten->setDisabled(true);
}

//Wait for start
void MainWindow::reset()
{
    stopExecution();

    ui->actionStarten->setText(QApplication::trUtf8("Starten"));
    ui->actionStarten->setEnabled(true);
    ui->actionSchritt->setEnabled(true);

    execution_started = false;
    automatic = false;

    world.setState(saved_state);
    interpreter.reset();

    refreshButtons();
}

void MainWindow::setCode()
{
    try {
        ui->statusBar->showMessage(QApplication::trUtf8("Parsen.."));

        code = ui->codeEdit->toPlainText().split("\n");
        interpreter.setCode(code);
        interpreter.reset();

        code_changed = false;

        ui->statusBar->showMessage(QApplication::trUtf8("Geparst!"));
    }
    catch (SteveInterpreterException &e)
    {
        handleError(e);
    }
}

void MainWindow::refreshButtons()
{
    if(automatic)
    {
        ui->buttonStep->setDisabled(true);
        ui->buttonLayDown->setDisabled(true);
        ui->buttonPickUp->setDisabled(true);
        ui->buttonTurnLeft->setDisabled(true);
        ui->buttonTurnRight->setDisabled(true);
        ui->buttonCube->setDisabled(true);

        return;
    }

    bool front_blocked = world.frontBlocked();
    bool wall = world.isWall();
    bool brick = !wall && world.getStackSize() > 0;
    bool placeCube = !brick && !wall;

    ui->buttonStep->setDisabled(front_blocked);
    ui->buttonPickUp->setEnabled(brick);
    ui->buttonLayDown->setDisabled(front_blocked);
    ui->buttonCube->setEnabled(placeCube);
    ui->buttonTurnLeft->setDisabled(false);
    ui->buttonTurnRight->setDisabled(false);
}

void MainWindow::loadFile(QString path)
{
    QFile file{path};
    QFileInfo file_info{file};

    settings.setValue("lastOpenDir", file_info.absolutePath());

    if(!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, trUtf8("Fehler beim Öffnen"), trUtf8("Die Datei '%1' konnte nicht geöffnet werden!").arg(file_info.fileName()));
        return;
    }

    ui->codeEdit->setPlainText(file.readAll());

    setCode();
}

void MainWindow::open()
{
    QString filename = QFileDialog::getOpenFileName(this, trUtf8("Programm öffnen"),
                                                    settings.value("lastOpenDir", QDir::homePath()).toString(),
                                                    trUtf8("Programme (*.steve);;Robot Karol Programme (*kdp);;Textdateien (*.txt);;Alle Dateien (*)"));

    //Open dialog closed or cancelled
    if(filename.isEmpty())
        return;

    loadFile(filename);
}

void MainWindow::save()
{
    QString filename = QFileDialog::getSaveFileName(this, trUtf8("Programm speichern"),
                                                    settings.value("lastOpenDir", QDir::homePath()).toString(),
                                                    trUtf8("Programme (*.steve);;Textdateien (*.txt);;Alle Dateien (*)"));

    if(filename.isEmpty())
        return;

    QFile file{filename};
    QFileInfo file_info{file};

    settings.setValue("lastOpenDir", file_info.absolutePath());

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, trUtf8("Fehler beim Öffnen"), trUtf8("Die Datei '%1' konnte nicht geöffnet werden!").arg(file_info.fileName()));
        return;
    }

    QTextStream file_stream{&file};
    file_stream << ui->codeEdit->toPlainText();
}

//Manual control
void MainWindow::cube()
{
    //The user can't be really fast, so animations always on
    world.setSpeed(2000);

    if(!world.isWall())
        world.setCube(!world.isCube());

    refreshButtons();
}

void MainWindow::pickUp()
{
    //The user can't be really fast, so animations always on
    world.setSpeed(2000);

    if(!world.frontBlocked())
        world.pickup(1);

    refreshButtons();
}

void MainWindow::layDown()
{
    //The user can't be really fast, so animations always on
    world.setSpeed(2000);

    if(!world.frontBlocked())
        world.deposit(1);

    refreshButtons();
}

void MainWindow::turnRight()
{
    //The user can't be really fast, so animations always on
    world.setSpeed(2000);

    world.turnRight(1);

    refreshButtons();
}

void MainWindow::turnLeft()
{
    //The user can't be really fast, so animations always on
    world.setSpeed(2000);

    world.turnLeft(1);

    refreshButtons();
}

void MainWindow::step()
{
    //The user can't be really fast, so animations always on
    world.setSpeed(2000);

    world.stepForward();

    refreshButtons();
}
