/*
 * Author: Fabian Vogt
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 * or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 * Use in public and private schools for educational purposes strongly permitted!
 */

#include <QDebug>

#include "glview.h"

GLView::GLView(World *world, QWidget *parent) :
    QGLWidget(parent),
    world(world)
{
    connect(&tick_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

void GLView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    atlas->bind();
    player_head->draw();
}

void GLView::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 1, 0);

    atlas = new TextureAtlas(*this, QPixmap(":/textures/char.png"));

    player_head = new GLBox(1, 1, 1, 0.5, 0.5, 0.5, atlas->getArea(8, 8, 8, 8), atlas->getArea(24, 8, 8, 8),
                    atlas->getArea(8, 0, 8, 8), atlas->getArea(16, 0, 8, 8),
                    atlas->getArea(0, 8, 8, 8), atlas->getArea(16, 8, 8, 8));

    player_head->setXPosition(1);
    player_head->setYPosition(1);
    player_head->setZPosition(-10.0f);

    tick_timer.start(10);
}

void GLView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(0, 1, 0, 1, 4, 30);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLView::tick()
{
    static int x = 0;

    //box->setXRotation(x);
    player_head->setYRotation(x);
    //box->setZRotation(x);
    x += 1;
    x %= 360;
    updateGL();
}

GLView::~GLView()
{
    delete player_head;
    delete player_body;
    delete player_leg_left;
    delete player_leg_right;
    delete player_arm_left;
    delete player_arm_right;

    delete atlas;
}
