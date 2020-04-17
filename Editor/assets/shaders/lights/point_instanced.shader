#shader Vertex
#version 420

layout(location = 0) in vec3 vert;
layout(location = 1) in mat4 model;

void main() {
	gl_Position = model * vec4(vert, 1);
}

#shader Geometry
#version 420

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 light_mats[6];

out vec4 FragPos;

void main() {
    for (int face = 0; face < 6; face++) {
        gl_Layer = face;
        for (int i = 0; i < 3; i++) {
            FragPos = gl_in[i].gl_Position;
            gl_Position = light_mats[face] * FragPos;
            EmitVertex();
        }

        EndPrimitive();
    }
}

#shader Fragment
#version 420

in vec4 FragPos;

uniform vec3 light_pos;
uniform float light_farPlane;

void main() {
    float distance = length(FragPos.xyz - light_pos) / light_farPlane;
    gl_FragDepth = distance;
}
