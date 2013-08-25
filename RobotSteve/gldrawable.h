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
    ~TextureAtlas();
    TextureAtlasEntry getArea(int x, int y, int w, int h);
    void bind();

private:
    QGLWidget *parent;
    QPixmap pixmap;
    int w, h;
};

class GLDrawable
{
public:
    virtual ~GLDrawable() {}

    virtual void setXPosition(float x) = 0;
    virtual void setYPosition(float y) = 0;
    virtual void setZPosition(float z) = 0;
    virtual void setXRotation(float x) = 0;
    virtual void setYRotation(float y) = 0;
    virtual void setZRotation(float z) = 0;
    virtual void draw() = 0;
};

#endif // GLDRAWABLE_H
