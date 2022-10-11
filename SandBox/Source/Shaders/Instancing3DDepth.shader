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

void main()
{
	gl_Position = u_ViewProjection * transformA * vec4(positionA, 1.0);
}


#shader fragment
#version 330 core

void main()
{

}