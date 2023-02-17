#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec2 uvA;
layout(location = 2) in vec4 colorA;
layout(location = 3) in vec4 transformMat1A;
layout(location = 4) in vec4 transformMat2A;
layout(location = 5) in vec4 transformMat3A;
layout(location = 6) in vec4 transformMat4A;
layout(location = 7) in vec4 additionalData0A;
layout(location = 8) in vec4 additionalData1A;
layout(location = 9) in vec4 textureDataA;

mat4 transform = mat4(
	transformMat1A,
	transformMat2A,
	transformMat3A,
	transformMat4A
);

uniform mat4 u_ViewProjectionMat4;

out vec2 uv;
out vec4 color;
out vec4 worldPixelPosition;
flat out vec4 worldPosition;
flat out vec4 additionalData0;
flat out vec4 additionalData1;
flat out vec4 textureData;

void main()
{
	worldPosition = transform[3].xyzw;
	worldPixelPosition = transform * vec4(positionA, 1.0);
	gl_Position = u_ViewProjectionMat4 * worldPixelPosition;
	uv = uvA;
	color = colorA;
	additionalData0 = additionalData0A;
	additionalData1 = additionalData1A;
	textureData = textureDataA;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 fragColor;

struct PointLight {
	vec3 position;
	vec3 color;
	float constant;
	float _linear;
	float quadratic;
};

#define MAX_LIGHT 32
uniform int pointLightSize;
uniform PointLight pointLights[MAX_LIGHT];

uniform float u_GlobalIntensity;
uniform sampler2D u_Texture[32];
//uniform vec2 u_Resolution;

in vec2 uv;
in vec4 color;
in vec4 worldPixelPosition;
flat in vec4 worldPosition;
flat in vec4 additionalData0;
flat in vec4 additionalData1;
flat in vec4 textureData;

bool RenderCircle(float innerRadius, float outerRadius)
{
	float distance = sqrt(pow(worldPixelPosition.x - worldPosition.x, 2) + pow(worldPixelPosition.y - worldPosition.y, 2)
		+ pow(worldPixelPosition.z - worldPosition.z, 2));
	if (distance < additionalData0.y && distance > additionalData0.x)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void main()
{
	if ((additionalData0.x > 0.0 || additionalData0.y > 0.0) && !RenderCircle(additionalData0.x, additionalData0.y))
	{
		discard;
	}

	vec4 textureColor = texture(u_Texture[int(textureData[0])], uv);
	if (textureColor.a < 0.05)
	{
		discard;
	}

	fragColor = textureColor * color;
}