#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QLabel>

#include "glworld.h"
#include "steveinterpreter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "examplesdialog.h"
#include "worlddialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    ui{new Ui::MainWindow},
    world{5, 5, 5, this},
    interpreter{&world},
    help{&interpreter},
    codeEdit{&help, this},
    save_shortcut(QKeySequence("Ctrl+S"), this)
{
    //UI
    ui->setupUi(this);
    codeEdit.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    ui->editFrame->addWidget(&codeEdit);
    ui->viewFrame->addWidget(&world);
    ui->chart_view->setScene(&chart_scene);
    switchViews(false);
    ui->statusBar->addWidget(&status);

    speed_slider = new QSlider(Qt::Horizontal, this);
    speed_slider->setMinimum(0);
    speed_slider->setMaximum(2000);
    speed_slider->setValue(500);
    ui->mainToolBar->addWidget(speed_slider);

    //Help
    if(!help.loadFile(":/help/help.xml"))
        QMessageBox::critical(this, trUtf8("Fehler beim Laden"), trUtf8("Die Hilfe konnte nicht geladen werden."));

    //Editor
    QFont font;
    font.setFamily("Monospace");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);
    codeEdit.setFont(font);
    QFontMetrics metrics{font};
    codeEdit.setTabStopWidth(4 * metrics.width(' '));

    //Syntax highlighting
    error_format.setForeground(Qt::white);
    error_format.setBackground(Qt::red);
    error_format.setFontItalic(true);
    error_format.setFontWeight(QFont::Bold);

    current_line_format.setBackground(Qt::yellow);
    current_line_format.setFontWeight(QFont::Bold);

    highlighter = new SteveHighlighter{&codeEdit, &interpreter};

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
    connect(&codeEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionSaveDirect, SIGNAL(triggered()), this, SLOT(saveDirect()));
    connect(&save_shortcut, SIGNAL(activated()), this, SLOT(saveDirect()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveWorld, SIGNAL(triggered()), this, SLOT(saveWorld()));
    connect(ui->actionLoadWorld, SIGNAL(triggered()), this, SLOT(openWorld()));
    connect(ui->actionExamples, SIGNAL(triggered()), this, SLOT(showExamples()));
    connect(ui->actionHideCode, SIGNAL(toggled(bool)), &codeEdit, SLOT(setHidden(bool)));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionSettingsWorld, SIGNAL(triggered()), this, SLOT(showWorldSettings()));
    connect(ui->actionResetWorld, SIGNAL(triggered()), this, SLOT(resetWorld()));
    connect(ui->actionDefaultTexture, SIGNAL(triggered()), this, SLOT(loadDefaultTexture()));
    connect(ui->actionLoadTexture, SIGNAL(triggered()), this, SLOT(loadTexture()));
    connect(ui->actionHelpDialog, SIGNAL(triggered()), this, SLOT(showHelpDialog()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(showAboutQt()));

    //Manual control
    connect(ui->buttonStep, SIGNAL(clicked()), this, SLOT(step()));
    connect(ui->buttonLayDown, SIGNAL(clicked()), this, SLOT(layDown()));
    connect(ui->buttonPickUp, SIGNAL(clicked()), this, SLOT(pickUp()));
    connect(ui->buttonTurnLeft, SIGNAL(clicked()), this, SLOT(turnLeft()));
    connect(ui->buttonTurnRight, SIGNAL(clicked()), this, SLOT(turnRight()));
    connect(ui->buttonCube, SIGNAL(clicked()), this, SLOT(cube()));
    connect(ui->buttonMark, SIGNAL(clicked()), this, SLOT(mark()));

    QFile player_texture{settings.value("playerTexture", ":/textures/char.png").toString()};
    if(!player_texture.exists())
        loadDefaultTexture();

    world.setPlayerTexture(settings.value("playerTexture", ":/textures/char.png").toString());

    showMessage(trUtf8("Hallo!"));
}

MainWindow::~MainWindow()
{
    //Slight hack prevents crashing on destruct
    codeEdit.blockSignals(true);
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
    showMessage(e.what());

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
    }
    catch (SteveInterpreterException &e)
    {
        reset();
        handleError(e);

        return;
    }

    try{
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

        if(automatic)
            clock.start(speed_ms);
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
    {
        try {
        setCode(); }
        catch (SteveInterpreterException &e)
        {
            handleError(e);
        }
    }

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
    code_saved = false;
}

