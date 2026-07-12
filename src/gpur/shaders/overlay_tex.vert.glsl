#version 330 core
uniform vec2 u_overlay_ndc_br;
out vec2 v_uv;
void main()
{
    bool is_right = (gl_VertexID == 1 || gl_VertexID == 3);
    bool is_top   = (gl_VertexID == 2 || gl_VertexID == 3);
    gl_Position = vec4(
        is_right ? u_overlay_ndc_br.x : -1.0,
        is_top   ? 1.0 : u_overlay_ndc_br.y,
        0.0, 1.0);
    v_uv = vec2(is_right ? 1.0 : 0.0, is_top ? 0.0 : 1.0);
}
