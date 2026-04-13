#version 410

out vec4 FragLighting;

in vec2 vs_texcoord;

struct Light{
  vec3 color;
  vec3 position;
  float radius;
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

const int LIGHT_COUNT = 64;
uniform Light lights[LIGHT_COUNT];

uniform vec3 camera;
uniform Material material;

vec3 blinnPhong(){

  //Sample from G-Buffer
  vec3 fragPosition = texture(g_position, vs_texcoord).rgb;
  vec3 normal = texture(g_normal, vs_texcoord).rgb;
  vec3 albedo = texture(g_albedo, vs_texcoord).rgb;

  //Apply ambient factor to lighting
  vec3 lighting = albedo * material.ambient;

  vec3 viewDirection = normalize(camera - fragPosition);

  //Iterate through lights and calculate diffuse
  for (int i = 0; i < LIGHT_COUNT; ++i){

    float volumeDistance = length(lights[i].position - fragPosition);

    //Skip Diffuse lighting if outside of volume radius
    if (volumeDistance > lights[i].radius)
      continue;

    vec3 lightDirection = normalize(lights[i].position - fragPosition);

    vec3 diffuse = max(dot(normal, lightDirection), 0.0) * albedo * lights[i].color;

    lighting += diffuse;
  }

  return lighting;
}

void main()
{
  vec3 lighting = blinnPhong();
  FragLighting = vec4(lighting, 1.0);
}