#include "gldrawable.h"
#include <QDebug>

TextureAtlas::TextureAtlas(QGLWidget &parent, QPixmap &&pixmap)
    : parent(&parent), pixmap(pixmap), w(pixmap.width()), h(pixmap.height())
{
}

TextureAtlasEntry TextureAtlas::getArea(int x, int y, int w, int h)
{
    float UperPx = 1.0 / this->w;
    float VperPx = 1.0 / this->h;
    static_assert(std::is_same<decltype(1.0 / this->h), double>::value, "Your compiler is bullshit.");
    static_assert(std::is_same<decltype(UperPx * x), float>::value, "Your compiler is bullshit #2.");

    return {UperPx * x, VperPx * y, UperPx * (x+w), VperPx * (y+h)};
}

void TextureAtlas::bind()
{
    glBindTexture(GL_TEXTURE_2D, parent->bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, QGLContext::NoBindOption));
}

TextureAtlas::~TextureAtlas()
{
    parent->deleteTexture(parent->bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, QGLContext::NoBindOption));
}
