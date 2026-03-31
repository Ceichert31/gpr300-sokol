#version 410

out vec4 FragLighting;

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

void main()
{
  FragLighting = vec4(light.color, 1.0);
}