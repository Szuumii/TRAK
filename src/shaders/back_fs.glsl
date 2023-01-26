#version 410 core
in vec3 Position_FS_in;
in vec3 Normal_FS_in;
in vec2 Texcoord_FS_in;
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 norm;
void main(void)
{
	FragColor = vec4(Position_FS_in, 1.0f);
	norm = vec4(Normal_FS_in,1.0f);
}