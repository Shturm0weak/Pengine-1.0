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

struct Shadows
{
	sampler2D color;
	bool isEnabled;
};

struct SSAO
{
	sampler2D color;
	bool isEnabled;
};

#define MAX_LIGHT 32
uniform int pointLightsSize;
uniform PointLight pointLights[MAX_LIGHT];
uniform int spotLightsSize;
uniform SpotLight spotLights[MAX_LIGHT];
uniform DirectionalLight u_DirectionalLight;
uniform Shadows u_Shadows;
uniform SSAO u_SSAO;
uniform vec3 u_CameraPosition;
uniform vec2 u_ViewportSize;
uniform sampler2D u_Texture[32];

uniform sampler2D u_WorldPosition;
uniform sampler2D u_Normal;
uniform sampler2D u_Albedo;

in vec2 uv;

vec4 worldPosition = texture(u_WorldPosition, uv);
vec3 normal = texture(u_Normal, uv).xyz;
vec3 albedo = texture(u_Albedo, uv).xyz;
vec3 shadow = vec3(0.0);

float ssao = texture(u_SSAO.color, uv).x;

vec3 viewDirection = normalize(u_CameraPosition - worldPosition.xyz);

vec3 DirectionalLightCompute()
{
	vec3 ambient = 0.3 * u_DirectionalLight.color;

	if (u_SSAO.isEnabled)
	{
		ambient *= ssao;
	}

	vec3 lightDirection = normalize(u_DirectionalLight.direction);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = u_DirectionalLight.color * diffuseStrength;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), 32.0);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = u_DirectionalLight.color * (specularf);

	return (ambient + (vec3(1.0) - shadow) * (diffuse + specular)) * u_DirectionalLight.intensity * albedo;
}

vec3 PointLightCompute(PointLight light)
{
	vec3 ambient = light.color;

	if (u_SSAO.isEnabled)
	{
		ambient *= ssao;
	}

	vec3 lightDirection = normalize(light.position - worldPosition.xyz);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.color * diffuseStrength;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), 32);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = light.color * specularf;

	float distance = length(light.position - worldPosition.xyz);
	float attenuation = 1.0 / (light.constant + light._linear * distance +
		light.quadratic * (distance * distance));

	ambient *= attenuation;
	specular *= attenuation;
	diffuse *= attenuation;

	return (ambient + (vec3(1.0) - shadow) * (diffuse + specular)) * albedo;
}

vec3 SpotLightCompute(SpotLight light)
{
	vec3 lightDirection = normalize(light.position - worldPosition.xyz);
	float theta = dot(lightDirection, normalize(-light.direction));

	if (theta > light.outerCutOff)
	{
		float epsilon = light.innerCutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		vec3 ambient = light.color;

		if (u_SSAO.isEnabled)
		{
			ambient *= ssao;
		}

		float diffuseStrength = max(dot(normal, lightDirection), 0.0);
		vec3 diffuse = light.color * diffuseStrength;

		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		float specularf = pow(max(dot(normal, halfwayDirection), 0.0), 32);
		if (diffuseStrength == 0.0) specularf = 0.0;
		vec3 specular = light.color * specularf;

		float distance = length(light.position - worldPosition.xyz);
		float attenuation = 1.0 / (light.constant + light._linear * distance +
			light.quadratic * (distance * distance));

		ambient *= attenuation;
		specular *= attenuation;
		diffuse *= attenuation;

		ambient *= intensity;
		specular *= intensity;
		diffuse *= intensity;

		return (ambient + (vec3(1.0) - shadow) * (diffuse + specular)) * albedo;
	}
	else  // Use ambient light, so scene isn't completely dark outside the spotlight.
	{
		return vec3(0.0, 0.0, 0.0);
	}
}

void main()
{
	if (u_Shadows.isEnabled)
	{
		shadow = texture(u_Shadows.color, gl_FragCoord.xy / u_ViewportSize).rgb;
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

	fragColor = vec4(vec3(result), 1.0);
}