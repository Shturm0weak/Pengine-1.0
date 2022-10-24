#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 uvA;
layout(location = 3) in vec3 tangentA;
layout(location = 4) in vec3 bitangentA;
layout(location = 5) in vec3 ambientAD;
layout(location = 6) in vec3 diffuseAD;
layout(location = 7) in vec3 specularAD;
layout(location = 8) in vec4 miscAD;
layout(location = 9) in mat4 transformA;
layout(location = 13) in mat3 inverseTransformA;

uniform mat4 u_ViewProjection;

out vec2 uv;
out vec3 n;
out vec4 worldPosition;
out vec3 ambient;
out mat3 TBN;
flat out int baseColorIndex;
flat out int normalMapIndex;
flat out int useNormalMap;

void main()
{
	uv = uvA;

	n = normalize(inverseTransformA * normalA);
	vec3 tangent = normalize(inverseTransformA * tangentA);
	vec3 bitangent = normalize(inverseTransformA * bitangentA);
	TBN = (mat3(tangent, bitangent, n));

	worldPosition = transformA * vec4(positionA, 1.0);

	ambient = ambientAD;
	baseColorIndex = int(miscAD[0]);
	normalMapIndex = int(miscAD[1]);
	useNormalMap = int(miscAD[3]);

	gl_Position = u_ViewProjection * worldPosition;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 baseColor;
layout(location = 1) out vec4 positionColor;
layout(location = 2) out vec4 normalColor;

uniform sampler2D u_Texture[32];

in vec2 uv;
in vec3 n;
in vec4 worldPosition;
in vec3 ambient;
in mat3 TBN;
flat in int baseColorIndex;
flat in int normalMapIndex;
flat in int useNormalMap;

vec3 normal = n;

void main()
{
	vec4 BaseColor = texture(u_Texture[baseColorIndex], uv);

	if (BaseColor.a < 0.5)
	{
		discard;
	}

	if (useNormalMap == 1)
	{
		normal = texture(u_Texture[normalMapIndex], uv).xyz;
		normal *= normal * 2.0 - 1.0;
		normal = normalize(TBN * normal);
	}

	baseColor = BaseColor * vec4(ambient, 1.0);
	positionColor = worldPosition;
	normalColor = vec4(normal, 1.0);
}