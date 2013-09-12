#include <QGLWidget>

#include "gldrawable.h"

TextureAtlas::TextureAtlas(QGLWidget &parent, QPixmap &&pixmap)
    : parent{&parent}, pixmap{pixmap}
{
}

TextureAtlasEntry TextureAtlas::getArea(int x, int y, int w, int h)
{
    float UperPx = 1.0 / pixmap.width();
    float VperPx = 1.0 / pixmap.height();

    return {UperPx * x, VperPx * y, UperPx * (x+w), VperPx * (y+h)};
}

void TextureAtlas::bind()
{
    glBindTexture(GL_TEXTURE_2D, parent->bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, QGLContext::MipmapBindOption));

#ifndef Q_OS_WIN
    //This line causes white textures on windows.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
#endif
}
