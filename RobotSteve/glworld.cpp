#include <cmath>
#include <iostream>
#include <QApplication>
#include <QMouseEvent>

#include "glworld.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

GLWorld::GLWorld(unsigned int width, unsigned int length, unsigned int max_height, QWidget *parent) :
    QGLWidget{parent},
    World{width, length, max_height},
    camera_rotX{-30},
    camera_rotY{15},
    camera_dist{8}
{
    anim_ticks.insert(ANIM_STANDING, 1);
    anim_ticks.insert(ANIM_STEP, 100);
    anim_ticks.insert(ANIM_BUMP, 100);
    anim_ticks.insert(ANIM_TURN, 100);
    anim_ticks.insert(ANIM_BEND, 100);
    anim_ticks.insert(ANIM_DEPOSIT_FLY, 248);
    anim_ticks.insert(ANIM_GREET1, 31);
    anim_ticks.insert(ANIM_GREET2, 200);
    anim_ticks.insert(ANIM_GREET3, 31);

    anim_next.insert(ANIM_STANDING, ANIM_STANDING);
    anim_next.insert(ANIM_STEP, ANIM_STANDING);
    anim_next.insert(ANIM_BUMP, ANIM_STANDING);
    anim_next.insert(ANIM_TURN, ANIM_STANDING);
    anim_next.insert(ANIM_BEND, ANIM_STANDING);
    anim_next.insert(ANIM_DEPOSIT_FLY, ANIM_STANDING);
    anim_next.insert(ANIM_GREET1, ANIM_GREET2);
    anim_next.insert(ANIM_GREET2, ANIM_GREET3);
    anim_next.insert(ANIM_GREET3, ANIM_STANDING);

    player_atlas = std::unique_ptr<TextureAtlas>(new TextureAtlas(*this, QPixmap(":/textures/char.png")));
    environment_atlas = std::unique_ptr<TextureAtlas>(new TextureAtlas(*this, QPixmap(":/textures/environment.png")));

    const double m_per_px = 0.0579;

    player_body = std::shared_ptr<GLBox>(new GLBox(8 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                            4 * m_per_px, 0 * m_per_px, 2 * m_per_px,
                            player_atlas->getArea(20, 20, 8, 12), player_atlas->getArea(32, 20, 8, 12),
                            player_atlas->getArea(20, 16, 8, 4), player_atlas->getArea(28, 16, 8, 4),
                            player_atlas->getArea(16, 20, 4, 12), player_atlas->getArea(28, 20, 4, 12)));

    player_body->setYRotation(player_rotY_from = 180);
    player_body->setYPosition(-1);

    //TODO: Mirror right arm + leg

    player_arm_left = std::shared_ptr<GLBox>(new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                2 * m_per_px, 10 * m_per_px, 2 * m_per_px,
                                player_atlas->getArea(44, 20, 4, 12), player_atlas->getArea(52, 20, 4, 12),
                                player_atlas->getArea(44, 16, 4, 4), player_atlas->getArea(48, 16, 4, 4),
                                player_atlas->getArea(48, 20, 4, 12), player_atlas->getArea(40, 20, 4, 12)));

    player_arm_left->setXPosition(-6 * m_per_px);
    player_arm_left->setYPosition(10 * m_per_px);
    player_arm_left->setZRotation(-5);
    player_body->addChild(player_arm_left);

    player_arm_right = std::shared_ptr<GLBox>(new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                 2 * m_per_px, 10 * m_per_px, 2 * m_per_px,
                                 player_atlas->getArea(44, 20, 4, 12), player_atlas->getArea(52, 20, 4, 12),
                                 player_atlas->getArea(44, 16, 4, 4), player_atlas->getArea(48, 16, 4, 4),
                                 player_atlas->getArea(40, 20, 4, 12), player_atlas->getArea(48, 20, 4, 12)));

    player_arm_right->setXPosition(6 * m_per_px);
    player_arm_right->setYPosition(10 * m_per_px);
    player_arm_right->setZRotation(5);
    player_body->addChild(player_arm_right);

    player_leg_left = std::shared_ptr<GLBox>(new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                 2 * m_per_px, 12 * m_per_px, 2 * m_per_px,
                                 player_atlas->getArea(4, 20, 4, 12), player_atlas->getArea(12, 20, 4, 12),
                                 player_atlas->getArea(4, 16, 4, 4), player_atlas->getArea(8, 16, 4, 4),
                                 player_atlas->getArea(8, 20, 4, 12), player_atlas->getArea(0, 20, 4, 12)));

    player_leg_left->setXPosition(-2 * m_per_px);
    player_leg_left->setZRotation(-5);
    player_body->addChild(player_leg_left);

    player_leg_right = std::shared_ptr<GLBox>(new GLBox(4 * m_per_px, 12 * m_per_px, 4 * m_per_px,
                                 2 * m_per_px, 12 * m_per_px, 2 * m_per_px,
                                 player_atlas->getArea(4, 20, 4, 12), player_atlas->getArea(12, 20, 4, 12),
                                 player_atlas->getArea(4, 16, 4, 4), player_atlas->getArea(8, 16, 4, 4),
                                 player_atlas->getArea(0, 20, 4, 12), player_atlas->getArea(8, 20, 4, 12)));

    player_leg_right->setXPosition(2 * m_per_px);
    player_leg_right->setZRotation(5);
    player_body->addChild(player_leg_right);

    player_head = std::shared_ptr<GLBox>(new GLBox(8 * m_per_px, 8 * m_per_px, 8 * m_per_px,
                            4 * m_per_px, 0, 4 * m_per_px,
                            player_atlas->getArea(8, 8, 8, 8), player_atlas->getArea(24, 8, 8, 8),
                            player_atlas->getArea(8, 0, 8, 8), player_atlas->getArea(16, 0, 8, 8),
                            player_atlas->getArea(0, 8, 8, 8), player_atlas->getArea(16, 8, 8, 8)));

    player_head->setYPosition(12 * m_per_px);
    player_body->addChild(player_head);

    player_hat = std::shared_ptr<GLBox>(new GLBox(9 * m_per_px, 9 * m_per_px, 9 * m_per_px,
                           4.5 * m_per_px, 0.5 * m_per_px, 4.5 * m_per_px,
                           player_atlas->getArea(40, 8, 8, 8), player_atlas->getArea(56, 8, 8, 8),
                           player_atlas->getArea(40, 0, 8, 8), player_atlas->getArea(48, 0, 8, 8),
                           player_atlas->getArea(32, 8, 8, 8), player_atlas->getArea(48, 8, 8, 8)));

    player_head->addChild(player_hat);

    brick = std::shared_ptr<GLBox>(new GLBox(0.5, 0.5, 0.5,
                                             0.25, 0, 0.25,
                                             environment_atlas->getArea(48, 0, 8, 8), environment_atlas->getArea(48, 0, 8, 8),
                                             environment_atlas->getArea(48, 0, 8, 8), environment_atlas->getArea(48, 0, 8, 8),
                                             environment_atlas->getArea(48, 0, 8, 8), environment_atlas->getArea(48, 0, 8, 8)));

    cube = std::shared_ptr<GLBox>(new GLBox(1, 1, 1,
                                            0.5, 0, 0.5,
                                            environment_atlas->getArea(16, 0, 16, 16), environment_atlas->getArea(16, 0, 16, 16),
                                            environment_atlas->getArea(16, 0, 16, 16), environment_atlas->getArea(16, 0, 16, 16),
                                            environment_atlas->getArea(16, 0, 16, 16), environment_atlas->getArea(16, 0, 16, 16)));

    cube->setYPosition(-1.5); //Ground level

    wall = std::unique_ptr<GLQuad>(new GLQuad(1, 1, 0.5, 0.5, 0.5, environment_atlas->getArea(16, 0, 16, 16)));

    wall->setXRotation(90);

    floor = std::unique_ptr<GLQuad>(new GLQuad(1, 1, 0.5, 0.5, 0.5, environment_atlas->getArea(0, 0, 16, 16)));

    floor->setYPosition(-1);

    marked_floor = std::unique_ptr<GLQuad>(new GLQuad(1, 1, 0.5, 0.5, 0.5, environment_atlas->getArea(32, 0, 16, 16)));

    marked_floor->setYPosition(-1);

    connect(&tick_timer, SIGNAL(timeout()), this, SLOT(tick()));
    connect(&refresh_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    refresh_timer.start(1000/30);

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

    double camera_x = camera_dist*(sin(radY) * cosradX) + World::size.first/2;
    double camera_y = camera_dist*sin(radX-M_PI);
    double camera_z = camera_dist*(cos(radY) * cosradX) + World::size.second/2;

    glTranslated(-camera_x, -camera_y, -camera_z);

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

    for(unsigned int x = 0; x < World::size.first; x++)
        for(unsigned int z = 0; z < World::size.second; z++)
        {
            WorldObject &obj = map[x][z];
            if(obj.has_mark)
            {
                marked_floor->setXPosition(x);
                marked_floor->setZPosition(z);
                marked_floor->draw();
            }
            else if(obj.has_cube)
            {
                cube->setXPosition(x);
                cube->setZPosition(z);
                cube->draw();
            }
            else
            {
                floor->setXPosition(x);
                floor->setZPosition(z);
                floor->draw();
            }

            brick->setXPosition(x);
            brick->setZPosition(z);

            float brick_y = 0;
            for(unsigned int height = 0; height < World::map[x][z].stack_size; height++, brick_y += 0.5)
            {
                brick->setYPosition(brick_y - 1.5);
                brick->draw();
            }
        }

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

    //Transparency effect
    qglClearColor(qApp->palette().color(QPalette::Window));
}

