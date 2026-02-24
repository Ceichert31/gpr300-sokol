#version 410

// attributes
layout(location = 0) in vec3 in_position;

// uniforms
uniform mat4 light_view_proj;
uniform mat4 model;


// varyings
out vec3 vs_position;

void main()
{
  vs_position = in_position;

  vec4 world_position = model * vec4(in_position, 1.0);

  gl_Position = light_view_proj * world_position;
}