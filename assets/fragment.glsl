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

//uniform sampler2D Texture;
//in vec2 Frag_UV;
//in vec4 Frag_Color;
//out vec4 Out_Color;
//void main()
//{
//    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);
//}
