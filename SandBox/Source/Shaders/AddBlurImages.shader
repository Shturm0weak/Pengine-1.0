#shader vertex
#version 400 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec2 uvA;

out vec2 uv;

void main()
{
    uv = uvA;
    gl_Position = vec4(positionA * 2.0, 1.0);
}

#shader fragment
#version 400 core

layout(location = 0) out vec4 fragColor;

in vec2 uv;

uniform int u_BlurTexturesSize;
uniform sampler2D u_BlurTextures[32];
uniform float u_Exposure;
uniform float u_Brightness;

void main()
{
    vec4 textureColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 sourceColor = texture(u_BlurTextures[0], uv);

    //float brightness = dot(sourceColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    //if (brightness > u_Brightness)
    //{
    //    textureColor += sourceColor;
    //}

    for (int i = 1; i < u_BlurTexturesSize; i++)
    {
        textureColor += texture(u_BlurTextures[i], uv);
    }

    fragColor = vec4(vec3(1.0) - exp(-textureColor.rgb * u_Exposure), textureColor.a);
}