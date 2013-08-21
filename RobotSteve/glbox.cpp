#include <qgl.h>
#include <QDebug>

#include "glbox.h"

GLBox::GLBox(float w, float h, float l, float cenX, float cenY, float cenZ, TextureAtlasEntry fr, TextureAtlasEntry ba, TextureAtlasEntry to, TextureAtlasEntry bo, TextureAtlasEntry le, TextureAtlasEntry ri)
    : posX(0), posY(0), posZ(0), rotX(0), rotY(0), rotZ(0), cenX(cenX), cenY(cenY), cenZ(cenZ)
{
    //Front
    vertices.append({0, 0, 0}); tex_coords.append({fr.left, fr.bottom}); normals.append({0, 0, 1});
    vertices.append({0, h, 0}); tex_coords.append({fr.left, fr.top}); normals.append({0, 0, 1});
    vertices.append({w, h, 0}); tex_coords.append({fr.right, fr.top}); normals.append({0, 0, 1});
    vertices.append({w, h, 0}); tex_coords.append({fr.right, fr.top}); normals.append({0, 0, 1});
    vertices.append({w, 0, 0}); tex_coords.append({fr.right, fr.bottom}); normals.append({0, 0, 1});
    vertices.append({0, 0, 0}); tex_coords.append({fr.left, fr.bottom}); normals.append({0, 0, 1});

    //Back
    vertices.append({w, 0, l}); tex_coords.append({ba.left, ba.bottom}); normals.append({0, 0, -1});
    vertices.append({w, h, l}); tex_coords.append({ba.left, ba.top}); normals.append({0, 0, -1});
    vertices.append({0, h, l}); tex_coords.append({ba.right, ba.top}); normals.append({0, 0, -1});
    vertices.append({0, h, l}); tex_coords.append({ba.right, ba.top}); normals.append({0, 0, -1});
    vertices.append({0, 0, l}); tex_coords.append({ba.right, ba.bottom}); normals.append({0, 0, -1});
    vertices.append({w, 0, l}); tex_coords.append({ba.left, ba.bottom}); normals.append({0, 0, -1});

    //Left
    vertices.append({0, 0, l}); tex_coords.append({le.left, le.bottom}); normals.append({-1, 0, 0});
    vertices.append({0, h, l}); tex_coords.append({le.left, le.top}); normals.append({-1, 0, 0});
    vertices.append({0, h, 0}); tex_coords.append({le.right, le.top}); normals.append({-1, 0, 0});
    vertices.append({0, h, 0}); tex_coords.append({le.right, le.top}); normals.append({-1, 0, 0});
    vertices.append({0, 0, 0}); tex_coords.append({le.right, le.bottom}); normals.append({-1, 0, 0});
    vertices.append({0, 0, l}); tex_coords.append({le.left, le.bottom}); normals.append({-1, 0, 0});

    //Right
    vertices.append({w, 0, 0}); tex_coords.append({ri.left, ri.bottom}); normals.append({1, 0, 0});
    vertices.append({w, h, 0}); tex_coords.append({ri.left, ri.top}); normals.append({1, 0, 0});
    vertices.append({w, h, l}); tex_coords.append({ri.right, ri.top}); normals.append({1, 0, 0});
    vertices.append({w, h, l}); tex_coords.append({ri.right, ri.top}); normals.append({1, 0, 0});
    vertices.append({w, 0, l}); tex_coords.append({ri.right, ri.bottom}); normals.append({1, 0, 0});
    vertices.append({w, 0, 0}); tex_coords.append({ri.left, ri.bottom}); normals.append({1, 0, 0});

    //Top
    vertices.append({0, h, 0}); tex_coords.append({to.left, to.bottom}); normals.append({0, 1, 0});
    vertices.append({0, h, l}); tex_coords.append({to.left, to.top}); normals.append({0, 1, 0});
    vertices.append({w, h, l}); tex_coords.append({to.right, to.top}); normals.append({0, 1, 0});
    vertices.append({w, h, l}); tex_coords.append({to.right, to.top}); normals.append({0, 1, 0});
    vertices.append({w, h, 0}); tex_coords.append({to.right, to.bottom}); normals.append({0, 1, 0});
    vertices.append({0, h, 0}); tex_coords.append({to.left, to.bottom}); normals.append({0, 1, 0});

    //Bottom
    vertices.append({w, 0, 0}); tex_coords.append({bo.left, bo.bottom}); normals.append({0, -1, 0});
    vertices.append({w, 0, l}); tex_coords.append({bo.left, bo.top}); normals.append({0, -1, 0});
    vertices.append({0, 0, l}); tex_coords.append({bo.right, bo.top}); normals.append({0, -1, 0});
    vertices.append({0, 0, l}); tex_coords.append({bo.right, bo.top}); normals.append({0, -1, 0});
    vertices.append({0, 0, 0}); tex_coords.append({bo.right, bo.bottom}); normals.append({0, -1, 0});
    vertices.append({w, 0, 0}); tex_coords.append({bo.left, bo.bottom}); normals.append({0, -1, 0});
}

void GLBox::draw()
{
    glPushMatrix();

    glTranslatef(posX + cenX, posY + cenY, posZ + cenZ);
    glRotatef(rotZ, 0, 0, 1);
    glRotatef(rotX, 1, 0, 0);
    glRotatef(rotY, 0, 1, 0);
    glTranslatef(-cenX, -cenY, -cenZ);

    glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
    glNormalPointer(GL_FLOAT, 0, normals.constData());
    glTexCoordPointer(2, GL_FLOAT, 0, tex_coords.constData());

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    for(auto i : childs)
        i->draw();

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glPopMatrix();
}