//Wait for interruption or program end
bool MainWindow::startExecution() throw (SteveInterpreterException)
{
    if(execution_started)
        return true;

    codeEdit.setReadOnly(true);
    if(code_changed)
        setCode();

    execution_started = true;

    saved_state = world.getState();

    refreshButtons();

    if(automatic)
        showMessage(QApplication::trUtf8("Programm läuft"));
    else
        showMessage(QApplication::trUtf8("Programm pausiert"));

    return true;
}

void MainWindow::pauseExecution()
{
    clock.stop();
    automatic = false;

    showMessage(QApplication::trUtf8("Programm pausiert"));
    ui->actionStarten->setText(QApplication::trUtf8("Starten"));
    ui->actionSchritt->setEnabled(true);

    refreshButtons();
}

//Wait for reset
void MainWindow::stopExecution()
{
    clock.stop();
    highlighter->resetHighlight();

    codeEdit.setReadOnly(false);

    showMessage(QApplication::trUtf8("Programm beendet"));

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

void MainWindow::setCode() throw (SteveInterpreterException)
{
    showMessage(QApplication::trUtf8("Parsen.."));

    code = codeEdit.toPlainText().split("\n");
    interpreter.setCode(code);
    interpreter.reset();

    code_changed = false;

    showMessage(QApplication::trUtf8("Geparst!"));
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
        ui->buttonMark->setDisabled(true);
        ui->actionLoadWorld->setDisabled(true);
        ui->actionSettingsWorld->setDisabled(true);
        ui->actionResetWorld->setDisabled(true);
        ui->actionOpen->setDisabled(true);
        ui->actionExamples->setDisabled(true);

        return;
    }

    bool front_blocked = world.frontBlocked();
    bool wall = world.isWall();
    bool brick = !wall && world.getStackSize() > 0;
    bool placeCube = !brick && !wall;

    ui->buttonStep->setDisabled(front_blocked);
    ui->buttonPickUp->setEnabled(brick);
    ui->buttonLayDown->setEnabled(!front_blocked && world.getStackSize() < world.getMaxHeight());
    ui->buttonCube->setEnabled(placeCube);
    ui->buttonTurnLeft->setDisabled(false);
    ui->buttonTurnRight->setDisabled(false);
    ui->buttonMark->setDisabled(false);
    ui->actionLoadWorld->setDisabled(false);
    ui->actionSettingsWorld->setDisabled(false);
    ui->actionResetWorld->setDisabled(false);
    ui->actionOpen->setDisabled(false);
    ui->actionExamples->setDisabled(false);
}

void MainWindow::loadWorldFile(QString path)
{
    QFileInfo file_info{path};

    if(!world.loadFile(file_info.absoluteFilePath()))
        QMessageBox::critical(this, trUtf8("Fehler beim Öffnen"), trUtf8("Die Datei '%1' konnte nicht geöffnet werden!").arg(file_info.fileName()));
}

void MainWindow::loadCodeFile(QString path)
{
    QFile file{path};
    QFileInfo file_info{file};

    if(!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, trUtf8("Fehler beim Öffnen"), trUtf8("Die Datei '%1' konnte nicht geöffnet werden!").arg(file_info.fileName()));
        return;
    }

    codeEdit.setPlainText(file.readAll());

    try {
    setCode(); }
    catch (SteveInterpreterException &e)
    {
        handleError(e);
    }

    if(!path.startsWith(":"))
    {
        save_file_name = path;
        ui->actionSaveDirect->setDisabled(false);
    }
    code_saved = true;
}

void MainWindow::open()
{
    QString filename = QFileDialog::getOpenFileName(this, trUtf8("Programm öffnen"),
                                                    settings.value("lastOpenDir", QDir::homePath()).toString(),
                                                    trUtf8("Programme (*.steve);;Robot Karol Programme (*kdp);;Textdateien (*.txt);;Alle Dateien (*)"));

    //Open dialog closed or cancelled
    if(filename.isEmpty())
        return;

    QFileInfo file_info{filename};

    settings.setValue("lastOpenDir", file_info.absolutePath());

    loadCodeFile(filename);
}

void MainWindow::saveDirect()
{
    if(save_file_name.isEmpty())
        return;

    QFile file{save_file_name};
    QFileInfo file_info{file};

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, trUtf8("Fehler beim Speichern"), trUtf8("Die Datei '%1' konnte nicht gespeichert werden!").arg(file_info.fileName()));
        return;
    }

    QTextStream file_stream{&file};
    file_stream << codeEdit.toPlainText();

    code_saved = true;

    showMessage(trUtf8("Gespeichert als '%1'!").arg(file_info.fileName()));
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
    save_file_name = filename;
    ui->actionSaveDirect->setDisabled(false);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, trUtf8("Fehler beim Speichern"), trUtf8("Die Datei '%1' konnte nicht gespeichert werden!").arg(file_info.fileName()));
        return;
    }

    QTextStream file_stream{&file};
    file_stream << codeEdit.toPlainText();

    code_saved = true;

    showMessage(trUtf8("Gespeichert als '%1'!").arg(file_info.fileName()));
}

