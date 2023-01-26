#version 410 core
layout(location = 0) in vec3 in_position;
uniform mat4 gWorld;
uniform mat4 gViewProjectMatrix;
out vec3 tex_coord;
void main(void)
{
	tex_coord = in_position;
	vec4 pos = gViewProjectMatrix * vec4(in_position, 1.0);
	gl_Position = vec4(pos.xy,pos.w-0.001,pos.w);
}