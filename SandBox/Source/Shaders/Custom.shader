#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 uvA;
layout(location = 3) in vec3 tangentA;
layout(location = 4) in vec3 bitangentA;
layout(location = 5) in mat4 transformA;
layout(location = 9) in mat3 inverseTransformA;
layout(location = 12) in float materialIndexA;

#include "Source/Shaders/Default/Common/Uniforms.incl"
uniform mat4 u_ViewProjection;

out vec2 uv;
out vec3 n;
out vec4 worldPosition;
out mat3 TBN;
flat out int materialIndex;

void main()
{
	uv = uvA;

	n = normalize(inverseTransformA * normalA);
	vec3 tangent = normalize(inverseTransformA * tangentA);
	vec3 bitangent = normalize(inverseTransformA * bitangentA);
	TBN = (mat3(tangent, bitangent, n));

	worldPosition = transformA * vec4(positionA, 1.0);
	gl_Position = u_ViewProjection * worldPosition;

	materialIndex = int(materialIndexA);
}

#shader fragment
#version 420 core

layout(location = 0) out vec4 baseColor;
layout(location = 1) out vec4 positionColor;
layout(location = 2) out vec4 normalColor;

#include "Source/Shaders/Default/Common/UniformObjects.incl"
#include "Source/Shaders/Default/Common/Uniforms.incl"

#define MAX_TEXTURE_SLOTS 32
uniform sampler2D u_Texture[MAX_TEXTURE_SLOTS];

uniform float u_NoiseSpeed;
uniform float u_Distortion;
uniform float u_Width;
uniform float u_EmissionScale;
uniform sampler2D u_Noise;

in vec2 uv;
in vec3 n;
in vec4 worldPosition;
in mat3 TBN;
flat in int materialIndex;

vec3 normal = n;

vec4 rect(vec2 uv, float width)
{
	vec2 newUV = uv * 2.0 - 1.0;
	float color = float(newUV.x > -width && newUV.x < width);
	return vec4(color, color, color, color);
}

void main()
{
	Material material = u_Materials[materialIndex];

	vec2 noiseColor = texture(u_Noise, uv.xy + u_NoiseSpeed * u_Time).xy;

	vec2 transformUV = mix(uv, noiseColor, u_Distortion);

	vec4 rectColor = rect(transformUV, u_Width);
	baseColor = vec4(rectColor.xyz * material.ambient * u_EmissionScale, 1.0);
}