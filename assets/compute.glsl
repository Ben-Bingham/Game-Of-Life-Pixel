#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(r8ui, binding = 0) uniform uimage2D boardA;
layout(r8ui, binding = 1) uniform uimage2D boardB;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    if ((gl_GlobalInvocationID.x / 64 + gl_GlobalInvocationID.y / 64) % 2 == 0) {
        imageStore(boardA, pos, uvec4(256, 0, 0, 0));
    } else {
        imageStore(boardA, pos, uvec4(0, 0, 0, 0));
    }
}