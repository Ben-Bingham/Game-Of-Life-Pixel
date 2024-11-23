#version 430 core

layout(local_size_x = 2, local_size_y = 2, local_size_z = 1) in;
layout(rgba8ui, binding = 0) uniform uimage2D boardA; // The source board
layout(rgba8ui, binding = 1) uniform uimage2D boardB; // The target board

uniform ivec2 boardSize;

shared uint x[16];

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

    // Coord relative to this work groups 4x4 grid
    // Points to either: x0, x2, x6, xD
    ivec2 topLeftCoord = ivec2(gl_LocalInvocationID * 2).xy;

    ivec2 topLeftOfQuad     = topLeftCoord + ivec2(0, 0);
    ivec2 topRightOfQuad    = topLeftCoord + ivec2(1, 0);
    ivec2 bottomLeftOfQuad  = topLeftCoord + ivec2(0, 1);
    ivec2 bottomRightOfQuad = topLeftCoord + ivec2(1, 1);

    int indexTopLeftOfQuad     = topLeftOfQuad    .y * 4 + topLeftOfQuad    .x;
    int indexTopRightOfQuad    = topRightOfQuad   .y * 4 + topRightOfQuad   .x;
    int indexBottomLeftOfQuad  = bottomLeftOfQuad .y * 4 + bottomLeftOfQuad .x;
    int indexBottomRightOfQuad = bottomRightOfQuad.y * 4 + bottomRightOfQuad.x;    

    // The first part, up to and inculding the -ivec2(1, 1) calculates the position of the top left corner of the grid above
    ivec2 globalTopLeftOfQuad     = ivec2((gl_WorkGroupID * gl_WorkGroupSize).xy) - ivec2(1, 1) + topLeftOfQuad     ;
    ivec2 globalTopRightOfQuad    = ivec2((gl_WorkGroupID * gl_WorkGroupSize).xy) - ivec2(1, 1) + topRightOfQuad    ;
    ivec2 globalBottomLeftOfQuad  = ivec2((gl_WorkGroupID * gl_WorkGroupSize).xy) - ivec2(1, 1) + bottomLeftOfQuad  ;
    ivec2 globalBottomRightOfQuad = ivec2((gl_WorkGroupID * gl_WorkGroupSize).xy) - ivec2(1, 1) + bottomRightOfQuad ;

    globalTopLeftOfQuad    .x = globalTopLeftOfQuad    .x < 0 ? boardSize.x - 1 : globalTopLeftOfQuad    .x;
    globalTopRightOfQuad   .x = globalTopRightOfQuad   .x < 0 ? boardSize.x - 1 : globalTopRightOfQuad   .x;
    globalBottomLeftOfQuad .x = globalBottomLeftOfQuad .x < 0 ? boardSize.x - 1 : globalBottomLeftOfQuad .x;
    globalBottomRightOfQuad.x = globalBottomRightOfQuad.x < 0 ? boardSize.x - 1 : globalBottomRightOfQuad.x;

    globalTopLeftOfQuad    .y = globalTopLeftOfQuad    .y < 0 ? boardSize.y - 1 : globalTopLeftOfQuad    .y;
    globalTopRightOfQuad   .y = globalTopRightOfQuad   .y < 0 ? boardSize.y - 1 : globalTopRightOfQuad   .y;
    globalBottomLeftOfQuad .y = globalBottomLeftOfQuad .y < 0 ? boardSize.y - 1 : globalBottomLeftOfQuad .y;
    globalBottomRightOfQuad.y = globalBottomRightOfQuad.y < 0 ? boardSize.y - 1 : globalBottomRightOfQuad.y;

    globalTopLeftOfQuad    .x = globalTopLeftOfQuad    .x >= boardSize.x ? 0 : globalTopLeftOfQuad    .x;
    globalTopRightOfQuad   .x = globalTopRightOfQuad   .x >= boardSize.x ? 0 : globalTopRightOfQuad   .x;
    globalBottomLeftOfQuad .x = globalBottomLeftOfQuad .x >= boardSize.x ? 0 : globalBottomLeftOfQuad .x;
    globalBottomRightOfQuad.x = globalBottomRightOfQuad.x >= boardSize.x ? 0 : globalBottomRightOfQuad.x;

    globalTopLeftOfQuad    .y = globalTopLeftOfQuad    .y >= boardSize.y ? 0 : globalTopLeftOfQuad    .y;
    globalTopRightOfQuad   .y = globalTopRightOfQuad   .y >= boardSize.y ? 0 : globalTopRightOfQuad   .y;
    globalBottomLeftOfQuad .y = globalBottomLeftOfQuad .y >= boardSize.y ? 0 : globalBottomLeftOfQuad .y;
    globalBottomRightOfQuad.y = globalBottomRightOfQuad.y >= boardSize.y ? 0 : globalBottomRightOfQuad.y;

    x[indexTopLeftOfQuad    ] = imageLoad(boardA, globalTopLeftOfQuad    ).x == 255 ? 1 : 0;
    x[indexTopRightOfQuad   ] = imageLoad(boardA, globalTopRightOfQuad   ).x == 255 ? 1 : 0;
    x[indexBottomLeftOfQuad ] = imageLoad(boardA, globalBottomLeftOfQuad ).x == 255 ? 1 : 0;
    x[indexBottomRightOfQuad] = imageLoad(boardA, globalBottomRightOfQuad).x == 255 ? 1 : 0;

    memoryBarrierShared();
    // Find cells current coordinate in the 4x4 grid above, translate that into an index to find the value of the current cell
    // Take the initialy found coord, and add and subtract unit vectors from it to find the coords of the adjacent cells.
    // Convert those coords to indices and check the x array.

    ivec2 threadPosIn4x4 = ivec2(gl_GlobalInvocationID.xy) - (ivec2((gl_WorkGroupID * gl_WorkGroupSize).xy) - ivec2(1, 1));

    int currentCellIndexIn4x4 = threadPosIn4x4.y * 4 + threadPosIn4x4.x;

    uint currentCell = x[currentCellIndexIn4x4];

    ivec2 x0Pos = threadPosIn4x4 + ivec2( 0,  1);
    ivec2 x1Pos = threadPosIn4x4 + ivec2( 1,  0);
    ivec2 x2Pos = threadPosIn4x4 + ivec2( 1,  1);
    ivec2 x3Pos = threadPosIn4x4 + ivec2( 0, -1);
    ivec2 x4Pos = threadPosIn4x4 + ivec2(-1,  0);
    ivec2 x5Pos = threadPosIn4x4 + ivec2(-1, -1);
    ivec2 x6Pos = threadPosIn4x4 + ivec2( 1, -1);
    ivec2 x7Pos = threadPosIn4x4 + ivec2(-1,  1);

    int x0 = x0Pos.y * 4 + x0Pos.x;
    int x1 = x1Pos.y * 4 + x1Pos.x;
    int x2 = x2Pos.y * 4 + x2Pos.x;
    int x3 = x3Pos.y * 4 + x3Pos.x;
    int x4 = x4Pos.y * 4 + x4Pos.x;
    int x5 = x5Pos.y * 4 + x5Pos.x;
    int x6 = x6Pos.y * 4 + x6Pos.x;
    int x7 = x7Pos.y * 4 + x7Pos.x;

    uint neighbourCount = x[x0] + x[x1] + x[x2] + x[x3] + x[x4] + x[x5] + x[x6] + x[x7];

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