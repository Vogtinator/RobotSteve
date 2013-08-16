#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>

#include "world.h"
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
    
private:
    SteveInterpreter interpreter;
    Ui::MainWindow *ui;
    int line = 0;
    World world;
};

#endif // MAINWINDOW_H
