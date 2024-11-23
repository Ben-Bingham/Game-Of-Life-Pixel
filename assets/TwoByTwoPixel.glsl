#version 430 core

layout(local_size_x = 2, local_size_y = 2, local_size_z = 1) in;
layout(rgba8ui, binding = 0) uniform uimage2D boardA; // The source board
layout(rgba8ui, binding = 1) uniform uimage2D boardB; // The target board

uniform ivec2 boardSize;

shared uint xA;
shared uint xB;
shared uint xC;
shared uint xD;

shared uint x0;
shared uint x1;
shared uint x2;
shared uint x3;
shared uint x4;
shared uint x5;
shared uint x6;
shared uint x7;
shared uint x8;
shared uint x9;
shared uint x10;
shared uint x11;

/*
=-------------------=
| x0 | x1 | x2 | x3 |
---------------------
| x4 | xA | xB | x5 |
---------------------
| x6 | xC | xD | x7 |
---------------------
| x8 | x9 | x10| x11|
=-------------------=
*/

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    if (gl_LocalInvocationIndex == 0) {
        xA = imageLoad(boardA, pos + ivec2(0,  0)).x == 255 ? 1 : 0;

        ivec2 x0Pos = pos + ivec2(-1, -1);
        ivec2 x1Pos = pos + ivec2( 0, -1);
        ivec2 x4Pos = pos + ivec2(-1,  0);

        x0Pos.x = x0Pos.x < 0 ? boardSize.x - 1 : x0Pos.x;
        x1Pos.x = x1Pos.x < 0 ? boardSize.x - 1 : x1Pos.x;
        x4Pos.x = x4Pos.x < 0 ? boardSize.x - 1 : x4Pos.x;

        x0Pos.y = x0Pos.y < 0 ? boardSize.y - 1 : x0Pos.y;
        x1Pos.y = x1Pos.y < 0 ? boardSize.y - 1 : x1Pos.y;
        x4Pos.y = x4Pos.y < 0 ? boardSize.y - 1 : x4Pos.y;

        x0Pos.x = x0Pos.x >= boardSize.x ? 0 : x0Pos.x;
        x1Pos.x = x1Pos.x >= boardSize.x ? 0 : x1Pos.x;
        x4Pos.x = x4Pos.x >= boardSize.x ? 0 : x4Pos.x;

        x0Pos.y = x0Pos.y >= boardSize.y ? 0 : x0Pos.y;
        x1Pos.y = x1Pos.y >= boardSize.y ? 0 : x1Pos.y;
        x4Pos.y = x4Pos.y >= boardSize.y ? 0 : x4Pos.y;

        x0 = imageLoad(boardA, x0Pos).x == 255 ? 1 : 0;
        x1 = imageLoad(boardA, x1Pos).x == 255 ? 1 : 0;
        x4 = imageLoad(boardA, x4Pos).x == 255 ? 1 : 0;

    } else if (gl_LocalInvocationIndex == 1) {
        xB = imageLoad(boardA, pos + ivec2(0,  0)).x == 255 ? 1 : 0;

        ivec2 x2Pos = pos + ivec2( 0, -1);
        ivec2 x3Pos = pos + ivec2( 1, -1);
        ivec2 x5Pos = pos + ivec2( 1,  0);

        x2Pos.x = x2Pos.x < 0 ? boardSize.x - 1 : x2Pos.x;
        x3Pos.x = x3Pos.x < 0 ? boardSize.x - 1 : x3Pos.x;
        x5Pos.x = x5Pos.x < 0 ? boardSize.x - 1 : x5Pos.x;

        x2Pos.y = x2Pos.y < 0 ? boardSize.y - 1 : x2Pos.y;
        x3Pos.y = x3Pos.y < 0 ? boardSize.y - 1 : x3Pos.y;
        x5Pos.y = x5Pos.y < 0 ? boardSize.y - 1 : x5Pos.y;

        x2Pos.x = x2Pos.x >= boardSize.x ? 0 : x2Pos.x;
        x3Pos.x = x3Pos.x >= boardSize.x ? 0 : x3Pos.x;
        x5Pos.x = x5Pos.x >= boardSize.x ? 0 : x5Pos.x;

        x2Pos.y = x2Pos.y >= boardSize.y ? 0 : x2Pos.y;
        x3Pos.y = x3Pos.y >= boardSize.y ? 0 : x3Pos.y;
        x5Pos.y = x5Pos.y >= boardSize.y ? 0 : x5Pos.y;

        x2 = imageLoad(boardA, x2Pos).x == 255 ? 1 : 0;
        x3 = imageLoad(boardA, x3Pos).x == 255 ? 1 : 0;
        x5 = imageLoad(boardA, x5Pos).x == 255 ? 1 : 0;

    } else if (gl_LocalInvocationIndex == 2) {
        xC = imageLoad(boardA, pos + ivec2(0,  0)).x == 255 ? 1 : 0;

        ivec2 x6Pos = pos + ivec2(-1,  0);
        ivec2 x8Pos = pos + ivec2(-1,  1);
        ivec2 x9Pos = pos + ivec2( 0,  1);

        x6Pos.x = x6Pos.x < 0 ? boardSize.x - 1 : x6Pos.x;
        x8Pos.x = x8Pos.x < 0 ? boardSize.x - 1 : x8Pos.x;
        x9Pos.x = x9Pos.x < 0 ? boardSize.x - 1 : x9Pos.x;

        x6Pos.y = x6Pos.y < 0 ? boardSize.y - 1 : x6Pos.y;
        x8Pos.y = x8Pos.y < 0 ? boardSize.y - 1 : x8Pos.y;
        x9Pos.y = x9Pos.y < 0 ? boardSize.y - 1 : x9Pos.y;

        x6Pos.x = x6Pos.x >= boardSize.x ? 0 : x6Pos.x;
        x8Pos.x = x8Pos.x >= boardSize.x ? 0 : x8Pos.x;
        x9Pos.x = x9Pos.x >= boardSize.x ? 0 : x9Pos.x;

        x6Pos.y = x6Pos.y >= boardSize.y ? 0 : x6Pos.y;
        x8Pos.y = x8Pos.y >= boardSize.y ? 0 : x8Pos.y;
        x9Pos.y = x9Pos.y >= boardSize.y ? 0 : x9Pos.y;

        x6 = imageLoad(boardA, x6Pos).x == 255 ? 1 : 0;
        x8 = imageLoad(boardA, x8Pos).x == 255 ? 1 : 0;
        x9 = imageLoad(boardA, x9Pos).x == 255 ? 1 : 0;

    } else if (gl_LocalInvocationIndex == 3) {
        xD = imageLoad(boardA, pos + ivec2(0,  0)).x == 255 ? 1 : 0;

        ivec2 x7Pos  = pos + ivec2( 1,  0);
        ivec2 x10Pos = pos + ivec2( 0,  1);
        ivec2 x11Pos = pos + ivec2( 1,  1);

        x7Pos.x = x7Pos.x < 0 ? boardSize.x - 1 : x7Pos.x;
        x10Pos.x = x10Pos.x < 0 ? boardSize.x - 1 : x10Pos.x;
        x11Pos.x = x11Pos.x < 0 ? boardSize.x - 1 : x11Pos.x;

        x7Pos.y = x7Pos.y < 0 ? boardSize.y - 1 : x7Pos.y;
        x10Pos.y = x10Pos.y < 0 ? boardSize.y - 1 : x10Pos.y;
        x11Pos.y = x11Pos.y < 0 ? boardSize.y - 1 : x11Pos.y;

        x7Pos.x = x7Pos.x >= boardSize.x ? 0 : x7Pos.x;
        x10Pos.x = x10Pos.x >= boardSize.x ? 0 : x10Pos.x;
        x11Pos.x = x11Pos.x >= boardSize.x ? 0 : x11Pos.x;

        x7Pos.y = x7Pos.y >= boardSize.y ? 0 : x7Pos.y;
        x10Pos.y = x10Pos.y >= boardSize.y ? 0 : x10Pos.y;
        x11Pos.y = x11Pos.y >= boardSize.y ? 0 : x11Pos.y;

        x7 = imageLoad(boardA, x7Pos).x == 255 ? 1 : 0;
        x10 = imageLoad(boardA, x10Pos).x == 255 ? 1 : 0;
        x11 = imageLoad(boardA, x11Pos).x == 255 ? 1 : 0;
    }

    memoryBarrierShared();

    uint neighbourCount = 0;
    uint currentCell = -1;
    if (gl_LocalInvocationIndex == 0) {
        currentCell = xA;
        neighbourCount = x0 + x1 + x2 + x4 + xB + x6 + xC + xD;

    } else if (gl_LocalInvocationIndex == 1) {
        currentCell = xB;
        neighbourCount = x1 + x2 + x3 + xA + x5 + xC + xD + x7;

    } else if (gl_LocalInvocationIndex == 2) {
        currentCell = xC;
        neighbourCount = x4 + xA + xB + x6 + xD + x8 + x9 + x10;

    } else if (gl_LocalInvocationIndex == 3) {
        currentCell = xD;
        neighbourCount = xA + xB + x5 + xC + x7 + x9 + x10 + x11;
    }

    uint valToStore = 0;

    if (currentCell == 0) {
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
    
    int val = valToStore == 1 ? 255 : 0;
    
    imageStore(boardB, pos, uvec4(val));
}