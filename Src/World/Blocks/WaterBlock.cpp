#include "WaterBlock.h"


WaterBlock::WaterBlock(int x, int y, int z)
	: Block(x, y - m_heightOffset, z, BLOCK_ID::WATER, FACES_TO_DISPLAY::TOP)
{
}