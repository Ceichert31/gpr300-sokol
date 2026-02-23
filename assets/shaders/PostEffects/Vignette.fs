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
  vec3 color = texture(screen, vs_texcoord).xyz;

  vec2 center = vec2(0.5);

  //Create empty space in center of screen
  float dist = distance(center, vs_texcoord);

  //Make circle larger
  dist = pow(dist, strength);

  vec3 black = vec3(0,0,0);

  color = mix(color, black, dist);

  FragColor = vec4(color, 1.0);
}