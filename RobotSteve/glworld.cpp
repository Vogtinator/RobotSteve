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
#include <QMouseEvent>

#include "glworld.h"

GLWorld::GLWorld(int width, int length, QWidget *parent) :
    QGLWidget(parent),
    World(width, length),
    camera_rotX(-30),
    camera_rotY(15),
    camera_dist(5),
    player_body(0),
    player_head(0),
    player_leg_left(0), player_leg_right(0),
    player_arm_left(0), player_arm_right(0)
{
    anim_ticks.insert(ANIM_STANDING, 1);
    anim_ticks.insert(ANIM_TURNLEFT, 124);
    anim_ticks.insert(ANIM_TURNRIGHT, 124);
    anim_ticks.insert(ANIM_BEND, 124);
    anim_ticks.insert(ANIM_WAIT, 62);
    anim_ticks.insert(ANIM_STEP, 124);
    anim_ticks.insert(ANIM_GREET1, 31);
    anim_ticks.insert(ANIM_GREET2, 200);
    anim_ticks.insert(ANIM_GREET3, 31);
    anim_next.insert(ANIM_GREET1, ANIM_GREET2);
    anim_next.insert(ANIM_GREET2, ANIM_GREET3);
    anim_next.insert(ANIM_GREET3, ANIM_STANDING);
    anim_next.insert(ANIM_STANDING, ANIM_STANDING);
    anim_next.insert(ANIM_WAIT, ANIM_WAIT);
    anim_next.insert(ANIM_TURNLEFT, ANIM_STANDING);
    anim_next.insert(ANIM_TURNRIGHT, ANIM_STANDING);
    anim_next.insert(ANIM_BEND, ANIM_STANDING);

    player_atlas = new TextureAtlas(*this, QPixmap(":/textures/char.png"));
    environment_atlas = new TextureAtlas(*this, QPixmap(":/textures/environment.png"));

    double m_per_px = 0.0579;

    static_assert(std::is_same<decltype(8 * m_per_px), decltype(m_per_px)>::value, "Your compiler is really bullshit!");

    player_body = new GLBox(8 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                            4 * m_per_px, 0 * m_per_px, 2 * m_per_px,
                            player_atlas->getArea(20, 20, 8, 12), player_atlas->getArea(32, 20, 8, 12),
                            player_atlas->getArea(20, 16, 8, 4), player_atlas->getArea(28, 16, 8, 4),
                            player_atlas->getArea(16, 20, 4, 12), player_atlas->getArea(28, 20, 4, 12));

    player_body->setYRotation(player_rotY_from = 180);
    player_body->setYPosition(-14 * m_per_px);

    player_arm_left = new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                2 * m_per_px, 10 * m_per_px, 2 * m_per_px,
                                player_atlas->getArea(44, 20, 4, 12), player_atlas->getArea(52, 20, 4, 12),
                                player_atlas->getArea(44, 16, 4, 4), player_atlas->getArea(48, 16, 4, 4),
                                player_atlas->getArea(48, 20, 4, 12), player_atlas->getArea(40, 20, 4, 12));

    player_arm_left->setXPosition(-6 * m_per_px);
    player_arm_left->setYPosition(10 * m_per_px);
    player_arm_left->setZRotation(-10);
    player_body->addChild(player_arm_left);

    player_arm_right = new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                 2 * m_per_px, 10 * m_per_px, 2 * m_per_px,
                                 player_atlas->getArea(44, 20, 4, 12), player_atlas->getArea(52, 20, 4, 12),
                                 player_atlas->getArea(44, 16, 4, 4), player_atlas->getArea(48, 16, 4, 4),
                                 player_atlas->getArea(40, 20, 4, 12), player_atlas->getArea(48, 20, 4, 12));

    player_arm_right->setXPosition(6 * m_per_px);
    player_arm_right->setYPosition(10 * m_per_px);
    player_arm_right->setZRotation(10);
    player_body->addChild(player_arm_right);

    player_leg_left = new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                 2 * m_per_px, 12 * m_per_px, 2 * m_per_px,
                                 player_atlas->getArea(4, 20, 4, 12), player_atlas->getArea(12, 20, 4, 12),
                                 player_atlas->getArea(4, 16, 4, 4), player_atlas->getArea(8, 16, 4, 4),
                                 player_atlas->getArea(8, 20, 4, 12), player_atlas->getArea(0, 20, 4, 12));

    player_leg_left->setXPosition(-2 * m_per_px);
    player_leg_left->setZRotation(-5);
    player_body->addChild(player_leg_left);

    player_leg_right = new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                 2 * m_per_px, 12 * m_per_px, 2 * m_per_px,
                                 player_atlas->getArea(4, 20, 4, 12), player_atlas->getArea(12, 20, 4, 12),
                                 player_atlas->getArea(4, 16, 4, 4), player_atlas->getArea(8, 16, 4, 4),
                                 player_atlas->getArea(0, 20, 4, 12), player_atlas->getArea(8, 20, 4, 12));

    player_leg_right->setXPosition(2 * m_per_px);
    player_leg_right->setZRotation(5);
    player_body->addChild(player_leg_right);

    player_head = new GLBox(8 * m_per_px, 8 * m_per_px, 8 * m_per_px,
                            4 * m_per_px, 0, 4 * m_per_px,
                            player_atlas->getArea(8, 8, 8, 8), player_atlas->getArea(24, 8, 8, 8),
                            player_atlas->getArea(8, 0, 8, 8), player_atlas->getArea(16, 0, 8, 8),
                            player_atlas->getArea(0, 8, 8, 8), player_atlas->getArea(16, 8, 8, 8));

    player_head->setYPosition(12 * m_per_px);
    player_body->addChild(player_head);

    player_hat = new GLBox(9 * m_per_px, 9 * m_per_px, 9 * m_per_px,
                           4.5 * m_per_px, 0.5 * m_per_px, 4.5 * m_per_px,
                           player_atlas->getArea(40, 8, 8, 8), player_atlas->getArea(56, 8, 8, 8),
                           player_atlas->getArea(40, 0, 8, 8), player_atlas->getArea(48, 0, 8, 8),
                           player_atlas->getArea(32, 8, 8, 8), player_atlas->getArea(48, 8, 8, 8));

    player_head->addChild(player_hat);

    wall = new GLQuad(1, 1, 0.5, 0.5, 0.5, environment_atlas->getArea(16, 0, 16, 16));

    wall->setXRotation(90);

    floor = new GLQuad(1, 1, 0.5, 0.5, 0.5, environment_atlas->getArea(0, 0, 16, 16));

    floor->setYPosition(-1);

    marked_floor = new GLQuad(1, 1, 0.5, 0.5, 0.5, environment_atlas->getArea(32, 0, 16, 16));

    marked_floor->setYPosition(-1);

    connect(&tick_timer, SIGNAL(timeout()), this, SLOT(tick()));
    connect(&refresh_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    refresh_timer.start(17);

    updateAnimationTarget();

    setAnimation(ANIM_GREET1);
}

