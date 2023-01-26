#version 410 core

in vec3 Position_FS_in;
in vec3 Normal_FS_in;
in vec2 Texcoord_FS_in;
in float Distance_FS_in;
layout(location = 0) out vec4 FragColor;
uniform float index;
uniform vec3 EyePosition;
uniform mat4 gViewProjectMatrix;
uniform sampler2D backWorldPos;
uniform sampler2D backWorldNorm;
uniform samplerCube skybox;
struct light{
	int type;
	vec3 position;
	vec3 color;
	float fade;
};

void main(void)
{
	vec4 ProjCoord = gViewProjectMatrix * vec4(Position_FS_in, 1.0);
	vec3 ProjCoord3 = ProjCoord.xyz / ProjCoord.w;
	vec2 texcoord = 0.5 * (ProjCoord3.xy + vec2(1.0, 1.0));

	float ratio = 1.00 / index;
	vec3 N = normalize(Normal_FS_in);
	vec3 I = normalize(Position_FS_in - EyePosition);

	vec3 T1 = refract(I, N, ratio);
	T1 = normalize(T1);

	float T1sin_in = length(cross(I, N));
	float T1theta_in = abs(asin(T1sin_in));
	float T1sin_out = T1sin_in * ratio;
	float T1theta_out = abs(asin(T1sin_out));

	vec3 backPoint = texture(backWorldPos, texcoord).xyz;

	if (backPoint == vec3(0, 0, 0))
	{
		backPoint = Position_FS_in;
	}
	float dv = distance(backPoint, Position_FS_in);
	float dn = Distance_FS_in;
	float ratio_theta = T1theta_out / T1theta_in;
	float d = ratio_theta*dv + (1 - ratio_theta)*dn;
	vec3 P2 = Position_FS_in + d * T1;

	vec4 ProjCoord_P2 = gViewProjectMatrix * vec4(P2, 1.0);
	vec3 ProjCoord3_P2 = ProjCoord_P2.xyz / ProjCoord_P2.w;
	vec2 texcoord_P2 = 0.5 * (ProjCoord3_P2.xy + vec2(1.0, 1.0));
	vec3 N2 = texture(backWorldNorm, texcoord_P2).xyz;
	if (N2 == vec3(0, 0, 0))
	{
		N2 = normalize(T1 - dot(I, T1)* I);
	}

	N2 = -N2;

	vec4 colorR2=vec4(0);
	vec3 T2 = refract(T1, N2, index);
	colorR2 = texture(skybox, T2);
	FragColor = colorR2;
}