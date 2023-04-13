#shader vertex
#version 420 core

layout(location = 0) in vec3 positionA;

#include "Source/Shaders/Default/Common/UniformObjects.incl"

out vec3 uv;

void main()
{
    uv = positionA;
    vec4 position = u_GlobalUniforms.projection * inverse(u_GlobalUniforms.cameraRotation) * vec4(positionA, 1.0);
    gl_Position = position.xyww;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 fragColor;

uniform samplerCube u_SkyBox;

in vec3 uv;

void main()
{
    vec4 color = texture(u_SkyBox, uv);

    color.rgb = pow(color.rgb, vec3(2.2 / 1.0));

    fragColor = color;
}
