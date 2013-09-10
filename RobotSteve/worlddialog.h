#ifndef WORLDDIALOG_H
#define WORLDDIALOG_H

#include <QDialog>

#include "world.h"

namespace Ui {
class WorldDialog;
}

class WorldDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit WorldDialog(World *world, QWidget *parent = 0);
    ~WorldDialog();

public slots:
    void accept() override;
    
private:
    World *world;
    Ui::WorldDialog *ui;
};

#endif // WORLDDIALOG_H