void GLWorld::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glPushMatrix();

    glRotatef(camera_rotX, -1, 0, 0);
    glRotatef(camera_rotY, 0, -1, 0);

    float radX = camera_rotX*(M_PI/180), radY = camera_rotY*(M_PI/180), cosradX = cos(radX);

    double camerax = camera_dist*(sin(radY) * cosradX) + World::size.first/2;
    double cameray = camera_dist*sin(radX-M_PI);
    double cameraz = camera_dist*(cos(radY) * cosradX) + World::size.second/2;

    glTranslated(-camerax, -cameray, -cameraz);

    environment_atlas->bind();

    glEnable(GL_CULL_FACE);

    wall->setYRotation(0);
    wall->setZPosition(0);
    drawWallX();
    wall->setYRotation(180);
    wall->setZPosition(World::size.second-1);
    drawWallX();

    wall->setYRotation(90);
    wall->setXPosition(0);
    drawWallZ();
    wall->setYRotation(270);
    wall->setXPosition(World::size.first - 1);
    drawWallZ();

    for(int x = 0; x < World::size.first; x++)
        for(int y = 0; y < World::size.second; y++)
        {
            if(World::map[x][y].has_mark)
            {
                marked_floor->setXPosition(x);
                marked_floor->setZPosition(y);
                marked_floor->draw();
            }
            else
            {
                floor->setXPosition(x);
                floor->setZPosition(y);
                floor->draw();
            }
        }

    //TODO: Cubes and bricks

    glDisable(GL_CULL_FACE);

    player_atlas->bind();
    player_body->draw();

    glPopMatrix();
}

