#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(r8ui, binding = 0) uniform uimage2D boardA; // The source board
layout(r8ui, binding = 1) uniform uimage2D boardB; // The target board

uniform ivec2 boardSize;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    //imageStore(boardB, pos, imageLoad(boardA, pos).r == 0 ? uvec4(255) : uvec4(0));
    //return;
    //uint val = uint(256.0 * (gl_GlobalInvocationID.x / 400)); 

    if (gl_GlobalInvocationID.x == gl_GlobalInvocationID.y) {
      //  imageStore(boardA, pos, uvec4(255, 0, 0, 0));
    }
    //return;

    /*
    |---------------
    | x0 | x1 | x2 |
    |---------------
    | x3 | x  | x5 |
    |---------------
    | x6 | x7 | x8 |
    |---------------
    */

    uint x = 0;

    uint x0 = 0;
    uint x1 = 0;
    uint x2 = 0;
    uint x3 = 0;
    uint x5 = 0;
    uint x6 = 0;
    uint x7 = 0;
    uint x8 = 0;

    // Fetch from boardB
    x  = imageLoad(boardA, pos + ivec2( 0,  0)).x;

    //imageStore(boardA, pos, uvec4(255, 0, 0, 0));


    //if (x != 0) {
    //    imageStore(boardA, pos, uvec4(256, 0, 0, 0));
    //}
    //return;
    
    x0 = imageLoad(boardA, pos + ivec2(-1, -1)).x == 255 ? 1 : 0;
    x1 = imageLoad(boardA, pos + ivec2( 0, -1)).x == 255 ? 1 : 0;
    x2 = imageLoad(boardA, pos + ivec2( 1, -1)).x == 255 ? 1 : 0;
    x3 = imageLoad(boardA, pos + ivec2(-1,  0)).x == 255 ? 1 : 0;
    x5 = imageLoad(boardA, pos + ivec2( 1,  0)).x == 255 ? 1 : 0;
    x6 = imageLoad(boardA, pos + ivec2(-1,  1)).x == 255 ? 1 : 0;
    x7 = imageLoad(boardA, pos + ivec2( 0,  1)).x == 255 ? 1 : 0;
    x8 = imageLoad(boardA, pos + ivec2( 1,  1)).x == 255 ? 1 : 0;

    uint neighbourCount = x0 + x1 + x2 + x3 + x5 + x6 + x7 + x8;

    /*
    if (neighbourCount == 3) {
        imageStore(boardB, pos, uvec4(255, 0, 0, 0));
    } else {
        imageStore(boardB, pos, uvec4(0, 0, 0, 0));
    }
    */
    //return;

    // Default action should be to clear the entry in boardA
    uint valToStore = 0;

    if (x == 0) {
        // Cell is dead
        if (neighbourCount == 3) {
            valToStore = 1;
        }
    } else {
        // Cell is alive
        if (neighbourCount == 2 || neighbourCount == 3) {
            valToStore = 1;
        }
    }

    imageStore(boardB, pos, uvec4(valToStore == 1 ? 255 : 0, 0, 0, 0));

    //if ((gl_GlobalInvocationID.x + gl_GlobalInvocationID.y) % 2 == 0) {
    //    imageStore(boardA, pos, uvec4(256, 0, 0, 0));
    //} else {
    //    imageStore(boardA, pos, uvec4(0, 0, 0, 0));
    //}
    
}