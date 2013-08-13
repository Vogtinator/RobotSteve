#include "glview.h"

GLView::GLView(World *world, QWidget *parent) :
    QGLWidget(parent),
    world(world)
{
}

void GLView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLView::initializeGL()
{
    glClearColor(0, 0, 1, 0);
}
