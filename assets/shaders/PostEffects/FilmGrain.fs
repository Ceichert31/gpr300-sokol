#version 410

out vec4 FragColor;

in vec2 vs_texcoord;

uniform sampler2D screen;
uniform sampler2D noise;

uniform float strength;
uniform float resolution;
uniform float time;

const float toRadians = 3.14 / 180;

void main()
{
  vec3 color = texture(screen, vs_texcoord).xyz;

  vec2 uv = vs_texcoord * vec2(resolution);

  float randomIntensity = fract(10000 * sin((uv.x + uv.y * time) * toRadians));

  float intensity = randomIntensity * 0.1 * strength;

  color.rgb += intensity;

  FragColor = vec4(color, 1.0);
}