#ifndef WATER_BLOCK_HPP
#define WATER_BLOCK_HPP

#include "Block.h"

class WaterBlock : public Block
{
public:

	WaterBlock(int x, int y, int z);

private:
	const float m_heightOffset = 0.2f;

};


#endif // !WATER_BLOCK_HPP