#version 410

precision mediump float;

out vec4 FragColor;

in vec2 vs_texcoord;

uniform sampler2D screen;
uniform sampler2D noise;

uniform float strength;

uniform vec2 screenResolution;

void main()
{
  //Calculate subdivisions
  vec2 subdivision = screenResolution / vec2(strength);

  vec2 pixelUV = subdivision * vs_texcoord;

  //Floor uv
  pixelUV = floor(pixelUV);

  pixelUV /= subdivision;

  vec3 color = texture(screen, pixelUV).xyz;

  FragColor = vec4(color, 1.0);
}