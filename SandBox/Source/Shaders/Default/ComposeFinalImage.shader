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
uniform sampler2D u_OutlineTexture;
uniform float u_Gamma;
uniform int u_OutlineThickness;
uniform vec3 u_OutlineColor;
uniform bool u_IsBloomEnabled;

in vec2 uv;

vec4 Outline()
{
    int w = u_OutlineThickness;

    if (texture(u_OutlineTexture, uv).rgb != vec3(1.0))
    {
        vec2 size = 1.0 / textureSize(u_OutlineTexture, 0);

        for (int i = -w; i <= +w; i++)
        {
            for (int j = -w; j <= +w; j++)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }

                vec2 offset = vec2(i, j) * size;

                if (texture(u_OutlineTexture, uv + offset).rgb == vec3(1.0))
                {
                    return vec4(u_OutlineColor, 1.0);
                }
            }
        }
    }

    return vec4(0.0, 0.0, 0.0, 0.0);
}

void main()
{
	vec4 sceneColor = texture(u_SceneTexture, uv);
	vec4 uiColor = texture(u_UITexture, uv);
    vec4 bloomColor = vec4(pow(texture(u_BloomTexture, uv).rgb, vec3(1.0 / u_Gamma)), 1.0);
    vec4 outlineColor = Outline();

	if (u_IsBloomEnabled)
	{
		sceneColor += bloomColor;
	}

    if (outlineColor.rgb != vec3(0.0))
    {
	    sceneColor  = outlineColor;
    }

	if (uiColor.a > 0)
	{
        sceneColor = uiColor;
	}

    fragColor = sceneColor;
}