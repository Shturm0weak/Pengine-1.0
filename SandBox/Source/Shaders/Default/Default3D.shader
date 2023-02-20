#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 uvA;
layout(location = 3) in vec3 tangentA;
layout(location = 4) in vec3 bitangentA;

#include "Source/Shaders/Default/Common/Uniforms.incl"

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat3 u_InverseTransform;

out vec2 uv;
out vec3 n;
out vec4 worldPosition;
out mat3 TBN;

void main()
{
	uv = uvA;

	n = normalize(u_InverseTransform * normalA);
	vec3 tangent = normalize(u_InverseTransform * tangentA);
	vec3 bitangent = normalize(u_InverseTransform * bitangentA);
	TBN = (mat3(tangent, bitangent, n));

	worldPosition = u_Transform * vec4(positionA, 1.0);
	gl_Position = u_ViewProjection * worldPosition;
}

#shader fragment
#version 420 core

layout(location = 0) out vec4 fragColor;

#include "Source/Shaders/Default/Common/UniformObjects.incl"
#include "Source/Shaders/Default/Common/Uniforms.incl"

uniform int u_MaterialIndex;
uniform sampler2D u_BaseColor;
uniform sampler2D u_NormalColor;

in vec2 uv;
in vec3 n;
in vec4 worldPosition;
in mat3 TBN;

vec3 viewDirection = normalize(u_CameraPosition - worldPosition.xyz);
vec3 normal = n;
Material material = u_Materials[u_MaterialIndex];
vec4 baseColorRGBA = texture(u_BaseColor, uv) * vec4(material.ambient, 1.0);
vec3 baseColor = baseColorRGBA.xyz;
vec3 shadow = vec3(0.0);

#include "Source/Shaders/Default/Common/DirectionalShadows.incl"

vec3 DirectionalLightCompute()
{
	vec3 ambient = 0.3 * u_DirectionalLight.color;

	vec3 lightDirection = normalize(u_DirectionalLight.direction);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = u_DirectionalLight.color * diffuseStrength;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), material.shiness);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = u_DirectionalLight.color * (specularf);

	return (ambient + (vec3(1.0) - shadow) * (diffuse + specular)) * u_DirectionalLight.intensity * baseColor;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
	vec3(1.0, 1.0, 1.0), vec3(1.0, -1.0, 1.0), vec3(-1.0, -1.0, 1.0), vec3(-1.0, 1.0, 1.0),
	vec3(1.0, 1.0, -1.0), vec3(1.0, -1.0, -1.0), vec3(-1.0, -1.0, -1.0), vec3(-1.0, 1.0, -1.0),
	vec3(1.0, 1.0, 0.0), vec3(1.0, -1.0, 0.0), vec3(-1.0, -1.0, 0.0), vec3(-1.0, 1.0, 0.0),
	vec3(1.0, 0.0, 1.0), vec3(-1.0, 0.0, 1.0), vec3(1.0, 0.0, -1.0), vec3(-1.0, 0.0, -1.0),
	vec3(0.0, 1.0, 1.0), vec3(0.0, -1.0, 1.0), vec3(0.0, -1.0, -1.0), vec3(0.0, 1.0, -1.0)
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
	vec3 pointLightShadow = vec3(0.0);
	for (int i = 0; i < samples; i++)
	{
		float closestDepth = texture(u_PointLightsShadowMap[light.shadowMapIndex], toLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= light.farPlane;
		if (currentDepth - bias > closestDepth)
		{
			pointLightShadow += 1.0;
		}
	}
	pointLightShadow /= float(samples);

	if (currentDepth > light.farPlane || dot(normalize(toLight), normal) > 0.0)
	{
		return vec3(0.0);
	}

	pointLightShadow *= (1.0 - (currentDepth / light.farPlane));
	pointLightShadow = clamp(pointLightShadow, 0.1, 1.0);

	float farPlaneEdge = light.farPlane * (1.0 - light.fog);
	if (distanceToCamera > farPlaneEdge)
	{
		float shadowFog = clamp((distanceToCamera - farPlaneEdge) / (light.farPlane * light.fog), 0.0, 1.0);
		pointLightShadow *= (1.0 - shadowFog);
	}

	return vec3(pointLightShadow);
}


