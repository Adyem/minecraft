#ifndef FONT_RENDERER_HPP
#define FONT_RENDERER_HPP


#include "../ft_vox.hpp"
class FontRenderer
{
  public:
    FontRenderer();
    FontRenderer(const FontRenderer &other);
    ~FontRenderer();
    FontRenderer &operator=(const FontRenderer &other);

    bool load(float pixel_height);
    void destroy();

    bool is_loaded() const;
    bool is_using_real_font() const;
    int line_height(float pixel_height_override = 0.0f) const;
    int text_width(const char *text, float pixel_height_override = 0.0f) const;

    void draw_text(uint32_t *pixels, int buf_w, int buf_h, int x, int y, const char *text,
                   uint32_t colour, float pixel_height_override = 0.0f) const;
    void draw_text_centered(uint32_t *pixels, int buf_w, int buf_h, int cx, int y, const char *text,
                            uint32_t colour, float pixel_height_override = 0.0f) const;

    static FontRenderer &instance();

  private:
    void *impl_;
    float pixel_height_;
    bool loaded_;
    bool use_real_font_;

    static const char *find_comic_sans();
    static const char *COMIC_CANDIDATES[];

    float effective_pixel_height(float pixel_height_override) const;
    bool load_real_font();
    void draw_text_real_font(uint32_t *pixels, int buf_w, int buf_h, int x, int y,
                             const char *text, uint32_t colour, float pixel_height) const;
    int text_width_real_font(const char *text, float pixel_height) const;
    void blend_glyph(uint32_t *pixels, int buf_w, int buf_h, int draw_x, int draw_y, uint8_t r,
                     uint8_t g, uint8_t b, uint8_t alpha) const;
};

#endif
