#version 410

out vec4 FragLighting;

in vec2 vs_texcoord;

struct Light{
  vec3 color;
  vec3 position;
  float radius;
};

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;

uniform vec2 screenSize;

uniform Light light;

uniform vec3 camera;

//Material breakdown:
//Material.r = ambient
//Material.g = diffuse
//Material.b = specular
//Material.a = shininess

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
  
  vec4 material = texture(g_material, uv);

  //Apply ambient factor to lighting
  vec3 ambient = albedo.rgb;

  //Direction calculations
  vec3 viewDirection = normalize(camera - fragPosition);
  vec3 lightDirection = normalize(light.position - fragPosition);
  vec3 halfwayDirection = normalize(lightDirection + viewDirection);

  //Diffuse calculation
  vec3 diffuse = max(dot(normal, lightDirection), 0.0) * albedo.rgb;

  //Specular calculation
  vec3 specular = pow(max(dot(normal, halfwayDirection), 0.0), material.a * 128.0) * vec3(material.b);

  return (ambient * material.r + diffuse * material.g + specular) * light.color;
}

void main()
{
  vec3 lighting = blinnPhong();
  FragLighting = vec4(lighting, 1.0);
}