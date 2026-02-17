#version 410

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D tex;
uniform sampler2D spec;
uniform sampler2D warp;

uniform float _deltaTime;

uniform vec2 _floatSpeed;
uniform vec3 _waterColor;

uniform vec3 camera_position;

float scale = 5.0;

vec3 effect() {
  return normalize(vs_normal.rgb);
}

const vec3 bright = vec3(0.299, 0.587, 0.144);

void main()
{
  vec2 uv = vs_texcoord + vec2(_deltaTime * _floatSpeed);
  // uv.x += 0.3f * cos(uv.x + _deltaTime);
  // uv.y += 0.3f * sin(uv.y + _deltaTime);

  vec4 firstWaterLayer = texture(tex, uv);
  vec4 secondWaterLayer = texture(spec, uv * 1.2);

  vec2 warp_uv = vs_texcoord * scale;
  warp_uv += (_deltaTime * _floatSpeed);
  vec2 warp_effect = texture(warp, warp_uv).xy;

  vec2 albedo_uv = vs_texcoord * scale;
  vec4 albedo = texture(tex, albedo_uv + warp_effect);

  vec3 finalColor = _waterColor + vec3(albedo.a);

  //specular/shimmer
  vec2 spec_uv = vs_texcoord * 10.0;
  spec_uv += (_deltaTime * _floatSpeed);
  vec3 specular = texture(spec, spec_uv).xyz;

  vec3 sample1 = texture(spec, spec_uv + vec2(0.5, 0.5) * _deltaTime).xyz;
  vec3 sample2 = texture(spec, spec_uv + vec2(-0.5, -0.5) * _deltaTime * 1.2).xyz;

  vec3 mixedSpec = sample1 + sample2;

  vec3 mixedColor = vec3((firstWaterLayer * 0.75) - (secondWaterLayer * 0.25));

  float fresnel = dot(normalize(camera_position), vec3(0, 1, 0));

  float brightness = dot(mixedSpec, bright);

  if (brightness <= 0.5 || brightness > 95.0){
    finalColor = mix(finalColor, finalColor + mixedSpec, fresnel);
  }
  FragColor = vec4(finalColor, 1.0);
}