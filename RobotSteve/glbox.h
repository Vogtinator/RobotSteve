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
    GLBox(float width, float height, float length, float cenX, float cenY, float cenZ, TextureAtlasEntry fr, TextureAtlasEntry ba, TextureAtlasEntry to, TextureAtlasEntry bot, TextureAtlasEntry le, TextureAtlasEntry ri);
    void addChild(std::shared_ptr<GLDrawable> child) { childs.append(child); }
    void draw() override;

private:
    float cenX, cenY, cenZ; //Center of rotation

    QVector<QVector2D> tex_coords;
    QVector<QVector3D> vertices, normals;
    QVector<std::shared_ptr<GLDrawable> > childs;
};

#endif // GLBOX_H
