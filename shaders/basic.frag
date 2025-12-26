#version 450 core
out vec4 FragColor;
in vec2 TexCoord;

uniform vec3 u_Color;

void main() {
    float borderSize = 0.01;
    
    // Find how close we are to any edge
    vec2 distances = min(TexCoord, 1.0 - TexCoord);
    float edgeDist = min(distances.x, distances.y);

    // 1.0 if we are in the border, 0.0 if we are in the block center
    float isBorder = 1.0 - step(borderSize, edgeDist);

    // Blend between block color and black
    vec3 finalColor = mix(u_Color, vec3(0.0, 0.0, 0.0), isBorder);
    
    FragColor = vec4(finalColor, 1.0);
}