#version 330 core
uniform vec2 u_sky_size;
out vec4 frag_color;
void main()
{
    float t = gl_FragCoord.y / u_sky_size.y;
    vec3 bot = vec3(0.322, 0.604, 0.878);
    vec3 top = vec3(0.384, 0.776, 0.965);
    frag_color = vec4(mix(bot, top, t), 1.0);
}
