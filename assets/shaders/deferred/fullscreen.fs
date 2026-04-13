#version 410

out vec4 FragColor;

in vec2 vs_texcoord;

uniform sampler2D screen;
uniform sampler2D lighting;

void main()
{
  vec3 color = texture(screen, vs_texcoord).rgb;
  vec3 light = texture(lighting, vs_texcoord).rgb;
  FragColor = vec4(color * light, 1.0);
}