#version 330 core

out vec4 FragColor;

in vec3 fragPos;

uniform ivec2 screenSize;

void main() {
	ivec2 fragCoord = ivec2((fragPos.xy * 0.5 + 0.5) * screenSize / 64);

	if ((fragCoord.x + fragCoord.y) % 2 == 0) {
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	} else {
		FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}