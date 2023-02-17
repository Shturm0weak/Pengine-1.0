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
    vec4 textureColor = vec4(0.0);

    for (int i = 0; i < u_BlurTexturesSize; i++)
    {
        vec4 mip = texture(u_BlurTextures[i], uv);
        mip = vec4(vec3(1.0) - exp(-mip.rgb * u_Exposure), mip.a);
        textureColor += clamp(mip, 0.0, 1.0);
    }

    fragColor = textureColor;
}