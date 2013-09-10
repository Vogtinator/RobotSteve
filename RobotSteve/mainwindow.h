#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMutex>
#include <QSlider>
#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>
#include <QSettings>

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
    void codeStep();
    void reset();
    void pauseExecution();
    void stopExecution();
    void setSpeed(int ms);
    void clockEvent();
    void switchViews(bool which);
    void textChanged();
    void refreshButtons();

    void open();
    void save();
    void showExamples();
    void loadExample(QString name, QString filename);
    void openWorld();
    void saveWorld();

    //Manual control:
    void step();
    void turnLeft();
    void turnRight();
    void layDown();
    void pickUp();
    void cube();
    void mark();
    
private:
    void handleError(SteveInterpreterException &e);
    void startExecution() throw (SteveInterpreterException);
    void setCode();
    void loadFile(QString path);

    float speed_ms;
    bool automatic = false, code_changed = true;
    QTimer clock;
    bool execution_started = false;
    Ui::MainWindow *ui;
    QSlider *speed_slider;
    GLWorld world;
    QGraphicsScene chart_scene;
    SteveInterpreter interpreter;
    QStringList code;
    SteveHighlighter *highlighter;
    QTextCharFormat current_line_format, error_format;
    WorldState saved_state;
    QSettings settings;
};

#endif // MAINWINDOW_H
