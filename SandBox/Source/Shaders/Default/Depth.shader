#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;

uniform mat4 u_TransformViewProjected;

void main()
{
	gl_Position = u_TransformViewProjected * vec4(positionA, 1.0);
}

#shader fragment
#version 330 core

void main()
{

}