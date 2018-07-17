#include "FreeList.hpp"
#include "Errors.hpp"

FreeList::FreeList(uint32_t numElements) :
    m_numElements(numElements),
    m_firstNonAllocated(0)
{}

int FreeList::allocate()
{
    if (!m_freeList.empty())
    {
        int index = static_cast<int>(m_freeList.back());
        m_freeList.pop_back();
        return index;
    }

    if (m_firstNonAllocated >= m_numElements) return -1;

    int index = static_cast<int>(m_firstNonAllocated);
    m_firstNonAllocated++;
    return index;
}

void FreeList::release(int offset)
{
    SP_ASSERT((offset >= 0) && (offset < m_firstNonAllocated),
              "Trying to release offset that is not in the range of allocated offsets");

    m_freeList.emplace_back(offset);
}