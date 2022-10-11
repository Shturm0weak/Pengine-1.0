#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 uvA;
layout(location = 3) in vec4 vertexColorA;
layout(location = 4) in vec3 ambientAD;
layout(location = 5) in vec3 diffuseAD;
layout(location = 6) in vec3 specularAD;
layout(location = 7) in vec2 miscAD;
layout(location = 8) in mat4 transformA;

uniform mat4 u_ViewProjection;

out vec2 uv;
out vec3 normal;
out vec4 vertexColor;
out vec4 worldPosition;
out vec3 ambient;
flat out int textureIndex;

void main()
{
	uv = uvA;
	normal = normalize(transpose(inverse(mat3(transformA))) * normalA);
	vertexColor = vertexColorA;
	worldPosition = transformA * vec4(positionA, 1.0);
	ambient = ambientAD;
	textureIndex = int(miscAD[0]);

	gl_Position = u_ViewProjection * worldPosition;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 baseColor;
layout(location = 1) out vec4 positionColor;
layout(location = 2) out vec4 normalColor;

uniform sampler2D u_Texture[32];

in vec2 uv;
in vec3 normal;
in vec4 vertexColor;
in vec4 worldPosition;
in vec3 ambient;
flat in int textureIndex;

void main()
{
	vec4 textureColor = texture(u_Texture[textureIndex], uv);
	if (textureColor.a <= 0.0)
	{
		discard;
	}

	baseColor = textureColor * vec4(ambient, 1.0) * vertexColor;
	positionColor = worldPosition;
	normalColor = vec4(normal, 1.0);
}