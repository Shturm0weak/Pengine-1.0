#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 uvA;
layout(location = 3) in vec4 vertexColorA;
layout(location = 4) in vec3 v; // Position.
layout(location = 5) in vec3 s; // Scale.
layout(location = 6) in vec3 ambientAD;
layout(location = 7) in vec3 diffuseAD;
layout(location = 8) in vec3 specularAD;
layout(location = 9) in vec2 miscAD;
layout(location = 10) in mat4 rotationMat4AD;

mat4 a_ModelMat4 = mat4(
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	v.x, v.y, v.z, 1.0);
mat4 a_ScaleMat4 = mat4(
	s.x, 0.0, 0.0, 0.0,
	0.0, s.y, 0.0, 0.0,
	0.0, 0.0, s.z, 0.0,
	0.0, 0.0, 0.0, 1.0);

uniform mat4 u_ViewProjection;

out vec2 uv;
out vec3 normal;
out vec4 vertexColor;
out vec4 worldPosition;
out vec3 ambient;
out vec3 diffuse;
out vec3 specular;
out float shininess;
flat out int textureIndex;

void main()
{
	mat4 transform = a_ModelMat4 * rotationMat4AD * a_ScaleMat4;

	uv = uvA;
	normal = normalize(transpose(inverse(mat3(transform))) * normalA);
	vertexColor = vertexColorA;
	worldPosition = transform * vec4(positionA, 1.0);
	ambient = ambientAD;
	diffuse = diffuseAD;
	specular = specularAD;
	shininess = miscAD[1];
	textureIndex = int(miscAD[0]);

	gl_Position = u_ViewProjection * worldPosition;
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

struct PointLight
{
	vec3 position;
	vec3 color;
	float constant;
	float _linear;
	float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 color;
	vec3 direction;
	float constant;
	float _linear;
	float quadratic;
	float innerCutOff;
	float outerCutOff;
};

#define MAX_LIGHT 32
uniform int pointLightsSize;
uniform PointLight pointLights[MAX_LIGHT];
uniform int spotLightsSize;
uniform SpotLight spotLights[MAX_LIGHT];
uniform DirectionalLight u_DirectionalLight;
uniform vec3 u_CameraPosition;
uniform vec2 u_ViewportSize;
uniform sampler2D u_Texture[32];
uniform sampler2D u_CSM[3];
uniform mat4 u_LightSpaceMatricies[32];
uniform mat4 u_View;
uniform float u_CascadesDistance[32];
uniform int u_CascadesCount;
uniform int u_Pcf;
uniform bool u_ShadowsEnabled;
uniform bool u_ShadowsVisualized;
uniform float u_Bias;
uniform float u_FarPlane;
uniform float u_Texels;
uniform float u_Fog;
uniform bool u_IsShadowBlurEnabled;

in vec2 uv;
in vec3 normal;
in vec4 vertexColor;
in vec4 worldPosition;
in vec3 ambient;
in vec3 diffuse;
in vec3 specular;
in float shininess;
flat in int textureIndex;

vec3 viewDirection = normalize(u_CameraPosition - worldPosition.xyz);

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

vec3 DirectionalLightCompute()
{
	vec3 shadow = vec3(0.0);
	if (u_ShadowsEnabled)
	{
		if (u_IsShadowBlurEnabled)
		{
			shadow = texture(u_Texture[1], gl_FragCoord.xy / u_ViewportSize).rgb;
		}
		else
		{
			shadow = ShadowCompute();
		}
	}
	
	vec3 baseColor = texture(u_Texture[textureIndex], uv).xyz;
	vec3 ambient = 0.3 * u_DirectionalLight.color * ambient * baseColor;

	vec3 lightDirection = normalize(u_DirectionalLight.direction);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = u_DirectionalLight.color * (diffuseStrength * diffuse) * baseColor;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), shininess);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = u_DirectionalLight.color * (specular * specularf) * baseColor;

	return (ambient + (vec3(1.0) - shadow) * (diffuse + specular)) * u_DirectionalLight.intensity;
}

vec3 PointLightCompute(PointLight light)
{
	vec3 baseColor = texture(u_Texture[textureIndex], uv).xyz;
	vec3 ambient = light.color * ambient * baseColor;

	vec3 lightDirection = normalize(light.position - worldPosition.xyz);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.color * (diffuseStrength * diffuse) * baseColor;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), shininess);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = light.color * (specular * specularf) * baseColor;

	float distance = length(light.position - worldPosition.xyz);
	float attenuation = 1.0 / (light.constant + light._linear * distance +
		light.quadratic * (distance * distance));

	ambient *= attenuation;
	specular *= attenuation;
	diffuse *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 SpotLightCompute(SpotLight light)
{
	vec3 lightDirection = normalize(light.position - worldPosition.xyz);
	float theta = dot(lightDirection, normalize(-light.direction));

	if (theta > light.outerCutOff)
	{
		float epsilon = light.innerCutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		vec3 baseColor = texture(u_Texture[textureIndex], uv).xyz;
		vec3 ambient = light.color * ambient * baseColor;

		float diffuseStrength = max(dot(normal, lightDirection), 0.0);
		vec3 diffuse = light.color * (diffuseStrength * diffuse) * baseColor;

		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		float specularf = pow(max(dot(normal, halfwayDirection), 0.0), shininess);
		if (diffuseStrength == 0.0) specularf = 0.0;
		vec3 specular = light.color * (specular * specularf) * baseColor;

		float distance = length(light.position - worldPosition.xyz);
		float attenuation = 1.0 / (light.constant + light._linear * distance +
			light.quadratic * (distance * distance));

		ambient *= attenuation;
		specular *= attenuation;
		diffuse *= attenuation;

		ambient *= intensity;
		specular *= intensity;
		diffuse *= intensity;

		return (ambient + diffuse + specular);
	}
	else  // Use ambient light, so scene isn't completely dark outside the spotlight.
	{
		return vec3(0.0, 0.0, 0.0);
	}
}

void main()
{
	if (texture(u_Texture[textureIndex], uv).a <= 0.0)
	{
		discard;
	}

	vec3 result = DirectionalLightCompute();

	for (int i = 0; i < pointLightsSize; i++)
	{
		result += PointLightCompute(pointLights[i]);
	}

	for (int i = 0; i < spotLightsSize; i++)
	{
		result += SpotLightCompute(spotLights[i]);
	}

	fragColor = vec4(result, 1.0);
}