#include "ChunkPiece.h"



void ChunkPiece::setup()
{
    // necessary so that blocks can't be placed on top of their original height
    usedBlockPositions.push_back((short)height + 1);
}

int ChunkPiece::searchForBlock(int y) const
{
    // returns the index of the block at given height
    for (int i = 0; i < blocks.size(); ++i)
    {
        if (blocks[i].get()->getLocalPos().y == y)
            return i;
    }

    return -1;
}

void ChunkPiece::addBlock(glm::vec3 pos, unsigned char visibleFaces)
{
    if (!blockPosUsed((short)pos.y))
    {
        if (pos.y == height && height > g_waterLevel)
        {
            blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::GRASS, visibleFaces)));
            usedBlockPositions.push_back((short)pos.y);
        }
        else if (pos.y == g_waterLevel)
        {
            blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::SAND, visibleFaces)));
            usedBlockPositions.push_back((short)pos.y);
        }
        else if (pos.y <= stoneHeight && pos.y >= g_minChunkHeight)
        {
            blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::STONE, visibleFaces)));
            usedBlockPositions.push_back((short)pos.y);
        }
        else
        {
            blocks.push_back(std::make_unique<Block>(Block((int)pos.x, (int)pos.y, (int)pos.z, BLOCK_ID::DIRT, visibleFaces)));
            usedBlockPositions.push_back((short)pos.y);
        }
    }
}

bool ChunkPiece::blockPosUsed(short y)
{
    for (size_t i = 0; i < usedBlockPositions.size(); ++i)
    {
        if (usedBlockPositions[i] == y)
        {
            return true;
        }
    }
    return false;
}

void ChunkPiece::deleteBlock(size_t index)
{
    blocks[index].reset();
    blocks.erase(blocks.begin() + index);
}
