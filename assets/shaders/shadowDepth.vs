#version 410

// attributes
layout(location = 0) in vec3 in_position;

// uniforms
uniform mat4 light_view_proj;
uniform mat4 model;


// varyings
out vec3 vs_position;
out vec3 vs_normal;
out vec2 vs_texcoord;
out mat3 vs_tangent_space;

void main()
{
  vs_position = in_position;

  vec4 world_position = model * vec4(in_position, 1.0);

  gl_Position = light_view_proj * world_position;
}