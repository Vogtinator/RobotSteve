#ifndef GLWORLD_H
#define GLWORLD_H

#include <memory>
#include <QGLWidget>
#include <QTimer>
#include <QHash>
#include <QXmlStreamReader>
#include <QGLFramebufferObject>

#include "world.h"
#include "glbox.h"
#include "glquad.h"

enum ANIMATION {
    ANIM_STANDING,
    ANIM_GREET1,
    ANIM_GREET2,
    ANIM_GREET3,
    ANIM_STEP,
    ANIM_BUMP,
    ANIM_TURN,
    ANIM_BEND,
    ANIM_DEPOSIT_FLY
};

enum SELECTION_TYPE {
    TYPE_NOTHING,
    TYPE_FLOOR,
    TYPE_BRICK
};

struct Selection {
    SELECTION_TYPE type;
    unsigned int x;
    unsigned int y;
    unsigned int h;
};

class GLWorld : public QGLWidget, public World
{
    Q_OBJECT
public:
    explicit GLWorld(unsigned int width, unsigned int length, unsigned int max_height, QWidget *parent = 0);

    void reset() override;
    bool resize(unsigned int width, unsigned int length) override;
    bool stepForward() override;
    void turnRight(int quarters) override;
    void turnLeft(int quarters) override;
    void setMark(bool b) override;
    bool setCube(bool b) override;
    bool deposit(unsigned int count) override;
    bool pickup(unsigned int count) override;
    bool setState(WorldState &state) override;
    bool loadXMLStream(QXmlStreamReader &file_reader) override;
    void setPlayerTexture(const QString &filename);
    bool isEditable() const { return editable; }

protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void setAnimation(ANIMATION animation, bool force_set = false);
    void updateFront();

public slots:
    void tick();
    void setSpeed(float ms) { speed_ms = ms; }
    void setVisible(bool visible) override;
    void setEditable(bool editable) { this->editable = editable; }
    void setMaxHeight(unsigned int max_height) override;
    void updateSelection(QPoint pos);
    void updateSelection();

signals:
    void changed();
    
private:
    void drawWallX();
    void drawWallZ();
    void updateAnimationTarget(bool force_set = false);
    void updateCamera();

    std::unique_ptr<QGLFramebufferObject> fbo; //To find out what the user clicked on
    bool fbo_dirty = true; //Whether to redraw click_image
    QImage click_image; //Color coded version of the rendered image
    bool editable = true; //Whether the user is allowed to edit the world using the context menu
    Selection current_selection{TYPE_NOTHING, 0, 0, 0};
    QTimer tick_timer, refresh_timer;
    std::unique_ptr<TextureAtlas> player_atlas, environment_atlas;
    QPoint last_pos; //Last mouse position to compute camera rotation
    float camera_rotX, camera_rotY, camera_dist, camera_calX, camera_calY, camera_calZ;
    std::shared_ptr<GLBox> player_body, player_head, player_hat, player_leg_left, player_leg_right, player_arm_left, player_arm_right, brick_top, brick_mid, cube;
    std::unique_ptr<GLQuad> wall, floor, marked_floor;
    ANIMATION current_animation = ANIM_STANDING;
    QHash<ANIMATION,int> anim_ticks;
    QHash<ANIMATION,ANIMATION> anim_next;
    int current_anim_ticks;
    float speed_ms = 500; //How much time an animation takes
    float anim_progress = 0, player_posX_from = 0, player_posZ_from = 0, player_posY_from = -0.8, player_posY_target = -0.8, player_posX_target = 0, player_posZ_target = 0, player_rotY_from = 0, player_rotY_target = 0;
    float bump_dir_x, bump_dir_z, bend_pos_y;
};

#endif // GLWORLD_H
