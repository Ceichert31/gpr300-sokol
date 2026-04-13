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

uniform vec2 screenSize;

uniform Light light;

uniform vec3 camera;
uniform Material material;

vec3 blinnPhong(){

  vec2 uv = gl_FragCoord.xy / screenSize;

  //Sample from G-Buffer
  vec3 fragPosition = texture(g_position, uv).rgb;

  float volumeDistance = length(light.position - fragPosition);

  //Stop all lighting calculation if outside of volume 
  if (volumeDistance > light.radius)
    discard;

  vec3 normal = texture(g_normal, uv).rgb;

  //Albedo alpha channel is specular
  vec4 albedo = texture(g_albedo, uv);
  
  //Apply ambient factor to lighting
  vec3 lighting = material.ambient;

  vec3 viewDirection = normalize(camera - fragPosition);

 //Diffuse calculation
  vec3 lightDirection = normalize(light.position - fragPosition);
  vec3 diffuse = max(dot(normal, lightDirection), 0.0) * albedo.rgb * light.color * material.diffuse;
  lighting += diffuse;

  //Specular calculation
  vec3 halfwayDirection = normalize(lightDirection + viewDirection);
  float spec = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);

  vec3 specular = light.color * spec * albedo.a;
  lighting += specular;

  return lighting;
}

void main()
{
  vec3 lighting = blinnPhong();
  FragLighting = vec4(lighting, 1.0);
}