void MainWindow::openWorld()
{
    QString filename = QFileDialog::getOpenFileName(this, trUtf8("Welt öffnen"),
                                                    settings.value("lastOpenWorldDir", QDir::homePath()).toString(),
                                                    trUtf8("Welt (*.stworld);;Alle Dateien (*)"));

    //Open dialog closed or cancelled
    if(filename.isEmpty())
        return;

    QFileInfo file_info{filename};

    settings.setValue("lastOpenWorldDir", file_info.absolutePath());

    loadWorldFile(filename);

    refreshButtons();
}

void MainWindow::saveWorld()
{
    QString filename = QFileDialog::getSaveFileName(this, trUtf8("Welt speichern"),
                                                    settings.value("lastOpenWorldDir", QDir::homePath()).toString(),
                                                    trUtf8("Welt (*.stworld);;Alle Dateien (*)"));

     if(filename.isEmpty())
         return;

     QFileInfo file_info{filename};

     settings.setValue("lastOpenWorldDir", file_info.absolutePath());

     if(!world.saveFile(filename))
         QMessageBox::critical(this, trUtf8("Fehler beim Speichern"), trUtf8("Die Datei '%1' konnte nicht gespeichert werden!").arg(file_info.fileName()));
}

void MainWindow::showExamples()
{
    ExamplesDialog examples_dialog{this};

    connect(&examples_dialog, SIGNAL(exampleChosen(QString,QString)), this, SLOT(loadExample(QString,QString)));

    examples_dialog.show();
    examples_dialog.exec();
}

void MainWindow::loadExample(QString name, QString filename)
{
    Q_UNUSED(name); //Will be used for tabs later.

    if(!world.loadFile(QString(":/examples/Examples/%1.stworld").arg(filename)))
    {
        QMessageBox::critical(this, trUtf8("Fehler beim Laden"), trUtf8("Das Beispiel konnte nicht geladen werden!"));
        return;
    }

    loadCodeFile(QString(":/examples/Examples/%1.steve").arg(filename));

    code_saved = true;
    code_changed = false;
}

void MainWindow::showWorldSettings()
{
    WorldDialog world_dialog{&world, this};

    world_dialog.show();
    world_dialog.exec();

    refreshButtons();
}

void MainWindow::resetWorld()
{
    world.reset();
}

void MainWindow::loadDefaultTexture()
{
    settings.setValue("playerTexture", ":/textures/char.png");

    world.setPlayerTexture(settings.value("playerTexture", ":/textures/char.png").toString());
}

void MainWindow::loadTexture()
{
    QString filename = QFileDialog::getOpenFileName(this, trUtf8("Textur Öffnen"),
                                                    settings.value("lastOpenSkinDir", QDir::homePath()).toString(),
                                                    trUtf8("Textur (*.png)"));

    //Open dialog closed or cancelled
    if(filename.isEmpty())
        return;

    QFileInfo file_info{filename};

    settings.setValue("lastOpenSkinDir", file_info.absolutePath());

    settings.setValue("playerTexture", filename);

    world.setPlayerTexture(settings.value("playerTexture", ":/textures/char.png").toString());
}

void MainWindow::showMessage(const QString &msg)
{
    status.setText(msg);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if(!code_saved)
    {
        if(QMessageBox::warning(this, trUtf8("Wirklich beenden?"), trUtf8("Das Programm wurde noch nicht gespeichert.\nSoll Robot Steve wirklich beendet werden?"), QMessageBox::Yes, QMessageBox::No)
                == QMessageBox::No)
        {
            e->ignore();
            return;
        }
    }

    e->accept();
}

void MainWindow::showHelpDialog()
{
    help.showHelp();
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "Robot Steve", "Robot Steve\n"
                       "Autor: Fabian Vogt\n\n"
                       "This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.\n"
                       "To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/\n"
                       "or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.");
}

void MainWindow::showAboutQt()
{
    QMessageBox::aboutQt(this);
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

void MainWindow::mark()
{
    //The user can't be really fast, so animations always on
    world.setSpeed(2000);

    world.setMark(!world.isMarked());

    //Refresh not needed
}
