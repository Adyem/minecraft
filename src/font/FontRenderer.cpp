#include "../../src/font/FontRenderer.hpp"

#if defined(FT_VOX_HAVE_FREETYPE)
# include <ft2build.h>
# include FT_FREETYPE_H
#endif

struct BitmapGlyph
{
    char character;
    const char *rows[7];
};

// clang-format off
static const BitmapGlyph GLYPHS[] = {
    {'A', {" ### ", "#   #", "#   #", "#####", "#   #", "#   #", "#   #"}},
    {'B', {"#### ", "#   #", "#   #", "#### ", "#   #", "#   #", "#### "}},
    {'C', {" ####", "#    ", "#    ", "#    ", "#    ", "#    ", " ####"}},
    {'D', {"#### ", "#   #", "#   #", "#   #", "#   #", "#   #", "#### "}},
    {'E', {"#####", "#    ", "#    ", "#####", "#    ", "#    ", "#####"}},
    {'F', {"#####", "#    ", "#    ", "#####", "#    ", "#    ", "#    "}},
    {'G', {" ####", "#    ", "#    ", "# ###", "#   #", "#   #", " ####"}},
    {'H', {"#   #", "#   #", "#   #", "#####", "#   #", "#   #", "#   #"}},
    {'I', {"#####", "  #  ", "  #  ", "  #  ", "  #  ", "  #  ", "#####"}},
    {'J', {"#####", "   # ", "   # ", "   # ", "#  # ", "#  # ", " ##  "}},
    {'K', {"#   #", "#  # ", "# #  ", "##   ", "# #  ", "#  # ", "#   #"}},
    {'L', {"#    ", "#    ", "#    ", "#    ", "#    ", "#    ", "#####"}},
    {'M', {"#   #", "## ##", "# # #", "#   #", "#   #", "#   #", "#   #"}},
    {'N', {"#   #", "##  #", "# # #", "#  ##", "#   #", "#   #", "#   #"}},
    {'O', {" ### ", "#   #", "#   #", "#   #", "#   #", "#   #", " ### "}},
    {'P', {"#### ", "#   #", "#   #", "#### ", "#    ", "#    ", "#    "}},
    {'Q', {" ### ", "#   #", "#   #", "#   #", "# # #", "#  # ", " ## #"}},
    {'R', {"#### ", "#   #", "#   #", "#### ", "# #  ", "#  # ", "#   #"}},
    {'S', {" ####", "#    ", "#    ", " ### ", "    #", "    #", "#### "}},
    {'T', {"#####", "  #  ", "  #  ", "  #  ", "  #  ", "  #  ", "  #  "}},
    {'U', {"#   #", "#   #", "#   #", "#   #", "#   #", "#   #", " ### "}},
    {'V', {"#   #", "#   #", "#   #", "#   #", " # # ", " # # ", "  #  "}},
    {'W', {"#   #", "#   #", "#   #", "# # #", "# # #", "## ##", "#   #"}},
    {'X', {"#   #", " # # ", "  #  ", "  #  ", "  #  ", " # # ", "#   #"}},
    {'Y', {"#   #", " # # ", "  #  ", "  #  ", "  #  ", "  #  ", "  #  "}},
    {'Z', {"#####", "    #", "   # ", "  #  ", " #   ", "#    ", "#####"}},
    {'0', {" ### ", "#   #", "#  ##", "# # #", "##  #", "#   #", " ### "}},
    {'1', {"  #  ", " ##  ", "  #  ", "  #  ", "  #  ", "  #  ", "#####"}},
    {'2', {" ### ", "#   #", "    #", "   # ", "  #  ", " #   ", "#####"}},
    {'3', {"#####", "    #", "   # ", "  ## ", "    #", "#   #", " ### "}},
    {'4', {"#   #", "#   #", "#   #", "#####", "    #", "    #", "    #"}},
    {'5', {"#####", "#    ", "#    ", "#### ", "    #", "    #", "#### "}},
    {'6', {" ### ", "#    ", "#    ", "#### ", "#   #", "#   #", " ### "}},
    {'7', {"#####", "    #", "   # ", "  #  ", " #   ", " #   ", " #   "}},
    {'8', {" ### ", "#   #", "#   #", " ### ", "#   #", "#   #", " ### "}},
    {'9', {" ### ", "#   #", "#   #", " ####", "    #", "    #", " ### "}},
    {' ', {"     ", "     ", "     ", "     ", "     ", "     ", "     "}},
    {'_', {"     ", "     ", "     ", "     ", "     ", "     ", "#####"}},
    {'-', {"     ", "     ", "     ", "#####", "     ", "     ", "     "}},
    {'.', {"     ", "     ", "     ", "     ", "     ", "  ## ", "  ## "}},
    {':', {"     ", "  ## ", "  ## ", "     ", "  ## ", "  ## ", "     "}},
    {'/', {"    #", "   # ", "   # ", "  #  ", " #   ", " #   ", "#    "}},
    {'%', {"##  #", "## # ", "  #  ", " # ##", "#  ##", "     ", "     "}}
};
// clang-format on

