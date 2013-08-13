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
