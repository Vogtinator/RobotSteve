#include <QMessageBox>

#include "worlddialog.h"
#include "ui_worlddialog.h"

WorldDialog::WorldDialog(World *world, QWidget *parent) :
    QDialog(parent),
    world{world},
    ui(new Ui::WorldDialog)
{
    ui->setupUi(this);

    ui->maxHeightSpinBox->setMinimum(0);
    ui->maxHeightSpinBox->setMaximum(100);
    ui->maxHeightSpinBox->setValue(world->getMaxHeight());

    const Size current_size = world->getSize();

    ui->widthSpinBox->setMinimum(World::minimum_size.first);
    ui->widthSpinBox->setMaximum(World::maximum_size.first);
    ui->widthSpinBox->setValue(current_size.first);

    ui->lengthSpinBox->setMinimum(World::minimum_size.second);
    ui->lengthSpinBox->setMaximum(World::maximum_size.second);
    ui->lengthSpinBox->setValue(current_size.second);
}

WorldDialog::~WorldDialog()
{
    delete ui;
}

void WorldDialog::accept()
{
    const Size old_size = world->getSize();
    if(old_size.first != static_cast<unsigned int>(ui->widthSpinBox->value()) ||
            old_size.second != static_cast<unsigned int>(ui->lengthSpinBox->value()))
    {
        if(!world->resize(ui->widthSpinBox->value(), ui->lengthSpinBox->value()))
            QMessageBox::critical(this, trUtf8("Änderung fehlgeschlagen!"), trUtf8("Die Größenänderung ist fehlgeschlagen!"));
    }

    if(world->getMaxHeight() != static_cast<unsigned int>(ui->maxHeightSpinBox->value()))
        world->setMaxHeight(ui->maxHeightSpinBox->value());

    this->close();
}
