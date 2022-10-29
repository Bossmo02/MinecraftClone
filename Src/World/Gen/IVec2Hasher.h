#ifndef IVEC2_HASHER
#define IVEC2_HASHER

#include <glm/ext/vector_int2.hpp>


struct IVec2Hasher
{
    size_t operator()(const glm::ivec2& c) const
    {
        return (std::hash<int>()(c.x) ^ std::hash<int>()(c.y));
    }
};


#endif // !IVEC2_HASHER
