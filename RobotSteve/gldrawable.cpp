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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    //This line causes white textures with Qt 5, maybe a bug.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
#endif
}
