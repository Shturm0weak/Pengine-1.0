#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec2 normalA;
layout(location = 1) in vec2 uvA;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 uv;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(positionA, 1.0);
	uv = uvA;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;

uniform sampler2D u_Diffuse;
uniform vec4 u_Color;

in vec2 uv;

void main()
{
	vec4 textureColor = texture(u_Diffuse, uv);
	
	gl_FragColor = u_Color * textureColor;
}