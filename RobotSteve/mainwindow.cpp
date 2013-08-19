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

#include "glview.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "steveinterpreter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    world(5, 5)
{
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(new GLView(&world, this));

    connect(ui->actionStarten, SIGNAL(triggered()), this, SLOT(runCode()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::runCode()
{
    QStringList code = ui->codeEdit->toPlainText().split("\n");
    try {
        interpreter.setCode(code);
        interpreter.dumpCode();
    }
    catch (SteveInterpreterException e) {
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
    }
    ui->codeEdit->clear();
    ui->codeEdit->appendHtml(code.join("<br>"));
}
