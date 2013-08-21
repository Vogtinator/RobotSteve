#ifndef GLBOX_H
#define GLBOX_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include "gldrawable.h"

class GLBox : public GLDrawable
{
public:
    GLBox(float width, float height, float length, float cenX, float cenY, float cenZ, TextureAtlasEntry fr, TextureAtlasEntry ba, TextureAtlasEntry to, TextureAtlasEntry bot, TextureAtlasEntry le, TextureAtlasEntry ri);
    void setXPosition(float x) override { posX = x; }
    void setYPosition(float y) override { posY = y; }
    void setZPosition(float z) override { posZ = z; }
    void setXRotation(float x) override { rotX = x; }
    void setYRotation(float y) override { rotY = y; }
    void setZRotation(float z) override { rotZ = z; }
    void addChild(GLDrawable* child) { childs.append(child); }
    void draw() override;

private:
    float posX, posY, posZ;
    float rotX, rotY, rotZ;
    float cenX, cenY, cenZ; //Center of rotation

    QVector<QVector2D> tex_coords;
    QVector<QVector3D> vertices, normals;
    QVector<GLDrawable*> childs;
};

#endif // GLBOX_H
