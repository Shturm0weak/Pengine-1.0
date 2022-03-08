#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec4 colorA;

uniform mat4 u_ViewProjectionMat4;

out vec4 color;

void main()
{
	gl_Position = u_ViewProjectionMat4 * vec4(positionA, 1.0);
	color = colorA;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;

in vec4 color;

void main()
{
	gl_FragColor = color;
}