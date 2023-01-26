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

void main(void)
{
	// rzutowanie P1 na screenspace
	vec4 P1 = gViewProjectMatrix * vec4(Position_FS_in, 1.0);
	vec3 P1vec3 = P1.xyz / P1.w;
	vec2 backPointCoord = 0.5 * (P1vec3.xy + vec2(1.0, 1.0));

	float ratio = 1.00 / index;
	vec3 N1 = normalize(Normal_FS_in);
	vec3 V = normalize(Position_FS_in - EyePosition);

	// pierwsza refrakcja
	vec3 T1 = refract(V, N1, ratio);
	T1 = normalize(T1);

	// wyliczanie P2 (z tyłu bryły)
	float thetaI = abs(asin(length(cross(V, N1))));
	float thetaT = abs(asin(length(cross(V, N1)) * ratio));
	vec3 backPoint = texture(backWorldPos, backPointCoord).xyz;
	float dv = distance(backPoint, Position_FS_in);
	float dn = Distance_FS_in;
	float thetaRatio = thetaT / thetaI;

	// interpolacja odległości do punktu
	float d = thetaRatio*dv + (1 - thetaRatio)*dn;
	vec3 P2 = Position_FS_in + d * T1;

	// rzutowanie P2 na screenspace
	vec4 projectedP2 = gViewProjectMatrix * vec4(P2, 1.0);
	vec3 projectedP2vec3 = projectedP2.xyz / projectedP2.w;
	vec2 normalCoord = 0.5 * (projectedP2vec3.xy + vec2(1.0, 1.0));
	vec3 N2 = texture(backWorldNorm, normalCoord).xyz;

	// odwrócenie normalnej
	N2 = -N2;

	// druga refrakcja
	vec4 colorOut=vec4(0);
	vec3 T2 = refract(T1, N2, index);
	colorOut = texture(skybox, T2);
	FragColor = colorOut;
}