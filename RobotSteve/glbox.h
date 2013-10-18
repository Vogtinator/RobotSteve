#ifndef GLBOX_H
#define GLBOX_H

#include <memory>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include "gldrawable.h"

class GLBox : public GLDrawable
{
public:
    GLBox(float width, float height, float length, float cenX, float cenY, float cenZ, TextureAtlasEntry fr, TextureAtlasEntry ba, TextureAtlasEntry to, TextureAtlasEntry bot, TextureAtlasEntry le, TextureAtlasEntry ri, bool draw_front, bool draw_back, bool draw_top, bool draw_bottom, bool draw_left, bool draw_right);
    GLBox(float width, float height, float length, float cenX, float cenY, float cenZ, TextureAtlasEntry fr, TextureAtlasEntry ba, TextureAtlasEntry to, TextureAtlasEntry bot, TextureAtlasEntry le, TextureAtlasEntry ri)
        : GLBox(width, height, length, cenX, cenY, cenZ, fr, ba, to, bot, le, ri, true, true, true, true, true, true) {}

    void addChild(std::shared_ptr<GLDrawable> child) { childs.append(child); }
    void draw() override;

private:
    float cenX, cenY, cenZ; //Center of rotation

    QVector<QVector2D> tex_coords;
    QVector<QVector3D> vertices, normals;
    QVector<std::shared_ptr<GLDrawable> > childs;
};

#endif // GLBOX_H
