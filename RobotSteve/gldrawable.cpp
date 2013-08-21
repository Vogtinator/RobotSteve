#include "gldrawable.h"

TextureAtlas::TextureAtlas(QGLWidget &parent, QPixmap &&pixmap)
    : parent(&parent), w(pixmap.width()), h(pixmap.height())
{
    texture_id = parent.bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, QGLContext::NoBindOption);
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
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

TextureAtlas::~TextureAtlas()
{
    parent->deleteTexture(texture_id);
}
