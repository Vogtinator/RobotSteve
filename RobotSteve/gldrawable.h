#ifndef GLDRAWABLE_H
#define GLDRAWABLE_H

#include <QGLWidget>

struct TextureAtlasEntry {
        float left;
        float top;
        float right;
        float bottom;
};

class TextureAtlas {
public:
    TextureAtlas(QGLWidget &parent, QPixmap &&pixmap);
    TextureAtlasEntry getArea(int x, int y, int w, int h);
    void bind();

private:
    QGLWidget *parent;
    QPixmap pixmap;
};

class GLDrawable
{
public:
    virtual ~GLDrawable() {}

    void setXPosition(float x) { posX = x; }
    float getXPosition() { return posX; }
    void setYPosition(float y) { posY = y; }
    float getYPosition() { return posY; }
    void setZPosition(float z) { posZ = z; }
    float getZPosition() { return posZ; }
    void setXRotation(float x) { rotX = x; }
    float getXRotation() { return rotX; }
    void setYRotation(float y) { rotY = y; }
    float getYRotation() { return rotY; }
    void setZRotation(float z) { rotZ = z; }
    float getZRotation() { return rotZ; }

    virtual void draw() = 0;

protected:
    float posX = 0, posY = 0, posZ = 0;
    float rotX = 0, rotY = 0, rotZ = 0;
};

#endif // GLDRAWABLE_H
