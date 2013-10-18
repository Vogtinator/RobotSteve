#include <qgl.h>

#include "glbox.h"

GLBox::GLBox(float w, float h, float l, float cenX, float cenY, float cenZ,
             TextureAtlasEntry fr, TextureAtlasEntry ba, TextureAtlasEntry to, TextureAtlasEntry bo, TextureAtlasEntry le, TextureAtlasEntry ri,
             bool draw_front, bool draw_back, bool draw_top, bool draw_bottom, bool draw_left, bool draw_right)
    : cenX{cenX}, cenY{cenY}, cenZ{cenZ}
{
    if(draw_front)
    {
        vertices.append({0, 0, 0}); tex_coords.append({fr.left, fr.bottom}); normals.append({0, 0, 1});
        vertices.append({0, h, 0}); tex_coords.append({fr.left, fr.top}); normals.append({0, 0, 1});
        vertices.append({w, h, 0}); tex_coords.append({fr.right, fr.top}); normals.append({0, 0, 1});
        vertices.append({w, h, 0}); tex_coords.append({fr.right, fr.top}); normals.append({0, 0, 1});
        vertices.append({w, 0, 0}); tex_coords.append({fr.right, fr.bottom}); normals.append({0, 0, 1});
        vertices.append({0, 0, 0}); tex_coords.append({fr.left, fr.bottom}); normals.append({0, 0, 1});
    }

    if(draw_back)
    {
        vertices.append({w, 0, l}); tex_coords.append({ba.left, ba.bottom}); normals.append({0, 0, -1});
        vertices.append({w, h, l}); tex_coords.append({ba.left, ba.top}); normals.append({0, 0, -1});
        vertices.append({0, h, l}); tex_coords.append({ba.right, ba.top}); normals.append({0, 0, -1});
        vertices.append({0, h, l}); tex_coords.append({ba.right, ba.top}); normals.append({0, 0, -1});
        vertices.append({0, 0, l}); tex_coords.append({ba.right, ba.bottom}); normals.append({0, 0, -1});
        vertices.append({w, 0, l}); tex_coords.append({ba.left, ba.bottom}); normals.append({0, 0, -1});
    }

    if(draw_left)
    {
        vertices.append({0, 0, l}); tex_coords.append({le.left, le.bottom}); normals.append({-1, 0, 0});
        vertices.append({0, h, l}); tex_coords.append({le.left, le.top}); normals.append({-1, 0, 0});
        vertices.append({0, h, 0}); tex_coords.append({le.right, le.top}); normals.append({-1, 0, 0});
        vertices.append({0, h, 0}); tex_coords.append({le.right, le.top}); normals.append({-1, 0, 0});
        vertices.append({0, 0, 0}); tex_coords.append({le.right, le.bottom}); normals.append({-1, 0, 0});
        vertices.append({0, 0, l}); tex_coords.append({le.left, le.bottom}); normals.append({-1, 0, 0});
    }

    if(draw_right)
    {
        vertices.append({w, 0, 0}); tex_coords.append({ri.left, ri.bottom}); normals.append({1, 0, 0});
        vertices.append({w, h, 0}); tex_coords.append({ri.left, ri.top}); normals.append({1, 0, 0});
        vertices.append({w, h, l}); tex_coords.append({ri.right, ri.top}); normals.append({1, 0, 0});
        vertices.append({w, h, l}); tex_coords.append({ri.right, ri.top}); normals.append({1, 0, 0});
        vertices.append({w, 0, l}); tex_coords.append({ri.right, ri.bottom}); normals.append({1, 0, 0});
        vertices.append({w, 0, 0}); tex_coords.append({ri.left, ri.bottom}); normals.append({1, 0, 0});
    }

    if(draw_top)
    {
        vertices.append({0, h, 0}); tex_coords.append({to.left, to.bottom}); normals.append({0, 1, 0});
        vertices.append({0, h, l}); tex_coords.append({to.left, to.top}); normals.append({0, 1, 0});
        vertices.append({w, h, l}); tex_coords.append({to.right, to.top}); normals.append({0, 1, 0});
        vertices.append({w, h, l}); tex_coords.append({to.right, to.top}); normals.append({0, 1, 0});
        vertices.append({w, h, 0}); tex_coords.append({to.right, to.bottom}); normals.append({0, 1, 0});
        vertices.append({0, h, 0}); tex_coords.append({to.left, to.bottom}); normals.append({0, 1, 0});
    }

    if(draw_bottom)
    {
        vertices.append({w, 0, 0}); tex_coords.append({bo.left, bo.bottom}); normals.append({0, -1, 0});
        vertices.append({w, 0, l}); tex_coords.append({bo.left, bo.top}); normals.append({0, -1, 0});
        vertices.append({0, 0, l}); tex_coords.append({bo.right, bo.top}); normals.append({0, -1, 0});
        vertices.append({0, 0, l}); tex_coords.append({bo.right, bo.top}); normals.append({0, -1, 0});
        vertices.append({0, 0, 0}); tex_coords.append({bo.right, bo.bottom}); normals.append({0, -1, 0});
        vertices.append({w, 0, 0}); tex_coords.append({bo.left, bo.bottom}); normals.append({0, -1, 0});
    }
}

void GLBox::draw()
{
    glPushMatrix();

    glTranslatef(posX, posY, posZ);
    glRotatef(rotY, 0, 1, 0);
    glRotatef(rotX, 1, 0, 0);
    glRotatef(rotZ, 0, 0, 1);
    glTranslatef(-cenX, -cenY, -cenZ);

    glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
    glNormalPointer(GL_FLOAT, 0, normals.constData());
    glTexCoordPointer(2, GL_FLOAT, 0, tex_coords.constData());

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    //Childs aligned on cen{X,Y,Z}
    glTranslatef(cenX, cenY, cenZ);

    for(auto i : childs)
        i->draw();

    glPopMatrix();
}
