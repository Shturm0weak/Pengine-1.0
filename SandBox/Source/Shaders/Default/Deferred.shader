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
#version 420 core

layout(location = 0) out vec4 fragColor;

#include "Source/Shaders/Default/Common/UniformObjects.incl"
#include "Source/Shaders/Default/Common/Uniforms.incl"

uniform SSAO u_SSAO;

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
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), 32.0f);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = u_DirectionalLight.color * (specularf);

	return (ambient + (vec3(1.0) - shadow) * (diffuse + specular)) * u_DirectionalLight.intensity * albedo;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
	vec3(1.0, 1.0, 1.0), vec3( 1.0, -1.0,  1.0), vec3(-1.0, -1.0,  1.0), vec3(-1.0, 1.0,  1.0),
	vec3(1.0, 1.0, -1.0), vec3(1.0, -1.0, -1.0), vec3(-1.0, -1.0, -1.0), vec3(-1.0, 1.0, -1.0),
	vec3(1.0, 1.0, 0.0), vec3( 1.0, -1.0,  0.0), vec3(-1.0, -1.0,  0.0), vec3(-1.0, 1.0,  0.0),
	vec3(1.0, 0.0, 1.0), vec3(-1.0,  0.0,  1.0), vec3( 1.0,  0.0, -1.0), vec3(-1.0, 0.0, -1.0),
	vec3(0.0, 1.0, 1.0), vec3( 0.0, -1.0,  1.0), vec3( 0.0, -1.0, -1.0), vec3( 0.0, 1.0, -1.0)
);

vec3 PointShadowCompute(PointLight light)
{
	vec3 toLight = worldPosition.xyz - light.position;
	vec3 toCamera = worldPosition.xyz - u_CameraPosition;
	float distanceToCamera = length(toCamera);
	float currentDepth = length(toLight);
	float bias = 0.01;
	float diskRadius = (1.0 + (distanceToCamera / light.farPlane)) / 100.0;
	int samples = 20;
	for (int i = 0; i < samples; i++)
	{
		float closestDepth = texture(u_PointLightsShadowMap[light.shadowMapIndex], toLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= light.farPlane; 
		if (currentDepth - bias > closestDepth)
		{
			shadow += 1.0;
		}
	}
	shadow /= float(samples);

	if (currentDepth > light.farPlane || dot(normalize(toLight), normal) > 0.0)
	{
		return vec3(0.0);
	}

	shadow *= (1.0 - (currentDepth / light.farPlane));
	shadow = clamp(shadow, 0.1, 1.0);

	float farPlaneEdge = light.farPlane * (1.0 - light.fog);
	if (distanceToCamera > farPlaneEdge)
	{
		float shadowFog = clamp((distanceToCamera - farPlaneEdge) / (light.farPlane * light.fog), 0.0, 1.0);
		shadow *= (1.0 - shadowFog);
	}

	return vec3(shadow);
}

vec3 PointLightCompute(PointLight light)
{
	vec3 shadow = vec3(0.0);

	if (u_IsShadowsEnabled && light.drawShadows == 1)
	{
		shadow = PointShadowCompute(light);
	}

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

		return (ambient + (diffuse + specular)) * albedo;
	}
	else  // Use ambient light, so scene isn't completely dark outside the spotlight.
	{
		return vec3(0.0, 0.0, 0.0);
	}
}

void main()
{
	if (u_DirectionalShadows.isEnabled && u_IsShadowsEnabled)
	{
		shadow = texture(u_DirectionalShadows.color, gl_FragCoord.xy / u_ViewportSize).rgb;
	}

	vec3 result = DirectionalLightCompute();

	for (int i = 0; i < u_PointLightsSize; i++)
	{
		result += PointLightCompute(u_PointLights[i]);
	}

	for (int i = 0; i < u_SpotLightsSize; i++)
	{
		result += SpotLightCompute(u_SpotLights[i]);
	}

	fragColor = vec4(vec3(result), 1.0);
}