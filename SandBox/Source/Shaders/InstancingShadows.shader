#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec2 uvA;

out vec2 uv;

void main()
{
	uv = uvA;
	gl_Position = vec4(positionA * 2.0, 1.0);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 fragColor;

struct DirectionalLight
{
	vec3 direction;
	vec3 color;
	float intensity;
};

uniform DirectionalLight u_DirectionalLight;
uniform sampler2D u_CSM[3];
uniform mat4 u_LightSpaceMatricies[32];
uniform mat4 u_View;
uniform vec3 u_CameraPosition;
uniform float u_CascadesDistance[32];
uniform int u_CascadesCount;
uniform int u_Pcf;
uniform bool u_ShadowsEnabled;
uniform bool u_ShadowsVisualized;
uniform float u_Bias;
uniform float u_FarPlane;
uniform float u_Texels;
uniform float u_Fog;

uniform sampler2D u_WorldPosition;
uniform sampler2D u_Normal;
uniform sampler2D u_Albedo;

in vec2 uv;

vec4 worldPosition = texture(u_WorldPosition, uv);
vec3 normal = texture(u_Normal, uv).xyz;

vec3 ShadowCompute()
{
	float depth = abs((u_View * worldPosition).z);

	int layer = -1;
	for (int i = 0; i < u_CascadesCount; ++i)
	{
		if (depth < u_CascadesDistance[i])
		{
			layer = i;
			break;
		}
	}
	if (layer == -1)
	{
		layer = u_CascadesCount;
	}

	vec4 lightSpacePosition = u_LightSpaceMatricies[layer] * worldPosition;
	vec3 projectedCoords = lightSpacePosition.xyz / lightSpacePosition.w;

	projectedCoords = projectedCoords * 0.5 + 0.5;

	vec3 shadow = vec3(0.0);

	float currentDepth = projectedCoords.z;
	if (currentDepth > 1.0)
	{
		return shadow;
	}

	float bias = clamp(u_Bias * tan(acos(dot(normal, u_DirectionalLight.direction))), 0.0, 0.005);
	if (layer == u_CascadesCount)
	{
		bias *= 1.0 / (u_FarPlane * 0.5);
	}
	else
	{
		bias *= 1.0 / (u_CascadesDistance[layer] * 0.5);
	}

	int pcf = layer == 0 ? u_Pcf : 0;

	vec2 texelSize = u_Texels / textureSize(u_CSM[layer], 0);
	for (int x = -pcf; x <= pcf; ++x)
	{
		for (int y = -pcf; y <= pcf; ++y)
		{
			float pcfDepth = texture(u_CSM[layer], projectedCoords.xy + vec2(x, y) * texelSize).r;

			if (u_ShadowsVisualized)
			{
				shadow[layer] = currentDepth - bias > pcfDepth ? 1.0 : 0.0;
			}
			else
			{
				shadow += vec3(currentDepth - bias > pcfDepth ? 1.0 : 0.0);
			}
		}
	}

	if (u_ShadowsVisualized)
	{
		shadow[layer] /= ((pcf * 2 + 1) * (pcf * 2 + 1));
	}
	else
	{
		shadow /= vec3(((pcf * 2 + 1) * (pcf * 2 + 1)));
	}

	float shadowDistance = distance(worldPosition.xyz, u_CameraPosition);
	float farPlaneEdge = u_FarPlane * (1.0 - u_Fog);
	if (shadowDistance > farPlaneEdge)
	{
		float shadowFog = clamp((shadowDistance - farPlaneEdge) / (u_FarPlane * u_Fog), 0.0, 1.0);
		shadow *= (1.0 - shadowFog);
	}

	return shadow;
}

void main()
{
	fragColor = vec4(ShadowCompute(), 1.0);
}