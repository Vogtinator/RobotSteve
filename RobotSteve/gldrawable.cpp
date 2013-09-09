#include <QGLWidget>

#include "gldrawable.h"

TextureAtlas::TextureAtlas(QGLWidget &parent, QPixmap &&pixmap)
    : parent{&parent}, pixmap{pixmap}, w{pixmap.width()}, h{pixmap.height()}
{
}

TextureAtlasEntry TextureAtlas::getArea(int x, int y, int w, int h)
{
    float UperPx = 1.0 / this->w;
    float VperPx = 1.0 / this->h;

    return {UperPx * x, VperPx * y, UperPx * (x+w), VperPx * (y+h)};
}

void TextureAtlas::bind()
{
    glBindTexture(GL_TEXTURE_2D, parent->bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, QGLContext::MipmapBindOption));
    //TODO: This line causes white textures on some computers.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
}

TextureAtlas::~TextureAtlas()
{
    parent->deleteTexture(parent->bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, QGLContext::NoBindOption));
}
