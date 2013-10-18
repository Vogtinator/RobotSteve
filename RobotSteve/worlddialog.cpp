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
    const Size new_size = {ui->widthSpinBox->value(), ui->lengthSpinBox->value()};

    if(old_size.first != static_cast<unsigned int>(ui->widthSpinBox->value()) ||
            old_size.second != static_cast<unsigned int>(ui->lengthSpinBox->value()))
    {
        if(new_size.first < old_size.first || new_size.second < old_size.second)
        {
            if((world->getX() >= new_size.first || world->getY() >= new_size.second) && world->getObject({0, 0}).has_cube)
            {
                if(QMessageBox::warning(this, trUtf8("Welt leeren?"), trUtf8("Durch die Änderung müsste die Welt geleert werden.\nÜbernehmen?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
                    return;
            }
        }

        if(!world->resize(ui->widthSpinBox->value(), ui->lengthSpinBox->value()))
        {
            QMessageBox::critical(this, trUtf8("Änderung fehlgeschlagen!"), trUtf8("Die Größenänderung ist fehlgeschlagen!"));
            return;
        }
    }

    unsigned int new_height = ui->maxHeightSpinBox->value();

    if(world->getMaxHeight() != new_height)
    {
        if(world->getMaxHeight() > new_height)
        {
            bool cut_down_tower = false;
            for(unsigned int x = 0; x < new_size.first; x++)
            {
                for(unsigned int y = 0; y < new_size.second; y++)
                {
                    if(world->getObject({x, y}).stack_size > new_height)
                    {
                        cut_down_tower = true;
                        break;
                    }
                }

                if(cut_down_tower)
                    break;
            }

            if(cut_down_tower
                    && QMessageBox::warning(this, trUtf8("Stapel kürzen?"), trUtf8("Durch die Änderung müsste mindestens ein Stapel kleiner gemacht werden.\nÜbernehmen?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
                return;
        }

        world->setMaxHeight(ui->maxHeightSpinBox->value());
    }

    this->close();
}
