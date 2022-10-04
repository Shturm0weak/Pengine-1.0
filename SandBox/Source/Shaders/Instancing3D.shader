#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 uvA;
layout(location = 3) in vec4 vertexColorA;
layout(location = 4) in vec3 v; // Position.
layout(location = 5) in vec3 s; // Scale.
layout(location = 6) in vec3 ambientAD;
layout(location = 7) in vec3 diffuseAD;
layout(location = 8) in vec3 specularAD;
layout(location = 9) in vec2 miscAD;
layout(location = 10) in mat4 rotationMat4AD;

mat4 a_ModelMat4 = mat4(
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	v.x, v.y, v.z, 1.0);
mat4 a_ScaleMat4 = mat4(
	s.x, 0.0, 0.0, 0.0,
	0.0, s.y, 0.0, 0.0,
	0.0, 0.0, s.z, 0.0,
	0.0, 0.0, 0.0, 1.0);

uniform mat4 u_ViewProjection;

out vec2 uv;
out vec3 normal;
out vec4 vertexColor;
out vec4 worldPosition;
out vec3 ambient;
out vec3 diffuse;
out vec3 specular;
out float shininess;
flat out int textureIndex;

void main()
{
	mat4 transform = a_ModelMat4 * rotationMat4AD * a_ScaleMat4;

	uv = uvA;
	normal = normalize(transpose(inverse(mat3(transform))) * normalA);
	vertexColor = vertexColorA;
	worldPosition = transform * vec4(positionA, 1.0);
	ambient = ambientAD;
	diffuse = diffuseAD;
	specular = specularAD;
	shininess = miscAD[1];
	textureIndex = int(miscAD[0]);

	gl_Position = u_ViewProjection * worldPosition;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 fragColor;

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float intensity;
};

uniform DirectionalLight u_DirectionalLight;
uniform vec3 u_CameraPosition;
uniform sampler2D u_Texture[32];

in vec2 uv;
in vec3 normal;
in vec4 vertexColor;
in vec4 worldPosition;
in vec3 ambient;
in vec3 diffuse;
in vec3 specular;
in float shininess;
flat in int textureIndex;

vec3 viewDirection = normalize(u_CameraPosition - worldPosition.xyz);

vec3 DirectionalLightCompute()
{
	vec3 baseColor = texture(u_Texture[textureIndex], uv).xyz;
	vec3 ambient = u_DirectionalLight.color * ambient * baseColor;

	vec3 lightDirection = normalize(u_DirectionalLight.direction);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = u_DirectionalLight.color * (diffuseStrength * diffuse) * baseColor;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), shininess);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = u_DirectionalLight.color * (specular * specularf) * baseColor;

	return (ambient + diffuse + specular) * u_DirectionalLight.intensity;
}

void main()
{
	fragColor = vec4(DirectionalLightCompute().xyz, 1.0);
}