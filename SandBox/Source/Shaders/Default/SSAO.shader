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

in vec2 uv;

uniform sampler2D u_Position;
uniform sampler2D u_Normal;
uniform sampler2D u_Noise;

uniform vec2 u_Resolution;

uniform vec3 u_Samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
uniform int u_KernelSize;
uniform float u_NoiseSize;
uniform float u_Radius;
uniform float u_Bias;

// tile noise texture over screen based on screen dimensions divided by noise size
vec2 noiseScale = vec2(u_Resolution.x / u_NoiseSize, u_Resolution.y / u_NoiseSize);

uniform mat4 u_ViewProjection;
uniform vec3 u_CameraPosition;
uniform vec3 u_CameraDirection;

void main()
{
    // get input for SSAO algorithm
    vec3 worldPosition = texture(u_Position, uv).xyz;
    vec3 normal = texture(u_Normal, uv).xyz;
    vec3 randomVector = normalize(texture(u_Noise, uv * noiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for (int i = 0; i < u_KernelSize; ++i)
    {
        // get sample position
        vec3 sample = TBN * u_Samples[i]; // From tangent to view-space
        sample = worldPosition + sample * u_Radius;

        float sampleZ = dot(sample - u_CameraPosition, u_CameraDirection); // cameraDirection must be normalized!

        vec4 offset = vec4(sample, 1.0);
        offset = u_ViewProjection * offset; // from world to clip-space NOTE: we use projection * view instead of view
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        vec3 sampleProjected = vec4(texture(u_Position, offset.xy)).xyz;

        vec3 toOriginal = sampleProjected - u_CameraPosition;
        float sampleDepth = dot(u_CameraDirection, toOriginal);

        //// range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, length(worldPosition - sampleProjected) / u_Radius);
        rangeCheck = 1.0 - rangeCheck;
        occlusion += (sampleDepth >= sampleZ + u_Bias ? 0.0 : 1.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / u_KernelSize);

    fragColor = vec4(occlusion, 0.0, 0.0, 1.0);
}