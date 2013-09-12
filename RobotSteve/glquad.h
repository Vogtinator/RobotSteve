#ifndef GLQUAD_H
#define GLQUAD_H

#include "gldrawable.h"

class GLQuad : public GLDrawable
{
public:
    GLQuad(float w, float l, float cenX, float cenY, float cenZ, TextureAtlasEntry tex);
    void draw() override;

private:
    float cenX, cenY, cenZ; //Center of rotation

    QVector<QVector2D> tex_coords;
    QVector<QVector3D> vertices, normals;
};

#endif // GLQUAD_H
