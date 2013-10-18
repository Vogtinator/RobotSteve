#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMutex>
#include <QSlider>
#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>
#include <QSettings>
#include <QShortcut>
#include <QLabel>

#include "glworld.h"
#include "steveinterpreter.h"
#include "stevehighlighter.h"
#include "steveedit.h"

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

    //Control of execution
    void runCode();
    void codeStep();
    void reset();
    void pauseExecution();
    void stopExecution();
    void clockEvent();
    void setSpeed(int ms);

    //Menu "File"
    void open();
    void saveDirect();
    void save();
    void showExamples();

    //Menu "World"
    void openWorld();
    void saveWorld();
    void showWorldSettings();
    void resetWorld();

    //Menu "Player"
    void loadTexture();
    void loadDefaultTexture();

    //Menu "Help"
    void showHelpDialog();
    void showAbout();
    void showAboutQt();

    //Manual control:
    void step();
    void turnLeft();
    void turnRight();
    void layDown();
    void pickUp();
    void cube();
    void mark();

    //Other stuff
    void switchViews(bool which);
    void textChanged();
    void refreshButtons();
    void loadExample(QString name, QString filename);
    void loadWorldFile(QString path);
    void loadCodeFile(QString path);

protected:
    void closeEvent(QCloseEvent *e);
    
private:
    void handleError(SteveInterpreterException &e);
    bool startExecution() throw (SteveInterpreterException);
    void setCode() throw (SteveInterpreterException);
    void showMessage(const QString &msg);

    float speed_ms;
    bool automatic = false, code_changed = true, code_saved = true;
    QString save_file_name;
    QTimer clock;
    bool execution_started = false;
    Ui::MainWindow *ui;
    QSlider speed_slider;
    GLWorld world;
    QGraphicsScene chart_scene;
    SteveInterpreter interpreter;
    QStringList code;
    SteveHelp help;
    QTextCharFormat current_line_format, error_format;
    WorldState saved_state;
    QSettings settings;
    SteveEdit codeEdit;
    SteveHighlighter highlighter;
    QShortcut save_shortcut;
    QLabel status;
};

#endif // MAINWINDOW_H
