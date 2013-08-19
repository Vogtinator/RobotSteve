/*
 * Author: Fabian Vogt
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 * or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 * Use in public and private schools for educational purposes strongly permitted!
 */

#ifndef GLVIEW_H
#define GLVIEW_H

#include <memory>
#include <QGLWidget>
#include "world.h"

class GLView : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLView(World *world, QWidget *parent = 0);
    void paintGL();
    void initializeGL();
    
private:
    World *world;
};

#endif // GLVIEW_H
