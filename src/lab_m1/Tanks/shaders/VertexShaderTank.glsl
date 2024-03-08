#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_texture;
layout(location = 3) in vec3 v_color;
// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;
uniform float health;

out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_texture;
out vec3 frag_color;
out float frag_health;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    frag_position = v_position;
    frag_normal = v_normal + vec3(cos(Time), cos(Time), 0.f);
    frag_texture = v_texture;
    frag_color = v_color;
    frag_health = health;
    float deformation = 0.0;
    if (health == 2) {
        deformation = rand(v_position.xy) * 0.03;
    } else if (health == 1) {
        deformation = rand(v_position.xy) * 0.05;
    } else if (health <= 0) {
        deformation = rand(v_position.xy) * 0.1;
    }

    vec3 deformation_vec = v_position + frag_normal * deformation;

    gl_Position = Projection * View * Model * vec4(deformation_vec, 1.0) + vec4(tan(Time), tan(Time), 0.f, 0.f);
}
