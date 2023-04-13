#shader vertex
#version 420 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 uvA;
layout(location = 3) in vec3 tangentA;
layout(location = 4) in vec3 bitangentA;
layout(location = 5) in vec4 weightsA;
layout(location = 6) in vec4 bonesIdsA;

#include "Source/Shaders/Default/Common/UniformObjects.incl"

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
	gl_Position = u_GlobalUniforms.viewProjection * worldPosition;
}

#shader fragment
#version 420 core

layout(location = 0) out vec4 fragColor;

#include "Source/Shaders/Default/Common/UniformObjects.incl"
#include "Source/Shaders/Default/Common/Uniforms.incl"

uniform int u_MaterialIndex;
uniform sampler2D u_BaseColor;
uniform sampler2D u_NormalColor;
uniform sampler2D u_MetallicColor;
uniform sampler2D u_RoughnessColor;
uniform sampler2D u_AoColor;

in vec2 uv;
in vec3 n;
in vec4 worldPosition;
in mat3 TBN;

vec3 viewDirection = normalize(u_GlobalUniforms.cameraPosition - worldPosition.xyz);
vec3 normal = n;
vec3 shadow = vec3(0.0);
Material material = u_Materials[u_MaterialIndex];
vec2 transformedUv = uv * material.uvTransform.xy + material.uvTransform.zw;
vec4 albedoRGBA = texture(u_BaseColor, transformedUv) * vec4(material.albedo * material.intensity, 1.0);
vec3 albedo = albedoRGBA.xyz;

vec3 basicReflectivity;
float ao;
float roughness;
float metallic;

#include "Source/Shaders/Default/Common/CommonPBR.incl"
#include "Source/Shaders/Default/Common/DirectionalShadows.incl"
#include "Source/Shaders/Default/Common/DirectionalLight.incl"
#include "Source/Shaders/Default/Common/PointShadows.incl"
#include "Source/Shaders/Default/Common/PointLight.incl"
#include "Source/Shaders/Default/Common/SpotShadows.incl"
#include "Source/Shaders/Default/Common/SpotLight.incl"

void main()
{
	if (albedoRGBA.a < 0.01)
	{
		discard;
	}

	if (length(albedo * material.albedo) > EMISSIVE_THRESHOLD)
	{
		fragColor = vec4(albedo * material.albedo * material.intensity, material.alpha);
		return;
	}

	if (material.useNormalMap > 0.0)
	{
		normal = texture(u_NormalColor, transformedUv).xyz;
		normal *= normal * 2.0 - 1.0;
		normal = normalize(TBN * normal);
	}

	if (u_DirectionalLight.isEnabled && u_Shadows.isEnabled)
	{
		shadow = DirectionalShadowCompute(normal, worldPosition);
	}

	if (material.useMetallicMap > 0.5)
	{
		metallic = texture(u_MetallicColor, transformedUv).r;
	}
	else
	{
		metallic = material.metallic;
	}

	if (material.useRoughnessMap > 0.5)
	{
		roughness = texture(u_RoughnessColor, transformedUv).r;
	}
	else
	{
		roughness = material.roughness;
	}

	ao = texture(u_AoColor, transformedUv).r;

	basicReflectivity = mix(vec3(0.05), albedo, metallic);

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