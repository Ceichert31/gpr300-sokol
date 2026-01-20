#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

struct Light{
  vec3 color;
  vec3 position;
};

struct Material{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

uniform vec3 camera;
uniform Light light;
uniform Material material;
uniform float alpha;

vec3 blinnphong(vec3 normal, vec3 frag_pos, Light light) {
  float angle = normalize(dot(normal, light.position));

  vec3 view_dir = normalize(camera - frag_pos);
  vec3 light_dir = normalize(light.position - frag_pos);
  vec3 reflect_dir = reflect(light_dir, normal);
  vec3 half_dir = normalize(light_dir + view_dir);

  //Calculate diffuse lighting (light diffusion w/ normal)
  float diffuse = max(dot(normal, light_dir), 0);

  //Calculate specular lighting
  float specular = max(dot(normal, half_dir), 0);
  specular = pow(specular, alpha * material.shininess);

  //Our uncolored lighting model
  vec3 lighting = diffuse * material.diffuse + specular * material.specular + material.ambient;

  return lighting * light.color;
}

void main()
{
  vec3 lighting = blinnphong(vs_normal, vs_position, light);
  vec3 object_color = vs_normal.rgb * 0.5 + 0.5;
  vec3 result = lighting * object_color;
  FragColor = vec4(result, 1.0);
}