#version 410

precision mediump float;

out vec4 FragColor;

in vec2 vs_texcoord;

uniform sampler2D screen;
uniform sampler2D noise;

uniform float strength;

const float offset = 1.0 / 300.0;

const vec2 offsets[9] = vec2[] 
(
    vec2(-offset,offset), //top-left
    vec2(0, offset), //top-middle
    vec2(offset,offset), //top-right

    vec2(-offset,0), //center-left
    vec2(0,0), //center-middle
    vec2(offset,0), //center-right

    vec2(-offset, -offset), //bottom-left
    vec2(0, -offset), //bottom-middle
    vec2(offset, -offset) //bottom-right
);

float gaussian[9] = float[](
    1, 2, 1,
    2, 4, 2,
    1, 2, 1
);

void main()
{
  vec3 color = texture(screen, vs_texcoord).xyz;
  
  for (int i = 0; i < 9; i++)
  {
      vec3 local = vec3(texture(screen, vs_texcoord + offsets[i]));
      color += local * (0.0625 * gaussian [i]) / strength;
  }
  FragColor = vec4(color, 1.0);
}