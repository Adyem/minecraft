#include "../../src/gpur/GpuSkyRenderer.hpp"

static const char *SKY_VERT = R"GLSL(
#version 330 core
void main()
{
    const vec2 QUAD[4] = vec2[4](
        vec2(-1.0, -1.0), vec2(1.0, -1.0),
        vec2(-1.0,  1.0), vec2(1.0,  1.0));
    gl_Position = vec4(QUAD[gl_VertexID], 0.9999, 1.0);
}
)GLSL";

static const char *SKY_FRAG = R"GLSL(
#version 330 core
uniform vec2 u_sky_size;
out vec4 frag_color;
void main()
{
    float t   = gl_FragCoord.y / u_sky_size.y;
    vec3  bot = vec3(0.322, 0.604, 0.878);
    vec3  top = vec3(0.384, 0.776, 0.965);
    frag_color = vec4(mix(bot, top, t), 1.0);
}
)GLSL";

static const char *TINT_FRAG = R"GLSL(
#version 330 core
uniform vec4 u_tint_color;
out vec4 frag_color;
void main() { frag_color = u_tint_color; }
)GLSL";

GpuSkyRenderer::GpuSkyRenderer()
    : _fullscreen_vao(0), _u_sky_size(-1), _u_tint_color(-1)
{}
GpuSkyRenderer::GpuSkyRenderer(const GpuSkyRenderer &other) { (void)other; }
GpuSkyRenderer::~GpuSkyRenderer() { destroy(); }
GpuSkyRenderer &GpuSkyRenderer::operator=(const GpuSkyRenderer &other)
{ (void)other; return *this; }

bool GpuSkyRenderer::initialize() noexcept
{
    if (_sky_shader.initialize(SKY_VERT, SKY_FRAG) != 0)
        return false;
    if (_tint_shader.initialize(SKY_VERT, TINT_FRAG) != 0)
        return false;
    _u_sky_size   = _sky_shader.uniform("u_sky_size");
    _u_tint_color = _tint_shader.uniform("u_tint_color");
    glGenVertexArrays(1, &_fullscreen_vao);
    return true;
}

void GpuSkyRenderer::destroy() noexcept
{
    if (_fullscreen_vao != 0)
    { glDeleteVertexArrays(1, &_fullscreen_vao); _fullscreen_vao = 0; }
    (void)_sky_shader.destroy();
    (void)_tint_shader.destroy();
}

void GpuSkyRenderer::draw_fullscreen() const noexcept
{
    glBindVertexArray(_fullscreen_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void GpuSkyRenderer::draw_sky(int width, int height) const noexcept
{
    _sky_shader.use();
    glUniform2f(_u_sky_size,
        static_cast<float>(width), static_cast<float>(height));
    draw_fullscreen();
}

void GpuSkyRenderer::draw_tint(float r, float g, float b, float a) const noexcept
{
    _tint_shader.use();
    glUniform4f(_u_tint_color, r, g, b, a);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    draw_fullscreen();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
