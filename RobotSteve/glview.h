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

#include <QGLWidget>
#include <QTimer>

#include "world.h"
#include "glbox.h"

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

class GLView : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLView(World *world, QWidget *parent = 0);
    ~GLView();

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
    
private:
    QTimer tick_timer, refresh_timer;
    World *world;
    TextureAtlas *atlas;
    QPoint last_pos;
    float camera_rotX, camera_rotY, camera_dist;
    GLBox *player_body, *player_head, *player_hat, *player_leg_left, *player_leg_right, *player_arm_left, *player_arm_right;
    ANIMATION current_animation = ANIM_STANDING;
    QMap<ANIMATION,int> anim_ticks;
    QMap<ANIMATION,ANIMATION> anim_next;
    int current_anim_ticks;

    float anim_float1;
};

#endif // GLVIEW_H
