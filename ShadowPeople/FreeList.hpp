#pragma once

#include <vector>
#include <stdint.h>

class FreeList
{
public:
    FreeList(uint32_t numElements);

    int allocate();
    void release(int offset);
private:
    std::vector<uint32_t>   m_freeList;
    uint32_t                m_firstNonAllocated;
    uint32_t                m_numElements;
};
