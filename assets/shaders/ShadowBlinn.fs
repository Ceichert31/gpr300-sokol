#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;
in mat3 vs_tangent_space;
in vec4 vs_light_proj_pos;

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
uniform sampler2D main_texture;
uniform sampler2D normal_map;
uniform bool normalMapOn;

uniform sampler2D shadow_map;

uniform float bias;

float ShadowCalculation(vec4 fragPositionLightSpace){
  
  //Perspective division
  vec3 proj_coords = fragPositionLightSpace.xyz / fragPositionLightSpace.w;

  proj_coords = proj_coords * 0.5 + 0.5;

  float closest = texture(shadow_map, proj_coords.xy).r;
  float current = proj_coords.z;
 
  float shadow = current - bias > closest ? 1.0 : 0.0;
  //float shadow = 0.75;
  return shadow;
}

vec3 blinnphong(vec3 frag_pos, Light light) {

  vec3 normal = vs_normal;

  //Branching bad, but testing!
  if (normalMapOn){
    //Sample normal map
    normal = texture(normal_map, vs_texcoord).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(vs_tangent_space * normal); 
  }

  //Get dot between light and normal
  float angle = normalize(dot(normal, light.position));

  vec3 view_dir = normalize(camera - frag_pos);
  vec3 light_dir = normalize(light.position - frag_pos);
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
  vec3 lighting = blinnphong(vs_position, light);

  float shadow = ShadowCalculation(vs_light_proj_pos);

  lighting *= (1.0 - shadow);

  FragColor = vec4(lighting, 1.0) * texture(main_texture, vs_texcoord);
}