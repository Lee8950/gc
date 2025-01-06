#include <gc.hpp>

inline ecl::gc::Allocator::Allocator(int Xms, int Xmx) : max_mem(Xmx), min_mem(Xms), cur_mem(Xms), allocated(nullptr)
{
    mems = new unsigned char[cur_mem];
}

void *ecl::gc::Allocator::gc_new(uint64_t size)
{
    // Step 0: If size > max_mem, then fail.
    if(size > max_mem)
        throw std::runtime_error("Allocator out of memory. Consider raising Xmx of Allocator.");
    // Step 1: Find available space.
    //  Case 1: if none was allocated, then is simple.
    if(allocated == nullptr)
    {
        while(size<cur_mem) {
            if(size * 2 > max_mem) size = max_mem;
            else size *= 2;
        }
        AllocatedMemoryBlock *amb = createAMB(0, size);
        return amb->start_address + mems;
    }
    //  Case 2: There are already allocated blocks.
    else
    {

    }

    // Step 2: If any space was found, return a AMB managing this block.
    //          If none was found, try collect.

    // Step 3: Collect.

    // Step 4: If collected, try step 1 again.
    //          If none was found, expand mem space
}

ecl::gc::AllocatedMemoryBlock *ecl::gc::Allocator::createAMB(uint64_t pos, uint64_t size)
{
    auto amb = new AllocatedMemoryBlock;
    amb->start_address = pos;
    amb->block_size = size;

}