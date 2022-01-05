#ifndef CHUNK_PIECE_HPP
#define CHUNK_PIECE_HPP

#include "Blocks/Block.h"

const int g_minChunkHeight = -40;
const int g_waterLevel = 0;


struct ChunkPiece
{
    std::vector<std::unique_ptr<Block>> blocks;

    // uses a lot of memory
    std::vector<short> reservedBlockPositions;
    //

    short height, stoneHeight;

    void setup();
    int searchForBlock(unsigned int y) const;
    void addBlock(glm::vec3 pos, unsigned char visibleFaces);
    bool blockReserved(short y);
};


#endif // !CHUNK_PIECE_HPP