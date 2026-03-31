#version 410

out vec4 FragLighting;

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

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;

uniform vec3 camera;
uniform Light light;
uniform Material material;

vec3 blinnPhong(vec3 position, vec3 normal, Material material){

  vec3 view_dir = normalize(camera - position);
  vec3 light_dir = normalize(light.position - position);
  vec3 reflect_dir = reflect(light_dir, normal);
  vec3 half_dir = normalize(light_dir + view_dir);

  //Calculate diffuse lighting (light diffusion w/ normal)
  float diffuse = max(dot(normal, light_dir), 0);

  //Calculate specular lighting
  float specular = max(dot(normal, half_dir), 0);
  specular = pow(specular, 128 * material.shininess);

  //Our uncolored lighting model
  vec3 lighting = diffuse * material.diffuse + specular * material.specular + material.ambient;

  return lighting * light.color;
}

void main()
{
  vec3 lighting = blinnPhong(texture(g_position, vs_texcoord).rgb, texture(g_normal, vs_texcoord).rgb, material);
  FragLighting = vec4(lighting, 1.0) * texture(g_albedo, vs_texcoord);
}