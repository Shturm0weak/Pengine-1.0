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
uniform sampler2D u_Shading;
uniform sampler2D u_SkyBox;

in vec2 uv;

vec4 worldPosition = texture(u_WorldPosition, uv);
vec3 normal = texture(u_Normal, uv).xyz;
vec3 albedo = texture(u_Albedo, uv).xyz;
vec3 shading = texture(u_Shading, uv).xyz;
vec3 skybox = texture(u_SkyBox, uv).xyz;
vec3 shadow = vec3(0.0);
vec3 basicReflectivity = mix(vec3(0.05), albedo, shading.x);
vec3 viewDirection = normalize(u_GlobalUniforms.cameraPosition - worldPosition.xyz);

float ssao = texture(u_SSAO.color, uv).x;
float ao = shading.z;
float roughness = shading.y;
float metallic = shading.x;

#include "Source/Shaders/Default/Common/CommonPBR.incl"
#include "Source/Shaders/Default/Common/DirectionalLight.incl"
#include "Source/Shaders/Default/Common/PointShadows.incl"
#include "Source/Shaders/Default/Common/PointLight.incl"
#include "Source/Shaders/Default/Common/SpotShadows.incl"
#include "Source/Shaders/Default/Common/SpotLight.incl"

void main()
{
	/*if (length(albedo) > EMISSIVE_THRESHOLD)
	{
		fragColor = vec4(albedo, 1.0);
		return;
	}*/

	if (u_SSAO.isEnabled)
	{
		albedo *= ssao;
	}

	if (u_DirectionalLight.isEnabled && u_Shadows.isEnabled)
	{
		shadow = texture(u_DirectionalLight.shadowColor, gl_FragCoord.xy / u_GlobalUniforms.viewportSize).rgb;
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

	if(normal.x + normal.y + normal.z == 0.0)
	{
		result = skybox;
	}

	fragColor = vec4(result, 1.0);
}