void GLWorld::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double height = (static_cast<double>(h)/w);
    glFrustum(-1, 1, -height, height, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void GLWorld::setPlayerTexture(const QString &filename)
{
    player_atlas = std::unique_ptr<TextureAtlas>(new TextureAtlas(*this, QPixmap(filename)));
}

inline float lin_terpolation(float from, float to, float x)
{
    return from + (to-from)*x;
}

void GLWorld::tick()
{
    switch(current_animation)
    {
    case ANIM_STANDING:
        break;
    case ANIM_BUMP:
    {
        float triangle_progress = this->anim_progress > 0.5 ? 0.5 - (this->anim_progress - 0.5) : this->anim_progress;
        player_body->setXPosition(lin_terpolation(player_posX_from, player_posX_from+bump_dir_x/2, triangle_progress));
        player_body->setZPosition(lin_terpolation(player_posZ_from, player_posZ_from+bump_dir_z/2, triangle_progress));
    }
    case ANIM_TURN:
    case ANIM_STEP:
        player_arm_left->setXRotation(sin(anim_progress * 2*M_PI) * 45.0);
        player_arm_right->setXRotation(sin(anim_progress * 2*M_PI) * -45.0);
        player_leg_left->setXRotation(sin(anim_progress * 2*M_PI) * -45.0);
        player_leg_right->setXRotation(sin(anim_progress * 2*M_PI) * 45.0);
        break;
    case ANIM_GREET1:
        player_arm_right->setXRotation(sin(anim_progress * M_PI/2) * 180.0);
        break;
    case ANIM_GREET2:
        player_arm_right->setZRotation(sin(anim_progress * M_PI*2) * 15.0 + 5.0);
        break;
    case ANIM_GREET3:
        player_arm_right->setXRotation(cos(anim_progress * M_PI/2) * 180.0);
        break;
    case ANIM_BEND:
        player_body->setXRotation(sin(anim_progress * M_PI) * -50);
        player_arm_left->setXRotation(sin(anim_progress * M_PI) * 90);
        player_arm_right->setXRotation(sin(anim_progress * M_PI) * 90);
        player_leg_left->setXRotation(sin(anim_progress * M_PI) * 50);
        player_leg_right->setXRotation(sin(anim_progress * M_PI) * 50);
        break;

    case ANIM_DEPOSIT_FLY:
        player_arm_left->setXRotation(sin(anim_progress * M_PI) * 90);
        player_arm_right->setXRotation(sin(anim_progress * M_PI) * 90);
        player_body->setYPosition(lin_terpolation(player_posY_from, bend_pos_y, sin(anim_progress * M_PI)));
        break;
    }

    if(player_rotY_target != player_rotY_from)
        player_body->setYRotation(lin_terpolation(player_rotY_from, player_rotY_target, sin(anim_progress * M_PI/2)));

    if(player_posX_target != player_posX_from || player_posY_target != player_posY_from || player_posZ_target != player_posZ_from)
    {
        //Supersteve!
        if(player_posY_target != player_posY_from)
            player_arm_right->setXRotation(sin(anim_progress * M_PI) * 150);

        player_body->setXPosition(lin_terpolation(player_posX_from, player_posX_target, sin(anim_progress * M_PI/2)));
        player_body->setYPosition(lin_terpolation(player_posY_from, player_posY_target, sin(anim_progress * M_PI/2)));
        player_body->setZPosition(lin_terpolation(player_posZ_from, player_posZ_target, sin(anim_progress * M_PI/2)));
    }

    current_anim_ticks++;
    anim_progress = (static_cast<float>(current_anim_ticks)) / static_cast<float>(anim_ticks[current_animation]);

    if(current_anim_ticks == anim_ticks[current_animation] + 1)
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

    tick_timer.stop();

    player_posX_from = player_posX_target;
    player_posY_from = player_posY_target;
    player_posZ_from = player_posZ_target;
    player_rotY_from = player_rotY_target;

    //If previous animation didn't complete, skip it
    if(anim_progress < 1.0f)
    {
        player_body->setXPosition(player_posX_from);
        player_body->setYPosition(player_posY_from);
        player_body->setZPosition(player_posZ_from);
        player_body->setYRotation(player_rotY_from);

        //For ANIM_GREET
        player_arm_right->setXRotation(0);
        player_arm_right->setZRotation(5);

        //For ANIM_BEND
        player_body->setXRotation(0);
        player_leg_left->setXRotation(0);
        player_leg_right->setXRotation(0);

        //For ANIM_BEND and ANIM_DEPOSIT
        player_arm_left->setXRotation(0);
    }

    current_anim_ticks = 0;
    current_animation = animation;

    //If no time for animation, don't do it at all
    if(speed_ms > 0)
        tick_timer.start(speed_ms / anim_ticks[current_animation]);

    anim_progress = 0;
}

bool GLWorld::stepForward()
{
    if(!World::stepForward())
    {
        SignedCoords wall = getForward();
        bump_dir_x = wall.first;
        bump_dir_z = wall.second;
        setAnimation(ANIM_BUMP);
        return false;
    }

    setAnimation(ANIM_STEP);
    updateAnimationTarget();

    return true;
}

void GLWorld::turnRight(int quarters)
{
    World::turnRight(quarters);
    setAnimation(ANIM_TURN);
    updateAnimationTarget();
}

void GLWorld::turnLeft(int quarters)
{
    World::turnLeft(quarters);
    setAnimation(ANIM_TURN);
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

    return true;
}

bool GLWorld::deposit(unsigned int count)
{
    if(!World::deposit(count))
        return false;

    unsigned int here = map[steve.first][steve.second].stack_size;
    bend_pos_y = static_cast<float>(getStackSize()) * 0.5 - 2.0;

    int diff = getStackSize();
    diff -= here;

    if(diff > 3)
        setAnimation(ANIM_DEPOSIT_FLY);
    else
        setAnimation(ANIM_BEND);

    return true;
}

bool GLWorld::pickup(unsigned int count)
{
    if(!World::pickup(count))
        return false;

    unsigned int here = map[steve.first][steve.second].stack_size;
    bend_pos_y = static_cast<float>(getStackSize()) * 0.5 - 2.0;

    int diff = getStackSize();
    diff -= here;

    if(diff > 3)
        setAnimation(ANIM_DEPOSIT_FLY);
    else
        setAnimation(ANIM_BEND);

    return true;
}

void GLWorld::drawWallX()
{
    for(unsigned int x = 0; x < World::size.first; x++)
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
    for(unsigned int y = 0; y < World::size.second; y++)
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

    //Closest direction (270° to 360° instad of 270° to 0°)
    if(player_rotY_from - player_rotY_target > 180)
        player_rotY_from -= 360;
    if(player_rotY_target - player_rotY_from > 180)
        player_rotY_from += 360;

    player_posX_target = steve.first;
    player_posZ_target = steve.second;
    player_posY_target = map[steve.first][steve.second].stack_size * 0.5 - 0.8;

    //tick not called
    if(speed_ms == 0)
        setAnimation(ANIM_STANDING); //Update state
}

void GLWorld::reset()
{
    World::reset();
    updateAnimationTarget();
    setAnimation(ANIM_STANDING);
}

bool GLWorld::setState(WorldState &state)
{
    if(!World::setState(state))
        return false;

    updateAnimationTarget();
    setAnimation(ANIM_STANDING);

    return true;
}

bool GLWorld::loadXMLStream(QXmlStreamReader &file_reader)
{
    if(!World::loadXMLStream(file_reader))
        return false;

    updateAnimationTarget();
    setAnimation(ANIM_STANDING);

    return true;
}
