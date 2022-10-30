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

layout(location = 0) out vec4 dirShadows;

struct DirectionalLight
{
	vec3 direction;
	vec3 color;
	float intensity;
};

struct Shadows
{
	sampler2D CSM[3];
	mat4 lightSpaceMatricies[3];
	mat4 view;
	float cascadesDistance[3];
	int cascadesCount;
	int pcf;
	bool isVisualized;
	float bias;
	float farPlane;
	float texels;
	float fog;
};

uniform DirectionalLight u_DirectionalLight;
uniform Shadows u_Shadows;
uniform vec3 u_CameraPosition;

uniform sampler2D u_WorldPosition;
uniform sampler2D u_Normal;

in vec2 uv;

vec4 worldPosition = texture(u_WorldPosition, uv);
vec3 normal = texture(u_Normal, uv).xyz;

vec3 DirectionalShadowCompute()
{
	float depth = abs((u_Shadows.view * worldPosition).z);

	int layer = -1;
	for (int i = 0; i < u_Shadows.cascadesCount; ++i)
	{
		if (depth < u_Shadows.cascadesDistance[i])
		{
			layer = i;
			break;
		}
	}
	if (layer == -1)
	{
		layer = u_Shadows.cascadesCount;
	}

	vec4 lightSpacePosition = u_Shadows.lightSpaceMatricies[layer] * worldPosition;
	vec3 projectedCoords = lightSpacePosition.xyz / lightSpacePosition.w;

	projectedCoords = projectedCoords * 0.5 + 0.5;

	vec3 shadow = vec3(0.0);

	float currentDepth = projectedCoords.z;
	if (currentDepth > 1.0)
	{
		return shadow;
	}

	float bias = clamp(u_Shadows.bias * tan(acos(dot(normal, u_DirectionalLight.direction))), 0.0, 0.005);
	if (layer == u_Shadows.cascadesCount)
	{
		bias *= 1.0 / (u_Shadows.farPlane * 0.5);
	}
	else
	{
		bias *= 1.0 / (u_Shadows.cascadesDistance[layer] * 0.5);
	}

	int pcf = layer == 0 ? u_Shadows.pcf : 0;

	vec2 texelSize = u_Shadows.texels / textureSize(u_Shadows.CSM[layer], 0);
	for (int x = -pcf; x <= pcf; ++x)
	{
		for (int y = -pcf; y <= pcf; ++y)
		{
			float pcfDepth = texture(u_Shadows.CSM[layer], projectedCoords.xy + vec2(x, y) * texelSize).r;

			if (u_Shadows.isVisualized)
			{
				shadow[layer] = currentDepth - bias > pcfDepth ? 1.0 : 0.0;
			}
			else
			{
				shadow += vec3(currentDepth - bias > pcfDepth ? 1.0 : 0.0);
			}
		}
	}

	if (u_Shadows.isVisualized)
	{
		shadow[layer] /= ((pcf * 2 + 1) * (pcf * 2 + 1));
	}
	else
	{
		shadow /= vec3(((pcf * 2 + 1) * (pcf * 2 + 1)));
	}

	float shadowDistance = distance(worldPosition.xyz, u_CameraPosition);
	float farPlaneEdge = u_Shadows.farPlane * (1.0 - u_Shadows.fog);
	if (shadowDistance > farPlaneEdge)
	{
		float shadowFog = clamp((shadowDistance - farPlaneEdge) / (u_Shadows.farPlane * u_Shadows.fog), 0.0, 1.0);
		shadow *= (1.0 - shadowFog);
	}

	return shadow;
}

void main()
{
	dirShadows = vec4(DirectionalShadowCompute(), 1.0);
}