vec3 SpotShadowCompute(SpotLight light)
{
	float shadow = 0.0f;
	vec4 lightSpacePosition = u_SpotLightShadowMapProjections[light.shadowMapIndex] * worldPosition;
	vec3 projectedCoords = lightSpacePosition.xyz / lightSpacePosition.w;

	if (projectedCoords.z <= 1.0f)
	{
		projectedCoords = (projectedCoords + 1.0f) / 2.0f;
		float currentDepth = projectedCoords.z;

		float bias = max(light.shadowBias * (1.0f - dot(normal, light.direction)), 0.000005f);

		vec2 pixelSize = 1.0 / textureSize(u_SpotLightsShadowMap[light.shadowMapIndex], 0);
		for (int y = -light.pcf; y <= light.pcf; y++)
		{
			for (int x = -light.pcf; x <= light.pcf; x++)
			{
				float closestDepth = texture(u_SpotLightsShadowMap[light.shadowMapIndex], projectedCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;
			}
		}

		shadow /= ((light.pcf * 2 + 1) * (light.pcf * 2 + 1));

		vec3 toLight = worldPosition.xyz - light.position;
		vec3 toCamera = worldPosition.xyz - u_CameraPosition;
		float distanceToCamera = length(toCamera);

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
	}

	return vec3(shadow);
}

vec3 PointLightCompute(PointLight light)
{
	vec3 pointLightShadow = vec3(0.0);

	if (u_IsShadowsEnabled && light.drawShadows == 1)
	{
		pointLightShadow = PointShadowCompute(light);
	}

	vec3 ambient = light.color;

	vec3 lightDirection = normalize(light.position - worldPosition.xyz);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.color * diffuseStrength;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), material.shiness);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = light.color * specularf;

	float distance = length(light.position - worldPosition.xyz);
	float attenuation = 1.0 / (light.constant + light._linear * distance +
		light.quadratic * (distance * distance));

	ambient *= attenuation;
	specular *= attenuation;
	diffuse *= attenuation;

	return (ambient + (vec3(1.0) - pointLightShadow) * (diffuse + specular)) * baseColor;
}

vec3 SpotLightCompute(SpotLight light)
{
	vec3 shadow = vec3(0.0);

	if (u_IsShadowsEnabled && light.drawShadows == 1)
	{
		shadow = SpotShadowCompute(light);
	}

	vec3 lightDirection = normalize(light.position - worldPosition.xyz);
	float theta = dot(lightDirection, normalize(-light.direction));

	if (theta > light.outerCutOff)
	{
		float epsilon = light.innerCutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		vec3 ambient = light.color;

		float diffuseStrength = max(dot(normal, lightDirection), 0.0);
		vec3 diffuse = light.color * diffuseStrength;

		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		float specularf = pow(max(dot(normal, halfwayDirection), 0.0), material.shiness);
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

		return ((vec3(1.0) - shadow) * (diffuse + specular)) * baseColor;
	}
	else  // Use ambient light, so scene isn't completely dark outside the spotlight.
	{
		return vec3(0.0, 0.0, 0.0);
	}
}

void main()
{
	if (baseColorRGBA.a < 0.01)
	{
		discard;
	}

	if (u_DirectionalShadows.isEnabled && u_IsShadowsEnabled)
	{
		shadow = DirectionalShadowCompute();
	}

	if (material.useNormalMap > 0.0)
	{
		normal = texture(u_NormalColor, uv).xyz;
		normal *= normal * 2.0 - 1.0;
		normal = normalize(TBN * normal);
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

	fragColor = vec4(result, material.alpha);
}