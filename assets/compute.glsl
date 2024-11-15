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
    
    ivec2 x0Pos = pos + ivec2(-1, -1);
    ivec2 x1Pos = pos + ivec2( 0, -1);
    ivec2 x2Pos = pos + ivec2( 1, -1);
    ivec2 x3Pos = pos + ivec2(-1,  0);
    ivec2 x5Pos = pos + ivec2( 1,  0);
    ivec2 x6Pos = pos + ivec2(-1,  1);
    ivec2 x7Pos = pos + ivec2( 0,  1);
    ivec2 x8Pos = pos + ivec2( 1,  1);

    x0Pos.x = x0Pos.x < 0 ? boardSize.x - 1 : x0Pos.x;
    x1Pos.x = x1Pos.x < 0 ? boardSize.x - 1 : x1Pos.x;
    x2Pos.x = x2Pos.x < 0 ? boardSize.x - 1 : x2Pos.x;
    x3Pos.x = x3Pos.x < 0 ? boardSize.x - 1 : x3Pos.x;
    x5Pos.x = x5Pos.x < 0 ? boardSize.x - 1 : x5Pos.x;
    x6Pos.x = x6Pos.x < 0 ? boardSize.x - 1 : x6Pos.x;
    x7Pos.x = x7Pos.x < 0 ? boardSize.x - 1 : x7Pos.x;
    x8Pos.x = x8Pos.x < 0 ? boardSize.x - 1 : x8Pos.x;

    x0Pos.y = x0Pos.y < 0 ? boardSize.y - 1 : x0Pos.y;
    x1Pos.y = x1Pos.y < 0 ? boardSize.y - 1 : x1Pos.y;
    x2Pos.y = x2Pos.y < 0 ? boardSize.y - 1 : x2Pos.y;
    x3Pos.y = x3Pos.y < 0 ? boardSize.y - 1 : x3Pos.y;
    x5Pos.y = x5Pos.y < 0 ? boardSize.y - 1 : x5Pos.y;
    x6Pos.y = x6Pos.y < 0 ? boardSize.y - 1 : x6Pos.y;
    x7Pos.y = x7Pos.y < 0 ? boardSize.y - 1 : x7Pos.y;
    x8Pos.y = x8Pos.y < 0 ? boardSize.y - 1 : x8Pos.y;

    x0Pos.x = x0Pos.x >= boardSize.x ? 0 : x0Pos.x;
    x1Pos.x = x1Pos.x >= boardSize.x ? 0 : x1Pos.x;
    x2Pos.x = x2Pos.x >= boardSize.x ? 0 : x2Pos.x;
    x3Pos.x = x3Pos.x >= boardSize.x ? 0 : x3Pos.x;
    x5Pos.x = x5Pos.x >= boardSize.x ? 0 : x5Pos.x;
    x6Pos.x = x6Pos.x >= boardSize.x ? 0 : x6Pos.x;
    x7Pos.x = x7Pos.x >= boardSize.x ? 0 : x7Pos.x;
    x8Pos.x = x8Pos.x >= boardSize.x ? 0 : x8Pos.x;

    x0Pos.y = x0Pos.y >= boardSize.y ? 0 : x0Pos.y;
    x1Pos.y = x1Pos.y >= boardSize.y ? 0 : x1Pos.y;
    x2Pos.y = x2Pos.y >= boardSize.y ? 0 : x2Pos.y;
    x3Pos.y = x3Pos.y >= boardSize.y ? 0 : x3Pos.y;
    x5Pos.y = x5Pos.y >= boardSize.y ? 0 : x5Pos.y;
    x6Pos.y = x6Pos.y >= boardSize.y ? 0 : x6Pos.y;
    x7Pos.y = x7Pos.y >= boardSize.y ? 0 : x7Pos.y;
    x8Pos.y = x8Pos.y >= boardSize.y ? 0 : x8Pos.y;

    x0 = imageLoad(boardA, x0Pos).x == 255 ? 1 : 0;
    x1 = imageLoad(boardA, x1Pos).x == 255 ? 1 : 0;
    x2 = imageLoad(boardA, x2Pos).x == 255 ? 1 : 0;
    x3 = imageLoad(boardA, x3Pos).x == 255 ? 1 : 0;
    x5 = imageLoad(boardA, x5Pos).x == 255 ? 1 : 0;
    x6 = imageLoad(boardA, x6Pos).x == 255 ? 1 : 0;
    x7 = imageLoad(boardA, x7Pos).x == 255 ? 1 : 0;
    x8 = imageLoad(boardA, x8Pos).x == 255 ? 1 : 0;

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