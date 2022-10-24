#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 uvA;
layout(location = 3) in vec3 tangentA;
layout(location = 4) in vec3 bitangentA;

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

struct Material
{
	sampler2D baseColor;
	sampler2D normalMap;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	float solid;
	int useNormalMap;
};

#define MAX_LIGHT 32
uniform int pointLightsSize;
uniform PointLight pointLights[MAX_LIGHT];
uniform int spotLightsSize;
uniform SpotLight spotLights[MAX_LIGHT];
uniform DirectionalLight u_DirectionalLight;
uniform vec3 u_CameraPosition;
uniform Material u_Material;

in vec2 uv;
in vec3 n;
in vec4 worldPosition;
in mat3 TBN;

vec3 viewDirection = normalize(u_CameraPosition - worldPosition.xyz);
vec4 baseColorRGBA = texture(u_Material.baseColor, uv);
vec3 baseColor = baseColorRGBA.xyz;
vec3 normal = n;

vec3 DirectionalLightCompute()
{
	vec3 ambient = 0.3 * u_DirectionalLight.color * u_Material.ambient * baseColor;

	vec3 lightDirection = normalize(u_DirectionalLight.direction);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = u_DirectionalLight.color * (diffuseStrength * u_Material.diffuse) * baseColor;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), u_Material.shininess);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = u_DirectionalLight.color * (u_Material.specular * specularf) * baseColor;

	return (ambient + diffuse + specular) * u_DirectionalLight.intensity;
}

vec3 PointLightCompute(PointLight light)
{
	vec3 ambient = light.color * u_Material.ambient * baseColor;

	vec3 lightDirection = normalize(light.position - worldPosition.xyz);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.color * (diffuseStrength * u_Material.diffuse) * baseColor;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), u_Material.shininess);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = light.color * (u_Material.specular * specularf) * baseColor;

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

		vec3 ambient = light.color * u_Material.ambient * baseColor;

		float diffuseStrength = max(dot(normal, lightDirection), 0.0);
		vec3 diffuse = light.color * (diffuseStrength * u_Material.diffuse) * baseColor;

		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		float specularf = pow(max(dot(normal, halfwayDirection), 0.0), u_Material.shininess);
		if (diffuseStrength == 0.0) specularf = 0.0;
		vec3 specular = light.color * (u_Material.specular * specularf) * baseColor;

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
	if (baseColorRGBA.a < 0.5)
	{
		discard;
	}

	if (u_Material.useNormalMap == 1)
	{
		normal = texture(u_Material.normalMap, uv).xyz;
		normal *= normal * 2.0 - 1.0;
		normal = normalize(TBN * normal);
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

	fragColor = vec4(result, u_Material.solid);
}