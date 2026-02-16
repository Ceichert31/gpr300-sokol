#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D main_texture;
uniform sampler2D secondary_texture;
uniform float _deltaTime;
uniform vec2 _floatSpeed;
uniform vec3 _waterColor;

vec3 effect() {
  return normalize(vs_normal.rgb);
}

void main()
{
  vec2 uv = vs_texcoord + vec2(_deltaTime * _floatSpeed);
  uv.x += 0.3f * cos(uv.x + _deltaTime);
  uv.y += 0.3f * sin(uv.y + _deltaTime);

  vec4 firstWaterLayer = texture(main_texture, uv);
  vec4 secondWaterLayer = texture(secondary_texture, uv * 1.2);

  vec3 mixedColor = vec3((firstWaterLayer * 0.75) - (secondWaterLayer * 0.25));

  FragColor = vec4(mixedColor + _waterColor, 1.0);
}