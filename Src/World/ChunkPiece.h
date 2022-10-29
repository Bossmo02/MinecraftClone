#ifndef CHUNK_PIECE_HPP
#define CHUNK_PIECE_HPP

#include "Blocks/Block.h"

#include <deque>

const int g_minChunkHeight = -127;
const int g_waterLevel = 0;
const int g_maxHeight = 127;

struct ChunkPiece
{
    std::deque<std::unique_ptr<Block>> blocks;

    // uses a lot of memory when a lot of blocks have been destroyed
    std::deque<short> usedBlockPositions;


    short height, stoneHeight, maxHeight;

    void setup();
    int searchForBlock(int y) const;
    void addBlock(glm::vec3 pos, unsigned char visibleFaces);
    bool blockPosUsed(short y);
    void deleteBlock(size_t index);
};


#endif // !CHUNK_PIECE_HPP