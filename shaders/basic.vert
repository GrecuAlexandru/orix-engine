#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 u_VP;    // View * Projection
uniform mat4 u_Model; // Per-chunk position

void main() {
    gl_Position = u_VP * u_Model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}