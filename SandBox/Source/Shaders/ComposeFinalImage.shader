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
#version 330 core

layout(location = 0) out vec4 fragColor;

uniform sampler2D u_SceneTexture;
uniform sampler2D u_UITexture;
uniform sampler2D u_BloomTexture;
uniform float u_Gamma;
uniform bool u_IsBloomEnabled;

in vec2 uv;

void main()
{
	vec4 sceneColor = texture(u_SceneTexture, uv);
	vec4 uiColor = texture(u_UITexture, uv);
    vec4 bloomColor = texture(u_BloomTexture, uv);

	bloomColor = vec4(pow(bloomColor.rgb, vec3(1.0 / u_Gamma)), 1.0);

	if (u_IsBloomEnabled)
	{
		sceneColor += bloomColor;
	}

	fragColor = sceneColor;

	if (uiColor.a > 0)
	{
		fragColor = uiColor;
	}
}