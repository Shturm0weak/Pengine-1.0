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

uniform sampler2D u_Image;
uniform sampler2D u_SourceImage;
uniform bool u_Horizontal;
uniform int u_Pixels;
uniform float u_Brightness;
uniform bool u_FirstFrameBufferIteration;

float weight[6] = float[](
    0.2578808493395437,
    0.20929982634420685,
    0.11188116791897133,
    0.03937394304056275,
    0.009116827159395895,
    0.001387810867091255);

void main()
{
    vec2 texelSize = 1.0 / textureSize(u_Image, 0);

    if (u_FirstFrameBufferIteration)
    {
        float brightness = dot(texture(u_SourceImage, uv).rgb, vec3(0.2126, 0.7152, 0.0722));
        if (brightness < u_Brightness)
        {
            discard;
        }
    }

    vec3 textureColor = texture(u_Image, uv).rgb;

    textureColor *= weight[0];
    
    if (u_Horizontal)
    {
        for (int i = 1; i < u_Pixels; ++i)
        {
            textureColor += texture(u_Image, uv + vec2(texelSize.x * i, 0.0)).rgb * weight[i];
            textureColor += texture(u_Image, uv - vec2(texelSize.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < u_Pixels; ++i)
        {
            textureColor += texture(u_Image, uv - vec2(0.0, texelSize.y * i)).rgb * weight[i];
            textureColor += texture(u_Image, uv + vec2(0.0, texelSize.y * i)).rgb * weight[i];
        }
    }

    fragColor = vec4(textureColor, 1.0);
}