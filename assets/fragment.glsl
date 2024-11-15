#version 330 core

out vec4 FragColor;

in vec3 fragPos;

uniform ivec2 screenSize;

uniform usampler2D boardA;

void main() {
	ivec2 fragCoord = ivec2((fragPos.xy * 0.5 + 0.5) * screenSize);

	FragColor = texture(boardA, vec2(fragCoord) / screenSize);

	FragColor = vec4(FragColor.r);
}