static const BitmapGlyph *find_glyph(char character)
{
    size_t index;

    if (character >= 'a' && character <= 'z')
        character = static_cast<char>(character - 'a' + 'A');
    index = 0U;
    while (index < sizeof(GLYPHS) / sizeof(GLYPHS[0]))
    {
        if (GLYPHS[index].character == character)
            return (&GLYPHS[index]);
        ++index;
    }
    return (ft_nullptr);
}

static void draw_glyph(uint32_t *pixels, int buf_w, int buf_h, int origin_x, int origin_y,
    const BitmapGlyph *glyph, int scale, uint32_t colour)
{
    int row_index;
    int col_index;
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    if (glyph == ft_nullptr || pixels == ft_nullptr || scale <= 0)
        return ;
    red = static_cast<uint8_t>((colour >> 16) & 0xFFU);
    green = static_cast<uint8_t>((colour >> 8) & 0xFFU);
    blue = static_cast<uint8_t>(colour & 0xFFU);
    row_index = 0;
    while (row_index < 7)
    {
        col_index = 0;
        while (col_index < 5)
        {
            if (glyph->rows[row_index][col_index] != ' ')
            {
                int sy = 0;

                while (sy < scale)
                {
                    int sx = 0;

                    while (sx < scale)
                    {
                        int px = origin_x + col_index * scale + sx;
                        int py = origin_y + row_index * scale + sy;

                        if (px >= 0 && px < buf_w && py >= 0 && py < buf_h)
                        {
                            pixels[py * buf_w + px] = (static_cast<uint32_t>(red) << 16)
                                | (static_cast<uint32_t>(green) << 8)
                                | static_cast<uint32_t>(blue);
                        }
                        ++sx;
                    }
                    ++sy;
                }
            }
            ++col_index;
        }
        ++row_index;
    }
}

FontRenderer::FontRenderer()
    : impl_(ft_nullptr), pixel_height_(0.0f), loaded_(false), use_real_font_(false)
{
    return ;
}

FontRenderer::FontRenderer(const FontRenderer &other)
    : impl_(ft_nullptr), pixel_height_(0.0f), loaded_(false), use_real_font_(false)
{
    (void)other;
    return ;
}

#if defined(FT_VOX_HAVE_FREETYPE)
struct FontRendererImpl
{
    FT_Library library;
    FT_Face face;
};
#endif

const char *FontRenderer::COMIC_CANDIDATES[] = {
    "C:\\Windows\\Fonts\\comic.ttf",
    "C:\\Windows\\Fonts\\Comic Sans MS.ttf",
    "/System/Library/Fonts/Supplemental/Comic Sans MS.ttf",
    "/Library/Fonts/Comic Sans MS.ttf",
    "/usr/share/fonts/truetype/comic-neue/ComicNeue-Regular.ttf",
    "/usr/share/fonts/truetype/comicneue/ComicNeue-Regular.ttf",
    "/usr/share/fonts/comic-neue/ComicNeue-Regular.ttf",
    ft_nullptr,
};

const char *FontRenderer::find_comic_sans()
{
    size_t index;

    index = 0U;
    while (COMIC_CANDIDATES[index] != ft_nullptr)
    {
        FILE *file;

        file = std::fopen(COMIC_CANDIDATES[index], "rb");
        if (file != ft_nullptr)
        {
            std::fclose(file);
            return (COMIC_CANDIDATES[index]);
        }
        ++index;
    }
    return (ft_nullptr);
}

FontRenderer::~FontRenderer()
{
    destroy();
    return ;
}

FontRenderer &FontRenderer::operator=(const FontRenderer &other)
{
    (void)other;
    return (*this);
}

