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
uniform float u_UpSamplingScale;
uniform bool u_UpSampling;

vec4 UpsampleTent(sampler2D image, vec4 sampleScale)
{
    vec2 texelSize = 1.0 / textureSize(image, 0);
    vec4 d = texelSize.xyxy * vec4(1.0, 1.0, -1.0, 0.0) * sampleScale;

    vec4 s;
    s = texture(image, (uv - d.xy));
    s += texture(image, (uv - d.wy)) * 2.0;
    s += texture(image, (uv - d.zy));

    s += texture(image, (uv + d.zw)) * 2.0;
    s += texture(image, (uv)) * 4.0;
    s += texture(image, (uv + d.xw)) * 2.0;

    s += texture(image, (uv + d.zy));
    s += texture(image, (uv + d.wy)) * 2.0;
    s += texture(image, (uv + d.xy));

    return s * (1.0 / 16.0);
}

void main()
{
    vec4 textureColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 sourceColor = texture(u_BlurTextures[0], uv);

    float brightness = dot(sourceColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > u_Brightness)
    {
        textureColor += sourceColor;
    }

    for (int i = 1; i < u_BlurTexturesSize; i++)
    {
        if (u_UpSampling)
        {
            textureColor += UpsampleTent(u_BlurTextures[i], vec4(u_UpSamplingScale));
        }
        else
        {
            textureColor += texture(u_BlurTextures[i], uv);
        }
    }

    fragColor = vec4(vec3(1.0) - exp(-textureColor.rgb * u_Exposure), textureColor.a);
}