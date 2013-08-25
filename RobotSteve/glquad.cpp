#include <QVector3D>
#include <QVector2D>

#include "glquad.h"

GLQuad::GLQuad(float w, float l, float cenX, float cenY, float cenZ, TextureAtlasEntry tex)
    : cenX(cenX), cenY(cenY), cenZ(cenZ)
{
    vertices.append({0, 0, 0}); tex_coords.append({tex.left, tex.bottom}); normals.append({0, 1, 0});
    vertices.append({0, 0, l}); tex_coords.append({tex.left, tex.top}); normals.append({0, 1, 0});
    vertices.append({w, 0, l}); tex_coords.append({tex.right, tex.top}); normals.append({0, 1, 0});
    vertices.append({w, 0, l}); tex_coords.append({tex.right, tex.top}); normals.append({0, 1, 0});
    vertices.append({w, 0, 0}); tex_coords.append({tex.right, tex.bottom}); normals.append({0, 1, 0});
    vertices.append({0, 0, 0}); tex_coords.append({tex.left, tex.bottom}); normals.append({0, 1, 0});
}

void GLQuad::draw()
{
    glPushMatrix();

    glTranslatef(posX, posY, posZ);
    glRotatef(rotY, 0, 1, 0);
    glRotatef(rotX, 1, 0, 0);
    glRotatef(rotZ, 0, 0, 1);

    glTranslatef(-cenX, -cenZ, -cenZ);

    glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
    glNormalPointer(GL_FLOAT, 0, normals.constData());
    glTexCoordPointer(2, GL_FLOAT, 0, tex_coords.constData());

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glPopMatrix();
}
