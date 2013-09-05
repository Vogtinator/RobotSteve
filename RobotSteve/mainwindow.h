/*
 * Author: Fabian Vogt
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 * or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 * Use in public and private schools for educational purposes strongly permitted!
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMutex>
#include <QSlider>
#include <QMainWindow>
#include <QPushButton>

#include "glworld.h"
#include "steveinterpreter.h"
#include "stevehighlighter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void runCode();
    void step();
    void reset();
    void pauseExecution();
    void stopExecution();
    void setSpeed(int ms);
    void clockEvent();
    void switchViews(bool which);
    void textChanged();
    
private:
    void handleError(SteveInterpreterException &e);
    void startExecution() throw (SteveInterpreterException);

    float speed_ms;
    bool automatic;
    QTimer clock;
    bool execution_started = false;
    Ui::MainWindow *ui;
    QSlider *speed_slider;
    GLWorld world;
    SteveInterpreter interpreter;
    QStringList code;
    SteveHighlighter *highlighter;
    QTextCharFormat current_line_format, error_format;
};

#endif // MAINWINDOW_H
