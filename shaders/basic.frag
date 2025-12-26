#version 450 core
out vec4 FragColor;
in vec2 TexCoord;
in float vBlockID;

void main() {
    float borderSize = 0.01;
    
    // Find how close we are to any edge
    vec2 distances = min(TexCoord, 1.0 - TexCoord);
    float edgeDist = min(distances.x, distances.y);

    // 1.0 if we are in the border, 0.0 if we are in the block center
    float isBorder = 1.0 - step(borderSize, edgeDist);

    // BlockType: Air=0, Grass=1, Dirt=2, Stone=3
    vec3 blockColor;
    if (vBlockID < 1.5) {
        blockColor = vec3(0.1, 0.8, 0.2); // Grass - bright green
    } else if (vBlockID < 2.5) {
        blockColor = vec3(0.6, 0.4, 0.2); // Dirt - brown
    } else {
        blockColor = vec3(0.5, 0.5, 0.5); // Stone - gray
    }

    // Blend between block color and black border
    vec3 finalColor = mix(blockColor, vec3(0.0, 0.0, 0.0), isBorder);
    
    FragColor = vec4(finalColor, 1.0);
}