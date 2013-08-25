/*
 * Author: Fabian Vogt
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 * or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 * Use in public and private schools for educational purposes strongly permitted!
 */

#ifndef GLWORLD_H
#define GLWORLD_H

#include <QGLWidget>
#include <QTimer>

#include "world.h"
#include "glbox.h"
#include "glquad.h"

enum ANIMATION {
    ANIM_STANDING,
    ANIM_GREET1,
    ANIM_GREET2,
    ANIM_GREET3,
    ANIM_STEP,
    ANIM_TURNLEFT,
    ANIM_TURNRIGHT,
    ANIM_BEND,
    ANIM_WAIT
};

class GLWorld : public QGLWidget, public World
{
    Q_OBJECT
public:
    explicit GLWorld(int width, int length, QWidget *parent = 0);
    ~GLWorld();

    void reset() override;
    bool stepForward() override;
    void turnRight() override;
    void turnLeft() override;
    void setMark(bool b) override;
    bool setCube(bool b) override;

protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void setAnimation(ANIMATION animation);

public slots:
    void tick();
    void setSpeed(float ms) { speed_ms = ms; }
    
private:
    void drawWallX();
    void drawWallZ();
    void updateAnimationTarget();

    QTimer tick_timer, refresh_timer;
    TextureAtlas *player_atlas, *environment_atlas;
    QPoint last_pos;
    float camera_rotX, camera_rotY, camera_dist;
    GLBox *player_body, *player_head, *player_hat, *player_leg_left, *player_leg_right, *player_arm_left, *player_arm_right;
    GLQuad *wall, *floor, *marked_floor;
    ANIMATION current_animation = ANIM_STANDING;
    QMap<ANIMATION,int> anim_ticks;
    QMap<ANIMATION,ANIMATION> anim_next;
    int current_anim_ticks;
    float speed_ms = 500; //How much time an animation takes
    float anim_progress = 0, player_posX_from = 0, player_posZ_from = 0, player_posX_target = 0, player_posZ_target = 0, player_rotY_from = 0, player_rotY_target = 0;
};

#endif // GLWORLD_H
