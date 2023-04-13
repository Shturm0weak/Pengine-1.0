#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec2 uvA;

out vec2 uv;

void main()
{
	uv = uvA;
	gl_Position = vec4(positionA * 2.0, 1.0);
}

#shader fragment
#version 420 core

layout(location = 0) out vec4 dirShadows;

#include "Source/Shaders/Default/Common/UniformObjects.incl"
#include "Source/Shaders/Default/Common/Uniforms.incl"

uniform sampler2D u_WorldPosition;
uniform sampler2D u_Normal;

in vec2 uv;

#include "Source/Shaders/Default/Common/DirectionalShadows.incl"

void main()
{
	vec4 worldPosition = texture(u_WorldPosition, uv);
	vec3 normal = texture(u_Normal, uv).xyz;

	dirShadows = vec4(DirectionalShadowCompute(normal, worldPosition), 1.0);
}