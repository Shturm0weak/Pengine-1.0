#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 uvA;
layout(location = 3) in vec4 vertexColorA;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 uv;
out vec3 normal;
out vec4 vertexColor;
out vec4 worldPosition;

void main()
{
	uv = uvA;
	normal = transpose(inverse(mat3(u_Transform))) * normalA;
	vertexColor = vertexColorA;
	worldPosition = u_Transform * vec4(positionA, 1.0);
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

struct Material {
	sampler2D baseColor;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	float solid;
};

uniform DirectionalLight u_DirectionalLight;
uniform Material u_Material;
uniform vec3 u_CameraPosition;

in vec2 uv;
in vec3 normal;
in vec4 vertexColor;
in vec4 worldPosition;

vec3 viewDirection = normalize(u_CameraPosition - worldPosition.xyz);

vec3 DirectionalLightCompute()
{
	vec3 ambient = u_DirectionalLight.color * u_Material.ambient;

	vec3 lightDirection = normalize(u_DirectionalLight.direction);
	float diffuseStrength = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = u_DirectionalLight.color * (diffuseStrength * u_Material.diffuse);

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularf = pow(max(dot(normal, halfwayDirection), 0.0), u_Material.shininess);
	if (diffuseStrength == 0.0) specularf = 0.0;
	vec3 specular = u_DirectionalLight.color * (u_Material.specular * specularf);

	return (ambient + diffuse + specular) * u_DirectionalLight.intensity;
}

void main()
{
	//vec4 textureColor = texture(u_Material.baseColor, uv);
	
	//fragColor = u_Material.ambient * vertexColor * textureColor;
	fragColor = vec4(DirectionalLightCompute().xyz, u_Material.solid);
}