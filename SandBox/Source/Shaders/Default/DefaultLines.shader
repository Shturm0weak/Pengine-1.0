#shader vertex
#version 420 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec4 colorA;

#include "Source/Shaders/Default/Common/UniformObjects.incl"

out vec4 color;

void main()
{
	gl_Position = u_GlobalUniforms.viewProjection * vec4(positionA, 1.0);
	color = colorA;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 fragColor;

in vec4 color;

void main()
{
	fragColor = color;
}