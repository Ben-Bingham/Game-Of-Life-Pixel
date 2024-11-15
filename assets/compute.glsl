#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(r8ui, binding = 0) uniform uimage2D boardA;
layout(r8ui, binding = 1) uniform uimage2D boardB;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    //uint val = uint(256.0 * (gl_GlobalInvocationID.x / 400)); 

    //imageStore(boardA, pos, uvec4(val, 0, 0, 0));

    if ((gl_GlobalInvocationID.x + gl_GlobalInvocationID.y) % 2 == 0) {
        imageStore(boardA, pos, uvec4(256, 0, 0, 0));
    } else {
        imageStore(boardA, pos, uvec4(0, 0, 0, 0));
    }
}