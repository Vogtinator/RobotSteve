#ifndef GLQUAD_H
#define GLQUAD_H

#include "gldrawable.h"

class GLQuad : public GLDrawable
{
public:
    GLQuad(float w, float l, float cenX, float cenY, float cenZ, TextureAtlasEntry tex);
    void setXPosition(float x) override { posX = x; }
    void setYPosition(float y) override { posY = y; }
    void setZPosition(float z) override { posZ = z; }
    void setXRotation(float x) override { rotX = x; }
    void setYRotation(float y) override { rotY = y; }
    void setZRotation(float z) override { rotZ = z; }
    void draw() override;

private:
    float posX = 0, posY = 0, posZ = 0;
    float rotX = 0, rotY = 0, rotZ = 0;
    float cenX, cenY, cenZ; //Center of rotation

    QVector<QVector2D> tex_coords;
    QVector<QVector3D> vertices, normals;
};

#endif // GLQUAD_H
