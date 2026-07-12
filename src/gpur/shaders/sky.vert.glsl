#version 330 core
void main()
{
    const vec2 QUAD[4] = vec2[4](
        vec2(-1.0, -1.0), vec2(1.0, -1.0),
        vec2(-1.0,  1.0), vec2(1.0,  1.0));
    gl_Position = vec4(QUAD[gl_VertexID], 0.9999, 1.0);
}