void GLWorld::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 1, 0);
}

void GLWorld::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

float lin_terpolation(float from, float to, float x)
{
    return from + (to-from)*x;
}

void GLWorld::tick()
{
    anim_progress = (static_cast<float>(current_anim_ticks)) / static_cast<float>(anim_ticks[current_animation]);

    switch(current_animation)
    {
    case ANIM_STANDING:
        break;
    case ANIM_TURNLEFT:
    case ANIM_TURNRIGHT:
    case ANIM_STEP:
        player_arm_left->setXRotation(sin(anim_progress * 2*M_PI) * 45.0);
        player_arm_right->setXRotation(sin(anim_progress * 2*M_PI) * -45.0);
        player_leg_left->setXRotation(sin(anim_progress * 2*M_PI) * -45.0);
        player_leg_right->setXRotation(sin(anim_progress * 2*M_PI) * 45.0);
        break;
    case ANIM_GREET1:
        player_arm_right->setZRotation(sin(anim_progress * M_PI/2) * -5);
        player_arm_right->setXRotation(sin(anim_progress * M_PI/2) * 180.0);
        break;
    case ANIM_GREET2:
        player_arm_right->setZRotation(sin(anim_progress * M_PI) * 15.0);
        break;
    case ANIM_GREET3:
        player_arm_right->setZRotation(cos(anim_progress * M_PI/2) * -5);
        player_arm_right->setXRotation(cos(anim_progress * M_PI/2) * 180.0);
        break;
    case ANIM_WAIT:
        player_head->setXRotation(sin(anim_progress * M_PI) * 15.0);
        break;
    case ANIM_BEND:
        player_body->setXRotation(sin(anim_progress * M_PI) * -50);
        player_leg_left->setXRotation(sin(anim_progress * M_PI) * 50);
        player_leg_right->setXRotation(sin(anim_progress * M_PI) * 50);
        player_arm_left->setXRotation(sin(anim_progress * M_PI) * 90);
        player_arm_right->setXRotation(sin(anim_progress * M_PI) * 90);
        break;
    }

    if(player_rotY_target != player_rotY_from)
        player_body->setYRotation(lin_terpolation(player_rotY_from, player_rotY_target, sin(anim_progress * M_PI/2)));

    if(player_posX_target != player_posX_from || player_posZ_target != player_posZ_from)
    {
        player_body->setXPosition(lin_terpolation(player_posX_from, player_posX_target, sin(anim_progress * M_PI/2)));
        player_body->setZPosition(lin_terpolation(player_posZ_from, player_posZ_target, sin(anim_progress * M_PI/2)));
    }

    current_anim_ticks++;
    if(current_anim_ticks == anim_ticks[current_animation])
        setAnimation(anim_next[current_animation]);
}

void GLWorld::mousePressEvent(QMouseEvent *event)
{
    last_pos = event->pos();
}

