#version 410

precision mediump float;

out vec4 FragColor;

in vec2 vs_texcoord;

uniform sampler2D screen;
uniform sampler2D noise;

uniform float strength;
uniform float resolution;

void main()
{
  vec3 noisyTexture = texture(noise, vs_texcoord * resolution).xyz * 0.01 * strength;
  vec2 noisyUV = vec2(vs_texcoord.x + noisyTexture.x, vs_texcoord.y + noisyTexture.y);
  vec3 color = texture(screen, noisyUV).xyz;

  //Sample screen with modified UV coords, add noise

  //color += noisyTexture * strength;

  FragColor = vec4(color, 1.0);
}