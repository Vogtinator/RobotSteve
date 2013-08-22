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
    world(world),
    player_body(0),
    player_head(0),
    player_leg_left(0), player_leg_right(0),
    player_arm_left(0), player_arm_right(0)
{
    connect(&tick_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

void GLView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    atlas->bind();
    player_body->draw();
}

void GLView::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 1, 0);

    atlas = new TextureAtlas(*this, QPixmap(":/textures/char.png"));

    double m_per_px = 0.0579;

    static_assert(std::is_same<decltype(8 * m_per_px), decltype(m_per_px)>::value, "Your compiler is really bullshit!");

    player_body = new GLBox(8 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                            4 * m_per_px, 0 * m_per_px, 2 * m_per_px,
                            atlas->getArea(20, 20, 8, 12), atlas->getArea(32, 20, 8, 12),
                            atlas->getArea(20, 16, 8, 4), atlas->getArea(28, 16, 8, 4),
                            atlas->getArea(16, 20, 4, 12), atlas->getArea(28, 20, 4, 12));

    player_body->setXPosition(8 * m_per_px);
    player_body->setYPosition(12 * m_per_px);
    player_body->setZPosition(-10.0f);

    player_head = new GLBox(8 * m_per_px, 8 * m_per_px, 8 * m_per_px,
                            4 * m_per_px, 0, 4 * m_per_px,
                            atlas->getArea(8, 8, 8, 8), atlas->getArea(24, 8, 8, 8),
                            atlas->getArea(8, 0, 8, 8), atlas->getArea(16, 0, 8, 8),
                            atlas->getArea(0, 8, 8, 8), atlas->getArea(16, 8, 8, 8));

    player_head->setYPosition(12 * m_per_px);
    player_body->addChild(player_head);

    player_hat = new GLBox(9 * m_per_px, 9 * m_per_px, 9 * m_per_px,
                           4.5 * m_per_px, 0.5 * m_per_px, 4.5 * m_per_px,
                           atlas->getArea(40, 8, 8, 8), atlas->getArea(56, 8, 8, 8),
                           atlas->getArea(40, 0, 8, 8), atlas->getArea(48, 0, 8, 8),
                           atlas->getArea(32, 8, 8, 8), atlas->getArea(48, 8, 8, 8));

    player_head->addChild(player_hat);

    player_arm_left = new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                2 * m_per_px, 10 * m_per_px, 2 * m_per_px,
                                atlas->getArea(44, 20, 4, 12), atlas->getArea(52, 20, 4, 12),
                                atlas->getArea(44, 16, 4, 4), atlas->getArea(48, 16, 4, 4),
                                atlas->getArea(48, 20, 4, 12), atlas->getArea(40, 20, 4, 12));

    player_arm_left->setXPosition(-6 * m_per_px);
    player_arm_left->setYPosition(10 * m_per_px);
    player_body->addChild(player_arm_left);

    player_arm_right = new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                 2 * m_per_px, 10 * m_per_px, 2 * m_per_px,
                                 atlas->getArea(44, 20, 4, 12), atlas->getArea(52, 20, 4, 12),
                                 atlas->getArea(44, 16, 4, 4), atlas->getArea(48, 16, 4, 4),
                                 atlas->getArea(40, 20, 4, 12), atlas->getArea(48, 20, 4, 12));

    player_arm_right->setXPosition(6 * m_per_px);
    player_arm_right->setYPosition(10 * m_per_px);
    player_body->addChild(player_arm_right);

    player_leg_left = new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                 2 * m_per_px, 12 * m_per_px, 2 * m_per_px,
                                 atlas->getArea(4, 20, 4, 12), atlas->getArea(12, 20, 4, 12),
                                 atlas->getArea(4, 16, 4, 4), atlas->getArea(8, 16, 4, 4),
                                 atlas->getArea(8, 20, 4, 12), atlas->getArea(0, 20, 4, 12));

    player_leg_left->setXPosition(-2 * m_per_px);
    player_body->addChild(player_leg_left);

    player_leg_right = new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                 2 * m_per_px, 12 * m_per_px, 2 * m_per_px,
                                 atlas->getArea(4, 20, 4, 12), atlas->getArea(12, 20, 4, 12),
                                 atlas->getArea(4, 16, 4, 4), atlas->getArea(8, 16, 4, 4),
                                 atlas->getArea(0, 20, 4, 12), atlas->getArea(8, 20, 4, 12));

    player_leg_right->setXPosition(2 * m_per_px);
    player_body->addChild(player_leg_right);

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
    static float s = 0;

    player_body->setYRotation(360-x);
    player_arm_left->setXRotation(sin(s) * 45.0);
    player_arm_right->setXRotation(sin(s) * -45.0);
    player_arm_left->setZRotation(-10);
    player_arm_right->setZRotation(10);
    player_leg_left->setXRotation(sin(s) * -45.0);
    player_leg_right->setXRotation(sin(s) * 45.0);

    s += 0.1;
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
