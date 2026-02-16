#version 410

precision mediump float;

out vec4 FragColor;

in vec2 vs_texcoord;

uniform sampler2D screen;

void main()
{
  vec3 color = texture(screen, vs_texcoord).rgb;
  vec3 grey = vec3(dot(color, vec3(0.299, 0.587, 0.114)));
  FragColor = vec4(grey, 1.0);
}