#if defined(FT_VOX_HAVE_FREETYPE)
bool FontRenderer::load_real_font()
{
    const char *font_path;
    FontRendererImpl *impl;

    font_path = find_comic_sans();
    if (font_path == ft_nullptr)
        return (false);
    impl = new (std::nothrow) FontRendererImpl();
    if (impl == ft_nullptr)
        return (false);
    if (FT_Init_FreeType(&impl->library) != 0)
    {
        delete impl;
        return (false);
    }
    if (FT_New_Face(impl->library, font_path, 0, &impl->face) != 0)
    {
        (void)FT_Done_FreeType(impl->library);
        delete impl;
        return (false);
    }
    impl_ = impl;
    return (true);
}
#else
bool FontRenderer::load_real_font()
{
    return (false);
}
#endif

bool FontRenderer::load(float pixel_height)
{
    destroy();
    pixel_height_ = pixel_height;
    loaded_ = true;
    use_real_font_ = load_real_font();
    return (true);
}

void FontRenderer::destroy()
{
#if defined(FT_VOX_HAVE_FREETYPE)
    if (use_real_font_ && impl_ != ft_nullptr)
    {
        FontRendererImpl *impl;

        impl = static_cast<FontRendererImpl *>(impl_);
        (void)FT_Done_Face(impl->face);
        (void)FT_Done_FreeType(impl->library);
        delete impl;
    }
#endif
    impl_ = ft_nullptr;
    use_real_font_ = false;
    loaded_ = false;
    return ;
}

bool FontRenderer::is_loaded() const
{
    return (loaded_);
}

bool FontRenderer::is_using_real_font() const
{
    return (use_real_font_);
}

float FontRenderer::effective_pixel_height(float pixel_height_override) const
{
    if (pixel_height_override > 0.0f)
        return (pixel_height_override);
    return (pixel_height_);
}

int FontRenderer::line_height(float pixel_height_override) const
{
    if (!loaded_)
        return (0);
    return (static_cast<int>(std::ceil(static_cast<double>(
        effective_pixel_height(pixel_height_override)))));
}

#if defined(FT_VOX_HAVE_FREETYPE)
int FontRenderer::text_width_real_font(const char *text, float pixel_height) const
{
    FontRendererImpl *impl;
    int width;
    size_t index;

    impl = static_cast<FontRendererImpl *>(impl_);
    (void)FT_Set_Pixel_Sizes(impl->face, 0,
        static_cast<FT_UInt>(std::lround(static_cast<double>(pixel_height))));
    width = 0;
    index = 0U;
    while (text[index] != '\0')
    {
        FT_ULong codepoint;

        codepoint = static_cast<FT_ULong>(static_cast<unsigned char>(text[index]));
        if (FT_Load_Char(impl->face, codepoint, FT_LOAD_DEFAULT) == 0)
            width += static_cast<int>(impl->face->glyph->advance.x >> 6);
        ++index;
    }
    return (width);
}
#else
int FontRenderer::text_width_real_font(const char *text, float pixel_height) const
{
    (void)text;
    (void)pixel_height;
    return (0);
}
#endif

int FontRenderer::text_width(const char *text, float pixel_height_override) const
{
    float height;
    int glyph_scale;

    if (!loaded_ || text == ft_nullptr)
        return (0);
    height = effective_pixel_height(pixel_height_override);
    if (use_real_font_)
        return (text_width_real_font(text, height));
    glyph_scale = std::max(1, static_cast<int>(std::lround(static_cast<double>(height) / 7.0)));
    return (static_cast<int>(std::strlen(text)) * glyph_scale * 6);
}

void FontRenderer::blend_glyph(uint32_t *pixels, int buf_w, int buf_h, int draw_x, int draw_y,
    uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) const
{
    uint32_t existing;
    uint32_t bg_r;
    uint32_t bg_g;
    uint32_t bg_b;
    uint32_t a;
    uint32_t out_r;
    uint32_t out_g;
    uint32_t out_b;

    if (pixels == ft_nullptr || draw_x < 0 || draw_x >= buf_w || draw_y < 0 || draw_y >= buf_h)
        return ;
    existing = pixels[draw_y * buf_w + draw_x];
    bg_r = (existing >> 16) & 0xFFU;
    bg_g = (existing >> 8) & 0xFFU;
    bg_b = existing & 0xFFU;
    a = static_cast<uint32_t>(alpha);
    out_r = (static_cast<uint32_t>(r) * a + bg_r * (255U - a)) / 255U;
    out_g = (static_cast<uint32_t>(g) * a + bg_g * (255U - a)) / 255U;
    out_b = (static_cast<uint32_t>(b) * a + bg_b * (255U - a)) / 255U;
    pixels[draw_y * buf_w + draw_x] = (out_r << 16) | (out_g << 8) | out_b;
    return ;
}

