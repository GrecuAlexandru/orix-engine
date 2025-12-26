#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aBlockID;

out vec2 TexCoord;
out float vBlockID;

uniform mat4 u_VP;    // View * Projection
uniform mat4 u_Model; // Per-chunk position

void main() {
    gl_Position = u_VP * u_Model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    vBlockID = aBlockID;
}