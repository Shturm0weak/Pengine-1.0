#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec2 uvA;
layout(location = 2) in vec4 colorA;
layout(location = 3) in vec4 transformMat1A;
layout(location = 4) in vec4 transformMat2A;
layout(location = 5) in vec4 transformMat3A;
layout(location = 6) in vec4 transformMat4A;
layout(location = 7) in float textureIndexA;

mat4 transform = mat4(
	transformMat1A,
	transformMat2A,
	transformMat3A,
	transformMat4A
);

uniform mat4 u_ViewProjectionMat4;

out vec2 uv;
out vec4 color;
flat out int textureIndex;

void main()
{
	gl_Position = u_ViewProjectionMat4 * transform * vec4(positionA, 1.0);
	uv = uvA;
	color = colorA;
	textureIndex = int(textureIndexA);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;

uniform sampler2D u_Texture[32];

in vec2 uv;
in vec4 color;
flat in int textureIndex;

void main()
{
	vec4 textureColor = texture(u_Texture[textureIndex], uv);
	if (textureColor.a < 0.05)
	{
		discard;
	}
	gl_FragColor = color * textureColor;
}