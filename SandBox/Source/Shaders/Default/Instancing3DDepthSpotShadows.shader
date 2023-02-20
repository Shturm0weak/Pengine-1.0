#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 5) in mat4 transformA;

uniform mat4 u_Projection;

void main()
{
    gl_Position = u_Projection * transformA * vec4(positionA, 1.0);
}

#shader fragment
#version 330 core

uniform vec3 u_LightPosition;
uniform float u_FarPlane;

in vec4 worldPosition;

void main()
{

}