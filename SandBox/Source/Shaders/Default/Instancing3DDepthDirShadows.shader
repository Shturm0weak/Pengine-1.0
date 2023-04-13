#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 7) in mat4 transformA;

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