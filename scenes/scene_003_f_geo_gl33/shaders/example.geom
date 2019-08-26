#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 64) out;

uniform float uSideFactor;

in vec4 vColor[]; // Output from vertex shader for each vertex
in float vSides[];

out vec4 fColor; // Output to fragment shader

const float PI = 3.1415926;

void main()
{
    fColor = vColor[0]; // Point has only one vertex
    float sides = floor(vSides[0] * uSideFactor);
    if (sides > 63.0) {
	sides = 63.0;
    }
    else if (sides < 2.0) {
	sides = 2.0;
    }
    for (int i = 0; i <= sides; i++) {
        // Angle between each side in radians
        float ang = PI * 2.0 / sides * i;

        // Offset from center of point (0.3 to accomodate for aspect ratio)
        vec4 offset = vec4(cos(ang) * 0.3, -sin(ang) * 0.4, 0.0, 0.0);
        gl_Position = gl_in[0].gl_Position + offset;

        EmitVertex();
    }

    EndPrimitive();
}