void GLWorld::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & (Qt::LeftButton | Qt::RightButton))
    {
        camera_rotX -= 0.1 * (event->y() - last_pos.y());
        camera_rotY -= 0.1 * (event->x() - last_pos.x());

        if(camera_rotX < -90)
            camera_rotX = -90;
        else if(camera_rotX > 0)
            camera_rotX = 0;

        while(camera_rotY > 360)
            camera_rotY -= 360;
        while(camera_rotY < 0)
            camera_rotY += 360;
    }

    last_pos = event->pos();
}

GLWorld::~GLWorld()
{
    delete player_body;
    delete player_head;
    delete player_hat;
    delete player_leg_left;
    delete player_leg_right;
    delete player_arm_left;
    delete player_arm_right;

    delete floor;
    delete wall;

    delete player_atlas;
    delete environment_atlas;
}

void GLWorld::wheelEvent(QWheelEvent *event)
{
    camera_dist -= event->delta() * 0.005;

    if(camera_dist < 5)
        camera_dist = 5;
    if(camera_dist > 50)
        camera_dist = 50;
}

void GLWorld::setAnimation(ANIMATION animation)
{
    if(speed_ms >= 500)
        speed_ms = 500;
    if(speed_ms <= 250)
        speed_ms = 0;

    //If previous animation didn't complete, skip it
    if(anim_progress != 1.0f)
    {
        player_body->setXPosition(player_posX_from = player_posX_target);
        player_body->setZPosition(player_posZ_from = player_posZ_target);
        player_body->setYRotation(player_rotY_from = player_rotY_target);
    }

    current_anim_ticks = 0;
    current_animation = animation;
    tick_timer.stop();

    if(speed_ms > 0)
        tick_timer.start(speed_ms / anim_ticks[current_animation]);

    anim_progress = 0;
}

bool GLWorld::stepForward()
{
    if(!World::stepForward())
        return false;

    setAnimation(ANIM_STEP);
    updateAnimationTarget();

    return true;
}

void GLWorld::turnRight()
{
    World::turnRight();
    setAnimation(ANIM_TURNRIGHT);
    updateAnimationTarget();
}

void GLWorld::turnLeft()
{
    World::turnLeft();
    setAnimation(ANIM_TURNLEFT);
    updateAnimationTarget();
}

void GLWorld::setMark(bool b)
{
    World::setMark(b);
    setAnimation(ANIM_BEND);
}

bool GLWorld::setCube(bool b)
{
    if(!World::setCube(b))
        return false;

    setAnimation(ANIM_BEND);
    updateAnimationTarget();

    return true;
}

void GLWorld::drawWallX()
{
    for(int x = 0; x < World::size.first; x++)
    {
        wall->setXPosition(x);
        wall->setYPosition(-1);
        wall->draw();
        wall->setYPosition(0);
        wall->draw();
        wall->setYPosition(1);
        wall->draw();
        wall->setYPosition(2);
        wall->draw();
    }
}

void GLWorld::drawWallZ()
{
    for(int y = 0; y < World::size.second; y++)
    {
        wall->setZPosition(y);
        wall->setYPosition(-1);
        wall->draw();
        wall->setYPosition(0);
        wall->draw();
        wall->setYPosition(1);
        wall->draw();
        wall->setYPosition(2);
        wall->draw();
    }
}

void GLWorld::updateAnimationTarget()
{
    switch(orientation)
    {
    case ORIENT_NORTH:
        player_rotY_target = 0;
        break;
    case ORIENT_WEST:
        player_rotY_target = 90;
        break;
    case ORIENT_SOUTH:
        player_rotY_target = 180;
        break;
    case ORIENT_EAST:
        player_rotY_target = 270;
        break;
    }

    if(player_rotY_from - player_rotY_target > 180)
        player_rotY_from -= 360;
    if(player_rotY_target - player_rotY_from > 180)
        player_rotY_from += 360;

    player_posX_target = steve.first;
    player_posZ_target = steve.second;
}

void GLWorld::reset()
{
    World::reset();
    updateAnimationTarget();
}
