#include "ChunkPiece.h"



void ChunkPiece::setup()
{
    for (short i = height; i >= g_minChunkHeight; --i)
    {
        reservedBlockPositions.push_back((short)i);
    }
}

int ChunkPiece::searchForBlock(unsigned int y) const
{
    // returns the index of the block at given height
    for (int i = 0; i < blocks.size(); ++i)
    {
        if (blocks[i].get()->getWorldPos().y == y)
            return i;
    }

    return -1;
}

void ChunkPiece::addBlock(glm::vec3 pos, unsigned char visibleFaces)
{
    if (blockReserved(pos.y))
    {
        if (pos.y == height && height > g_waterLevel)
            blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::GRASS, visibleFaces)));
        else if (pos.y == g_waterLevel)
            blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::SAND, visibleFaces)));
        else if (pos.y <= stoneHeight && pos.y >= g_minChunkHeight)
            blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::STONE, visibleFaces)));
        else
            blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::DIRT, visibleFaces)));
    }
}

bool ChunkPiece::blockReserved(short y)
{
    for (size_t i = 0; i < reservedBlockPositions.size(); ++i)
    {
        if (reservedBlockPositions[i] == (short)y)
        {
            reservedBlockPositions.erase(reservedBlockPositions.begin() + i);
            return true;
        }
    }
    return false;
}
