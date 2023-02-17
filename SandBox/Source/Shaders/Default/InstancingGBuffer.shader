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

in vec2 uv;
in vec3 n;
in vec4 worldPosition;
in mat3 TBN;
flat in int materialIndex;

vec3 normal = n;

void main()
{
	Material material = u_Materials[materialIndex];

	vec4 BaseColor = texture(u_Texture[material.baseColor], uv);

	if (BaseColor.a < 0.5)
	{
		discard;
	}

	if (material.useNormalMap > 0.0)
	{
		normal = texture(u_Texture[material.normalColor], uv).xyz;
		normal *= normal * 2.0 - 1.0;
		normal = normalize(TBN * normal);
	}

	baseColor = BaseColor * vec4(material.ambient, 1.0);
	positionColor = worldPosition;
	normalColor = vec4(normal, 1.0);
}