#if defined(FT_VOX_HAVE_FREETYPE)
void FontRenderer::draw_text_real_font(uint32_t *pixels, int buf_w, int buf_h, int x, int y,
    const char *text, uint32_t colour, float pixel_height) const
{
    FontRendererImpl *impl;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    int cursor_x;
    int baseline_y;
    size_t index;

    impl = static_cast<FontRendererImpl *>(impl_);
    (void)FT_Set_Pixel_Sizes(impl->face, 0,
        static_cast<FT_UInt>(std::lround(static_cast<double>(pixel_height))));
    red = static_cast<uint8_t>((colour >> 16) & 0xFFU);
    green = static_cast<uint8_t>((colour >> 8) & 0xFFU);
    blue = static_cast<uint8_t>(colour & 0xFFU);
    cursor_x = x;
    baseline_y = y + static_cast<int>(impl->face->size->metrics.ascender >> 6);
    index = 0U;
    while (text[index] != '\0')
    {
        FT_ULong codepoint;
        FT_GlyphSlot slot;
        int row;

        codepoint = static_cast<FT_ULong>(static_cast<unsigned char>(text[index]));
        if (FT_Load_Char(impl->face, codepoint, FT_LOAD_RENDER) != 0)
        {
            ++index;
            continue ;
        }
        slot = impl->face->glyph;
        row = 0;
        while (row < static_cast<int>(slot->bitmap.rows))
        {
            int col;

            col = 0;
            while (col < static_cast<int>(slot->bitmap.width))
            {
                uint8_t alpha;

                alpha = slot->bitmap.buffer[static_cast<size_t>(row) *
                    static_cast<size_t>(slot->bitmap.pitch) + static_cast<size_t>(col)];
                if (alpha != 0U)
                    blend_glyph(pixels, buf_w, buf_h, cursor_x + slot->bitmap_left + col,
                        baseline_y - slot->bitmap_top + row, red, green, blue, alpha);
                ++col;
            }
            ++row;
        }
        cursor_x += static_cast<int>(slot->advance.x >> 6);
        ++index;
    }
    return ;
}
#else
void FontRenderer::draw_text_real_font(uint32_t *pixels, int buf_w, int buf_h, int x, int y,
    const char *text, uint32_t colour, float pixel_height) const
{
    (void)pixels;
    (void)buf_w;
    (void)buf_h;
    (void)x;
    (void)y;
    (void)text;
    (void)colour;
    (void)pixel_height;
    return ;
}
#endif

void FontRenderer::draw_text(uint32_t *pixels, int buf_w, int buf_h, int x, int y, const char *text,
    uint32_t colour, float pixel_height_override) const
{
    float height;
    int glyph_scale;
    int cursor_x;
    int index;

    if (!loaded_ || pixels == ft_nullptr || text == ft_nullptr)
        return ;
    height = effective_pixel_height(pixel_height_override);
    if (use_real_font_)
    {
        draw_text_real_font(pixels, buf_w, buf_h, x, y, text, colour, height);
        return ;
    }
    glyph_scale = std::max(1, static_cast<int>(std::lround(static_cast<double>(height) / 7.0)));
    cursor_x = x;
    index = 0;
    while (text[index] != '\0')
    {
        const BitmapGlyph *glyph;

        glyph = find_glyph(text[index]);
        if (glyph != ft_nullptr)
            draw_glyph(pixels, buf_w, buf_h, cursor_x, y, glyph, glyph_scale, colour);
        cursor_x += glyph_scale * 6;
        ++index;
    }
    return ;
}

void FontRenderer::draw_text_centered(uint32_t *pixels, int buf_w, int buf_h, int cx, int y,
    const char *text, uint32_t colour, float pixel_height_override) const
{
    float height;
    int width;

    if (!loaded_ || text == ft_nullptr)
        return ;
    height = effective_pixel_height(pixel_height_override);
    width = text_width(text, height);
    draw_text(pixels, buf_w, buf_h, cx - width / 2, y, text, colour, height);
    return ;
}

FontRenderer &FontRenderer::instance()
{
    static FontRenderer fr;
    return (fr);
}

