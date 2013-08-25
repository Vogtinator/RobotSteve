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

#include <QMainWindow>

#include "glworld.h"
#include "steveinterpreter.h"

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
    void stopExecution();
    
private:
    void handleError(SteveInterpreterException &e);
    void startExecution() throw (SteveInterpreterException);

    bool execution_started = false;
    Ui::MainWindow *ui;
    GLWorld world;
    SteveInterpreter interpreter;
    QStringList code;
};

#endif // MAINWINDOW_H
