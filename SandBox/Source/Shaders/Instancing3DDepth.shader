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

void main()
{
	gl_Position = u_ViewProjection * a_ModelMat4 * rotationMat4AD * a_ScaleMat4 * vec4(positionA, 1.0);
}


#shader fragment
#version 330 core

void main()
{

}