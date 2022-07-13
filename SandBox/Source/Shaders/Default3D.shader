#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec2 normalA;
layout(location = 2) in vec2 uvA;
layout(location = 3) in vec4 vertexColorA;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 uv;
out vec4 vertexColor;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(positionA, 1.0);
	uv = uvA;
	vertexColor = vertexColorA;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 fragColor;

uniform sampler2D u_Diffuse;
uniform vec4 u_Color;

in vec2 uv;
in vec4 vertexColor;

void main()
{
	vec4 textureColor = texture(u_Diffuse, uv);
	
	fragColor = u_Color * vertexColor * textureColor;
}