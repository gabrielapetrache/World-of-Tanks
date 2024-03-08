#version 330

// Input
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture;
in vec3 frag_color;
in float frag_health;

uniform vec3 object_color;
// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec3 light_color = vec3(0.1f, 0.1f, 0.05f);
    if (frag_health == 3) {
        out_color = vec4(object_color, 1.0f);
    } else if (frag_health == 2) {
        out_color = vec4(object_color - light_color, 1.0f);
    } else if (frag_health == 1) {
        out_color = vec4(object_color - 2 * light_color, 1.0f);
    } else {
        out_color = vec4(object_color - 3 * light_color, 1.0f);
    }
}
