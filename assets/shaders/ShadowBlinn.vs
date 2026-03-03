#version 410

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec3 in_tangent;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;
uniform mat4 light_view_proj;

// varyings
out vec3 vs_position;
out vec3 vs_normal;
out vec2 vs_texcoord;
out mat3 vs_tangent_space;
out vec4 vs_light_proj_pos;

void main()
{
  //Transform to world space coordinates
  vec3 tangent = normalize(vec3(model * vec4(in_tangent, 0.0)));
  vec3 normal = normalize(vec3(model * vec4(in_normal, 0.0)));

  //Use Gram-Schmidt process to re-orthogonalize tangent vector
  tangent = normalize(tangent - dot(tangent, normal) * normal);

  vec3 bitangent = cross(normal, tangent);

  //Combine into 3x3 matrix
  vs_tangent_space = mat3(tangent,bitangent,normal);

  vs_position = vec3(model * vec4(in_position, 1.0));
  vs_light_proj_pos = light_view_proj * vec4(vs_position, 1.0);

  vs_normal = transpose(inverse(mat3(model))) * in_normal;
  vs_texcoord = in_texcoord;
  gl_Position = view_proj * model * vec4(in_position, 1.0);
}