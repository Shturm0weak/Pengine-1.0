#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 5) in mat4 transformA;

void main()
{
	gl_Position = transformA * vec4(positionA, 1.0);
}

#shader geometry
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 u_Projection[6];

out vec4 worldPosition;

void main()
{
    for (int face = 0; face < 6; face++)
    {
        gl_Layer = face;
        for (int i = 0; i < 3; ++i)
        {
            worldPosition = gl_in[i].gl_Position;
            gl_Position = u_Projection[face] * worldPosition;
            EmitVertex();
        }
        EndPrimitive();
    }
}

#shader fragment
#version 330 core

uniform vec3 u_LightPosition;
uniform float u_FarPlane;

in vec4 worldPosition;

void main()
{
    float lightDistance = length(worldPosition.xyz - u_LightPosition);

    lightDistance = lightDistance / u_FarPlane;

    gl_FragDepth = lightDistance;
}