#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba8ui, binding = 0) uniform uimage2D board;

layout(std430, binding = 2) readonly buffer setCellsBuffer {
    ivec2 cells[];
};

void main() {
    imageStore(board, cells[gl_GlobalInvocationID.x], uvec4(255));
}